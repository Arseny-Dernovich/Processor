#include "My_features.h"
#include <stdio.h>
#include <string.h>
#include "Processor.h"

#define SIGNATURE "MYCPU"
#define MAX_LABELS 100
#define LABEL_NAME_MAX 50

struct Label {
    char name[LABEL_NAME_MAX];
    int address;
};

struct LabelTable {
    Label labels[MAX_LABELS];
    int label_count;
};
int RegisterToNumber (const char* reg_name)
{
    if (stricmp (reg_name , "ax") == 0) return 1;
    else if (stricmp (reg_name , "bx") == 0) return 2;
    else if (stricmp (reg_name , "cx") == 0) return 3;
    else if (stricmp (reg_name , "dx") == 0) return 4;
    else return 0;
}

FILE* Open_File (const char* filename_read)
{
    FILE* fp = fopen (filename_read  , "r");

    if (fp == NULL) {

        printf ("Error: Oткрыть файл %s не удалось"  , filename_read);
        perror ("EDA");
    }

    return fp;
}

int CountWordsInFile (const char* filename)
{
    FILE* file = fopen (filename , "r");
    if (file == NULL) {
        printf ("Ошибка открытия файла\n");
        return -1;
    }

    int word_count = 0;
    char word[50];


    while (fscanf (file , "%s" , word) != EOF) {
        word_count++;
    }

    fclose (file);
    return word_count;
}

void Processor_init (Processor* proc , const char* input_filename) {
    strcpy (proc->header.signature , SIGNATURE);

    int word_count = CountWordsInFile (input_filename);
    if (word_count <= 0) {
        printf("Ошибка: файл пустой или не найден\n");
        return;
    }

    proc->code_capacity = word_count * 2;
    proc->code = (int*) calloc (word_count , sizeof (int));
    proc->code_size = 0;
    proc->ip = 0;
}

void Processor_save_bin (Processor* proc , const char* output_filename)
{
    FILE* output_file = fopen (output_filename , "wb");
    if (output_file == NULL) {
        printf ("Ошибка открытия файла для записи\n");
        return;
    }

    fwrite (&proc->header , sizeof (Header) , 1 , output_file);
    fwrite (proc->code , sizeof (int) , proc->code_size , output_file);

    fclose (output_file);
}


void Processor_save (Processor* proc , const char* output_filename)
{
    FILE* output_file = fopen (output_filename , "w");
    if (output_file == NULL) {
        printf ("Ошибка открытия файла для записи\n");
        return;
    }

    fprintf (output_file  , "%s\n"  , proc->header.signature);
    fprintf (output_file  , "%d\n"  , proc->header.code_size);
    for (int i = 0 ; i < proc->code_size ; i++)
        fprintf (output_file  , "%d\t"  , proc->code[i]);
    fprintf (output_file  , "\n");

    fclose (output_file);
}

void LabelTable_init (LabelTable* table)
{
    table->label_count = 0;
}

int LabelTable_add (LabelTable* table , const char* label_name , int address)
{
    if (table->label_count >= MAX_LABELS) {
        printf("Ошибка: слишком много меток\n");
        return -1;
    }
    strcpy (table->labels[table->label_count].name , label_name);
    table->labels[table->label_count].address = address;
    table->label_count++;

    return 0;
}

int LabelTable_get_address (LabelTable* table , const char* label_name)
{
    for (int i = 0; i < table->label_count; i++) {
        if (stricmp(table->labels[i].name , label_name) == 0) {
            return table->labels[i].address;
        }
    }
    return -1;
}

int FirstPass(FILE* input_file, LabelTable* labels, Processor* proc)
{
    my_assert(input_file == NULL);

    char command[50];
    int current_address = 0;

    while (fscanf(input_file, "%s", command) != EOF) {

        if (command[strlen(command) - 1] == ':') {
            command[strlen(command) - 1] = '\0';
            LabelTable_add(labels, command, current_address);
        } else {

            if (strncmp(command, "push", 4) == 0 || strncmp(command, "pop", 3) == 0) {
                char arg[50];
                fscanf(input_file, "%s", arg);

                if (strchr(arg, '+') != NULL) {

                    current_address += 4;
                } else {

                    current_address += 3;
                }
            } else {
                current_address++;
            }
        }
    }

    fclose(input_file);
    return 0;
}


