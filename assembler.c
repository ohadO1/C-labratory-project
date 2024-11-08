#include "assembler.h"
#include "build-output-files/build-output-files.h"
#include "pre-assembler/pre-assembler.h"
#include "second-stage/second-stage.h"

int main(int argc, char **argv) {
  int i, IC = 0, DC = 0;
  MacroNode *macroTableHead;
  SymbolNode *symbolTableHead;
  VerbalNounTable *verbalNounPtr;
  Bool didFindErrors = False;

  /** Ic + Dc max value should equal to the overall memory size which is 4096
   * words, so I gave each array the initial memory size and if ic length +  dc
   * length is bigger than the memory size, an error will be thrown */
  Memory icMemory[MEMORY_SIZE] = {0};
  Memory dcMemory[MEMORY_SIZE] = {0};

  initializeVerbalNounTable(&verbalNounPtr);

  for (i = 1; i < argc; i++) {
    preAssembler(argv[i], &macroTableHead, *verbalNounPtr, &didFindErrors);
    freeMacroList(macroTableHead);
    macroTableHead = NULL;

    if (didFindErrors) {
      printf("ERRORS found on pre assembler stage. exiting "
             "now...\n");
      return ERROR_CODE;
    }

    symbolTableHead = NULL;

    firstStage(argv[i], dcMemory, icMemory, &symbolTableHead, *verbalNounPtr,
               &IC, &DC, &didFindErrors);

    if (didFindErrors) {
      printf("ERRORS found on first stage of the assembler. exiting "
             "now...\n");
      return ERROR_CODE;
    }

    /* reset IC for recounting in second pass */
    IC = 0;

    secondStage(argv[i], dcMemory, icMemory, &symbolTableHead, *verbalNounPtr,
                &IC, &DC, &didFindErrors);

    if (didFindErrors) {
      printf("ERRORS were found on second stage. exiting...\n");
      return ERROR_CODE;
    }

    if (IC || DC) {
      /* for debugging memory after assembler runs - uncomment to see binary
       * representation of each word */

      /* printMemory(icMemory, "IC"); */
      /* printMemory(dcMemory, "DC"); */
      /* printSymbols(symbolTableHead); */
      buildOutputFiles(IC, DC, icMemory, dcMemory, argv[i], &symbolTableHead);
    }

    /* restart assembler for next file */
    freeSymbolList(symbolTableHead);
    symbolTableHead = NULL;

    cleanMemory(icMemory, &IC);
    cleanMemory(dcMemory, &DC);

    IC = 0;
    DC = 0;
  }

  return SUCCESS_CODE;
}
