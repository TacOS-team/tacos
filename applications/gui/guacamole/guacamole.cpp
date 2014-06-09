#include <algorithm>
#include <signal.h>
#include <stdio.h>
#include <time.h>
#include <vector>
#include <stdio.h>

#include <gwindows_manager.h>
#include <libtacos.h>
#include <pronlib.h>

using namespace std;
using namespace pron;

Display *display = NULL;
GWindow *windowLeftButtonPressed   = NULL;
GWindow *windowResizeButtonPressed = NULL;
int mouseLastXPosition = -1;
int mouseLastYPosition = -1;
int mouseActualXPosition = -1;
int mouseActualYPosition = -1;
bool moving = false, resizing = false;
unsigned int w_num = 0;

void handleMouseMove() {
  int xMove = mouseLastXPosition - mouseActualXPosition;
  int yMove = mouseLastYPosition - mouseActualYPosition;

  if (windowLeftButtonPressed) {
    if (!moving) {
      moving = true;
      pronUnmapWindow(display, windowLeftButtonPressed->window);
    }
    windowLeftButtonPressed->move(xMove, yMove);
  } else if (windowResizeButtonPressed) {
    if (!resizing) {
      resizing = true;
      pronUnmapWindow(display, windowResizeButtonPressed->window);
    }
    windowResizeButtonPressed->resize(
        windowResizeButtonPressed->parentAttributes.width  + xMove,
        windowResizeButtonPressed->parentAttributes.height + yMove);
  }
  mouseActualXPosition = mouseLastXPosition;
  mouseActualYPosition = mouseLastYPosition;
}

