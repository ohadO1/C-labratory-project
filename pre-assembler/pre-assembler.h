#include "../helper-functions/helper-functions.h"
#include "../macro-table/macro-table.h"

#ifndef PRE_ASSEMBLER
#define PRE_ASSEMBLER

#define INSIDE_MACRO 0
#define OUTSIDE_MACRO 1
#define INITIAL_MACRO_SIZE 15
#define PRE_ASSEMBLER_CHECK_LEN 100

/** checks if a given line is inside the macro list */
int isMacroDefinition(const char *line);

/** checks if macr definition is valid */
Bool scanMacroSyntax(int lineCounter, char *fileName, char *macroName);

/** checks if endmacr definition is valid  */
Bool scanEndMacroSyntax(int lineCounter, char *fileName,
                        char *lineStartingPoint);

/** checks if the macro reached to its end */
int isEndMacro(const char *command, char macroContainer[100]);

/** process data inside the macro*/
Bool processInsideMacro(char **macroContainer, FILE *readFile,
                        int *currentMacroLen, char *lineStartingPoint,
                        char *fileName, int lineCounter);

/**  takes an assembly txt file, compiles it into a an am files with spreaded
 * macro */
int preAssembler(char *fileName, MacroNode **macroTableHead,
                 VerbalNounTable VerbalNounPtr, unsigned int *didFindPtr);

/* Validates if a suspected macro name is not a register name of a verbal noun
 * name, if so returns false*/
Bool validateMacroDefinition(MacroNode *macroTableHead, char *macroName,
                             unsigned int *didfindErrors,
                             VerbalNounTable VerbalNounTablePtr,
                             int lineCounter, char *fileName, char *line);

#endif
