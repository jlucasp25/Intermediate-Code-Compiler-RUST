#ifndef CODEC
#define CODEC

#include "code.h"
#include "table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

int COUNTER = 1;
int LABEL_COUNTER = 1;
SymbolList* table;

//  code.c
// Contains compile functions and builders
// Francisco Gonçalves, up201604505
// João Pires, up201606617


//////////////////////////
// Instruction Builders //
//////////////////////////

//Instruction builder
Instr* InstrConstructor(InstrKind kind, Atom* a1, Atom* a2,Atom* a3) {
    Instr* i = (Instr*) malloc(sizeof(Instr));
    i->kind = kind;
    i->addr1 = a1;
    i->addr2 = a2;
    i->addr3 = a3;
    return i;
}

///////////////////
// Atom builders //
///////////////////

//Atom builder from values
Atom* AtomValConstructor(int val) {
    Atom* a = (Atom*) malloc(sizeof(Atom));
    a->kind = NUMBER;
    a->u.val = val;
    return a;
}

//Atom builder from variables/temporaries
Atom* AtomVarConstructor(char* var) {
    Atom* a = (Atom*) malloc(sizeof(Atom));
    a->kind = VAR;
    a->u.var = strdup(var);
    return a;
}

////////////////////////////////////////////////
// List of instructions builder and functions //
////////////////////////////////////////////////

//Instruction list builder
InstrList* InstrListConstructor(Instr* data, InstrList* next) {
    InstrList * l = (InstrList*) malloc(sizeof(InstrList));
    l->data = data;
    l->next = next;
    return l;
}

//Appends two instruction lists, left one is the final one.
void appendList(InstrList* left, InstrList* right){
    InstrList* aux = left;

    while (aux->next != NULL)
        aux = aux->next;

    aux->next = right;
}

//Returns the data on the list's head
Instr* getHead(InstrList* l) {
    return l->data;
}

//Returns the list's tail
InstrList* getTail(InstrList* l) {
    return l->next;
}

//Returns the list's last element
InstrList* getLast(InstrList* l) {
    InstrList* aux = l;
    while (aux->next != NULL) {
        aux = aux->next;
    }
    return aux;
}

/////////////////////////////////////////////////////
// Compile commands, command lists and expressions //
/////////////////////////////////////////////////////

//Compiles a command into an instructions list
InstrList* compileCmd(Cmd* c) {
    InstrList* l;
    switch (c->kind) {
        case C_WHILE:
            l = compileWhile(c->op.whil);
        break;
        case C_IF_THEN:
            l = compileIfThen(c->op.ifthen);
        break; 
        case C_IF_THEN_ELSE:
            l = compileIfThenElse(c->op.ifthenelse);
        break;
        case C_ASSIGN:
            l = compileAssign(c->op.assign);
        break;
        case C_PRINT:
            l = compilePrint(c->op.printread);
        break;
        case C_READ:
            l = compileRead(c->op.printread);
        break;
        case C_DECLARATION:
            l = compileDeclaration(c->op.declaration);
        break;
        default: printf("ERROR ON COMPILE_CMD!\n"); exit(-1);
    }
    return l;
}

//Compiles a list of commands into an instructions list
InstrList* compileCmdList(CmdList* l) {
    CmdList* aux = l;
    InstrList* auxList;
    InstrList* finalList = compileCmd(aux->data);
    if (aux->next == NULL)
        return finalList;
    else {
        aux = aux->next;
        while (aux != NULL) {
            auxList = compileCmd(aux->data);
            appendList(finalList,auxList);
            aux = aux->next;
        }
        return finalList;
    }
    return NULL;
}

