// Tokens
%token 
  SYM_INT  
  SYM_BOOL_T
  SYM_BOOL_F
  SYM_PLUS
  SYM_MINUS
  SYM_MULT
  SYM_DIV
  SYM_REST
  SYM_EQUAL
  SYM_DIFFERENT
  SYM_GREATER
  SYM_LESS
  SYM_EQGREATER
  SYM_EQLESS
  SYM_IF
  SYM_THEN
  SYM_ELSE
  SYM_LET
  SYM_ASSIGNEQUAL
  SYM_SEMICOLON
  SYM_OPENBRACE
  SYM_CLOSEBRACE
  SYM_OPENPAR
  SYM_CLOSEPAR
  SYM_WHILE
  SYM_VAR
  SYM_PRINT
  SYM_READ
  SYM_INITFUNC

// Operator associativity & precedence
%left SYM_ASSIGN
%left SYM_PLUS SYM_MINUS
%left SYM_MULT SYM_DIV SYM_REST
%left SYM_GREATER SYM_LESS SYM_EQGREATER SYM_EQLESS
%left SYM_EQUAL SYM_DIFFERENT

// Root-level grammar symbol
%start program;

// Types/values in association to grammar symbols.
%union {
  int intValue;
  bool boolValue;
  char* varValue;
  Expr* exprValue;
  ExprBool* exprBoolValue;
  Cmd* cmdValue;
  CmdList* cmdListValue;
}

%type <intValue> SYM_INT
%type <boolValue> SYM_BOOL_T
%type <boolValue> SYM_BOOL_F
%type <exprValue> expr
%type <exprBoolValue> boolexpr
%type <cmdValue> cmd
%type <cmdListValue> list
%type <varValue> SYM_VAR


// Use "%code requires" to make declarations go
// into both parser.c and parser.h
%code requires {
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "ast.h"

extern int yylex();
extern int yyline;
extern char* yytext;
extern FILE* yyin;
extern void yyerror(const char* msg);
CmdList* root;
}

%%
program: SYM_INITFUNC SYM_OPENBRACE list SYM_CLOSEBRACE {
  root = $3;
};

list:
  cmd list {
    $$ = ast_cmdList($1,$2);
  }
  |
  cmd {
    $$ = ast_cmdList($1,NULL);
  }
  |
  SYM_LET SYM_VAR SYM_ASSIGNEQUAL expr SYM_SEMICOLON list {
    $$ = appendCmdListBison(ast_declaration_assign_expr($2,$4),$6);
  }
  ;
cmd:
  SYM_IF boolexpr SYM_OPENBRACE list SYM_CLOSEBRACE {
    $$ = ast_if_then($2,$4);
  }
  |
  SYM_IF boolexpr SYM_OPENBRACE list SYM_CLOSEBRACE SYM_ELSE SYM_OPENBRACE list SYM_CLOSEBRACE {
    $$ = ast_if_then_else($2, $4, $8);
  }
  |
  SYM_WHILE boolexpr SYM_OPENBRACE list SYM_CLOSEBRACE {
    $$ = ast_while($2, $4);
  }
  |
  SYM_PRINT SYM_OPENPAR SYM_VAR SYM_CLOSEPAR SYM_SEMICOLON {
    $$ = ast_println($3);
  }
  |
  SYM_READ SYM_OPENPAR SYM_VAR SYM_CLOSEPAR SYM_SEMICOLON {
    $$ = ast_readln($3);
  }
  |
  SYM_VAR SYM_ASSIGNEQUAL expr SYM_SEMICOLON {
    $$ = ast_assign($1,$3);
  }
  |
  SYM_LET SYM_VAR SYM_SEMICOLON {
    $$ = ast_declaration($2);
  }
  ;
expr: 
  SYM_INT { 
    $$ = ast_number($1); 
  }
  | 
  expr SYM_PLUS expr { 
    $$ = ast_operation(PLUS, $1, $3); 
  }
  | 
  expr SYM_MINUS expr { 
    $$ = ast_operation(MINUS, $1, $3); 
  }
  | 
  expr SYM_MULT expr { 
    $$ = ast_operation(MULT, $1, $3); 
  }
  | 
  expr SYM_DIV expr { 
    $$ = ast_operation(DIV, $1, $3); 
  }
  |
  SYM_VAR {
    $$ = ast_variable($1);
  }
  ;
boolexpr:
  SYM_BOOL_T { 
    $$ = ast_boolValue(true); 
  }
  | 
  SYM_BOOL_F { 
    $$ = ast_boolValue(false); 
  }
  |
  boolexpr SYM_EQUAL boolexpr { 
    $$ = ast_boolOperation(EQUAL, $1, $3); 
  }
  | 
  boolexpr SYM_DIFFERENT boolexpr { 
    $$ = ast_boolOperation(DIF, $1, $3); 
  }
  | 
  boolexpr SYM_GREATER boolexpr { 
    $$ = ast_boolOperation(GT, $1, $3); 
  }
  | 
  boolexpr SYM_LESS boolexpr { 
    $$ = ast_boolOperation(LT, $1, $3); 
  }
  | 
  boolexpr SYM_EQGREATER boolexpr { 
    $$ = ast_boolOperation(GE, $1, $3); 
  }
  | 
  boolexpr SYM_EQLESS boolexpr { 
    $$ = ast_boolOperation(LE, $1, $3); 
  }
  |
  SYM_VAR {
    $$ = ast_boolVariable($1);
  }
  ;
%%

void yyerror(const char* err) {
  printf("Line %d: %s - '%s'\n", yyline, err, yytext  );
}

