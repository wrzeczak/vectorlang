#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// make sure to include local headers AFTER this
#define MESSAGES_H_ERRORCODE 15
#define MESSAGES_H_GOTONAME __wvlang_main_error_exit
#include "messages.h"

#include "tokenizer.h"
#include "parser.h"

//------------------------------------------------------------------------------

int main(int argc, char ** argv) {
    // ARGUMENT HANDLING
    //------------------------------------------------------------------------------

    if(argc <= 1) {
        printf("WRZ: ERROR: Too few arguments (%d)!", argc);
        exit(MESSAGES_H_ERRORCODE); // not doing the whole goto rigamarole because we have so far done nothing needing cleanup
    } else if(argc > 2) {
        wprint("Discarding extra arguments (usage: ./wvcomp <input filename>).");
    }

    const char * input_filename = argv[1];
    FILE * input_fp = fopen(input_filename, "r");

    if(input_fp == NULL) {
        eprintf("fopen(\"%s\", \"r\") failed! errno = %d.", input_filename, errno);
    }

    // this scratch file will be used for tokenization
    FILE * scratch_fp = fopen(".wvcomp_temp", "w"); // this file is remove() at the end of the program

    token_split_wv_file(input_fp, scratch_fp);
    fclose(scratch_fp);
    scratch_fp = fopen(".wvcomp_temp", "r");

    struct build_tokens_ret_t lexer_tokens = build_tokens(scratch_fp);
    dprintf("Read %d tokens.", lexer_tokens.num_tokens_parsed);

    parse_lexer_tokens(lexer_tokens.tokens, lexer_tokens.num_tokens_parsed, lexer_tokens.num_lines);

    // EXITING and ERROR HANDLING if necessary
    //------------------------------------------------------------------------------
    __wvlang_main_error_exit:

    if(messages_h_goto_exit) wprint("Exiting abnormally at MESSAGES_H_GOTONAME!");

    if(input_fp != NULL) {
        dprintf("Closing \"%s\"...", input_filename);
        fclose(input_fp);
    }

    fclose(scratch_fp);
    // remove(".wvcomp_temp");

    destroy_tokens(lexer_tokens.tokens, lexer_tokens.num_tokens_parsed);

    if(messages_h_goto_exit) exit(MESSAGES_H_ERRORCODE);

    //----------------------------

    return 0;
}