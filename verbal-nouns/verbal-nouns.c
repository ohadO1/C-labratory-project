#include "./verbal-nouns.h"

void initializeVerbalNounTable(VerbalNounTable **table) {
  VerbalNounTable instruction_set[VERBAL_NOUNS_COUNT] = {
      {{"mov", 0, {0, 1, 2, 3}, {1, 2, 3}, 2},
       {"cmp", 1, {0, 1, 2, 3}, {0, 1, 2, 3}, 2},
       {"add", 2, {0, 1, 2, 3}, {1, 2, 3}, 2},
       {"sub", 3, {0, 1, 2, 3}, {1, 2, 3}, 2},
       {"lea", 4, {1}, {1, 2, 3}, 2},
       {"clr", 5, {-1}, {1, 2, 3}, 1},
       {"not", 6, {-1}, {1, 2, 3}, 1},
       {"inc", 7, {-1}, {1, 2, 3}, 1},
       {"dec", 8, {-1}, {1, 2, 3}, 1},
       {"jmp", 9, {-1}, {1, 2}, 1},
       {"bne", 10, {-1}, {1, 2}, 1},
       {"red", 11, {-1}, {1, 2, 3}, 1},
       {"prn", 12, {-1}, {0, 1, 2, 3}, 1},
       {"jsr", 13, {-1}, {1, 2}, 1},
       {"rts", 14, {-1}, {-1}, 0},
       {"stop", 15, {-1}, {-1}, 0}}};

  *table = instruction_set;
}

VerbalNoun *findVerbalNounInTable(VerbalNounTable table, char *verb) {
  int i;
  for (i = 0; i < VERBAL_NOUNS_COUNT; i++) {
    if (strcmp(verb, table[i].verbalNoun) == 0) {
      return &(table[i]);
    }
  }
  return NULL;
}

void printVerbalNounTable(VerbalNounTable table) {
  int i;
  printf("*************************** Printing Verbal Noun Table "
         "***************************************\n");
  for (i = 0; i < VERBAL_NOUNS_COUNT; i++) {
    printf("VerbalNoun: %s, Opcode: %u\n", table[i].verbalNoun,
           table[i].opcode);
  }
  printf("*********************************************************************"
         "************************\n");
}
