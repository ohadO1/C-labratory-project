#include "helper-functions.h"

char *generateFileName(char *originalFileName, char *extension) {
  size_t newFileNameLength = strlen(originalFileName) + strlen(extension) + 1;
  char *readFileWithExtension = (char *)malloc(newFileNameLength);
  if (readFileWithExtension == NULL) {
    printf("Failed to allocate memory\n");
    return NULL;
  }
  strcpy(readFileWithExtension, originalFileName);
  strcat(readFileWithExtension, extension);
  return readFileWithExtension;
}

char *mallocString(const char *string) {
  /* size of char is one so no need for multiplication*/
  char *updatedString = malloc(strlen(string) + 1);

  if (updatedString == NULL) {
    printf("cannot allocate memory for string\n");
    return NULL;
  }
  strcpy(updatedString, string);
  return updatedString;
}

int isValidRegister(char *trimLine) {
  if (strlen(trimLine) == 2 && trimLine[0] == 'r' && trimLine[1] <= '7' &&
      trimLine[1] >= '0') {
    /* return register number */
    return trimLine[1] - '0';
  } else
    /* returning error code because if r0 is selected a zero (fasley value will
     * be returned) */
    return ERROR_CODE;
}

void codeFirstPass(char *trimLine, VerbalNoun *verbalNoun,
                   unsigned int lineCount, unsigned int *didFindErrors,
                   Memory *memoryPtr, int *IC, char *fileName) {
  Word firstWord = {0};
  int trimLineLength;
  unsigned int numOfOperandsFound = 0, sourceOperandAddressingCode = -1,
               registerCacheSrc = -1, registerCacheDest = -1, numCacheSrc = 0,
               numCacheDest = 0, numCacheTemp = 0, registerCacheTemp = -1,
               destOperandAddressingCode = -1;
  STATES commandState = VERBAL_NOUN;

  if (verbalNoun->numOfOperands == 1) {
    commandState++;
  }
  /** 5 and not 4 for the null character */
  while ((trimLine = strtok(NULL, " ,\n")) != NULL) {
    commandState++;
    numOfOperandsFound++;
    trimLineLength = strlen(trimLine);
    if (isAddressingModeZero(trimLine, trimLineLength)) {
      if (!validateAddressingModeWithVerbalNoun(commandState, verbalNoun, 0)) {
        printErrorMessageAddressingMode(lineCount, 0, verbalNoun->verbalNoun,
                                        commandState, didFindErrors, fileName);
      }
      numCacheTemp = validateAddressingModeZero(trimLine, didFindErrors,
                                                lineCount, fileName);

      if (commandState == SRC) {
        numCacheSrc = numCacheTemp;
        sourceOperandAddressingCode = 0;
      } else if (commandState == DEST) {
        destOperandAddressingCode = 0;
        numCacheDest = numCacheTemp;
      }

      turnOnWordBit(&firstWord, commandState == SRC ? 7 : 3);

    } else if (isAddressingModeTwo(trimLine)) {
      if ((registerCacheTemp = isValidRegister(&(trimLine[1]))) == ERROR_CODE) {
        printErrorMessageRegister(lineCount, trimLine, didFindErrors, fileName);
      };
      if (!validateAddressingModeWithVerbalNoun(commandState, verbalNoun, 2)) {
        printErrorMessageAddressingMode(lineCount, 2, verbalNoun->verbalNoun,
                                        commandState, didFindErrors, fileName);
      }

      if (commandState == SRC) {
        sourceOperandAddressingCode = 2;
        registerCacheSrc = registerCacheTemp;
      } else {
        /* means destination */
        destOperandAddressingCode = 2;
        registerCacheDest = registerCacheTemp;
      }

      turnOnWordBit(&firstWord, commandState == SRC ? 9 : 5);

    } else if (isAddressingModeThree(trimLine)) {
      if ((registerCacheTemp = isValidRegister(trimLine)) == ERROR_CODE) {
        printErrorMessageRegister(lineCount, trimLine, didFindErrors, fileName);
      };
      if (!validateAddressingModeWithVerbalNoun(commandState, verbalNoun, 3)) {
        printErrorMessageAddressingMode(lineCount, 3, verbalNoun->verbalNoun,
                                        commandState, didFindErrors, fileName);
        *didFindErrors = True;
      }

      if (commandState == SRC) {
        sourceOperandAddressingCode = 3;
        registerCacheSrc = registerCacheTemp;
      } else {
        destOperandAddressingCode = 3;
        registerCacheDest = registerCacheTemp;
      }
      turnOnWordBit(&firstWord, commandState == SRC ? 10 : 6);
    } else {
      /* addressingMode is one */
      if (!validateAddressingModeWithVerbalNoun(commandState, verbalNoun, 1)) {
        printErrorMessageAddressingMode(lineCount, 1, verbalNoun->verbalNoun,
                                        commandState, didFindErrors, fileName);
      }

      if (commandState == SRC)
        sourceOperandAddressingCode = 1;
      else if (commandState == DEST)
        destOperandAddressingCode = 1;
      turnOnWordBit(&firstWord, commandState == SRC ? 8 : 4);
    }
  }

  codeFirstWord(verbalNoun, trimLine, IC, memoryPtr, &firstWord);

  codeOperands(verbalNoun, numOfOperandsFound, sourceOperandAddressingCode,
               destOperandAddressingCode, didFindErrors, lineCount, IC,
               memoryPtr, registerCacheSrc, registerCacheDest, numCacheSrc,
               numCacheDest, fileName);
}

