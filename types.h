#ifndef DB_LB_1_TYPES_H
#define DB_LB_1_TYPES_H

#endif //DB_LB_1_TYPES_H

#include "constants.h"

typedef struct library {
    int key;
    char name[MAX_LIBRARY_NAME_LEN];
    char phoneNumber[MAX_PHONE_NUMBER_LEN];
    int isState;
    int firstBookIdx;
    int booksCnt;
} lib;

typedef struct book {
    int key;
    char name[MAX_BOOK_NAME_LEN];
    char author[MAX_BOOK_AUTHOR_LEN];
    int isAvailable;
    int libKey;
    int nextBookIdx;
} book;

typedef struct indexRecord {
    int key;
    long idx;
} idxRec;
