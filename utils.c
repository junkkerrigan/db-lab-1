#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

void logBook(book* b) {
    printf("key: %d\nlibKey: %d\nname: %s\nauthor: %s\nisAvailable: %d\nnextBookAddr: %d\n",
           b->key, b->libKey, b->name, b->author, b->isAvailable, b->nextBookIdx);
}

void logLibrary(lib* l) {
    printf("key: %d\nname: %s\nphoneNumber: %s\nisState: %s\nbooksCnt: %d\n",
           l->key, l->name, l->phoneNumber, (l->isState == 1)? "state" : "private", l->booksCnt);
}

void printLibraryHeader() {
    printf("************************************    Libraries    **********************************************\n\n");
    printf("    Key        Name                  Type        Amount of books       Phone number       Index\n\n");
}

void printLibrary(lib l, int libIdx) {
    printf("    %-11d", l.key);
    printf("%-22s", l.name);
    printf("%-12s", (l.isState ? "State" : "Private"));
    printf("%-22d", l.booksCnt);
    printf("%-19s", l.phoneNumber);
    printf("%d\n", libIdx);
}

void printLibraryFooter() {
    printf("***************************************************************************************************\n");
}

long getFileSize(FILE* f) {
    long cur = ftell(f);
    fseek(f, 0, SEEK_END);
    long ans = ftell(f);
    fseek(f, cur, SEEK_SET);
    return ans;
}

void printBook(book b, int bookIdx) {
    printf("    %-11d", b.key);
    printf("%-23s", b.name);
    printf("%-22s", b.author);
    printf("%-16s", (b.isAvailable ? "Available" : "In use"));
    printf("%-18d", b.libKey);
    printf("%d\n", bookIdx);
}

void printBooksHeader() {
    printf("****************************************     Books     ***********************************************\n\n");
    printf("    Key        Name                   Author                Status          Library key       Index\n\n");
}

void printBooksFooter() {
    printf("******************************************************************************************************\n");
}

void generateRandString(char *str, size_t size)
{
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJK...";
    if (size) {
        --size;
        for (size_t n = 0; n < size; n++) {
            int key = rand() % (int) (sizeof charset - 1);
            str[n] = charset[key];
        }
        str[size] = '\0';
    }
}

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

void generateLib(lib* l) {
    initLib(l);
    generateRandString(l->name, 10);
    generateRandString(l->phoneNumber, 10);
    l->isState = 1;
}

void generateBook(book* b) {
    initBook(b);
    generateRandString(b->name, 10);
    generateRandString(b->author, 10);
    b->isAvailable = 1;
}