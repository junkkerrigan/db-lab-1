#ifndef DB_LB_1_CONSTANTS_H
#define DB_LB_1_CONSTANTS_H

#endif //DB_LB_1_CONSTANTS_H


#define MAX_PHONE_NUMBER_LEN 14
#define MAX_LIBRARY_NAME_LEN 21
#define MAX_BOOK_NAME_LEN 21
#define MAX_BOOK_AUTHOR_LEN 21
#define MAX_LIBRARIES_COUNT 3
#define MAX_BOOKS_COUNT 6

#define _libsDbFileName "libraries.data";
#define _libsIdxFileName "libraries.idx";
#define _libsGzFileName "libraries.gz";
#define _booksDbFileName "books.data";
#define _booksIdxFileName "books.idx";
#define _booksGzFileName "books.gz";

extern const char libsDbFileName[];
extern const char libsIdxFileName[];
extern const char libsGzFileName[];
extern const char booksDbFileName[];
extern const char booksIdxFileName[];
extern const char booksGzFileName[];

extern const int idxFileOffset;
