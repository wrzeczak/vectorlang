#pragma once
#include "tokenizer.h"

//------------------------------------------------------------------------------

typedef struct
char ** known_names;
unsigned int num_known_names = 0;

//------------------------------------------------------------------------------

void parse_lexer_tokens(LexerToken * tokens, unsigned int num_tokens, unsigned int num_lines) {
    unsigned int line_offset = 0;
    for(unsigned int l = 0; l < num_lines; l++) {
        unsigned int num_line_tokens = 0;

        // count the number of tokens on this line
        while(tokens[line_offset].line - 1 == l) {
            num_line_tokens++;
            line_offset++;
        }

        dprintf("There are %d tokens on this line [%d, %d)", num_line_tokens, line_offset - num_line_tokens, line_offset);

        LexerToken statement[num_line_tokens];
        memset(statement, 0, num_line_tokens * sizeof(LexerToken));

        // construct a statement
        for(int i = (line_offset - num_line_tokens); i < line_offset; i++) {
            statement[i - (line_offset - num_line_tokens)] = tokens[i];
        }

        // now, iterate over a statement
        
    }
}