#ifndef TABLEH
#define TABLEH

#include <stdbool.h>

/////////////////////
// Data Structures //
/////////////////////

//Table Entry
typedef struct _Entry {
    char* regname;
    char* name;
} Entry;

//Symbol Table
typedef struct _SymbolList {
    Entry* entry;
    struct _SymbolList* next;
} SymbolList;

///////////////////////
// Function Headers  //
///////////////////////

//Symbol Table builder and functions
SymbolList* symbolListConstructor(Entry* entry, SymbolList* next);
Entry* getSymbolListHead(SymbolList* l);
SymbolList* getSymbolListTail(SymbolList* l);
void appendSymbolList(SymbolList* left, SymbolList* right);
//Entry builders and functions
Entry* entryConstructor(char* regname, char* name);
bool checkPresence(SymbolList* l, char* name);
char* getAssociatedRegister(SymbolList* l,char* name);

#endif