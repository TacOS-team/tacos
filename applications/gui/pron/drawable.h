#ifndef __DRAWABLE_H__
#define __DRAWABLE_H__

/**
 * @file drawable.h
 * Drawable class definition.
 */

#include <proto/bits/image.h>
#include <proto/bits/text.h>
#include <clip_zone.h>

using namespace std;
using namespace pron;

class Window;
class Screen;
class Client;

/**
 * Drawable types.
 */
enum DrawableType {
  D_WINDOW, /**< Window (displayed on screen) */
  D_PIXMAP, /**< Pixmap (stored in memory) */
};

/**
 * Drawable (entity the user can draw into).
 */
class Drawable {
 private:
  unsigned int type; /**< Drawable type (D_WINDOW or D_PIXMAP) */
  Screen *screen; /**< Screen the drawable belongs to */
  unsigned int id; /**< Drawable id */
  Client *creator; /**< Client who has created the drawable */
  int width; /**< Drawable width */
  int height; /**< Drawable height */

 protected: 
  /**
   * Constructor.
   * @param type The type of the drawable (D_WINDOW or D_PIXMAP)
   * @param screen The screen the drawable belongs to
   * @param id The id of the drawable
   * @param creator The client who has created the drawable
   * @param width The width of the drawable
   * @param height The height of the drawable
   */
  Drawable(int type, Screen *screen, int id, Client *creator, int width, int height);

  /**
   * Destructor.
   */
  ~Drawable(); 

  /**
   * Reduces the given rectangle so that it fits into the drawable.
   * @param x The top-left x-coordinate of the rectangle, relative to this drawable
   * @param y The top-left y-coordinate of the rectangle, relative to this drawable
   * @param width The width of the rectangle
   * @param height The height of the rectangle
   */
  void reduce(int &x, int &y, int &width, int &height);
 
  /**
   * Gets the address of the pixel (x, y) in memory.
   * @param x The x-coordinate of the pixel
   * @param y The y-coordinate of the pixel
   */ 
  virtual void* pixelAddr(int x, int y) = 0;

  /**
   * Returns true if we can draw at position (x, y).
   * @param x The x-coordinate of the point to check
   * @param y The y-coordinate of the point to check
   * @return true if we can draw at position (x, y)
   */
  virtual bool isValid(int x, int y) = 0;

  /**
   * Callback function called before drawing.
   * @param x1 The minimum x-coordinate of the area where to draw
   * @param y1 The minimum y-coordinate of the area where to fraw
   * @param x2 The maximum x-coordinate of the area where to draw
   * @param y2 The maximum y-coordinate of the area where to draw
   * @return the state of the area (fully visible, fully invisible
   * or partially visible)
   */
  virtual ClipState beforeDrawing(int x1, int y1, int x2, int y2);

  /**
   * Callback function called before drawing.
   * @param x1 The minimum x-coordinate of the drawn area
   * @param y1 The minimum y-coordinate of the drawn area
   * @param x2 The maximum x-coordinate of the drawn area
   * @param y2 The maximum y-coordinate of the drawn area
   */
  virtual void afterDrawing(int x1, int y1, int x2, int y2);

  /**
   * Draws an horizontal line between (x, y) and (x + width, y), using
   * the current graphics context.
   * @param x The x-coordinate of the first point of the line
   * @param y The y-coordinate of the first point of the line
   * @param width The width of the line
   */
  void drawHorizLine(int x, int y, int width);
  
  /**
   * Draws a vertical line between (x, y) and (x, y + height), using
   * the current graphics context.
   * @param x The x-coordinate of the first point of the line
   * @param y The y-coordinate of the first point of the line
   * @param height The height of the line
   */
  void drawVertLine(int x, int y, int height);

 public: 
  /**
   * Returns the type of the drawable (window or pixmap).
   * @return the type of the drawable
   */
  int getType() {
    return this->type;
  }

  /**
   * Returns the screen the drawable belongs to.
   * @return the screen the drawable belongs to
   */
  Screen* getScreen() {
    return this->screen;
  }

  /**
   * Returns the id of the drawable.
   * @return the id of the drawable
   */
  unsigned int getId() const {
    return this->id;
  }

  /**
   * Returns the creator of the drawable.
   * @return the creator of the drawable
   */
  Client* getCreator() {
    return this->creator;
  }

  /**
   * Returns the width of the drawable.
   * @return the width of the drawable
   */
  int getWidth() {
    return this->width;
  }

