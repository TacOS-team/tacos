#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sock_types.h>

#include <tsock.h>

int tsock_listen(const char *path) {
  int server = open("/dev/sock", O_RDWR);

  if (server == -1) {
    //perror("socket");
    return -1;
  }

  if (ioctl(server, SOCK_LISTEN, (char*) path) != 0) {
    //perror("listen");
    return -1;
  }

  return server;
}

int tsock_connect(const char *path) {
  int client = open("/dev/sock", O_RDWR);

  if (client == -1) {
    //perror("socket");
    return -1;
  }

  if (ioctl(client, SOCK_CONNECT, (char*) path) != 0) {
    //perror("connect");
    return -1;
  }

  return client;
}

int tsock_accept(int server) {
  int newServer;

  if (ioctl(server, SOCK_ACCEPT, &newServer) != 0) {
    //perror("accept");
    return -1;
  }

  return newServer;
}

ssize_t tsock_read(int tsock, void *buffer, size_t len) {
  ssize_t nbBytesRcvd = read(tsock, buffer, len);

  if (nbBytesRcvd == -1) {
    //perror("read");
  }

  return nbBytesRcvd;
}

ssize_t tsock_write(int tsock, void * buffer, size_t len) {
  ssize_t nbBytesSent = write(tsock, buffer, len);

  if (nbBytesSent == -1) {
    //perror("write");
  }

  return nbBytesSent;
}

void tsock_close(int tsock) {
  if (close(tsock) != 0) {
    //perror("close");
  }
}

int tsock_set_nonblocking(int tsock) {
  return ioctl(tsock, SOCK_SETNONBLOCK, NULL);
}

int tsock_set_blocking(int tsock) {
  return ioctl(tsock, SOCK_SETBLOCK, NULL);
}
