/**
 * @file mouse.cpp
 * Mouse class implementation.
 */

#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdio.h>

#include <libtacos.h>
#include <mouse.h>
#include <screen.h>
#include <window.h>

Mouse*       Mouse::instance       = NULL;
unsigned int Mouse::defaultPointer = 0;

static int alphaColor = 0x00ff2cf3; /**< Alpha color (will be considered as transparent) */

Mouse::Mouse() {
  this->fd = open("/dev/mouse", 0);
  this->pointerBackupX = 0;
  this->pointerBackupY = 0;
  Screen *screen = Screen::getInstance();
  this->pointer        = new Pixmap(screen, 1, NULL, PRON_MOUSE_POINTER_WIDTH, PRON_MOUSE_POINTER_HEIGHT, screen->bitsPerPixel);
  this->pointerBackup  = new Pixmap(screen, 2, NULL, PRON_MOUSE_POINTER_WIDTH, PRON_MOUSE_POINTER_HEIGHT, screen->bitsPerPixel);
  this->pointerHidden  = true;

  this->mouseX  = 0;
  this->mouseY  = 0;
  this->mouseB1 = false;
  this->mouseB2 = false;
  this->mouseB3 = false;
  this->mouseB4 = false;
  this->mouseB5 = false;
  this->mouseB6 = false;

  this->lastMouseEvent = 0;
  this->lastSentX      = 0;
  this->lastSentY      = 0;

  /** @todo xxx 0x00FDFEFB is transparent */
  Color oldFg = screen->getGC()->getFg();
  screen->getGC()->setFg(Color((alphaColor&0x00ff0000)>> 16, (alphaColor&0x0000ff00)>> 8, (alphaColor&0x000000ff)));

  this->pointer->fillRectangle(0, 0, PRON_MOUSE_POINTER_WIDTH, PRON_MOUSE_POINTER_HEIGHT);

  screen->getGC()->setFg(Color(0, 0, 0));

  // Interrieur noir
  this->pointer->drawLine(PRON_MOUSE_POINTER_WIDTH_SHIFT  + 1,
                          PRON_MOUSE_POINTER_HEIGHT_SHIFT + 2,
                          PRON_MOUSE_POINTER_WIDTH_SHIFT  + 1,
                          PRON_MOUSE_POINTER_HEIGHT_SHIFT + 15);
  this->pointer->drawLine(PRON_MOUSE_POINTER_WIDTH_SHIFT  + 2,
                          PRON_MOUSE_POINTER_HEIGHT_SHIFT + 3,
                          PRON_MOUSE_POINTER_WIDTH_SHIFT  + 2,
                          PRON_MOUSE_POINTER_HEIGHT_SHIFT + 14);
  this->pointer->drawLine(PRON_MOUSE_POINTER_WIDTH_SHIFT  + 3,
                          PRON_MOUSE_POINTER_HEIGHT_SHIFT + 4,
                          PRON_MOUSE_POINTER_WIDTH_SHIFT  + 3,
                          PRON_MOUSE_POINTER_HEIGHT_SHIFT + 13);
  this->pointer->drawLine(PRON_MOUSE_POINTER_WIDTH_SHIFT  + 4,
                          PRON_MOUSE_POINTER_HEIGHT_SHIFT + 5,
                          PRON_MOUSE_POINTER_WIDTH_SHIFT  + 4,
                          PRON_MOUSE_POINTER_HEIGHT_SHIFT + 13);
  this->pointer->drawLine(PRON_MOUSE_POINTER_WIDTH_SHIFT  + 5,
                          PRON_MOUSE_POINTER_HEIGHT_SHIFT + 6,
                          PRON_MOUSE_POINTER_WIDTH_SHIFT  + 5,
                          PRON_MOUSE_POINTER_HEIGHT_SHIFT + 14);
  this->pointer->drawLine(PRON_MOUSE_POINTER_WIDTH_SHIFT  + 6,
                          PRON_MOUSE_POINTER_HEIGHT_SHIFT + 7,
                          PRON_MOUSE_POINTER_WIDTH_SHIFT  + 6,
                          PRON_MOUSE_POINTER_HEIGHT_SHIFT + 16);
  this->pointer->drawLine(PRON_MOUSE_POINTER_WIDTH_SHIFT  + 7,
                          PRON_MOUSE_POINTER_HEIGHT_SHIFT + 8,
                          PRON_MOUSE_POINTER_WIDTH_SHIFT  + 7,
                          PRON_MOUSE_POINTER_HEIGHT_SHIFT + 11);
  this->pointer->drawLine(PRON_MOUSE_POINTER_WIDTH_SHIFT  + 7,
                          PRON_MOUSE_POINTER_HEIGHT_SHIFT + 15,
                          PRON_MOUSE_POINTER_WIDTH_SHIFT  + 7,
                          PRON_MOUSE_POINTER_HEIGHT_SHIFT + 17);
  this->pointer->drawLine(PRON_MOUSE_POINTER_WIDTH_SHIFT  + 8,
                          PRON_MOUSE_POINTER_HEIGHT_SHIFT + 8,
                          PRON_MOUSE_POINTER_WIDTH_SHIFT  + 8,
                          PRON_MOUSE_POINTER_HEIGHT_SHIFT + 11);
  this->pointer->drawLine(PRON_MOUSE_POINTER_WIDTH_SHIFT  + 9,
                          PRON_MOUSE_POINTER_HEIGHT_SHIFT + 10,
                          PRON_MOUSE_POINTER_WIDTH_SHIFT  + 9,
                          PRON_MOUSE_POINTER_HEIGHT_SHIFT + 11);
  this->pointer->drawPoint(PRON_MOUSE_POINTER_WIDTH_SHIFT + 10,
                          PRON_MOUSE_POINTER_HEIGHT_SHIFT + 10);

  screen->getGC()->setFg(oldFg);
  // Contour blanc
  this->pointer->drawLine(PRON_MOUSE_POINTER_WIDTH_SHIFT  + 0,
                          PRON_MOUSE_POINTER_HEIGHT_SHIFT + 0,
                          PRON_MOUSE_POINTER_WIDTH_SHIFT  + 0,
                          PRON_MOUSE_POINTER_HEIGHT_SHIFT + 17);
  this->pointer->drawLine(PRON_MOUSE_POINTER_WIDTH_SHIFT  + 0,
                          PRON_MOUSE_POINTER_HEIGHT_SHIFT + 0,
                          PRON_MOUSE_POINTER_WIDTH_SHIFT  + 12,
                          PRON_MOUSE_POINTER_HEIGHT_SHIFT + 12);
  this->pointer->drawLine(PRON_MOUSE_POINTER_WIDTH_SHIFT  + 0,
                          PRON_MOUSE_POINTER_HEIGHT_SHIFT + 17,
                          PRON_MOUSE_POINTER_WIDTH_SHIFT  + 3,
                          PRON_MOUSE_POINTER_HEIGHT_SHIFT + 14);
  this->pointer->drawLine(PRON_MOUSE_POINTER_WIDTH_SHIFT  + 4,
                          PRON_MOUSE_POINTER_HEIGHT_SHIFT + 14,
                          PRON_MOUSE_POINTER_WIDTH_SHIFT  + 5,
                          PRON_MOUSE_POINTER_HEIGHT_SHIFT + 15);
  this->pointer->drawLine(PRON_MOUSE_POINTER_WIDTH_SHIFT  + 5,
                          PRON_MOUSE_POINTER_HEIGHT_SHIFT + 16,
                          PRON_MOUSE_POINTER_WIDTH_SHIFT  + 7,
                          PRON_MOUSE_POINTER_HEIGHT_SHIFT + 18);
  this->pointer->drawPoint(PRON_MOUSE_POINTER_WIDTH_SHIFT + 6,
                          PRON_MOUSE_POINTER_HEIGHT_SHIFT + 18);
  this->pointer->drawLine(PRON_MOUSE_POINTER_WIDTH_SHIFT  + 8,
                          PRON_MOUSE_POINTER_HEIGHT_SHIFT + 15,
                          PRON_MOUSE_POINTER_WIDTH_SHIFT  + 8,
                          PRON_MOUSE_POINTER_HEIGHT_SHIFT + 18);
  this->pointer->drawLine(PRON_MOUSE_POINTER_WIDTH_SHIFT  + 7,
                          PRON_MOUSE_POINTER_HEIGHT_SHIFT + 12,
                          PRON_MOUSE_POINTER_WIDTH_SHIFT  + 7,
                          PRON_MOUSE_POINTER_HEIGHT_SHIFT + 14);
  this->pointer->drawLine(PRON_MOUSE_POINTER_WIDTH_SHIFT  + 8,
                          PRON_MOUSE_POINTER_HEIGHT_SHIFT + 12,
                          PRON_MOUSE_POINTER_WIDTH_SHIFT  + 12,
                          PRON_MOUSE_POINTER_HEIGHT_SHIFT + 12);

  Mouse::defaultPointer = this->pointer->getId();
}

