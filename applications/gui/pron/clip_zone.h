#ifndef __CLIP_ZONE_H__
#define __CLIP_ZONE_H__

/**
 * @file clip_zone.h
 * ClipZone class definition.
 */

#include <clip_rect.h>
#include <vector>

class Window;

using namespace std;

/**
 * States of an area regarding the clipping zone
 */
enum ClipState {
  INVISIBLE, /**< Totally invisible */
  PARTIAL, /**< Possibly partially visible */
  VISIBLE, /**< Totally visible */
};

/**
 * Clipping zone.
 * Defines a set of rectangles we can draw into.
 * Any point that is not in the clipping zone won't be drawn.
 */
class ClipZone {
public:
  /**
   * Constructor.
   * Creates an initial clipping zone containing the given rectangle.
   * @param x The x-coordinate of the top-left corner of the rectangle
   * @param y The y-coordinate of the top-left corner of the rectangle
   * @param width The width of the rectangle
   * @param height The height of the rectangle
   */
  ClipZone(int x, int y, int width, int height);

  /**
   * Constructor.
   * Creates a clipping zone containing the visible areas of the given window.
   * @param w The window for which to set the clipping
   */
  ClipZone(Window *w);

  /**
   * Returns true if the clipping zone contains (x, y).
   * @param x The x-coordinate of the point
   * @param y The y-coordinate of the point
   * @return true if the clipping zone contains (x, y)
   */
  bool contains(int x, int y);

  /**
   * Checks a rectangular area against the clipping zone.
   * The area can be totally visible, totally invisible or
   * possibly partially visible.
   * @param x1 The x-coordinate of the top-left corner of the area
   * @param y1 The y-coordinate of the top-left corner of the area
   * @param x2 The x-coordinate of the bottom-right corner of the area
   * @param y2 The y-coordinate of the bottom-right corner of the area
   * @return the state of the area (fully visible, fully invisible
   * or partially visible)
   */
  ClipState checkArea(int x1, int y1, int x2, int y2);

  /**
   * Prints the clipping zone (for debugging purposes).
   */
  void print();

  /**
   * Returns the set of rectangles that composes the clipping zone.
   * @return the set of rectangles that composes the clipping zone.
   */
  vector<ClipRect*>& getClipRects() {
    return this->clipRects;
  }

private:
  vector<ClipRect*> clipRects; /**< Set of rectangles composing the clipping zone */
  ClipRect cache; /**< Cache rectangle for quicker checks */
  bool cacheRes; /**< Cached result (whether points are plottable or not in the cache rectangle) */
};

#endif
