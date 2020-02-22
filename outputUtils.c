#include <stdio.h>
#include "outputUtils.h"

void logBook(book* b) {
    printf("key: %d\nlibKey: %d\nname: %s\nauthor: %s\nisAvailable: %d\nnextBookAddr: %d\n",
           b->key, b->libKey, b->name, b->author, b->isAvailable, b->nextBookIdx);
}

void logLibrary(lib* l) {
    printf("key: %d\nname: %s\nphoneNumber: %s\nisState: %d\nfirstBookAddr: %d\nbooksCnt: %d\n",
           l->key, l->name, l->phoneNumber, l->isState, l->firstBookIdx, l->booksCnt);
}

void printLibraryHeader() {
    printf("******************************    Libraries    ********************************\n\n");
    printf("    Key        Name        Type       Amount of books       Phone number \n\n");
}

void printLibraryFooter() {
    printf("*******************************************************************************\n");
}

long getFileSize(FILE* f) {
    long cur = ftell(f);
    fseek(f, 0, SEEK_END);
    long ans = ftell(f);
    fseek(f, cur, SEEK_SET);
    return ans;
}

void printLibrary(lib* l) {
    printf("    %-11d", l->key);
    printf("%-12s", l->name);
    printf("%-11s", (l->isState ? "State" : "Private"));
    printf("%-22d", l->booksCnt);
    printf("%s\n", l->phoneNumber);
}

void printBook(book* b) {
    printf("    %-11d", b->key);
    printf("%-13s", b->name);
    printf("%-14s", b->author);
    printf("%-13s", (b->isAvailable ? "Available" : "In use"));
    printf("%d\n", b->libKey);
}

void printBooksHeader() {
    printf("*****************************     Books     **************************\n\n");
    printf("    Key        Name         Author        Status       Library key \n\n");
}

void printBooksFooter() {
    printf("**********************************************************************\n");
}
