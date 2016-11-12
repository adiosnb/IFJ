#include "str.h"
#include <stdlib.h>

int main() {
    str_list_t list;
    list = str_list_init();
    char *ptr;


    for (int i = 0; i < 10000; i++) {
        ptr = malloc(sizeof(char) * 16);
        str_list_add(&list, ptr);
    }

    str_list_destroy(&list);

    return 0;
}