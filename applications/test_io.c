#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

static void padding(int c) {
	while (c-- > 0) {
		printf(" ");
	}
}

static void unit_test_int(const char* msg, int attendu, int obtenu) {
	int len = strlen(msg);
	printf("%s", msg);
	if (attendu == obtenu) {
		padding(80 - len - 4);
		printf("[OK]\n");
	} else {
		padding(80 - len - 7);
		printf("[ERROR]\n");
		printf("Attendu : %d, Obtenu : %d\n", attendu, obtenu);
	}
}

static void unit_test_str(const char* msg, const char* attendu, const char* obtenu) {
	int len = strlen(msg);
	printf("%s", msg);
	if (strcmp(attendu, obtenu) == 0) {
		padding(80 - len - 4);
		printf("[OK]\n");
	} else {
		padding(80 - len - 7);
		printf("[ERROR]\n");
		printf("Attendu : %s, Obtenu : %s\n", attendu, obtenu);
	}
}

int main() {
	FILE * f;
	struct stat buf;

	// Tests 1-6: Test la création de fichier selon le flag.

	// Test 1:
	unlink("fichiertestio");
	f = fopen("fichiertestio", "r");
	unit_test_int("Ouverture d'un fichier inexistant avec 'r'", 1, f == NULL);
	if (f) fclose(f);

	// Test 2:
	unlink("fichiertestio");
	f = fopen("fichiertestio", "r+");
	unit_test_int("Ouverture d'un fichier inexistant avec 'r+'", 1, f == NULL);
	if (f) fclose(f);

	// Test 3:
	unlink("fichiertestio");
	f = fopen("fichiertestio", "w");
	unit_test_int("Ouverture d'un fichier inexistant avec 'w'", 1, f != NULL);
	if (f) fclose(f);

	// Test 4:
	unlink("fichiertestio");
	f = fopen("fichiertestio", "w+");
	unit_test_int("Ouverture d'un fichier inexistant avec 'w+'", 1, f != NULL);
	if (f) fclose(f);

	// Test 5:
	unlink("fichiertestio");
	f = fopen("fichiertestio", "a");
	unit_test_int("Ouverture d'un fichier inexistant avec 'a'", 1, f != NULL);
	if (f) fclose(f);

	// Test 6:
	unlink("fichiertestio");
	f = fopen("fichiertestio", "a+");
	unit_test_int("Ouverture d'un fichier inexistant avec 'a+'", 1, f != NULL);
	if (f) fclose(f);

	// Test 7:
	unlink("fichiertestio");
	f = fopen("fichiertestio", "w+");
	if (f) fclose(f);
	stat("fichiertestio", &buf);
	unit_test_int("Longueur d'un fichier ouvert avec 'w+' doit etre de 0 (fichier inexistant)", 0, buf.st_size);
	
	// Test 8:
	unlink("fichiertestio");
	f = fopen("fichiertestio", "w");
	fwrite("0123456789", sizeof(char), 10, f);
	if (f) fclose(f);
	f = fopen("fichiertestio", "w+");
	if (f) fclose(f);
	stat("fichiertestio", &buf);
	unit_test_int("Longueur d'un fichier ouvert avec 'w+' doit etre de 0 (fichier non vide)", 0, buf.st_size);

	// Test 9:
	unlink("fichiertestio");
	f = fopen("fichiertestio", "w");
	fwrite("0123456789", sizeof(char), 10, f);
	if (f) fclose(f);
	f = fopen("fichiertestio", "r");
	char buffer[11];
	fread(buffer, sizeof(char), 10, f);
	buffer[10] = '\0';
	if (f) fclose(f);
	unit_test_str("Lecture d'un fichier depuis le début.", "0123456789", buffer);

	// Test 10:
	unlink("fichiertestio");
	f = fopen("fichiertestio", "w");
	fwrite("0123456789", sizeof(char), 10, f);
	if (f) fclose(f);
	f = fopen("fichiertestio", "a");
	fwrite("1011121314", sizeof(char), 10, f);
	if (f) fclose(f);
	char buffer2[21];
	f = fopen("fichiertestio", "r");
	fread(buffer2, sizeof(char), 20, f);
	buffer2[20] = '\0';
	if (f) fclose(f);
	unit_test_str("Ecriture, fermeture, Append, Lecture.", "01234567891011121314", buffer2);

	// Test 11:
	unlink("fichiertestio");
	f = fopen("fichiertestio", "w");
	fwrite("01234567891011121314", sizeof(char), 20, f);
	if (f) fclose(f);
	f = fopen("fichiertestio", "r");
	fseek(f, 5, SEEK_SET);
	fread(buffer, sizeof(char), 10, f);
	if (f) fclose(f);
	unit_test_str("Ecriture, fermeture, fseek, lecture.", "5678910111", buffer);

	return 0;
}
