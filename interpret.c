#include "interpret.h"
#include "instruction_list.h"
#include "error.h"

stack_t *glob_stack;
stab_t *glob_stable;
instruction_list_t *glob_ins_list;
argument_var_t tmp_var;
argument_var_t *tmp_ptr;

int interpret(instruction_list_t *instruction_list, stab_t *stable) {
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
            case INST_EXPR_ADD:
                expr_add();
                break;
            case INST_EXPR_SUB:
                expr_sub();
                break;
            case INST_EXPR_MUL:
                expr_mul();
                break;
            case INST_EXPR_DIV:
                expr_div();
                break;
            case INST_EXPR_LOWER:
            case INST_EXPR_HIGHER:
            case INST_EXPR_LOW_EQ:
            case INST_EXPR_HIG_EQ:
            case INST_EXPR_EQUAL:
            case INST_EXPR_NOT_EQ:
                compare();
                break;
            case INST_EXPR_STR_ADD:
                expr_str_add();
                break;
            case INST_PUSH:
            case INST_PUSH_INT:
            case INST_PUSH_DOUBLE:
            case INST_PUSH_STRING:
                push();
                break;
            case INST_POP:
                pop();
                break;
            case INST_STORE:
                store();
                break;
            case INST_HALT:
                halt();
                instruction_list->active = glob_ins_list->last;
                break;
            case INST_CALL:
                call();
                break;
            case INST_CALL_PRINT:
                call_print();
                break;
            case INST_CALL_CMP:
                call_str_cmp();
                break;
            case INST_CALL_FIND:
                call_str_find();
                break;
            case INST_CALL_SORT:
                call_str_sort();
                break;
            case INST_CALL_SUBSTR:
                call_str_substr();
                break;
            case INST_CALL_LEN:
                call_str_len();
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
            case INST_READ_STRING:
                read_string();
                break;
            case INST_JZ:
                jump_zero();
                break;
            case INST_JNZ:
                jump_not_zero();
                break;
            case INST_JEQ:
                jump_equal();
                break;
            case INST_JNEQ:
                jump_not_equal();
                break;
            case INST_STR_INIT:
                interpret_str_init();
                break;
            case INST_STR_REINIT:
                interpret_str_reinit();
                break;
            case INST_STR_LEN:
                str_len();
                break;
            case INST_STR_CONCATENATE:
                concatenate();
                break;
            case INST_STR_CMP:
                interpret_str_cmp();
                break;
            default:
                printf("Interpret: Unnknown intruction\n");
                break;
        }

        //print_stack(glob_stack);
        //stable_print(stable);
        glob_ins_list->active = glob_ins_list->active->next;
    }

    stack_destroy(&glob_stack);
    return 0;
}

void halt() {
    if (glob_stack != NULL) {
        while (glob_stack->used) {
            pop();
        }
    }
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

void ret(){
    int prev_base;
    int current_base, current_used;
    argument_var_t *return_value, *destination;

    //pristup k hodnotam pred volanim funckie
    tmp_var = stack_ebp_relative(glob_stack, 0);
    prev_base = tmp_var.data.i;

    //zapisanie aktualnych poloh na zasobniku kvoli cisteniu stringov
    current_base = glob_stack->base;
    current_used = glob_stack->used;

    //ziskanie hodnoty ktora sa predava z funkcie
    return_value = glob_ins_list->active->instruction.addr1;

    //pozrieme ci mame navratit nejaku hodnotu, ak nie len skocime von z volania funkcie
    if (return_value != NULL) {
        if (return_value->arg_type == STACK_EBP) {
            return_value = stack_ebp_relative_ptr(glob_stack, return_value->data.i);
        }
        if (return_value->arg_type == ON_TOP) {
            return_value = stack_top_ptr(glob_stack);
        }

        //prepisanie aktualnych hodnot riadeni interpretu
        tmp_var = stack_ebp_relative(glob_stack, -1);
        glob_ins_list->active = tmp_var.data.instruction;

        for (int i = glob_stack->used;i>prev_base;i--)

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
                *destination = *return_value;
            }
        }
    } else {
        //vratenie sa spat odkial sa volala funkcia
        tmp_var = stack_ebp_relative(glob_stack, -1);
        glob_ins_list->active = tmp_var.data.instruction;

        //upratanie zasobnik apo volani funckie
        glob_stack->used = glob_stack->base;
        glob_stack->base = prev_base;

        if (glob_ins_list->active->instruction.addr3 != (void *) 6) {
            error_and_die(RUNTIME_UNINITIALIZED, "Error :no return");
        }
    }

    //vycistenie stringov zo zasobnika
    //TODO test it
    for (int i = current_base; i < current_used ; i++){
        if (glob_stack->data[i].arg_type == STRING){
            str_destroy(&glob_stack->data[i].data.s);
        }
    }
}

