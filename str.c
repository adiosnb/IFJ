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
	if(str.str)
	    free(str.str);
	str.str = NULL;
}

/*
 * Nasleduje spajany zoznam nad char*
 */

str_list_t str_list_init() {
    str_list_t list;
    list.first = NULL;
    list.last = NULL;
    return list;
}

void str_list_add(str_list_t *list, char *str) {
    str_list_item_t *new_item;
    if ((new_item = malloc(sizeof(struct str_list))) == NULL) {
        //TODO error
        exit(1);
    }
    if (list->last != NULL) {
        new_item->str = str;
        new_item->next = NULL;
        list->last->next = new_item;
        list->last = new_item;
    } else {
        list->first = list->last = new_item;
        new_item->str = str;
    }
}

void str_list_destroy(str_list_t *list) {
    str_list_item_t *current;

    while (list->first != NULL) {
        current = list->first;
        list->first = list->first->next;
        free(current->str);
        free(current);
    }
}
