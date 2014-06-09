#ifndef __MOUSE_H__
#define __MOUSE_H__

/**
 * @file mouse.h
 * Mouse class definition.
 */

#include <mouse_types.h>
#include <pixmap.h>

#define PRON_MOUSE_POINTER_WIDTH  26 /**< Width of the pointer */
#define PRON_MOUSE_POINTER_WIDTH_SHIFT  8 /**< Shift on width of the pointer */
#define PRON_MOUSE_POINTER_HEIGHT 26 /**< Height of the pointer */
#define PRON_MOUSE_POINTER_HEIGHT_SHIFT 8 /**< Shift on height of the pointer */

/**
 * Mouse.
 */
class Mouse {
 private: 
  static Mouse *instance; /**< Unique Mouse instance (singleton) */
  static unsigned int defaultPointer; /**< Id of the pixmap containing the default pointer */

  int fd; /**< File descriptor used to communicate with the mouse driver */
  
  int mouseX; /**< Mouse absolute x position */
  int mouseY; /**< Mouse absolute y position */
  bool mouseB1; /**< State of button 1 (left button) */
  bool mouseB2; /**< State of button 2 (right button) */
  bool mouseB3; /**< State of button 3 */
  bool mouseB4; /**< State of button 4 */
  bool mouseB5; /**< State of button 5 */
  bool mouseB6; /**< State of button 6 */

  unsigned long lastMouseEvent; /**< Time of the last mouse event */
  int lastSentX; /**< Last sent mouse x position */
  int lastSentY; /**< Last sent mouse y position */

  bool pointerHidden; /**< True if the pointer has been hidden and the area under the pointer has been backuped */
  Pixmap *pointer; /**< Pointer pixmap */
  Pixmap *pointerBackup; /**< Backup of the area under the pointer */
  int pointerBackupX; /**< Top-left corner x-coordinate of the backuped area */
  int pointerBackupY; /**< Top-left corner y-coordinate of the backuped area */
  
  /**
   * Constructor.
   */
  Mouse();

 public:
  /**
   * Returns the unique Mouse instance (singleton). Creates it if necessary.
   * @return the unique Mouse instance (singleton)
   */
  static Mouse* getInstance() {
    if (Mouse::instance == NULL) {
      Mouse::instance = new Mouse();
    }

    return Mouse::instance;
  }

  /**
   * Reads events from the mouse and handles them.
   */
  void checkEvents();

  /**
   * Updates the window the mouse is currently in.
   */
  void updateMouseWin();

  /**
   * Updates the window that currently has the focus.
   */
  void updateFocusWin();

  /**
   * Returns whether a rectangular drawing area overlaps
   * the pointer area or not.
   * @param x1 The minimum x-coordinate of the area
   * @param y1 The minimum y-coordinate of the area
   * @param x2 The maximum x-coordinate of the area
   * @param y2 The maximum y-coordinate of the area
   * @return whether the area overlaps the pointer area or not.
   */
  bool overlapsPointer(int x1, int y1, int x2, int y2) {
    return !(x1 >= this->mouseX - PRON_MOUSE_POINTER_WIDTH_SHIFT + PRON_MOUSE_POINTER_WIDTH ||
        y1 >= this->mouseY - PRON_MOUSE_POINTER_HEIGHT_SHIFT + PRON_MOUSE_POINTER_HEIGHT ||
        x2 < this->mouseX - PRON_MOUSE_POINTER_WIDTH_SHIFT ||
        y2 < this->mouseY - PRON_MOUSE_POINTER_HEIGHT_SHIFT);
  }

  /**
   * Hides the pointer by restoring the area under the mouse pointer.
   */
  void hidePointer();

  /**
   * Shows the mouse pointer. The area under the pointer is backuped.
   */
  void showPointer();

  /**
   * Handles mouse motion events.
   */
  void handleMotion(mousestate_t *state);

  /**
   * Handles mouse button events.
   */
  void handleButton(mousestate_t *state);

  /**
   * Returns the x position of the mouse.
   * @return the x position of the mouse
   */
  int getMouseX() {
    return this->mouseX;
  }

  /**
   * Returns the y position of the mouse.
   * @return the y position of the mouse
   */
  int getMouseY() {
    return this->mouseY;
  }

  /**
   * Returns the state of button 1 (left button).
   * @return the state of button 1 (left button)
   */
  bool getMouseB1() {
    return this->mouseB1;
  }

  /**
   * Returns the state of button 2 (right button).
   * @return the state of button 2 (right button)
   */
  bool getMouseB2() {
    return this->mouseB2;
  }

  /**
   * Returns the state of button 3.
   * @return the state of button 3
   */
  bool getMouseB3() {
    return this->mouseB3;
  }

  /**
   * Returns the state of button 4.
   * @return the state of button 4
   */
  bool getMouseB4() {
    return this->mouseB4;
  }

  /**
   * Returns the state of button 5.
   * @return the state of button 5
   */
  bool getMouseB5() {
    return this->mouseB5;
  }

  /**
   * Returns the state of button 6.
   * @return the state of button 6
   */
  bool getMouseB6() {
    return this->mouseB6;
  }
};

#endif
