#ifndef STRUCT
#define STRUCT

#include <stdlib.h>

#define ERROR_IO    10

#define TRUE  1
#define FALSE 0
#define true  1
#define false 0

typedef int8_t bool;
typedef struct transition_t transition_t;
typedef struct state_t      state_t;


state_t*         state_current_automaton;
// char             stack_automaton[256] = "";
// extern bool      stack_enable         = false;
// extern u_int16_t stack_control        = 0;


struct transition_t
{
    state_t*       state_destiny;
    char           simbol_transition;
    char           simbol_read_stack;
    char           simbol_write_stack;
    transition_t*  next_transition;
};

struct state_t
{
    u_int16_t      id;
    bool           final_state;
    transition_t*  transition;
};

#endif
