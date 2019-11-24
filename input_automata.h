#ifndef  INPUT
#define  INPUT

#include <string.h>
#include "structs.h"

#define ERRO_TYPE  30
#define ERRO_VALUE 31

typedef enum
{
    START=0, STR, CMD, NUM, FLOAT, COMENT, FCOMENT, COMENT_C, COMENT_CC, FCOMENT_C,
}state_e;

typedef enum
{
    COMAND=0, FEOF, OPEN_KEY, PCLOSE_KEY, CLOSE_KEY, STRNG=5, EQUAL, ALL, OPEN_COMENT, ERROR,
    CLOSE_COMENT=10, NUMBER,
}token_e;

typedef struct
{
    char    token[256];
    token_e type;
}token_t;

FILE* arch_file;

void    coment();
void    read_xml(const char path[]);
void    erro(token_e requerido, token_e lexema);
token_t parser();
token_t consome_token(token_e requerido);

void    start();
void    stack();
void    single();
void    get_state_single();
void    get_trans_sigle();
void    type_automata(int type);

#endif
