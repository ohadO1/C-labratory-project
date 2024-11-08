#include "memory.h"
#include "../helper-functions/helper-functions.h"

void insertToMemory(Memory *memoryptr, int *addressDecimal, int value,
                    INSERTION_MODES mode) {
  memoryptr[*addressDecimal + MEMORY_STARTING_INDEX].addressDecimal =
      *addressDecimal;
  memoryptr[*addressDecimal + MEMORY_STARTING_INDEX].binaryWord.value = value;
  if (mode == INSERT_MODE)
    *addressDecimal += 1;
}

void cleanMemory(Memory *memoryPtr, int *counter) {
  int i;
  for (i = 0; i < *counter; i++) {
    memoryPtr[MEMORY_STARTING_INDEX + i].binaryWord.value = 0;
    memoryPtr[MEMORY_STARTING_INDEX + i].addressDecimal = 0;
  }
  *counter = 0;
}

void printMemory(Memory *memoryptr, char *header) {
  int i;
  Memory current;
  printf("\n---------------------------- in print %s memory "
         "------------------------\n",
         header);
  printf("*****************************************************************"
         "******************** \n");
  for (i = 0; i < MEMORY_SIZE; i++) {
    current = memoryptr[i];
    if (current.addressDecimal != 0 || current.binaryWord.value != 0) {
      printf("Index: %d\n", i);
      printf("addressDecimal: %d\nbinaryWord: ", current.addressDecimal);
      printWordInBinary(&(current.binaryWord));
      printf("*****************************************************************"
             "********************* \n");
    }
  }
  printf("\n----------------------------- outside print memory "
         "------------------------------------\n\n");
}
