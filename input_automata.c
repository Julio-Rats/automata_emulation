#include "input_automata.h"

void read_xml(char path[])
{
    FILE* arch_input = fopen(path, "r");

    if (!arch_input)
    {
        fprintf(stderr, "\n[ERROR:] LEITURA ARQUIVO XML\n\n");
        exit(ERROR_IO);
    }

    

}
