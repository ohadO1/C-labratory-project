#include "second-stage.h"

Bool secondStage(char *fileName, Memory *dcMemoryPtr, Memory *icMemoryPtr,
                 SymbolNode **SymbolTableHead, VerbalNounTable verbalNounTable,
                 int *IC, int *DC, Bool *didFindErrors) {
  FILE *readFile;
  char line[MAX_LINE_LENGTH], tokenizedLine[MAX_LINE_LENGTH], *updatedFileName,
      *trimLine;
  unsigned int lineCounter = 0;
  unsigned int prevAddressingMode;

  updatedFileName = generateFileName(fileName, PRE_ASSEMBLER_FILE_EXTENSION);

  readFile = fopen(updatedFileName, "r");

  if (readFile == NULL) {
    perror("Error opening file");
    return ERROR_CODE;
  }

  while (fgets(line, sizeof(line), readFile) != NULL) {
    prevAddressingMode = 10;
    lineCounter++;
    cleanLineForAssembler(line);
    strcpy(tokenizedLine, line);
    trimLine = strtok(tokenizedLine, " ");

    if (isLineLabel(trimLine)) {
      trimLine = strtok(NULL, " ");
    }

    if (isDataOrStringInstruction(trimLine) || isExtern(trimLine)) {
      continue;
    }

    if (isEntry(trimLine)) {
      trimLine = strtok(NULL, " ");
      updateSymbolToEntry(*SymbolTableHead, trimLine, fileName, lineCounter,
                          didFindErrors);
    } else {
      if (!codeSecondPass(trimLine, IC, SymbolTableHead, icMemoryPtr,
                          &prevAddressingMode, fileName, lineCounter)) {
        return False;
      }
    }
  }
  if (!*didFindErrors) {
    updateAddresses(*DC, *IC, icMemoryPtr, dcMemoryPtr);
  } else {
    free(updatedFileName);
    return False;
  }
  free(updatedFileName);
  return True;
}

Bool codeSecondPass(char *trimLine, int *IC, SymbolNode **symbolTableHead,
                    Memory *memoryPtr, unsigned int *prevAddressingMode,
                    char *fileName, unsigned int lineNumber) {
  Word wordToInsert = {0};
  unsigned int trimLineLen = strlen(trimLine);
  SymbolNode *foundSymbol;
  *IC += 1;
  while ((trimLine = strtok(NULL, " ,\n")) != NULL) {
    if (isAddressingModeZero(trimLine, trimLineLen)) {
      *IC += 1;
      *prevAddressingMode = 0;
      continue;
    } else if (isAddressingModeTwo(trimLine) ||
               isAddressingModeThree(trimLine)) {
      /* checking if both src and dest are registers, if so add one instead of
       * 2 operands */
      if (*prevAddressingMode == 2) {
        *prevAddressingMode = 2;
        return True;
      } else {
        *IC += 1;
      }
      *prevAddressingMode = 2;
    } else {
      /* addressingMode is 1 */
      *prevAddressingMode = 1;
      foundSymbol = searchsymbol(*symbolTableHead, trimLine);
      if (!foundSymbol) {
        printf("ERROR in line %d of file %s: the symbol %s wasn't defined in "
               "the symbol "
               "table. make sure that you declared it.\n",
               lineNumber, fileName, trimLine);
        return False;
      }
      if (strcmp(EXTERN, foundSymbol->symbol.type) == 0) {
        insertSymbol(symbolTableHead, foundSymbol->symbol.label, *IC + 100,
                     EXTERN, fileName);
        turnOnWordBit(&wordToInsert, 0);
      } else {
        turnOnRangeInWord(&wordToInsert, foundSymbol->symbol.decimalAddress, 3);
        turnOnWordBit(&wordToInsert, 1);
      }
      insertToMemory(memoryPtr, IC, wordToInsert.value, UPDATE_MODE);
      *IC += 1;
    }
  }
  return True;
}

void updateAddresses(int DC, int IC, Memory *icMemoryPtr, Memory *dcMemoryPtr) {
  int i;
  for (i = 0; i < DC; i++) {
    dcMemoryPtr[i + 100].addressDecimal = 100 + i + IC;
  }

  for (i = 0; i < IC; i++) {
    icMemoryPtr[i + 100].addressDecimal = i + 100;
  }
}
