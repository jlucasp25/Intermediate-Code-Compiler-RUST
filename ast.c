#ifndef ASTC
#define ASTC

#include <stdlib.h>
#include <string.h>
#include "ast.h"

/////////////////////////////
// Expression AST Builders //
/////////////////////////////

//Builds an arithmetic operation
Expr* ast_operation(OpKind kind,Expr* left, Expr* right) {
  Expr* e = (Expr*) malloc(sizeof(Expr));
  e->kind = E_OPERATION;
  e->attr.op.operator = kind;
  e->attr.op.left = left;
  e->attr.op.right = right;
  return e;
}

//Builds a variable
Expr* ast_variable(char* var) {
  Expr* e = (Expr*) malloc(sizeof(Expr));
  e->kind = E_VARIABLE;
  e->attr.var = strdup(var);
  return e;
}

//Builds an integer
Expr* ast_number(int val) {
  Expr* e = (Expr*) malloc(sizeof(Expr));
  e->kind = E_INTEGER;
  e->attr.value = val;
  return e;
}

/////////////////////////////////////
// Boolean Expression AST Builders //
/////////////////////////////////////

//Builds a boolean operation
ExprBool* ast_boolOperation(RelopKind kind, ExprBool* left, ExprBool* right) {
  ExprBool* eb = (ExprBool*) malloc(sizeof(ExprBool));
  eb->kind = EB_OPERATION;
  eb->attr.op.operator = kind;
  eb->attr.op.left = left;
  eb->attr.op.right = right;
  return eb;
}

//Builds a boolean variable
ExprBool* ast_boolVariable(char* var) {
  ExprBool* eb = (ExprBool*) malloc(sizeof(ExprBool));
  eb->kind = EB_VARIABLE;
  eb->attr.var = strdup(var);
  return eb;
}

//Builds a boolean value
ExprBool* ast_boolValue(bool val) {
  ExprBool* eb = (ExprBool*) malloc(sizeof(ExprBool));
  eb->kind = EB_BOOLEAN;
  eb->attr.value = val;
  return eb;
}

//////////////////////////
// Command AST Builders //
//////////////////////////

//Builds a WHILE command
Cmd* ast_while(ExprBool* condition, CmdList* execute) {
  Cmd* c = (Cmd*) malloc(sizeof(Cmd));
  c->kind = C_WHILE;
  c->op.whil = whileConstructor(condition,execute);
  return c;
}

//Builds a READ command
Cmd* ast_readln(char* string) {
  Cmd* c = (Cmd*) malloc(sizeof(Cmd));
  c->kind = C_READ;
  c->op.printread = ioConstructor(string);
  return c;
}

//Builds a PRINT command
Cmd* ast_println(char* string) {
  Cmd* c = (Cmd*) malloc(sizeof(Cmd));
  c->kind = C_PRINT;
  c->op.printread = ioConstructor(string);
  return c;
}

//Builds an assignment command
Cmd* ast_assign(char* left, Expr* right) {
  Cmd* c = (Cmd*) malloc(sizeof(Cmd));
  c->kind = C_ASSIGN;
  c->op.assign = assignConstructor(left,right);
  return c;
}

//Builds an IF-THEN command
Cmd* ast_if_then(ExprBool* condition, CmdList* execute) {
  Cmd* c = (Cmd*) malloc(sizeof(Cmd));
  c->kind = C_IF_THEN;
  c->op.ifthen = ifThenConstructor(condition,execute);
  return c;
}

//Builds an IF-THEN-ELSE command
Cmd* ast_if_then_else(ExprBool* condition, CmdList* execute1, CmdList* execute2) {
  Cmd* c = (Cmd*) malloc(sizeof(Cmd));
  c->kind = C_IF_THEN_ELSE;
  c->op.ifthenelse = ifThenElseConstructor(condition,execute1,execute2);
  return c;
}

Cmd* ast_declaration(char* var) {
  Cmd* c = (Cmd*) malloc(sizeof(Cmd));
  c->kind = C_DECLARATION;
  c->op.declaration = declarationConstructor(strdup(var));
  return c;
}

CmdList* ast_declaration_assign_expr(char* var, Expr* expr) {
  CmdList* l = ast_cmdList(ast_declaration(var),NULL);
  appendCmdList(l,ast_cmdList(ast_assign(var,expr),NULL));
  return l;
}

////////////////////////////////////////////
// Command List AST Builder and functions //
////////////////////////////////////////////

//Builds a list of commands
CmdList* ast_cmdList(Cmd* data, CmdList* next) {
  CmdList* cl = (CmdList*) malloc(sizeof(CmdList));
  cl->data = data;
  cl->next = next;
  return cl;
}

//Appends two lists of commands
//left is the final one
void appendCmdList(CmdList* left, CmdList* right) {
  CmdList* aux = left;
  while (aux->next != NULL)
    aux = aux->next;
  aux->next = right;
  return;
}

CmdList* appendCmdListBison(CmdList* left, CmdList* right) {
  CmdList* aux = left;
  while (aux->next != NULL)
    aux = aux->next;
  aux->next = right;
  return left;
}

///////////////////////////
// Sub-commands Builders //
///////////////////////////

//Builds an IF-THEN sub-command
IfThen* ifThenConstructor(ExprBool* condition, CmdList* execute) {
  IfThen* it = (IfThen*) malloc(sizeof(IfThen));
  it->condition = condition;
  it->execute = execute;
  return it;
}

//Builds an IF-THEN-ELSE sub-command
IfThenElse* ifThenElseConstructor(ExprBool* condition, CmdList* execute1, CmdList* execute2) {
  IfThenElse* ite = (IfThenElse*) malloc(sizeof(IfThenElse));
  ite->condition = condition;
  ite->execute1 = execute1;
  ite->execute2 = execute2;
  return ite;
}

//Builds an ASSIGN sub-command
Assign* assignConstructor(char* leftSide, Expr* rightSide) {
  Assign* as = (Assign*) malloc(sizeof(Assign));
  as->leftSide = strdup(leftSide);
  as->rightSide = rightSide;
  return as;
}

//Builds an Input/Output operation sub-command
IO* ioConstructor(char* string) {
  IO* io = (IO*) malloc(sizeof(IO));
  io->string = strdup(string);
  return io;
}

//Builds a WHILE sub-command
While* whileConstructor(ExprBool* condition, CmdList* execute) {
  While* wh = (While*) malloc(sizeof(While));
  wh->condition = condition;
  wh->execute = execute;
  return wh;
}

//Builds a DECLARATION sub-command
Declaration* declarationConstructor(char* name) {
  Declaration* dl = (Declaration*) malloc(sizeof(Declaration));
  dl->name = strdup(name);
  return dl;
}

#endif