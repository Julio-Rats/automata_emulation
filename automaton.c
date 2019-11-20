#include "automaton.h"

bool execute_automata(state_t* automata, char input[])
{
    state_current_automaton = automata;
    for(int i=0; i < strlen(input); i++)
    {
        state_current_automaton = transition_automaton(state_current_automaton, input[i]);
        if (!state_current_automaton)
          break;

    }
    if (state_current_automaton)
       return state_current_automaton->final_state;
    return false;
}

state_t* transition_automaton(state_t* state_current, char simbol)
{
    state_t* new_state = NULL;
    for(transition_t* tr=state_current->transition; tr; tr = tr->next_transition)
        if (tr->simbol_transition == simbol)
        {
            new_state = tr->state_destiny;
            break;
        }
    return new_state;
}
