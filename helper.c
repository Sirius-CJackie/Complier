// Implement data alignment helper tools
#include "compiler.h"
#include "helpers/vector.h"
#include <assert.h>
/**
 * @brief Gets the variable size from the given variable node
 * 
 * @param var_node 
 * @return size_t 
 */
size_t variable_size(struct node* var_node)
{
    assert(var_node->type == NODE_TYPE_VARIABLE);
    return datatype_size(&var_node->var.type);
}
/**
 * @brief Sums the variable size of all variable nodes inside the variable list node
 * Returns the result
 * 
 * @param var_list_node 
 * @return size_t The sum of all variable node sizes in the list.
 */
size_t variable_size_for_list(struct node* var_list_node)
{
    assert(var_list_node->type == NODE_TYPE_VARIABLE_LIST);
    size_t size = 0;
    vector_set_peek_pointer(var_list_node->var_list.list, 0);
    struct node* var_node = vector_peek_ptr(var_list_node->var_list.list);
    while(var_node)
    {
        size += variable_size(var_node);
        var_node = vector_peek_ptr(var_list_node->var_list.list);
    }

    return size;
}
// to it is a padding size, val is this body size ,such as val=21 tp=4 return=3,so we need add 3 paddings to this body 
int padding(int val, int to)
{
    if (to <= 0)
    {
        return 0;
    }

    if ((val % to) == 0)
    {
        return 0;
    }

    return to - (val % to) % to;
}
//after padding the body size
int align_value(int val, int to)
{
    if (val % to)
    {
        val += padding(val, to);
    }
    return val;
}
//If the input val is negative, we can convert it to a positive number
int align_value_treat_positive(int val, int to)
{
    assert(to >= 0);
    if (val < 0)
    {
        to = -to;
    }
    return align_value(val, to);
}
//If there are multiple padding, sum the padding
int compute_sum_padding(struct vector* vec)
{
    int padding = 0;
    int last_type = -1;
    bool mixed_types = false;
    vector_set_peek_pointer(vec, 0);
    struct node* cur_node = vector_peek_ptr(vec);
    struct node* last_node = NULL;
    while(cur_node)
    {
        if (cur_node->type != NODE_TYPE_VARIABLE)
        {
            cur_node = vector_peek_ptr(vec);
            continue;
        }

        padding += cur_node->var.padding;
        last_type = cur_node->var.type.type;
        last_node = cur_node;
        cur_node = vector_peek_ptr(vec);
    }

    return padding;

}
struct node* variable_struct_or_union_body_node(struct node* node)
{
    if (!node_is_struct_or_union_variable(node))
    {
        return NULL;
    }

    if (node->var.type.type == DATA_TYPE_STRUCT)
    {
        return node->var.type.struct_node->_struct.body_n;
    }

    // return the union body.
    #warning "Remember to implement unions"
    printf("NO UNION NODES ARE YET IMPLEMENTED\n");
    exit(1);
}