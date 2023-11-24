%{
  #include "../bison.tab.h"
  #include <iostream>
  using namespace std;
  extern int yylex();
%}

%option header-file="lexer.hpp"
%option noyywrap
%%


".global" {return GLOBAL;}
".extern" {return EXTERN;}
".section" {return SECTION;}
".word" {return WORD;}
".skip" {return SKIP;}
".end" {return END;}
"halt" {return HALT;}
"int" {return INT;}
"iret" {return IRET;}
"call" {return CALL;}
"ret" { return RET; }
"jmp" { return JMP; }
"beq" { return BEQ; }
"bne" { return BNE; }
"bgt" { return BGT; }
"push" { return PUSH; }
"pop" { return POP; }
"xchg" { return XCHG; }
"add" { return ADD; }
"sub" { return SUB; }
"mul" { return MUL; }
"div" { return DIV; }
"not" { return NOT; }
"and" { return AND; }
"or" { return OR; }
"xor" { return XOR; }
"shl" { return SHL; }
"shr" { return SHR; }
"ld" { return LD; }
"st" { return ST; }
"csrrd" { return CSRRD; }
"csrwr" { return CSRWR; }
%r[0-9] {yylval.ival=atoi(yytext+2);return REG;}
%r1[0-3] {yylval.ival=atoi(yytext+2);return REG;}
%r14 {yylval.ival=atoi(yytext+2);return REG;}
%r15 {yylval.ival=atoi(yytext+2);return REG;}
%sp {yylval.ival=14;return REG;}
%pc {yylval.ival=15;return REG;}
%status { return STATUS; }
%handler  { return HANDLER; }
%cause  { return CAUSE; }
"$" {return DOLLAR;}
"," {return COMMA;}
"\n" {return EOL;}
"#"[^\n\r]* {} //preskoci komentare "\n" {return EOL;}
[ \r\t]* {} //preskoci whitespaces
":" {return COLON;}
"+" {return PLUS;}
"[" {return L_BRACKET;}
"]" {return R_BRACKET;}
";" {return SEMICOLON;}
"%" {return PERCENT;}
"0x"[0-9A-Fa-f]{1,8} {yylval.ival =(int)stol(yytext+2,0,16);return HEXADECIMAL;}
[0-9]+ {yylval.ival=atoi(yytext);return NUMBER;}
[A-Za-z_][A-Za-z0-9_]* {yylval.sval=strdup(yytext);return SYMBOL;}



%%