#ifndef STR
#define STR

#define STR_ALLOC_SIZE  256

typedef struct string {
    unsigned len;
    unsigned max;
    char *str;
} string_t;

string_t str_init();
void str_reinit(string_t *str);
void str_destroy(string_t str);

#define ADD_CHAR(string,c)  {if (string.len < string.max)string.str[string.len++] = c;\
                                else str_resize(&string);}


void str_print(string_t str);
void str_resize(string_t *str);

#endif  //STR