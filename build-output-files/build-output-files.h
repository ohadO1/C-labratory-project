#ifndef BUILD_OUTPUT_FILES_H
#define BUILD_OUTPUT_FILES_H

#include "../helper-functions/helper-functions.h"

/* Consts */
#define ENTRY_FILE_EXTENSION ".ent"
#define EXTERN_FILE_EXTENSION ".ext"
#define OBJ_FILE_NAME_EXTENSION ".ob"

/* Responsible for building output files, decoding words to octal, taking values
 * from memory, building and generating new filename, etc.. */
Bool buildOutputFiles(int IC, int DC, Memory *icMemoryPtr, Memory *dcMemoryPtr,
                      char *fileName, SymbolNode **symbolHeadPtr);

/* Responsible for building the object file */
Bool buildObjFile(int IC, int DC, Memory *icMemoryPtr, Memory *dcMemoryPtr,
                  char *fileName, FILE *objFile);

/* Builds the object file header by using IC and DC values */
Bool buildObjFileHeader(int IC, int DC, char *fileName, char *firstLine,
                        FILE *objFile);

/* Checks if there is a need to build the ext and ent files, if retruns false
 * then files will not be built (checks if the types are existed in the current
 * state of the symbol table)*/
Bool areThereSymbolsWithExtension(SymbolNode **symbolHeadPtr,
                                  char *searchedType);

#endif
