#ifndef MEMORY_H
#define MEMORY_H

#include "../assembler.h"

/* Consts */
#define MEMORY_SIZE 4096
#define MEMORY_STARTING_INDEX 100

/* Defines the struct of a memory unit, includes a word (which is a 15 bit
 * int, and an addressDecimal - represents the memory image address)  */
typedef struct {
  int addressDecimal;
  Word binaryWord;
} Memory;

/* Insertion modes in order for insertToMemory function (update doesn't change
 * the IC, insert does) */
typedef enum INSERTION_MODES { UPDATE_MODE, INSERT_MODE } INSERTION_MODES;

/* Inserts a value to the IC or DC memory image */
void insertToMemory(Memory *memoryptr, int *addressDecimal, int value,
                    INSERTION_MODES valueCounter);

/* Prints memory (mainly for debugging reasons) */
void printMemory(Memory *memoryptr, char *header);
/* Make a new node in order to insert it into the memory (an inner function that
 * is used in insertToMemroy)*/
Memory *makeNewNode(char *binaryValue, int addressDecimal, char *type);

/* Clean up function the iterates between each memory node and frees it (each
 * node is dynamically allocated)*/
void cleanMemory(Memory *memoryPtr, int *counter);

#endif
