#include "My_features.h"
// #define NDEBUG
Stack_error Stack_Error (Stack* stk)
{
    my_assert(stk == NULL);
#ifndef NDEBUG
    canary_t* left_data_canary = (canary_t*) stk->data - 1;
    canary_t* right_data_canary = (canary_t*) ((char*) (stk->data + stk->capacity) + stk->length_bytes_aligment);


    if (stk->checksum_stack != Stack_Calculate_Hashsum(stk)) {

        return BAD_HASH_STACK;
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
#endif

    return COMPLETE_VALUE;
}


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

//----------------------------------------------------------------------------------------------------------------------------------------
FILE* Open_Log_File (const char* filename_write)
{
    FILE* fp = fopen (filename_write , "w");

    if (fp == NULL) {

        printf ("Error: Oткрыть файл %s не удалось" , filename_write);
        perror ("EDA");

    }

    return fp;
}
//--------------------------------------------------------------------------------------------------------
Stack_error Stack_Dump (Stack* stk)
{
    my_assert (stk == NULL);
    FILE* fp = Open_Log_File ("Log_File.txt");
#ifndef NDEBUG
    canary_t* left_data_canary  = (canary_t*) stk->data - 1;
    canary_t* right_data_canary = (canary_t*) ((char*) (stk->data + stk->capacity) + stk->length_bytes_aligment);



    fprintf (fp , "\n\ncanary_1 = %lld , &canary_1 = %x\n\n" , stk->canary_1 , &stk->canary_1);

    fprintf (fp , "canary_2 = %lld , &canary_2 = %x\n\n" , stk->canary_2 , &stk->canary_2);

    if (Stack_Error (stk) == BAD_HASH_STACK) {

        printf ("Error: Содержимое структуры повреждено! Expected: %lu , Found: %lu\n" , stk->checksum_stack , Stack_Calculate_Hashsum (stk));
        fprintf (fp , "canary_3 = fail , &canary_3 = %x\n\n" , left_data_canary);
        fprintf (fp , "canary_4 = fail , &canary_4 = %x\n\n" , right_data_canary);
        fprintf (fp , "Массив данных повреждён , САМОЛИКВИДИРУЮСЬ :(((((((((((");
        assert (0);
    }

    else {

        fprintf (fp , "canary_3 = %lld , &canary_3 = %x\n\n" , *left_data_canary , left_data_canary);
        fprintf (fp , "canary_4 = %lld , &canary_4 = %x\n\n" , *right_data_canary , right_data_canary);
    }
#endif
    fprintf (fp , "size = %d\n\n" , stk->size);

    fprintf (fp , "capacity = %d\n\n" , stk->capacity);
#ifndef NDEBUG
    fprintf (fp , "Checksum_arr = %lu  Maybe_changed_checksum_arr = %lu\n\n" , stk->checksum_arr , Calculate_checksum (stk->data , stk->capacity * sizeof (stack_element)));

    fprintf (fp , "Checksum_stack = %lu Myabe_changed_checksum_stack = %lu\n\n" , stk->checksum_stack , Stack_Calculate_Hashsum(stk));
#endif
    fprintf (fp , "&data = %x\n\n" , stk->data);

    fprintf(fp, "[");

   for (int i = 0; i < stk->capacity; i++) {

        if (i < stk->size)

            fprintf(fp, " %lg ", stk->data[i]);

        else

            fprintf(fp, " _ ");
    }

    fprintf(fp, "]");

    fclose (fp);

    return COMPLETE_VALUE;
}

Stack_error Stack_Ctor (Stack* stk , size_t capacity)
{
#ifndef NDEBUG
    stk->canary_1 = val_canary_1;
    stk->canary_2 = val_canary_2;
#endif
    stk->size = 0;
#ifndef NDEBUG
    stk->checksum_arr = 0;
    stk->checksum_stack = 0;
#endif
    stk->capacity = capacity;

    Calculate_Arr_Aligment (capacity , stk);

    stack_element* data = (stack_element*) ((char*) calloc (1 , Calculate_Size_Arr_Bytes (capacity , stk)));
    my_assert(data == NULL);

#ifndef NDEBUG
    *((canary_t*) data) = val_canary_3;
    stk->data = (stack_element*) ((canary_t*) data + 1);
    *((canary_t*) ((char*) (stk->data + capacity) + stk->length_bytes_aligment)) = val_canary_4;
#else
    stk->data = data;
#endif

#ifndef NDEBUG
    stk->checksum_stack = Stack_Calculate_Hashsum (stk);
#endif

    STACK_ASSERT (stk);
    return COMPLETE_VALUE;
}
//---------------------------------------------------------------------
Stack_error Stack_Push (Stack* stk , stack_element value)
{
    my_assert (stk == NULL);
    STACK_ASSERT (stk);

    if (stk->size >= stk->capacity)

        Stack_Realloc_Up (stk);

    stk->data[stk->size++] = value;
#ifndef NDEBUG
    stk->checksum_arr = Calculate_checksum (stk->data , stk->capacity * sizeof (stack_element));
    stk->checksum_stack = Stack_Calculate_Hashsum (stk);
#endif
    STACK_ASSERT (stk);

    return COMPLETE_VALUE;
}
//---------------------------------------------------------------------------------------
stack_element Stack_Pop (Stack* stk)
{
    my_assert(stk == NULL);
    STACK_ASSERT(stk);

    if (stk->size == 0) {
        printf("Error: массив пустой! Size = %zu\n" , stk->size);
        assert (0);
    }

    stk->size--;

    stack_element popped_value = stk->data[stk->size];

    stk->data[stk->size] = 0;

    if (stk->size < 0.25 * stk->capacity)

        Stack_Realloc_Down (stk);

#ifndef NDEBUG
    stk->checksum_arr = Calculate_checksum (stk->data , stk->capacity * sizeof (stack_element));
    stk->checksum_stack = Stack_Calculate_Hashsum(stk);
#endif
    STACK_ASSERT(stk);


    // printf ("%lg\n" , popped_value);

    return popped_value;
}

Stack_error Stack_Realloc_Up (Stack* stk)
{
    Stack_Realloc (stk , FACTOR_INCR);
    return COMPLETE_VALUE;
}

Stack_error  Stack_Realloc_Down (Stack* stk)
{
    Stack_Realloc (stk , FACTOR_DECR);
    return COMPLETE_VALUE;
}


Stack_error Stack_Realloc (Stack* stk , double factor)
{
    my_assert (stk == NULL);

    int new_capacity = stk->capacity * factor;

    Calculate_Arr_Aligment (new_capacity , stk);
#ifndef NDEBUG
    stack_element* new_data = (stack_element*) realloc ((canary_t*) stk->data - 1 , Calculate_Size_Arr_Bytes (new_capacity , stk));
    my_assert (new_data == NULL);

    *((canary_t*) new_data) = val_canary_3;

    stk->data = (stack_element*) ((canary_t*) new_data + 1);
#else
    stack_element* new_data = (stack_element*) realloc (stk->data , Calculate_Size_Arr_Bytes (new_capacity , stk));
    my_assert (new_data == NULL);

    stk->data = new_data;
#endif


    if (factor > 1) {

        memset (stk->data + stk->capacity , 0 , (new_capacity - stk->capacity) * sizeof (stack_element));
    }
#ifndef NDEBUG
    *((canary_t*) ((char*) (stk->data + new_capacity) + stk->length_bytes_aligment)) = val_canary_4;
#endif
    stk->capacity = new_capacity;


    return COMPLETE_VALUE;
}
//-----------------------------------------------------------------------------------------------------------
Stack_error Stack_Dtor (Stack* stk)
{
    my_assert(stk == NULL);
#ifndef NDEBUG
    if (stk->data != NULL) {

        free ((canary_t*) stk->data - 1);
        stk->data = NULL;
    }
#else
    if (stk->data != NULL) {

        free (stk->data);
        stk->data = NULL;
    }
#endif
    return COMPLETE_VALUE;
}

