#ifndef STR
#define STR

#define STR_ALLOC_SIZE  256

typedef struct string {
    unsigned len;
    unsigned max;
    char *str;
} string_t;

typedef struct str_list {
    char *str;
    struct str_list *next;
} str_list_item_t;

typedef struct {
    str_list_item_t *first;
    str_list_item_t *last;
} str_list_t;


string_t str_init();
void str_reinit(string_t *str);
void str_destroy(string_t str);
void str_print(string_t str);
void str_resize(string_t *str);
void str_append_str(string_t *str_dest,string_t *str_src);

int str_find(string_t *str,string_t *search);   //TODO
string_t str_sort(string_t str);                //TODO
string_t sub_str(string_t str, int i, int n);   //TODO

void str_concatenate(string_t *str_dest, string_t *str_src1, string_t *str_src2);
void str_append_chars(string_t *str_dest,char *str_src);
void str_read_str_stdin(string_t* dest_str);

int str_cmp(string_t str1, string_t str2);

//makro pre pridanie znaku do stringu
#define ADD_CHAR(string,c)  {if (!(string.len < string.max)) str_resize(&string);\
                             string.str[string.len++] = c;   }

//inline funkcie pre pridanie znaku, asi lepsia alternativa ako makro
static inline void str_add_char(string_t *string, char c){
    if (string->len >= string->max) {
        str_resize(string);
    }
    string->str[string->len++] = c;
}



#endif  //STR