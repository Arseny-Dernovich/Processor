#include "My_features.h"

int Stack_Error (Stack* stk)
{
    my_assert(stk == NULL);

    canary_t* left_data_canary = (canary_t*) stk->data - 1;
    canary_t* right_data_canary = (canary_t*) ((char*) (stk->data + stk->capacity) + stk->length_bytes_aligment);


    if (stk->checksum_stack != Stack_Calculate_Hashsum(stk)) {

        printf ("Error: Содержимое структуры повреждено! Expected: %lu , Found: %lu\n" , stk->checksum_stack , Stack_Calculate_Hashsum(stk));
        abort();
    }

    if (stk->checksum_arr != Calculate_checksum (stk->data , stk->capacity * sizeof (stack_element))) {

        printf ("Error: Содержимое массива данных повреждено! Expected: %lu , Found: %lu\n" , stk->checksum_arr , Calculate_checksum (stk->data , stk->capacity * sizeof (stack_element)));
        return BAD_HASH_ARR;
    }

    if (stk->canary_1 != val_canary_1) {

        printf("Error: Stack canary_1 повреждено! Expected: %lld , Found: %lld\n" , val_canary_1 , stk->canary_1);
        return BAD_CANARY_1;
    }

    if (stk->canary_2 != val_canary_2) {

        printf("Error: Stack canary_2 повреждено! Expected: %lld , Found: %lld\n" , val_canary_2 , stk->canary_2);
        return BAD_CANARY_2;
    }

    if (*left_data_canary != val_canary_3) {

        printf("Error: Left data canary повреждено! Expected: %lld , Found: %lld\n" , val_canary_3 , *left_data_canary);
        return BAD_CANARY_2;
    }

    if (*right_data_canary != val_canary_4) {

        printf("Error: Right data canary повреждено! Expected: %lld , Found: %lld\n" , val_canary_4 , *right_data_canary);
        return BAD_CANARY_4;
    }



    return COMPLETE_VALUE;
}
