#include"Comp.h"

int main (int argc, char* argv[])
{
    Processor proc = {};
    LabelTable labels = {};
    LabelTable_init (&labels);

    char* filename = argv[1];

    RemoveComments (filename);



    Processor_init (&proc , filename);

    FILE* input_file = fopen (filename , "r");
    int code_size = 0;

    FirstPass (filename , &labels , &proc);

    Compile (filename , &proc , &labels);
    Processor_save (&proc , "output.txt");
    Processor_save_bin (&proc  , "output.bin");
    free (proc.code);
    return 0;
}

//========================================================================================
int Compile (const char* input_filename , Processor* proc , LabelTable* labels)
{
    FILE* input_file = fopen (input_filename , "r");

    my_assert (input_file == NULL);
    if (input_file == NULL) {
        printf("Ошибка открытия файла\n");
        return -1;
    }

    char command[50] = "";
    while (fscanf (input_file , "%s" , command) != EOF){
    // fprintf(stderr, "cycle, command = '%s'\n", command);

        if (command[strlen (command) - 1] == ':') {
            continue;

        } else if (stricmp (command , "push") == 0) {
            Process_Push_Command (input_file , proc);

        } else if (stricmp (command , "pop") == 0) {
            Process_Pop_Command (input_file , proc);

        } else if (stricmp (command , "call") == 0) {
            Process_Jump_Call_Command (proc , labels , CMD_CALL , input_file);

        } else if (stricmp (command , "ret") == 0) {
        //  fprintf (stderr ,"HUILO1\n");
            proc->code[proc->ip++] = CMD_RET;

        } else if (stricmp (command , "add") == 0) {
            proc->code[proc->ip++] = CMD_ADD;

        } else if (stricmp (command , "sub") == 0) {
            proc->code[proc->ip++] = CMD_SUB;

        } else if (stricmp (command , "mul") == 0) {
            proc->code[proc->ip++] = CMD_MUL;

        } else if (stricmp (command , "div") == 0) {
            proc->code[proc->ip++] = CMD_DIV;

        } else if (strcmp (command , "MEOUW") == 0) {
            proc->code[proc->ip++] = CMD_MEOUW;

        } else if (stricmp (command , "out") == 0) {
            proc->code[proc->ip++] = CMD_OUT;

        } else if (stricmp (command, "in") == 0) {
            proc->code[proc->ip++] = CMD_IN;

        } else if (stricmp (command , "hlt") == 0) {
            proc->code[proc->ip++] = CMD_HLT;

        } else if (stricmp (command , "draw") == 0) {
            proc->code[proc->ip++] = CMD_DRAW;

        } else if (stricmp (command , "sqrt") == 0) {
            proc->code[proc->ip++] = CMD_SQRT;

        } else if (stricmp (command , "mod") == 0) {
            proc->code[proc->ip++] = CMD_MOD;

        } else if (stricmp (command , "jmp") == 0) {
            Process_Jump_Call_Command (proc , labels , CMD_JMP , input_file);

        } else if (stricmp (command , "ja") == 0) {
            Process_Jump_Call_Command (proc , labels , CMD_JA , input_file);

        } else if (stricmp (command , "jae") == 0) {
            Process_Jump_Call_Command (proc , labels , CMD_JAE , input_file);

        } else if (stricmp (command , "jb") == 0) {
            Process_Jump_Call_Command (proc , labels , CMD_JB , input_file);

        } else if (stricmp (command , "jbe") == 0) {
            Process_Jump_Call_Command (proc , labels , CMD_JBE , input_file);

        } else if (stricmp (command , "je") == 0) {
            Process_Jump_Call_Command (proc , labels , CMD_JE , input_file);

        } else if (stricmp (command , "jne") == 0) {
            Process_Jump_Call_Command (proc , labels , CMD_JNE , input_file);
        }

        else {
            printf ("Ошибка , нераспознанная команда '%s'" , command);
        }
    }

    proc->header.code_size = proc->ip;
    proc->code_size = proc->ip;
    my_assert(input_file == NULL);
    fclose (input_file);
    return 0;
}