int codeOperands(VerbalNoun *currentVerbalNoun, int numOfOperandsFound,
                 unsigned int srcAddressingMode,
                 unsigned int destAddressingMode, unsigned int *didFindErrors,
                 unsigned int lineCount, int *IC, Memory *memoryPtr,
                 unsigned int registerCacheSrc, unsigned int registerCacheDest,
                 int numCacheSrc, int numCacheDest, char *fileName) {

  Word srcOperandWord = {0}, destOperandWord = {0};

  if (numOfOperandsFound != currentVerbalNoun->numOfOperands) {
    printf("ERROR in line %u of file: %s: command %s requires %u operands. "
           "Found %u "
           "%s instead.\n",
           lineCount, fileName, currentVerbalNoun->verbalNoun,
           currentVerbalNoun->numOfOperands, numOfOperandsFound,
           numOfOperandsFound == 1 ? "operand" : "operands");
    *didFindErrors = True;
    return False;
  }
  if (srcAddressingMode == 2 || destAddressingMode == 2 ||
      srcAddressingMode == 3 || destAddressingMode == 3) {

    /* if addressingMode is 2 or 3 for both(means they both registers so they
     * need to split one word)*/
    if ((srcAddressingMode == 2 || srcAddressingMode == 3) &&
        (destAddressingMode == 2 || destAddressingMode == 3)) {
      turnOnRangeInWord(&srcOperandWord, registerCacheDest, 3);
      turnOnRangeInWord(&srcOperandWord, registerCacheSrc, 6);
      turnOnWordBit(&srcOperandWord, 2);
      insertToMemory(memoryPtr, IC, srcOperandWord.value, INSERT_MODE);
      return True;

    } else if (srcAddressingMode == 2 || srcAddressingMode == 3) {
      turnOnWordBit(&srcOperandWord, 2);
      turnOnRangeInWord(&srcOperandWord, registerCacheSrc, 6);
    } else if (destAddressingMode == 2 || destAddressingMode == 3) {
      turnOnWordBit(&destOperandWord, 2);
      turnOnRangeInWord(&destOperandWord, registerCacheDest, 3);
    }
  }

  if (srcAddressingMode == 0 || destAddressingMode == 0) {

    if (srcAddressingMode == 0) {
      turnOnWordBit(&srcOperandWord, 2);
      turnOnRangeInWord(&srcOperandWord, numCacheSrc, 3);
    }
    if (destAddressingMode == 0) {
      turnOnWordBit(&destOperandWord, 2);
      turnOnRangeInWord(&destOperandWord, numCacheDest, 3);
    }
  }

  if (srcAddressingMode == 1)
    *IC += 1;

  else if (srcAddressingMode != -1)
    insertToMemory(memoryPtr, IC, srcOperandWord.value, INSERT_MODE);

  if (destAddressingMode == 1)
    *IC += 1;

  else if (destAddressingMode != -1)
    insertToMemory(memoryPtr, IC, destOperandWord.value, INSERT_MODE);

  return True;
}

