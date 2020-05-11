#ifndef ASTH
#define ASTH

//  AST.h
// Contains all the AST structures, builders and functions
// Francisco Gonçalves, upXXXXXXXXXX
// João Pires, up201606617

#include <stdbool.h>

/////////////////////
// Data Structures //
/////////////////////

//Type of arithmetic operation
typedef enum OpKind {PLUS,MINUS,DIV,MULT} OpKind;

//Type of logic operation
typedef enum RelopKind {EQUAL, DIF, LT, GT, LE ,GE } RelopKind;

//Expression
typedef struct _Expr {
  
  enum { 
    E_INTEGER,
    E_OPERATION,
    E_VARIABLE,
  } kind;
  
  union {
    int value; // for integer values
    char* var; //for variable expression
    struct { 
      enum OpKind operator; // PLUS, MINUS
      struct _Expr* left;
      struct _Expr* right;
    } op; // for binary expressions

  } attr;

} Expr;

//Boolean Expression
typedef struct _ExprBool {

  enum {
    EB_BOOLEAN,
    EB_OPERATION,
    EB_VARIABLE
  } kind;

  union {
    bool value; //boolean values
    char* var; //variable values
    struct {
      enum RelopKind operator; //EQUAL, DIF, LT,GT...
      struct _ExprBool* left;
      struct _ExprBool* right;
    } op; //binary expressions
  } attr;
  
} ExprBool;

//Sub-commands

//If then
typedef struct _IfThen {
  ExprBool* condition;
  struct _CmdList* execute;
}  IfThen;

//If then else
typedef struct _IfThenElse {
  ExprBool* condition;
  struct _CmdList* execute1;
  struct _CmdList* execute2;
}  IfThenElse;

//Assign value to variable
typedef struct _Assign {
  char* leftSide; //variable
  Expr* rightSide;
} Assign;

//IO operations
typedef struct _IO {
  char* string;
} IO;

//while loops
typedef struct _While {
  ExprBool* condition;
  struct _CmdList* execute;
} While;

//Declare variable
typedef struct _Declaration {
  char* name;
} Declaration;

//List of commands
typedef struct _CmdList {
  struct _Cmd* data;
  struct _CmdList* next;
} CmdList;

//Command
typedef struct _Cmd { 

  enum {
    C_IF_THEN,
    C_IF_THEN_ELSE,
    C_ASSIGN,
    C_WHILE,
    C_PRINT,
    C_READ,
    C_DECLARATION
  } kind;  

  union {
    IO* printread;
    IfThen* ifthen;
    IfThenElse* ifthenelse;
    Assign* assign;
    While* whil;
    Declaration* declaration;
  } op; 

} Cmd;

///////////////////////
// Function Headers  //
///////////////////////

//Expression AST builders
Expr* ast_operation(OpKind kind,Expr* left, Expr* right);
Expr* ast_variable(char* var);
Expr* ast_number(int val);

//Boolean Expression AST builders
ExprBool* ast_boolOperation(RelopKind kind, ExprBool* left, ExprBool* right);
ExprBool* ast_boolVariable(char* var);
ExprBool* ast_boolValue(bool val);


//Command AST builders
Cmd* ast_while(ExprBool* condition, CmdList* execute);
Cmd* ast_readln(char* string);
Cmd* ast_println(char* string);
Cmd* ast_assign(char* left, Expr* right);
Cmd* ast_if_then(ExprBool* condition, CmdList* execute);
Cmd* ast_if_then_else(ExprBool* condition, CmdList* execute1, CmdList* execute2);
Cmd* ast_declaration(char* var);
CmdList* ast_declaration_assign_expr(char* var, Expr* expr);

//Command List functions
CmdList* ast_cmdList(Cmd* data, CmdList* next);
void appendCmdList(CmdList* left, CmdList* right);
CmdList* appendCmdListBison(CmdList* left, CmdList* right);

//Command Subtypes builders
Assign* assignConstructor(char* leftSide, Expr* rightSide);
IO* ioConstructor(char* string);
IfThen* ifThenConstructor(ExprBool* condition, CmdList* execute);
IfThenElse* ifThenElseConstructor(ExprBool* condition, CmdList* execute1, CmdList* execute2);
While* whileConstructor(ExprBool* condition, CmdList* execute);
Declaration* declarationConstructor(char* name);



#endif