#include "My_features.h"
#include "Processor.h"
//#include "TXLib.h"
#define NUM_REGISTERS 5
#define SIGNATURE "MYCPU"
#define MAX_LINE_LENGTH 256


Stack_error Stack_Error (Stack* stk)
{
    my_assert(stk == NULL);
#ifndef NDEBUG
    canary_t* left_data_canary = (canary_t*) stk->data - 1;
    canary_t* right_data_canary = (canary_t*) ((char*) (stk->data + stk->capacity) + stk->length_bytes_aligment);


    if (stk->checksum_stack != Stack_Calculate_Hashsum(stk)) {

        return BAD_HASH_STACK;
    }

    if (stk->checksum_arr != Calculate_checksum (stk->data  , stk->capacity * sizeof (stack_element))) {

        printf ("Error: Содержимое массива данных повреждено! Expected: %lu  , Found: %lu\n"  , stk->checksum_arr  , Calculate_checksum (stk->data  , stk->capacity * sizeof (stack_element)));
        return BAD_HASH_ARR;
    }

    if (stk->canary_1 != val_canary_1) {

        printf("Error: Stack canary_1 повреждено! Expected: %lld  , Found: %lld\n"  , val_canary_1  , stk->canary_1);
        return BAD_CANARY_1;
    }

    if (stk->canary_2 != val_canary_2) {

        printf("Error: Stack canary_2 повреждено! Expected: %lld  , Found: %lld\n"  , val_canary_2  , stk->canary_2);
        return BAD_CANARY_2;
    }

    if (*left_data_canary != val_canary_3) {

        printf("Error: Left data canary повреждено! Expected: %lld  , Found: %lld\n"  , val_canary_3  , *left_data_canary);
        return BAD_CANARY_2;
    }

    if (*right_data_canary != val_canary_4) {

        printf("Error: Right data canary повреждено! Expected: %lld  , Found: %lld\n"  , val_canary_4  , *right_data_canary);
        return BAD_CANARY_4;
    }
#endif

    return COMPLETE_VALUE;
}


unsigned long Calculate_checksum (void* data  , size_t size_in_bytes)
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
    unsigned long checksum = Calculate_checksum(stk , sizeof(Stack));
    stk->checksum_stack = original_checksum_stack;
    stk->checksum_arr = original_checksum_arr;
    return checksum;
#else
    return 0;
#endif
}
//-------------------------------------------------------------------------------
Stack_error Calculate_Arr_Aligment (size_t capacity  , Stack* stk)
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
int Calculate_Size_Arr_Bytes (size_t capacity  , Stack* stk)
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
    FILE* fp = fopen (filename_write  , "w");

    if (fp == NULL) {

        printf ("Error: Oткрыть файл %s не удалось"  , filename_write);
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



    fprintf (fp  , "\n\ncanary_1 = %lld  , &canary_1 = %x\n\n"  , stk->canary_1  , &stk->canary_1);

    fprintf (fp  , "canary_2 = %lld  , &canary_2 = %x\n\n"  , stk->canary_2  , &stk->canary_2);

    if (Stack_Error (stk) == BAD_HASH_STACK) {

        printf ("Error: Содержимое структуры повреждено! Expected: %lu  , Found: %lu\n"  , stk->checksum_stack  , Stack_Calculate_Hashsum (stk));
        fprintf (fp  , "canary_3 = fail  , &canary_3 = %x\n\n"  , left_data_canary);
        fprintf (fp  , "canary_4 = fail  , &canary_4 = %x\n\n"  , right_data_canary);
        fprintf (fp  , "Массив данных повреждён  , САМОЛИКВИДИРУЮСЬ :(((((((((((");
        assert (0);
    }

    else {

        fprintf (fp  , "canary_3 = %lld  , &canary_3 = %x\n\n"  , *left_data_canary  , left_data_canary);
        fprintf (fp  , "canary_4 = %lld  , &canary_4 = %x\n\n"  , *right_data_canary  , right_data_canary);
    }
#endif
    fprintf (fp  , "size = %d\n\n"  , stk->size);

    fprintf (fp  , "capacity = %d\n\n"  , stk->capacity);
#ifndef NDEBUG
    fprintf (fp  , "Checksum_arr = %lu  Maybe_changed_checksum_arr = %lu\n\n"  , stk->checksum_arr  , Calculate_checksum (stk->data  , stk->capacity * sizeof (stack_element)));

    fprintf (fp  , "Checksum_stack = %lu Myabe_changed_checksum_stack = %lu\n\n"  , stk->checksum_stack  , Stack_Calculate_Hashsum(stk));
#endif
    fprintf (fp  , "&data = %x\n\n"  , stk->data);

    fprintf(fp , "[");

for (int i = 0; i < stk->capacity; i++) {

        if (i < stk->size)

            fprintf(fp , " %lg " , stk->data[i]);

        else

            fprintf(fp , " _ ");
    }

    fprintf(fp , "]");

    fclose (fp);

    return COMPLETE_VALUE;
}

