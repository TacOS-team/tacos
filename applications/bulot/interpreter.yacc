%token ACCO
%token ACCF
%token SEMI
%token COLL

%token IFLO
%token THEN
%token ENIF
%token WHIL
%token FORL
%token ENLO
%token BEPA
%token ENPA

%token CDBU
%token CDEB
%token KIBU
%token EXBU
%token HIBU
%token BUBU
%token KIKU
%token SLBU
%token VBBU
%token FGBU
%token BGBU
%token JOBU
%token ABBU
%token ALBU
%token UNAL
%token ALEQ
%token NBUL
%token NLBU


%token FUNCTION_NAME

%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include "const.h"
#include "arbre_yacc.h"

#include "table_symboles.h"
#include "historic.h"
#include "jobs.h"
#include "shell.h"
#include "init.h"
#include "alias.h"
#include "command.h"
#include "customs.h"
#include "builtcommand.h"


        int insert_alias(char **tab, char *command, int *i, int n);

	char *comm_tab[LENGTH_LINE];
	int index_tab = 0;
	int index_pipe = 0;
	int count_coml = 0;
	
	int yywrap(void);
	void yyerror(const char*);
	int yylex(void);
%}


%left '<' '>' '|' REDA EQLO NOEQ '=' ANLO ORLO INVL REDL
%left '+' '-' 
%left '*' '/'
%nonassoc IFX 
%nonassoc ELSE


%union {
	int num;
	char *vindex;
	noeud_s *node;
	char *str;
};

%token <vindex> VARL
%token <num> NUML INDL
%token <str> STRL COML ALVA TORE WILDCHAR
%type <node> program command instruction bloc linescript affect expr controle cond builtin

%error-verbose;

%%

program:
			instruction					{ inserer_arbre($1); parse_tree($1); supprimer_arbre($1); }
		|	error						{ printf("erreur de syntaxe\n"); }
		;	

bloc: 
			ACCO instruction ACCF				{ $$ = $2; }
		;

instruction:
			instruction linescript				{ $$ = create_op_node(";", $1, NULL, $2); }
		|										{ $$ = NULL; }
		;

linescript: 
			affect SEMI							{ $$ = $1; }
		|	command SEMI						{ $$ = $1; }
		|	controle							{ $$ = $1; }
		|	builtin SEMI						{ $$ = $1; }
		|	FUNCTION_NAME BEPA VARL ENPA SEMI	{ $$ = create_op_node("print", NULL, NULL, create_var_node($3)); }
		;	

affect:
			VARL '=' expr						{ $$ = create_op_node("=", create_var_node($1), NULL, $3); }
		;

controle:
			IFLO cond bloc ELSE bloc			{ $$ = create_op_node("if", $2, $3, $5); }
		|	IFLO cond bloc 						{ $$ = create_op_node("if", $2, $3, NULL); }
		|	WHIL cond bloc						{ $$ = create_op_node("while", $2, $3, NULL); }
		;

expr: 
			VARL								{ $$ = create_var_node($1); }
		|	NUML								{ $$ = create_const_node($1); }
		|	COML								{ $$ = create_var_node($1); }
		|	expr '+' expr						{ $$ = create_op_node("+", $1, NULL, $3); }
		|	expr '-' expr						{ $$ = create_op_node("-", $1, NULL, $3); }
		|	expr '*' expr						{ $$ = create_op_node("*", $1, NULL, $3); }
		|	expr '/' expr						{ $$ = create_op_node("/", $1, NULL, $3); }
		|	BEPA expr ENPA						{ $$ = $2; }
		;

cond:
			BEPA cond ENPA						{ $$ = $2; }
		|	expr '<' expr						{ $$ = create_op_node("<", $1, NULL, $3); }
		|	expr '>' expr						{ $$ = create_op_node(">", $1, NULL, $3); }
		|	expr EQLO expr						{ $$ = create_op_node("==", $1, NULL, $3); }
		|	expr NOEQ expr						{ $$ = create_op_node("!=", $1, NULL, $3); }
		|	expr ANLO expr						{ $$ = create_op_node("&&", $1, NULL, $3); }
		|	expr ORLO expr						{ $$ = create_op_node("||", $1, NULL, $3); }
		|	cond EQLO cond						{ $$ = create_op_node("==", $1, NULL, $3); }
		|	cond NOEQ cond						{ $$ = create_op_node("!=", $1, NULL, $3); }
		|	cond ANLO cond						{ $$ = create_op_node("&&", $1, NULL, $3); }
		|	cond ORLO cond						{ $$ = create_op_node("||", $1, NULL, $3); }
		;