void Mouse::checkEvents() {
  // Get the mouse state
  mousestate_t state;
  memset(&state, 0, sizeof(mousestate_t));
  read(this->fd, &state, sizeof(mousestate_t));

  // Handle motion mouse events
  this->handleMotion(&state);

  // Handle button mouse events
  this->handleButton(&state);
}

void Mouse::handleMotion(mousestate_t *state) {
  Screen *screen = Screen::getInstance();

  // Has pointer moved since the previous select ?
  if (this->mouseX != state->x || this->mouseY != state->y) {
    // Set the new coordinates in the screen
    this->mouseX = state->x;
    this->mouseY = state->y;

    // Move the pointer
    this->hidePointer();
    this->showPointer();

    // We have to recompute the mouseWin
    if (screen->getGrabWin() == NULL) {
      this->updateMouseWin();
      this->updateFocusWin();
    }
  }

  if (this->lastSentX != this->mouseX || this->lastSentY != this->mouseY) {
    struct timeval now;
    gettimeofday(&now, NULL);
    unsigned long now_ms = now.tv_sec * 1000 + now.tv_usec/1000;
    if (now_ms - this->lastMouseEvent > 60) {
      // Send the event to the current mouseWin
      Window *mouseWin = screen->getMouseWin();
      
      int xMove = this->mouseX - this->lastSentX;
      int yMove = this->mouseY - this->lastSentY;

      Window *wToDeliverEvent = screen->getGrabWin();

      if(wToDeliverEvent == NULL) {
        wToDeliverEvent = mouseWin;
      }
      
      EventPointerMoved pointerMoved(wToDeliverEvent->getId(),
            state->x - wToDeliverEvent->getX(), state->y - wToDeliverEvent->getY(), state->x, state->y, xMove, yMove);
      wToDeliverEvent->deliverDeviceEvent(&pointerMoved, sizeof(pointerMoved));

      this->lastMouseEvent = now_ms;
      this->lastSentX = this->mouseX;
      this->lastSentY = this->mouseY;
    }
  }
}

