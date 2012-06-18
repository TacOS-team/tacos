#include <tsock.h>
#include <stdio.h>
#include <string.h>

void wait() {
	printf("[Press Enter to continue...]\n");
	while (getchar() != '\n');
}

int main() {
	printf("Listening for connections...\n");
	int sd = tsock_listen("/server.sock");
	printf("OK sd = %d, accepting new connection...\n", sd);
	int newSd = tsock_accept(sd);
	printf("OK newSd = %d\n", newSd);

	wait();

	char msg[255];
	printf("Reading message from client...\n", newSd);
	tsock_read(newSd, msg, sizeof(msg));
	printf("Message from client: %s\n", msg);

	wait();

	char resp[255] = "I have read '";
	strcat(resp, msg);
	strcat(resp, "'");
	printf("Replying to client...\n");
	tsock_write(newSd, resp, sizeof(resp));

	wait();

	printf("Closing the sockets...\n");
	tsock_close(newSd);
	tsock_close(sd);

	return 0;
}