#include <kerrno.h>
#include <fs/devfs.h>
#include <types.h>
#include <klog.h>
#include <klibc/string.h>
#include <kmalloc.h>
#include <kprocess.h>
#include <ksem.h>
#include <scheduler.h>
#include <vfs.h>

#include <sock_types.h>

#define MAX_OPEN_SOCK 10
#define MAX_MESSAGE_SIZE 1024
#define MAX_MESSAGES 10

// Note: La gestion des erreurs avec errno a été commentée : cette variable n'existe que dans la libc.

enum socket_type {
  client,
  nonlistening_server,
  listening_server
};

enum socket_state {
  closed,
  listen,
  established
};

struct socket_msg {
  char data[MAX_MESSAGE_SIZE];
  int size;
};

typedef struct socket {
  int sd;
  open_file_descriptor *ofd;
  enum socket_type type;
  bool blocking;
  char *meetpoint;
  enum socket_state state;
  struct socket **new_conn_req;
  int pending_conn_req;
  int max_conn_req_backlog;
  struct socket *paired_sock;
  struct socket_msg in_buffer[MAX_MESSAGES];
  int in_base;
  int in_count;
  uint8_t accept_sem;
  uint8_t connect_sem;
  uint8_t read_sem;
  uint8_t write_sem;
} socket_t;

socket_t *socket_descriptors[MAX_OPEN_SOCK];

void init_socket(int sd, open_file_descriptor *ofd, enum socket_type type, char *meetpoint) {
  socket_t *sock = socket_descriptors[sd];
  sock->sd = sd;
  sock->ofd = ofd;
  sock->type = type;
  sock->blocking = true;
  if (sock->type == listening_server) {
    sock->meetpoint = kmalloc(strlen(meetpoint) + 1);
    strcpy(sock->meetpoint, meetpoint);
  } else {
    sock->meetpoint = NULL;
  }
  sock->state = closed;
  sock->new_conn_req = NULL;
  sock->pending_conn_req = 0;
  sock->max_conn_req_backlog = 0;
  sock->paired_sock = NULL;
  memset(sock->in_buffer, 0, MAX_MESSAGES * sizeof(struct socket_msg));
  sock->in_base = 0;
  sock->in_count = 0;
  int val = 0;
  sock->accept_sem = ksemget(SEM_NEW, SEM_CREATE);
  ksemctl(sock->accept_sem, SEM_SET, &val);
  sock->connect_sem = ksemget(SEM_NEW, SEM_CREATE);
  ksemctl(sock->connect_sem, SEM_SET, &val);
  sock->read_sem = ksemget(SEM_NEW, SEM_CREATE);
  ksemctl(sock->read_sem, SEM_SET, &val);
  sock->write_sem = ksemget(SEM_NEW, SEM_CREATE);
  val = MAX_MESSAGES;
  ksemctl(sock->write_sem, SEM_SET, &val);
}

socket_t* create_socket(open_file_descriptor *ofd, enum socket_type type, char *meetpoint) {
  int sd;

  for (sd = 0; sd < MAX_OPEN_SOCK; sd++) {
    if (socket_descriptors[sd] == NULL) {
      socket_descriptors[sd] = kmalloc(sizeof(socket_t));

      if (socket_descriptors[sd] == NULL) {
        klog("kmalloc failed.");
        // errno = ENOMEM; // XXX
        return NULL;
      }

      klog("Found socket descriptor %d, initializing socket...", sd);
      init_socket(sd, ofd, type, meetpoint);

      return socket_descriptors[sd];
    }
  }

  klog("No free socket descriptor found.");
  // errno = ENFILE; // XXX
  return NULL;
}

socket_t* get_socket(char *meetpoint) {
  int sd;

  for (sd = 0; sd < MAX_OPEN_SOCK; sd++) {
    if (socket_descriptors[sd] != NULL &&
        socket_descriptors[sd]->meetpoint != NULL &&
        strcmp(socket_descriptors[sd]->meetpoint, meetpoint) == 0) {
      return socket_descriptors[sd];
    }
  }

  return NULL;
}

int sock_listen(open_file_descriptor *ofd, char *meetpoint) {
  // Create LISTENING_SERVER socket
  klog("Creating LISTENING_SERVER socket, meetpoint = %s.", meetpoint);
  socket_t *sock = create_socket(ofd, listening_server, meetpoint);
  if (sock != NULL) {
    sock->max_conn_req_backlog = 1;
    sock->new_conn_req = kmalloc(sock->max_conn_req_backlog * sizeof(socket_t*));
  } else {
    // errno set by create_socket()
    // return -errno;
    return -1;
  }

  return sock->sd;
}

int sock_connect(open_file_descriptor *ofd, char *meetpoint) {
  // Create CLIENT socket
  klog("Getting listening server for meetpoint %s...", meetpoint);
  socket_t *paired_sock = get_socket(meetpoint);

  if (paired_sock == NULL) {
    return -ENOENT;
  } else if (paired_sock->pending_conn_req > 0) {
    return -EAGAIN;
  }

  klog("OK paired_sd = %d, creating CLIENT socket...", paired_sock->sd);

  socket_t *sock = create_socket(ofd, client, NULL);
  sock->paired_sock = paired_sock;
  paired_sock->new_conn_req[0] = sock;
  paired_sock->pending_conn_req++;
  ksemV(paired_sock->accept_sem);
  paired_sock->ofd->inode->i_size = paired_sock->pending_conn_req;
  if (paired_sock->ofd->select_sem > 0) {
    ksemV(paired_sock->ofd->select_sem);
  }

  ksemP(sock->connect_sem);

  return sock->sd;
}

