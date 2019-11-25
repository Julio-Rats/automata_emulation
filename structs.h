#ifndef STRUCT
#define STRUCT

#include <stdio.h>
#include <stdlib.h>

#define ERROR_IO    10

#define TRUE  1
#define FALSE 0
#define true  1
#define false 0

typedef int8_t bool;
typedef struct transition_t transition_t;
typedef struct stack_move   stack_move;
typedef struct state_t      state_t;


extern state_t*  state_current_automaton;
extern state_t*  start_state;
// char             stack_automaton[256] = "";
// extern bool      stack_enable         = false;
// extern u_int16_t stack_control        = 0;

extern state_t*  states;
extern u_int16_t number_states;

extern transition_t*  transitions;
extern u_int16_t      number_transitions;

struct transition_t
{
    state_t*       state_origin;
    u_int16_t      id_origin;
    state_t*       state_destiny;
    u_int16_t      id_destiny;
    char           simbol_transition[64];
    stack_move*    simbol_stacking;
    transition_t*  next_transition;
};

struct stack_move
{
    char           simbol_read_stack;
    char           simbol_write_stack;
};

struct state_t
{
    u_int16_t      id;
    bool           final_state;
    transition_t*  transition;
};

#endif
