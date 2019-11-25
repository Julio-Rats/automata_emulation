#include "input_automata.h"

state_t*  state_current_automaton = NULL;
state_t*  start_state             = NULL;
state_t*  states                  = NULL;
transition_t*  transitions        = NULL;
u_int16_t number_states           = 0;
u_int16_t number_transitions      = 0;

void read_xml(const char path[])
{
    arch_file = fopen(path, "r");

    if (!arch_file)
    {
        fprintf(stderr, "\n[ERROR:] LEITURA ARQUIVO '%s' XML\n\n", path);
        exit(ERROR_IO);
    }

    start();

    fclose(arch_file);

    printf("\nLEITURA DO XML EFETUADA COM SUCESSO\n\n");
}

token_t parser()
{
    state_e  state_machine = START;
    char     token;
    u_int8_t token_len = 0;
    token_t  lexema;

    while (true)
    {
        token = fgetc(arch_file);

        if (feof(arch_file))
        {
            lexema.type = FEOF;
            strcpy(lexema.token,"FIM");
            token_len = 3;
            break;
        }

        if (state_machine == START)
        {
            if (token == '<')
            {
                strcpy(lexema.token, "<");
                state_machine = COMENT;
                token_len = 1;
                continue;
            }

            if (token == '>')
            {
                lexema.type = CLOSE_KEY;
                strcpy(lexema.token, ">");
                token_len = 1;
                break;
            }

            if (token == '/')
            {
                lexema.type = PCLOSE_KEY;
                strcpy(lexema.token, "/");
                token_len = 1;
                break;
            }

            if (token == '=')
            {
                lexema.type = EQUAL;
                strcpy(lexema.token, "=");
                token_len = 1;
                break;
            }

            if (token == '\"')
            {
                state_machine = STR;
                continue;
            }

            if (token == '-')
            {
                state_machine = FCOMENT;
                lexema.token[token_len++] = token;
                continue;
            }

            if (((token >= 'a') && (token <= 'z')) || ((token >= 'A') && (token <= 'Z')))
            {
                state_machine = CMD;
                lexema.token[token_len++] = token;
                continue;
            }

            if ((token >= '0') && (token <= '9'))
            {
                state_machine = NUM;
                lexema.token[token_len++] = token;
                continue;
            }

        } // start

        if (state_machine == STR)
        {
            if (token == '\"')
            {
                lexema.type = STRNG;
                break;
            }

            if (token == '\\')
            {
                token = fgetc(arch_file);
                lexema.token[token_len++] = token;
                continue;
            }
            lexema.token[token_len++] = token;
        } // STR

        if (state_machine == CMD)
        {
            if (((token >= 'a') && (token <= 'z')) || ((token >= 'A') && (token <= 'Z')) || ((token >= '0') && (token <= '9')))
            {
                lexema.token[token_len++] = token;
                continue;
            }
            else
            {
                ungetc(token, arch_file);
                lexema.type = COMAND;
                break;
            }

        } // COMAND

        if (state_machine == COMENT)
        {
            if (token == '!')
            {
                lexema.token[token_len++] = token;
                state_machine = COMENT_C;
                continue;
            }
            ungetc(token, arch_file);
            lexema.type = OPEN_KEY;
            break;
        } // COMENT

        if (state_machine == COMENT_C)
        {
            if (token == '-')
            {
                lexema.token[token_len++] = token;
                state_machine = COMENT_CC;
                continue;
            }
            ungetc(token, arch_file);
            ungetc('!', arch_file);
            lexema.type = OPEN_KEY;
            token_len--;
            break;
        } // COMENT_C

        if (state_machine == COMENT_CC)
        {
            if (token == '-')
            {
                lexema.token[token_len++] = token;
                lexema.type = OPEN_COMENT;
                break;
            }
            ungetc(token, arch_file);
            ungetc('-', arch_file);
            ungetc('!', arch_file);
            lexema.type = OPEN_KEY;
            token_len -= 2;
            break;
        } // COMENT

        if (state_machine == FCOMENT)
        {
            if (token == '-')
            {
                lexema.token[token_len++] = token;
                state_machine = FCOMENT_C;
                continue;
            }
            lexema.type = ERROR;
            break;
        } // FCOMENT

        if (state_machine == FCOMENT_C)
        {
            if (token == '>')
            {
                lexema.token[token_len++] = token;
                lexema.type = CLOSE_COMENT;
                break;
            }
            lexema.type = ERROR;
            break;
        } // FCOMENT_C

        if (state_machine == NUM)
        {
            if ((token >= '0') && (token <= '9'))
            {
                lexema.token[token_len++] = token;
                continue;
            }
            else if (token == '.')
            {
                state_machine = FLOAT;
                lexema.token[token_len++] = token;
                continue;
            }
            ungetc(token, arch_file);
            lexema.type = NUMBER;
            break;
        } // NUM


        if (state_machine == FLOAT)
        {
            if ((token >= '0') && (token <= '9'))
            {
                lexema.token[token_len++] = token;
                continue;
            }
            else if (token == '.')
            {
                lexema.token[token_len++] = token;
                lexema.type = ERROR;
                break;
            }
            else
            {
                ungetc(token, arch_file);
                lexema.type = NUMBER;
                break;
            }
        } // FLOAT

    } // WHILE

    lexema.token[token_len] = '\0';
    // printf("Parser: %s\n", lexema.token);
    return lexema;
}

