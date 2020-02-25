#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "constants.h"
#include "utils.h"

int freeLibKey = 0;
int actualLibsCnt = 0;
int readableLibsCnt = 0;
int freeBookKey = 0;
int actualBooksCnt = 0;
int readableBooksCnt = 0;

int enterLib(lib* l) {
    char buf[20];

    memset(buf, '\0', 20);
    printf("Enter name (max. %d symbols):\n", MAX_LIBRARY_NAME_LEN);
    scanf("%s", buf);
    if (strlen(buf) > MAX_LIBRARY_NAME_LEN) {
        printf("Invalid input: name is too long.\n");
        return -1;
    }
    strcpy(l->name, buf);

    printf("Enter phone number (max. %d symbols):\n", MAX_PHONE_NUMBER_LEN);
    memset(buf, '\0', 20);
    scanf("%s", buf);
    if (strlen(buf) > MAX_PHONE_NUMBER_LEN) {
        printf("Invalid input: phone number is too long.\n");
        return -1;
    }
    strcpy(l->phoneNumber, buf);

    printf("Is the library state or private? (`s` for state, `p` for private)\n");
    memset(buf, '\0', 20);
    scanf("%s", buf);
    if (strcmp(buf, "s") == 0) {
        l->isState = 1;
    } else if (strcmp(buf, "p") == 0) {
        l->isState = 0;
    } else {
        printf("Invalid input.\n");
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

int writeLibToDbOnIdx(lib* l, int libIdx) {
    FILE* libsDbFl = NULL;
    if (0 != openFileSafe(&libsDbFl, libsDbFileName, "r+b", "libraries database")) {
        return -2;
    }

    fseek(libsDbFl, libIdx * sizeof(lib), SEEK_SET);
    fwrite(l, sizeof(lib), 1, libsDbFl);
    fclose(libsDbFl);

    return 0;
}

int retrieveFreeLibIdx() {
    // is enough space checked before
    if (readableLibsCnt <= MAX_LIBRARIES_COUNT) {
        readableLibsCnt++;
        return readableLibsCnt - 1;
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

int addLibRecToIdxFl(int libKey, int libIdx) {
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

int addBookRecToIdxFl(int bookKey, int bookIdx) {
    FILE* bookIdxFl = NULL;
    if (0 != openFileSafe(&bookIdxFl, booksIdxFileName, "r+b", "books index")) {
        return -2;
    }

    idxRec i;
    i.key = bookKey;
    i.idx = bookIdx;

    if (bookIdx >= 0) {
        fseek(bookIdxFl, 0, SEEK_END);
    } else {
        fseek(bookIdxFl, bookKey * sizeof(idxRec), SEEK_SET);
    }
    fwrite(&i, sizeof(idxRec), 1, bookIdxFl);

    fclose(bookIdxFl);
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

    if (0 != writeLibToDbOnIdx(&l, idx)) {
        return -1;
    }
    if (0 != addLibRecToIdxFl(l.key, idx)) {
        return -1;
    }
    actualLibsCnt++;

    return 0;
}

int isValidInt(char* str, int len) {
    if (len > 9) {
        return 0;
    }
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
    // TODO: fix bug with name len 20, then name and phone will concat IN SOME REASON
    char buf[20];

    printf("Enter name (max. %d symbols):\n", MAX_BOOK_NAME_LEN);
    memset(buf, '\0', 20);
    scanf("%s", buf);
    if (strlen(buf) > MAX_BOOK_NAME_LEN) {
        printf("Invalid input: name is too long.\n");
        return -1;
    }
    strcpy(b->name, buf);

    printf("Enter author (max. %d symbols):\n", MAX_BOOK_AUTHOR_LEN);
    memset(buf, '\0', 20);
    scanf("%s", buf);
    if (strlen(buf) > MAX_BOOK_AUTHOR_LEN) {
        printf("Invalid input: author`s name is too long.\n");
        return -1;
    }
    strcpy(b->author, buf);

    printf("Is the book available or in use? (`a` for available, `u` for in use)\n");
    memset(buf, '\0', 20);
    scanf("%s", buf);
    if (strcmp(buf, "a") == 0) {
        b->isAvailable = 1;
    } else if (strcmp(buf, "u") == 0) {
        b->isAvailable = 0;
    } else {
        printf("Invalid input.\n");
        return -1;
    }

    printf("Enter key of the library to assign book with:\n");
    scanf("%s", buf);
    if (!isValidInt(buf, strlen(buf))) {
        printf("Invalid input: no such library key: `%s`.\n", buf);
        return -1;
    }
    int libKey = atoi(buf);
    b->libKey = libKey;

    return 0;
}

int getBookIdxByKey(int bookKey) {
    if (bookKey >= freeBookKey) {
        return -1;
    }
    FILE* booksIdxFl = NULL;
    if (0 != openFileSafe(&booksIdxFl, booksIdxFileName, "rb", "books index")) {
        return -2;
    }

    fseek(booksIdxFl, bookKey * sizeof(idxRec), SEEK_SET);
    idxRec i;
    fread(&i, sizeof(idxRec), 1, booksIdxFl);
    fclose(booksIdxFl);

    return i.idx;
}

int retrieveFreeBookIdx() {
    // is enough space checked before
    if (readableBooksCnt <= MAX_BOOKS_COUNT) {
        readableBooksCnt++;
        return readableBooksCnt - 1;
    }
    if (actualBooksCnt == 0) {
        return 0;
    }

    FILE* booksGzFl = NULL;
    if (0 != openFileSafe(&booksGzFl, booksGzFileName, "rb", "books garbage zone")) {
        return -2;
    }

    int freeIdx = 0;
    long curPos = 0;
    for(int j = 0; j < MAX_LIBRARIES_COUNT; j++) {
        curPos = ftell(booksGzFl);
        fread(&freeIdx, sizeof(int), 1, booksGzFl);

        if (freeIdx != -1) {
            fseek(booksGzFl, curPos, SEEK_SET);
            int minusOne = -1;
            fwrite(&minusOne, sizeof(int), 1, booksGzFl);
            fclose(booksGzFl);
            return freeIdx;
        }
    }

    fclose(booksGzFl);

    return -3;
}

int writeBookToDbOnIdx(book* b, int bookIdx) {
    FILE* booksDbFl = NULL;
    if (0 != openFileSafe(&booksDbFl, booksDbFileName, "r+b", "books database")) {
        return -2;
    }

    fseek(booksDbFl, bookIdx * sizeof(book), SEEK_SET);
    fwrite(b, sizeof(book), 1, booksDbFl);
    fclose(booksDbFl);

    return 0;
}

int getBookByIdx(book* b, int bookIdx) {
    FILE* booksDbFl = NULL;
    if (0 != openFileSafe(&booksDbFl, booksDbFileName, "r+b", "books database")) {
        return -2;
    }

    fseek(booksDbFl, bookIdx * sizeof(book), SEEK_SET);
    fread(b, sizeof(book), 1, booksDbFl);

    fclose(booksDbFl);
    return 0;
}

int addBookToLib(int libIdx, int bookIdx) {
    FILE* libsDbFl = NULL;
    if (0 != openFileSafe(&libsDbFl, libsDbFileName, "r+b", "libraries database")) {
        return -2;
    }

    lib l;
    fseek(libsDbFl, sizeof(lib) * libIdx, SEEK_SET);
    fread(&l, sizeof(lib), 1, libsDbFl);

    if (l.firstBookIdx != -1) {
        FILE* booksDbFl = NULL;
        if (0 != openFileSafe(&booksDbFl, booksDbFileName, "r+b", "books database")) {
            return -2;
        }

        int curBookIdx = -1;
        int nextBookIdx = l.firstBookIdx;
        book b;
        while (nextBookIdx != -1) {
            fseek(booksDbFl, sizeof(book) * nextBookIdx, SEEK_SET);
            fread(&b, sizeof(book), 1, booksDbFl);
            curBookIdx = nextBookIdx;
            nextBookIdx = b.nextBookIdx;
        }
        b.nextBookIdx = bookIdx;

        fseek(booksDbFl, sizeof(book) * curBookIdx, SEEK_SET);
        fwrite(&b, sizeof(book), 1, booksDbFl);
        fclose(booksDbFl);
    } else {
        l.firstBookIdx = bookIdx;
    }

    l.booksCnt++;
    fseek(libsDbFl, sizeof(lib) * libIdx, SEEK_SET);
    fwrite(&l, sizeof(lib), 1, libsDbFl);

    fclose(libsDbFl);
    return 0;
}

int addBook() {
    if (actualBooksCnt >= MAX_BOOKS_COUNT) {
        printf("Not enough space.\n");
        return -1;
    }
    if (actualLibsCnt == 0) {
        printf("Impossible to add book, no libraries yet.\n");
        return -1;
    }

    book b;
    initBook(&b);
    if (0 != enterBook(&b)) {
        return -1;
    }
    int libIdx = getLibIdxByKey(b.libKey);
    if (0 > libIdx) {
        printf("Wrong input: no library with key %d.\n", b.libKey);
        return -1;
    }
    b.key = freeBookKey++;

    int bookIdx = retrieveFreeBookIdx();
    if (bookIdx < 0) {
        return -1;
    }
    if (0 != addBookToLib(libIdx, bookIdx)) {
        return -1;
    }

    if (0 != writeBookToDbOnIdx(&b, bookIdx)) {
        return -1;
    }
    if (0 != addBookRecToIdxFl(b.key, bookIdx)) {
        return -1;
    }
    actualBooksCnt++;

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
           "12) Print this cheatsheet (H)\n");
}

int printAllLibs() {
    if (actualLibsCnt == 0) {
        printf("There are no libraries at moment ;(\n");
        return 0;
    }
    FILE* libsDb = NULL;
    if (0 != openFileSafe(&libsDb, libsDbFileName, "rb", "libraries database")) {
        return -2;
    }

    printLibraryHeader();
    lib l;
    for(int j = 0; j < readableLibsCnt; j++) {
        fread(&l, sizeof(lib), 1, libsDb);
        if (-1 != getLibIdxByKey(l.key)) {
            printLibrary(l);
            printf("\n");
        }
    }
    printLibraryFooter();
    fclose(libsDb);

    return 0;
}

int printLibByKey(int libKey) {
    int libIdx = getLibIdxByKey(libKey);
    if (0 > libIdx) {
        printf("No library with such key.\n");
        return -1;
    }

    FILE* libsDbFl = NULL;
    if (0 != openFileSafe(&libsDbFl, libsDbFileName, "rb", "libraries database")) {
        return -2;
    }

    lib l;
    fseek(libsDbFl, libIdx * sizeof(lib), SEEK_SET);
    fread(&l, sizeof(lib), 1, libsDbFl);

    printLibraryHeader();
    printLibrary(l);
    printf("\n");
    printLibraryFooter();
    fclose(libsDbFl);

    return 0;
}

int printAllBooks() {
    if (actualBooksCnt == 0) {
        printf("There are no books at moment ;(\n");
        return 0;
    }
    FILE* booksDbFl = NULL;
    if (0 != openFileSafe(&booksDbFl, booksDbFileName, "rb", "books database")) {
        return -2;
    }

    printBooksHeader();
    book b;
    for(int j = 0; j < readableBooksCnt; j++) {
        fread(&b, sizeof(book), 1, booksDbFl);
        if (-1 != getBookIdxByKey(b.key)) {
            printBook(b);
            printf("\n");
        }
    }
    printBooksFooter();
    fclose(booksDbFl);

    return 0;
}

int printBooksByLibKey(int libKey) {
    int libIdx = getLibIdxByKey(libKey);
    if (libIdx < 0) {
        printf("No library with such key.\n");
        return -1;
    }

    FILE* libsDbFl = NULL;
    if (0 != openFileSafe(&libsDbFl, libsDbFileName, "rb", "libraries database")) {
        return -2;
    }

    lib l;
    fseek(libsDbFl, libIdx * sizeof(lib), SEEK_SET);
    fread(&l, sizeof(lib), 1, libsDbFl);
    fclose(libsDbFl);

    int curBookIdx = l.firstBookIdx;
    if (curBookIdx == -1) {
        printf("Library with key %d has no books yet.\n", libKey);
        return 0;
    }

    FILE* booksDbFl = NULL;
    if (0 != openFileSafe(&booksDbFl, booksDbFileName, "rb", "books database")) {
        return -2;
    }

    printBooksHeader();
    book b;
    while (curBookIdx != -1) {
        fseek(booksDbFl, curBookIdx * sizeof(book), SEEK_SET);
        fread(&b, sizeof(book), 1, booksDbFl);
        printBook(b);
        printf("\n");
        curBookIdx = b.nextBookIdx;
    }
    printBooksFooter();

    fclose(booksDbFl);
    return 0;
}

int addFreeBookIdx(int bookIdx, int freeBookIdxsCnt) {
    FILE* booksGzFl = NULL;
    if (0 != openFileSafe(&booksGzFl, booksGzFileName, "r+b", "books garbage zone")) {
        return -2;
    }

    fseek(booksGzFl, freeBookIdxsCnt * sizeof(int), SEEK_SET);
    fwrite(&bookIdx, sizeof(int), 1, booksGzFl);

    fclose(booksGzFl);
    return 0;
}


int getLibByIdx(lib* l, int libIdx) {
    FILE* libsDbFl = NULL;
    if (0 != openFileSafe(&libsDbFl, libsDbFileName, "rb", "libraries database")) {
        return -2;
    }

    fseek(libsDbFl, libIdx * sizeof(lib), SEEK_SET);
    fread(l, sizeof(lib), 1, libsDbFl);

    fclose(libsDbFl);
    return 0;
}

int deleteBook(int bookKey) {
    int bookIdx = getBookIdxByKey(bookKey);
    if (0 > bookIdx) {
        printf("Wrong input: no book with key %d.\n", bookKey);
        return -1;
    }

    book b;
    if (0 != getBookByIdx(&b, bookIdx)) {
        return -1;
    }

    if (0 != addBookRecToIdxFl(bookKey, -1)) {
        return -1;
    }
    if (0 != addFreeBookIdx(bookIdx, MAX_BOOKS_COUNT - actualBooksCnt)) {
        return -1;
    }

    lib l;
    int libIdx = getLibIdxByKey(b.libKey);
    if (0 != getLibByIdx(&l, libIdx)) {
        return -1;
    }

    if (l.firstBookIdx == bookIdx) { // first book in lib
        l.firstBookIdx = b.nextBookIdx;
        writeLibToDbOnIdx(&l, getLibIdxByKey(l.key));
    } else {
        int curBookIdx = -1;
        int nextBookIdx = l.firstBookIdx;

        while (bookIdx != nextBookIdx) { // book to delete is next
            getBookByIdx(&b, nextBookIdx);
            curBookIdx = nextBookIdx;
            nextBookIdx = b.nextBookIdx;
        }

        book bToDel;
        getBookByIdx(&bToDel, nextBookIdx);
        b.nextBookIdx = bToDel.nextBookIdx;
        writeBookToDbOnIdx(&b, curBookIdx);
    }
    l.booksCnt--;
    writeLibToDbOnIdx(&l, libIdx);

    actualBooksCnt--;
    return 0;
}

int interactWithDb() {
    printf("\nChoose an option (H or 12 to see cheatsheet):\n\n");

    char buf[9];
    scanf("%2s", buf);
    fflush(stdin);
    printf("\n");
    if (strcmp(buf, "H") == 0) {
        printCheatsheet();
        return 0;
    } else if (strcmp(buf, "Q") == 0) {
        return 1; // TODO: add save
    }
    int option = atoi(buf);
    if (option < 1 || option > 12) {
        printf("Invalid input: no such option. Try again.\n");
        fflush(stdin);
        return -1;
    }

    int success;
    switch (option) {
        case 1: {
            if (0 == addLib()) {
                printf("Library successfully added.\n");
            } else {
                printf("Failed to add library.\n");
                return -1;
            }
            break;
        }
        case 2: {
            if (0 == addBook()) {
                printf("Book successfully added.\n");
            } else {
                printf("Failed to add book.\n");
                return -1;
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
            success = 1;
            printf("Enter library key:\n\n");
            scanf("%s", buf);
            fflush(stdin);
            printf("\n");
            if (!isValidInt(buf, strlen(buf))) {
                printf("Invalid input: no such library key: `%s`.\n", buf);
                success = 0;
            }
            int libKey = atoi(buf);
            if (!success || 0 != printLibByKey(libKey)) {
                printf("Failed to print library.\n");
            }
            break;
        }
        case 5: {
            if (0 != printAllBooks()) {
                printf("Failed to print books.\n");
            };
            break;
        }
        case 6: {
            success = 1;
            printf("Enter library key:\n\n");
            scanf("%s", buf);
            fflush(stdin);
            printf("\n");
            if (!isValidInt(buf, strlen(buf))) {
                printf("Invalid input: no such library key: `%s`.\n", buf);
                success = 0;
            }
            int libKey = atoi(buf);
            if (!success || 0 != printBooksByLibKey(libKey)) {
                printf("Failed to print library.\n");
            }
            break;
        }
        case 7: {
            success = 1;
            printf("Enter book key:\n\n");
            scanf("%s", buf);
            fflush(stdin);
            printf("\n");
            if (!isValidInt(buf, strlen(buf))) {
                printf("Invalid input: no such book key: `%s`.\n", buf);
                success = 0;
            }
            int bookKey = atoi(buf);
            if (success && 0 == deleteBook(bookKey)) {
                printf("Book successfully deleted.\n");
            } else {
                printf("Failed to delete book.\n");
                return -1;
            }
            break;
        }
        case 8: {

        }
        case 9: {

        }
        case 10: {

        }
        case 11: {
            return 1;
        }
        case 12: {
            printCheatsheet();
            return 0;
        }
        default: {
            return -2;
            break;
        }
    }

    return 0;
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
    srand(time(0));
    initFiles();
    while(1 != interactWithDb());
    return 0;
}
