#ifndef HELPERFUNCTIONS_H
#define HELPERFUNCTIONS_H

#include "../assembler.h"
#include "../memory/memory.h"
#include "../symbol-table/symbol-table.h"
#include "../verbal-nouns/verbal-nouns.h"

#define DATA ".data"
#define STRING ".string"
#define EXTERN ".extern"
#define ENTRY ".entry"
#define CODE ".code"
#define SYMBOL_TEMP_ADDRESS -1

/* States for finding the correct stage of the codeFirstPass */
typedef enum STATES { VERBAL_NOUN, SRC, DEST } STATES;

/* Takes a fileName and a file extension and returns a ptr to the combined
 filename and FileExtension */
char *generateFileName(char *fileName, char *FileExtension);

/* Takes a string and returns a pointer to the malloced version of it with  */
char *mallocString(const char *string);

/* Codes the first pass and inserts it into the IC memory image array (removes
 * blank spaces for the second pass)*/
void codeFirstPass(char *trimLine, VerbalNoun *verbalNoun,
                   unsigned int lineCount, unsigned int *didFindErrors,
                   Memory *memoryPtr, int *IC, char *fileName);

/* Prints a generic register error */
void printErrorMessageRegister(unsigned int lineCount, char *trimLine,
                               unsigned int *didFindErrors, char *filename);
/* codes the first word of each line (verbal noun) and store it into memory */
void codeFirstWord(VerbalNoun *verbalNoun, char *trimLine, int *IC,
                   Memory *MemoryPtr, Word *firstWord);

/*Checks if an addressing mode is zero (by syntax) */
int isAddressingModeZero(char *trimLine, int trimLineLength);

/* Checks if addressing mode is two (by syntax)*/
int isAddressingModeTwo(char *trimLine);

/* Checks if addressing mode is three (by syntax)*/
int isAddressingModeThree(char *trimLine);

/* Checks if an addressing mode fits to the found verbal noun*/
int validateAddressingModeWithVerbalNoun(STATES currentElementState,
                                         VerbalNoun *currentVerbalNoun,
                                         int foundAddressingCode);
/* Validates that an addressing mode is zero */
int validateAddressingModeZero(char *trimLine, Bool *didFindErrors,
                               int lineCounter, char *fileName);

/* Prints a custom error by the found addressingMode and found verbal nouns*/
void printErrorMessageAddressingMode(unsigned int lineCount,
                                     unsigned int addressingMode,
                                     char *verbalNoun, STATES commandState,
                                     unsigned int *didFindErrors,
                                     char *fileName);

/* Checks if a given string is a valid register*/
int isValidRegister(char *trimLine);

/* Turn on a single bit in a given word*/
void turnOnWordBit(Word *word, unsigned int bitLocation);

/* print a word in binary (mostly for debugging reasons) */
void printWordInBinary(Word *word);

/* inserts an int (numToToggle) to a word after being shifted to the left x time
 * (startingRange)  */
void turnOnRangeInWord(Word *word, unsigned int numToToggle,
                       unsigned int startingRange);

/* This function codes and inserts to memory the known operands (all but labels
 * - addressing mode of 1) that are found within the first stage. Returns the
 * actual number of operands needed (can't be different from definition if found
 * 2 registers with addressingMode 2 or 3) or 0 if error is found */
int codeOperands(VerbalNoun *currentVerbalNoun, int numOfOperandsFound,
                 unsigned int srcAddressingMode,
                 unsigned int destAddressingMode, unsigned int *didFindErrors,
                 unsigned int lineCount, int *IC, Memory *MemoryPtr,
                 unsigned int registerCacheSrc, unsigned int registerCacheDest,
                 int numsCacheDest, int numsCacheSrc, char *fileName);

/* cleans a line from '\n' and '\t' for the assembler */
void cleanLineForAssembler(char *line);

/* checks whether a line is a label*/
int isLineLabel(char *trimLine);

/*Takes an int and a string and puts the octal representation of the given
 * string into the stringStore */
void convertNumToOctal(int num, char *stringStore);

/* Checks if a given string contains unwanted commas (double, trailing or
 * starting) */
Bool checkCommandSyntax(char *sentence, Bool isLineLabel, int lineCounter,
                        char *fileName);

/* validates each argument of the data command to make sure it has no unvalid
 * character (a-z) */
Bool validateData(char *trimLine);

/* validate the syntax of extern and entry definition */
Bool validateExternAndEntrySyntax(char *line, int lineCounter, char *fileName,
                                  Bool isExtern);
#endif
