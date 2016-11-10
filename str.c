#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "str.h"

string_t str_init(){
    string_t new_str;

    new_str.str = calloc(STR_ALLOC_SIZE + 1,sizeof(char));
    if (new_str.str == NULL){
        //TODO error
    }
    new_str.len = 0;

    return new_str;
}

void str_reinit(string_t *str){
    if (str->str != NULL) {
        free(str->str);
    }

    str->max = STR_ALLOC_SIZE;
    str->len = 0;

    if ((str->str = calloc(STR_ALLOC_SIZE + 1,sizeof(char))) != NULL) {
        //TODO error
    }
}
void str_print(string_t str){
    printf("%s\n",str.str);
    printf("len: %d\n",str.len);
}
void str_resize(string_t *str) {
    char *new_str;
    str->max += STR_ALLOC_SIZE;
    if ((new_str = calloc(str->max + 1, sizeof(char))) == NULL){
        //TODO error
    }
    strcpy(new_str,str->str);
    free(str->str);
    str->str = new_str;
}

void str_destroy(string_t str) {
    free(str.str);
}