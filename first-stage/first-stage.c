#include "./first-stage.h"

int firstStage(char *fileName, Memory *dcMemoryPtr, Memory *icMemoryPtr,
               SymbolNode **SymbolTableHead, VerbalNounTable verbalNounTable,
               int *IC, int *DC, unsigned int *didFindErrors) {
  unsigned int lineCounter = 0, isLabelFlag = OUTSIDE_FLAG;
  FILE *readFile;
  char line[MAX_LINE_LENGTH], tokenizedLine[MAX_LINE_LENGTH], *updatedFileName,
      label[MAX_LABEL_LENGTH], *trimLine, *dataInstructionCommand;
  VerbalNoun *currentVerbalNoun;

  updatedFileName = generateFileName(fileName, PRE_ASSEMBLER_FILE_EXTENSION);

  readFile = fopen(updatedFileName, "r");

  if (readFile == NULL) {
    perror("Error opening file");
    return ERROR_CODE;
  }

  while (fgets(line, sizeof(line), readFile) != NULL) {
    lineCounter++;
    cleanLineForAssembler(line);

    strcpy(tokenizedLine, line);

    trimLine = strtok(tokenizedLine, " ");

    if (isLineLabel(trimLine)) {
      trimLine[strlen(trimLine) - 1] = '\0';
      if (findVerbalNounInTable(verbalNounTable, trimLine) != NULL ||
          isValidRegister(trimLine) != ERROR_CODE) {
        printf("ERROR in line %d of file %s: label can't be a register name or "
               "a verbal noun \n",
               lineCounter, fileName);
        continue;
      }
      strcpy(label, trimLine);
      trimLine = strtok(NULL, " ");
      if (trimLine == NULL) {
        printf("ERROR in line %d of file %s: found an empty label definition\n",
               lineCounter, fileName);
        *didFindErrors = True;
        continue;
      }
      isLabelFlag = IN_FLAG;
    }

    if (isDataOrStringInstruction(trimLine)) {
      if (isLabelFlag == IN_FLAG &&
          validateFirstStageSymbolBeforeInsertion(
              *SymbolTableHead, label, fileName, lineCounter, didFindErrors)) {
        insertSymbol(SymbolTableHead, label, *DC, DATA, fileName);
      }
      dataInstructionCommand = mallocString(trimLine);

      insertDataToDcMemory(dataInstructionCommand, trimLine, DC, *IC,
                           dcMemoryPtr, lineCounter, didFindErrors, fileName,
                           line, isLabelFlag);

      free(dataInstructionCommand);

      isLabelFlag = OUTSIDE_FLAG;
    }

    else if (isExternOrEntryInstruction(trimLine)) {
      if (isLabelFlag == IN_FLAG) {
        printf("WARNING at line %d of file %s: You should not define a label "
               "for an "
               "extern or entry symbol, "
               "the assembler will ignore this label! \n",
               lineCounter, fileName);
        isLabelFlag = OUTSIDE_FLAG;
      }
      if (isExtern(trimLine)) {
        /* validate extern */
        if (!validateExternAndEntrySyntax(line, lineCounter, fileName, True)) {
          *didFindErrors = True;
          continue;
        }
        trimLine = strtok(NULL, " ");
        if (validateFirstStageSymbolBeforeInsertion(*SymbolTableHead, trimLine,
                                                    fileName, lineCounter,
                                                    didFindErrors)) {
          insertSymbol(SymbolTableHead, trimLine, SYMBOL_TEMP_ADDRESS, EXTERN,
                       fileName);
        }
      } else {
        /* validate entry*/
        if (!validateExternAndEntrySyntax(line, lineCounter, fileName, False)) {
          *didFindErrors = True;
          continue;
        }
      }

    } else {
      /* in command sentence */
      if (!checkCommandSyntax(line, isLabelFlag, lineCounter, fileName)) {
        *didFindErrors = True;
        continue;
      }

      if ((currentVerbalNoun =
               findVerbalNounInTable(verbalNounTable, trimLine)) != NULL) {

        if (isLabelFlag == IN_FLAG && validateFirstStageSymbolBeforeInsertion(
                                          *SymbolTableHead, label, fileName,
                                          lineCounter, didFindErrors)) {
          insertSymbol(SymbolTableHead, label, *IC + 100, CODE, fileName);
        }

        codeFirstPass(trimLine, currentVerbalNoun, lineCounter, didFindErrors,
                      icMemoryPtr, IC, fileName);
      } else {
        printf("ERROR in line %d of file: %s didnt find any verbal noun that "
               "fits to the "
               "required action. the unrecognized verb noun: %s \nexiting...\n",
               lineCounter, fileName, trimLine);
        *didFindErrors = True;
      }
    }

    isLabelFlag = OUTSIDE_FLAG;
  }

  if (*didFindErrors)
    return ERROR_CODE;

  updateSymbolTableValue(SymbolTableHead, IC, DATA);

  return SUCCESS_CODE;
}

