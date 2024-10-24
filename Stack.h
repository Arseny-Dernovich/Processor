
typedef double stack_element;

#ifndef NDEBUG
    typedef long long int canary_t;
    enum canary_values {
        val_canary_1 = 777777,
        val_canary_2 = 666666,
        val_canary_3 = 334334,
        val_canary_4 = 333333
    };
#endif

#define STACK_ASSERT(stk)                                   \
        if (Stack_Error (stk) != COMPLETE_VALUE) {          \
            Stack_Dump (stk);                               \
            assert (0);                                     \
        }

struct Stack {
#ifndef NDEBUG
    canary_t canary_1;
#endif
    stack_element* data;
    unsigned int length_bytes_aligment;
    size_t size;
    size_t capacity;
#ifndef NDEBUG
    unsigned long checksum_stack;
    unsigned long checksum_arr;
    canary_t canary_2;
#endif
};

enum Stack_error {
    BAD_HASH_ARR = 52,
    BAD_HASH_STACK = 69,
    BAD_SIZE = 111,
#ifndef NDEBUG
    BAD_CANARY_1 = 1,
    BAD_CANARY_2 = 2,
    BAD_CANARY_3 = 3,
    BAD_CANARY_4 = 4,
#endif
    COMPLETE_VALUE = 0,
};

enum Factor_Realloc {
    UP,
    DOWN
};

const double FACTOR_INCR = 2;
const double FACTOR_DECR = 1.0 / FACTOR_INCR;

Stack_error Stack_Error (Stack* stk);
Stack_error Stack_Dump (Stack* stk);
FILE* Open_Log_File (const char* filename_write);
Stack_error Stack_Dtor (Stack* stk);
Stack_error Stack_Ctor (Stack* stk , size_t capacity);
Stack_error Stack_Push (Stack* stk , stack_element value);
stack_element Stack_Pop (Stack* stk);
unsigned long Stack_Calculate_Hashsum (Stack* stk);
unsigned long Calculate_checksum (void* data , size_t size_in_bytes); // header
int Calculate_Size_Arr_Bytes (size_t capacity , Stack* stk);
Stack_error Calculate_Arr_Aligment (size_t capacity , Stack* stk);
Stack_error  Stack_Realloc_Down (Stack* stk);
Stack_error Stack_Realloc_Up (Stack* stk);
Stack_error Stack_Realloc (Stack* stk , double factor);

/*
data[0] = 1
data[1] = -7
data[2] = 0
data[3] = FREE
data[4] = FREE
data[5] = FREE
data[6] = 8
data[7] = FREE
data[8] = FREE
*/