//Compiles an expression (recursively) into an instructions list
InstrList* compileExpr(Expr* e) {
    if (e->kind == E_INTEGER) {
        return InstrListConstructor(InstrConstructor(I_ATTRIB,AtomVarConstructor(generateRegister()),AtomValConstructor(e->attr.value),NULL),NULL);
    }
    else if (e->kind == E_VARIABLE) {
        return InstrListConstructor(InstrConstructor(I_ATTRIB,AtomVarConstructor(generateRegister()),AtomVarConstructor(e->attr.var),NULL),NULL);
    }
    else if (e->kind == E_OPERATION) {
        InstrList* left = compileExpr(e->attr.op.left);
        char* leftRegister = strdup(getLast(left)->data->addr1->u.var);
        InstrList* right = compileExpr(e->attr.op.right);
        char* rightRegister = strdup(getLast(right)->data->addr1->u.var);
        appendList(left,right);
        switch (e->attr.op.operator) {
            case PLUS: // +
                appendList(left,InstrListConstructor(InstrConstructor(I_ADD,AtomVarConstructor(generateRegister()),AtomVarConstructor(leftRegister),AtomVarConstructor(rightRegister)),NULL));
                break;
            case MINUS: // -
                appendList(left,InstrListConstructor(InstrConstructor(I_SUB,AtomVarConstructor(generateRegister()),AtomVarConstructor(leftRegister),AtomVarConstructor(rightRegister)),NULL));
                break; 
            case MULT: // *
                appendList(left,InstrListConstructor(InstrConstructor(I_MULT,AtomVarConstructor(generateRegister()),AtomVarConstructor(leftRegister),AtomVarConstructor(rightRegister)),NULL));
                break;
            case DIV: // /
                appendList(left,InstrListConstructor(InstrConstructor(I_DIV,AtomVarConstructor(generateRegister()),AtomVarConstructor(leftRegister),AtomVarConstructor(rightRegister)),NULL));
                break;
        }
        return left;
    }
    else {
        printf("ERROR ON COMPILE_EXPR!\n");
        exit(-1);
    }
}

//Compiles a boolean expression (recursively) into an instructions list
InstrList* compileExprBool(ExprBool* e) {
    if (e->kind == EB_BOOLEAN) {
        return InstrListConstructor(InstrConstructor(I_ATTRIB,AtomVarConstructor(generateRegister()),AtomValConstructor((int) e->attr.value),NULL),NULL);
    }
    else if (e->kind == EB_VARIABLE) {
        return InstrListConstructor(InstrConstructor(I_ATTRIB,AtomVarConstructor(generateRegister()),AtomVarConstructor(strdup(e->attr.var)),NULL),NULL);
    }
    else if (e->kind == EB_OPERATION) {
        InstrList* left = compileExprBool(e->attr.op.left);
        char* leftRegister = strdup(getLast(left)->data->addr1->u.var);
        InstrList* right = compileExprBool(e->attr.op.right);
        char* rightRegister = strdup(getLast(right)->data->addr1->u.var);
        appendList(left,right);
        switch (e->attr.op.operator) {
            case EQUAL: // ==
                appendList(left,InstrListConstructor(InstrConstructor(I_EQUAL,AtomVarConstructor(generateRegister()),AtomVarConstructor(leftRegister),AtomVarConstructor(rightRegister)),NULL));
                break;
            case DIF: // !=
                appendList(left,InstrListConstructor(InstrConstructor(I_NOTEQUAL,AtomVarConstructor(generateRegister()),AtomVarConstructor(leftRegister),AtomVarConstructor(rightRegister)),NULL));
                break; 
            case LT: // <
                appendList(left,InstrListConstructor(InstrConstructor(I_LESSTHAN,AtomVarConstructor(generateRegister()),AtomVarConstructor(leftRegister),AtomVarConstructor(rightRegister)),NULL));
                break;
            case GT: // >
                appendList(left,InstrListConstructor(InstrConstructor(I_GREATERTHAN,AtomVarConstructor(generateRegister()),AtomVarConstructor(leftRegister),AtomVarConstructor(rightRegister)),NULL));
                break;
            case LE: // <=
                appendList(left,InstrListConstructor(InstrConstructor(I_LESSEQUAL,AtomVarConstructor(generateRegister()),AtomVarConstructor(leftRegister),AtomVarConstructor(rightRegister)),NULL));
                break;
            case GE: // >=
                appendList(left,InstrListConstructor(InstrConstructor(I_GREATEREQUAL,AtomVarConstructor(generateRegister()),AtomVarConstructor(leftRegister),AtomVarConstructor(rightRegister)),NULL));
                break;
        }
        return left;
    }
    else {
        printf("ERROR ON COMPILE_EXPR!\n");
        exit(-1);
    }
}