//========================================================================================
int RegisterToNumber (const char* reg_name)
{
    if (stricmp (reg_name , "ax") == 0) return 1;
    else if (stricmp (reg_name , "bx") == 0) return 2;
    else if (stricmp (reg_name , "cx") == 0) return 3;
    else if (stricmp (reg_name , "dx") == 0) return 4;
    else if (stricmp (reg_name , "as") == 0) return 5;
    else if (stricmp (reg_name , "bs") == 0) return 6;
    else if (stricmp (reg_name , "cs") == 0) return 7;
    else if (stricmp (reg_name , "bp") == 0) return 8;
    else if (stricmp (reg_name , "sp") == 0) return 9;
    else return -1;
}
//========================================================================================
void RemoveComments(const char* filename)
{
    FILE* input_file = fopen(filename, "r");
    if (input_file == NULL) {
        perror("Failed to open input file");
        return;
    }

    char temp_filename[] = "temp_file.txt";
    FILE* temp_file = fopen(temp_filename, "w");
    if (temp_file == NULL) {
        perror("Failed to open temporary file");
        fclose(input_file);
        return;
    }

    char line[256] = "";
    while (fgets(line, sizeof(line), input_file)) {

        char* comment_pos = strchr(line, ';');
        if (comment_pos != NULL)
            *comment_pos = '\0';



        char* start = line;
        while (isspace((unsigned char)*start)) start++;
        char* end = start + strlen(start) - 1;
        while (end > start && isspace((unsigned char)*end)) end--;
        *(end + 1) = '\0';


        if (strlen(start) > 0) {
            fputs(start, temp_file);
            fputc('\n', temp_file);
        }
    }

    fclose(input_file);
    fclose(temp_file);


    remove(filename);
    rename(temp_filename, filename);
}

//========================================================================================
void Process_Jump_Call_Command (Processor* proc, LabelTable* labels, int cmd_code , FILE* input_file)
{
    char label[LABEL_NAME_MAX] = "";
    fscanf(input_file, "%s", label);
    int address = LabelTable_get_address(labels, label);
    if (address == -1) {

        printf("Ошибка: метка %s не найдена , adress = %d\n", label , address);
        exit(-1);
    }
    proc->code[proc->ip++] = cmd_code;
    proc->code[proc->ip++] = address;
}

