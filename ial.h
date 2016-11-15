//
// Created by k on 26.10.2016.
//

#ifndef IFJ_IAL_H
#define IFJ_IAL_H

//rekurzivna funkcia triediaca pole
void ial_qsort(char* arr, int left, int right);
//Knuth–Morris–Pratt algoritmus na vyhľadávanie podreťazca v reťazci
int KMP_hladaj(char *retazec, char *podretazec);

#endif //IFJ_IAL_H
