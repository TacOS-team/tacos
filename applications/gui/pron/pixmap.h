#ifndef __PIXMAP_H__
#define __PIXMAP_H__

/**
 * @file pixmap.h
 * Pixmap class definition.
 */

#include <drawable.h>
#include <pixmap.h>

class Drawable;
class Screen;

/**
 * Pixmap (drawable stored in memory).
 */
class Pixmap : public Drawable {
 public:
  /**
   * Constructor.
   * @param screen The screen the drawable belongs to
   * @param id The id of the drawable
   * @param creator The client who has created the drawable
   * @param width The width of the drawable
   * @param height The height of the drawable
   * @param depth The depth of the pixmap
   */
  Pixmap(Screen *screen, int id, Client *creator, int width, int height, int depth);

  /**
   * Destructor.
   */
  ~Pixmap();

  /**
   * Gets the address of the pixel (x, y) in memory.
   * @param x The x-coordinate of the pixel
   * @param y The y-coordinate of the pixel
   */ 
  void* pixelAddr(int x, int y);

  /**
   * Returns true if we can draw at position (x, y).
   * @param x The x-coordinate of the point to check
   * @param y The y-coordinate of the point to check
   * @return true if we can draw at position (x, y)
   */
  bool isValid(int x, int y);
  
 private:
  char *buf; /**< Pixmap buffer */
  int depth; /**< Pixmap depth */
};

#endif