void write() {
    switch (tmp_ptr->arg_type) {
        case INTEGER:
            printf("%d",tmp_ptr->data.i);
            break;
        case DOUBLE:
            printf("%lf",tmp_ptr->data.d);
            break;
        case STRING:
            printf("%s", tmp_ptr->data.s.str);
            break;
        case ON_TOP:
            tmp_var = stack_top(glob_stack);
            tmp_ptr = &tmp_var;
            write();
            break;
        case STACK_EBP:
            tmp_var = stack_ebp_relative(glob_stack,tmp_ptr->data.i);
            tmp_ptr = &tmp_var;
            write();
            break;
        default:
            error_and_die(RUNTIME_ERROR, "INST_WRITE error");
            break;
    }

}

void read_int(){
    int readed_int;
    //nacita premennu do docasnej premennej
    if (scanf("%d", &readed_int) == EOF) {
        error_and_die(RUNTIME_READ_ERROR, "read integer");
    }

    tmp_ptr = glob_ins_list->active->instruction.addr1;

    if (tmp_ptr->arg_type == STACK_EBP){
        tmp_var = stack_ebp_relative(glob_stack,tmp_ptr->data.i);
        tmp_var.data.i = readed_int;
        stack_actualize_from_ebp(glob_stack,tmp_var,tmp_ptr->data.i);
    } else {
        tmp_ptr->data.i = readed_int;
    }
}

void read_double(){
    double readed_double;
    //nacita premennu do docasnej premennej
    if (scanf("%lf", &readed_double) == EOF) {
        error_and_die(RUNTIME_READ_ERROR, "read double");
    }

    tmp_ptr = glob_ins_list->active->instruction.addr1;

    if (tmp_ptr->arg_type == STACK_EBP){
        tmp_var = stack_ebp_relative(glob_stack,tmp_ptr->data.i);
        tmp_var.data.d = readed_double;
        stack_actualize_from_ebp(glob_stack,tmp_var,tmp_ptr->data.i);
    } else {
        tmp_ptr->data.d = readed_double;
    }
}

void read_string() {
    argument_var_t *arg_ptr = glob_ins_list->active->instruction.addr1;
    if (arg_ptr->arg_type == STACK_EBP) {
        arg_ptr = stack_ebp_relative_ptr(glob_stack, arg_ptr->data.i);
    }
    str_read_str_stdin(&arg_ptr->data.s);
}

void push() {
    tmp_ptr = glob_ins_list->active->instruction.addr1;

    switch (glob_ins_list->active->instruction.type) {
        case INST_PUSH:
            if (tmp_ptr != NULL) {
                if (tmp_ptr->arg_type == STACK_EBP) {
                    tmp_var = stack_ebp_relative(glob_stack, tmp_ptr->data.i);
                } else {
                    tmp_var = *tmp_ptr;
                }
                if (tmp_var.arg_type == STRING) {
                    argument_var_t local_var;
                    local_var.arg_type = STRING;
                    local_var.data.s = str_init();
                    str_append_str(&local_var.data.s, &tmp_var.data.s);
                    tmp_var = local_var;
                }
            }
            break;
        case INST_PUSH_INT:
            tmp_var.arg_type = INTEGER;
            break;
        case INST_PUSH_DOUBLE:
            tmp_var.arg_type = DOUBLE;
            break;
        case INST_PUSH_STRING:
            tmp_var.arg_type = STRING;
            tmp_var.data.s = str_init();
            break;
	default:
		error_and_die(INTERNAL_ERROR, "push() unknown");

    }
    stack_push(&glob_stack, tmp_var);
}

void store() {
    argument_var_t *arg1,*arg2;

    arg1 = glob_ins_list->active->instruction.addr1;
    arg2 = glob_ins_list->active->instruction.addr2;

    if (arg1->arg_type == STACK_EBP) {
        arg1 = stack_ebp_relative_ptr(glob_stack,arg1->data.i);
    } else {
        if (arg1->arg_type == ON_TOP) {
            arg1 = stack_top_ptr(glob_stack);
        }
    }

    if (arg2->arg_type == STACK_EBP) {
        arg2 = stack_ebp_relative_ptr(glob_stack,arg1->data.i);
    } else {
        if (arg2->arg_type == ON_TOP) {
            arg2 = stack_top_ptr(glob_stack);
        }
    }

    //ulozenie hodnoty do ciela
    if (arg1->arg_type == STRING) {
        str_reinit(&arg1->data.s);
        str_append_str(&arg1->data.s, &arg2->data.s);
    } else {
        *arg1 = *arg2;
    }
}