Stack_error Stack_Ctor (Stack* stk  , size_t capacity)
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

    Calculate_Arr_Aligment (capacity  , stk);

    stack_element* data = (stack_element*) ((char*) calloc (1  , Calculate_Size_Arr_Bytes (capacity  , stk)));
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
Stack_error Stack_Push (Stack* stk  , stack_element value)
{
    my_assert (stk == NULL);
    STACK_ASSERT (stk);

    if (stk->size >= stk->capacity)

        Stack_Realloc_Up (stk);

    stk->data[stk->size++] = value;
#ifndef NDEBUG
    stk->checksum_arr = Calculate_checksum (stk->data  , stk->capacity * sizeof (stack_element));
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

    // Stack_Dump (stk);

    if (stk->size == 0) {
        printf("Error: массив пустой! Size = %zu\n"  , stk->size);
        assert (0);
    }

    stk->size--;

    stack_element popped_value = stk->data[stk->size];

    stk->data[stk->size] = 0;

//     if (stk->size < 0.25 * stk->capacity)
//
//         Stack_Realloc_Down (stk);

#ifndef NDEBUG
    stk->checksum_arr = Calculate_checksum (stk->data  , stk->capacity * sizeof (stack_element));
    stk->checksum_stack = Stack_Calculate_Hashsum(stk);
#endif
    STACK_ASSERT(stk);


    //printf ("%lg\n"  , popped_value);

    return popped_value;
}

Stack_error Stack_Realloc_Up (Stack* stk)
{
    Stack_Realloc (stk  , FACTOR_INCR);
    return COMPLETE_VALUE;
}

Stack_error  Stack_Realloc_Down (Stack* stk)
{
    Stack_Realloc (stk  , FACTOR_DECR);
    return COMPLETE_VALUE;
}


Stack_error Stack_Realloc (Stack* stk  , double factor)
{
    my_assert (stk == NULL);

    int new_capacity = (int)(stk->capacity * factor);
    if (new_capacity == 0) {
        new_capacity = 1;
    }

    Calculate_Arr_Aligment (new_capacity  , stk);

    if (stk->data == NULL) {
    printf("Ошибка: Указатель на данные стека NULL перед realloc!\n");
    assert(0);
    }
#ifndef NDEBUG
    stack_element* new_data = (stack_element*) realloc ((canary_t*) stk->data - 1  , Calculate_Size_Arr_Bytes (new_capacity  , stk));
    my_assert (new_data == NULL);

    *((canary_t*) new_data) = val_canary_3;

    stk->data = (stack_element*) ((canary_t*) new_data + 1);
#else
    stack_element* new_data = (stack_element*) realloc (stk->data  , Calculate_Size_Arr_Bytes (new_capacity  , stk));
    my_assert (new_data == NULL);

    stk->data = new_data;
#endif


    if (factor > 1) {

        memset (stk->data + stk->capacity  , 0  , (new_capacity - stk->capacity) * sizeof (stack_element));
    }
#ifndef NDEBUG
    *((canary_t*) ((char*) (stk->data + new_capacity) + stk->length_bytes_aligment)) = val_canary_4;
#endif
    stk->capacity = new_capacity;


    return COMPLETE_VALUE;
}

