#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>

#include "../utils/include/unittest.h"

/**
 * Test l'affichage de floats / doubles.
 */
void test_sprintf()
{
	char buf[80];

	sprintf(buf, "%f", 42.5);
	unit_test_str("Nombre > 0:", "42.500000", buf);

	sprintf(buf, "%f", .5);
	unit_test_str("Nombre > 0 et < 1:", "0.500000", buf);

	sprintf(buf, "%f", 12.34567891);
	unit_test_str("Nombre > 0 arrondi:", "12.345679", buf);

	sprintf(buf, "%f", -12.34567891);
	unit_test_str("Nombre < 0 arrondi:", "-12.345679", buf);

	sprintf(buf, "%lf", 42.5);
	unit_test_str("Nombre > 0 avec ajout de 0's:", "42.500000", buf);

	sprintf(buf, "%f", 42.001);
	unit_test_str("Nombre > 0 avec risque arrondi:", "42.001000", buf);

	sprintf(buf, "%lf", 12.34567891);
	unit_test_str("Affichage nombre avec %lf:", "12.345679", buf);

	sprintf(buf, "%.2f", 12.34567891);
	unit_test_str("Affichage avec %.2f:", "12.35", buf);

	sprintf(buf, "%.1f", .99);
	unit_test_str("Nombre > 0 et < 1 avec %.1f:", "1.0", buf);

	sprintf(buf, "%.5f", 12.34567891);
	unit_test_str("Affichage avec %.5f:", "12.34568", buf);

	sprintf(buf, "%.15f", 12.3456789123456789);
	unit_test_str("Affichage avec %.15f et arrondi:", "12.345678912345679", buf);

	sprintf(buf, "%.15f", 12.34567891);
	unit_test_str("Affichage avec %.15f et padding 0:", "12.345678910000000", buf);
}

/**
 * Test les ouvertures / lectures / écritures dans des fichiers.
 */
