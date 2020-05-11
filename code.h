#ifndef CODEH
#define CODEH

#include "ast.h"

//  code.h
// Contains instruction structures, compile functions and builders
// Francisco Gonçalves, up201604505
// João Pires, up201606617

/////////////////////
// Data Structures //
/////////////////////

//Type of instruction
typedef enum InstrKind { 
    //Expr
    I_ATTRIB,
    I_ADD,
    I_SUB,
    I_MULT,
    I_DIV,
    //ExprBool
    I_EQUAL,
    I_NOTEQUAL,
    I_GREATERTHAN,
    I_GREATEREQUAL,
    I_LESSTHAN,
    I_LESSEQUAL,
    //Cmd
    I_GOTO,
    I_LABEL,
    I_IFTHENELSE,
    I_READ,
    I_PRINT
} InstrKind;

//Type of atom
typedef enum AtomKind { NUMBER, VAR } AtomKind;

//Atom (An atom can contain a variable name or integer value.)
typedef struct _Atom {
    AtomKind kind;
    union {
        char* var;
        int val;
    } u;
} Atom;

//Instruction 
// Three address instruction --> reg := "kind", at1, at2 
typedef struct _Instr {
    InstrKind kind;
    Atom* addr1;
    Atom* addr2;
    Atom* addr3;
} Instr;

//List of instructions
typedef struct _InstrList {
    Instr* data;
    struct _InstrList* next;
} InstrList;


///////////////////////
// Function Headers  //
///////////////////////

//Instruction builders
Instr* InstrConstructor(InstrKind kind, Atom* a1, Atom* a2,Atom* a3);

//Atom builders
Atom* AtomValConstructor(int val);
Atom* AtomVarConstructor(char* var);

//List of instructions builder and functions
InstrList* InstrListConstructor(Instr* data, InstrList* next);
void appendList(InstrList* left, InstrList* right);
Instr* getHead(InstrList* l);
InstrList* getTail(InstrList* l);

//Compile commands, command lists and expressions.
InstrList* compileCmd(Cmd* c);
InstrList* compileCmdList(CmdList* l);
InstrList* compileExpr(Expr* e);

//Compile sub-commands
InstrList* compileIfThen(IfThen* cmd);
InstrList* compileIfThenElse(IfThenElse* cmd);
InstrList* compileWhile(While* cmd);
InstrList* compileAssign(Assign* cmd);
InstrList* compilePrint(IO* cmd);
InstrList* compileRead(IO* cmd);
InstrList* compileDeclaration(Declaration* cmd);

//Other functions
void printInstr(Instr* i);
void printInstrList(InstrList* l);
char* generateRegister();
char* generateLabel();

#endif