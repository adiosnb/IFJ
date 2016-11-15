//
// Created by k on 26.10.2016.
//

#include <stdio.h>
#include <string.h>
#include "ial.h"

void swap(char *arg1, char *arg2){
    char pom = *arg1;

    *arg1 = *arg2;
    *arg2 = pom;

    return;
}

void qsort(char *arr, int left, int right){

    if (left < right) {
        int pom = left;

        for (int i = left + 1; i <= right; i++)
            if (arr[left] < arr[i])
                swap(&arr[++pom], &arr[i]);
        swap(&arr[left], &arr[pom]);
        //volanie qsortu pre ľavú časť
        qsort(arr, left, pom - 1);
        //volanie qsortu pre pravú časť
        qsort(arr, pom + 1, right);
    }
    return;
}

void KMP_spracovanie_podretazca(char *podretazec, int *Table){

    //index zaciatku porovnavania pri nezhode podretazca s retazcom
    int ret_index = 0;

    Table[0] = -1;

    for (int i = 1; i < strlen(podretazec); i++){
        ret_index = Table[i-1];
        while (ret_index > -1 && podretazec[i-1] != podretazec[ret_index])
            ret_index = Table[ret_index];
        Table[i] = ret_index + 1;
    }


}

int KMP_hladaj(char *retazec, char *podretazec){

    int *Table;

    if ((Table = malloc(sizeof(Table) * strlen(podretazec))) == NULL)
        return -1;

    KMP_spracovanie_podretazca(podretazec, Table);

    int Retazec_index = 0;
    int Podretazec_index = 0;
    while (Podretazec_index < strlen(podretazec) && Retazec_index < strlen(retazec)){
        if (Podretazec_index == 0 || retazec[Retazec_index] == podretazec[Podretazec_index]){
            Retazec_index ++;
            Podretazec_index ++;
        }
        else
            Podretazec_index = Table[Podretazec_index];
    }

    return (Podretazec_index > strlen(podretazec)) ? (Retazec_index - strlen(podretazec)) : (Retazec_index);

}