void test_io() {
	FILE * f;
	struct stat buf;
	int r;
	const char * fichiertest = "/tacos/fichiertestio";
	const char * dossiertest = "/tacos/dossiertestio";

	// Tests 1-6: Test la création de fichier selon le flag.

	// Test 1:
	unlink(fichiertest);
	f = fopen(fichiertest, "r");
	unit_test_int("Ouverture d'un fichier inexistant avec 'r'", 1, f == NULL);
	if (f) fclose(f);

	// Test 2:
	unlink(fichiertest);
	f = fopen(fichiertest, "r+");
	unit_test_int("Ouverture d'un fichier inexistant avec 'r+'", 1, f == NULL);
	if (f) fclose(f);

	// Test 3:
	unlink(fichiertest);
	f = fopen(fichiertest, "w");
	unit_test_int("Ouverture d'un fichier inexistant avec 'w'", 1, f != NULL);
	if (f) fclose(f);

	// Test 4:
	unlink(fichiertest);
	f = fopen(fichiertest, "w+");
	unit_test_int("Ouverture d'un fichier inexistant avec 'w+'", 1, f != NULL);
	if (f) fclose(f);

	// Test 5:
	unlink(fichiertest);
	f = fopen(fichiertest, "a");
	unit_test_int("Ouverture d'un fichier inexistant avec 'a'", 1, f != NULL);
	if (f) fclose(f);

	// Test 6:
	unlink(fichiertest);
	f = fopen(fichiertest, "a+");
	unit_test_int("Ouverture d'un fichier inexistant avec 'a+'", 1, f != NULL);
	if (f) fclose(f);

	// Test 7:
	unlink(fichiertest);
	f = fopen(fichiertest, "w+");
	if (f) fclose(f);
	stat(fichiertest, &buf);
	unit_test_int("Longueur d'un fichier ouvert avec 'w+' doit etre de 0 (fichier inexistant)", 0, buf.st_size);
	
	// Test 8:
	unlink(fichiertest);
	f = fopen(fichiertest, "w");
	if (f) {
		fwrite("0123456789", sizeof(char), 10, f);
		fclose(f);
	}
	f = fopen(fichiertest, "w+");
	if (f) fclose(f);
	stat(fichiertest, &buf);
	unit_test_int("Longueur d'un fichier ouvert avec 'w+' doit etre de 0 (fichier non vide)", 0, buf.st_size);

	// Test 9:
	unlink(fichiertest);
	f = fopen(fichiertest, "w");
	if (f) {
		fwrite("0123456789", sizeof(char), 10, f);
		fclose(f);
	}
	f = fopen(fichiertest, "r");
	char buffer[11];
	if (f) {
		fread(buffer, sizeof(char), 10, f);
		buffer[10] = '\0';
		fclose(f);
	}
	unit_test_str("Lecture d'un fichier depuis le début.", "0123456789", buffer);

	// Test 10:
	unlink(fichiertest);
	f = fopen(fichiertest, "w");
	if (f) {
		fwrite("0123456789", sizeof(char), 10, f);
		fclose(f);
	}
	f = fopen(fichiertest, "a");
	if (f) {
		fwrite("1011121314", sizeof(char), 10, f);
		fclose(f);
	}
	char buffer2[21];
	f = fopen(fichiertest, "r");
	if (f) {
		fread(buffer2, sizeof(char), 20, f);
		buffer2[20] = '\0';
		fclose(f);
	}
	unit_test_str("Ecriture, fermeture, Append, Lecture.", "01234567891011121314", buffer2);

	// Test 11:
	unlink(fichiertest);
	f = fopen(fichiertest, "w");
	if (f) {
		fwrite("01234567891011121314", sizeof(char), 20, f);
		fclose(f);
	}
	f = fopen(fichiertest, "r");
	if (f) {
		fseek(f, 5, SEEK_SET);
		fread(buffer, sizeof(char), 10, f);
		fclose(f);
	}
	unit_test_str("Ecriture, fermeture, fseek, lecture.", "5678910111", buffer);

	// Test 12:
	unlink(fichiertest);
	f = fopen(fichiertest, "w");
	if (f) fclose(f);
	f = fopen(fichiertest, "r");
	if (f) {
		r = fwrite("01234567891011121314", sizeof(char), 20, f);
		fclose(f);
	} else {
		r = -1;
	}
	unit_test_int("Ecriture sur fichier ouvert en lecture (retour fonction).", 0, r);
	stat(fichiertest, &buf);
	unit_test_int("Ecriture sur fichier ouvert en lecture (taille fichier).", 0, buf.st_size);

	// Test 13:
	unlink(fichiertest);
	f = fopen(fichiertest, "w");
	if (f) {
		r = fwrite("01234567891011121314", sizeof(char), 20, f);
		fclose(f);
	}
	unit_test_int("Ecriture sur fichier ouvert en écriture.", 20, r);
	unlink(fichiertest);

	// Test 14:
	DIR *d = opendir(dossiertest);
	unit_test_int("Ouverture d'un dossier non existant.", (int)NULL, (int)d);

	// Test 15:
	mkdir(dossiertest, 777);
	stat(dossiertest, &buf);
	unit_test_int("Création d'un dossier non existant.", 1, S_ISDIR(buf.st_mode));

	// Test 16:
	r = mkdir(dossiertest, 777);
	unit_test_int("Création d'un dossier existant doit renvoyer EEXIST.", EEXIST, errno);

	// Test 17:
	f = fopen(fichiertest, "w");
	r = mkdir(fichiertest, 777);
	unit_test_int("Création d'un dossier portant un nom déjà pris doit renvoyer EEXIST.", EEXIST, errno);
	unlink(fichiertest);
	fclose(f);

	// Test 18:
	r = mkdir("/tacos/bla123456789/bla", 777);
	unit_test_int("Création d'un dossier avec un des sous-dossier inexistant (ENOENT).", ENOENT, errno);

	// Test 19:
	mkdir(dossiertest, 777);
	r = rmdir(dossiertest);
	unit_test_int("Suppression d'un dossier existant (retour fonction).", 0, r);
	DIR* dir = opendir(dossiertest);
	unit_test_int("Suppression d'un dossier existant (opendir).", (int)NULL, (int)dir);
}

