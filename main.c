#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//library: setNumber, address, name, isState, books
// * book: setNumber, name, author, isAvailable, nextId

#define MAX_PHONE_NUMBER_LEN 13
#define MAX_LIBRARY_NAME_LEN 20
#define MAX_BOOK_NAME_LEN 20
#define MAX_BOOK_AUTHOR_LEN 20
#define MAX_LIBRARIES_COUNT 10
#define MAX_BOOKS_COUNT 20

int freeLibKey = 0;
int actualLibsCnt = 0;
int freeBookKey = 0;
int actualBooksCnt = 0;

const char libDbFileName[] = "libraries.data";
const char libIdxFileName[] = "libraries.idx";
const char booksDbFileName[] = "books.data";
const char booksIdxFileName[] = "books.idx";

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
    printf("*******************************************************************************\n\n");
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
    printf("**********************************************************************\n\n");
}

int printAllBooks() {
    int j = 0;
    printBooksHeader();
    while(j < booksPrimaryKey) {
        printBookByIndex(j);
        j++;
    }
    printBooksFooter();
    return 0;
}

int Entering() {

    printf("\nChoose an option:\n\n"
           "1) Add new library\n"
           "2) Add new book\n"
           "3) Display all the libraries\n"
           "4) Display library by it's key\n"
           "5) Display all the books\n"
           "6) Display books of specific library\n"
           "7) Delete library\n"
           "8) Delete book\n"
           "9) Modify data about library (only address or/and phone number)\n"
           "10) Modify data about book (only name and author)\n"
           "11) Terminate work with database (Q)\n\n");
    printf("LIST OF COMMANDS:\n INPUT(I)- use it to create new Library(L) or Book(B)\n UPDATE(U) - use it to update information of the Library(L) or of the Book(B)\n DELETE(D) - use it to delete the Library(L) by key ,\n or Book by company key and passport \n SHOW(S) - use it to show information about Libraries(L) or about Books(B)\n EXIT(Q) - use it to exit program\n");

    char optionStr[2];
    scanf("%2s", optionStr);
    int option = atoi(optionStr);
    if (option < 1 || option > 11) {
        printf("Invalid input: no such option. Try again.\n\n");
        return -1;
    }

    switch (option) {
        case 1: {
            createLib();
            break;
        }
        case 2: {
            createBook();
            break;
        }
        case 3: {
            printAllLibs();
            break;
        }
        case 4: {
            printf("\nEnter library key: ");
            char buf[9];
            scanf("%9s", buf);
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
            printLibByKey(libKey);
        }
        case 5: {
            printAllBooks();
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


    if(strcmp(option, "INPUT") == 0 || strcmp(option, "I") == 0) {
        printf("\nLIBRARY(L) | BOOK (B)\n\n");
        scanf("%s", option);
        if(strcmp(option, "LIBRARY") == 0 || strcmp(option, "L") == 0) {
            createLib();
        }
        else if(strcmp(option, "BOOK") == 0 || strcmp(option, "B") == 0) {
            createBook();
        }
        else {
            printf("\nUNKNOWN COMMAND\n\n");
        }
    }
    else if(strcmp(option, "UPDATE") == 0 || strcmp(option, "U") == 0) {
        printf("\nLIBRARY(L) | BOOK (B)\n\n");
        scanf("%s", option);
        if(strcmp(option, "LIBRARY") == 0 || strcmp(option, "L") == 0) {
            AllIndexes();
            int num;
            printf("\nChoose library key\n\n");
            scanf("%d",&num);
            rewriteLib(num);
        }
        else if(strcmp(option, "BOOK") == 0 || strcmp(option, "B") == 0) {
            AllIndexes();
            printAllBooks();
            int c;
            int num;
            printf("\nChoose Library and key of the Book\n\n");
            scanf("%d%d",&c,&num);
            Reinput_Programmer(c,num);
        }
        else {
            printf("\nUNKNOWN COMMAND\n\n");
        }
    }
    else if(strcmp(option, "DELETE") == 0 || strcmp(option, "D") == 0) {
        printf("\nLIBRARY(L) | BOOK (B)\n\n");
        scanf("%s", option);
        if(strcmp(option, "LIBRARY") == 0 || strcmp(option, "L") == 0) {
            AllIndexes();
            char* num1[50];
            printf("\nChoose library key\n\n");
            scanf("%s", num1);
            int num=atoi(num1);
            printf("%d",num);
            Delete_Company(num);
        }
        else if(strcmp(option, "BOOK") == 0 || strcmp(option, "B") == 0) {
            AllIndexes();
            printAllBooks();
            int c;
            int num;
            printf("\nChoose Library and key of the Book\n\n");
            scanf("%d%d",&c,&num);
            Delete_programmer(c,num);
        }
        else {
            printf("\nUNKNOWN COMMAND\n\n");
        }
    }
    else if(strcmp(option, "SHOW") == 0 || strcmp(option, "S") == 0) {
        printf("\nLIBRARY(L) | BOOK(B) | INDEX(I)\n\n");
        scanf("%s", option);
        if(strcmp(option, "LIBRARY") == 0 || strcmp(option, "L") == 0) {
            printf("\nChoose ALL|ONE\n\n");
            scanf("%s", option);
            if(strcmp(option, "ALL") == 0) { printAllLibs();}
            else if(strcmp(option, "ONE") == 0) {
                int libKey;
                AllIndexes();
                printf("\nChoose library libKey\n\n");
                scanf("%d", &libKey);
                printf("----------------------------LIBRARIES----------------------------\n\n");
                printf(" Key:       Name:       Address:       Amount of books:       Phone number:\n\n");
                printLibByKey(libKey);
                printf("-----------------------------------------------------------------\n");
            }
            else {
                printf("UNKNOWN COMMAND\n\n");
            }
        }
        else if(strcmp(option, "BOOK") == 0 || strcmp(option, "B") == 0) {
            printf("\nChoose ALL | BY LIBRARY(key)\n\n");
            scanf("%s", option);
            if(strcmp(option, "ALL") == 0) {
                printAllBooks();
            }
            else if(strcmp(option, "LIBRARY") == 0 || strcmp(option, "L") == 0) {
                AllIndexes();
                int c;
                printf("\nChoose Library\n\n");
                scanf("%d",&c);
                Find_Programmers_by_company(c);
            }
            else {
                printf("UNKNOWN COMMAND\n\n");
            }

        }
        else if(strcmp(option, "INDEX") == 0 || strcmp(option, "I") == 0) {
            AllIndexes();
        }
        else {
            printf("UNKNOWN COMMAND\n\n");
        }
    }
    else if(strcmp(option, "EXIT") == 0 || strcmp(option, "X") == 0) {
        return 0;
    }
    else if(strcmp(option, "HELP") == 0 || strcmp(option, "H") == 0) {
        printf("LIST OF COMMANDS:\n INPUT(I)- use it to create new Library(L) or Book(B)\n UPDATE(U) - use it to update information of the Library(L) or of the Book(B)\n DELETE(D) - use it to delete the Library(L) by key ,\n or Book by company key and passport \n SHOW(S) - use it to show information about Libraries(L) or about Books(B)\n EXIT(Q) - use it to exit program\n");
    }
    else {
        printf("UNKNOWN COMMAND\n\n");
    }
    return 1;
}

int Save() {
    FILE* out_save;
    out_save=fopen(backupFileName,"r+b");
    if (out_save  ==  NULL)  {
        out_save = fopen(backupFileName, "w+b");
        printf("Couldn't find the file. The file is created and process is terminated.\n");
    }
    fseek(out_save,0,SEEK_SET);
    fwrite(&librariesPrimaryKey,sizeof(int),1,out_save);
    fseek(out_save,sizeof(int),SEEK_SET);
    fwrite(&booksPrimaryKey,sizeof(int),1,out_save);
    fseek(out_save,2*sizeof(int),SEEK_SET);
    fwrite(&idx,sizeof(int),1,out_save);
    fclose(out_save);
}
int Open() {
    FILE* in_save;
    in_save=fopen(backupFileName,"r+b");
    fseek(in_save,0,SEEK_SET);
    fread(&librariesPrimaryKey,sizeof(int),1,in_save);
    fseek(in_save,sizeof(int),SEEK_SET);
    fread(&booksPrimaryKey,sizeof(int),1,in_save);
    fseek(in_save,2*sizeof(int),SEEK_SET);
    fread(&idx,sizeof(int),1,in_save);
    fclose(in_save);
}

int main()
 {
    printf("OPEN PREVIOUS DATA BASE OR CREATE NEW?\n\nPRESS 1 - TO USE PREVIOUS \nPRESS 0 - TO CREATE NEW\n\n");
    char answer[20];
    scanf("%s", answer);
    int result = atoi(answer);
    if(result == 0) {ClearFiles();
        Deleted_array_C();
        Deleted_array_S();}
    else {
        Open();
    }
    printf("\n(ENTER \"HELP\" TO SEE LIST OF COMMANDS)\n");
    while(1) {
        if(Entering() == 0)  {
            break;
        }
        Save();
    }
//    FILE* f = fopen("./asdd.ss", "w+b");
//    book b;
//    b.isExist = 0;
//    b.nextBookIdx = 2;
//    strcpy(b.author, "d");
//    strcpy(b.name, "dd");
//    b.libKey = 123;
//    b.setNumber = 123123;
//    int k = 123130;
////    fwrite(&k, sizeof(int), 1, f);
//    fwrite(&b, sizeof(book), 1, f);
//    fseek(f, 0, SEEK_SET);
//    book bb, bbb;
//    fread(&bb, sizeof(book), 1, f);
//    printBook(&bb);
//    int cur = ftell(f);
//    fseek(f, 0, SEEK_END);
//    if (cur == ftell(f)) {
//        printf("Finish");
//        return 0;
//    }
//    fread(&bbb, sizeof(book), 1, f);
//    printBook(&bbb);
//    fclose(f);
    return 0;
}
