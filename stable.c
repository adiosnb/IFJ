//
// Created by k on 9.10.2016.
//
#include <stdlib.h>
#include <string.h>
#include "stable.h"

unsigned hash_fun_ptr(char *id, unsigned stab_size){
    int i = 0, pom = 0;

    while (id[i] != '\0'){
        pom += (int) id[i];
        i++;
    }
    return pom % stab_size;
}

//alokuje priestor pre tabulku a zaroven ho nuluje
stab_t *stable_init(unsigned size){
    stab_t *pom;
    if ((pom = calloc(1, sizeof(stab_t) + sizeof(stab_element_t) * size)) == NULL) //TODO : kiko pozri ci tu nema byt *stab_element_t
        return NULL;
    pom->stab_size = size;
    pom->hash_fun_ptr = hash_fun_ptr;
    pom->stab_count = 0;

    return pom;
}

//rekurzivne prejde jednosmerne viazany zoznam elementov a uvolni pamat
void stable_destroy_element(stab_element_t *p_element){
    if (p_element->stab_next->stab_next != NULL)
        stable_destroy_element(p_element->stab_next);
    free(p_element->stab_next);
    p_element->stab_next = NULL;
    return;
}

//zavola funkciu na destrukciu prvkov v tabulke a potm uvolni tabulku
void stable_destroy(stab_t **p_table) {
    stab_element_t *current, *next;

    for (unsigned i = 0; i < p_table[0]->stab_size; i++) {
        current = p_table[0]->arr[i];
        while (current != NULL) {
            next = current->stab_next;
            free(current);
            current = next;
        }
    }
    free(*p_table);
    return;
}

//pridava polozku do zoznamu
int stable_add_var(stab_t *p_stable, char *id, data_t p_var){

    unsigned index = hash_fun_ptr(id, p_stable->stab_size);
    stab_element_t *pom = p_stable->arr[index];

    if (p_stable->arr[index] != NULL) {
        //nastavy pom na posledny prvok
        while (pom->stab_next != NULL)
            pom = pom->stab_next;
        //za posledny prvok sa naalokuje miesto na dalsi
        if ((pom->stab_next = malloc(sizeof(stab_element_t))) == NULL){
            return 1; //todo dohodnut sa na error hlaske
        }
        pom = pom->stab_next;
    } else {
        if ((p_stable->arr[index] = malloc(sizeof(stab_element_t))) == NULL) {
            return 1;
        }

        pom = p_stable->arr[index];
    }

    pom->stab_content.inter_table = p_var.inter_table;
    pom->stab_content.type = p_var.type;
    pom->stab_key = id;
    pom->stab_next = NULL;


    return 0;
}

//vrati polozku zo zoznamu
data_t *stable_get_var(stab_t *p_stable, char *id){
    unsigned index = hash_fun_ptr(id, p_stable->stab_size);
    stab_element_t *pom = p_stable->arr[index];

    if (pom == NULL)
        return NULL;

    //najde polozku s id ( x == y tak strcmp == 0)
    while (strcmp(pom->stab_key, id))//todo skontroluj
        if ((pom = pom->stab_next) == NULL)
            return NULL;

    return &pom->stab_content;
}

//odstrni polozkiu zo zoznamu
void stable_remove_var(stab_t *p_stable, char *id){
    unsigned index = hash_fun_ptr(id, p_stable->stab_size);
    stab_element_t *pom = p_stable->arr[index];
    stab_element_t *vymaz;

    if (pom == NULL)
        return;

    //ak je to 1. polozka
    if (!strcmp(pom->stab_key, id)) {
        p_stable->arr[index] = pom->stab_next;
        vymaz = pom;
    }
    else {
        //ak je to 2. az n polozka
        //najde polozku pred polozkou s id
        while (strcmp(pom->stab_next->stab_key, id)) //todo skontroluj
            if ((pom = pom->stab_next) == NULL)
                return;
        vymaz = pom->stab_next;
        pom->stab_next = pom->stab_next->stab_next;
    }
    free(vymaz);

    return;
}

bool stable_search(stab_t *p_stable, char *srch_el){
    unsigned index = hash_fun_ptr(srch_el, p_stable->stab_size);
    stab_element_t *pom = p_stable->arr[index];

    if (pom == NULL)
        return false;

    //najde polozku s id
    while (strcmp(pom->stab_key, srch_el))
        if ((pom = pom->stab_next) == NULL)
            return false;

    return true;
}

bool stable_add_concatenate(stab_t *p_stable, char* clss, char *fnct, char *local, data_t data){
    char *pom;
    int j = 0, i = 0;
    int size = 0;

    if (clss != NULL) {
        size += strlen(clss);
        if (fnct != NULL) {
            size += strlen(fnct);
            if (local != NULL)
                size += strlen(local);
        }
    }

    if ((pom = malloc(size * sizeof(char) + 2)) == NULL) //+2 lebo sa vkadaju max 2x '.'
        return false;

    if(clss != NULL)
        while (clss[i] != '\0')
            pom[j++] = clss[i++];

    i = 0;
    if(fnct != NULL) {
        pom[j++] = '.';
        while (fnct[i] != '\0')
            pom[j++] = fnct[i++];
    }

    i=0;
    if(local != NULL) {
        pom[j++] = '.';
        while (local[i] != '\0')
            pom[j++] = local[i++];
    }

    if(stable_add_var(p_stable, pom, data))
        return false;
    return true;


}