#define FA  0
#define PDA 1

void start()
{
    token_t lexema;
    do
    {
        lexema = consome_token(ALL);
    }while ((lexema.type != COMAND) || (strcmp(lexema.token, "structure")!=0));

    consome_token(CLOSE_KEY);
    // <structure> FIM
    // <type>

    // ----------------------------------------------------------------
    //              TRATAMENTO DE COMENTARIOS NO XML
    // ----------------------------------------------------------------

    lexema = consome_token(ALL);

    if (lexema.type == OPEN_COMENT)
      do
      {
          if (lexema.type == OPEN_COMENT)
              coment();
          lexema = consome_token(ALL);
      }while (lexema.type != OPEN_KEY);

      lexema = consome_token(COMAND);
      // ----------------------------------------------------------------
      //           [FIM] TRATAMENTO DE COMENTARIOS NO XML
      // ----------------------------------------------------------------

    if (strcmp(lexema.token, "type") != 0)
    {
        fprintf(stderr, "\n[ERRO:] Comando não definido -> \"%s\"\n\n", lexema.token);
        exit(ERRO_VALUE);
    }
    consome_token(CLOSE_KEY);
    // <type> FIM
    // get type
    lexema = consome_token(COMAND);
    char type[256];
    strcpy(type, lexema.token);
    // </type>
    consome_token(OPEN_KEY);
    consome_token(PCLOSE_KEY);
    lexema = consome_token(COMAND);
    if (strcmp(lexema.token, "type") != 0)
    {
        fprintf(stderr, "\n[ERRO:] Comando type não fechado \n\n");
        exit(ERRO_VALUE);
    }
    consome_token(CLOSE_KEY);
    // <automaton>
    if (strcmp(type, "fa") == 0)
        type_automata(FA);
    else if (strcmp(type, "pda") == 0)
        type_automata(PDA);
    else
    {
        fprintf(stderr, "\n[ERRO:] Comando não definido -> \"%s\"\n\n", lexema.token);
        exit(ERRO_VALUE);
    }

    lexema = consome_token(ALL);

    if (lexema.type == OPEN_COMENT)
      do
      {
          if (lexema.type == OPEN_COMENT)
              coment();
          lexema = consome_token(ALL);
      }while (lexema.type != OPEN_KEY);

    // </structure>
    // consome_token(OPEN_KEY);
    consome_token(PCLOSE_KEY);
    lexema = consome_token(COMAND);
    if (strcmp(lexema.token, "structure") != 0)
    {
        fprintf(stderr, "\n[ERRO:] Comando structure não fechado \n\n");
        exit(ERRO_VALUE);
    }
    consome_token(CLOSE_KEY);

    lexema = consome_token(ALL);

    if (lexema.type == OPEN_COMENT)
      do
      {
          if (lexema.type == OPEN_COMENT)
              coment();
          lexema = consome_token(ALL);
      }while (lexema.type == OPEN_KEY);

    if (lexema.type != FEOF)
    {
        fprintf(stderr, "\n[ERRO:] Comando apos strucue não esperado\n\n");
        exit(ERRO_TYPE);
    }
}

