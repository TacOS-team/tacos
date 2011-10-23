#ifndef _TERMIOS_H
#define _TERMIOS_H

typedef unsigned int tcflag_t;
typedef unsigned int cc_t;

#define NCCS 15

struct termios {
	tcflag_t c_iflag;
	tcflag_t c_oflag;
	tcflag_t c_cflag;
	tcflag_t c_lflag;
	cc_t c_cc[NCCS];
};

int tcgetattr(int fd, struct termios *termios_p);
int tcsetattr(int fd, int optional_actions, const struct termios *termios_p);

#endif