void add(){
    argument_var_t  *arg1,*arg2,*arg3;

    //nacita hodnoty z tabulky symbolov
    arg1 = glob_ins_list->active->instruction.addr1;
    arg2 = glob_ins_list->active->instruction.addr2;
    arg3 = glob_ins_list->active->instruction.addr3;

    double a,b;

    //nacita hodnoty zo stacku ak su tam, ak nie su to globalne premenne a berie ich priamo z tabulky symbolov
    if (arg1->arg_type == STACK_EBP){
        arg1 = stack_ebp_relative_ptr(glob_stack,arg1->data.i);
    }

    if (arg2->arg_type == STACK_EBP){
        arg2 = stack_ebp_relative_ptr(glob_stack,arg2->data.i);
    }

    if (arg3->arg_type == STACK_EBP){
        arg3 = stack_ebp_relative_ptr(glob_stack,arg3->data.i);
    }

    //nacitanie do lokalnych premennych
    if (arg2->arg_type == DOUBLE){
        a = arg2->data.d;
    } else {
        a = arg2->data.i;
    }

    if (arg3->arg_type == DOUBLE){
        b = arg3->data.d;
    } else {
        b = arg3->data.i;
    }

    //zapisanie vysledku
    if (arg1->arg_type == DOUBLE){
        arg1->data.d = a + b;
    } else {
        arg1->data.i = (int) (a + b);
    }
}

void sub(){
    argument_var_t  *arg1,*arg2,*arg3;

    //nacita hodnoty z tabulky symbolov
    arg1 = glob_ins_list->active->instruction.addr1;
    arg3 = glob_ins_list->active->instruction.addr3;
    arg2 = glob_ins_list->active->instruction.addr2;

    double a,b;

    //nacita hodnoty zo stacku ak su tam, ak nie su to globalne premenne a berie ich priamo z tabulky symbolov
    if (arg1->arg_type == STACK_EBP){
        arg1 = stack_ebp_relative_ptr(glob_stack,arg1->data.i);
    }

    if (arg2->arg_type == STACK_EBP){
        arg2 = stack_ebp_relative_ptr(glob_stack,arg2->data.i);
    }

    if (arg3->arg_type == STACK_EBP){
        arg3 = stack_ebp_relative_ptr(glob_stack,arg3->data.i);
    }

    //nacitanie do lokalnych premennych
    if (arg2->arg_type == DOUBLE){
        a = arg2->data.d;
    } else {
        a = arg2->data.i;
    }

    if (arg3->arg_type == DOUBLE){
        b = arg3->data.d;
    } else {
        b = arg3->data.i;
    }

    //zapisanie vysledku
    if (arg1->arg_type == DOUBLE){
        arg1->data.d = a - b;
    } else {
        arg1->data.i = (int) (a - b);
    }
}

void mul(){
    argument_var_t  *arg1,*arg2,*arg3;

    //nacita hodnoty z tabulky symbolov
    arg1 = glob_ins_list->active->instruction.addr1;
    arg3 = glob_ins_list->active->instruction.addr3;
    arg2 = glob_ins_list->active->instruction.addr2;

    double a,b;

    //nacita hodnoty zo stacku ak su tam, ak nie su to globalne premenne a berie ich priamo z tabulky symbolov
    if (arg1->arg_type == STACK_EBP){
        arg1 = stack_ebp_relative_ptr(glob_stack,arg1->data.i);
    }

    if (arg2->arg_type == STACK_EBP){
        arg2 = stack_ebp_relative_ptr(glob_stack,arg2->data.i);
    }

    if (arg3->arg_type == STACK_EBP){
        arg3 = stack_ebp_relative_ptr(glob_stack,arg3->data.i);
    }

    //nacitanie do lokalnych premennych
    if (arg2->arg_type == DOUBLE){
        a = arg2->data.d;
    } else {
        a = arg2->data.i;
    }

    if (arg3->arg_type == DOUBLE){
        b = arg3->data.d;
    } else {
        b = arg3->data.i;
    }

    //zapisanie vysledku
    if (arg1->arg_type == DOUBLE){
        arg1->data.d = a*b;
    } else {
        arg1->data.i = (int) (a * b);
    }
}

