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
    new_str.max = STR_ALLOC_SIZE;

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


void str_append_chars(string_t *str_dest,char *str_src){
    int i = 0;

    while (str_src[i] != '\0'){
        if (str_dest->len >= str_dest->max) {
            str_resize(str_dest);
        }
        str_dest->str[str_dest->len++] = str_src[i++];
    }
}

void str_append_str(string_t *str_dest,string_t *str_src){
    while ((str_dest->len + str_src->len) > str_dest->max) {
        str_resize(str_dest);
    }

    for (int i = 0; i < str_src->len; i++){
        str_dest->str[str_dest->len++] = str_src->str[i];
    }

}

void str_read_str_stdin(string_t* dest_str){
    int c;

    while ((c = fgetc(stdin)) != EOF && c != '\n'){
       str_add_char(dest_str,(char)c);
    }
}

void str_destroy(string_t str) {
    free(str.str);
}