stack_element Stack_Top(Stack* stk)
{
    my_assert(stk == NULL);
    STACK_ASSERT(stk);

    if (stk->size == 0) {
        printf ("Error: массив пустой! Size = %zu\n" , stk->size);
        assert (0);
    }

    return stk->data[stk->size - 1]; // Возвращаем верхний элемент , не уменьшая размер стека
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

int Processor_init (Processor* proc , const char* filename)
{

    FILE* input_file = fopen (filename , "r");
    if (input_file == NULL) {
        printf ("Ошибка: не удалось открыть файл %s\n" , filename);
        return -1;
    }

    char line[MAX_LINE_LENGTH] = "";


    if (fgets(line , sizeof (line) , input_file) != NULL) {
        line[strcspn (line , "\n")] = '\0';
        if (strcmp (line , SIGNATURE) != 0) {
            printf ("Ошибка: неверная сигнатура файла\n");
            fclose (input_file);
            return -1;
        }
    } else {
        printf ("Ошибка: не удалось считать сигнатуру\n");
        fclose (input_file);
        return -1;
    }


    if (fgets (line , sizeof (line) , input_file) != NULL) {
        proc->code_size = atoi (line);
        proc->code_capacity = proc->code_size;
    } else {
        printf ("Ошибка: не удалось считать размер кода\n");
        fclose (input_file);
        return -1;
    }

    proc->code = (int*) calloc (proc->code_size , sizeof (int));
    if (proc->code == NULL) {
        printf ("Ошибка: недостаточно памяти для массива кода\n");
        fclose (input_file);
        return -1;
    }

    for (int i = 0 ; i < proc->code_size ; i++) {
        if (fscanf (input_file , "%d" , &proc->code[i]) != 1) {
            printf ("Ошибка: некорректные данные в коде\n");
            free (proc->code);
            fclose (input_file);
            return -1;
        }
    }


    fclose (input_file);

    for (int i = 0 ; i < NUM_REGISTERS ; i++) {
    proc->registers[i] = 0;
    }

    proc->RAM = (int*) calloc (SIZE_X * SIZE_Y , sizeof (int));

    // for (int i = 0 ; i < proc->code_size ; i++) {
    //     printf ("Code[%d] = %d\n" , i , proc->code[i]);
    // }

    return 0;
}

int Get_Arg_Push (Processor* proc)
{
    int arg_type = proc->code[proc->ip++];
    // printf("Get_Arg_Push: arg_type = %d, ip = %d\n\n", arg_type, proc->ip);
    int arg_value = 0;

    if (arg_type & First_Bit) {
        arg_value = proc->code[proc->ip++];
    }

    if (arg_type & Second_Bit) {
        arg_value += proc->registers[proc->code[proc->ip++]];
    }

    if (arg_type & Thirst_Bit) {
        arg_value = proc->RAM[arg_value];
    }
    // fprintf (stderr , " argvalue_push = %d\n" , arg_value);
    return arg_value;
}

int* Get_Arg_Pop (Processor* proc) {
    int arg_type = proc->code[proc->ip++];
    //printf("Get_Arg_Pop: arg_type = %d, ip = %d\n", arg_type, proc->ip);
    int* arg_adress = NULL;
    int arg_value = 0;

    if ((arg_type & Second_Bit) && !(arg_type & Thirst_Bit) && !(arg_type & First_Bit)) {
        arg_adress = &proc->registers[proc->code[proc->ip++]];
    }

    if (arg_type & Thirst_Bit) {
        if (arg_type & Second_Bit) {
            // fprintf (stderr , "second bit\n");
            arg_value = proc->registers[proc->code[proc->ip++]];
        }

        if (arg_type & First_Bit) {

            arg_value += proc->code[proc->ip++];
        }

        arg_adress = &proc->RAM[arg_value];
        // fprintf (stderr , " arg_value_pop = %d\n" , arg_value);
        // fprintf (stderr , " arg_adress_pop = %d\n" , arg_adress);
        // fprintf (stderr , " &RAM[0] = %d\n" , &proc->RAM[0]);
        // fprintf (stderr , " IP_POP = %d\n" , proc->ip);
    }

    return arg_adress;
}


void Interpret (Processor* proc)
{
    Stack stk = {};
    Stack func_asm = {};
    Stack_Ctor(&stk , 10);
    Stack_Ctor (&func_asm , 10);


    while (proc->ip < proc->code_size) {
        int cmd = proc->code[proc->ip++];
        // fprintf(stderr, "cycle, command = '%d'\n", cmd);

        switch (cmd) {
            case CMD_PUSH: {
                // printf ("==========================================================================\n");
                // printf ("PUSH  Current IP: %d , Command: %d , proc->code[ip] = %d stk.size = %d\n\n"  , proc->ip , cmd , proc->code[proc->ip] , stk.size);
                Stack_Push (&stk , (double) Get_Arg_Push (proc));
                // printf ("==========================================================================\n");
                // Stack_Dump (&stk);
                break;

            }

            case CMD_ADD: {
                int a = Stack_Pop (&stk);
                int b = Stack_Pop (&stk);
                Stack_Push  (&stk , (double) (b + a));
                // printf("Current IP: %d , Command: %d\n" , proc->ip , cmd);
                // printf("Register AX: %d , BX: %d , CX: %d , DX: %d\n" , proc->registers[1] , proc->registers[2] , proc->registers[3] , proc->registers[4]);
                // printf("Stack size: %d , Top element: %lg\n" , stk.size , stk.size > 0 ? Stack_Top(&stk) : -1);
                break;
            }

            case CMD_SUB: {
                int a = Stack_Pop (&stk);
                int b = Stack_Pop (&stk);
                Stack_Push (&stk , (double) (b - a));
                // printf("Current IP: %d , Command: %d\n" , proc->ip , cmd);
                // printf("Register AX: %d , BX: %d , CX: %d , DX: %d\n" , proc->registers[1] , proc->registers[2] , proc->registers[3] , proc->registers[4]);
                // printf("Stack size: %d , Top element: %lg\n" , stk.size , stk.size > 0 ? Stack_Top(&stk) : -1);
                break;
            }

            case CMD_MUL: {
                int a = Stack_Pop (&stk);
                int b = Stack_Pop (&stk);
                Stack_Push (&stk , (double) (b * a));
                // printf("Current IP: %d , Command: %d\n" , proc->ip , cmd);
                // printf("Register AX: %d , BX: %d , CX: %d , DX: %d\n" , proc->registers[1] , proc->registers[2] , proc->registers[3] , proc->registers[4]);
                // printf("Stack size: %d , Top element: %lg\n" , stk.size , stk.size > 0 ? Stack_Top(&stk) : -1);
                break;
            }

            case CMD_DIV: {
                int a = Stack_Pop (&stk);
                int b = Stack_Pop (&stk);
                Stack_Push (&stk , (double) (b / a));
                // printf("Current IP: %d , Command: %d\n" , proc->ip , cmd);
                // printf("Register AX: %d , BX: %d , CX: %d , DX: %d\n" , proc->registers[1] , proc->registers[2] , proc->registers[3] , proc->registers[4]);
                // printf("Stack size: %d , Top element: %lg\n" , stk.size , stk.size > 0 ? Stack_Top(&stk) : -1);
                break;
            }

            case CMD_MOD: {
                int a = Stack_Pop (&stk);
                int b = Stack_Pop (&stk);
                Stack_Push (&stk , (double) (b % a));
                // printf("Current IP: %d , Command: %d\n" , proc->ip , cmd);
                // printf("Register AX: %d , BX: %d , CX: %d , DX: %d\n" , proc->registers[1] , proc->registers[2] , proc->registers[3] , proc->registers[4]);
                // printf("Stack size: %d , Top element: %lg\n" , stk.size , stk.size > 0 ? Stack_Top(&stk) : -1);
                break;
            }


            case CMD_OUT: {
                printf ("%d\n" , (int) Stack_Pop (&stk));
                break;
            }

            case CMD_POP: {
                // printf ("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
                // printf ("POP Current IP: %d , Command: %d , proc->code[ip] = %d , stk.size = %d \n"  , proc->ip , cmd , proc->code[proc->ip] , stk.size);
                // fprintf (stderr , "stack_top =  %lg" , Stack_Top (&stk));
                *Get_Arg_Pop (proc) = (int) Stack_Pop (&stk);
                // printf ("POP Current IP: %d , Command: %d , proc->code[ip] = %d , stk.size = %d \n"  , proc->ip , cmd , proc->code[proc->ip] , stk.size);
                // printf ("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
                break;
            }

            case CMD_HLT: {
                printf ("УРААА ЗАРАБОТАЛО:)))))))\n");
                printf ("Программа завершена.\n");
                Stack_Dtor (&stk);
                return;
            }
            case CMD_CALL: {
                int return_adress = proc->ip + 1;
                Stack_Push (&func_asm  , (double) return_adress);
                proc->ip = proc->code[proc->ip];
                break;
            }

            case CMD_RET: {
                proc->ip = (int) Stack_Pop (&func_asm);
                break;
            }

            case CMD_JMP: {
                int address = proc->code[proc->ip++];
                proc->ip = address;
                break;

            }

            case CMD_JA: {
                int a = Stack_Pop (&stk);
                int b = Stack_Pop (&stk);
                int address = proc->code[proc->ip++];
                if (b > a) {
                    proc->ip = address;
                }

                break;
            }

            case CMD_JAE: {
                int a = Stack_Pop (&stk);
                int b = Stack_Pop (&stk);
                int address = proc->code[proc->ip++];
                if (b >= a) {
                    proc->ip = address;
                }

                break;
            }

            case CMD_JB: {
                int a = Stack_Pop(&stk);
                int b = Stack_Pop(&stk);
                int address = proc->code[proc->ip++];
                // printf("JB: a = %d, b = %d, address = %d, IP before: %d\n", a, b, address, proc->ip);

                if (b < a)
                    proc->ip = address;


                break;
            }

            case CMD_JBE: {
                int a = Stack_Pop (&stk);
                int b = Stack_Pop (&stk);
                int address = proc->code[proc->ip++];
                if (b <= a) {
                    proc->ip = address;
                }

                break;
            }

            case CMD_JE: {
                int a = Stack_Pop (&stk);
                int b = Stack_Pop (&stk);
                int address = proc->code[proc->ip++];
                if (b == a) {
                    proc->ip = address;
                }

                break;
            }

            case CMD_JNE: {
                int a = Stack_Pop (&stk);
                int b = Stack_Pop (&stk);
                int address = proc->code[proc->ip++];
                if (b != a) {
                    proc->ip = address;
                }

                break;
            }

            case CMD_DRAW: {
                for (int i = 0 ; i < SIZE_X * SIZE_Y ; i++) {
                        if (i % SIZE_X == 0)
                            printf ("\n");

                        if (proc->RAM[i] == 0)
                            printf ("**");

                        else
                            printf ("##");
                }
                break;
            }

            case CMD_SQRT: {
                int value = (int) Stack_Pop (&stk);
                Stack_Push (&stk, (double) sqrt (value));
                break;
            }

            default: {
                printf ("Current IP: %d , Command: %d , proc->code[ip] = %d\n"  , proc->ip , cmd , proc->code[proc->ip]);
                printf ("Ошибка: неизвестная команда %d\n" , cmd);
                return;
            }
        }
    }
}

int main ()
{
    Processor proc = {};
    if (Processor_init (&proc , "output.txt") != 0) {
        printf("Ошибка загрузки машинного кода\n");
        return -1;
    }

    Interpret (&proc);

    return 0;
}