//Compiles boolean expression and creates an IFTHENELSE instruction with the labels provided
//$tx := boolean result
//$t(x+1) := if $tx == true goto label1 else goto label2
InstrList* compileExprBoolLabel(ExprBool* e, char* labelTrue, char* labelFalse) {
    InstrList* l = compileExprBool(e);
    char* result = getLast(l)->data->addr1->u.var;
    appendList(l, InstrListConstructor(InstrConstructor(I_IFTHENELSE,AtomVarConstructor(result),AtomVarConstructor(labelTrue),AtomVarConstructor(labelFalse)),NULL));
    return l;
}

//Compiles attribution instruction
InstrList* compileAssign(Assign* cmd) {
    InstrList* l = compileExpr(cmd->rightSide);
    char* result = getLast(l)->data->addr1->u.var;
    bool exists = checkPresence(table,cmd->leftSide);
    if (!exists) {
        printf("COMPILEASSIGN ERROR: Cannot assign to variable that doesnt exist\n");
        exit(-1);
    }
    appendList(l,InstrListConstructor(InstrConstructor(I_ATTRIB,AtomVarConstructor(generateRegister()),AtomVarConstructor(cmd->leftSide),AtomVarConstructor(result)),NULL));
    return l;
}

InstrList* compileDeclaration(Declaration* cmd) {
    char* current = generateRegister();
    
    if (table == NULL)
        table = symbolListConstructor(entryConstructor(current,cmd->name),NULL);
    else {
        bool exists = checkPresence(table,cmd->name);
        if (exists) {
            printf("COMPILE_DECLARATION ERROR : DECLARED VARIABLE ALREADY EXISTS!!\n");
            exit(-1);
        }
        appendSymbolList(table,symbolListConstructor(entryConstructor(current,cmd->name),NULL));
    }
    return InstrListConstructor(InstrConstructor(I_ATTRIB,AtomVarConstructor(generateRegister()),AtomVarConstructor(cmd->name),NULL),NULL);
}

//Compiles IF-THEN command
// code1 ++ [LABEL label 1] ++ code2 ++ [LABEL label 2]
InstrList* compileIfThen(IfThen* cmd) {
    char* label1 = generateLabel();
    char* label2 = generateLabel();
    InstrList* l = compileExprBoolLabel(cmd->condition,label1,label2);
    appendList(l,InstrListConstructor(InstrConstructor(I_LABEL,AtomVarConstructor(label1),NULL,NULL),NULL));
    appendList(l,compileCmdList(cmd->execute));
    appendList(l,InstrListConstructor(InstrConstructor(I_LABEL,AtomVarConstructor(label2),NULL,NULL),NULL));
    return l;
}

//Compiles IF-THEN-ELSE command
//code1 ++ [LABEL label 1] ++ code2 ++ [GOTO label 3, LABEL label 2] ++ code 3 ++ [LABEL label 3]
InstrList* compileIfThenElse(IfThenElse* cmd) {
    char* label1 = generateLabel();
    char* label2 = generateLabel();
    char* label3 = generateLabel();
    InstrList* l = compileExprBoolLabel(cmd->condition,label1,label2);
    appendList(l,InstrListConstructor(InstrConstructor(I_LABEL,AtomVarConstructor(label1),NULL,NULL),NULL));
    appendList(l,compileCmdList(cmd->execute1));
    appendList(l,InstrListConstructor(InstrConstructor(I_GOTO,AtomVarConstructor(label3),NULL,NULL),NULL));
    appendList(l,InstrListConstructor(InstrConstructor(I_LABEL,AtomVarConstructor(label2),NULL,NULL),NULL));
    appendList(l,compileCmdList(cmd->execute2));
    appendList(l,InstrListConstructor(InstrConstructor(I_LABEL,AtomVarConstructor(label3),NULL,NULL),NULL));
    return l;
}