void divisoin(){
    argument_var_t  *arg1,*arg2,*arg3;

    //nacita hodnoty z tabulky symbolov
    arg1 = glob_ins_list->active->instruction.addr1;
    arg3 = glob_ins_list->active->instruction.addr3;
    arg2 = glob_ins_list->active->instruction.addr2;

    double a,b;

    //nacita hodnoty zo stacku ak su tam, ak nie su to globalne premenne a berie ich priamo z tabulky symbolov
    if (arg1->arg_type == STACK_EBP){
        arg1 = stack_ebp_relative_ptr(glob_stack,arg1->data.i);
    }

    if (arg2->arg_type == STACK_EBP){
        arg2 = stack_ebp_relative_ptr(glob_stack,arg2->data.i);
    }

    if (arg3->arg_type == STACK_EBP){
        arg3 = stack_ebp_relative_ptr(glob_stack,arg3->data.i);
    }

    //nacitanie do lokalnych premennych
    if (arg2->arg_type == DOUBLE){
        a = arg2->data.d;
    } else {
        a = arg2->data.i;
    }

    if (arg3->arg_type == DOUBLE){
        b = arg3->data.d;
    } else {
        b = arg3->data.i;
    }
    if (!b) exit(9);
    //zapisanie vysledku
    if (arg1->arg_type == DOUBLE){
        arg1->data.d = a / b;
    } else {
        arg1->data.i = (int) (a / b);
    }
}

void expr_add(){
    argument_var_t dest, op1, op2;

    op2 = stack_pop(glob_stack);
    op1 = stack_pop(glob_stack);

    if (op1.arg_type == INTEGER && op2.arg_type == INTEGER){
        dest.arg_type = INTEGER;
        dest.data.i = op1.data.i + op2.data.i;
    } else {
        if (op1.arg_type == DOUBLE && op2.arg_type == DOUBLE)
        {
            dest.arg_type = DOUBLE;
            dest.data.d = op1.data.d + op2.data.d;
        } else {
            if (op1.arg_type == DOUBLE && op2.arg_type == INTEGER) {
                dest.arg_type = DOUBLE;
                dest.data.d = op1.data.d + op2.data.i;
            } else {
                dest.arg_type = DOUBLE;
                dest.data.d = op1.data.i + op2.data.d;
            }
        }
    }

    stack_push(&glob_stack,dest);
}

void expr_sub() {
    argument_var_t dest, op1, op2;

    op2 = stack_pop(glob_stack);
    op1 = stack_pop(glob_stack);

    if (op1.arg_type == INTEGER && op2.arg_type == INTEGER) {
        dest.arg_type = INTEGER;
        dest.data.i = op1.data.i - op2.data.i;
    } else {
        if (op1.arg_type == DOUBLE && op2.arg_type == DOUBLE) {
            dest.arg_type = DOUBLE;
            dest.data.d = op1.data.d - op2.data.d;
        } else {
            if (op1.arg_type == DOUBLE && op2.arg_type == INTEGER) {
                dest.arg_type = DOUBLE;
                dest.data.d = op1.data.d - op2.data.i;
            } else {
                dest.arg_type = DOUBLE;
                dest.data.d = op1.data.i - op2.data.d;
            }
        }
    }

    stack_push(&glob_stack, dest);
}

void expr_mul() {
    argument_var_t dest, op1, op2;

    op2 = stack_pop(glob_stack);
    op1 = stack_pop(glob_stack);

    if (op1.arg_type == INTEGER && op2.arg_type == INTEGER) {
        dest.arg_type = INTEGER;
        dest.data.i = op1.data.i * op2.data.i;
    } else {
        if (op1.arg_type == DOUBLE && op2.arg_type == DOUBLE) {
            dest.arg_type = DOUBLE;
            dest.data.d = op1.data.d * op2.data.d;
        } else {
            if (op1.arg_type == DOUBLE && op2.arg_type == INTEGER) {
                dest.arg_type = DOUBLE;
                dest.data.d = op1.data.d * op2.data.i;
            } else {
                dest.arg_type = DOUBLE;
                dest.data.d = op1.data.i * op2.data.d;
            }
        }
    }

    stack_push(&glob_stack, dest);
}

void expr_div() {
    argument_var_t dest, op1, op2;

    op2 = stack_pop(glob_stack);
    op1 = stack_pop(glob_stack);

    if (op2.arg_type == INTEGER) {
        if (!op2.data.i) { //division by zero error
            error_and_die(RUNTIME_NULLDIVISION, "Division by 0");
        }
    } else {
        if (!op2.data.d) {
            error_and_die(RUNTIME_NULLDIVISION, "Division by 0");
        }
    }

    if (op1.arg_type == INTEGER && op2.arg_type == INTEGER) {
        dest.arg_type = INTEGER;
        dest.data.i = op1.data.i / op2.data.i;
    } else {
        if (op1.arg_type == DOUBLE && op2.arg_type == DOUBLE) {
            dest.arg_type = DOUBLE;
            dest.data.d = op1.data.d / op2.data.d;
        } else {
            if (op1.arg_type == DOUBLE && op2.arg_type == INTEGER) {
                dest.arg_type = DOUBLE;
                dest.data.d = op1.data.d / op2.data.i;
            } else {
                dest.arg_type = DOUBLE;
                dest.data.d = op1.data.i / op2.data.d;
            }
        }
    }

    stack_push(&glob_stack, dest);
}

