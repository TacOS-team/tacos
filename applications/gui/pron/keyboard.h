#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

/**
 * @file keyboard.h
 * Keyboard class definition.
 */

/**
 * Keyboard.
 */
class Keyboard {
 public:
  /**
   * Constructor.
   */
  Keyboard();

  /**
   * Reads events from the keyboard and handles them.
   */
  void checkEvents();

 private:
  int fd; /**< File descriptor used to communicate with the keyboard driver */
};

#endif
