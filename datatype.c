#include "compiler.h"
bool datatype_is_struct_or_union(struct datatype* dtype)
{
    return dtype->type == DATA_TYPE_EXPECT_STRUCT || dtype->type == DATA_TYPE_UNION;
}
//determine this datatype is struct or union or not
bool datatype_is_struct_or_union_for_name(const char* name)
{
    return S_EQ(name,"union") || S_EQ(name,"struct");
}

size_t datatype_size_for_array_access(struct datatype* dtype)
{
    if (datatype_is_struct_or_union(dtype) && dtype->flags & DATATYPE_FLAG_IS_POINTER && 
        dtype->pointer_depth == 1)
    {
        // struct abc* abc; abc[0];
        return dtype->size;
    }

    return datatype_size(dtype);
}
//return the element size ,such as we enter a char a[],return sizeof(char)=1
size_t datatype_element_size(struct datatype* dtype)
{
    if (dtype->flags & DATATYPE_FLAG_IS_POINTER)
    {
        return DATA_SIZE_DWORD;
    }

    return dtype->size;
}
//ingron the pointer return the datatype size 
//such as int *a
//retrun is sizeof(a)
size_t datatype_size_no_ptr(struct datatype* dtype)
{
    if (dtype->flags & DATATYPE_FLAG_IS_ARRAY)
    {
        return dtype->array.size;
    }

    return dtype->size;
}

size_t datatype_size(struct datatype* dtype)
{   
    //handle the pointer datatype size
    if (dtype->flags & DATATYPE_FLAG_IS_POINTER && dtype->pointer_depth > 0)
    {
        return DATA_SIZE_DWORD;
    }
    //handle the array datatype size
    if (dtype->flags & DATATYPE_FLAG_IS_ARRAY)
    {
        return dtype->array.size;
    }

    return dtype->size;
}
//exclude the struct and union, all the datatype we can handle as primitive 
bool datatype_is_primitive(struct datatype* dtype)
{
    return !datatype_is_struct_or_union(dtype);
}