void compare(){
    argument_var_t op1, op2, dest;

    dest.arg_type = INTEGER;
    op2 = stack_pop(glob_stack);
    op1 = stack_pop(glob_stack);

    switch (glob_ins_list->active->instruction.type){
        case INST_EXPR_LOWER:
            if (op1.arg_type == INTEGER && op2.arg_type == INTEGER) {
                dest.data.i = op1.data.i < op2.data.i;
            } else {
                if (op1.arg_type == DOUBLE && op2.arg_type == DOUBLE) {
                    dest.data.i = op1.data.d < op2.data.d;
                } else {
                    if (op1.arg_type == DOUBLE && op2.arg_type == INTEGER) {
                        dest.data.i = op1.data.d < op2.data.i;
                    } else {
                        dest.data.i = op1.data.i < op2.data.d;
                    }
                }
            }
            break;
        case INST_EXPR_HIGHER:
            if (op1.arg_type == INTEGER && op2.arg_type == INTEGER) {
                dest.data.i = op1.data.i > op2.data.i;
            } else {
                if (op1.arg_type == DOUBLE && op2.arg_type == DOUBLE) {
                    dest.data.i = op1.data.d > op2.data.d;
                } else {
                    if (op1.arg_type == DOUBLE && op2.arg_type == INTEGER) {
                        dest.data.i = op1.data.d > op2.data.i;
                    } else {
                        dest.data.i = op1.data.i > op2.data.d;
                    }
                }
            }
            break;
        case INST_EXPR_LOW_EQ:
            if (op1.arg_type == INTEGER && op2.arg_type == INTEGER) {
                dest.data.i = op1.data.i <= op2.data.i;
            } else {
                if (op1.arg_type == DOUBLE && op2.arg_type == DOUBLE) {
                    dest.data.i = op1.data.d <= op2.data.d;
                } else {
                    if (op1.arg_type == DOUBLE && op2.arg_type == INTEGER) {
                        dest.data.i = op1.data.d <= op2.data.i;
                    } else {
                        dest.data.i = op1.data.i <= op2.data.d;
                    }
                }
            }
            break;
        case INST_EXPR_HIG_EQ:
            if (op1.arg_type == INTEGER && op2.arg_type == INTEGER) {
                dest.data.i = op1.data.i >= op2.data.i;
            } else {
                if (op1.arg_type == DOUBLE && op2.arg_type == DOUBLE) {
                    dest.data.i = op1.data.d >= op2.data.d;
                } else {
                    if (op1.arg_type == DOUBLE && op2.arg_type == INTEGER) {
                        dest.data.i = op1.data.d >= op2.data.i;
                    } else {
                        dest.data.i = op1.data.i >= op2.data.d;
                    }
                }
            }
            break;
        case INST_EXPR_EQUAL:
            if (op1.arg_type == INTEGER && op2.arg_type == INTEGER) {
                dest.data.i = op1.data.i == op2.data.i;
            } else {
                if (op1.arg_type == DOUBLE && op2.arg_type == DOUBLE) {
                    dest.data.i = op1.data.d == op2.data.d;
                } else {
                    if (op1.arg_type == DOUBLE && op2.arg_type == INTEGER) {
                        dest.data.i = op1.data.d == op2.data.i;
                    } else {
                        dest.data.i = op1.data.i == op2.data.d;
                    }
                }
            }
            break;
        case INST_EXPR_NOT_EQ:
            if (op1.arg_type == INTEGER && op2.arg_type == INTEGER) {
                dest.data.i = op1.data.i != op2.data.i;
            } else {
                if (op1.arg_type == DOUBLE && op2.arg_type == DOUBLE) {
                    dest.data.i = op1.data.d != op2.data.d;
                } else {
                    if (op1.arg_type == DOUBLE && op2.arg_type == INTEGER) {
                        dest.data.i = op1.data.d != op2.data.i;
                    } else {
                        dest.data.i = op1.data.i != op2.data.d;
                    }
                }
            }
            break;
        default:
            break;
    }

    stack_push(&glob_stack,dest);
}

