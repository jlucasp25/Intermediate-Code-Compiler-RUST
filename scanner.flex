%{
// HEADERS
#include <stdlib.h>
#include "parser.h"

// variables maintained by the lexical analyser
int yyline = 1;
%}

%option noyywrap

%%
[ \t]+ {  }
#.*\n { yyline++; }
\n { yyline++; }


\-?[0-9]+ { 
   yylval.intValue = atoi(yytext);
   return SYM_INT; 
}

"+" 		{ return SYM_PLUS; 			}
"-" 		{ return SYM_MINUS;			}
"*" 		{ return SYM_MULT;			}
"/" 		{ return SYM_DIV;  			}
"%" 		{ return SYM_REST; 			}
"==" 		{ return SYM_EQUAL;			}
"!=" 		{ return SYM_DIFFERENT;		}
"<"	 		{ return SYM_LESS;			}
">" 		{ return SYM_GREATER;		}
"<=" 		{ return SYM_EQLESS;		}
">=" 		{ return SYM_EQGREATER;		}
"while" 	{ return SYM_WHILE;			}
"let" 		{ return SYM_LET;			}
"="			{ return SYM_ASSIGNEQUAL;	}
";" 		{ return SYM_SEMICOLON;		}
"{" 		{ return SYM_OPENBRACE;		}
"}" 		{ return SYM_CLOSEBRACE;	}
"("			{ return SYM_OPENPAR;		}
")"			{ return SYM_CLOSEPAR;		}
"if" 		{ return SYM_IF;			}
"then" 		{ return SYM_THEN;			}
"else" 		{ return SYM_ELSE;			}
"println!"	{ return SYM_PRINT;			}
"read_line" { return SYM_READ;			}
"fn main()"	{ return SYM_INITFUNC;		}
"true" {return SYM_BOOL_T; }
"false" {return SYM_BOOL_F; }


[a-zA-Z][a-zA-Z0-9]* {
   yylval.varValue = strdup(yytext);
   return SYM_VAR;
}

.  { yyerror("unexpected character"); }
%%

