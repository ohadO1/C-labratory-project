#ifndef FIRSTSTAGE_H
#define FIRSTSTAGE_H

#include "../helper-functions/helper-functions.h"

#define IN_FLAG 1
#define OUTSIDE_FLAG 0

/* Responsible for everything related to the first stage of the assembler.
 * iterates between lines. checks for errors, writing to memory and symbol
 * table, etc */
int firstStage(char *fileName, Memory *dcMemoryPtr, Memory *icMemoryPtr,
               SymbolNode **symbolTableHead, VerbalNounTable VerbalNounTable,
               int *IC, int *DC, unsigned int *didFindErrors);

/* Checks whether a string is data or string instruction (by syntax) */
int isDataOrStringInstruction(char *trimLine);

/* Checks wether a command is a data command (by syntax) */
int isCommandDataCommand(char *command);

/* Takes a data command and inserts it to memory, also scans for errors in
 * strings and numbers */
void insertDataToDcMemory(char *dataInstructionCommand, char *trimLine, int *DC,
                          int IC, Memory *memoryPtr, int lineCounter,
                          Bool *didFindErrors, char *fileName, char *fullLine,
                          Bool isLineLabel);

/* Checks whether a command is an entry command (by syntax)*/
int isEntry(char *trimLine);

/* Checks whether a command is an extern or entry command (by syntax)*/
int isExternOrEntryInstruction(char *trimLine);

/* Checks whether a command is an extern (by syntax)*/
int isExtern(char *trimLine);

/* Checks if IC + DC is bigger than memory size (if so exits immediately) */
void checkIfMemoryOverflow(int IC, int DC, char *fileName, int lineCounter);

#endif
