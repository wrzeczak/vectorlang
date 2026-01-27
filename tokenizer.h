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
int token_split_wv_file(FILE * input_file, FILE * scratch_file);

static bool is_lower_alphabetical(char c);      // a .. z
static bool is_upper_alphabetical(char c);      // A .. Z
static bool is_numeric(char c);                 // 0 .. 9, -

static bool is_valid_starting_name_char(char c);
static bool is_valid_nonstarting_name_char(char c);

static bool is_equality_operator_char(char c);  // =, :
static bool is_xpend_operator_char(char c);     // <, >, -

//------------------------------------------------------------------------------

static void lexer_error(unsigned int line_number, int char_number, const char * function, int fn_line_number, const char * message, char * line) {
    #define PRINT_RED "\x1b[31m"
    #define STOP_PRINTING_IN_RED_PLEASE "\x1b[0m"
    printf(PRINT_RED "\n\n\nBOOOO!!!!!! BOO BOO BOO!!!!! BOOO!!!!!!!! BOOOOOOO!!!!!!!!!\n\n\n" STOP_PRINTING_IN_RED_PLEASE "WRZ: LEXER ERROR: In %s(), source line %d, column %d:\t" PRINT_RED "%s\n" STOP_PRINTING_IN_RED_PLEASE "----- The offending line lies below... -----\n%s\n\n", function, line_number, char_number, message, line);

    wprintf("This is a c-c-catastrophic failure in tokenizer.h:%d, and b-because it is in a d-different file, the error handling performed by eprintf() (a g-g-.. oh I can't say it... a g-g-goto...) will not work. The program will now exit, scarily,... sp-pookily... AAUGH!!", fn_line_number);
    exit(80085);
}

int token_split_wv_file(FILE * input_file, FILE * scratch_file) {
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
                if(!is_xpend_operator_char(line_buffer[i + 1])) {
                    // subtract
                    dprint("subtract");
                    fprintf(scratch_file, "-\n");
                    // continue;
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
                } else {
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

    return -1;
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