void Mouse::handleButton(mousestate_t *state) {
  Screen *screen = Screen::getInstance();

  // We get the window which consairns the event
  Window *mouseWin = screen->getMouseWin();

  bool updateGrabWin = this->mouseB1 != state->b1;

  // We have to compare the state of each buttons and if one button has changed, send the event
  if (this->mouseB1 != state->b1 ||
      this->mouseB2 != state->b2 ||
      this->mouseB3 != state->b3 ||
      this->mouseB4 != state->b4 ||
      this->mouseB5 != state->b5 ||
      this->mouseB6 != state->b6 ) {

    //debug("Mouse button state %d %d %d %d %d %d \n", state->b1, state->b2, state->b3, state->b4, state->b5, state->b6);

    this->mouseB1 = state->b1;
    this->mouseB2 = state->b2;
    this->mouseB3 = state->b3;
    this->mouseB4 = state->b4;
    this->mouseB5 = state->b5;
    this->mouseB6 = state->b6;

    Window *wToDeliverEvent = screen->getGrabWin();

    if(wToDeliverEvent == NULL) {
      wToDeliverEvent = mouseWin;
    }

    //printf("wToDeliverEvent Ox%x\n", wToDeliverEvent->getId());
    // Computing the relative coordinates
    int relX = state->x - wToDeliverEvent->getX();
    int relY = state->y - wToDeliverEvent->getY();

    // delivers the event
    EventMouseButton mouseButton(wToDeliverEvent->getId(), state->b1, state->b2,
                                 state->b3, state->b4, state->b5, state->b6,
                                 relX, relY, state->x, state->y);
    wToDeliverEvent->deliverDeviceEvent(&mouseButton, sizeof(mouseButton));
  }

  // Set the grabWin only when B1 has changed
  if (updateGrabWin) {
    if (state->b1 == true) {
      screen->setGrabWin(mouseWin);
    } else {
      screen->setGrabWin(NULL);
      this->updateMouseWin();
      this->updateFocusWin();
    }
  }
}

