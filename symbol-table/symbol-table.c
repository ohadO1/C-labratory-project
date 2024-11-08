#include "symbol-table.h"
#include "../helper-functions/helper-functions.h"

SymbolNode *createSymbolNode(const char *label, int decimalAddress, char *type,
                             char *fileName) {
  SymbolNode *newNode = (SymbolNode *)malloc(sizeof(SymbolNode));
  if (!newNode) {
    printf("Memory allocation failed!\n");
    return NULL;
  }
  strcpy(newNode->symbol.label, label);
  strcpy(newNode->symbol.type, type);
  newNode->symbol.decimalAddress = decimalAddress;
  newNode->next = NULL;
  return newNode;
}

void updateSymbolToEntry(SymbolNode *head, char *label, char *fileName,
                         int lineCounter, Bool *didFindErrorsFlag) {
  SymbolNode *foundSymbol;

  foundSymbol = searchsymbol(head, label);

  if (foundSymbol == NULL) {
    printf(
        "ERROR in line %d of File %s: defined an entry %s but didn't declare "
        "it.\n",
        lineCounter, fileName, label);
    *didFindErrorsFlag = True;
    return;
  }

  validateEntrySymbolBeforeInsertion(head, label, fileName, ENTRY, lineCounter,
                                     didFindErrorsFlag);
  if (*didFindErrorsFlag)
    return;

  strcpy(foundSymbol->symbol.type, ENTRY);
}

void validateEntrySymbolBeforeInsertion(SymbolNode *head, const char *label,
                                        char *fileName, char *type,
                                        unsigned int lineCounter,
                                        Bool *didFindErrorsFlag) {
  SymbolNode *current = head;
  while (current != NULL) {
    if (strcmp(label, current->symbol.label) == 0 &&
        strcmp(current->symbol.type, EXTERN) == 0) {
      printf("ERROR in %s line %u: cant define the same label %s to both "
             "entry and "
             "extern variables within the same file.\n",
             fileName, lineCounter, label);
      *didFindErrorsFlag = True;
    }

    if (strcmp(label, current->symbol.label) == 0 &&
        strcmp(type, current->symbol.type) == 0) {
      printf(
          "ERROR in %s line %u: can't insert symbol %s of type %s because it "
          "was already "
          "created. will exit program after finding all errors...\n",
          fileName, lineCounter, label, type);
      *didFindErrorsFlag = True;
    }

    current = current->next;
  }
}
Bool validateFirstStageSymbolBeforeInsertion(SymbolNode *head,
                                             const char *label, char *fileName,
                                             unsigned int lineCounter,
                                             Bool *didFindErrorsFlag) {

  if (searchsymbol(head, label) != NULL) {
    printf("ERROR in %s line %u: found duplicated label definition for label "
           "%s\n",
           fileName, lineCounter, label);
    *didFindErrorsFlag = True;
    return False;
  }
  return True;
}

void insertSymbol(SymbolNode **head, const char *label, int decimalAddress,
                  char *type, char *fileName) {
  SymbolNode *newNode;
  SymbolNode *current;

  newNode = createSymbolNode(label, decimalAddress, type, fileName);
  newNode->next = NULL;

  if (*head == NULL) {
    *head = newNode;
  } else {
    current = *head;
    while (current->next != NULL) {
      current = current->next;
    }
    current->next = newNode;
  }
}

SymbolNode *searchsymbol(SymbolNode *head, const char *label) {
  SymbolNode *current = head;

  while (current != NULL) {
    if ((strcmp(current->symbol.label, label) == 0))
      return current;
    current = current->next;
  }

  return NULL;
}

void updateSymbolTableValue(SymbolNode **symbolHeadPtr, int *IC, char *type) {
  SymbolNode *current = *symbolHeadPtr;
  while (current != NULL) {
    if (strcmp(current->symbol.type, type) == 0) {
      current->symbol.decimalAddress += (*IC + 100);
    }
    current = current->next;
  }
}

Bool areThereSymbolsWithExtension(SymbolNode **symbolHeadPtr,
                                  char *searchedType) {
  SymbolNode *current = *symbolHeadPtr;
  while (current != NULL) {
    if (strcmp(current->symbol.type, searchedType) == 0) {
      return True;
    }
    current = current->next;
  }
  return False;
}

void codeSpecificExtensionSymbol(SymbolNode **symbolHeadPtr, char *searchedType,
                                 FILE *file) {
  char line[MAX_LINE_LENGTH];
  char convertedAddress[MAX_TYPE_LEN];
  SymbolNode *current = *symbolHeadPtr;
  while (current != NULL) {
    if (strcmp(current->symbol.type, searchedType) == 0 &&
        current->symbol.decimalAddress != -1) {
      sprintf(convertedAddress, "%d", current->symbol.decimalAddress);
      strcpy(line, current->symbol.label);
      strcat(line, " ");
      strcat(line, convertedAddress);
      strcat(line, "\n");
      fputs(line, file);
    }
    current = current->next;
  }
}

void printSymbols(SymbolNode *head) {
  SymbolNode *current;
  printf("---------------------------------- printing symbol table "
         "-----------------------------------\n");
  current = head;
  while (current != NULL) {
    printf("\t\tsymbol label: %s\n", current->symbol.label);
    printf("\t\tsymbol decimalAddress: %d\n", current->symbol.decimalAddress);
    printf("\t\tsymbol type: %s\n", current->symbol.type);
    printf("-------------------------------------------------------------------"
           "---------------------------------\n");
    current = current->next;
  }
}

void freeSymbolList(SymbolNode *head) {
  SymbolNode *current = head;
  SymbolNode *nextNode;
  while (current != NULL) {
    nextNode = current->next;
    free(current);
    current = nextNode;
  }
}
