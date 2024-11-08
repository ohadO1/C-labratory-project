#ifndef MACRO_TABLE_H
#define MACRO_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MACRO_BODY_MAX_LEN 200
#define MAX_MACRO_NAME_LEN 20

typedef struct Macro {
  char name[MAX_MACRO_NAME_LEN];
  char body[MACRO_BODY_MAX_LEN];
} Macro;

typedef struct MacroNode {
  Macro macro;
  struct MacroNode *next;
} MacroNode;

/**Function to create a new node */
MacroNode *createMacroNode(const char *name, const char *body);

/** Function to insert a macro into the linked list */
void insertMacro(MacroNode **head, const char *name, const char *body);

/** looks up for a micro, if found returns the node, otherwise Null */
MacroNode *searchMacro(MacroNode *head, const char *name);

/** prints the nodes of the nodelist */
void displayMacros(MacroNode *head);

/** frees the macroList memory*/
void freeMacroList(MacroNode *head);

#endif
