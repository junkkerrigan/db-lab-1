#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "constants.h"
#include "outputUtils.h"

int freeLibKey = 0;
int actualLibsCnt = 0;
int readableLibsCnt = 0;
int freeBookKey = 0;
int actualBooksCnt = 0;
int readableBooksCnt = 0;

void initLib(lib* l) {
    l->key = -1;
    l->firstBookIdx = -1;
    l->booksCnt = 0;
}

void initBook(book* b) {
    b->key = -1;
    b->nextBookIdx = -1;
    b->libKey = -1;
}

int enterLib(lib* l) {
    char buf[20];

    printf("Enter name (max. %d symbols):\n", MAX_LIBRARY_NAME_LEN);
    scanf("%20s", buf);
    strncpy(l->name, buf, MAX_LIBRARY_NAME_LEN);

    printf("Enter phone number (max. %d symbols):\n", MAX_PHONE_NUMBER_LEN);
    scanf("%20s", buf);
    strncpy(l->phoneNumber, buf, MAX_PHONE_NUMBER_LEN);

    printf("Is the library state or private? (`s` for state, `p` for private)\n");
    scanf("%20s", buf);
    if (strcmp(buf, "s") == 0) {
        l->isState = 1;
    } else if (strcmp(buf, "p") == 0) {
        l->isState = 0;
    } else {
        printf("Invalid input.");
        return -1;
    }

    return 0;
}

int openFileSafe(FILE** f, const char* fileName, const char* mode, const char* fileLabel) {
    (*f) = fopen(fileName, mode);
    if ((*f) == NULL) {
        fclose(*f);
        (*f) = fopen(fileName, "w+b");
        fclose(*f);
        printf("Failed to open %s file. The file is created now.\n", fileLabel);
        return -1;
    }
    return 0;
}

int writeLibToDb(lib* l, int idx) {
    FILE* libsDbFl = NULL;
    if (0 != openFileSafe(&libsDbFl, libsDbFileName, "r+b", "libraries database")) {
        return -2;
    }

    fseek(libsDbFl, idx * sizeof(lib), SEEK_SET);
    fwrite(l, sizeof(lib), 1, libsDbFl);
    fclose(libsDbFl);

    return 0;
}

int retrieveFreeLibIdx() {
    if (readableLibsCnt <= MAX_LIBRARIES_COUNT) {
        return readableLibsCnt++;
    }
    if (actualLibsCnt == 0) {
        return 0;
    }

    FILE* libsGz = NULL;
    if (0 != openFileSafe(&libsGz, libsGzFileName, "rb", "libraries garbage zone")) {
        return -2;
    }

    int freeIdx = 0;
    long curPos = 0;
    for(int j = 0; j < MAX_LIBRARIES_COUNT; j++) {
        curPos = ftell(libsGz);
        fread(&freeIdx, sizeof(int), 1, libsGz);

        if (freeIdx != -1) {
            fseek(libsGz, curPos, SEEK_SET);
            int minusOne = -1;
            fwrite(&minusOne, sizeof(int), 1, libsGz);
            fclose(libsGz);
            return freeIdx;
        }
    }

    fclose(libsGz);
    return -3;
}

int addLibRecToIdx(int libKey, int libIdx) {
    FILE* libsIdxFl = NULL;
    if (0 != openFileSafe(&libsIdxFl, libsIdxFileName, "r+b", "libraries index")) {
        return -2;
    }

    idxRec i;
    i.key = libKey;
    i.idx = libIdx;

    fseek(libsIdxFl, 0, SEEK_END);
    fwrite(&i, sizeof(idxRec), 1, libsIdxFl);
    fclose(libsIdxFl);

    return 0;
}

int addLib() {
    if (actualLibsCnt >= MAX_LIBRARIES_COUNT) {
        printf("Not enough space.\n");
        return -1;
    }

    lib l;
    initLib(&l);
    if (0 != enterLib(&l)) {
        return -1;
    }
    l.key = freeLibKey++;

    int idx = retrieveFreeLibIdx();
    if (idx < 0) {
        return -1;
    }

    if (0 != writeLibToDb(&l, idx)) {
        return -1;
    }
    if (0 != addLibRecToIdx(l.key, idx)) {
        return -1;
    }
    actualLibsCnt++;

    return 0;
}

