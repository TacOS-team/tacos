#ifndef __SCREEN_H__
#define __SCREEN_H__

/**
 * @file screen.h
 * Screen class definition.
 */

#include <font.h>
#include <clip_zone.h>
#include <gc.h>
#include <windowsTree.h>
#include <vesa_types.h>

#include <algorithm>
#include <vector>

using namespace std;

class Window;

/**
 * Screen.
 * @todo XXX public members bourrin
 */
class Screen {
 private:
  static Screen *instance; /**< Unique Screen instance (singleton) */
  vector<Font*> fonts; /**< Fonts loaded on the server */
  Window *clipWin; /**< Window for which the clipping zone is set */
  Window *grabWin; /**< Window grabbed */
  ClipZone *clipZone; /**< Rectangles where we are allowed to draw */
  bool clippingCheck; /**< Whether a check against the clipping zone is required or not before drawing */
  Window *mouseWin; /**< Window containing the mouse pointer */
  Window *focusWin; /**< Window having the focus */
  GC defaultGC; /**< Default graphics context for this screen */
  char *videoBuffer; /**< Video memory */
  GC *gc; /**< Current graphics context */
  int vesa_fd; /**< File descriptor used to communicate with the vesa driver */
  vector<Drawable*> drawables; /**< Drawables belonging to this screen @todo XXX ABR ? Rouge/noir ? B-Arbre ? */
  
  /**
   * Constructor.
   * @param width The width of the screen
   * @param height The height of the screen
   * @param bitsPerPixel The number of bits per pixel (8, 16, 24, 32)
   */
  Screen(int width, int height, int bitsPerPixel);

 public:
  int width; /**< Width of the screen */
  int height; /**< Height of the screen */
  int bitsPerPixel; /**< Bits per pixel (8, 16, 24, 32) */
  int bytesPerPixel; /**< Bytes per pixel (1, 2, 3, 4) */
  WindowsTree *tree; /**< Tree of windows belonging to this screen */

  /**
   * Returns the unique Screen instance (singleton). Creates it if necessary.
   * @param width The width of the screen
   * @param height The height of the screen
   * @param bitsPerPixel The number of bits per pixel (8, 16, 24, 32)
   * @return the unique Screen instance (singleton)
   */
  static Screen* getInstance(int width, int height, int bitsPerPixel) {
    if (Screen::instance == NULL) {
      Screen::instance = new Screen(width, height, bitsPerPixel);
    }
    return instance;
  }

  /**
   * Returns the unique Screen instance (singleton).
   * @return the unique Screen instance (singleton)
   */
  static Screen* getInstance() {
    return instance; 
  }

  /**
   * Gets the address of the pixel (x, y) in memory.
   * @param x The x-coordinate of the pixel
   * @param y The y-coordinate of the pixel
   */ 
  void* pixelAddr(int x, int y) { 
    return this->videoBuffer + (y * this->width + x) * this->bytesPerPixel;
  }

  /**
   * Returns true if we can draw at position (x, y).
   * @param x The x-coordinate of the point to check
   * @param y The y-coordinate of the point to check
   * @return true if we can draw at position (x, y)
   */
  bool isValid(int x, int y) {
    return (x >= 0 && x < this->width && y >= 0 && y < this->height &&
        (!this->clippingCheck || this->clipZone->contains(x, y)));
  }
 
  /**
   * Prepares the screen for drawing in the given drawable,
   * with the given graphics context.
   * @param d The drawable where to draw
   * @param gc The graphics context to draw with
   * @return success
   */
  bool prepareDrawing(Drawable *d, GC *gc = NULL);

  /**
   * Gets the queried drawable from the drawable list.
   * @param id The id of the queried drawable
   * @param drawableType The type of the queried drawable (window or pixmap)
   * @return the queried drawable (NULL if not found)
   */
  Drawable* getDrawable(unsigned int id, int drawableType);

  /**
   * Gets the queried drawable from the drawable list.
   * @param id The id of the queried drawable
   * @return the queried drawable (NULL if not found)
   */
  Drawable* getDrawable(unsigned int id);

  /**
   * Adds the given drawable to the drawable list.
   * @param d The drawable to add
   */
  void addDrawable(Drawable *d) {
    this->drawables.push_back(d);
  }

  /**
   * Removes the given drawable from the drawable list.
   * @param d The drawable to remove
   */
  void removeDrawable(Drawable *d) {
    this->drawables.erase(std::find(this->drawables.begin(), this->drawables.end(), d));
  }

  /**
   * Returns the area (list of rectangles) where we are allowed to draw
   * @return the area (list of rectangles) where we are allowed to draw
   */
  ClipZone* getClipZone() {
    return this->clipZone;
  }

  /**
   * Returns the window for which the clipping is currently set.
   * @return the window for which the clipping is currently set
   */
  Window* getClipWin() {
    return this->clipWin;
  }

  /**
   * Sets the clipping for the given window.
   * @param w The window for which to set the clipping
   */
  void setClipWin(Window *w);

  /**
   * Returns the window the mouse is currently in.
   * @return the window the mouse is currently in
   */
  Window* getMouseWin() {
    return this->mouseWin;
  }

  /**
   * Sets the window the mouse is currently in.
   * @param mouseWin The window the mouse is currently in
   */
  void setMouseWin(Window *mouseWin) {
    this->mouseWin = mouseWin;
  }

  /**
   * Returns the window currently grabbed.
   * @return the window currently grabbed
   */
  Window* getGrabWin() {
    return this->grabWin;
  }

  /**
   * Sets the window currently grabbed.
   * @param grabWin the window currently grabbed.
   */
  void setGrabWin(Window *grabWin) {
    this->grabWin = grabWin;
  }

  /**
   * Returns the window that currently has the focus.
   * @return the window that currently has the focus.
   */
  Window* getFocusWin() {
    return this->focusWin;
  }

  /**
   * Sets the window that currently has the focus.
   * @param focusWin The window that currently has the focus
   */
  void setFocusWin(Window *focusWin) {
    this->focusWin = focusWin;
  }

  /**
   * Returns the root of the windows tree.
   * @return the root of the windows tree
   */
  Window* getRoot() {
    return this->tree->getRoot();
  }
  
  /**
   * Destroys the given window.
   * @param w The window to destroy
   */
  void destroy(Window * w);

  /**
   * Sets a new parent to the given window.
   * @param child The window to reparent
   * @param newParent The new parent window
   */
  void reparent(Window *child, Window *newParent);

  /**
   * Returns the current graphics context.
   * @return the current graphics context
   */
  GC* getGC() {
    return this->gc;
  }

  /**
   * Sets the current graphics context.
   * @param gc The new graphics context to use
   */
  void setGC(GC *gc) {
    this->gc = gc;
  }

  /**
   * Gets the requested font from the font list.
   * @param id The font id
   * @return The requested font
   */
  Font* getFont(int id) {
    if (id < 0 || id >= (int) this->fonts.size()) {
      return this->fonts[0];
    }
    
    return this->fonts[id];
  }

  /**
   * Enables or disables the check against the clipping before drawing.
   */
  void setClippingCheck(bool clippingCheck) {
    this->clippingCheck = clippingCheck;
  }

  /**
   * Prints the clipping zone (for debugging purposes).
   */
  void printClipZone() {
    this->clipZone->print();
  }

  /**
   * Prints the windows tree (for debugging purposes).
   */
  void traceWindows();
};

#endif
