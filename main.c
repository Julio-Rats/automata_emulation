#include <stdio.h>
#include "automaton.h"

int main(int argc, char const *argv[]) {

    state_t a1 = {1, true, NULL};
    state_t a2 = {2, true, NULL};
    state_t a3 = {3, true, NULL};

    transition_t b2 = {&a2, 'B', ' ', ' ',  NULL}; // b
    transition_t b1 = {&a1, 'A', ' ', ' ',  &b2};  // a*
    transition_t b3 = {&a3, 'C', ' ', ' ',  &b2};  // b*
    transition_t b4 = {&a3, 'C', ' ', ' ',  NULL}; // b*

    a1.transition = &b1;
    a2.transition = &b3;
    a3.transition = &b4;


    printf("%d\n", execute_automata(&a1, argv[1]));
    return 0;
}