int Compile (const char* input_filename , Processor* proc , LabelTable* labels)
{
    FILE* input_file = fopen (input_filename , "r");

    my_assert (input_file == NULL);
    if (input_file == NULL) {
        printf("Ошибка открытия файла\n");
        return -1;
    }

    char command[50];
    while (fscanf (input_file , "%s" , command) != EOF) {

        if (command[strlen (command) - 1] == ':') {
            continue;

        } else if (stricmp(command , "push") == 0) {
    char token[50];
    fscanf (input_file , "%s" , token);

    if (token[0] == '[') {
        char inside_brackets[50];
        sscanf (token , "[%[^]]]" , inside_brackets);


        char* plus_sign = strchr (inside_brackets  , '+');
        if (plus_sign != NULL) {                                          // Память + регистр + константа
            char reg[10] = {0};
            int value = 0;


            strncpy (reg , inside_brackets , 2 );
            char* value_start = plus_sign + 1;
            while (*value_start == ' ') {
                value_start++;
            }
            value = atoi (value_start);

            int reg_number = RegisterToNumber (reg);
            proc->code[proc->ip++] = CMD_PUSH;
            proc->code[proc->ip++] = ARG_MEM_REG_CONST;
            proc->code[proc->ip++] = value;
            proc->code[proc->ip++] = reg_number;

        } else if (isalpha (inside_brackets[0])) {                        // Память + регистр
            int reg_number = RegisterToNumber(inside_brackets);
            proc->code[proc->ip++] = CMD_PUSH;
            proc->code[proc->ip++] = ARG_MEM_REG;
            proc->code[proc->ip++] = reg_number;

        } else if (isdigit (inside_brackets[0])) {                        // Память + константа
            int address = atoi (inside_brackets);
            proc->code[proc->ip++] = CMD_PUSH;
            proc->code[proc->ip++] = ARG_MEM_CONST;
            proc->code[proc->ip++] = address;
        }

        } else if (isdigit (token[0])) {                                   // Константа
            int value = atoi (token);
            proc->code[proc->ip++] = CMD_PUSH;
            proc->code[proc->ip++] = ARG_CONST;
            proc->code[proc->ip++] = value;

        } else {                                                          // Аргумент - регистр или регистр + константа
            char* plus_sign = strchr (token , '+');
            if (plus_sign != NULL) {
                char reg[10] = {0};
                int value = 0;


                strncpy (reg , token , plus_sign - token);
                char* value_start = plus_sign + 1;
                while (*value_start == ' ') {
                    value_start++;
                }
                value = atoi (value_start);

                int reg_number = RegisterToNumber (reg);
                proc->code[proc->ip++] = CMD_PUSH;
                proc->code[proc->ip++] = ARG_REG_CONST;                 // Регистр + константа
                proc->code[proc->ip++] = value;
                proc->code[proc->ip++] = reg_number;
            } else {
                int reg_number = RegisterToNumber (token);
                proc->code[proc->ip++] = CMD_PUSH;
                proc->code[proc->ip++] = ARG_REG;                           // Регистр
                proc->code[proc->ip++] = reg_number;
            }
        }

        } else if (stricmp (command , "pop") == 0) {
        char token[50];
        fscanf (input_file , "%s" , token);

        int reg_number = RegisterToNumber (token);
        if (reg_number != -1) {
            proc->code[proc->ip++] = CMD_POP;
            proc->code[proc->ip++] = ARG_REG;
            proc->code[proc->ip++] = reg_number;
        }

        else if (token[0] == '[') {
            char inside_brackets[50];
            sscanf(token , "[%[^]]]" , inside_brackets);

            char* plus_sign = strchr (inside_brackets , '+');
            if (plus_sign != NULL) {
                char reg[10] = {0};
                int value = 0;

                strncpy (reg , inside_brackets , 2);
                char* value_start = plus_sign + 1;
                while (*value_start == ' ') {
                    value_start++;
                }
                value = atoi (value_start);

                int reg_number = RegisterToNumber (reg);
                proc->code[proc->ip++] = CMD_POP;
                proc->code[proc->ip++] = ARG_MEM_REG_CONST;
                proc->code[proc->ip++] = value;
                proc->code[proc->ip++] = reg_number;

            } else if (isalpha (inside_brackets[0])) {
                int reg_number = RegisterToNumber (inside_brackets);
                proc->code[proc->ip++] = CMD_POP;
                proc->code[proc->ip++] = ARG_MEM_REG; // Память + регистр
                proc->code[proc->ip++] = reg_number;

            } else if (isdigit (inside_brackets[0])) {  // Память + константа
                int address = atoi (inside_brackets);
                proc->code[proc->ip++] = CMD_POP;
                proc->code[proc->ip++] = ARG_MEM_CONST;
                proc->code[proc->ip++] = address;
            }
        } else
            printf ("Ошибка: неверный аргумент для pop!\n");



        } else if (stricmp (command , "call") == 0) {
            char label[LABEL_NAME_MAX];
            fscanf (input_file , "%s" , label);
            int address = LabelTable_get_address (labels , label);
            if (address == -1) {
                printf ("Ошибка: метка %s не найдена\n" , label);
                return -1;
            }
        proc->code[proc->ip++] = CMD_CALL;
        proc->code[proc->ip++] = address;

        } else if (stricmp (command , "ret") == 0) {
            proc->code[proc->ip++] = CMD_RET;

        } else if (stricmp (command , "add") == 0) {
            proc->code[proc->ip++] = CMD_ADD;

        } else if (stricmp (command , "sub") == 0) {
            proc->code[proc->ip++] = CMD_SUB;

        } else if (stricmp (command , "mul") == 0) {
            proc->code[proc->ip++] = CMD_MUL;

        } else if (stricmp (command , "div") == 0) {
            proc->code[proc->ip++] = CMD_DIV;

        } else if (stricmp (command , "out") == 0) {
            proc->code[proc->ip++] = CMD_OUT;

        } else if (stricmp (command , "hlt") == 0) {
            proc->code[proc->ip++] = CMD_HLT;

        } else if (stricmp (command , "jmp") == 0) {
            char label[LABEL_NAME_MAX];
            fscanf (input_file , "%s" , label);
            int address = LabelTable_get_address (labels , label);
            if (address == -1) {
                printf ("Ошибка: метка %s не найдена\n" , label);
                return -1;
            }
            proc->code[proc->ip++] = CMD_JMP;
            proc->code[proc->ip++] = address;

        } else if (stricmp (command , "ja") == 0) {
            char label[LABEL_NAME_MAX];
            fscanf (input_file , "%s" , label);
            int address = LabelTable_get_address (labels , label);
            if (address == -1) {
                printf ("Ошибка: метка %s не найдена\n" , label);
                return -1;
            }
            proc->code[proc->ip++] = CMD_JA;
            proc->code[proc->ip++] = address;

        } else if (stricmp (command , "jae") == 0) {
            char label[LABEL_NAME_MAX];
            fscanf (input_file , "%s" , label);
            int address = LabelTable_get_address (labels , label);
            if (address == -1) {
                printf ("Ошибка: метка %s не найдена\n" , label);
                return -1;
            }
            proc->code[proc->ip++] = CMD_JAE;
            proc->code[proc->ip++] = address;

        } else if (stricmp (command , "jb") == 0) {
            char label[LABEL_NAME_MAX];
            fscanf (input_file , "%s" , label);
            int address = LabelTable_get_address (labels , label);
            if (address == -1) {
                printf ("Ошибка: метка %s не найдена\n" , label);
                return -1;
            }
            proc->code[proc->ip++] = CMD_JB;
            proc->code[proc->ip++] = address;

        } else if (stricmp (command , "jbe") == 0) {
            char label[LABEL_NAME_MAX];
            fscanf (input_file , "%s" , label);
            int address = LabelTable_get_address (labels , label);
            if (address == -1) {
                printf ("Ошибка: метка %s не найдена\n" , label);
                return -1;
            }
            proc->code[proc->ip++] = CMD_JBE;
            proc->code[proc->ip++] = address;

        } else if (stricmp (command , "je") == 0) {
            char label[LABEL_NAME_MAX];
            fscanf (input_file , "%s" , label);
            int address = LabelTable_get_address (labels , label);
            if (address == -1) {
                printf ("Ошибка: метка %s не найдена\n" , label);
                return -1;
            }
            proc->code[proc->ip++] = CMD_JE;
            proc->code[proc->ip++] = address;

        } else if (stricmp (command , "jne") == 0) {
            char label[LABEL_NAME_MAX];
            fscanf (input_file , "%s" , label);
            int address = LabelTable_get_address (labels , label);
            if (address == -1) {
                printf ("Ошибка: метка %s не найдена\n" , label);
                return -1;
            }
            proc->code[proc->ip++] = CMD_JNE;
            proc->code[proc->ip++] = address;
        }

    }

    proc->header.code_size = proc->ip;
    proc->code_size = proc->ip;
    fclose (input_file);
    return 0;
}


int main() {
    Processor proc = {};
    LabelTable labels;
    LabelTable_init(&labels);

    Processor_init(&proc , "text.txt");

    FILE* input_file = fopen("text.txt" , "r");
    int code_size = 0;

    FirstPass(input_file , &labels , &proc);

    Compile("text.txt" , &proc , &labels);

    Processor_save(&proc , "output.txt");
    Processor_save_bin (&proc  , "output.bin");

    fclose(input_file);
    return 0;
}




/*
int main() {
    LabelTable table = {};
    Processor proc = {};

    LabelTable_init(&table);
    Processor_init(&proc , "text.txt");

    FILE* fp = fopen("text.txt" , "r");

    FirstPass(fp , &table , &proc.code_size);
    Compile("text.txt" , &proc , &table);
    Processor_save(&proc , "output.txt");

    fclose(fp);

    return 0;
}
*/
