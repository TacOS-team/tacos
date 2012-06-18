#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include "customs.h"

/*
 *insère une chaine provenant de la traduction d'un wildchar dans la
 *table passée en argument (typiquement, la table des commandes) en
 *incrémentant son indice et en veillant à ce qu'il ne dépasse pas n
 */
int insert(char **tab, char *file, char *dir, int *i, int n){

     if (strlen(file) <= 2){
	  if (strncmp(file, "..", strlen(file)) == 0){
	       return 0; /*on évite d'inséré les . et ..*/
	  }
     }
     char **ptab = &tab[*i];
     char *chem = cmalloc((strlen(file) + strlen(dir) + 2) * sizeof(char));
     strncpy(chem, dir, strlen(dir) + 1);
     if (*i < n) {
	  strncat(chem, file, strlen(file) + 1);
	  *ptab = chem;
	  *i = *i + 1;
     }
     
     return 0;
     
}

/*traduit un wildchar*/
int traduce(char *wildchar, char *dir, char **tab, int *i, int n){
	struct dirent *curdir;
	char *substr;
	int begin = 0;
	int not = 0;
	DIR *rep;
	char *token = cmalloc((strlen(wildchar) + 1) * sizeof(char));
	char *s = cmalloc((strlen(wildchar ) + 1) * sizeof(char));
	int l = strlen(wildchar);
	char *memtoken = token;
	char *mems = s;
	char *sauv = malloc(sizeof(char) * (l + 1));
	strncpy(sauv, wildchar, l + 1);
	
	rep = opendir(dir);
	if (rep == NULL) {
		perror("Ne peut ouvrir le répertoire\n");
	} else {
		//printf("wildchar = %s\n", wildchar);
		switch (*wildchar) {
		case '*':/*deux cas sont traité: l'étoile seule et l'étoile suivie d'un texte*/
			if (l > 1)
			     sauv++; /*on saute le char * */
			while ((curdir = readdir(rep))) {
				if (l > 1) {
				     /*l'étoile est suivie d'un texte, il faut chercher les correspondances*/
				     substr = strstr(curdir->d_name, sauv);
				     while (substr != NULL){
					  /*il faut que le texte derière l'étoile se trouve à la fin du nom de fichier*/
					  if ( (substr + strlen(sauv)) == (curdir->d_name + strlen(curdir->d_name))){
					       insert(tab, curdir->d_name, dir, i, n);
					  }
					  substr = strstr(++substr, sauv);
				     } 
				     
				} else { /*Si l'étoile est seule on insère tout*/
				     insert(tab, curdir->d_name, dir, i, n);
				}
			}
			break;
		case '?':
			break;
		case '!':
			if (l > 1)
				wildchar++;
			not = 1;
		default:/*pour les expressions de type [aaa,b,ccc]*/
		     /*on détecte si on a la containte d'être au début*/
		     if (*wildchar == '^'){
			  wildchar++;
			  begin = 1;
		     }
		     strcpy(s, wildchar);
		     token = strtok(s, ",");
		     while(token != NULL) {
			  l = strlen(token);
			  while ((curdir = readdir(rep))) {
			       /*on cherche si le token correspond à une sous chaine du nom de fichier*/
			       substr = strstr(curdir->d_name, token);
			       if ( substr != NULL) {
				    if (not == 0){
					 /*si la contrainte begin était présente, il faut en plus que le token corresponde au début du fichier*/
					 if (begin == 1){
					      (substr == curdir->d_name) ? insert(tab, curdir->d_name, dir, i, n) : 0;
					 } else {
					      insert(tab, curdir->d_name, dir, i, n);
					 }
				    } else { /*not == 1 on cherche donc tous les noms de fichier ne commençant pas par l'expression*/
					 if (begin == 1)
					      (substr == curdir->d_name) ? 0 : insert(tab, curdir->d_name, dir, i, n);
				    }
			       } else {
				    /*cas ou pas de sous chaine n'est trouvée, il faut insérer que si on avait une négation*/
				    if (not == 1)
					 insert(tab, curdir->d_name, dir, i, n);
			       }
			  }
			  token = strtok(NULL, ",");
			  closedir(rep);
			  rep = opendir(dir);
		     }
		     break;
		}
		closedir(rep);
	}
	cfree(memtoken);
	cfree(mems);
	return 0;
}
