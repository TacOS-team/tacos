#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sock_types.h>

#include <tsock.h>

int tsock_listen(const char *path) {
  int tsockServer = open("/dev/sock", O_RDWR);

  if (tsockServer == -1) {
    //perror("socket");
    return -1;
  }

  if (ioctl(tsockServer, SOCK_LISTEN, (char*) path) != 0) {
    //perror("listen");
    return -1;
  }

  /*if (ioctl(tsockServer, SOCK_SETNONBLOCK, NULL) != 0) {
    //perror("setnonblock");
    return -1;
  }*/

  return tsockServer;
}

int tsock_connect(const char *path) {
  int tsockClient = open("/dev/sock", O_RDWR);

  if (tsockClient == -1) {
    //perror("socket");
    return -1;
  }

  if (ioctl(tsockClient, SOCK_CONNECT, (char*) path) != 0) {
    //perror("connect");
    return -1;
  }

  return tsockClient;
}

int tsock_accept(int tsockServer) {
  int tsockNewServer;

  if (ioctl(tsockServer, SOCK_ACCEPT, &tsockNewServer) != 0) {
    //perror("accept");
    return -1;
  }

  /*if (tsockNewServer >= 0) {
    if (ioctl(tsockNewServer, SOCK_SETNONBLOCK, NULL) != 0) {
      //perror("setnonblock");
      return -1;
    }
  }*/

  return tsockNewServer;
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
