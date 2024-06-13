/*this function implement the c process of complier the c project
like complier success and compiler fail and so on*/
#include "helpers/vector.h"
#include <stdio.h>
#include <compiler.h>
#include <stdlib.h>
struct compile_process* compile_process_create(const char* filename, const char* filename_out, int flags)
{
    FILE* file = fopen(filename,"r");
    if(!file){
        return NULL;
    }

    FILE* out_file = NULL;
    if(filename_out){

        out_file = fopen(filename_out,"w");
        if(!out_file){
            return NULL;
        }
    }

    struct  compile_process* process = calloc(1,sizeof(struct compile_process));
    process->node_vec = vector_create(sizeof(struct node*));
    process->node_tree_vec = vector_create(sizeof(struct node*));

    process->flags = flags;
    process->cfile.fp = file;
    process->ofile = out_file;
    symresolver_initialize(process);
    symresolver_new_table(process);

    return process;
 
}
//this process is uesd to getc the char , the return value is the line char 
char compile_process_next_char(struct lex_process* lex_process)
{
    struct compile_process* compiler = lex_process->compiler;
    char c = getc(compiler->cfile.fp);
    if(c == '\n')
    {
        compiler->pos.line +=1;
        compiler->pos.col = 1;
    } 
    return c;
}

//this function is get the peek char,why need the ungtec,becuse call the getc this char will output from stream
char compile_process_peek_char(struct lex_process* lex_process)
{
    struct compile_process* compiler = lex_process->compiler;
    char c = getc(compiler->cfile.fp);
    ungetc(c ,compiler->cfile.fp);
    return c;
}

void compile_process_push_char(struct lex_process* lex_process,char c)
{
    struct compile_process* compiler = lex_process->compiler;
    ungetc(c ,compiler->cfile.fp);
}