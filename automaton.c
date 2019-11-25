#include "automaton.h"

void unify()
{
    for (size_t i = 0; i < number_transitions; i++)
    {
        transitions[i].state_origin  = get_state(transitions[i].id_origin);
        transitions[i].state_destiny = get_state(transitions[i].id_destiny);

        if (!transitions[i].state_origin)
        {
            fprintf(stderr, "\n[ERRO:] Estado de ID '%d' não encontrado\n\n", transitions[i].id_origin);
            exit(ERRO_NOSTATE);
        }
        if (!transitions[i].state_destiny)
        {
            fprintf(stderr, "\n[ERRO:] Estado de ID '%d' não encontrado\n\n", transitions[i].id_destiny);
            exit(ERRO_NOSTATE);
        }
    }
    link_trans();
}

state_t* get_state(u_int16_t id)
{
    for (size_t i = 0; i < number_states; i++)
    {
        if (states[i].id == id)
          return &(states[i]);
    }
    return NULL;
}

void link_trans()
{
    for (size_t i = 0; i < number_transitions; i++)
    {
        if (!transitions[i].state_origin->transition)
        {
            transitions[i].state_origin->transition = &(transitions[i]);
            continue;
        }
        transition_t* aux = transitions[i].state_origin->transition;
        while(aux->next_transition)
        {
            aux = aux->next_transition;
        }
        aux->next_transition = &(transitions[i]);
    }
}
