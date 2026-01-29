#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "messages.h"

//------------------------------------------------------------------------------
 
// Print a crazy really scary error message...
// also gotos to the messages_h error goto hehe
static void lexer_error(unsigned int line_number, int char_number, const char * function, int fn_line_number, const char * message, char * line);

//----------------------------

// Tokenizes the input_file and outputs the tokens (more specifically, the lexemes), separated by newlines, into scratch_file 
//
// An example program:
// vec1 = [ 10 .. 40 ]
// vec1 <- 50
// Becomes, in scratch_file:
// vec1
// =
// [
// 10
// ..
// 40
// ]
// __NEWLINE
// vec1
// ...
// returns the number of lexemes + newlines parsed
void token_split_wv_file(FILE * input_file, FILE * scratch_file);

static bool is_lower_alphabetical(char c);      // a .. z
static bool is_upper_alphabetical(char c);      // A .. Z
static bool is_numeric(char c);                 // 0 .. 9, -

static bool is_valid_starting_name_char(char c);
static bool is_valid_nonstarting_name_char(char c);

static bool is_equality_operator_char(char c);  // =, :
static bool is_xpend_operator_char(char c);     // <, >, -

//----------------------------

typedef enum {
    TOK_NUMBER = -3, // types that use the .value field are negative
    TOK_NAME,
    TOK_MACRO,       // == -1
    TOK_LPAREN = 1,  // types that are simply do not
    TOK_RPAREN,
    TOK_LSQUARE,
    TOK_RSQUARE,
    TOK_COMMA,
    TOK_RANGE,
    TOK_LAPPEND,
    TOK_RAPPEND,
    TOK_ASSIGN,
    TOK_EQUALS,
    TOK_LEQUALS,
    TOK_REQUALS,
    TOK_CEQUALS,
    TOK_ADD,
    TOK_SUBTRACT,
} TokenType;

typedef struct {
    unsigned int line;
    TokenType type;
    char * value;
    unsigned int value_len;
} LexerToken;

struct build_tokens_ret_t {
    LexerToken * tokens;
    unsigned int num_tokens_parsed;
    unsigned int num_lines;
};

struct build_tokens_ret_t build_tokens(FILE * scratch_fp);

//------------------------------------------------------------------------------

static void lexer_error(unsigned int line_number, int char_number, const char * function, int fn_line_number, const char * message, char * line) {
    #define PRINT_RED "\x1b[31m"
    #define STOP_PRINTING_IN_RED_PLEASE "\x1b[0m"
    printf(PRINT_RED "\n\n\nBOOOO!!!!!! BOO BOO BOO!!!!! BOOO!!!!!!!! BOOOOOOO!!!!!!!!!\n\n\n" STOP_PRINTING_IN_RED_PLEASE "WRZ: LEXER ERROR: In %s(), source line %d, column %d:\t" PRINT_RED "%s\n" STOP_PRINTING_IN_RED_PLEASE "----- The offending line lies below... -----\n%s\n\n", function, line_number, char_number, message, line);

    wprintf("This is a c-c-catastrophic failure in tokenizer.h:%d, and b-because it is in a d-different file, the error handling performed by eprintf() (a g-g-.. oh I can't say it... a g-g-goto...) will not work. The program will now exit, scarily,... sp-pookily... AAUGH!!", fn_line_number);
    exit(80085);
}

