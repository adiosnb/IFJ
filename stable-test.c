#include "stable.h"

int main(){

    stab_t *sym_table;
    sym_table = stable_init(1024);



    stable_destroy(&sym_table);

    return 0;
}