#include <stdio.h>

#include "interpret.h"
#include "stack.h"
#include "instruction_list.h"

#include <stdlib.h>

stack_t *glob_stack;
inter_table_t *glob_stable;
instruction_list_t *glob_ins_list;
argument_var_t tmp_var;
argument_var_t *tmp_ptr;

int interpret(instruction_list_t *instruction_list, inter_table_t *stable) {
    glob_stack = stack_init();
    glob_ins_list = instruction_list;
    glob_stable = stable;

    glob_ins_list->active = glob_ins_list->first;

    while (instruction_list->active != NULL) {

        switch (instruction_list->active->instruction.type) {
            case INST_NOP :
                break;
            case INST_LABEL:
                break;
            case INST_ADD:
                add();
                break;
            case INST_SUB:
                sub();
                break;
            case INST_MUL:
                mul();
                break;
            case INST_DIV:
                divisoin();
                break;
            case INST_PUSH:
                push();
                break;
            case INST_POP:
                pop();
                break;
            case INST_STORE:
                store();
                break;
            case INST_HALT:
                instruction_list->active = glob_ins_list->last;
                break;
            case INST_CALL:
                call();
                break;
            case INST_RET:
                ret();
                break;
            case INST_JMP:
                inst_jump();
                break;
            case INST_WRITE:
                //nastavujeme ukazateln uz tu, pretoze sa funkcia vola rekurzivne a robilo by to bordel
                tmp_ptr = glob_ins_list->active->instruction.addr1;
                write();
                printf("\n");
                break;
            case INST_READ_INT:
                read_int();
                break;
            case INST_READ_DOUBLE:
                read_double();
                break;
            case INST_JZ:
                jump_zero();
                break;
            case INST_JEQ:
                jump_equal();
                break;
            case INST_JNEQ:
                jump_not_equal();
                break;
            default:
                printf("Interpret: Unnknown intruction\n");
                break;
        }
        //print_stack(glob_stack);
        glob_ins_list->active = glob_ins_list->active->next;
    }

    stack_destroy(&glob_stack);
    return 0;
}

void call() {
    tmp_var.data.instruction = glob_ins_list->active;
    tmp_var.arg_type = INSTRUCTION;
    stack_push(&glob_stack, tmp_var);             //pushing current instruction
    tmp_var.arg_type = INTEGER;
    tmp_var.data.i = glob_stack->base;     //pushig base, similar to push ebp in assemlby
    stack_push(&glob_stack, tmp_var);
    //glob_stack->used += 2;                      //added counter for stack
    glob_stack->base = glob_stack->used;
    tmp_ptr = glob_ins_list->active->instruction.addr1;
    glob_ins_list->active = tmp_ptr->data.instruction;
}

void ret() {
    int prev_base;
    argument_var_t *return_value, *destination;

    //pristup k hodnotam pred volanim funckie
    tmp_var = stack_ebp_relative(glob_stack, 0);
    prev_base = tmp_var.data.i;

    //ziskanie hodnoty ktora sa predava z funkcie
    return_value = glob_ins_list->active->instruction.addr1;

    //pozrieme ci mame navratit nejaku hodnotu, ak nie len skocime von z volania funkcie
    if (return_value != NULL) {
        if (return_value->arg_type == STACK_EBP) {
            return_value = stack_ebp_relative_ptr(glob_stack, return_value->data.i);
        }

        //prepisanie aktualnych hodnot riadeni interpretu
        tmp_var = stack_ebp_relative(glob_stack, -1);
        glob_ins_list->active = tmp_var.data.instruction;

        //upratanie zasobnik apo volani funckie
        glob_stack->used = glob_stack->base;
        glob_stack->base = prev_base;

        //ziskanie polohykam zapisat navratovu hodnotu a jej zapis
        destination = glob_ins_list->active->instruction.addr2;
        //ak nechecme nikam ulozit navratovau hodnotu funkcie tak zapis preskocime
        if (destination != NULL) {
            if (destination->arg_type == STACK_EBP) {
                stack_actualize_from_ebp(glob_stack, *return_value, destination->data.i);
            } else {
                *destination = *return_value;   //TODO check
            }
        }
    } else {
        //vratenie sa spat odkial sa volala funkcia
        tmp_var = stack_ebp_relative(glob_stack, -1);
        glob_ins_list->active = tmp_var.data.instruction;

        //upratanie zasobnik apo volani funckie
        glob_stack->used = glob_stack->base;
        glob_stack->base = prev_base;

    }
}

