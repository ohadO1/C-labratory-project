#ifndef SYMBOL_TABLE
#define SYMBOL_TABLE

#include "../assembler.h"
#include "stdio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Consts */
#define MAX_TYPE_LEN 10
#define DUPLICATED_LABEL_ERROR_CODE 1
#define ENTRY_NOT_SAME_FILE_ERROR 1

/* Each symbol contains a label, decimal address and a type*/
typedef struct Symbol {
  char label[MAX_LABEL_LENGTH];
  int decimalAddress;
  char type[MAX_TYPE_LEN];
} SymbolTable;

/* The struct of a symbol node, it has a node and a pointer to the next symbol
 * on the list */
typedef struct SymbolNode {
  SymbolTable symbol;
  struct SymbolNode *next;
} SymbolNode;

/* Function to create a new node */
SymbolNode *createSymbolNode(const char *label, int decimalAddress, char *type,
                             char *fileName);

/* Validates if an entry defition is fine to insert into the symbolTable,
 * adjusts didFindErrors if invalidity is found */
void validateEntrySymbolBeforeInsertion(SymbolNode *head, const char *label,
                                        char *fileName, char *type,
                                        unsigned int lineCounter,
                                        Bool *didFindErrorsFlag);

/** Function to insert a symbol into the linked list */
void insertSymbol(SymbolNode **head, const char *label, int decimalAddress,
                  char *type, char *fileName);

/** Looks up for a symbol, if found returns the node, otherwise Null */
SymbolNode *searchsymbol(SymbolNode *head, const char *label);

/** After first stage, runs on symbol table and update to the correct memory
 * address (IC + 100 + current)  */
void updateSymbolTableValue(SymbolNode **symbolHeadPtr, int *IC, char *type);

/** Prints the nodes of the symbol linked list */
void printSymbols(SymbolNode *head);

/** Frees the symbolList memory*/
void freeSymbolList(SymbolNode *head);

/* Connects label from the symbol table to its entry node, also validates it */
void updateSymbolToEntry(SymbolNode *head, char *label, char *fileName,
                         int lineCounter, Bool *didFindErrorsFlag);

/* Validates that there are no duplications while inserting to the symbol table
 */
Bool validateFirstStageSymbolBeforeInsertion(SymbolNode *head,
                                             const char *label, char *fileName,
                                             unsigned int lineCounter,
                                             Bool *didFindErrorsFlag);

/* Takes a file and writes to it instances from a certain file (for ouput files)
 * for example: ent, ext files */
void codeSpecificExtensionSymbol(SymbolNode **symbolHeadPtr, char *searchedType,
                                 FILE *file);

#endif