//Compiles WHILE command
//[LABEL label1] ++ code1 ++ [LABEL label 2] ++ code2 ++ [GOTO label 1, LABEL label3]
InstrList* compileWhile(While* cmd) {
    char* label1 = generateLabel();
    char* label2 = generateLabel();
    char* label3 = generateLabel();
    InstrList* l = InstrListConstructor(InstrConstructor(I_LABEL,AtomVarConstructor(label1),NULL,NULL),NULL);
    appendList(l,compileExprBoolLabel(cmd->condition,label2,label3));
    appendList(l,InstrListConstructor(InstrConstructor(I_LABEL,AtomVarConstructor(label2),NULL,NULL),NULL));
    appendList(l,compileCmdList(cmd->execute));
    appendList(l,InstrListConstructor(InstrConstructor(I_GOTO,AtomVarConstructor(label1),NULL,NULL),NULL));
    appendList(l,InstrListConstructor(InstrConstructor(I_LABEL,AtomVarConstructor(label3),NULL,NULL),NULL));
    return l;
}

//Compiles READ command
InstrList* compileRead(IO* cmd) {
    return InstrListConstructor(InstrConstructor(I_READ,AtomVarConstructor(cmd->string),NULL,NULL),NULL);
}

//compiles PRINT ocmmand
InstrList* compilePrint(IO* cmd) {
    return InstrListConstructor(InstrConstructor(I_PRINT,AtomVarConstructor(cmd->string),NULL,NULL),NULL);
}

