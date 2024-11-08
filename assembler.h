#ifndef ASSEMBLER_H
#define ASSEMBLER_H

/* Consts */
#define ERROR_CODE -1
#define SUCCESS_CODE 0
#define BITS_IN_BYTE 8
#define WORD_BITS_AMOUNT 15
#define PRE_ASSEMBLER_FILE_EXTENSION ".am"
#define INPUT_FILE_EXTENSION ".as"
#define MAX_LINE_LENGTH 82
#define MAX_LABEL_LENGTH 15

/* type of a word (15-bit integer)*/
typedef struct {
  int value : 15;
} Word;

/* Defined a custom Bool enum for true/false values */
typedef enum { False = 0, True = 1 } Bool;

#endif