void expr_str_add(){ //TODO test
    argument_var_t dest, *op1, *op2;
    char str_op1[1024] = {0,};
    char str_op2[1024] = {0,};

    op2 = stack_top_ptr(glob_stack);
    op1 = stack_from_top_ptr(glob_stack, 1);

    dest.arg_type = STRING;
    dest.data.s = str_init();
    if (op2->arg_type == STRING && op1->arg_type == STRING) {
        str_concatenate(&dest.data.s, &op1->data.s, &op2->data.s);
    } else {
        if (op1->arg_type == INTEGER) {
            sprintf(str_op1, "%d", op1->data.i);
        } else {
            if (op1->arg_type == DOUBLE) {
                sprintf(str_op1, "%g", op1->data.d);
            }
        }
        if (op1->arg_type == STRING) {
            str_append_str(&dest.data.s, &op1->data.s);
        } else {
            str_append_chars(&dest.data.s, str_op1);
        }


        if (op2->arg_type == INTEGER) {
            sprintf(str_op2, "%d", op1->data.i);
        } else {
            if (op2->arg_type == DOUBLE) {
                sprintf(str_op2, "%g", op1->data.d);
            }
        }
        if (op2->arg_type == STRING) {
            str_append_str(&dest.data.s, &op2->data.s);
        } else {
            str_append_chars(&dest.data.s, str_op2);
        }
    }

    stack_pop(glob_stack);
    stack_pop(glob_stack);
    stack_push(&glob_stack,dest);
}

void pop(){
    argument_var_t tmp;
    tmp = stack_pop(glob_stack);
    if (tmp.arg_type == STRING) {
        str_destroy(&tmp.data.s);
    }
}

void inst_jump() {
    tmp_ptr = glob_ins_list->active->instruction.addr1;     //nacita z tabulky symbolov ukazatel na instrukciu
    glob_ins_list->active = tmp_ptr->data.instruction;                                  //priradi ukazatel do listu, takze zmeni tok programu
}

void jump_zero() {
    tmp_ptr = glob_ins_list->active->instruction.addr2;     //nacita operand pre porovnanie

    switch (tmp_ptr->arg_type){
        case STACK_EBP:
            tmp_ptr = stack_ebp_relative_ptr(glob_stack,tmp_ptr->data.i);
            break;
        case ON_TOP:
            tmp_ptr = stack_top_ptr(glob_stack);
            break;
        default:
            break;
    }

    if (tmp_ptr->data.i == 0) {                                                            //ak je operand nulovy takze false, urobi sa skok
        tmp_ptr = glob_ins_list->active->instruction.addr1;  //nacita z tabulky symbolov ukazatel na instrukciu
        glob_ins_list->active = tmp_ptr->data.instruction;                               //priradi ukazatel do listu, takze zmeni tok programu
    }
}

void jump_not_zero(){
    tmp_ptr = glob_ins_list->active->instruction.addr2;     //nacita operand pre porovnanie

    switch (tmp_ptr->arg_type){
        case STACK_EBP:
            tmp_ptr = stack_ebp_relative_ptr(glob_stack,tmp_ptr->data.i);
            break;
        case ON_TOP:
            tmp_ptr = stack_top_ptr(glob_stack);
            break;
        default:
            break;
    }

    if (tmp_ptr->data.i) {                                                            //ak je operand nenulovy takze true, urobi sa skok
        tmp_ptr = glob_ins_list->active->instruction.addr1;  //nacita z tabulky symbolov ukazatel na instrukciu
        glob_ins_list->active = tmp_ptr->data.instruction;                               //priradi ukazatel do listu, takze zmeni tok programu
    }
}

void jump_equal() {
    argument_var_t *op1, *op2;

    //nacitanie oprandov z tabulky symbolov
    op1 = glob_ins_list->active->instruction.addr2;
    op2 = glob_ins_list->active->instruction.addr3;

    //nacitanie operandov ak su na zasobniku
    if (op1->arg_type == STACK_EBP){
        op1 = stack_ebp_relative_ptr(glob_stack,op1->data.i);
    }
    if (op2->arg_type == STACK_EBP){
        op2 = stack_ebp_relative_ptr(glob_stack,op1->data.i);
    }

    //porovnavanie  a ak su rovnke skoci na danu adresu
    if (op1->arg_type == INTEGER){
        if (op2->arg_type == INTEGER){
            if (op1->data.i == op2->data.i){
                tmp_ptr = glob_ins_list->active->instruction.addr1;  //nacita z tabulky symbolov ukazatel na instrukciu
                glob_ins_list->active = tmp_ptr->data.instruction;
                return;
            }
        } else {
            if (op1->data.i == op2->data.d){
                tmp_ptr = glob_ins_list->active->instruction.addr1;  //nacita z tabulky symbolov ukazatel na instrukciu
                glob_ins_list->active = tmp_ptr->data.instruction;
                return;
            }
        }
    } else {
        if (op2->arg_type == INTEGER){
            if (op1->data.d == op2->data.i){
                tmp_ptr = glob_ins_list->active->instruction.addr1;  //nacita z tabulky symbolov ukazatel na instrukciu
                glob_ins_list->active = tmp_ptr->data.instruction;
                return;
            }
        } else {
            if (op1->data.d == op2->data.d){
                tmp_ptr = glob_ins_list->active->instruction.addr1;  //nacita z tabulky symbolov ukazatel na instrukciu
                glob_ins_list->active = tmp_ptr->data.instruction;
                return;
            }
        }
    }
}