int validateAddressingModeWithVerbalNoun(STATES currentElementState,
                                         VerbalNoun *currentVerbalNoun,
                                         int foundAddressingCode) {
  int i;
  for (i = 0; i < 5; i++) {
    if (currentElementState == DEST &&
        currentVerbalNoun->operandsNumsDest[i] == foundAddressingCode) {
      return True;
    } else if (currentElementState == SRC &&
               currentVerbalNoun->operandsNumsSource[i] ==
                   foundAddressingCode) {
      return True;
    }
  }
  return False;
}

int isAddressingModeThree(char *trimLine) {
  int strLen = strlen(trimLine);
  return strLen <= 2 && trimLine[0] == 'r' &&
         ((trimLine[1] >= '0' && trimLine[1] <= '9') || trimLine[1] == '+' ||
          trimLine[1] == '-');
}

int isAddressingModeTwo(char *trimLine) {
  return trimLine[0] == '*' && trimLine[1] == 'r';
}

int isAddressingModeZero(char *trimLine, int trimLineLength) {
  return trimLine[0] == '#';
}

void codeFirstWord(VerbalNoun *verbalNoun, char *trimLine, int *IC,
                   Memory *memoryPtr, Word *firstWord) {

  int opcode = verbalNoun->opcode;
  turnOnRangeInWord(firstWord, opcode, 11);
  /** add A (of A.R.E) to the binary value */
  turnOnWordBit(firstWord, 2);

  insertToMemory(memoryPtr, IC, firstWord->value, INSERT_MODE);
}

void printErrorMessageRegister(unsigned int lineCount, char *trimLine,
                               unsigned int *didFindErrors, char *fileName) {
  printf("ERROR in line %u of file %s: the register name %s is not a valid "
         "register name.\n",
         lineCount, fileName, trimLine);
  *didFindErrors = True;
}

void printErrorMessageAddressingMode(unsigned int lineCount,
                                     unsigned int addressingMode,
                                     char *verbalNoun, STATES commandState,
                                     unsigned int *didFindErrors,
                                     char *fileName) {
  printf(
      "ERROR in line %u of file %s: addresing mode %u is not compatible with "
      "the verbalNoun %s as %s\n",
      lineCount, fileName, addressingMode, verbalNoun,
      commandState == DEST ? "destination" : "source");
  *didFindErrors = True;
}

void turnOnWordBit(Word *word, unsigned int bitIndex) {
  word->value |= 1 << (bitIndex);
}

void turnOnRangeInWord(Word *word, unsigned int numToToggle,
                       unsigned int startingRange) {
  numToToggle = numToToggle << startingRange;
  word->value |= numToToggle;
}

void printWordInBinary(Word *word) {
  int i;
  for (i = WORD_BITS_AMOUNT - 1; i >= 0; --i) {
    printf("%d", (word->value >> i) & 1);
  }
  printf("\n");
}

int validateAddressingModeZero(char *trimLine, Bool *didFindErrors,
                               int lineCounter, char *fileName) {
  int i;
  /* i is 1 in order to skip the letter '#' */
  for (i = 1; trimLine[i] != '\0'; i++) {
    if (i == 1 && (trimLine[i] == '-' || trimLine[i] == '+'))
      continue;
    if (trimLine[i] < '0' || trimLine[i] > '9') {
      printf("ERROR in line %d of file %s: in definition of addressing mode "
             "zero a "
             "non-integer value was found!\n",
             lineCounter, fileName);
      *didFindErrors = True;
      return 0;
    }
  }
  /* skip # */
  return atoi((trimLine + 1));
}

void cleanLineForAssembler(char *line) {
  int i;
  for (i = 0; i < strlen(line); i++) {
    if (line[i] == '\t' || line[i] == '\n') {
      line[i] = ' ';
    }
  }
}

void convertNumToOctal(int num, char *stringStore) {
  int len, extractPosition;
  char temp[20];

  sprintf(temp, "%05o", num);

  len = strlen(temp);

  if (len > 5) {
    extractPosition = len - 5;
    strncpy(stringStore, &temp[extractPosition], 5);
  } else {
    strcpy(stringStore, temp);
  }

  stringStore[5] = '\0';
}

