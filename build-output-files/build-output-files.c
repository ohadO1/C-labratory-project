#include "build-output-files.h"

Bool buildOutputFiles(int IC, int DC, Memory *icMemoryPtr, Memory *dcMemoryPtr,
                      char *fileName, SymbolNode **symbolHeadPtr) {
  char *objFileName, *entryFileName, *externFileName;
  FILE *objFile, *entryFile, *externFile;

  objFileName = generateFileName(fileName, OBJ_FILE_NAME_EXTENSION);
  objFile = fopen(objFileName, "w");

  buildObjFile(IC, DC, icMemoryPtr, dcMemoryPtr, fileName, objFile);

  if (areThereSymbolsWithExtension(symbolHeadPtr, ENTRY)) {
    entryFileName = generateFileName(fileName, ENTRY_FILE_EXTENSION);
    externFileName = generateFileName(fileName, EXTERN_FILE_EXTENSION);
    entryFile = fopen(entryFileName, "w");
    externFile = fopen(externFileName, "w");

    codeSpecificExtensionSymbol(symbolHeadPtr, ENTRY, entryFile);
    codeSpecificExtensionSymbol(symbolHeadPtr, EXTERN, externFile);

    free(entryFileName);
  }

  free(objFileName);
  return True;
}

Bool buildObjFile(int IC, int DC, Memory *icMemoryPtr, Memory *dcMemoryPtr,
                  char *fileName, FILE *objFile) {

  int i;
  char line[80], convertedOctalNum[20], convertedAddress[20];
  if (!buildObjFileHeader(IC, DC, fileName, line, objFile))
    return False;

  for (i = 0; i < IC; i++) {
    convertNumToOctal(icMemoryPtr[i + 100].binaryWord.value, convertedOctalNum);
    sprintf(convertedAddress, "%04d", i + 100);
    strcpy(line, convertedAddress);
    strcat(line, " ");
    strcat(line, convertedOctalNum);
    strcat(line, "\n");
    fputs(line, objFile);
  }

  for (i = 0; i < DC; i++) {
    convertNumToOctal(dcMemoryPtr[i + 100].binaryWord.value, convertedOctalNum);
    sprintf(convertedAddress, "%04d", dcMemoryPtr[i + 100].addressDecimal);
    strcpy(line, convertedAddress);
    strcat(line, " ");
    strcat(line, convertedOctalNum);
    strcat(line, "\n");
    fputs(line, objFile);
  }

  return True;
}

Bool buildObjFileHeader(int IC, int DC, char *fileName, char *firstLine,
                        FILE *objFile) {
  char *convertedIc, *convertedDc;

  /* size is 7 to have extra space*/
  convertedIc = (char *)malloc(7 * sizeof(char));
  if (convertedIc == NULL) {
    printf("Error while allocating memory\n");
    return False;
  }
  convertedDc = (char *)malloc(7 * sizeof(char));
  if (convertedDc == NULL) {
    printf("Error while allocating memory\n");
    return False;
  }

  if (IC > 0 || DC > 0) {
    sprintf(convertedIc, "%d", IC);
    sprintf(convertedDc, "%d", DC);
    strcpy(firstLine, convertedIc);
    strcat(firstLine, " ");
    strcat(firstLine, convertedDc);
    strcat(firstLine, "\n");
    fputs(firstLine, objFile);
  }

  free(convertedIc);
  free(convertedDc);

  return True;
}
