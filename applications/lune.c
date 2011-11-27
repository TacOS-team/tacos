#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

static int fd;

static void note_DO(char octave, char duree) {
	char buf[2];
	buf[0] = octave << 4;
	buf[1] = duree | 0x80;
	write(fd, buf, 2);
}

static void note_RE(char octave, char duree) {
	char buf[2];
	buf[0] = octave << 4 | 1;
	buf[1] = duree | 0x80;
	write(fd, buf, 2);
}


static void note_MI(char octave, char duree) {
	char buf[2];
	buf[0] = octave << 4 | 2;
	buf[1] = duree | 0x80;
	write(fd, buf, 2);
}

static void note_FA(char octave, char duree) {
	char buf[2];
	buf[0] = octave << 4 | 3;
	buf[1] = duree | 0x80;
	write(fd, buf, 2);
}

static void note_SOL(char octave, char duree) {
	char buf[2];
	buf[0] = octave << 4 | 4;
	buf[1] = duree | 0x80;
	write(fd, buf, 2);
}

static void note_LA(char octave, char duree) {
	char buf[2];
	buf[0] = octave << 4 | 5;
	buf[1] = duree | 0x80;
	write(fd, buf, 2);
}

static void note_SI(char octave, char duree) {
	char buf[2];
	buf[0] = octave << 4 | 6;
	buf[1] = duree | 0x80;
	write(fd, buf, 2);
}

int main() {
	fd = open("/dev/beeper", 0); //XXX: flags.
	
	// Play Au clair de la lune.
	
	note_DO(1, 1);
	note_DO(1, 1);
	note_DO(1, 1);
	note_RE(1, 1);
	note_MI(1, 2);
	note_RE(1, 2);
	
	note_DO(1, 1);
	note_MI(1, 1);
	note_RE(1, 1);
	note_RE(1, 1);
	note_DO(1, 4);
	
	note_DO(1, 1);
	note_DO(1, 1);
	note_DO(1, 1);
	note_RE(1, 1);
	note_MI(1, 2);
	note_RE(1, 2);
	
	note_DO(1, 1);
	note_MI(1, 1);
	note_RE(1, 1);
	note_RE(1, 1);
	note_DO(1, 4);
	
	note_RE(1, 1);
	note_RE(1, 1);
	note_RE(1, 1);
	note_RE(1, 1);
	note_LA(0, 2);
	note_LA(0, 2);
	
	note_RE(1, 1);
	note_DO(1, 1);
	note_SI(0, 1);
	note_LA(0, 1);
	note_SOL(0, 4);
	
	note_DO(1, 1);
	note_DO(1, 1);
	note_DO(1, 1);
	note_RE(1, 1);
	note_MI(1, 2);
	note_RE(1, 2);
	
	note_DO(1, 1);
	note_MI(1, 1);
	note_RE(1, 1);
	note_RE(1, 1);
	note_DO(1, 4);

	return 0;
}