void type_automata(int type)
{
    token_t lexema = consome_token(ALL);

    if (lexema.type == OPEN_COMENT)
      do
      {
          if (lexema.type == OPEN_COMENT)
              coment();
          lexema = consome_token(ALL);
      }while (lexema.type != OPEN_KEY);

    lexema = consome_token(COMAND);
    if (strcmp(lexema.token, "automaton")!=0)
    {
        fprintf(stderr, "\n[ERRO:] Comando 'automaton' não encontrado, encontrado -> \"%s\"\n\n", lexema.token);
        exit(ERRO_VALUE);
    }
    consome_token(CLOSE_KEY);

    if (type == FA)
    {
        single();
    }
    else
    // PDA //
    {
        stack();
    }

    lexema = consome_token(COMAND);
    if (strcmp(lexema.token, "automaton")!=0)
    {
        fprintf(stderr, "\n[ERRO:] Fechamento do 'automaton' não encontrado, encontrado -> \"%s\"\n\n", lexema.token);
        exit(ERRO_VALUE);
    }
    consome_token(CLOSE_KEY);
}

void single()
{
    while(1)
    {
        token_t lexema = consome_token(ALL);

        if (lexema.type == OPEN_COMENT)
          do{
              if (lexema.type == OPEN_COMENT)
                  coment();
              lexema = consome_token(ALL);
          }while (lexema.type != OPEN_KEY);

        lexema = consome_token(ALL);

        if (lexema.type == PCLOSE_KEY) // close automaton
          break;

        if ((lexema.type == COMAND) && (strcmp(lexema.token, "state")==0))
            get_state_single();
        else if ((lexema.type == COMAND) && (strcmp(lexema.token, "transition")==0))
            get_trans_sigle();
        else
        {
            fprintf(stderr, "\n[ERRO:] Comando não esperado -> \"%s\"\n\n", lexema.token);
            exit(ERRO_VALUE);
        }
        consome_token(CLOSE_KEY);
    }
}

void get_state_single()
{
    token_t lexema = consome_token(COMAND);


    if (!states)
    {
        states = (state_t*) malloc(sizeof(state_t));
        number_states=1;
    }
    else
    {
        states = (state_t*) realloc(states, sizeof(state_t)*(++number_states));
    }
    states[number_states-1].final_state = false;
    states[number_states-1].transition  = NULL;

    do{

        if ((strcmp(lexema.token, "id")==0)||(strcmp(lexema.token, "name")==0))
        {
            consome_token(EQUAL);
            if (strcmp(lexema.token, "id")==0)
                states[number_states-1].id = atoi(consome_token(STRNG).token);
            else
                consome_token(STRNG);
        }
        else
        {
            fprintf(stderr, "\n[ERRO:] Propriedade -> \"%s\" não existente para 'state'\n\n", lexema.token);
            exit(ERRO_VALUE);
        }

        lexema = consome_token(ALL);
    }while (lexema.type == COMAND);

    if (lexema.type != CLOSE_KEY)
    {
        fprintf(stderr, "\n[ERRO:] Comando não esperado -> \"%s\"\n\n", lexema.token);
        exit(ERRO_VALUE);
    }

    // get Propriedades

    while(1)
    {
        lexema = consome_token(ALL);
        if (lexema.type == OPEN_COMENT)
        {
          do{
              coment();
              lexema = consome_token(ALL);
          }while(lexema.type == OPEN_COMENT);
        }else if (lexema.type != OPEN_KEY)
        {
            fprintf(stderr, "\n[ERRO:] Comando não esperado -> \"%s\"\n\n", lexema.token);
            exit(ERRO_VALUE);
        }
        lexema = consome_token(ALL);
        if (lexema.type == COMAND)
        {
            if ((strcmp(lexema.token, "x")==0)||(strcmp(lexema.token, "y")==0))
            {
                consome_token(CLOSE_KEY);
                consome_token(NUMBER);
                consome_token(OPEN_KEY);
                consome_token(PCLOSE_KEY);
                consome_token(COMAND);
                consome_token(CLOSE_KEY);
            }else if ((strcmp(lexema.token, "initial")==0)||(strcmp(lexema.token, "final")==0))
            {
                if (strcmp(lexema.token, "initial")==0)
                    start_state = &(states[number_states-1]);
                else
                    states[number_states-1].final_state = true;
                consome_token(PCLOSE_KEY);
                consome_token(CLOSE_KEY);
            }
            else
            {
                fprintf(stderr, "\n[ERRO:] Comando não esperado -> \"%s\"\n\n", lexema.token);
                exit(ERRO_VALUE);
            }
        }
        else if (lexema.type == PCLOSE_KEY)
        {
            lexema = consome_token(COMAND);
            if (strcmp(lexema.token, "state")!=0)
            {
                fprintf(stderr, "\n[ERRO:] Fechamento incorreto de state, recebido -> \"%s\"\n\n", lexema.token);
                exit(ERRO_VALUE);
            }
            return;
        }
        else
        {
            fprintf(stderr, "\n[ERRO:] Comando -> \"%s\" não definido para state\n\n", lexema.token);
            exit(ERRO_VALUE);
        }
    }
}

