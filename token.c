#include "compiler.h"
#define PRIMITIVE_TYPES_TOTAL 7
const char* primitive_types[PRIMITIVE_TYPES_TOTAL] = {
    "void", "char", "short", "int", "long", "float", "double"
};

// determine the string is keyword?
bool token_is_keyword(struct token* token, const char* value)
{ 
    return token && token->type == TOKEN_TYPE_KEYWORD && S_EQ(token->sval,value);
}
bool token_is_symbol(struct token* token, char c)
{
    return token && token->type == TOKEN_TYPE_SYMBOL && token->cval == c;
}
//determine token is newline or comment or ..
bool token_is_nl_or_comment_or_newline_seperator(struct token* token)
{   
    if(!token)
        return false;
    return token->type == TOKEN_TYPE_NEWLINE ||
            token->type == TOKEN_TYPE_COMMENT ||
            token_is_symbol(token,"\\");
}
//determine datatype is primitive keyword
bool token_is_primitive_keyword(struct token* token)
{   
    if(!token)
        return false;
    if (token->type != TOKEN_TYPE_KEYWORD)
        return false;
    
    for (int i = 0; i < PRIMITIVE_TYPES_TOTAL; i++)
    {
        if (S_EQ(primitive_types[i], token->sval))
            return true;
    }

    return false;
}

bool token_is_operator(struct token *token, const char *op)
{
    return token && token->type == TOKEN_TYPE_OPERATOR && S_EQ(token->sval, op);
}
bool token_is_identifier(struct token* token)
{
    return token && token->type == TOKEN_TYPE_IDENTIFIER;
}
