#include <stdbool.h>
#include <stdbool.h>
#include "gc.h"

#pragma once

typedef union answer answer_t;

union answer {
    int int_t;
    char *str_t;
};

typedef bool (*check_func) (char *str, void *extra, heap_t *h);
typedef answer_t (*convert_func) (char *str, void *extra, heap_t *h);

/// @brief Ask a question and get the answer
/// @param question The question to be asked
/// @param check the check fucntion
/// @param check_extra argument for the check function
/// @param convert the convertion function
/// @param convert_extra argument for the check function
/// @return the answer
answer_t
ask_question (char *question, check_func check, void *check_extra, convert_func convert, void *convert_extra, heap_t *h);

/// @brief ask a question with a string as an answer
/// @param question the question to be asked
/// @return the answered string
char *ask_question_string (char *question, heap_t *h);

/// @brief ask a question with an int as an answer
/// @param question the question to be asked
/// @return the answered int
int ask_question_int (char *question, heap_t *h);

/// @brief checks if a string is not empty
/// @param str the string
/// @return true if the string is not empty
bool not_empty (char *str);

/// @brief checks if a string is a number
/// @param str the string
/// @return true if the string is a number
bool is_number (char *str);

/// @brief ask a question with a unsigned int as an answer
/// @param question the question to be asked
/// @return the answered unsigned int
int ask_question_u_int (char *question, heap_t *h);

/// @brief converts a string to an int
/// @param str the string
/// @param extra non-used argument, set to NULL
/// @return an answer_t with the int inside
answer_t str_to_int (char *str, void *extra, heap_t *h);

/// @brief converts a string to a string
/// @param str the string
/// @param extra non-used argument, set to NULL
/// @return an answer_t with the new string inside
answer_t str_to_str (char *str, void *extra, heap_t *h);