//Prints instructions to the screen
void printInstr(Instr* i) {
    
    if (i == NULL) {
        printf("Null instruction!!\n");
        return;
    }

    switch(i->kind) {
        case I_ATTRIB:
            printf("\t");
            if (i->addr2->kind == NUMBER)
                printf("%s := %d\n",i->addr1->u.var, i->addr2->u.val);
            else {
                if (i->addr3 == NULL)
                    printf("%s := %s\n",i->addr1->u.var, i->addr2->u.var);
                else
                    if (i->addr3->kind == NUMBER )
                        printf("%s := %s := %d\n",i->addr1->u.var, i->addr2->u.var, i->addr3->u.val);
                    else
                        printf("%s := %s := %s\n",i->addr1->u.var, i->addr2->u.var, i->addr3->u.var);
            }             
            break;
        case I_ADD:
            printf("\t");
            if (i->addr2->kind == NUMBER)
                if (i->addr3->kind == NUMBER)
                    printf("%s := %d + %d\n",i->addr1->u.var, i->addr2->u.val,i->addr3->u.val);
                else
                    printf("%s := %d + %s\n",i->addr1->u.var, i->addr2->u.val,i->addr3->u.var);
            else
                if (i->addr3->kind == NUMBER)
                    printf("%s := %s + %d\n",i->addr1->u.var, i->addr2->u.var,i->addr3->u.val);
                else
                    printf("%s := %s + %s\n",i->addr1->u.var, i->addr2->u.var,i->addr3->u.var);
            break; 
        case I_SUB:
            printf("\t");
            if (i->addr2->kind == NUMBER)
                if (i->addr3->kind == NUMBER)
                    printf("%s := %d - %d\n",i->addr1->u.var, i->addr2->u.val,i->addr3->u.val);
                else
                    printf("%s := %d - %s\n",i->addr1->u.var, i->addr2->u.val,i->addr3->u.var);
            else
                if (i->addr3->kind == NUMBER)
                    printf("%s := %s - %d\n",i->addr1->u.var, i->addr2->u.var,i->addr3->u.val);
                else
                    printf("%s := %s - %s\n",i->addr1->u.var, i->addr2->u.var,i->addr3->u.var);
            break;
        case I_MULT:
            printf("\t");
            if (i->addr2->kind == NUMBER)
                if (i->addr3->kind == NUMBER)
                    printf("%s := %d * %d\n",i->addr1->u.var, i->addr2->u.val,i->addr3->u.val);
                else
                    printf("%s := %d * %s\n",i->addr1->u.var, i->addr2->u.val,i->addr3->u.var);
            else
                if (i->addr3->kind == NUMBER)
                    printf("%s := %s * %d\n",i->addr1->u.var, i->addr2->u.var,i->addr3->u.val);
                else
                    printf("%s := %s * %s\n",i->addr1->u.var, i->addr2->u.var,i->addr3->u.var);
            break;
        case I_DIV:
            printf("\t");
            if (i->addr2->kind == NUMBER)
                if (i->addr3->kind == NUMBER)
                    printf("%s := %d / %d\n",i->addr1->u.var, i->addr2->u.val,i->addr3->u.val);
                else
                    printf("%s := %d / %s\n",i->addr1->u.var, i->addr2->u.val,i->addr3->u.var);
            else
                if (i->addr3->kind == NUMBER)
                    printf("%s := %s / %d\n",i->addr1->u.var, i->addr2->u.var,i->addr3->u.val);
                else
                    printf("%s := %s / %s\n",i->addr1->u.var, i->addr2->u.var,i->addr3->u.var);
            break;
        case I_EQUAL:
            printf("\t");
            if (i->addr2->kind == NUMBER)
                if (i->addr3->kind == NUMBER)
                    printf("%s := %d == %d\n",i->addr1->u.var, i->addr2->u.val,i->addr3->u.val);
                else
                    printf("%s := %d == %s\n",i->addr1->u.var, i->addr2->u.val,i->addr3->u.var);
            else
                if (i->addr3->kind == NUMBER)
                    printf("%s := %s == %d\n",i->addr1->u.var, i->addr2->u.var,i->addr3->u.val);
                else
                    printf("%s := %s == %s\n",i->addr1->u.var, i->addr2->u.var,i->addr3->u.var);
            break;
        case I_NOTEQUAL:
            printf("\t");
            if (i->addr2->kind == NUMBER)
                if (i->addr3->kind == NUMBER)
                    printf("%s := %d != %d\n",i->addr1->u.var, i->addr2->u.val,i->addr3->u.val);
                else
                    printf("%s := %d != %s\n",i->addr1->u.var, i->addr2->u.val,i->addr3->u.var);
            else
                if (i->addr3->kind == NUMBER)
                    printf("%s := %s != %d\n",i->addr1->u.var, i->addr2->u.var,i->addr3->u.val);
                else
                    printf("%s := %s != %s\n",i->addr1->u.var, i->addr2->u.var,i->addr3->u.var);
            break;
        case I_GREATERTHAN:
            printf("\t");
            if (i->addr2->kind == NUMBER)
                if (i->addr3->kind == NUMBER)
                    printf("%s := %d > %d\n",i->addr1->u.var, i->addr2->u.val,i->addr3->u.val);
                else
                    printf("%s := %d > %s\n",i->addr1->u.var, i->addr2->u.val,i->addr3->u.var);
            else
                if (i->addr3->kind == NUMBER)
                    printf("%s := %s > %d\n",i->addr1->u.var, i->addr2->u.var,i->addr3->u.val);
                else
                    printf("%s := %s > %s\n",i->addr1->u.var, i->addr2->u.var,i->addr3->u.var);
            break;
        case I_GREATEREQUAL:
            printf("\t");
            if (i->addr2->kind == NUMBER)
                if (i->addr3->kind == NUMBER)
                    printf("%s := %d >= %d\n",i->addr1->u.var, i->addr2->u.val,i->addr3->u.val);
                else
                    printf("%s := %d >= %s\n",i->addr1->u.var, i->addr2->u.val,i->addr3->u.var);
            else
                if (i->addr3->kind == NUMBER)
                    printf("%s := %s >= %d\n",i->addr1->u.var, i->addr2->u.var,i->addr3->u.val);
                else
                    printf("%s := %s >= %s\n",i->addr1->u.var, i->addr2->u.var,i->addr3->u.var);
            break;
        case I_LESSTHAN:
            printf("\t");
            if (i->addr2->kind == NUMBER)
                if (i->addr3->kind == NUMBER)
                    printf("%s := %d < %d\n",i->addr1->u.var, i->addr2->u.val,i->addr3->u.val);
                else
                    printf("%s := %d < %s\n",i->addr1->u.var, i->addr2->u.val,i->addr3->u.var);
            else
                if (i->addr3->kind == NUMBER)
                    printf("%s := %s < %d\n",i->addr1->u.var, i->addr2->u.var,i->addr3->u.val);
                else
                    printf("%s := %s < %s\n",i->addr1->u.var, i->addr2->u.var,i->addr3->u.var);
            break;
        case I_LESSEQUAL:
            printf("\t");
            if (i->addr2->kind == NUMBER)
                if (i->addr3->kind == NUMBER)
                    printf("%s := %d <= %d\n",i->addr1->u.var, i->addr2->u.val,i->addr3->u.val);
                else
                    printf("%s := %d <= %s\n",i->addr1->u.var, i->addr2->u.val,i->addr3->u.var);
            else
                if (i->addr3->kind == NUMBER)
                    printf("%s := %s <= %d\n",i->addr1->u.var, i->addr2->u.var,i->addr3->u.val);
                else
                    printf("%s := %s <= %s\n",i->addr1->u.var, i->addr2->u.var,i->addr3->u.var);
            break;
        case I_LABEL:
            printf("LABEL %s: \n",i->addr1->u.var);
            break;
        case I_GOTO:
            printf("\t");
            printf("GOTO %s\n",i->addr1->u.var);
            break;
        case I_IFTHENELSE:
            printf("\t");
            printf("IF %s THEN %s ELSE %s\n",i->addr1->u.var,i->addr2->u.var,i->addr3->u.var);
            break;
        case I_READ:
            printf("\t");
            printf("READ %s\n",i->addr1->u.var);
            break;
        case I_PRINT:
            printf("\t");
            printf("PRINT %s\n",i->addr1->u.var);
            break;
        default:
            printf("Undefined instruction kind %d\n", i->kind);
            exit(-1);
    }
}

