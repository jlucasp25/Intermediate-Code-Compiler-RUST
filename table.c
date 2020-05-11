#ifndef TABLEC
#define TABLEC

#include <stdlib.h>
#include <string.h>
#include "table.h"


SymbolList* symbolListConstructor(Entry* entry, SymbolList* next) {
    SymbolList* l = (SymbolList*) malloc(sizeof(SymbolList));
    l->entry = entry;
    l->next = next;
    return l;
}

void appendSymbolList(SymbolList* left, SymbolList* right) {
    SymbolList* aux = left;
    while (aux->next != NULL) {
        aux = aux->next;
    }
    aux->next = right;
}

Entry* getSymbolListHead(SymbolList* l) {
    return l->entry;
}

SymbolList* getSymbolListTail(SymbolList* l) {
    return l->next;
}

Entry* entryConstructor(char* regname, char* name) {
    Entry* e = (Entry*) malloc(sizeof(Entry));
    e->regname = strdup(regname);
    e->name = strdup(name);
    return e;
}

bool checkPresence(SymbolList* l, char* name) {
    SymbolList* aux = l;
    while (aux != NULL) {
        if (!strcmp(aux->entry->name,name))
            return true;
        aux = aux->next;
    }
    return false;
}

char* getAssociatedRegister(SymbolList* l,char* name) {
    SymbolList* aux = l;
    while (aux != NULL) {
        if (!strcmp(aux->entry->name,name))
            return aux->entry->regname;
        aux = aux->next;
    }
    return NULL;
}

#endif