  /**
   * Sets the width of the drawable.
   * @param width The new width of the drawable
   */
  void setWidth(int width) {
    this->width = width;
  }

  /**
   * Returns the height of the drawable.
   * @return the height of the drawable
   */
  int getHeight() {
    return this->height;
  }

  /**
   * Sets the height of the drawable.
   * @param height The new height of the drawable
   */
  void setHeight(int height) {
    this->height = height;
  }

  /**
   * Draws a point at (x, y), using the current graphics context.
   * @param x The x-coordinate of the point
   * @param y The y-coordinate of the point
   */
  void drawPoint(int x, int y);

  /**
   * Draws a line between (x1, y1) and (x2, y2), using the current
   * graphics context.
   * @param x1 The x-coordinate of the first point to join
   * @param y1 The y-coordinate of the first point to join
   * @param x2 The x-coordinate of the second point to join
   * @param y2 The y-coordinate of the second point to join
   */
  void drawLine(int x1, int y1, int x2, int y2);

  /**
   * Draws a rectangle at (x,y) width height sized, using the current
   * graphics context.
   * @param x The x-coordinate of the top-left corner of the rectangle
   * @param y The y-coordinate of the top-left corner of the rectangle
   * @param width The width of the rectangle
   * @param height The height of the rectangle
   */
  void drawRect(int x, int y, int width, int height);

  /**
   * Draws an ellipse with given center (x0,y0) and radiuses a and b,
   * using the current graphics context.
   * @param x0 The x-coordinate of the center of the ellipse
   * @param y0 The y-coordinate of the center of the ellipse
   * @param a The longest radius of the ellipse
   * @param b The shortest radius of the ellipse
   */
  void drawEllipse(int x0, int y0, int a, int b);

  /**
   * Draws a filled rectangle at (x,y) width height sized, using the current
   * graphics context.
   * @param x The x-coordinate of the top-left corner of the rectangle
   * @param y The y-coordinate of the top-left corner of the rectangle
   * @param width The width of the rectangle
   * @param height The height of the rectangle
   */
  void fillRectangle(int x, int y, int width, int height);
  
  /**
   * Draws a filled ellipse with given center (x0,y0) and radiuses a and b,
   * using the current graphics context.
   * @param x0 The x-coordinate of the center of the ellipse
   * @param y0 The y-coordinate of the center of the ellipse
   * @param a The longest radius of the ellipse
   * @param b The shortest radius of the ellipse
   */
  void fillEllipse(int x0, int y0, int a, int b);

  /**
   * Puts an image into the drawable.
   * @param image The PronImage to put
   * @param x The destination top-left corner x-coordinate
   * @param y The destination top-left corner y-coordinate
   */
  void putImage(PronImage *image, int x, int y);
  
  /**
   * Puts a segment of an image into the drawable.
   * @param image The PronImage to put
   * @param x The destination top-left corner x-coordinate
   * @param y The destination top-left corner y-coordinate
   * @param offset The offset corresponding to the segment
   * @param size The size of the image segment
   */
  void putImage(PronImage *image, int x, int y, int offset, int size);

  /**
   * Copies a Drawable area into this Drawable.
   * @param dstX The top-left x-coordinate of the destination area
   * @param dstY The top-left y-coordinate of the destination area
   * @param d The source Drawable
   * @param srcX The top-left x-coordinate of the source area
   * @param srcY The top-left y-coordinate of the source area
   * @param width The width of the area
   * @param height The height of the area
   */
  void copyArea(int dstX, int dstY, Drawable *d, int srcX, int srcY, int width, int height);

  /**
   * Draws the given text.
   * @param x The x-coordinate of the origin of the first character
   * @param y The y-coordinate of the origin of the first character
   * @param text The text to draw
   * @param length The length of the text
   * @param hpos The horizontal position of the text
   * @param vpos The vertical position of the text
   */
  void drawText(int x, int y, const char *text, int length,
      HPosition hpos = LEFT, VPosition vpos = MIDDLE);

  /**
   * Gets the pixel at (x, y).
   * @param x The x-coordinate of the pixel to get
   * @param y The y-coordinate of the pixel to get
   * @return the value (color) of the pixel at (x, y)
   */ 
  int getPixel(int x, int y);

  /**
   * Sets the pixel at (x, y).
   * @param x The x-coordinate of the pixel to set
   * @param y The y-coordinate of the pixel to set
   * @param pixel The value (color) to set
   */
  void setPixel(int x, int y, int pixel);

};

#endif
