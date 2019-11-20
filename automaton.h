#ifndef AUTOMATON
#define AUTOMATON

#include <string.h>
#include <stdio.h>
#include "structs.h"

bool     execute_automata(state_t* automata, char input[]);
state_t* transition_automaton(state_t* state_current, char simbol);



#endif
