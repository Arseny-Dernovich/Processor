#ifndef COMP_H
#define COMP_H

#include "My_features.h"
#include "Processor.h"

#define LABEL_NAME_MAX 50
#define MAX_LABELS 1000
#define SIGNATURE "MYCPU"

#define CMD_MEOUW 1001
#define CMD_IN 1002

#if defined(_WIN32)
#define stricmp _stricmp
#else
#include <strings.h>
#define stricmp strcasecmp
#endif

struct Label {
    char name[LABEL_NAME_MAX];
    int address;
};

struct LabelTable {
    Label labels[MAX_LABELS];
    int label_count;
};

void LabelTable_init (LabelTable * table);
void Processor_init (Processor * proc , const char * input_filename);
void Processor_save (Processor * proc , const char * output_filename);
void Processor_save_bin (Processor * proc , const char * output_filename);
int FirstPass (const char * filename , LabelTable * labels , Processor * proc);
int Compile (const char * input_filename , Processor * proc , LabelTable * labels);
void RemoveComments (const char * filename);
void ToLowerCase (char * str);
int LabelTable_get_address (LabelTable * table , const char * label_name);
int RegisterToNumber (const char * reg_name);
void Process_Jump_Call_Command (Processor * proc , LabelTable * labels , int cmd_code , FILE * input_file);
void Process_Push_Command (FILE * input_file , Processor * proc);
void Process_Pop_Command (FILE * input_file , Processor * proc);

#endif
