/**
 * @file pron.cpp
 * Pron main file.
 */ 

#include <stdio.h>
#include <string>
#include <unistd.h>
#include <vector>

#include <client.h>
#include <keyboard.h>
#include <mouse.h>
#include <screen.h>
#include <window.h>

#include <libtacos.h>
#include <tsock.h>

using namespace std;

static Screen *screen; /**< Screen instance */
static Mouse *mouse; /**< Mouse instance  */
static Keyboard *keyboard; /**< Keyboard instance */
int clientsFd; /**< File descriptor of the socket listening for new clients */
unsigned int newClientID; /**< Id given to the last client who has connected to pron */

/**
 * Initialisation function.
 * Creates the screen and the root window, initializes devices
 * (keyboard and mouse) and listens for clients.
 */
void PronInit() {
  printf(                                             
    "                                             " "\n"
    "     _/_/_/    _/  _/_/    _/_/    _/_/_/    " "\n"
    "    _/    _/  _/_/      _/    _/  _/    _/   " "\n"
    "   _/    _/  _/        _/    _/  _/    _/    " "\n"
    "  _/_/_/    _/          _/_/    _/    _/     " "\n"
    " _/                                          " "\n"
    "_/                                           " "\n"
  );

  // Initialize screen
  screen = Screen::getInstance(800, 600, 24);
  
  // Initialize devices
  mouse = Mouse::getInstance();
  keyboard = new Keyboard();
  
  // Create root window
  screen->tree->setRoot(new Window(screen, 0, NULL, NULL, 0, 0, 800, 600));
  screen->tree->getRoot()->map();
  screen->tree->getRoot()->clear();

  // The id of the first client to connect will be 0
  newClientID = 0;

  // Listen for clients
  unlink("/tmp/pron.sock");
  clientsFd = tsock_listen("/tmp/pron.sock");
  tsock_set_nonblocking(clientsFd);
}

/**
 * Tries to accept a new client.
 */
void PronAcceptClient() {
  int newClientFd;
  if ((newClientFd = tsock_accept(clientsFd)) > 0) {
    tsock_set_nonblocking(newClientFd);
    new Client(++newClientID, newClientFd);
  }
}

/**
 * Main iteration: reads all file descriptors to accept new clients,
 * handle existing clients and read from devices (keyboard and mouse).
 */
void PronSelect() {
  // Try to accept new client
  PronAcceptClient();

  // Handle requests from clients
  for (unsigned int client = 0; client < Client::clients.size(); client++) {
    Client::clients[client]->handle();
  }
  
  // Read from devices
  mouse->checkEvents();
  keyboard->checkEvents();
}

/**
 * Pron main function.
 */
int main() {
  PronInit();

  while (1) {
    PronSelect();
  }

  return 0;
}
