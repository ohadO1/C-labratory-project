#include "./pre-assembler.h"
#include <stdio.h>

int isMacroDefinition(const char *command) {
  return strcmp(command, "macr") == 0;
}

int isEndMacro(const char *command, char macroContainer[100]) {
  return strcmp(command, "endmacr") == 0;
}

Bool scanMacroSyntax(int lineCounter, char *fileName, char *macroNameLine) {
  int i = 0;

  /* skip macr definition */
  for (; macroNameLine[i] != ' ' && macroNameLine[i] != '\t' &&
         macroNameLine[i] != '\n';
       i++) {
  }

  /* skip spaces and tabs */
  for (; macroNameLine[i] == '\t' || macroNameLine[i] == ' ' ||
         macroNameLine[i] == '\n';
       i++)
    ;

  if (macroNameLine[i] == '\0') {
    printf("ERROR in line %d of file %s: declared a macro with no name\n",
           lineCounter, fileName);
    return False;
  }

  /* skip macro name to check if has valid syntax */
  for (; macroNameLine[i] != ' ' && macroNameLine[i] != '\t' &&
         macroNameLine[i] != '\n';
       i++)
    ;

  /* skip spaces and tabs */
  for (; macroNameLine[i] == '\t' || macroNameLine[i] == ' ' ||
         macroNameLine[i] == '\n';
       i++)
    ;

  /* means that are non space letters that exist after macro
   * definition*/
  if (macroNameLine[i] != '\0') {
    printf("ERROR in line %d of file %s: found extraneous character '%c' after "
           " macro definition\n",
           lineCounter, fileName, macroNameLine[i]);
    return False;
  }
  return True;
}

Bool scanEndMacroSyntax(int lineCounter, char *fileName,
                        char *lineStartingPoint) {
  int i;

  /* skip endmacr */
  for (i = 0; ((lineStartingPoint[i]) >= 'a' && (lineStartingPoint[i] <= 'z'));
       i++)
    ;

  if (lineStartingPoint[i] == '\0') {
    return True;
  }

  /* skip spaces */
  for (; lineStartingPoint[i] == ' ' || lineStartingPoint[i] == '\n' ||
         lineStartingPoint[i] == '\t';
       i++)
    ;

  if (lineStartingPoint[i] != '\0') {
    printf("ERROR in line %d of file %s: found extraneous character '%c' after "
           " endmacr definition\n",
           lineCounter, fileName, lineStartingPoint[i]);
    return False;
  }
  return True;
}

Bool validateMacroDefinition(MacroNode *macroTableHead, char *macroName,
                             unsigned int *didfindErrors,
                             VerbalNounTable VerbalNounTablePtr,
                             int lineCounter, char *fileName, char *line) {

  if (!scanMacroSyntax(lineCounter, fileName, line)) {
    *didfindErrors = 1;
    return False;
  };

  /* checking logic */
  if (findVerbalNounInTable(VerbalNounTablePtr, macroName) != NULL ||
      isValidRegister(macroName) != ERROR_CODE) {
    printf("ERROR in line %d of file %s: The macro name %s is a register or a "
           "verbal noun\n",
           lineCounter, fileName, macroName);
    *didfindErrors = True;
    return False;
  } else if (searchMacro(macroTableHead, macroName) != NULL) {
    printf("ERROR in line %d of file %s: You already declared a macro with the "
           "macro name of %s\n",
           lineCounter, fileName, macroName);
    *didfindErrors = True;
    return False;
  } else {
    return True;
  }
}

Bool processInsideMacro(char **macroContainer, FILE *readFile,
                        int *currentMacroLen, char *lineStartingPoint,
                        char *fileName, int lineCounter) {
  if (strlen(lineStartingPoint) + strlen(*macroContainer) >
      MACRO_BODY_MAX_LEN) {
    printf("ERROR in line %d of file %s: macro body length exceedes the max "
           "macro body length allowed\n",
           lineCounter, fileName);
    return False;
  } else if (strlen(lineStartingPoint) + strlen(*macroContainer) >=
             *currentMacroLen - 1) {
    *macroContainer = realloc(*macroContainer, *currentMacroLen * 2);
    if (macroContainer == NULL) {
      printf("ERROR in line %d of file %s: could not expand memory\n",
             lineCounter, fileName);
    }
    *currentMacroLen *= 2;
  }

  strcat(*macroContainer, lineStartingPoint);
  return True;
}

