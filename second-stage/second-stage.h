#ifndef SECOND_STAGE_H
#define SECOND_STAGE_H

#include "../first-stage/first-stage.h"

/* Handling the second stage. Will toggle didFindErrors if found an error */
Bool secondStage(char *fileName, Memory *dcMemoryPtr, Memory *icMemoryPtr,
                 SymbolNode **SymbolTableHead, VerbalNounTable verbalNounTable,
                 int *IC, int *DC, Bool *didFindErrors);

/* Completes the symboltable and codes the data to be in the right order*/
Bool codeSecondPass(char *trimLine, int *IC, SymbolNode **SymbolTableHead,
                    Memory *memoryPtr, unsigned int *preAddressingMode,
                    char *fileName, unsigned int lineNumber);

/* Updates DC addresses to be in the correct address (after IC)*/
void updateAddresses(int DC, int IC, Memory *memoryPtr, Memory *dcMemoryPtr);

#endif
