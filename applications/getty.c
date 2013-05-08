#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

int main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "Usage : %s /dev/ttyX\n", argv[0]);
	}

	//printf("Ouverture de %s\n", argv[1]);

	// On ferme les descripteurs actuels.
	fclose(stdin);
	fclose(stdout);
	fclose(stderr);
	
	stdin = fopen(argv[1], "r");
	stdout = fopen(argv[1], "w");
	stderr = fopen(argv[1], "w");

	int cttyfd = open(argv[1], O_RDWR);
	ioctl(cttyfd, TIOCSCTTY, argv[1]);
	close(cttyfd);

	exec_elf("/tacos/bin/mishell");

	return 0;
}