void get_trans_sigle()
{
    consome_token(CLOSE_KEY);

    if (!transitions)
    {
        transitions = (transition_t*) malloc(sizeof(transition_t));
        number_transitions=1;
    }
    else
    {
        transitions = (transition_t*) realloc(transitions, sizeof(transition_t)*(++number_transitions));
    }
    transitions[number_transitions-1].next_transition = NULL;

    while(1)
    {
        token_t lexema = consome_token(ALL);
        if (lexema.type == OPEN_COMENT)
        {
          do{
              coment();
              lexema = consome_token(ALL);
          }while(lexema.type == OPEN_COMENT);
        }else if (lexema.type != OPEN_KEY)
        {
            fprintf(stderr, "\n[ERRO:] Comando não esperado -> \"%s\"\n\n", lexema.token);
            exit(ERRO_VALUE);
        }
        lexema = consome_token(ALL);
        if (lexema.type == COMAND)
        {
            if ((strcmp(lexema.token, "from")==0)||(strcmp(lexema.token, "to")==0))
            {
                consome_token(CLOSE_KEY);
                if (strcmp(lexema.token, "from")==0)
                {

                    // transitions[number_transitions-1].state_origin=get_state(atoi(consome_token(NUMBER).token));
                    transitions[number_transitions-1].id_origin=atoi(consome_token(NUMBER).token);
                }
                else
                {
                    // transitions[number_transitions-1].state_destiny=get_state(atoi(consome_token(NUMBER).token));
                    transitions[number_transitions-1].id_destiny=atoi(consome_token(NUMBER).token);
                }
                consome_token(OPEN_KEY);
                consome_token(PCLOSE_KEY);
                consome_token(COMAND);
                consome_token(CLOSE_KEY);
            }
            else if (strcmp(lexema.token, "read")==0)
            {
                lexema = consome_token(ALL);
                if (lexema.type == CLOSE_KEY)
                {
                    strcpy(transitions[number_transitions-1].simbol_transition, consome_token(COMAND).token);
                    consome_token(OPEN_KEY);
                    consome_token(PCLOSE_KEY);
                    consome_token(COMAND);
                    consome_token(CLOSE_KEY);
                }
                else if (lexema.type == PCLOSE_KEY)
                {
                    strcpy(transitions[number_transitions-1].simbol_transition, "\0");
                    consome_token(CLOSE_KEY);
                }
            }
            else
            {
                fprintf(stderr, "\n[ERRO:] Comando -> \"%s\" não definido para state\n\n", lexema.token);
                exit(ERRO_VALUE);
            }
        }
        else if (lexema.type == PCLOSE_KEY)
        {
            lexema = consome_token(COMAND);
            if (strcmp(lexema.token, "transition")!=0)
            {
                fprintf(stderr, "\n[ERRO:] Fechamento incorreto de state, recebido -> \"%s\"\n\n", lexema.token);
                exit(ERRO_VALUE);
            }
            return;
        }
        else
        {
            fprintf(stderr, "\n[ERRO:] Comando -> \"%s\" não definido para state\n\n", lexema.token);
            exit(ERRO_VALUE);
        }
    }
}

void stack()
{

}



// -----------------------------------------------------

void coment()
{
    token_t lexema;
    do{
        lexema = consome_token(ALL);
    }while(lexema.type != CLOSE_COMENT);
}

token_t consome_token(token_e requerido)
{
    token_t lexema = parser();

    if ((requerido != ALL) && (lexema.type != requerido))
        erro(requerido, lexema.type);

    return lexema;
}

void erro(token_e requerido, token_e lexema)
{
    fprintf(stderr, "\n[ERRO:] Era esperado o token \"%d\" mas foi recebido \"%d\"\n\n", requerido, lexema);
    exit(ERRO_TYPE);
}