int isValidInt(char* str, int len) {
    for(int j = 0; j < len; j++) {
        if (!isdigit(str[j])) {
            return 0;
        }
    }

    return 1;
}

int getLibIdxByKey(int libKey) {
    if (libKey >= freeLibKey) {
        return -1;
    }
    FILE* libsIdxFl = NULL;
    if (0 != openFileSafe(&libsIdxFl, libsIdxFileName, "rb", "libraries index")) {
        return -2;
    }

    fseek(libsIdxFl, libKey * sizeof(idxRec), SEEK_SET);
    idxRec i;
    fread(&i, sizeof(idxRec), 1, libsIdxFl);
    fclose(libsIdxFl);

    return i.idx;
}

int enterBook(book* b) {
    char buf[20];

    printf("Enter name (max. %d symbols):\n", MAX_BOOK_NAME_LEN);
    scanf("%20s", buf);
    strncpy(b->name, buf, MAX_BOOK_NAME_LEN);

    printf("Enter author (max. %d symbols):\n", MAX_BOOK_AUTHOR_LEN);
    scanf("%20s", buf);
    strncpy(b->author, buf, MAX_BOOK_AUTHOR_LEN);

    printf("Is the book available or in use? (`a` for available, `u` for in use)\n");
    scanf("%20s", buf);
    if (strcmp(buf, "a") == 0) {
        b->isAvailable = 1;
    } else if (strcmp(buf, "u") == 0) {
        b->isAvailable = 0;
    } else {
        printf("Invalid input.");
        return -1;
    }

    printf("Enter key of the library to assign book with:\n");
    scanf("%s", buf);
    if (!isValidInt(buf, strlen(buf))) {
        printf("Invalid input.");
        return -1;
    }
    int libKey = atoi(buf);
    b->libKey = libKey;

    return 0;
}

int retrieveFreeBookIdx() {
    if (actualBooksCnt == 0) {
        return 0;
    }
    if (readableBooksCnt <= MAX_BOOKS_COUNT) {
        return readableBooksCnt++;
    }

    FILE* booksGz = NULL;
    if (0 != openFileSafe(&booksGz, booksGzFileName, "rb", "books garbage zone")) {
        return -2;
    }

    int freeIdx = 0;
    long curPos = 0;
    for(int j = 0; j < MAX_LIBRARIES_COUNT; j++) {
        curPos = ftell(booksGz);
        fread(&freeIdx, sizeof(int), 1, booksGz);

        if (freeIdx != -1) {
            fseek(booksGz, curPos, SEEK_SET);
            int minusOne = -1;
            fwrite(&minusOne, sizeof(int), 1, booksGz);
            fclose(booksGz);
            return freeIdx;
        }
    }

    fclose(booksGz);

    return -3;
}

int writeBookToDb(book* b, int idx) {
    FILE* booksDbFl = NULL;
    if (0 != openFileSafe(&booksDbFl, booksDbFileName, "r+b", "books database")) {
        return -2;
    }

    fseek(booksDbFl, idx * sizeof(book), SEEK_SET);
    fwrite(b, sizeof(book), 1, booksDbFl);
    fclose(booksDbFl);

    return 0;
}

int addBook() {
    if (actualBooksCnt >= MAX_BOOKS_COUNT) {
        printf("Not enough space.\n");
        return -1;
    }

    book b;
    initBook(&b);
    if (0 != enterBook(&b)) {
        return -1;
    }
    if (-1 == getLibIdxByKey(b.libKey)) {
        printf("Wrong input: no library with such key.\n");
        return -1;
    }
    b.key = freeBookKey++;

    int idx = retrieveFreeBookIdx();
    if (idx < 0) {
        return -1;
    }

    if (0 != writeBookToDb(&b, idx)) {
        return -1;
    }

    return 0;
}

