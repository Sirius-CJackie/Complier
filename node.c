//this file is some operations about node vector 
#include "compiler.h"
#include <assert.h>
#include "helpers/vector.h"

struct vector *node_vector = NULL;
struct vector *node_vector_root = NULL;
struct node* parser_current_body = NULL;
struct node* parser_current_function = NULL;

void node_set_vector(struct vector *vec, struct vector *root_vec)
{
    node_vector = vec;
    node_vector_root = root_vec;
}
//some base operations, which is samilar as the buffer
void node_push(struct node *node)
{
    vector_push(node_vector, &node);
}

struct node *node_peek_or_null()
{
    return vector_back_ptr_or_null(node_vector);
}
void make_body_node(struct vector *body_vec, size_t size, bool padded, struct node *largest_var_node)
{
    node_create(&(struct node){NODE_TYPE_BODY, .body.statements = body_vec, .body.size = size, .body.padded = padded, .body.largest_var_node = largest_var_node});
}
struct node *node_peek()
{   
    //because the vector_back_ptr_or_null() return value data type need convert
    return *(struct node**)(vector_back(node_vector));
}

struct node *node_pop()
{
    struct node *last_node = vector_back_ptr(node_vector);
    
    struct node *last_node_root = vector_empty(node_vector) ? NULL : vector_back_ptr_or_null(node_vector_root);

    vector_pop(node_vector);

    if (last_node == last_node_root)
    {
        // if this vector is root node,we can pop the root node vector 
        vector_pop(node_vector_root);
    }

    return last_node;
}
bool node_is_expressionable(struct node *node)
{   
    //this all allowance token type in the expression
    return node->type == NODE_TYPE_EXPRESSION || node->type == NODE_TYPE_EXPRESSION_PARENTHESIS || node->type == NODE_TYPE_UNARY || node->type == NODE_TYPE_IDENTIFIER || node->type == NODE_TYPE_NUMBER || node->type == NODE_TYPE_STRING;
}
//this peek is samilar the peek node, just this use to peek this token wether expression
struct node* node_peek_expressionable_or_null()
{
    struct node* last_node = node_peek_or_null();
    return node_is_expressionable(last_node) ? last_node : NULL;
}
//make a exp struct node to store a normal completed expression
void make_exp_node(struct node *node_left, struct node *node_right, const char *op)
{
    // Expressions must have left and right operands..
    assert(node_left);
    assert(node_right);
    node_create(&(struct node){NODE_TYPE_EXPRESSION, .exp.op = op, .exp.left = node_left, .exp.right = node_right});
}
//make bracket node
void make_bracket_node(struct node* node)
{
    node_create(&(struct node){.type = NODE_TYPE_BRACKET, .bracket.inner = node});
}
// node create function
struct node *node_create(struct node *_node)
{
    struct node *node = malloc(sizeof(struct node));
    memcpy(node, _node, sizeof(struct node));
    node->binded.owner = parser_current_body;
    node->binded.function = parser_current_function;
    node_push(node);
    return node;
}

bool node_is_struct_or_union_variable(struct node* node)
{
    if (node->type != NODE_TYPE_VARIABLE)
    {
        return false;
    }

    return datatype_is_struct_or_union(&node->var.type);
}
//return the node variable
struct node* variable_node(struct node* node)
{
    struct node* var_node = NULL;
    switch(node->type)
    {
        case NODE_TYPE_VARIABLE:
            var_node = node;
        break;

        case NODE_TYPE_STRUCT:
            var_node = node->_struct.var;
        break;

        case NODE_TYPE_UNION:
            //var_node = node->_union.var;
            assert(1 == 0 && "Unions are not yet supported\n");
        break;
    }

    return var_node;
}
//determine this node is primitive datatype node
bool variable_node_is_primitive(struct node* node)
{
    assert(node->type == NODE_TYPE_VARIABLE);
    return datatype_is_primitive(&node->var.type);
}

struct node* variable_node_or_list(struct node* node)
{
    if (node->type == NODE_TYPE_VARIABLE_LIST)
    {
        return node;
    }

    return variable_node(node);
}
void make_struct_node(const char* name, struct node* body_node)
{
    int flags = 0;
    if (!body_node)
    {
        flags |= NODE_FLAG_IS_FORWARD_DECLARATION;
    }

    node_create(&(struct node){.type=NODE_TYPE_STRUCT, ._struct.body_n=body_node,._struct.name=name,.flags=flags});
}
struct node* node_from_sym(struct symbol* sym)
{
    if (sym->type != SYMBOL_TYPE_NODE)
    {
        return NULL;
    }

    return sym->data;
}

struct node* node_from_symbol(struct compile_process* current_process, const char* name)
{
    struct symbol* sym = symresolver_get_symbol(current_process, name);
    if (!sym)
    {
        return NULL;
    }
    return node_from_sym(sym);
}


