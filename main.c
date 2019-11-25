#include <stdio.h>
#include "automaton.h"
#include "input_automata.h"

int main(int argc, char const *argv[]) {

    read_xml(argv[1]);
    unify();

    for (size_t i = 0; i < number_states; i++)
    {
        printf("ID: %d\tTrans: ", states[i].id);
        for (transition_t* aux = states[i].transition; aux; aux=aux->next_transition)
            printf("[%d {%s}]\t", aux->state_destiny->id, aux->simbol_transition);
        printf("\n");
    }
    printf("\n");
    return 0;
}
