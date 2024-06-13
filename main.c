#include <stdio.h>
#include "compiler.h"

int main(){

    int res = compile_file("./test.c","./test",0);
    if (res == COMPILER_FILE_COMPILED_OK)
    {
        printf("compiler is success\n");

    }else if(res == COMPILER_FAIL_WITH_ERRORS)
    {
        printf("compiler is fail\n");
    }else{
        printf("unknow fault\n");
    }
    return 0;
}