void printCheatsheet() {
    printf("1) Add new library\n"
           "2) Add new book\n"
           "3) Display all the libraries\n"
           "4) Display library by it's key\n"
           "5) Display all the books\n"
           "6) Display books of specific library\n"
           "7) Delete library\n"
           "8) Delete book\n"
           "9) Modify data about library (only address or/and phone number)\n"
           "10) Modify data about book (only name and author)\n"
           "11) Terminate work with database (Q)\n"
           "12) Print this cheatsheet (H)\n\n");
}

int printAllLibs() {
    FILE* libsDb = NULL;
    if (0 != openFileSafe(&libsDb, libsDbFileName, "rb", "libraries database")) {
        return -2;
    }

    printLibraryHeader();
    lib l;
    for(int j = 0; j < readableLibsCnt; j++) {
        fread(&l, sizeof(lib), 1, libsDb);
        if (-1 != getLibIdxByKey(l.key)) {
            printLibrary(&l);
            printf("\n");
        }
    }
    printLibraryFooter();
    fclose(libsDb);

    return 0;
}



int printLibByKey(int libKey) {
    int libIdx = getLibIdxByKey(libKey);
    if (-1 == libIdx) {
        printf("No library with such key.\n");
        return -1;
    }

    FILE* libsDb = NULL;
    if (0 != openFileSafe(&libsDb, libsDbFileName, "rb", "libraries database")) {
        return -2;
    }

    lib l;
    fseek(libsDb, libIdx * sizeof(lib), SEEK_SET);
    fread(&l, sizeof(lib), 1, libsDb);

    printLibraryHeader();
    printLibrary(&l);
    printf("\n");
    printLibraryFooter();
    fclose(libsDb);

    return 0;
}


int interactWithDb() {
    printf("\nChoose an option (H or 12 to see cheatsheet):\n\n");

    char optionStr[2];
    scanf("%2s", optionStr);
    printf("\n");
    if (strcmp(optionStr, "H") == 0) {
        printf("\n");
        printCheatsheet();
        scanf("%2s", optionStr);
        printf("\n");
    }
    int option = atoi(optionStr);
    if (option < 1 || option > 11) {
        printf("Invalid input: no such option. Try again.\n");
        fflush(stdin);
        return -1;
    }

    switch (option) {
        case 1: {
            if (0 != addLib()) {
                printf("Failed to add library.\n");
            }
            break;
        }
        case 2: {
            if (0 != addBook()) {
                printf("Failed to add book.\n");
            }
            break;
        }
        case 3: {
            if (0 != printAllLibs()) {
                printf("Failed to print libraries.\n");
            }
            break;
        }
        case 4: {
            printf("Enter library key:\n\n");
            char buf[9];
            scanf("%9s", buf);
            printf("\n");
            if (strlen(buf) > 9) {
                printf("Invalid input: no such library key");
                return -1;
            }
            for(int j = 0; j < strlen(buf); j++) {
                if (!isdigit(buf[j])) {
                    printf("Invalid input: no such library key");
                    return -1;
                }
            }
            int libKey = atoi(buf);
            if (0 != printLibByKey(libKey)) {
                printf("Failed to print library.\n");
            }
        }
        case 5: {
            break;
        }
        case 6: {

        }
        case 7: {

        }
        case 8: {

        }
        case 9: {

        }
        case 10: {

        }
        case 11: {

        }
        default: {
            break;
        }
    }
}

void initFiles() {
    FILE* f;

    f = fopen(booksDbFileName, "wb");
    fclose(f);

    f = fopen(booksGzFileName, "wb");
    fclose(f);

    f = fopen(booksIdxFileName, "wb");
    fclose(f);

    f = fopen(libsDbFileName, "wb");
    fclose(f);

    f = fopen(libsGzFileName, "wb");
    fclose(f);

    f = fopen(libsIdxFileName, "wb");
    fclose(f);
}

int main() {
    initFiles();
    while(1 != interactWithDb());
    return 0;
}
