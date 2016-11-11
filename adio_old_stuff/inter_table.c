//
// Created by k on 9.10.2016.
//
#include <stdlib.h>
#include "inter_table.h"

unsigned hash_fun_ptr(unsigned id, unsigned stab_size) {
    return id % stab_size;
}

//alokuje priestor pre tabulku a zaroven ho nuluje
inter_table_t *inter_table_init(unsigned size) {
    inter_table_t *pom;
    if ((pom = calloc(1, sizeof(inter_table_t) + sizeof(inter_table_elem_t) * size)) ==
        NULL) //TODO : kiko pozri ci tu nema byt *inter_table_elem_t
        return NULL;
    pom->tab_size = size;
    pom->hash_fun_ptr = hash_fun_ptr;
    pom->tab_count = 0;

    return pom;
}

//rekurzivne prejde jednosmerne viazany zoznam elementov a uvolni pamat
void stable_destroy_element(inter_table_elem_t *p_element) {
    if (p_element->tab_next->tab_next != NULL)
        stable_destroy_element(p_element->tab_next);
    free(p_element->tab_next);
    p_element->tab_next = NULL;
    return;
}

//zavola funkciu na destrukciu prvkov v tabulke a potm uvolni tabulku
void inter_table_destroy(inter_table_t **p_table) {
    inter_table_elem_t *current, *next;

    for (unsigned i = 0; i < p_table[0]->tab_size; i++) {
        current = p_table[0]->arr[i];
        while (current != NULL) {
            next = current->tab_next;
            free(current);
            current = next;
        }
    }
    free(*p_table);
    return;
}

//pridava polozku do zoznamu
int inter_table_add_var(inter_table_t *p_stable, unsigned id, argument_var_t *p_var) {
    unsigned index = hash_fun_ptr(id, p_stable->tab_size);
    inter_table_elem_t *pom = p_stable->arr[index];

    if (p_stable->arr[index] != NULL) {
        //nastavy pom na posledny prvok
        while (pom->tab_next != NULL)
            pom = pom->tab_next;
        //za posledny prvok sa naalokuje miesto na dalsi
        if ((pom->tab_next = malloc(sizeof(inter_table_elem_t))) == NULL) {
            return 1; //todo dohodnut sa na error hlaske
        }
        pom = pom->tab_next;
    } else {
        if ((p_stable->arr[index] = malloc(sizeof(inter_table_elem_t))) == NULL) {
            return 1;
        }

        pom = p_stable->arr[index];
    }

    pom->tab_content = *p_var;
    pom->tab_key = id;
    pom->tab_next = NULL;


    return 0;
}

//vrati polozku zo zoznamu
argument_var_t *inter_table_get_var(inter_table_t *p_stable, unsigned id) {
    unsigned index = hash_fun_ptr(id, p_stable->tab_size);
    inter_table_elem_t *pom = p_stable->arr[index];

    if (pom == NULL)
        return NULL;

    //najde polozku s id
    while (pom->tab_key != id)
        if ((pom = pom->tab_next) == NULL)
            return NULL;

    return &pom->tab_content;
}

//odstrni polozkiu zo zoznamu
void inter_table_remove_var(inter_table_t *p_stable, unsigned id) {
    unsigned index = hash_fun_ptr(id, p_stable->tab_size);
    inter_table_elem_t *pom = p_stable->arr[index];

    if (pom == NULL)
        return;

    //najde polozku pred polozkou s id
    while (pom->tab_next->tab_key != id)
        if ((pom = pom->tab_next) == NULL)
            return;

    return;
}