void Mouse::updateMouseWin() {
  Screen *screen = Screen::getInstance();
  // We have to explorate window tree from the  screen to his children
  // and check if the pointer is inside them : We can stop when we have found one window
  // because it is necessarily the one which is drawn
  Window *currentWin = screen->getRoot();
  // We stop when currentWin has no children 
  while (currentWin->lastChild != NULL) {
    // we have to run throug the childs from the right to the left
    // and take the first (the one with the bigger Z)
    Window *currentChild = currentWin->lastChild;
    while (currentChild != NULL) {
      if (currentChild->realized() && currentChild->contains(this->mouseX, this->mouseY)) {
        currentWin = currentChild;
        break;// We have found the first containing child we have to stop
      } 
      currentChild = currentChild->prevSibling;
    }
    if (currentChild == NULL) {
      break;// We have no child which contains the pointer we can stop
    }
  }

  if(currentWin != NULL) {
    Pixmap *newPointer = (Pixmap*) screen->getDrawable(currentWin->pointer, D_PIXMAP);
    if(newPointer != NULL) {
      this->pointer = newPointer;
    } else {
      this->pointer = (Pixmap*) screen->getDrawable(Mouse::defaultPointer, D_PIXMAP);
    }
  } else {
    this->pointer = (Pixmap*) screen->getDrawable(Mouse::defaultPointer, D_PIXMAP);
  }

  // MouseWin is computed we can store it in the screen to increzse performances
  screen->setMouseWin(currentWin);
}

void Mouse::updateFocusWin() {
  Screen *screen = Screen::getInstance();
  Window *newFocusWin = NULL;
  unsigned int eventMask = PRON_EVENTMASK(EV_KEY_PRESSED);

  Window *w = screen->getMouseWin();
  while (w != NULL) {
    if (w->acceptsEvents(eventMask)) {
      newFocusWin = w;
      break;
    } else if (w == screen->getRoot() || (w->dontPropagateMask & eventMask)) {
      newFocusWin = screen->getRoot();
      break;
    }
    w = w->parent;
  }

  screen->setFocusWin(newFocusWin);
}

void Mouse::hidePointer() {
  if (!this->pointerHidden) {
    Screen *screen = Screen::getInstance();

    for (int y = 0; y < PRON_MOUSE_POINTER_HEIGHT; ++y) {
      for (int x = 0; x < PRON_MOUSE_POINTER_WIDTH; ++x) {
        if (x + this->pointerBackupX - PRON_MOUSE_POINTER_WIDTH_SHIFT >= 0
            && x + this->pointerBackupX - PRON_MOUSE_POINTER_WIDTH_SHIFT < screen->width
            && y + this->pointerBackupY - PRON_MOUSE_POINTER_HEIGHT_SHIFT >= 0
            && y + this->pointerBackupY - PRON_MOUSE_POINTER_HEIGHT_SHIFT < screen->height) {
          void *destination = screen->getRoot()->pixelAddr(x + this->pointerBackupX - PRON_MOUSE_POINTER_WIDTH_SHIFT,
                                                           y + this->pointerBackupY - PRON_MOUSE_POINTER_HEIGHT_SHIFT);
          void *source = this->pointerBackup->pixelAddr(x, y);
          memcpy(destination, source, screen->bytesPerPixel);
        }
      }
    }
    
    this->pointerHidden= true;
  }
}

void Mouse::showPointer() {
  if (this->pointerHidden) {
    Screen *screen = Screen::getInstance();

    // Save the pointer background and draw the pointer
    this->pointerBackupX = this->mouseX;
    this->pointerBackupY = this->mouseY;

    for (int y = 0; y < PRON_MOUSE_POINTER_HEIGHT; ++y) {
      for (int x = 0; x < PRON_MOUSE_POINTER_WIDTH; ++x) {
        if (x + this->mouseX - PRON_MOUSE_POINTER_WIDTH_SHIFT >= 0
            && x + this->mouseX - PRON_MOUSE_POINTER_WIDTH_SHIFT < screen->width
            && y + this->mouseY - PRON_MOUSE_POINTER_HEIGHT_SHIFT >= 0
            && y + this->mouseY - PRON_MOUSE_POINTER_HEIGHT_SHIFT < screen->height) {
          // Backup old pixel
          void *destination = this->pointerBackup->pixelAddr(x, y);
          void *source = screen->getRoot()->pixelAddr(x + this->mouseX - PRON_MOUSE_POINTER_WIDTH_SHIFT,
                                                      y + this->mouseY - PRON_MOUSE_POINTER_HEIGHT_SHIFT);
          memcpy(destination, source, screen->bytesPerPixel);

          // Draw new pixel
          /** @todo xxx 0x00FDFEFB is transparent haha degueu */
          if (this->pointer->getPixel(x, y) != alphaColor) {
            destination = source;
            source = this->pointer->pixelAddr(x, y);
            memcpy(destination, source, screen->bytesPerPixel);
          }
        }
      }
    }

    this->pointerHidden = false;
  }
}