void jump_not_equal() {
    argument_var_t *op1, *op2;

    //nacitanie oprandov z tabulky symbolov
    op1 =
            glob_ins_list->active->instruction.addr2;
    op2 =
            glob_ins_list->active->instruction.addr3;

    //nacitanie operandov ak su na zasobniku
    if (op1->arg_type == STACK_EBP){
        op1 = stack_ebp_relative_ptr(glob_stack,op1->data.i);
    }
    if (op2->arg_type == STACK_EBP){
        op2 = stack_ebp_relative_ptr(glob_stack,op1->data.i);
    }

    //porovnavanie  a ak su rovnke skoci na danu adresu
    if (op1->arg_type == INTEGER){
        if (op2->arg_type == INTEGER){
            if (op1->data.i != op2->data.i){
                tmp_ptr =
                        glob_ins_list->active->instruction.addr1;  //nacita z tabulky symbolov ukazatel na instrukciu
                glob_ins_list->active = tmp_ptr->data.instruction;
                return;
            }
        } else {
            if (op1->data.i != op2->data.d){
                tmp_ptr =
                        glob_ins_list->active->instruction.addr1;  //nacita z tabulky symbolov ukazatel na instrukciu
                glob_ins_list->active = tmp_ptr->data.instruction;
                return;
            }
        }
    } else {
        if (op2->arg_type == INTEGER){
            if (op1->data.d != op2->data.i){
                tmp_ptr = glob_ins_list->active->instruction.addr1;  //nacita z tabulky symbolov ukazatel na instrukciu
                glob_ins_list->active = tmp_ptr->data.instruction;
                return;
            }
        } else {
            if (op1->data.d != op2->data.d){
                tmp_ptr = glob_ins_list->active->instruction.addr1;  //nacita z tabulky symbolov ukazatel na instrukciu
                glob_ins_list->active = tmp_ptr->data.instruction;
            }
        }
    }}

void interpret_str_init() {
    argument_var_t *arg_ptr = glob_ins_list->active->instruction.addr1;
    if (arg_ptr->arg_type == STACK_EBP) {
        arg_ptr = stack_ebp_relative_ptr(glob_stack, arg_ptr->data.i);
    }
    arg_ptr->data.s = str_init();
}

void interpret_str_reinit() {
    argument_var_t *arg_ptr = glob_ins_list->active->instruction.addr1;
    if (arg_ptr->arg_type == STACK_EBP) {
        arg_ptr = stack_ebp_relative_ptr(glob_stack, arg_ptr->data.i);
    }
    str_reinit(&arg_ptr->data.s);
}

void str_len() {
    argument_var_t *arg1, *arg2;

    //nacita hodnoty z tabulky symbolov
    arg1 = glob_ins_list->active->instruction.addr1;
    arg2 = glob_ins_list->active->instruction.addr2;

    if (arg1->arg_type == STACK_EBP) {
        arg1 = stack_ebp_relative_ptr(glob_stack, arg1->data.i);
    }

    if (arg2->arg_type == STACK_EBP) {
        arg2 = stack_ebp_relative_ptr(glob_stack, arg2->data.i);
    }

    arg1->data.i = arg2->data.s.len;
}

void concatenate() {
    argument_var_t *arg1, *arg2, *arg3;

    //nacita hodnoty z tabulky symbolov
    arg1 = glob_ins_list->active->instruction.addr1;
    arg2 = glob_ins_list->active->instruction.addr2;
    arg3 = glob_ins_list->active->instruction.addr3;

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

    str_concatenate(&arg1->data.s, &arg2->data.s, &arg3->data.s);
}

void interpret_str_cmp() {
    argument_var_t *arg1, *arg2, *arg3;

    //nacita hodnoty z tabulky symbolov
    arg1 = glob_ins_list->active->instruction.addr1;
    arg2 = glob_ins_list->active->instruction.addr2;
    arg3 = glob_ins_list->active->instruction.addr3;

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

    arg1->data.i = str_cmp(arg2->data.s, arg3->data.s);
}

