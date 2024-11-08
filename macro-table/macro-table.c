#include "macro-table.h"

MacroNode *createMacroNode(const char *name, const char *body) {
  MacroNode *newNode = (MacroNode *)malloc(sizeof(MacroNode));
  if (!newNode) {
    printf("Memory allocation failed!\n");
    return NULL;
  }
  strcpy(newNode->macro.name, name);
  strcpy(newNode->macro.body, body);
  newNode->next = NULL;
  return newNode;
}

void insertMacro(MacroNode **head, const char *name, const char *body) {
  MacroNode *newNode = createMacroNode(name, body);
  newNode->next = *head;
  *head = newNode;
}

MacroNode *searchMacro(MacroNode *head, const char *name) {
  MacroNode *current = head;
  while (current != NULL) {
    if (strcmp(current->macro.name, name) == 0) {
      return current;
    }
    current = current->next;
  }
  return NULL;
}

void displayMacros(MacroNode *head) {
  MacroNode *current = head;
  while (current != NULL) {
    printf("Macro Name: %s\n", current->macro.name);
    printf("Macro Body: \n%s\n", current->macro.body);
    printf("-----------------------\n");
    current = current->next;
  }
}

void freeMacroList(MacroNode *head) {
  MacroNode *current = head;
  MacroNode *nextNode;
  while (current != NULL) {
    nextNode = current->next;
    free(current);
    current = nextNode;
  }
}
