#ifndef DB_LB_1_OUTPUTUTILS_H
#define DB_LB_1_OUTPUTUTILS_H

#endif //DB_LB_1_OUTPUTUTILS_H

#include <stdio.h>
#include "types.h"

void logBook(book* b);

void logLibrary(lib* l);

void printLibraryHeader();

void printLibraryFooter();

long getFileSize(FILE* f);

void printLibrary(lib l, int libIdx);

void printBook(book b, int bookIdx);

void printBooksHeader();

void printBooksFooter();

void generateRandString(char *str, size_t size);

void initLib(lib* l);

void initBook(book* b);

void generateLib(lib* l);

void generateBook(book* b);

int min(int a, int b);