#include <tsock.h>
#include <stdio.h>
#include <string.h>

void wait() {
	printf("[Press Enter to continue...]\n");
	while (getchar() != '\n');
}

int main() {
	printf("Connecting to server...\n");
	int sd = tsock_connect("/server.sock");
	printf("OK sd = %d\n", sd);

	wait();

	char msg[255] = "Coucou :-)";
	printf("Writing message...\n");
	tsock_write(sd, msg, sizeof(msg));

	wait();

	printf("Reading message...\n");
	tsock_read(sd, msg, sizeof(msg));
	printf("Response from server: %s\n", msg);

	wait();
	
	printf("Closing socket...\n");
	tsock_close(sd);

	return 0;
}