void insertDataToDcMemory(char *dataInstructionCommand, char *trimLine, int *DC,
                          int IC, Memory *memoryPtr, int lineCounter,
                          Bool *didFindErrors, char *fileName, char *fullLine,
                          Bool isLineLabel) {
  int i;

  if (isCommandDataCommand(dataInstructionCommand)) {

    if (!checkCommandSyntax(fullLine, isLineLabel, lineCounter, fileName)) {

      *didFindErrors = True;
      return;
    }

    while ((trimLine = strtok(NULL, ",")) != NULL) {

      if (!validateData(trimLine)) {
        *didFindErrors = True;
        printf("ERROR in line %d of file %s: unvalid characters while "
               "defining data at %s\n",
               lineCounter, fileName, trimLine);
        return;
      };

      checkIfMemoryOverflow(IC, *DC, fileName, lineCounter);
      insertToMemory(memoryPtr, DC, atoi(trimLine), INSERT_MODE);
    }
  } else {
    /* in string definition */
    i = 0;
    trimLine = strtok(NULL, " ");
    while (*trimLine == ' ')
      i++;
    ;

    if (trimLine[i] != '"') {
      printf("ERROR in line %d of file: %s: missing opening quotes\n",
             lineCounter, fileName);
      *didFindErrors = True;
      return;
    }

    i++;

    for (; trimLine[i] != '"' && trimLine[i] != '\0'; i++) {
      if ((trimLine[i] < 'a' || trimLine[i] > 'z') &&
          (trimLine[i] < 'A' || trimLine[i] > 'Z')) {
        printf("ERROR in line %d of file %s: Invalid string\n", lineCounter,
               fileName);
        *didFindErrors = True;
        return;
      } else {
        checkIfMemoryOverflow(IC, *DC, fileName, lineCounter);
        insertToMemory(memoryPtr, DC, trimLine[i], INSERT_MODE);
      }
    }

    /* means that reached null, no closing quotes */
    if (trimLine[i] != '"') {
      printf("ERROR in line %d of file %s: invalid space or missing closing "
             "quotes\n",
             lineCounter, fileName);
      *didFindErrors = True;
      return;
    }

    i++;

    trimLine = strtok(NULL, "\0");
    if (trimLine != NULL) {
      for (i = 0; trimLine[i] != '\0'; i++) {
        if (trimLine[i] != ' ' && trimLine[i] != '\n') {
          printf("ERROR in line %d of file %s: cannot write extra characters "
                 "after finishing string definition\n",
                 lineCounter, fileName);
          *didFindErrors = True;
          return;
        }
      }
    }
    /** insert null */
    checkIfMemoryOverflow(IC, *DC, fileName, lineCounter);
    insertToMemory(memoryPtr, DC, 0, INSERT_MODE);
  }
}

int isDataOrStringInstruction(char *trimLine) {
  return strcmp(trimLine, ".data") == 0 || strcmp(trimLine, ".string") == 0;
}

int isExternOrEntryInstruction(char *trimLine) {
  return strcmp(trimLine, ENTRY) == 0 || strcmp(trimLine, EXTERN) == 0;
}

void checkIfMemoryOverflow(int IC, int DC, char *fileName, int lineCounter) {
  if (IC + DC + 100 > MEMORY_SIZE) {
    printf("ERROR in line %d of file %s: reached memory overflow!\n",
           lineCounter, fileName);

    exit(ERROR_CODE);
  }
}

int isExtern(char *trimLine) { return strcmp(trimLine, EXTERN) == 0; }

int isEntry(char *trimLine) { return strcmp(trimLine, ENTRY) == 0; }

int isCommandDataCommand(char *command) { return strcmp(command, DATA) == 0; }
