%{  
  #include "../inc/asembler.hpp"
  #include <iostream>
  using namespace std; 
  extern int yylex();
  extern int yyparse();
  extern FILE *yyin;
  extern Asembler *a1;
  void yyerror(const char *s);
%}


%union {
  int ival;
  float fval;
  char *sval;
}


%token EOL
%token GLOBAL
%token EXTERN
%token SECTION
%token SKIP
%token WORD
%token END
%token HALT
%token INT
%token IRET
%token CALL
%token RET
%token JMP
%token BEQ
%token BNE
%token BGT
%token PUSH
%token POP
%token XCHG
%token ADD
%token SUB
%token MUL
%token DIV
%token NOT
%token AND
%token OR
%token XOR
%token SHL
%token SHR
%token LD
%token ST
%token CSRRD
%token CSRWR
%token COMMA
%token COLON
%token SEMICOLON
%token PERCENT
%token DOLLAR
%token SP
%token PC
%token STATUS
%token HANDLER
%token CAUSE
%token PLUS
%token L_BRACKET
%token R_BRACKET


%token<ival> REG
%token<sval> SYMBOL
%token<ival> HEXADECIMAL
%token<ival> NUMBER
%token<sval> LABEL

%%

input:
| EOL input
| line EOL input
| label line EOL input
| label EOL input


;

line:
  glob
