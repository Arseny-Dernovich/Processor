#include "My_features.h"

unsigned long Calculate_checksum (void* data , size_t size_in_bytes)
{
    my_assert (data == NULL);

#ifndef NDEBUG
    unsigned long checksum = 0;
    for (size_t i = 0; i < size_in_bytes; i++) {
        checksum += *((char*) data + i);
    }
    return checksum;
#else
    return 0;
#endif
}
//---------------------------------------------------------------------------------
unsigned long Stack_Calculate_Hashsum (Stack* stk)
{
    my_assert (stk == NULL);

#ifndef NDEBUG
    unsigned long original_checksum_stack = stk->checksum_stack;
    unsigned long original_checksum_arr = stk->checksum_arr;
    stk->checksum_stack = 0;
    stk->checksum_arr = 0;
    unsigned long checksum = Calculate_checksum(stk, sizeof(Stack));
    stk->checksum_stack = original_checksum_stack;
    stk->checksum_arr = original_checksum_arr;
    return checksum;
#else
    return 0;
#endif
}
//-------------------------------------------------------------------------------
Stack_error Calculate_Arr_Aligment (size_t capacity , Stack* stk)
{
    stk->length_bytes_aligment = 0;
#ifndef NDEBUG
    if ((capacity * sizeof (stack_element)) % 8 != 0) {
        stk->length_bytes_aligment = 8 - (capacity * sizeof (stack_element)) % 8;
    }
#endif

    return COMPLETE_VALUE;
}
//--------------------------------------------------------------------------------------------
int Calculate_Size_Arr_Bytes (size_t capacity , Stack* stk)
{
#ifndef NDEBUG
   return (capacity * sizeof (stack_element) +  2 * sizeof (canary_t) + stk->length_bytes_aligment);
#else
    return (capacity * sizeof (stack_element) + stk->length_bytes_aligment);
#endif
}
