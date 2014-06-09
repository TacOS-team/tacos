#ifndef __PRONPROTO_EVENTS_H__
#define __PRONPROTO_EVENTS_H__

#include <proto/base.h>
#include <proto/bits/attributes.h>
#include <stdlib.h>

#define PRON_EVENTMASK(n) (1 << (n & ~CATEGORY_MASK))

namespace pron {

/**
 * Base class representing a pron event message.
 */
struct PronEvent : public PronMessage {
  /**
   * Constructor.
   * @param type The message type
   * @param window The window the event is delivered to
   */
  PronEvent(MessageType type, unsigned int window)
      : PronMessage(type) {
    this->window = window;
  }

  unsigned int window; /**< Window the event is delivered to */
};

/**
 * WindowCreated event.
 * Sent by the server when a window has been created.
 */
struct EventWindowCreated : public PronEvent {
  /**
   * Constructor.
   * @param window The id of the created window
   * @param parent The id of the parent of the created window
   * @param attributes The attributes of the created window
   */
  EventWindowCreated(unsigned int window, unsigned int parent, const PronWindowAttributes &attributes)
      : PronEvent(EV_WINDOW_CREATED, window) {
    this->parent = parent;
    this->attributes = attributes;
  }

  unsigned int parent; /**< Parent of the created window */
  PronWindowAttributes attributes; /**< Attributes of the created window */
};

/**
 * Expose event.
 * Sent by the server when a window area has been exposed.
 */
struct EventExpose : public PronEvent {
  /**
   * Constructor.
   * @param window The window the event is delivered to
   * @param x The x-coordinate of top-left corner of the exposed area
   * @param y The y-coordinate of top-left corner of the exposed area
   * @param width The width of the exposed area
   * @param height The height of the exposed area
   */
  EventExpose(unsigned int window, int x, int y, int width, int height)
      : PronEvent(EV_EXPOSE, window) {
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
  }

  int x; /**< Exposed area top-left x-coordinate */
  int y; /**< Exposed area top-left y-coordinate */
  int width; /**< Exposed area width */
  int height; /**< Exposed area height */
};

/**
 * PointerMoved event.
 * Sent by the server when the pointer has moved.
 * @todo subwindow
 * @see https://www.etud.insa-toulouse.fr/redmine/projects/tacos-gui/wiki/%C3%89v%C3%A9nements_souris
 */
struct EventPointerMoved : public PronEvent {
  /**
   * Constructor.
   * @param window The window the event is delivered to
   * @param x The x-coordinate of the mouse (relative to window)
   * @param y The y-coordinate of the mouse (relative to window)
   * @param xRoot The x-coordinate of the mouse (relative to the root window)
   * @param yRoot The y-coordinate of the mouse (relative to the root window)
   * @param xMove The mouse x movement
   * @param yMove The mouse y movement
   */
  EventPointerMoved(unsigned int window, int x, int y, int xRoot, int yRoot, int xMove, int yMove)
      : PronEvent(EV_POINTER_MOVED, window) {
    this->x = x;
    this->y = y;
    this->xRoot = xRoot;
    this->yRoot = yRoot;
    this->xMove = xMove;
    this->yMove = yMove;
  }

  int x; /**< Mouse x position (relative to window) */
  int y; /**< Mouse y position (relative to window) */
  int xRoot; /**< Mouse absolute x position */
  int yRoot; /**< Mouse absolute y position */
  int xMove; /**< Mouse x movement */
  int yMove; /**< Mouse y movement */
};

/**
 * MouseButton event.
 * Sent by the server when a mouse button has changed state.
 * @todo subwindow
 * @see https://www.etud.insa-toulouse.fr/redmine/projects/tacos-gui/wiki/%C3%89v%C3%A9nements_souris
 */
struct EventMouseButton : public PronEvent {
 protected:
  /**
   * Initialization of a MouseButton event.
   * @param b1 The state of button 1 (left button)
   * @param b2 The state of button 2 (right button)
   * @param b3 The state of button 3
   * @param b4 The state of button 4
   * @param b5 The state of button 5
   * @param b6 The state of button 6
   * @param x The x-coordinate of the mouse (relative to window)
   * @param y The y-coordinate of the mouse (relative to window)
   * @param xRoot The x-coordinate of the mouse (relative to the root window)
   * @param yRoot The y-coordinate of the mouse (relative to the root window)
   */
  void init(bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, 
      int x, int y, int xRoot, int yRoot) {
    this->b1 = b1;
    this->b2 = b2;
    this->b3 = b3;
    this->b4 = b4;
    this->b5 = b5;
    this->b6 = b6;
    this->x  = x;
    this->y  = y;
    this->xRoot = xRoot;
    this->yRoot = yRoot;
  }

 public:
  /**
   * Constructor.
   * @param window The window the event is delivered to
   * @param b1 The state of button 1 (left button)
   * @param b2 The state of button 2 (right button)
   * @param b3 The state of button 3
   * @param b4 The state of button 4
   * @param b5 The state of button 5
   * @param b6 The state of button 6
   * @param x The x-coordinate of the mouse (relative to window)
   * @param y The y-coordinate of the mouse (relative to window)
   * @param xRoot The x-coordinate of the mouse (relative to the root window)
   * @param yRoot The y-coordinate of the mouse (relative to the root window)
   */
  EventMouseButton(unsigned int window, bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, 
      int x, int y, int xRoot, int yRoot)
      : PronEvent(EV_MOUSE_BUTTON, window) {
    this->init(b1, b2, b3, b4, b5, b6, x, y, xRoot, yRoot);
  }

