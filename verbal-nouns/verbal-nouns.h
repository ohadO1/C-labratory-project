#ifndef VERBAL_NOUN_H
#define VERBAL_NOUN_H

#include "stdio.h"
#include "string.h"

/* Consts */
typedef enum OPERANDS_NUM { ZERO, ONE, TWO, THREE } OperandsNum;
#define VERBAL_NOUNS_COUNT 16

/* VerbalNoun definition*/
typedef struct {
  char verbalNoun[10];
  unsigned int opcode;
  OperandsNum operandsNumsSource[4];
  OperandsNum operandsNumsDest[4];
  unsigned int numOfOperands;
} VerbalNoun;

/* Defines a verbalNoun const */
typedef VerbalNoun VerbalNounTable[VERBAL_NOUNS_COUNT];

/* Prints the constant verbal noun table (for debugging)*/
void printVerbalNounTable(VerbalNounTable table);

/* Takes a string and checks if it in the verbalNoun table, if so return sthe
 * right verbalNoun */
VerbalNoun *findVerbalNounInTable(VerbalNounTable table, char *verbalNoun);

/* Initializes a vebal noun table (used once)*/
void initializeVerbalNounTable(VerbalNounTable **tablePtr);

#endif