void write() {
    switch (tmp_ptr->arg_type) {
        case INTEGER:
            printf("%d", tmp_ptr->data.i);
            break;
        case DOUBLE:
            printf("%lf", tmp_ptr->data.d);
            break;
        case STRING:
            printf("%s", tmp_ptr->data.s);
            break;
        case ON_STACK:
            tmp_var = stack_top(glob_stack);
            tmp_ptr = &tmp_var;
            write();
            break;
        case STACK_EBP:
            tmp_var = stack_ebp_relative(glob_stack, tmp_ptr->data.i);
            tmp_ptr = &tmp_var;
            write();
        default:
            //TODO error maybe
            break;
    }

}

void read_int() {
    int readed_int;
    //nacita premennu do docasnej premennej
    scanf("%d", &readed_int);

    tmp_ptr = glob_ins_list->active->instruction.addr1;

    if (tmp_ptr->arg_type == STACK_EBP) {
        tmp_var = stack_ebp_relative(glob_stack, tmp_ptr->data.i);
        tmp_var.data.i = readed_int;
        stack_actualize_from_ebp(glob_stack, tmp_var, tmp_ptr->data.i);
    } else {
        tmp_ptr->data.i = readed_int;
    }
}

void read_double() {
    double readed_double;
    //nacita premennu do docasnej premennej
    scanf("%lf", &readed_double);

    tmp_ptr = glob_ins_list->active->instruction.addr1;

    if (tmp_ptr->arg_type == STACK_EBP) {
        tmp_var = stack_ebp_relative(glob_stack, tmp_ptr->data.i);
        tmp_var.data.d = readed_double;
        stack_actualize_from_ebp(glob_stack, tmp_var, tmp_ptr->data.i);
    } else {
        tmp_ptr->data.d = readed_double;
    }
}

void push() {
    tmp_ptr = glob_ins_list->active->instruction.addr1;

    if (tmp_ptr->arg_type == STACK_EBP) {
        tmp_var = stack_ebp_relative(glob_stack, tmp_ptr->data.i);
        stack_push(&glob_stack, tmp_var);
    } else {
        tmp_var = *tmp_ptr;
        stack_push(&glob_stack, tmp_var);
    }
}

void store() {
    argument_var_t *arg1, *arg2;

    arg1 = glob_ins_list->active->instruction.addr1;
    arg2 = glob_ins_list->active->instruction.addr2;

    if (arg1->arg_type == STACK_EBP) {
        arg1 = stack_ebp_relative_ptr(glob_stack, arg1->data.i);
    }
    if (arg2->arg_type == STACK_EBP) {
        arg2 = stack_ebp_relative_ptr(glob_stack, arg1->data.i);
    }

    //ulozenie hodnoty do ciela
    *arg1 = *arg2;
}

void add() {
    argument_var_t *arg1, *arg2, *arg3;

    //nacita hodnoty z tabulky symbolov
    arg1 = glob_ins_list->active->instruction.addr1;
    arg3 = glob_ins_list->active->instruction.addr3;
    arg2 = glob_ins_list->active->instruction.addr2;

    double a, b;

    //nacita hodnoty zo stacku ak su tam, ak nie su to globalne premenne a berie ich priamo z tabulky symbolov
    if (arg1->arg_type == STACK_EBP) {
        arg1 = stack_ebp_relative_ptr(glob_stack, arg1->data.i);
    }

    if (arg2->arg_type == STACK_EBP) {
        arg2 = stack_ebp_relative_ptr(glob_stack, arg2->data.i);
    }

    if (arg3->arg_type == STACK_EBP) {
        arg3 = stack_ebp_relative_ptr(glob_stack, arg3->data.i);
    }

    //nacitanie do lokalnych premennych
    if (arg2->arg_type == DOUBLE) {
        a = arg2->data.d;
    } else {
        a = arg2->data.i;
    }

    if (arg3->arg_type == DOUBLE) {
        b = arg3->data.d;
    } else {
        b = arg3->data.i;
    }

    //zapisanie vysledku
    if (arg1->arg_type == DOUBLE) {
        arg1->data.d = a + b;
    } else {
        arg1->data.i = (int) (a + b);
    }
}

void sub() {
    argument_var_t *arg1, *arg2, *arg3;

    //nacita hodnoty z tabulky symbolov
    arg1 = glob_ins_list->active->instruction.addr1;
    arg3 = glob_ins_list->active->instruction.addr3;
    arg2 = glob_ins_list->active->instruction.addr2;

    double a, b;

    //nacita hodnoty zo stacku ak su tam, ak nie su to globalne premenne a berie ich priamo z tabulky symbolov
    if (arg1->arg_type == STACK_EBP) {
        arg1 = stack_ebp_relative_ptr(glob_stack, arg1->data.i);
    }

    if (arg2->arg_type == STACK_EBP) {
        arg2 = stack_ebp_relative_ptr(glob_stack, arg2->data.i);
    }

    if (arg3->arg_type == STACK_EBP) {
        arg3 = stack_ebp_relative_ptr(glob_stack, arg3->data.i);
    }

    //nacitanie do lokalnych premennych
    if (arg2->arg_type == DOUBLE) {
        a = arg2->data.d;
    } else {
        a = arg2->data.i;
    }

    if (arg3->arg_type == DOUBLE) {
        b = arg3->data.d;
    } else {
        b = arg3->data.i;
    }

    //zapisanie vysledku
    if (arg1->arg_type == DOUBLE) {
        arg1->data.d = a - b;
    } else {
        arg1->data.i = (int) (a - b);
    }
}

