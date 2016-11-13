#include <stdio.h>
#include "str.h"

int main() {

    string_t str1, str2, str3;

    str1 = str_init();
    str2 = str_init();
    str3 = str_init();

    printf("\n###################TEST1####################\n");

    for (int i=0; i< 3000 ; i++){
        str_add_char(&str1,'0' + i%10);
    }

    str_print(str1);
    printf("\n###################TEST2####################\n");

    str_reinit(&str1);
    for (int i=0; i< 30 ; i++){
        str_add_char(&str1,'0' + i%10);
    }
    str_print(str1);
    str_reinit(&str1);
    printf("\n###################TEST3####################\n");
    str_append_chars(&str1,"prvy string\n");
    str_append_chars(&str1,"druhy string\n");

    printf("Ocakavany string :\n"
                   "prvy string\n"
                   "druhy string\n"
                   "Vystup :\n");
    str_print(str1);
    printf("\n###################TEST4####################\n");
    str_reinit(&str1);
    str_reinit(&str2);
    str_append_chars(&str1,"<toto je string1>");
    str_append_chars(&str2,"<toto je string2>");
    str_append_str(&str1,&str2);
    printf(""
                   "Ocakavany vystup:\n"
                   "<toto je string1><toto je string2>\n");
    str_print(str1);
    printf("\n###################TEST5####################\n");
    str_reinit(&str1);
    printf("Zadaj data na vstup:");
    str_read_str_stdin(&str1);
    str_print(str1);

    printf("\n################## TEST6 ###################\n");
    printf("str3 = str2 + str1\n");
    str_reinit(&str1);
    str_reinit(&str2);
    str_reinit(&str3);

    str_append_chars(&str1, "abcdefgh");
    str_append_chars(&str2, "123456789");
    str_concatenate(&str3, &str2, &str1);

    printf("str1 = ");
    str_print(str1);
    printf("str2 = ");
    str_print(str2);
    printf("str3 = ");
    str_print(str3);


    printf("\n################## TEST7 ###################\n");
    printf("str2 = str1 + str1\n");
    str_reinit(&str1);
    str_reinit(&str2);

    str_append_chars(&str1, "abcdefgh");
    str_concatenate(&str2, &str1, &str1);

    printf("str1 = ");
    str_print(str1);
    printf("str2 = ");
    str_print(str2);


    printf("\n################## TEST8 ###################\n");
    printf("str1 = str1 + str1\n");
    str_reinit(&str1);

    str_append_chars(&str1, "abcdefgh");

    printf("<before> str1 = ");
    str_print(str1);
    str_concatenate(&str1, &str1, &str1);
    printf("<after> str1 = ");
    str_print(str1);



    str_destroy(str1);
    str_destroy(str2);
    str_destroy(str3);

    return 0;
}