void token_split_wv_file(FILE * input_file, FILE * scratch_file) {
    // set up a line buffer
    char line_buffer[1024];
    memset(line_buffer, 0, 1024);
    unsigned int line_number = 0;

    // begin lexing...
    char name_buffer[256];
    memset(name_buffer, 0, 256);
    unsigned int name_buffer_offset = 0;
    bool building_name = false;

    char macro_name_buffer[256];
    memset(macro_name_buffer, 0, 256);
    unsigned int macro_name_buffer_offset = 0;
    bool building_macro_name = false;

    char vector_buffer[256];
    memset(vector_buffer, 0, 256);
    unsigned int vector_buffer_offset = 0;
    bool building_vector = false;

    while(fgets(line_buffer, 1024, input_file)) {
        int scratch_file_pos = ftell(scratch_file);
        int line_length = strlen(line_buffer);
        line_number++;

        for(int i = 0; i < line_length; i++) {
            char rc = line_buffer[i];

            // skip spaces
            if(rc == ' ') continue;
            
            // build comments
            if(rc == '/') {
                if(line_buffer[i + 1] == '/') {
                    break; // skip the rest of this line
                }
            }

            // build names
            if(is_valid_starting_name_char(rc)) {
                // dprintf("valid starting char: %c", rc);
                if(!building_name) building_name = true;
                name_buffer_offset++;
                name_buffer[name_buffer_offset - 1] = rc;
                // continue;
            } else if(is_valid_nonstarting_name_char(rc) && building_name) {
                // dprintf("valid nonstarting char: %c", rc);
                name_buffer_offset++;
                name_buffer[name_buffer_offset - 1] = rc;
                // continue;

                char nrc = line_buffer[i + 1];
                if(!is_valid_nonstarting_name_char(nrc)) {
                    // stop building the name
                    dprintf("built name \"%s\", %d characters long", name_buffer, name_buffer_offset);
                    fprintf(scratch_file, "%s\n", name_buffer);

                    building_name = false;
                    memset(name_buffer, 0, 256);
                    name_buffer_offset = 0;
                    continue;
                }
            }

            // since macros are here we can do parentheses here
            if(rc == ')') {
                dprint("rparen");
                fprintf(scratch_file, ")\n");
                continue;
            }

            if(rc == '(') {
                dprint("lparen");
                fprintf(scratch_file, "(\n");
                continue;
            }

            // build macro names
            if(is_upper_alphabetical(rc) || ((rc == '_') && building_macro_name)) { // A..Z + _ if not at beginning
                if(!building_macro_name) building_macro_name = true;
                macro_name_buffer_offset++;
                macro_name_buffer[macro_name_buffer_offset - 1] = rc;
                // continue;

                char nrc = line_buffer[i + 1];
                if(!is_upper_alphabetical(nrc) && (nrc != '_')) { // i probably don't need to be so verbose but it helps clarity i think
                    dprintf("built macro name \"%s\", %d characters long", macro_name_buffer, macro_name_buffer_offset);
                    fprintf(scratch_file, "%s\n", macro_name_buffer);

                    building_macro_name = false;
                    memset(macro_name_buffer, 0, 256);
                    macro_name_buffer_offset = 0;
                    continue;
                }
            }

            // build equality operators: =, ==, :==, ==:, :=:
            if(is_equality_operator_char(rc)) {
                // so we could check for this here; but this first stage is just loading symbols into the scratch file
                // technically, we aren't really lexing, just moving (valid) symbols to an IR
                // so if a valid symbol is found, it should end up in scratch, even if it is out place and easily detectable
                // if(last_built_symbol == 1) { (i was checking to see if it succeeded a variable name)
                if(!is_equality_operator_char(line_buffer[i + 1])) {
                    // =, assignment
                    dprint("assignment");
                    fprintf(scratch_file, "=\n");
                    // continue;
                } 
                else if(strncmp(line_buffer + (unsigned char) i, "==", 2) == 0) {
                    dprint("equality");
                    fprintf(scratch_file, "==\n");
                    i++;
                    continue;
                } else if(strncmp(line_buffer + (unsigned char) i, ":==", 3) == 0) {
                    dprint("left-equality");
                    fprintf(scratch_file, ":==\n");
                    i += 2; // skip the next two characters; otherwise, they will be read as an equality and an assignment
                    continue;
                } else if(strncmp(line_buffer + (unsigned char) i, "==:", 3) == 0) { 
                    dprint("right-equality");
                    fprintf(scratch_file, "==:\n");
                    i += 2;
                    continue;
                } else if(strncmp(line_buffer + (unsigned char) i, ":=:", 3) == 0) {
                    dprint("cheek-equality");
                    fprintf(scratch_file, ":=:\n");
                    i += 2;
                    continue;
                } else {
                    dprintf("equals sign found without correct operator matching: line %d, col %d", line_number, i + 1);
                    lexer_error(line_number, i + 1, "token_split_wv_file", __LINE__, "An invalid equality-like operator (probably :=, instead of ==:/:==) was found!", line_buffer);
                }
            }
            
            // since subtract is here, let's put + here too
            if(rc == '+') {
                dprint("addition");
                fprintf(scratch_file, "+\n");
                continue;
            }

            // build append/prepend/subtract operators: <-, ->, -
            if(is_xpend_operator_char(rc)) {
                if(!is_xpend_operator_char(line_buffer[i + 1]) && !((line_buffer[i + 1] >= '0') && (line_buffer[i + 1] <= '9'))) {
                    // subtract
                    dprintf("I promise this is not a negative number; line_buffer[i + 1] = %c!", line_buffer[i + 1]);
                    dprint("subtract");
                    fprintf(scratch_file, "-\n");
                    continue;
                } else if(strncmp(line_buffer + (unsigned char) i, "->", 2) == 0) {
                    dprint("prepend");
                    fprintf(scratch_file, "->\n");
                    i++; // skip the next char ('-') so it doesn't get read as subtract
                    continue;
                } else if(strncmp(line_buffer + (unsigned char) i, "<-", 2) == 0) {
                    dprint("append");
                    fprintf(scratch_file, "<-\n");
                    i++;
                    continue;
                } else if(!((line_buffer[i + 1] >= '0') && (line_buffer[i + 1] <= '9'))) { // this in case of negative numbers
                    dprintf("xpend sign found without correct operator matching: line %d, col %d", line_number, i + 1);
                    lexer_error(line_number, i + 1, "token_split_wv_file", __LINE__, "An invalid append/prepend-like operator was found (probably --)!", line_buffer);
                }
            }
            
            // since vectors are here, do square brackets
            if(rc == '[') {
                dprint("lsquare");
                fprintf(scratch_file, "[\n");
                continue;
            }

            if(rc == ']') {
                dprint("rsquare");
                fprintf(scratch_file, "]\n");
                continue;
            }

            if(rc == ',') {
                dprint("comma");
                fprintf(scratch_file, ",\n");
                continue;
            }

            if(rc == '.') {
                if(strncmp(line_buffer + (unsigned char) i, "..", 2) == 0) {
                    dprint("range");
                    fprintf(scratch_file, "..\n");
                    i++;
                    continue;
                } else {
                    // .x is currently wrong, more to come here probably
                    lexer_error(line_number, i + 1, "token_split_wv_file", __LINE__, "A stray '.' was found (probably at attempt at floating point)! Get your ass back to the IEEE!!", line_buffer);
                }
            }

            // build vectors/numbers
            if(is_numeric(rc)) {
                if(!building_vector) building_vector = true;
                vector_buffer_offset++;
                vector_buffer[vector_buffer_offset - 1] = rc;

                char nrc = line_buffer[i + 1];
                if(!((nrc >= '0') && (nrc <= '9')) && building_vector) {
                    dprintf("built number %s", vector_buffer);
                    fprintf(scratch_file, "%s\n", vector_buffer);

                    building_vector = false;
                    memset(vector_buffer, 0, 256);
                    vector_buffer_offset = 0;
                }
            }
        }

        // line has ended, print __NEWLINE
        // but only if there is not already newline there
        int new_scratch_file_pos = ftell(scratch_file);
        if(new_scratch_file_pos > scratch_file_pos) fprintf(scratch_file, "__NEWLINE\n");
    }
}

