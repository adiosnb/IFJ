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
void str_print(string_t str);
void str_resize(string_t *str);
void str_append_str(string_t *str_dest,string_t *str_src);
void str_append_chars(string_t *str_dest,char *str_src);


//makro pre pridanie znaku do stringu
#define ADD_CHAR(string,c)  {if (string.len < string.max)str_resize(&string);\
                             string.str[string.len++] = c;   }

//inline funkcie pre pridanie znaku, asi lepsia alternativa ako makro
static inline void str_add_char(string_t *string, char c){
    if (string->len < string->max) {
        str_resize(string);
    }
    string->str[string->len++] = c;
}



#endif  //STR