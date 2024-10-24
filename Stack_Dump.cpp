#include "My_features.h"

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
