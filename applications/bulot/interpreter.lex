%{
#include <stdlib.h>
#include "arbre_yacc.h"
#include "y.tab.h"
#include "customs.h"

	int yyerror();
	char tmpstr[1024];
	char tmpchar[10];

%}

%x ALIAS
%x UNALIAS
%x AFTEREQ
%x AFTERALIAS
%x CALCUL
%x ENDWILDCHAR

%%
%error-verbose;

<<EOF>>					{ yyterminate(); }

 /*shebang*/
"#!/bin/bulot -x"		{ ; }

 /*début de bloc et semicol*/
"{"						return ACCO;
"}"						return ACCF;
";"						return SEMI;
","						return COLL;

 /*structures de controle*/
"if"					return IFLO;
"else"					return ELSE;
"end if"				return ENIF;
"while"					return WHIL;
"end loop"				return ENLO;
"for"					return FORL;

 /*opérateurs*/
<INITIAL>"="			return *yytext;
<CALCUL,INITIAL>[<>]	return *yytext;
<CALCUL,INITIAL>"=="	return EQLO;
<CALCUL,INITIAL>"!="	return NOEQ;
<CALCUL,INITIAL>"("		return BEPA;
<CALCUL,INITIAL>")"		return ENPA; 
<CALCUL,INITIAL>"&&"	return ANLO;
<CALCUL,INITIAL>"||"	return ORLO;
<CALCUL,INITIAL>"~"		return INVL;
<CALCUL,INITIAL>">>"	return REDL;

<INITIAL>"= "			{ BEGIN CALCUL; return yytext[0]; }

 /*builtin*/
"cd"(" ")*				return CDEB;
"kill"					return KIBU;
"exit"					return EXBU;
"q"						return EXBU;
"hist"					return HIBU;
"bulot"					return BUBU;
"kikoo"					return KIKU;
"sl"					return SLBU;
"vbpt"					return VBBU;
"fg"					return FGBU;
"bg"					return BGBU;
"jobs"					return JOBU;
"arbre"					return ABBU;
"read"                                  scanf("%s",tmpchar);
"alias"					{ BEGIN ALIAS; return ALBU; }

"unalias"								{ BEGIN UNALIAS; return UNAL; }

<UNALIAS>" "[^=;\n ]+					{ yylval.str = strdup(yytext + 1); BEGIN 0; return TORE; }

<ALIAS>" "[^=\n]+"="					{ BEGIN AFTEREQ; yylval.str = strdup(yytext + 1); return TORE; }

<AFTEREQ>[^;]*							{ BEGIN AFTERALIAS; yylval.str = strdup(yytext); return ALVA; }

<ALIAS,AFTERALIAS>";"					{ BEGIN 0; return SEMI; }

<ALIAS>[^=\n;]*							{ BEGIN 0;}

"$(("									{ BEGIN CALCUL; }

<CALCUL>"$(("							{ ; }

<CALCUL>"))"							{ BEGIN 0;}

<CALCUL>";"								{ BEGIN 0; return SEMI; }

<CALCUL>[-+*/&|]						{ return *yytext; }

<CALCUL>[1-9][0-9]*						{ yylval.num = atoi(yytext); return NUML; }

<CALCUL>[^-+*/&|0-9\$a-zA-Z ]			{ BEGIN 0; }

<CALCUL>0								{ yylval.num = atoi(yytext); return NUML; }

[&|]									{ return *yytext; }

\%[0-9]*								{ yylval.num = atoi(yytext+1); return INDL; }

<INITIAL,CALCUL>\$[a-z][A-Za-z0-9]*		{ yylval.vindex = strdup(yytext+1); return VARL; }

("-")*([0-9A-Za-z\-\/\.\_\~\#\@\+\/\`]*("\\"[^\n])*[0-9A-Za-z\-\/\.\_\~\#\@\+\/\`]*)*	{ yylval.str = strdup(yytext); return COML; }

\"[^"\n]*["\n]							{
											yylval.str = strdup(yytext+1);
											if (yylval.str[yyleng-2] != '"') {
												yyerror("improperly terminated string");
											} else {
												yylval.str[yyleng-2] = '\0';
											}
											/*retourner la chaine yylval*/
											return COML;
										} 

"*"[a-zA-Z0-9\.]*							{ yylval.str = strdup(yytext); return WILDCHAR; }

[a-zA-Z0-9]+[a-zA-Z0-9]*"*"				{ 
											yytext[strlen(yytext)-1] = '\0'; 
											tmpstr[0] = '^';
											strncpy(&tmpstr[1], yytext, strlen(yytext));
											yylval.str = strndup(tmpstr, strlen(yytext) + 1);
											return WILDCHAR; 
										}

"["([\^a-zA-Z0-9\.\!]*","?)*				{ BEGIN ENDWILDCHAR; yylval.str = strdup(yytext + 1); return WILDCHAR; }

<ENDWILDCHAR>"]"							{ BEGIN 0; }


[ \t\n]+									{ ; }	/*ignore les espaces, tabulations et espaces consécutifs*/

"/*""/"*([^*/]|[^*]"/"|"*"[^/])*"*"*"*/"	{ ; }	/*ignore les commentaires*/


.											{ printf("Caractère inconnu\n"); }

%%

int yywrap(void){
  /*suppression des warnings*/
  int t = (int)input;
  int v = (int)yyunput;
  v += t;
  YY_FLUSH_BUFFER;
  return 1;
}
