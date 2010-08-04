#include <ctype.h>

int isalnum(int c) {
	return isalpha(c) || isdigit(c);
}

int isalpha(int c) {
	return isupper(c) || islower(c);
}

int isblank(int c) {
	return c == ' ' || c == '\t';
}

int iscntrl(int c) {
	return c <= 0x1F || c == 0x7F;
}

int isdigit(int c) {
	return c >= '0' && c <= '9';
}

int islower(int c) {
	return (c >= 'a' && c <= 'z'); 
}

int isprint(int c) {
	// TODO
	return !isalnum(c) && !isspace(c); // Attention, simple approximation !
}

int ispunct(int c) {
	return !isalnum(c) && !isspace(c);
}

int isspace(int c) {
	return c == ' ' || c == '\t' || c == '\f' || c == '\n' || c == '\r' || c == '\v';
}

int isupper(int c) {
	return (c >= 'A' && c <= 'Z');
}

int isxdigit(int c) {
	return isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

int tolower(int c) {
	if (isupper(c)) {
		return c + ('a' - 'A');
	}
  return c;
}

int toupper(int c) {
	if (islower(c)) {
		return c + ('A' - 'a');
	}
  return c;
}