struct node* struct_node_for_name(struct compile_process* current_process, const char* name)
{
    struct node* node = node_from_symbol(current_process, name);
    if (!node)
        return NULL;

    if (node->type != NODE_TYPE_STRUCT)
        return NULL;

    return node;  
}
void make_function_node(struct datatype* ret_type, const char* name, struct vector* arguments, struct node* body_node)
{
    node_create(&(struct node){.type=NODE_TYPE_FUNCTION,.func.name=name,.func.args.vector=arguments,.func.body_n=body_node,.func.rtype=*ret_type,.func.args.stack_addition=DATA_SIZE_DDWORD});

}
size_t function_node_argument_stack_addition(struct node* node)
{
    assert(node->type == NODE_TYPE_FUNCTION);
    return node->func.args.stack_addition;
}
void make_exp_parentheses_node(struct node* exp_node)
{
    node_create(&(struct node){.type=NODE_TYPE_EXPRESSION_PARENTHESIS,.parenthesis.exp=exp_node});
}
bool node_is_expression_or_parentheses(struct node* node)
{
    return node->type == NODE_TYPE_EXPRESSION_PARENTHESIS || node->type == NODE_TYPE_EXPRESSION;
}

bool node_is_value_type(struct node* node)
{
    return node_is_expression_or_parentheses(node) || node->type == NODE_TYPE_IDENTIFIER || node->type == NODE_TYPE_NUMBER || node->type == NODE_TYPE_UNARY || node->type == NODE_TYPE_TENARY || node->type == NODE_TYPE_STRING;
}
void make_if_node(struct node* cond_node, struct node* body_node, struct node* next_node)
{
    node_create(&(struct node){.type=NODE_TYPE_STATEMENT_IF, .stmt.if_stmt.cond_node=cond_node,.stmt.if_stmt.body_node=body_node,.stmt.if_stmt.next=next_node});
}
void make_else_node(struct node* body_node)
{
    node_create(&(struct node){.type=NODE_TYPE_STATEMENT_ELSE, .stmt.else_stmt.body_node=body_node});
}
void make_return_node(struct node* exp_node)
{
    node_create(&(struct node){.type=NODE_TYPE_STATEMENT_RETURN,.stmt.return_stmt.exp=exp_node});
}
void make_for_node(struct node* init_node, struct node* cond_node, struct node* loop_node, struct node* body_node)
{
    node_create(&(struct node) {.type=NODE_TYPE_STATEMENT_FOR, .stmt.for_stmt.init_node=init_node, .stmt.for_stmt.cond_node=cond_node,.stmt.for_stmt.loop_node=loop_node,.stmt.for_stmt.body_node=body_node});
}
void make_while_node(struct node* exp_node, struct node* body_node)
{
    node_create(&(struct node){.type=NODE_TYPE_STATEMENT_WHILE, .stmt.while_stmt.exp_node=exp_node,.stmt.while_stmt.body_node=body_node});
}
void make_do_while_node(struct node* body_node, struct node* exp_node)
{
    node_create(&(struct node){.type=NODE_TYPE_STATEMENT_DO_WHILE,.stmt.do_while_stmt.body_node=body_node,.stmt.do_while_stmt.exp_node=exp_node});
}
void make_switch_node(struct node* exp_node, struct node* body_node, struct vector* cases, bool has_default_case)
{
    node_create(&(struct node){.type=NODE_TYPE_STATEMENT_SWITCH,.stmt.switch_stmt.exp=exp_node,.stmt.switch_stmt.body=body_node,.stmt.switch_stmt.cases=cases,.stmt.switch_stmt.has_default_case=has_default_case});
}
void make_continue_node()
{
    node_create(&(struct node){.type=NODE_TYPE_STATEMENT_CONTINUE});
}
void make_break_node()
{
    node_create(&(struct node){.type=NODE_TYPE_STATEMENT_BREAK});
}
void make_label_node(struct node* name_node)
{
    node_create(&(struct node){.type=NODE_TYPE_LABEL,.label.name=name_node});
}
void make_goto_node(struct node* label_node)
{
    node_create(&(struct node){.type=NODE_TYPE_STATEMENT_GOTO, .stmt._goto.label=label_node});
}
void make_case_node(struct node* exp_node)
{
    node_create(&(struct node){.type=NODE_TYPE_STATEMENT_CASE, .stmt._case.exp=exp_node});
}
void make_tenary_node(struct node* true_node, struct node* false_node)
{
    node_create(&(struct node){.type=NODE_TYPE_TENARY,.tenary.true_node=true_node,.tenary.false_node=false_node});
}
void make_cast_node(struct datatype* dtype, struct node* operand_node)
{
    node_create(&(struct node){.type=NODE_TYPE_CAST,.cast.dtype=*dtype, .cast.operand=operand_node});
}
