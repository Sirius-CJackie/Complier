#include "compiler.h"
#include <stdarg.h>
#include <stdlib.h>
//initializes lex function
struct lex_process_functions compiler_lex_functions ={
    .next_char=compile_process_next_char,
    .peek_char=compile_process_peek_char,
    .push_char=compile_process_push_char
};

void compiler_error(struct compile_process* compiler, const char* msg,...)
{
    va_list args;
    va_start(args,msg);
    vfprintf(stderr,msg,args);
    va_end(args);

    fprintf(stderr," on line %i , col %i in the file %s \n", compiler->pos.line,compiler->pos.col,compiler->pos.filename);
    exit(-1);
}

void compiler_warning(struct compile_process* compiler, const char* msg,...)
{
    va_list args;
    va_start(args,msg);
    vfprintf(stderr,msg,args);
    va_end(args);

    fprintf(stderr," on line %i , col %i in the file %s \n", compiler->pos.line,compiler->pos.col,compiler->pos.filename);
    //exit(-1);
}


int compile_file(const char* filename, const char* out_filename, int flags)
{   
    struct compile_process* process = compile_process_create(filename, out_filename, flags);
    if(!process){
        return COMPILER_FAIL_WITH_ERRORS;
    }

    //preform the lexical analysis
    struct lex_process* lex_process = lex_process_create(process,&compiler_lex_functions,NULL);
    if(!lex_process)
    {
        return COMPILER_FAIL_WITH_ERRORS;
    }

    if(lex(lex_process) != LEXICAL_ANALYSIS_ALL_OK)
    {
        return COMPILER_FAIL_WITH_ERRORS;
    }
    // assgin the lexer vector whioh store the complete tokens to the compiler process,in order to the next step handle
    process->token_vec = lex_process->token_vec;
    
    //perform the parser
    if (parse(process) != PARSE_ALL_OK)
    {
        return COMPILER_FAIL_WITH_ERRORS;
    }
    return COMPILER_FILE_COMPILED_OK;
}