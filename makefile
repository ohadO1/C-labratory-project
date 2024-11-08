CC = gcc
CFLAGS = -ansi -pedantic -Wall
PRE_ASSEMBLER_DIR = ./pre-assembler
HELPER_FUNCTIONS_DIR = ./helper-functions
MACRO_TABLE_DIR = ./macro-table
MEMORY_TABLE_DIR = ./memory
SYMBOL_TABLE_DIR = ./symbol-table
FIRST_STAGE_DIR = ./first-stage
VERBAL_NOUNS_DIR = ./verbal-nouns
SECOND_STAGE_DIR = ./second-stage
BUILD_OUTPUT_FILES_DIR = ./build-output-files


HEADERS = assembler.h \
					$(VERBAL_NOUNS_DIR)/verbal-nouns.h \
          $(HELPER_FUNCTIONS_DIR)/helper-functions.h \
          $(PRE_ASSEMBLER_DIR)/pre-assembler.h \
          $(MACRO_TABLE_DIR)/macro-table.h \
					$(MEMORY_TABLE_DIR)/memory.h \
					$(SYMBOL_TABLE_DIR)/symbol-table.h \
					$(FIRST_STAGE_DIR)/first-stage.h \
					$(SECOND_STAGE_DIR)/second-stage.h \
					$(BUILD_OUTPUT_FILES_DIR)/build-output-files.h


C_FILES = assembler.c \
					$(VERBAL_NOUNS_DIR)/verbal-nouns.c \
          $(HELPER_FUNCTIONS_DIR)/helper-functions.c \
					$(PRE_ASSEMBLER_DIR)/pre-assembler.c \
					$(MACRO_TABLE_DIR)/macro-table.c \
					$(MEMORY_TABLE_DIR)/memory.c \
					$(SYMBOL_TABLE_DIR)/symbol-table.c \
					$(FIRST_STAGE_DIR)/first-stage.c \
					$(SECOND_STAGE_DIR)/second-stage.c \
					$(BUILD_OUTPUT_FILES_DIR)/build-output-files.c

all: assembler

assembler: $(C_FILES) 
	$(CC) $(CFLAGS) $(C_FILES) -o assembler

clean: rm -f *~ assembler
