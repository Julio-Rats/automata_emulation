#ifndef AUTOMATON
#define AUTOMATON

#define  ERRO_NOSTATE   40

#include <string.h>
#include <stdio.h>
#include "structs.h"

void     unify();
void     link_trans();
state_t* get_state(u_int16_t id);

#endif