void mul() {
    argument_var_t *arg1, *arg2, *arg3;

    //nacita hodnoty z tabulky symbolov
    arg1 = glob_ins_list->active->instruction.addr1;
    arg3 = glob_ins_list->active->instruction.addr3;
    arg2 = glob_ins_list->active->instruction.addr2;

    double a, b;

    //nacita hodnoty zo stacku ak su tam, ak nie su to globalne premenne a berie ich priamo z tabulky symbolov
    if (arg1->arg_type == STACK_EBP) {
        arg1 = stack_ebp_relative_ptr(glob_stack, arg1->data.i);
    }

    if (arg2->arg_type == STACK_EBP) {
        arg2 = stack_ebp_relative_ptr(glob_stack, arg2->data.i);
    }

    if (arg3->arg_type == STACK_EBP) {
        arg3 = stack_ebp_relative_ptr(glob_stack, arg3->data.i);
    }

    //nacitanie do lokalnych premennych
    if (arg2->arg_type == DOUBLE) {
        a = arg2->data.d;
    } else {
        a = arg2->data.i;
    }

    if (arg3->arg_type == DOUBLE) {
        b = arg3->data.d;
    } else {
        b = arg3->data.i;
    }

    //zapisanie vysledku
    if (arg1->arg_type == DOUBLE) {
        arg1->data.d = a * b;
    } else {
        arg1->data.i = (int) (a * b);
    }
}

void divisoin() {
    argument_var_t *arg1, *arg2, *arg3;

    //nacita hodnoty z tabulky symbolov
    arg1 = glob_ins_list->active->instruction.addr1;
    arg3 = glob_ins_list->active->instruction.addr3;
    arg2 = glob_ins_list->active->instruction.addr2;

    double a, b;

    //nacita hodnoty zo stacku ak su tam, ak nie su to globalne premenne a berie ich priamo z tabulky symbolov
    if (arg1->arg_type == STACK_EBP) {
        arg1 = stack_ebp_relative_ptr(glob_stack, arg1->data.i);
    }

    if (arg2->arg_type == STACK_EBP) {
        arg2 = stack_ebp_relative_ptr(glob_stack, arg2->data.i);
    }

    if (arg3->arg_type == STACK_EBP) {
        arg3 = stack_ebp_relative_ptr(glob_stack, arg3->data.i);
    }

    //nacitanie do lokalnych premennych
    if (arg2->arg_type == DOUBLE) {
        a = arg2->data.d;
    } else {
        a = arg2->data.i;
    }

    if (arg3->arg_type == DOUBLE) {
        b = arg3->data.d;
    } else {
        b = arg3->data.i;
    }

    //zapisanie vysledku
    if (arg1->arg_type == DOUBLE) {
        arg1->data.d = a / b;
    } else {
        arg1->data.i = (int) (a / b);
    }
}

void expr_add() {
    argument_var_t dest, op1, op2;

    op1 = stack_from_top(glob_stack, 0);
    op2 = stack_from_top(glob_stack, 1);

    double a, b;

    if (op1.arg_type == INTEGER && op2.arg_type == INTEGER) {
        dest.arg_type == INTEGER;
        dest.data.d = op1.data.i + op2.data.i;
    } else {
        if (op1.arg_type == DOUBLE && op2.arg_type == DOUBLE) {
            dest.arg_type == DOUBLE;
            dest.data.d = op1.data.d + op2.data.d;
        } else {
            if (op1.arg_type == DOUBLE && !op2.arg_type == DOUBLE) {
                dest.arg_type == DOUBLE;
                dest.data.d = op1.data.d + op2.data.i;
            } else {
                dest.arg_type == DOUBLE;
                dest.data.d = op1.data.i + op2.data.d;
            }
        }
    }

    pop();
    pop();
    stack_push(&glob_stack, dest);
}

void pop() {
    stack_pop(glob_stack);
}

void inst_jump() {
    tmp_ptr = glob_ins_list->active->instruction.addr1;     //nacita z tabulky symbolov ukazatel na instrukciu
    glob_ins_list->active = tmp_ptr->data.instruction;                                  //priradi ukazatel do listu, takze zmeni tok programu
}