//========================================================================================
void Process_Push_Command (FILE* input_file , Processor* proc)
{
    char token[50] = "";
        fscanf (input_file , "%s" , token);
        if (token[0] == '[') {
            char inside_brackets[50]= "";
            sscanf (token , "[%[^]]" , inside_brackets);

            char* plus_sign = strchr (inside_brackets  , '+');
            if (plus_sign != NULL) {                                          // Память + регистр + константа
                char reg[10] = "";
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
                int reg_number = RegisterToNumber (inside_brackets);
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
                char reg[10] = "";
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
}

//========================================================================================
void Process_Pop_Command (FILE* input_file , Processor* proc)
{
    char token[50] = {};
            fscanf (input_file , "%s" , token);

            int reg_number = RegisterToNumber (token);
            if (reg_number != -1) {
                proc->code[proc->ip++] = CMD_POP;
                proc->code[proc->ip++] = ARG_REG;
                proc->code[proc->ip++] = reg_number;
        }

        else if (token[0] == '[') {
            char inside_brackets[50] = {};
            sscanf (token , "[%[^]]" , inside_brackets);

            //printf ("%s" , inside_brackets);

            char* plus_sign = strchr (inside_brackets , '+');
            if (plus_sign != NULL) {
                char reg[10] = {};
                int value = 0;

                strncpy (reg , inside_brackets , 2);
                char* value_start = plus_sign + 1;
                while (*value_start == ' ') {
                    value_start++;
                }
                value = atoi (value_start);
                //printf ("reg %s" , reg);

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
}

//========================================================================================
FILE* Open_File (const char* filename_read)
{
    FILE* fp = fopen (filename_read  , "r");

    if (fp == NULL) {

        printf ("Error: Oткрыть файл %s не удалось"  , filename_read);
        perror ("EDA");
    }

    return fp;
}

//========================================================================================
int CountWordsInFile (const char* filename)
{
    FILE* file = fopen (filename , "r");
    if (file == NULL) {
        printf ("Ошибка открытия файла\n");
        return -1;
    }

    int word_count = 0;
    char word[50] = "";


    while (fscanf (file , "%s" , word) != EOF) {
        word_count++;
    }

    fclose (file);
    return word_count;
}

//========================================================================================
void Process_File (const char* filename)
{
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    char line[200] = "";

    while (fgets(line, sizeof(line), file)) {
        // Найти начало комментария
        char* comment_start = strchr(line, ';');
        if (comment_start) {
            *comment_start = '\0'; // Убрать всё после точки с запятой
        }

        // Удалить завершающие пробелы или символы перевода строки
        size_t len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == ' ' || line[len - 1] == '\t')) {
            line[--len] = '\0';
        }

        // Если строка после удаления комментариев не пустая, обработать её
        if (strlen(line) > 0) {
            printf("Processing: %s\n", line);
            // Здесь можно вставить код обработки строки
        }
    }

    fclose(file);
}

//========================================================================================
void Processor_init (Processor* proc , const char* input_filename)
{
    strcpy (proc->header.signature , SIGNATURE);

    int word_count = CountWordsInFile (input_filename);
    if (word_count <= 0) {
        printf("Ошибка: файл пустой или не найден\n");
        return;
    }

    proc->code_capacity = word_count * 2;
    proc->code = (int*) calloc (proc->code_capacity , sizeof (int));
    proc->code_size = 0;
    proc->ip = 0;
}

//========================================================================================
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

//========================================================================================
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

//========================================================================================
void LabelTable_init (LabelTable* table)
{
    table->label_count = 0;
}

//========================================================================================
int LabelTable_add(LabelTable* table, const char* label_name, int address) {
    for (int i = 0; i < table->label_count; i++) {
        if (strcmp(table->labels[i].name, label_name) == 0) {
            printf("Ошибка: метка '%s' уже существует\n", label_name);
            return -1;  // Возвращаем ошибку, если метка дублируется
        }
    }

    if (table->label_count >= MAX_LABELS) {
        printf("Ошибка: слишком много меток\n");
        return -1;
    }

    strcpy(table->labels[table->label_count].name, label_name);
    table->labels[table->label_count].address = address;
    table->label_count++;

    return 0;
}

//========================================================================================
int LabelTable_get_address (LabelTable* table , const char* label_name)
{
    char temp_name[50] = "";
    strcpy(temp_name, label_name);
    ToLowerCase(temp_name);

    for (int i = 0; i < table->label_count; i++) {
        printf ("temp_name = %s\n" , temp_name);
        if (stricmp(table->labels[i].name, temp_name) == 0) {
            return table->labels[i].address;
        }
    }
    return -1;
}

//========================================================================================
int FirstPass (const char * filename , LabelTable* labels , Processor* proc)
{
    my_assert (filename == NULL);

    FILE* input_file = fopen (filename , "r");
    char command[50] = "";
    int current_address = 0;

    while (fscanf(input_file , "%s" , command) != EOF) {

        fprintf(stderr, "command = '%s'\n", command);
        if (command[strlen (command) - 1] == ':') {

            printf ("metka: '%s'\n\n" , command);
            command[strlen (command) - 1] = '\0';
            if (LabelTable_get_address(labels, command) != -1) {

                printf("Ошибка: метка '%s' уже существует\n", command);
                continue; // Пропускаем дублирующуюся метку
            }
            LabelTable_add (labels , command , current_address);

        } else {

            if (strncmp (command, "push", 4) == 0 || strncmp (command, "pop", 3) == 0) {

                char arg[50] = "";
                fscanf (input_file , "%s" , arg);

                if (strchr (arg , '+') != NULL) {
                    current_address += 4;

                } else {
                    current_address += 3;
                }

            } else
                current_address++;

        }
    }

    for (int i = 0; i < labels->label_count; i++) {
        printf("Метка: %s, Адрес: %d\n", labels->labels[i].name, labels->labels[i].address);
    }

    fclose (input_file);
    return 0;
}

//========================================================================================
void ToLowerCase(char* str)
{
    for (int i = 0; str[i]; i++) {
        str[i] = tolower((unsigned char)str[i]);
    }
}


