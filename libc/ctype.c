#include "ctype.h"

int isalnum(int c) {
	return isalpha(c) || isdigit(c);
}

int isalpha(int c) {
	return isupper(c) || islower(c);
}

int isblank(int c) {
	return isspace(c) || c == '\t';
}

int iscntrl(int c) {
	return c <= 0x1F || c == 0x7F;
}

int isdigit(int c) {
	return c >= '0' && c <= '9';
}

int islower(int c) {
	return (c >= 'a' && c <= 'z') || iscntrl(c) || isdigit(c) || ispunct(c) || isspace(c);
}

int isprint(int c) {
	// TODO
	return 0;
}

int ispunct(int c) {
	return !isalnum(c) && !isspace(c);
}

int isspace(int c) {
	return c == ' ';
}

int isupper(int c) {
	return (c >= 'A' && c <= 'Z') || iscntrl(c) || isdigit(c) || ispunct(c) || isspace(c);
}

int isxdigit(int c) {
	// TODO
	return 0;
}

int tolower(int c) {
	if (isupper(c)) {
		return c + ('a' - 'A');
	}
}

int toupper(int c) {
	if (islower(c)) {
		return c + ('A' - 'a');
	}
}