int preAssembler(char *fileName, MacroNode **macroTableHead,
                 VerbalNounTable VerbalNounTablePtr,
                 unsigned int *didfindErrors) {
  int isInMacro = OUTSIDE_MACRO, currentMacroLen = INITIAL_MACRO_SIZE,
      lineCounter = 0;
  FILE *readFile, *writeFile;
  char line[PRE_ASSEMBLER_CHECK_LEN], tokenizedLine[MAX_LINE_LENGTH],
      *readFileWithExtension, *command, *writtenFileName, *lineStartingPoint,
      macroName[MAX_MACRO_NAME_LEN] = {0}, *macroBody,
      *suspectedMacroDefinition;
  MacroNode *foundHead;

  readFileWithExtension = generateFileName(fileName, INPUT_FILE_EXTENSION);

  readFile = fopen(readFileWithExtension, "r");
  if (readFile == NULL) {
    perror("Error opening file");
    return ERROR_CODE;
  }

  writtenFileName = generateFileName(fileName, PRE_ASSEMBLER_FILE_EXTENSION);
  writeFile = fopen(writtenFileName, "w");
  if (writeFile == NULL) {
    perror("Error creating am file.\n");
    return ERROR_CODE;
  }

  while (fgets(line, sizeof(line), readFile) != NULL) {
    lineCounter++;
    if (strlen(line) > MAX_LINE_LENGTH - 1) {
      printf("ERROR in line %d: command is longer than the max line length (82 "
             "including null value)\n",
             lineCounter);
      *didfindErrors = True;
      return ERROR_CODE;
    }
    strcpy(tokenizedLine, line);

    lineStartingPoint = line;
    while (*lineStartingPoint == ' ' || *lineStartingPoint == '\t') {
      lineStartingPoint++;
    }
    command = strtok(tokenizedLine, " \n\t");
    /** skip comments and empty lines */
    if (*lineStartingPoint == ';' || *lineStartingPoint == '\0' ||
        *lineStartingPoint == '\n') {
      continue;
    }

    if (command == NULL) {
      continue;
    }

    if (isInMacro == INSIDE_MACRO) {
      if (isEndMacro(command, macroBody)) {
        if (!scanEndMacroSyntax(lineCounter, fileName, lineStartingPoint)) {
          *didfindErrors = True;
          return False;
        }
        isInMacro = OUTSIDE_MACRO;
        insertMacro(macroTableHead, macroName, macroBody);

        free(macroBody);
        macroBody = (char *)malloc(sizeof(char) * INITIAL_MACRO_SIZE);
        currentMacroLen = INITIAL_MACRO_SIZE;
        continue;
      }
      if (!processInsideMacro(&macroBody, readFile, &currentMacroLen,
                              lineStartingPoint, fileName, lineCounter)) {
        return False;
      };
    } else {
      if (isMacroDefinition(command)) {
        isInMacro = INSIDE_MACRO;
        suspectedMacroDefinition = strtok(NULL, "\n");
        if (!validateMacroDefinition(
                *macroTableHead, suspectedMacroDefinition, didfindErrors,
                VerbalNounTablePtr, lineCounter, fileName, lineStartingPoint)) {
          return False;
        }

        if (strlen(suspectedMacroDefinition) >= MAX_MACRO_NAME_LEN) {
          printf("ERORR in line %d of file %s: the macro name %s is above the "
                 "max length %d\n",
                 lineCounter, fileName, suspectedMacroDefinition,
                 MAX_MACRO_NAME_LEN);
          return False;
        }

        strcpy(macroName, suspectedMacroDefinition);

        /* allocate memory for new macro */
        macroBody = (char *)malloc(sizeof(char) * INITIAL_MACRO_SIZE);
        /* initialize macro body to be an empty string */
        strcpy(macroBody, "");
        continue;
      } else if ((foundHead = searchMacro(*macroTableHead, command))) {
        fputs(foundHead->macro.body, writeFile);
      } else {
        fputs(lineStartingPoint, writeFile);
      }
    }
  }

  free(readFileWithExtension);
  free(writtenFileName);
  fclose(writeFile);
  fclose(readFile);

  return True;
}
