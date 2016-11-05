#include "str.h"

int main() {

    string_t str1;

    str1 = str_init();

    for (int i=0; i< 300000 ; i++){
        ADD_CHAR(str1,(i%10)+'0');
    }

    str_print(str1);

    str_reinit(&str1);
    for (int i=0; i< 30 ; i++){
        ADD_CHAR(str1,(i%10)+'0');
    }
    str_print(str1);

    str_destroy(str1);

    return 0;
}