  /**
   * Default constructor
   */
  EventMouseButton()
      : PronEvent(EV_MOUSE_BUTTON, 0) {
    this->init(false, false, false, false, false, false,
      0, 0, 0, 0);
  }

  bool b1; /**< State of button 1 (left button)   */
  bool b2; /**< State of button 2 (middle button) */
  bool b3; /**< State of button 3 (right button)  */
  bool b4; /**< State of button 4 */
  bool b5; /**< State of button 5 */
  bool b6; /**< State of button 6 */
  int x; /**< Mouse x position (relative to window) */
  int y; /**< Mouse y position (relative to window) */
  int xRoot; /**< Mouse absolute x position */
  int yRoot; /**< Mouse absolute y position */
};

/**
 * Base class for pron key events (EventKeyPressed or EventKeyReleased).
 */
struct EventKey : public PronEvent {
  /**
   * Constructor.
   * @param type The message type (EV_KEY_PRESSED, EV_KEY_RELEASED).
   * @param window The window the event is delivered to
   * @param subwindow The window the event has occured in
   * @param x The x-coordinate of the mouse (relative to window)
   * @param y The y-coordinate of the mouse (relative to window)
   * @param xRoot The x-coordinate of the mouse (relative to the root window)
   * @param yRoot The y-coordinate of the mouse (relative to the root window)
   * @param keysym The pron internal key code
   * @param modifiers The modifiers (CTRL, ALT, SHIFT...)
   * @param ch The ASCII character
   */
  EventKey(MessageType type, unsigned int window, unsigned int subwindow,
      int x, int y, int xRoot, int yRoot,
      unsigned int keysym, unsigned int modifiers, char ch)
      : PronEvent(type, window) {
    this->subwindow = subwindow;
    this->x = x;
    this->y = y;
    this->xRoot = xRoot;
    this->yRoot = yRoot;
    this->keysym = keysym;
    this->modifiers = modifiers;
    this->ch = ch;
  }

  unsigned int subwindow; /**< Window the event has occured in */
  int x; /**< Mouse x position (relative to window) */
  int y; /**< Mouse y position (relative to window) */
  int xRoot; /**< Mouse absolute x position */
  int yRoot; /**< Mouse absolute y position */
  unsigned int keysym; /**< Pron internal key code */
  unsigned int modifiers; /**< Modifiers (CTRL, ALT, SHIFT...) */
  char ch; /**< ASCII character */
};

/**
 * KeyPressed event.
 * Sent by the server when a key has been pressed.
 */
struct EventKeyPressed : public EventKey {
  /**
   * Constructor.
   * @param window The window the event is delivered to 
   * @param subwindow The window the event has occured in
   * @param x The x-coordinate of the mouse (relative to window)
   * @param y The y-coordinate of the mouse (relative to window)
   * @param xRoot The x-coordinate of the mouse (relative to the root window)
   * @param yRoot The y-coordinate of the mouse (relative to the root window)
   * @param keysym The pron internal key code
   * @param modifiers The modifiers (CTRL, ALT, SHIFT...)
   * @param ch The ASCII character
   */
  EventKeyPressed(unsigned int window, unsigned int subwindow, int x, int y,
      int xRoot, int yRoot, unsigned int keysym,
      unsigned int modifiers, char ch)
      : EventKey(EV_KEY_PRESSED, window, subwindow,
          x, y, xRoot, yRoot, keysym, modifiers, ch) {}
};

/**
 * KeyReleased event.
 * Sent by the server when a key has been released.
 */
struct EventKeyReleased : public EventKey {
  /**
   * Constructor.
   * @param window The window the event is delivered to
   * @param subwindow The window the event has occured in
   * @param x The x-coordinate of the mouse (relative to window)
   * @param y The y-coordinate of the mouse (relative to window)
   * @param xRoot The x-coordinate of the mouse (relative to the root window)
   * @param yRoot The y-coordinate of the mouse (relative to the root window)
   * @param keysym The pron internal key code
   * @param modifiers The modifiers (CTRL, ALT, SHIFT...)
   * @param ch The ASCII character
   */
  EventKeyReleased(unsigned int window, unsigned int subwindow, int x, int y,
      int xRoot, int yRoot, unsigned int keysym,
      unsigned int modifiers, char ch)
      : EventKey(EV_KEY_RELEASED, window, subwindow,
          x, y, xRoot, yRoot, keysym, modifiers, ch) {}
};

/**
 * DestroyWindow event.
 * Sent by the server when a window has been destroyed.
 */
struct EventDestroyWindow : public PronEvent {
  /**
   * Constructor.
   * @param window The id of the destroyed window
   */
  EventDestroyWindow(unsigned int window)
      : PronEvent(EV_DESTROY_WINDOW, window) {}
};

/**
 * ResizeWindow event.
 * Sent by the server when a window has been resized.
 */
struct EventResizeWindow : public PronEvent {
  /**
   * Constructor.
   * @param width The new width
   * @param height The new height
   */
  EventResizeWindow(int width, int height)
      : PronEvent(EV_RESIZE_WINDOW, window) {
    this->width  = width;
    this->height = height;
  }

  int width; /**< New width of the window */
  int height; /**< New height of the window */
};

} // namespace pron

#endif // __PRONPROTO_EVENTS_H__
