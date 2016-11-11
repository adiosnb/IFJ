
#include <stdlib.h>
#include <stdio.h>
#include "stable.h"

int main(){

    data_t cont1, cont2, cont3;
    data_t *pom = NULL;
    cont1.type = 1;
    cont1.inter_table = 1;
    cont2.type = 2;
    cont2.inter_table = 2;
    cont3.type = 3;
    cont3.inter_table = 3;


    char string1[] = "string1";
    char string2[] = "string2";
    char string3[] = "string3";
    char string_concat1[] = "string1.string2";
    char string_concat2[] = "string1.string2.";
    char string_blank[] = "";

    printf("\n################### TEST_INIT ####################\n");
    stab_t *sym_table = stable_init(1024);
    if(sym_table == NULL)
        printf("\nstable_init FAILED !\n");
     else
        printf("\nstable_init OK !\n");




    printf("\n################### TEST_ADD_VAR ##################\n");

    if(stable_add_var(sym_table, string1, cont1) == 1)
        printf("\nstable_add_var FAILED !\n");
    else
        printf("\nstable add_var OK !\n");

    stable_add_var(sym_table, string2, cont2);
    stable_add_var(sym_table, string3, cont3);


    pom = stable_get_var(sym_table, string1);
    if  (pom != NULL)
        printf("string1:  %d\n", pom->type);

    pom = stable_get_var(sym_table, string3);
    if  (pom != NULL)
        printf("string3:  %d\n", pom->type);

    if(!stable_add_concatenate(sym_table, string1, string2, NULL, cont2))
        printf("nedostatok miesta\n");

    pom = stable_get_var(sym_table, string_concat1);
    printf("string1.string2:  %d\n", pom->type);

    if(!stable_add_concatenate(sym_table, string1, string2, string_blank, cont3))
        printf("nedostatok miesta\n");

    pom = stable_get_var(sym_table, string_concat2);
    printf("string1.string2.:  %d\n", pom->type);


    if(stable_search(sym_table, string1))
        printf("string 1 sa nasiel\n");
    else
        printf("string 1 sa nenasiel\n");

    if(stable_search(sym_table, string2))
        printf("string 2 sa nasiel\n");
    else
        printf("string 2 sa nenasiel\n");

    printf("\n################### TEST_REMOVE_VAR ##################\n");
    if(stable_remove_var(sym_table, string1) == false)
        printf("\nstable_remove_var FAILED !\n");
    else
        printf("\nstable_remove_var OK !\n");
    stable_remove_var(sym_table, string2);
    stable_remove_var(sym_table, string3);

    if(stable_search(sym_table, string1))
        printf("string 1 sa nasiel\n");
    else
        printf("string 1 sa nenasiel\n");

    stable_destroy(&sym_table);

    return 0;
}