| ext
| SECTION SYMBOL {a1->ssection($2);free($2);}
| word
| skip
| END {YYACCEPT;}
| HALT {a1->hhalt();}
| INT{a1->iint();}
| IRET{a1->iiret();}
| CALL SYMBOL{a1->ccall($2);free($2);}
| CALL NUMBER{a1->ccall($2);}
| CALL HEXADECIMAL{a1->ccall($2);}
| RET {a1->rret();}
| JMP SYMBOL{a1->jjmp($2);free($2);}
| JMP NUMBER{a1->jjmp($2);}
| JMP HEXADECIMAL{a1->jjmp($2);}
| BEQ REG COMMA REG COMMA SYMBOL{a1->bbeq($2,$4,$6);free($6);}
| BEQ REG COMMA REG COMMA NUMBER{a1->bbeq($2,$4,$6);}
| BEQ REG COMMA REG COMMA HEXADECIMAL{a1->bbeq($2,$4,$6);}
| BNE REG COMMA REG COMMA SYMBOL{a1->bbne($2,$4,$6);free($6);}
| BNE REG COMMA REG COMMA NUMBER{a1->bbne($2,$4,$6);}
| BNE REG COMMA REG COMMA HEXADECIMAL{a1->bbne($2,$4,$6);}
| BGT REG COMMA REG COMMA SYMBOL{a1->bbgt($2,$4,$6);free($6);}
| BGT REG COMMA REG COMMA NUMBER{a1->bbgt($2,$4,$6);}
| BGT REG COMMA REG COMMA HEXADECIMAL{a1->bbgt($2,$4,$6);}
| PUSH REG{a1->ppush($2);}
| POP REG{a1->ppop($2);}
| XCHG REG COMMA REG{a1->xxchg($2,$4);}
| ADD REG COMMA REG{a1->aadd($2,$4);}
| SUB REG COMMA REG{a1->ssub($2,$4);}
| MUL REG COMMA REG{a1->mmul($2,$4);}
| DIV REG COMMA REG{a1->ddiv($2,$4);}
| NOT REG{a1->nnot($2);}
| AND REG COMMA REG{a1->aand($2,$4);}
| OR REG COMMA REG{a1->oor($2,$4);}
| XOR REG COMMA REG{a1->xxor($2,$4);}
| SHL REG COMMA REG{a1->sshl($2,$4);}
| SHR REG COMMA REG{a1->sshr($2,$4);}
| LD DOLLAR SYMBOL COMMA REG{a1->lldNeposredno($3,$5);free($3);}
| LD DOLLAR NUMBER COMMA REG{a1->lldNeposredno($3,$5);}
| LD DOLLAR HEXADECIMAL COMMA REG{a1->lldNeposredno($3,$5);}
| LD SYMBOL COMMA REG{a1->lldMemdir($2,$4);free($2);}
| LD NUMBER COMMA REG{a1->lldMemdir($2,$4);}
| LD HEXADECIMAL COMMA REG{a1->lldMemdir($2,$4);}
| LD REG COMMA REG{a1->lldRegdir($2,$4);}
| LD L_BRACKET REG R_BRACKET COMMA REG{a1->lldRegind($3,$6);}
| LD L_BRACKET REG PLUS NUMBER R_BRACKET COMMA REG{a1->lldRegindpom($3,$5,$8);}
| LD L_BRACKET REG PLUS HEXADECIMAL R_BRACKET COMMA REG{a1->lldRegindpom($3,$5,$8);}
| LD L_BRACKET REG PLUS SYMBOL R_BRACKET COMMA REG{a1->lldRegindpom($3,$5,$8);free($5);}
| ST REG COMMA DOLLAR SYMBOL{a1->sstNeposredno($2,$5);free($5);}
| ST REG COMMA DOLLAR NUMBER{a1->sstNeposredno($2,$5);}
| ST REG COMMA DOLLAR HEXADECIMAL{a1->sstNeposredno($2,$5);}
| ST REG COMMA SYMBOL{a1->sstMemdir($2,$4);free($4);}
| ST REG COMMA NUMBER{a1->sstMemdir($2,$4);}
| ST REG COMMA HEXADECIMAL{a1->sstMemdir($2,$4);}
| ST REG COMMA REG{a1->sstRegdir($2,$4);}
| ST REG COMMA L_BRACKET REG R_BRACKET{a1->sstRegind($2,$5);}
| ST REG COMMA L_BRACKET REG PLUS NUMBER R_BRACKET{a1->sstRegindpom($2,$5,$7);}
| ST REG COMMA L_BRACKET REG PLUS HEXADECIMAL R_BRACKET{a1->sstRegindpom($2,$5,$7);}
| ST REG COMMA L_BRACKET REG PLUS SYMBOL R_BRACKET{a1->sstRegindpom($2,$5,$7);free($7);}
| CSRRD STATUS COMMA REG{a1->ccsrrdStatus($4);}
| CSRRD HANDLER COMMA REG{a1->ccsrrdHandler($4);}
| CSRRD CAUSE COMMA REG{a1->ccsrrdCause($4);}
| CSRWR REG COMMA STATUS{a1->ccsrwrStatus($2);}
| CSRWR REG COMMA HANDLER{a1->ccsrwrHandler($2);}
| CSRWR REG COMMA CAUSE{a1->ccsrwrCause($2);}
;


label:
  SYMBOL COLON{a1->llabel($1);free($1);}
;

glob:
  GLOBAL SYMBOL {a1->gglobal($2);free($2);}
| glob COMMA SYMBOL {a1->gglobal($3);free($3);}
;

ext:
  EXTERN SYMBOL {a1->eextern($2);free($2);}
| ext COMMA SYMBOL{a1->eextern($3);free($3);}
;

skip:
  SKIP HEXADECIMAL{a1->sskip($2);}
|  SKIP NUMBER{a1->sskip($2);}
;

word:
  WORD SYMBOL{a1->wword($2);free($2);}
| WORD NUMBER{a1->wword($2);}
| WORD HEXADECIMAL{a1->wword($2);}
| word COMMA SYMBOL{a1->wword($3);free($3);}
| word COMMA NUMBER{a1->wword($3);}
| word COMMA HEXADECIMAL{a1->wword($3);}
;


%%

void yyerror(const char *s) {
  cout << "Greska u parsiranju!!!: " << s << endl;
  // might as well halt now:
  exit(-1);
}