int sock_accept(open_file_descriptor *ofd) {
  socket_t *sock = (socket_t*) ofd->extra_data;

  // Create SERVER socket
  if (!sock->blocking && sock->pending_conn_req == 0) {
    return -EAGAIN;
  } else {
    // Wait while there is no pending connection
    ksemP(sock->accept_sem);
  }

  socket_t *client = sock->new_conn_req[0];
  sock->pending_conn_req--;
  sock->ofd->inode->i_size = sock->pending_conn_req;

  socket_t *server = create_socket(ofd, nonlistening_server, NULL);
  server->paired_sock = client;
  server->state = established;

  client->paired_sock = server;
  client->state = established;
  ksemV(client->connect_sem);

  return server->sd;
}

static int sock_ioctl(open_file_descriptor *ofd, unsigned int request, void *data) {
  switch (request) {
    case SOCK_CONNECT: {
      char *meetpoint = (char*) data;
      int sd = sock_connect(ofd, meetpoint);
      if (sd < 0) {
        return sd;
      } else {
        ofd->extra_data = socket_descriptors[sd];
        return 0;
      }
    }
    case SOCK_LISTEN: {
      char *meetpoint = (char*) data;
      int sd = sock_listen(ofd, meetpoint);
      if (sd < 0) {
        return sd;
      } else {
        ofd->extra_data = socket_descriptors[sd];
        return 0;
      }
    }
    case SOCK_ACCEPT: {
      int newSd = sock_accept(ofd);
      if (newSd < 0) {
        *((int*) data) = -1;
        return newSd;
      } else {
        // XXX: création d'un nouvel ofd, moche
        process_t *process = get_current_process();
        int i = 0;
        while (process->fd[i]) {
          i++;
        }

        process->fd[i] = kmalloc(sizeof(open_file_descriptor));
        memcpy(process->fd[i], ofd, sizeof(open_file_descriptor));
        process->fd[i]->extra_data = socket_descriptors[newSd];
        *((int*) data) = i;
        return 0;
      }
    }
    case SOCK_SETBLOCK: {
      socket_t *sock = (socket_t*) ofd->extra_data;
      sock->blocking = true;
      return 0;
    }
    case SOCK_SETNONBLOCK: {
      socket_t *sock = (socket_t*) ofd->extra_data;
      sock->blocking = false;
      return 0;
    }
    default: {
      return -1;
    }
  }
}

static ssize_t sock_read(open_file_descriptor* ofd, void* buf, size_t count __attribute__((unused))) {
  socket_t *sock = (socket_t*) ofd->extra_data;

  //klog("Waiting for a message to read on socket %d...", sock->sd);

  if (!sock->blocking && sock->in_count == 0) {
    return -EAGAIN;
  } else {
    // Wait while in_buffer is empty
    ksemP(sock->read_sem);
  }

  int lrcvd = sock->in_buffer[sock->in_base].size;
  memcpy(buf, sock->in_buffer[sock->in_base].data, lrcvd);
  sock->in_base = (sock->in_base + 1) % MAX_MESSAGES;
  sock->in_count--;
  ksemV(sock->write_sem);
  ofd->inode->i_size = sock->in_count;

  //klog("Read %d bytes from socket %d.", lrcvd, sock->sd);

  return lrcvd;
}

static ssize_t sock_write(open_file_descriptor* ofd, const void* buf, size_t count) {
  socket_t *sock = (socket_t*) ofd->extra_data;
  socket_t *paired_sock = sock->paired_sock;

  //klog("Waiting for the target buffer to have an empty place on socket %d...", sock->sd);

  if (!sock->blocking && paired_sock->in_count == MAX_MESSAGES) {
    return -EAGAIN;
  } else {
    // Wait while paired sock in_buffer is full
    ksemP(paired_sock->write_sem);
  }

  int in_top = (paired_sock->in_base + paired_sock->in_count) % MAX_MESSAGES;
  paired_sock->in_buffer[in_top].size = count;
  memcpy(&paired_sock->in_buffer[in_top].data, buf, count);
  paired_sock->in_count++;
  ksemV(paired_sock->read_sem);
  paired_sock->ofd->inode->i_size = paired_sock->in_count;
  if (paired_sock->ofd->select_sem > 0) {
    ksemV(paired_sock->ofd->select_sem);
  }

  //klog("Wrote %d bytes from socket %d to socket %d.", count, sock->sd, paired_sock->sd);

  return count;
}

int sock_open(open_file_descriptor *ofd) {
  ofd->inode->i_size = 0;
  return 0;
}

int sock_close(open_file_descriptor *ofd) {
  socket_t *sock = (socket_t*) ofd->extra_data;

  if (sock == NULL) {
    return -EBADF;
  }

  if (sock->new_conn_req != NULL) {
    kfree(sock->new_conn_req);
  }
  if (sock->meetpoint != NULL) {
    kfree(sock->meetpoint);
  }
  ksemctl(sock->accept_sem, SEM_DEL, NULL);
  ksemctl(sock->connect_sem, SEM_DEL, NULL);
  ksemctl(sock->read_sem, SEM_DEL, NULL);
  ksemctl(sock->write_sem, SEM_DEL, NULL);
  kfree(sock);
  ofd->extra_data = NULL;
  return 0;
}

static chardev_interfaces di = {
  .read = sock_read,
  .write = sock_write,
  .open = sock_open,
  .close = sock_close,
  .ioctl = sock_ioctl
};

void init_sock() {
  klog("initializing sock driver...");

  if (register_chardev("sock", &di) != 0) {
    kerr("error registering sock driver.");
  }
}
