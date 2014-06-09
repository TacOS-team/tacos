/**
 * @file keyboard.cpp
 * Keyboard class implementation.
 */

#include <fcntl.h>
#include <screen.h>
#include <string.h>
#include <unistd.h>

#include <keyboard.h>
#include <proto/bits/keys.h>
#include <kbd_types.h>
#include <libtacos.h>
#include <window.h>

Keyboard::Keyboard() {
  this->fd = open("/dev/keyboard", 0);
}

void Keyboard::checkEvents() {
  Screen *screen = Screen::getInstance();

  // Get the keyboard state
  kbdstate_t state;
  memset(&state, 0, sizeof(kbdstate_t));

  if (read(this->fd, &state, 0) > 0) {
    Window *src = (Window *) screen->getFocusWin();
    if (src != NULL) {
      if (state.state == PRON_RELEASED) {
        EventKeyReleased keyReleased(src->getId(), src->getId(), 0, 0, 0, 0, state.keysym, state.modifiers, state.ch);
        src->deliverDeviceEvent(&keyReleased, sizeof(keyReleased));
      } else if (state.state == PRON_PRESSED) {
        EventKeyPressed keyPressed(src->getId(), src->getId(), 0, 0, 0, 0, state.keysym, state.modifiers, state.ch);
        src->deliverDeviceEvent(&keyPressed, sizeof(keyPressed));
      }
    }
  }
}