static inline bool is_lower_alphabetical(char c) {
    return ((c >= 'a') && (c <= 'z'));
}

static inline bool is_upper_alphabetical(char c) {
    return ((c >= 'A') && (c <= 'Z'));
}

static inline bool is_numeric(char c) {
    return ((c >= '0') && (c <= '9')) || (c == '-');
}

static bool is_valid_starting_name_char(char c) {
    return is_lower_alphabetical(c) || (c == '_');
}

static bool is_valid_nonstarting_name_char(char c) {
    return is_valid_starting_name_char(c) || is_numeric(c); // a..z, 0..9, '-', '_'
}

static bool is_equality_operator_char(char c) {
    return (c == '=') || (c == ':');
}

static bool is_xpend_operator_char(char c) {
    return (c == '-') || (c == '<') || (c == '>');
}

struct build_tokens_ret_t build_tokens(FILE * scratch_fp) {
    unsigned int line_number = 1;
    char line_buffer[256];
    memset(line_buffer, 0, 256);

    unsigned int num_tokens_parsed = 0;
    LexerToken * tokens = malloc(num_tokens_parsed * sizeof(LexerToken));

    while(fgets(line_buffer, 256, scratch_fp)) {
        if(strncmp(line_buffer, "__NEWLINE\n", strlen("__NEWLINE\n")) == 0) {
            // dprintf("Newline found; this makes line number %d", line_number + 1);
            line_number++;
            continue;
        }

        // simple operator matching
        TokenType simple_match = 0;

        #define SMT(str, tok_t) else if(strncmp(line_buffer, str, strlen(str)) == 0) simple_match = tok_t

        if(strncmp(line_buffer, "(", strlen("(")) == 0) simple_match = TOK_LPAREN;
        SMT(")", TOK_RPAREN);
        SMT("[", TOK_LSQUARE);
        SMT("]", TOK_RSQUARE);
        SMT(",", TOK_COMMA);
        SMT("..", TOK_RANGE);
        SMT("->", TOK_LAPPEND);
        SMT("<-", TOK_RAPPEND);
        SMT("=", TOK_ASSIGN);
        SMT("==", TOK_EQUALS);
        SMT(":==", TOK_LEQUALS);
        SMT("==:", TOK_REQUALS);
        SMT(":=:", TOK_CEQUALS);
        SMT("+", TOK_ADD);
        // SMT("-", TOK_SUBTRACT);
        else if(strncmp(line_buffer, "-", strlen("-")) == 0 && !((line_buffer[1] >= '0') && (line_buffer[1] <= '9'))) {
            dprintf("I promise this is not a negative number: line_buffer[1] is %c!", line_buffer[1]);
            simple_match = TOK_SUBTRACT;
        }

        #undef SMT

        if(simple_match != 0) {
            LexerToken tok = (LexerToken) { .line = line_number, .type = simple_match, .value = NULL, .value_len = 0 };
            num_tokens_parsed++;
            tokens = realloc(tokens, num_tokens_parsed * sizeof(LexerToken));
            tokens[num_tokens_parsed - 1] = tok;
            dprintf("Token no. %d is %s.", num_tokens_parsed, line_buffer);
        } else {
            // otherwise, it's either a name, a macro, or a vector
            TokenType complex_type = 0;
            if(is_lower_alphabetical(line_buffer[0])) {
                complex_type = TOK_NAME;
            } else if(is_upper_alphabetical(line_buffer[0])) {
                complex_type = TOK_MACRO;
            } else if(is_numeric(line_buffer[0])) {
                complex_type = TOK_NUMBER;
            }

            char * value = malloc(strlen(line_buffer)); // intentionally dropping the +1...
            memset(value, 0, strlen(line_buffer));
            strncpy(value, line_buffer, strlen(line_buffer) - 1); // to chop off the \n character

            LexerToken tok = (LexerToken) { .line = line_number, .type = complex_type, .value = value, .value_len = strlen(value) };
            num_tokens_parsed++;
            tokens = realloc(tokens, num_tokens_parsed * sizeof(LexerToken));
            tokens[num_tokens_parsed - 1] = tok;

            if(complex_type == TOK_MACRO) dprintf("Token no. %d is (macro) %s.", num_tokens_parsed, value);
            if(complex_type == TOK_NAME) dprintf("Token no. %d is (name) %s.", num_tokens_parsed, value);
            else dprintf("Token no. %d is (number) %s.", num_tokens_parsed, value);
            
            if(complex_type == 0) {
                lexer_error(line_number, 0, "build_tokens", __LINE__, "Found a token that did not match any of the types defined in the TokenType enum (probably an error in token_split_wv_file()?)!", line_buffer);
            }
        }
    }

    return (struct build_tokens_ret_t) { .num_tokens_parsed = num_tokens_parsed, .tokens = tokens, .num_lines = line_number - 1 };
}

// complex types malloc() their value; these need to be freed
void destroy_tokens(LexerToken * tokens, unsigned int token_count) {
    for(unsigned int i = 0; i < token_count; i++) {
        LexerToken tok = tokens[i];

        if(tok.type < 0) {
            free(tok.value);
        }
    }
    free(tokens);
}