void call_print() {
    argument_var_t *num_of_str = glob_ins_list->active->instruction.addr1;
    argument_var_t str_to_print;

    if (num_of_str->arg_type == STACK_EBP) {
        num_of_str = stack_ebp_relative_ptr(glob_stack, num_of_str->data.i);
    }

    for (int i = num_of_str->data.i - 1; i >= 0; i--) {
        str_to_print = stack_from_top(glob_stack, i);
        //printf("%s",str_to_print.data.s.str);
        switch (str_to_print.arg_type) {
            case STACK_EBP:
                str_to_print = stack_ebp_relative(glob_stack, str_to_print.data.i);
            case INTEGER:
                printf("%d", str_to_print.data.i);
                break;
            case DOUBLE:
                printf("%g", str_to_print.data.d);
                break;
            case STRING:
                printf("%s", str_to_print.data.s.str);
                break;
            default:
                break;
        }
    }
}

void call_str_cmp() {
    argument_var_t *ret, str1, str2;
    str1 = stack_from_top(glob_stack, 1);
    str2 = stack_top(glob_stack);
    ret = glob_ins_list->active->instruction.addr1;

    if (str1.arg_type == STACK_EBP) {
        str1 = stack_ebp_relative(glob_stack, str1.data.i);
    }
    if (str2.arg_type == STACK_EBP) {
        str2 = stack_ebp_relative(glob_stack, str2.data.i);
    }
    if (ret->arg_type == STACK_EBP) {
        ret = stack_ebp_relative_ptr(glob_stack, ret->data.i);
    }

    ret->data.i = str_cmp(str1.data.s, str2.data.s);
}

void call_str_find() {
    argument_var_t *ret, *str, *search;
    str = stack_from_top_ptr(glob_stack, 1);
    search = stack_top_ptr(glob_stack);

    ret = glob_ins_list->active->instruction.addr1;

    if (str->arg_type == STACK_EBP) {
        str = stack_ebp_relative_ptr(glob_stack, str->data.i);
    }
    if (search->arg_type == STACK_EBP) {
        search = stack_ebp_relative_ptr(glob_stack, search->data.i);
    }
    if (ret->arg_type == STACK_EBP) {
        ret = stack_ebp_relative_ptr(glob_stack, ret->data.i);
    }

    if (!search->data.s.len){
        ret->data.i = 0;
    } else {
        int tmp = str_find(&str->data.s, &search->data.s);
        if (tmp == str->data.s.len){
            ret->data.i = -1;
        } else {
            ret->data.i = tmp;
        }
    }
}

void call_str_sort() {
    argument_var_t *source, *destination;

    source = stack_top_ptr(glob_stack);
    destination = glob_ins_list->active->instruction.addr1;

    if (source->arg_type == STACK_EBP){
        source = stack_ebp_relative_ptr(glob_stack,source->data.i);
    }
    if (destination->arg_type == STACK_EBP) {
        destination = stack_ebp_relative_ptr(glob_stack, destination->data.i);
    }

    str_reinit(&destination->data.s);
    str_append_str(&destination->data.s,&source->data.s);
    str_sort(&destination->data.s);
}

void call_str_substr() {
    argument_var_t *ret, *str, *i,*n;
    str = stack_from_top_ptr(glob_stack, 2);
    i = stack_from_top_ptr(glob_stack, 1);
    n = stack_top_ptr(glob_stack);
    ret = glob_ins_list->active->instruction.addr1;

    if (str->arg_type == STACK_EBP) {
        str = stack_ebp_relative_ptr(glob_stack, str->data.i);
    }
    if (i->arg_type == STACK_EBP) {
        i = stack_ebp_relative_ptr(glob_stack, i->data.i);
    }
    if (n->arg_type == STACK_EBP) {
        n = stack_ebp_relative_ptr(glob_stack, n->data.i);
    }
    if (ret->arg_type == STACK_EBP) {
        ret = stack_ebp_relative_ptr(glob_stack, ret->data.i);
    }

    //vycistenie stringu a ulozenie vysledku
    str_destroy(&ret->data.s);
    ret->data.s = str_sub_str(str->data.s,i->data.i,n->data.i);
}

void call_str_len(){
    argument_var_t *source, *destination;

    source = stack_top_ptr(glob_stack);
    destination = glob_ins_list->active->instruction.addr1;

    if (source->arg_type == STACK_EBP){
        source = stack_ebp_relative_ptr(glob_stack,source->data.i);
    }
    if (destination->arg_type == STACK_EBP) {
        destination = stack_ebp_relative_ptr(glob_stack, destination->data.i);
    }

    destination->data.i = source->data.s.len;
}
