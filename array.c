#include "compiler.h"
#include "helpers/vector.h"

#include <assert.h>
//create the array brackets
struct array_brackets* array_brackets_new()
{
    struct array_brackets* brackets = calloc(sizeof(struct array_brackets), 1);
    brackets->n_brackets = vector_create(sizeof(struct node*));
    return brackets;
}

void array_brackets_free(struct array_brackets* brackets)
{
    free(brackets);
}
// add the barcket to brackets ,which mean add the array diamension
void array_brackets_add(struct array_brackets* brackets, struct node* bracket_node)
{
    assert(bracket_node->type == NODE_TYPE_BRACKET);
    vector_push(brackets->n_brackets, &bracket_node);
}

struct vector* array_brackets_node_vector(struct array_brackets* brackets)
{
    return brackets->n_brackets;
}


size_t array_brackets_calculate_size_from_index(struct datatype* type, struct array_brackets* brackets, int index)
{
    
    struct vector* array_vec = array_brackets_node_vector(brackets);
    //total size
    size_t sum = type->size;
    if (index >= vector_count(array_vec))
    {   
        //like the situation a[1]=1, which is not the array just a int value,so return is size of int
        return sum;
    }
    
    vector_set_peek_pointer(array_vec, index);
    struct node* array_bracket_node = vector_peek_ptr(array_vec);
    if (!array_bracket_node)
        return 0;
    
    while(array_bracket_node)
    {
        /**
         * this is very easily understand, like int i[10][2]
         * the size of int i[10][2] = (sizeof(int))*10*2 = 4*10*2
        */
        assert(array_bracket_node->bracket.inner->type = NODE_TYPE_NUMBER);
        int number = array_bracket_node->bracket.inner->llnum;

        sum *= number;
        array_bracket_node = vector_peek_ptr(array_vec);
    }

    return sum;
}

size_t array_brackets_calculate_size(struct datatype* type, struct array_brackets* brackets)
{
    return array_brackets_calculate_size_from_index(type, brackets, 0);
}
//return this arry the number of indexes
int array_total_indexes(struct datatype* dtype)
{
    assert (dtype->flags & DATATYPE_FLAG_IS_ARRAY);
    struct array_brackets* brackets = dtype->array.brackets;
    return vector_count(brackets->n_brackets);
}