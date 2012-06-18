#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "autocomp.h"


/*
 *retourne le dernier mot d'une chaine (le délimiteur étant delin) et met a jour afc qui vaut 1 si on le dernier mot de la chaine se trouve derière une commande
 *le mot retournée est copié dans le tas
*/
char *lastword(char *s, char *end, int *afc, char delim)
{
	char *pc = end;
	size_t n = 0;
	char *w = NULL;

	if (s != NULL) {
		if(*pc == delim){
			pc--;
		}
		while(*pc != delim && pc >= s) {
			n++;
			pc--;
		}
		if (*pc == delim) {
			*afc = 1;
		} else {
			*afc = 0;
		}
		pc++;
		w = cmalloc(n  * sizeof(char));
		strncpy(w, pc, n);
		w[n] = 0;
	} else {
		w = cmalloc(sizeof(char));
		strncpy(w, "\0", 1);
	}

	return w;
} 

/* Fonction interne */
char *build_path(char *word, int *n)
{
	char *path = malloc( (strlen(word) + 3) * sizeof(char));
	/*si le dernier mot de la ligne est un répertoire l'autocompletion explorera ce répertoire*/
	if (word[strlen(word) - 1] == '/') {
		/*par défaut, on cherche dans le répertoire courant*/
		if (strncmp(word, "/", 1) && strncmp(word, "./", 2) && strncmp(word, "../", 3)) {
			strncpy(path, "./", 2);
		} else {
			strncpy(path, "\0", 1);
		}
		strncat(path, word, strlen(word) + 1);
		*n = 1;
		return path;
	} else {
		/*sinon, on cherche dans le path*/
		return getenv("PATH");
	}
}

/*cherche une commande qui commence comme la chaine word*/
char* look_up_command(char *word, int nb, int afc)
{
        char *p_env = getenv("PATH");
	char *p = p_env;
	char def[] = "./:../:";

	/*rien n'avait été tapé*/
	if (strncmp("\0", word, 1) == 0) {
		p = p_env;
	}

	/*une commande avait été tapée*/
	if (afc) {
		if (strncmp("\0", word, 1) == 0) {
			p = def;
		} else {
			/*on récupère le path de recherche*/
			char *ptemp = word + (strlen(word) - 1);
			char *pathtmp = cmalloc((strlen(word) + 1) * sizeof(char));
			int cpt = strlen(word);

			while (*ptemp != '/' && ptemp >= word) {
				ptemp--;
				cpt--;
			}
			if (ptemp < word) {
				p = def;
			} else {
				strncpy(pathtmp, word, cpt);
				*(pathtmp + cpt) = '\0';
				p = pathtmp;
			}
			word = ptemp + 1;
		}
	} else {
		if (strncmp("./", word, 2) == 0) {
			p = def;
			word = word + 2;
		}
		if (strncmp("../", word, 3) == 0) {
			p = def + 3;
			word = word + 3;
		}
	}

	char *token = cmalloc(sizeof(char) * strlen(p));
	strncpy(token, p, strlen(p) + 1);
	char *path = strtok(token, ":");
	size_t n = strlen(word);
	int found = nb;
	struct dirent *cur_dir;
	DIR *rep;
	/*variables temporaires pour la construction de la commande*/
	char *command1 = cmalloc(256 * sizeof(char));
	char *command2 = cmalloc(256 * sizeof(char));
	/*variable retournée*/
	char *command = command2;
	while (found > 0) {
		if (path != NULL) {
			rep = opendir(path);
			if (rep != NULL) {
				while ((cur_dir = readdir(rep)) && found > 0) {
					if (strncmp(word, cur_dir->d_name, n) == 0) {
						found--;/*on décrémente pour atteindre 0*/
					}
					if (found == 0) {
						strncpy(command2, cur_dir->d_name, strlen(cur_dir->d_name) + 1);
						if (afc || p == def || p == def + 3) {
							strncpy(command1, path, strlen(path) + 1);
							strncat(command1, command2, strlen(command2) + 1);
							cfree(command2);
							if (opendir(command1) != NULL) {
								strncat(command1, "/", 1);
							}
							command = command1;
						} else {
							cfree(command1);
						}
					}
				}/*found peut etre non nul, on continue alors dans le répertoire suivant*/
			}
			path = strtok(NULL, ":");/*rep suivant*/
			closedir(rep);
		} else {
			found = 0;
			strncpy(command2, "", 1);/*rien n'est trouvé, "" est retourné*/
		}
	}
	free(token);
	return command;
}