//Prints an instruction list to the screen
void printInstrList(InstrList* l) {
    InstrList* aux = l;
    while (aux != NULL) {
        printInstr(aux->data);
        aux = aux->next;
    }
}

//Generates a new temporary from the current value & increments it
char* generateRegister() {
    char* str = (char*) malloc(sizeof(char)*10);
    sprintf(str,"$t%d",COUNTER);
    COUNTER++;
    return str;
}

//Generates a new label from the current value & increments it
char* generateLabel() {
    char* str = (char*) malloc(sizeof(char)*10);
    sprintf(str,"L%d",LABEL_COUNTER);
    LABEL_COUNTER++;
    return str;
}

//Test function for debugging
void test() {
    SymbolList* table = symbolListConstructor(NULL,NULL);  
    Expr* e = ast_operation(MINUS,ast_variable("x"),ast_operation(MULT,ast_number(2),ast_number(5)));
    ExprBool* b = ast_boolOperation(EQUAL, ast_boolValue(true), ast_boolVariable("z"));
    InstrList* aux = InstrListConstructor(InstrConstructor(I_LABEL,AtomVarConstructor(generateLabel()),NULL,NULL),NULL);
    InstrList* l = compileExpr(e);
    appendList(aux,l);
    appendList(aux,InstrListConstructor(InstrConstructor(I_LABEL,AtomVarConstructor(generateLabel()),NULL,NULL),NULL));
    InstrList* bl = compileExprBool(b);
    appendList(aux, InstrListConstructor(InstrConstructor(I_IFTHENELSE,AtomVarConstructor("$t2"),AtomVarConstructor(generateLabel()),AtomVarConstructor(generateLabel())),NULL));
    appendList(aux,bl);
    printInstrList(aux);
}

int main(int argc, char** argv) {
    table = NULL;
  --argc; ++argv;
  if (argc != 0) {
    yyin = fopen(*argv, "r");
    if (!yyin) {
      printf("'%s': could not open file\n", *argv);
      return 1;
    }
  } //  yyin = stdin
  if (yyparse() == 0) {
      InstrList* program = compileCmdList(root);
      printf("Intermediate Three Adress Code:\n\n");
      printInstrList(program);
      printf("\n\n");
      printf("MIPS Assembly Code:\n\n");

  }
  return 0;
}

#endif