int isLineLabel(char *trimLine) {
  return trimLine[strlen(trimLine) - 1] == ':';
}

Bool validateData(char *trimLine) {

  int i = 0;
  while (trimLine[i] == ' ' || trimLine[i] == '\t') {
    i++;
    continue;
  }

  /* trimline - 1 to escape \n */
  for (; i != ' ' && i < strlen(trimLine) - 1; i++) {
    if ((trimLine[i] < '0' || trimLine[i] > '9') &&
        (trimLine[i] != '+' && trimLine[i] != '-')) {
      printf("ERROR! %c\n", trimLine[i]);
      return False;
    }
  }
  return True;
}

Bool checkCommandSyntax(char *sentence, Bool isLabelFlag, int lineCounter,
                        char *fileName) {
  Bool isInComma, isSeperated, areThereUnvalidChars;
  int i = 0;
  char currentLetter;

  isInComma = False;
  isSeperated = False;

  /*if command is already identified as a label skip it */
  if (isLabelFlag) {
    for (; sentence[i] != ':'; i++) {
      currentLetter = sentence[i];

      areThereUnvalidChars = ((sentence[i] < '0' || sentence[i] > '9') &&
                              (sentence[i] < 'a' || sentence[i] > 'z') &&
                              (sentence[i] < 'A' || sentence[i] > 'Z'));

      if (areThereUnvalidChars) {
        printf("ERROR in line %d of file %s (found in label): %c is an unvalid "
               "char\n",
               lineCounter, fileName, currentLetter);
        return False;
      }
    }
    i++;
  }
  /* skip spaces after label or before command  */
  for (; sentence[i] == ' '; i++)
    ;

  /** skip command (there shouldn't be any commas) */
  for (; sentence[i] != ' '; i++) {
    currentLetter = sentence[i];
    if (currentLetter == ',') {
      printf("ERROR in %d of file %s: found a comma at command stage\n",
             lineCounter, fileName);
      return False;
    }
  }

  for (; sentence[i] == ' '; i++)
    ;

  for (; sentence[i] != '\0'; i++) {
    currentLetter = sentence[i];

    if (currentLetter == ' ') {
      if (!isSeperated)
        isSeperated = True;
      continue;
    } else if (currentLetter == ',') {
      /* double commas */
      if (isInComma) {
        printf("ERROR in line %d of file %s: found double commas\n",
               lineCounter, fileName);
        return False;
      }
      isInComma = True;
      /* argument had space but no comma in between, error */
    } else if (isSeperated && !isInComma) {
      printf("ERROR in line %d of file %s: cannot seperate arguments "
             "without "
             "a comma\n",
             lineCounter, fileName);
      return False;

      /* normal character */
    } else {
      if (isInComma)
        isInComma = False;
      if (isSeperated)
        isSeperated = False;
    }
  }

  /* checking if the last letter is ',' */
  if (isInComma) {
    printf("ERROR in line %d of file %s: cannot end line with a comma\n",
           lineCounter, fileName);
    return False;
  }

  return True;
}

Bool validateExternAndEntrySyntax(char *line, int lineCounter, char *fileName,
                                  Bool isExtern) {
  int i;
  /* skip extern / entry word */
  for (i = 0; line[i] != ' '; i++)
    ;
  /* skip spaces */
  for (; line[i] == ' '; i++)
    ;

  if (line[i] == '\0') {
    printf("ERROR in line %d of file %s: empty %s definition\n", lineCounter,
           fileName, isExtern ? "extern" : "entry");
    return False;
  }

  /* make sure extern or entry has a valid name*/
  for (; line[i] != ' '; i++) {
    if ((line[i] < 'a' || line[i] > 'z') && (line[i] < 'A' || line[i] > 'Z') &&
        (line[i] < '0' || line[i] > '9')) {
      printf("ERROR in line %d of file %s: unvalid character %c at %s "
             "definition\n",
             lineCounter, fileName, line[i], isExtern ? "extern" : "entry");
      return False;
    }
  }

  for (; line[i] != '\0'; i++) {
    if (line[i] != ' ') {
      printf(
          "ERROR in line %d of file %s: unvalid character %c after finishing "
          "to define %s\n",
          lineCounter, fileName, line[i], isExtern ? "extern" : "entry");
      return False;
    }
  }

  return True;
}
