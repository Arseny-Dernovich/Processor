#define SIZE_REG 8
#define SIZE_X 100
#define SIZE_Y 100

struct Header{
    char signature[6];
    int code_size;
};


struct Processor {
    Header header;        // Заголовок
    int* code;            // Массив с машинным кодом
    int code_capacity;    // Максимальный размер массива кода
    int code_size;        // Фактическое количество команд
    int ip;               // Instruction pointer (номер текущей команды)
    int registers[SIZE_REG];
    int* RAM;
};

enum CMD {
    CMD_HLT = -1,
    CMD_PUSH = 20,
    CMD_ADD = 2,
    CMD_SUB = 3,
    CMD_MUL = 4,
    CMD_DIV = 5,
    CMD_OUT = 6,
    CMD_POP = 30,
    CMD_JMP = 9 ,
    CMD_JA = 10 ,
    CMD_JAE = 11 ,
    CMD_JB = 12 ,
    CMD_JBE = 13 ,
    CMD_JE = 14 ,
    CMD_JNE = 15 ,
    CMD_RET = 16 ,
    CMD_CALL = 17 ,
    CMD_SQRT = 18 ,
    CMD_MOD = 19 ,

    CMD_DRAW = 111
};

enum Type_push_pop {
    ARG_CONST = 1 ,
    ARG_MEM_REG_CONST = 7 ,
    ARG_MEM_REG = 6 ,
    ARG_MEM_CONST = 5 ,
    ARG_REG_CONST = 3 ,
    ARG_REG = 2
};

enum Magic_const  {
    First_Bit = 1 ,
    Second_Bit = 2 ,
    Thirst_Bit = 4
};