void jump_zero() {
    tmp_ptr = glob_ins_list->active->instruction.addr2;     //nacita operand pre porovnanie
    if (tmp_ptr->arg_type == STACK_EBP) {
        tmp_var = stack_ebp_relative(glob_stack,
                                     tmp_ptr->data.i);                       //ak je na zasobniku zoberie hodnotu operandu z tade
    } else {
        tmp_var = *tmp_ptr;
    }

    if (tmp_var.data.i ==
        0) {                                                            //ak je operand nulovy takze false, urobi sa skok
        tmp_ptr = glob_ins_list->active->instruction.addr1;  //nacita z tabulky symbolov ukazatel na instrukciu
        glob_ins_list->active = tmp_ptr->data.instruction;                               //priradi ukazatel do listu, takze zmeni tok programu
    }
}

void jump_equal() {      //TODO test it
    argument_var_t *op1, *op2;

    //nacitanie oprandov z tabulky symbolov
    op1 = glob_ins_list->active->instruction.addr2;
    op2 = glob_ins_list->active->instruction.addr3;

    //nacitanie operandov ak su na zasobniku
    if (op1->arg_type == STACK_EBP) {
        op1 = stack_ebp_relative_ptr(glob_stack, op1->data.i);
    }
    if (op2->arg_type == STACK_EBP) {
        op2 = stack_ebp_relative_ptr(glob_stack, op1->data.i);
    }

    //porovnavanie  a ak su rovnke skoci na danu adresu
    if (op1->arg_type == INTEGER) {
        if (op2->arg_type == INTEGER) {
            if (op1->data.i == op2->data.i) {
                tmp_ptr = glob_ins_list->active->instruction.addr1;  //nacita z tabulky symbolov ukazatel na instrukciu
                glob_ins_list->active = tmp_ptr->data.instruction;
                return;
            }
        } else {
            if (op1->data.i == op2->data.d) {
                tmp_ptr = glob_ins_list->active->instruction.addr1;  //nacita z tabulky symbolov ukazatel na instrukciu
                glob_ins_list->active = tmp_ptr->data.instruction;
                return;
            }
        }
    } else {
        if (op2->arg_type == INTEGER) {
            if (op1->data.d == op2->data.i) {
                tmp_ptr = glob_ins_list->active->instruction.addr1;  //nacita z tabulky symbolov ukazatel na instrukciu
                glob_ins_list->active = tmp_ptr->data.instruction;
                return;
            }
        } else {
            if (op1->data.d == op2->data.d) {
                tmp_ptr = glob_ins_list->active->instruction.addr1;  //nacita z tabulky symbolov ukazatel na instrukciu
                glob_ins_list->active = tmp_ptr->data.instruction;
                return;
            }
        }
    }
}

void jump_not_equal() {      //TODO test it
    argument_var_t *op1, *op2;

    //nacitanie oprandov z tabulky symbolov
    op1 =
            glob_ins_list->active->instruction.addr2;
    op2 =
            glob_ins_list->active->instruction.addr3;

    //nacitanie operandov ak su na zasobniku
    if (op1->arg_type == STACK_EBP) {
        op1 = stack_ebp_relative_ptr(glob_stack, op1->data.i);
    }
    if (op2->arg_type == STACK_EBP) {
        op2 = stack_ebp_relative_ptr(glob_stack, op1->data.i);
    }

    //porovnavanie  a ak su rovnke skoci na danu adresu
    if (op1->arg_type == INTEGER) {
        if (op2->arg_type == INTEGER) {
            if (op1->data.i != op2->data.i) {
                tmp_ptr =
                        glob_ins_list->active->instruction.addr1;  //nacita z tabulky symbolov ukazatel na instrukciu
                glob_ins_list->active = tmp_ptr->data.instruction;
                return;
            }
        } else {
            if (op1->data.i != op2->data.d) {
                tmp_ptr =
                        glob_ins_list->active->instruction.addr1;  //nacita z tabulky symbolov ukazatel na instrukciu
                glob_ins_list->active = tmp_ptr->data.instruction;
                return;
            }
        }
    } else {
        if (op2->arg_type == INTEGER) {
            if (op1->data.d != op2->data.i) {
                tmp_ptr = glob_ins_list->active->instruction.addr1;  //nacita z tabulky symbolov ukazatel na instrukciu
                glob_ins_list->active = tmp_ptr->data.instruction;
                return;
            }
        } else {
            if (op1->data.d != op2->data.d) {
                tmp_ptr = glob_ins_list->active->instruction.addr1;  //nacita z tabulky symbolov ukazatel na instrukciu
                glob_ins_list->active = tmp_ptr->data.instruction;
            }
        }
    }
}