int main() {
  srand(time(NULL));
  
  // connection to pron
  display = pronConnect();
  
  GWindowsManager::init(display->rootWindow);
  // Get the root window informations
  pronGetWindowAttributes(display, display->rootWindow, &GWindowsManager::getInstance()->getRootWindowAttributes());
  
  debug("Root window width : %d, height : %d\n", GWindowsManager::getInstance()->getRootWindowAttributes().width,
    GWindowsManager::getInstance()->getRootWindowAttributes().height);
  
  // Subscribe to root window events
  int currentRootEventMask =  PRON_EVENTMASK(EV_WINDOW_CREATED)
    | PRON_EVENTMASK(EV_KEY_PRESSED)
    | PRON_EVENTMASK(EV_KEY_RELEASED);
  pronSelectInput(display, display->rootWindow, currentRootEventMask);
  PronEvent *e = getPronEvent();

  while (1) {
    if (!pronNextEvent(display, e)) {
      fprintf(stderr, "pron has closed the connection.\n");
      exit(1);
    }

    switch (e->type) {
      case EV_WINDOW_CREATED : {
        //debug("EVENT_WINDOW_CREATED reçu\n");
        EventWindowCreated *windowCreated = (EventWindowCreated*) e;

        // we don't want to add a window that is already known
        if (windowCreated->parent == 0 && windowCreated->attributes.wm_decorate &&
            GWindowsManager::getInstance()->getGWindow(windowCreated->window) == NULL) {
          new GWindow(windowCreated->window, windowCreated->attributes, true, display);
        }
        break;
      }
      case EV_KEY_PRESSED : {
        EventKeyPressed *keyPressed = (EventKeyPressed*) e;
        if (keyPressed->keysym == pron::PRONK_F10) {
          //printf("F10 PRESSED OMG I HAVE TO GO FULLSCREEN\n");
          GWindow *w = GWindowsManager::getInstance()->getGWindow(keyPressed->window);
          if (w != NULL) {
            w->fullscreen();
          }
        }

        if (keyPressed->keysym == pron::PRONK_TAB && (keyPressed->modifiers & pron::PRONK_F11)) {
          GWindow *w = GWindowsManager::getInstance()->get(w_num++);
          if (w != NULL) {
            w->raise();
          }
        }

        printf("Key pressed : %d\n", keyPressed->keysym);
        break;
      }
      case EV_KEY_RELEASED : {
        EventKeyReleased *keyReleased = (EventKeyReleased*) e;
        //printf("Key released : %d\n", keyReleased->keysym);
        break;
      }
      case EV_DESTROY_WINDOW : {
        EventDestroyWindow *destroyWindowEvent = (EventDestroyWindow*) e;
        //printf("DestroyWindow event received for %d\n", destroyWindowEvent->window);

        // Sending destroy request for the parent window
        GWindow *pgwin = GWindowsManager::getInstance()->getGWindow(destroyWindowEvent->window);
        if (pgwin) {
          GWindowsManager::getInstance()->destroy(destroyWindowEvent->window);
        }
        break;
      }
      case EV_MOUSE_BUTTON : {
        EventMouseButton *mouseButtonEvent = (EventMouseButton*) e;
        //printf("mouse button event. ID : %d\n", mouseButtonEvent->window);
        GWindow *gwin = GWindowsManager::getInstance()->getGWindow(mouseButtonEvent->window);
        if (gwin && mouseButtonEvent->b1) {
          // If the window is the decoration window
          if (mouseButtonEvent->window == gwin->closeButton) {
            GWindowsManager::getInstance()->destroy(gwin->parent);
          } else if (mouseButtonEvent->window == gwin->maximiseButton) {
            gwin->maximise();
          } else {
            if (mouseButtonEvent->window == gwin->resizeButton) {
              windowResizeButtonPressed = gwin;
            }
            else if (mouseButtonEvent->window == gwin->parent) {
              windowLeftButtonPressed = gwin;
            }
            // Subscribe to pointer moved and mouse button of the root window
            //   to avoid problems if it moves too fast
            currentRootEventMask |= PRON_EVENTMASK(EV_POINTER_MOVED) | PRON_EVENTMASK(EV_MOUSE_BUTTON);
            // Ask to reset the last mouse position
            mouseActualXPosition = -1;
            // Puts the window on foreground
            gwin->raise();
          }
        } else if (!mouseButtonEvent->b1) {
          if (moving) {
            moving = false;
            pronMapWindow(display, windowLeftButtonPressed->window);
          }
          if (resizing) {
            resizing = false;
            pronMapWindow(display, windowResizeButtonPressed->window);
            pronResizeWindow(display, windowResizeButtonPressed->window,
                windowResizeButtonPressed->attributes.width, windowResizeButtonPressed->attributes.height);
          }
          windowLeftButtonPressed   = NULL;
          windowResizeButtonPressed = NULL;
          // Unsubscribe of events of the root window to avoid useless events
          currentRootEventMask &= ~PRON_EVENTMASK(EV_MOUSE_BUTTON);
          currentRootEventMask &= ~PRON_EVENTMASK(EV_POINTER_MOVED);
        } else {
          //printf("OLOL PUTAIN gwin = %p pour mouseButtonEvent->window = %x\n", gwin, mouseButtonEvent->window);
        }
        pronSelectInput(display, display->rootWindow, currentRootEventMask);
        break;
      }
      case EV_POINTER_MOVED : {
        EventPointerMoved *mousePointerEvent = (EventPointerMoved*) e;
        if (mouseActualXPosition == -1) {
          mouseActualXPosition = mousePointerEvent->xRoot;
          mouseActualYPosition = mousePointerEvent->yRoot;
        } else {
          handleMouseMove();
        }
        mouseLastXPosition = mousePointerEvent->xRoot;
        mouseLastYPosition = mousePointerEvent->yRoot;
        break;
      }
      case EV_EXPOSE : {
        EventExpose *mouseExposeEvent = (EventExpose*) e;
        GWindow *gwin = GWindowsManager::getInstance()->getGWindow(mouseExposeEvent->window);
        if (gwin) {
          gwin->decorate();
        }
      }
      default:
      break;
    }
  }
}
