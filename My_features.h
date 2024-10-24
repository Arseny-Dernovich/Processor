#ifndef DRA_HEADER
#define DRA_HEADER
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <assert.h>
#include <sys/stat.h>
#include "Stack.h"



#ifndef NDEBUG


    #define my_assert(condition)                                                                    \
        if ((condition) == true) {                                                                  \
            printf ("Assertaion Error : "#condition" , file  %s , line %d" , __FILE__ , __LINE__);  \
            abort ();                                                                               \
        }                                                                                           \

#else

    #define my_assert(condition)

#endif


typedef int (*My_Compare) (const void* point1 , const void* point2);




enum indicators_progress { // индикаторы выполнениия
    complete_value = 0 ,
    incomplete_value = 1 ,
};

enum indicators_compare {  // индикаторы сравнения
    str1_longer_str2 = 1 ,
    str2_longer_str1 = -1 ,
    str1_equals_str2 = 3802
};

enum Sort_Type {           // индикаторы типа сортировки
    SORT_ALPHABETICALLY = 1,
    SORT_BY_RHYME = 2 ,
    INCRE = 1 ,
    DECRE = -1
};

enum Error_fstat {
    error_fstat = -1
};




const float RAND_MAX_F = RAND_MAX;


const char* const YELLOW         = "\033[3;33;40m";
const char* const RED             = "\033[3;31;40m";
const char* const BLUE_DARK       = "\033[3;34;40m";
const char* const BLUE            = "\033[3;36;40m";
const char* const BLUE_DARK_FLASH = "\033[5;34;40m";
const char* const GREEN           = "\033[3;32;40m";
const char* const END             = "\033[0m";




#endif