command:
			pipe redir background                   { $$ = create_comm_node(copy_pipe(pipe_tab), index_pipe, print_pipe(pipe_tab), infile, outfile, context,dir);
			                                          index_pipe = 0;
			                                        }


pipe:
			pipe '|' line                           { pipe_tab[index_pipe] = copy_tab(comm_tab); 
				                                  index_pipe++; 
				                                  pipe_tab[index_pipe] = NULL; 
			                                        }
		|	line                                    { pipe_tab[index_pipe] = copy_tab(comm_tab); 
			                                          index_pipe++;
			                                          pipe_tab[index_pipe] = NULL; 
			                                        }
		;

redir:
			'<' COML                                { infile = open($2, O_RDONLY);
			                                          outfile = 0;
			                                        }
		|	'>' COML                                { infile = 0;
				                                  outfile = open($2,O_RDWR | O_CREAT | O_TRUNC);
			                                        }
		|	REDL COML                               { infile = 0;
				                                  outfile = open($2, O_RDWR | O_CREAT | O_APPEND);
			                                        }
		|	                                        { infile = 0;
				                                  outfile = 0;
			                                        }
		;

background:
			'&'	                                { context = 1; }
		|		                                { context = 0; }
		;
 
line:
			line COML                               { count_coml++;
				                                  if (count_coml == 1){
				                                  	insert_alias(comm_tab, $2, &index_tab, 90);
				                                  	} else {
				                                  	comm_tab[index_tab] = $2; 
				                                  	index_tab++; 
				                                   }
				                                   comm_tab[index_tab] = NULL;
				                                   builtin = 0;
			                                         }
		|	line VARL                                { tmp = malloc(sizeof(char)*strlen($2)+2);
				                                   tmp[0] = '$';
				                                   strncpy(tmp+1, $2, strlen($2) + 1);
				                                   comm_tab[index_tab] = tmp;
				                                   index_tab++;
				                                   comm_tab[index_tab] = NULL;
				                                   builtin = 0;
			                                         }
	
		|	line COML WILDCHAR                       { comm_tab[index_tab] = $2;
				                                   index_tab++;
				                                   tmp = malloc(sizeof(char)*strlen($3)+2);
				                                   tmp[0] = '#';
				                                   strncpy(tmp+1, $3, strlen($3) + 1);
				                                   comm_tab[index_tab] = tmp;
				                                   index_tab++;
				                                   if (count_coml == 0) {	/*ce qui précède le wildchar est une commande*/
				                                   	dir = NULL;
				                                   } else {			/*sinon, il faut chercher dans le répertoire qui précède le wildchar*/
				                                   	dir = $2;
				                                   }
				                                   comm_tab[index_tab] = NULL;
				                                   builtin = 0;
			                                         }
                |		                                 { index_tab = 0;
			                                           count_coml = 0; 
                                                                 }
                ;

builtin:
			CDEB COML							{ $$ = create_bu_node("bu", cd1, 0, $2, 0); }
		|	CDEB INVL							{ $$ = create_bu_node("bu", cd2, 0, NULL, 0); } 
		|	EXBU								{ $$ = create_bu_node("bu", ex, 0, NULL, 0); } 
		|	HIBU								{ $$ = create_bu_node("bu", hi, 0, NULL, 0); } 
		|	KIKU COML							{ $$ = create_bu_node("bu", ki, 0, $2, 0); } 
		|	SLBU								{ $$ = create_bu_node("bu", sl1, 0, NULL, 0); }
		|	VBBU								{ $$ = create_bu_node("bu", vb, 0, NULL, 0); }
		|	FGBU INDL							{ $$ = create_bu_node("bu", fg1, $2, NULL, 0); } 
		|	FGBU								{ $$ = create_bu_node("bu", fg2, 0, NULL, 0); } 
		|	BGBU								{ $$ = create_bu_node("bu", bg1, 0, NULL, 0); } 
		|	BGBU INDL							{ $$ = create_bu_node("bu", bg2, $2, NULL, 0); } 
		|	JOBU								{ $$ = create_bu_node("bu", jo, 0, NULL, 0); } 
		|	ALBU TORE ALVA						{ $$ = create_bu_node("bu", al1, 0, $2, $3); } 
		|	ALBU								{ $$ = create_bu_node("bu", al2, 0, NULL, 0); } 
		|	UNAL TORE							{ $$ = create_bu_node("bu", un, 0, $2, 0); } 
		|	ABBU								{ ; }
		;
%%

void yyerror(const char *s) {
	index_pipe = 0;
	index_tab = 0;
	pipe_tab[0] = NULL;
	printf("Erreur de syntaxe %s\n",s);
	yywrap();
}
