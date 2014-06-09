#ifndef __PRONPROTO_REQUESTS_H__
#define __PRONPROTO_REQUESTS_H__

#include <string.h>

#include <proto/base.h>
#include <proto/bits/attributes.h>
#include <proto/bits/gc.h>
#include <proto/bits/text.h>

namespace pron {

/**
 * Base class representing a pron request message.
 */
struct PronRequest : public PronMessage {
  /**
   * Constructor.
   * @param type The message type
   */
  PronRequest(MessageType type)
      : PronMessage(type) {
  }
};

/**
 * Hello request.
 * Sent by a client as the first message of the handshake.
 */
struct RqHello : public PronRequest {
  /**
   * Constructor.
   * @param protoVersion The protocol version used
   */
  RqHello(int protoVersion)
      : PronRequest(RQ_HELLO) {
    this->protoVersion = protoVersion;
  }

  int protoVersion; /**< Protocol version used */
};

/**
 * Goodbye request.
 * Sent by a client to disconnect from pron
 */
struct RqGoodbye : public PronRequest {
  /**
   * Constructor.
   */
  RqGoodbye()
      : PronRequest(RQ_GOODBYE) {}
};

/**
 * CreateWindow request.
 * Sent by a client to create a new window.
 */
struct RqCreateWindow : public PronRequest {
  /**
   * Constructor.
   * @param id The id of the window to create
   * @param parent The id of the the parent window
   * @param x The x-coordinate of the top-left corner of the window
   * @param y The y-coordinate of the top-left corner of the window
   * @param width The width of the window
   * @param height The height of the window
   */
  RqCreateWindow(unsigned int id, unsigned int parent, int x, int y, int width, int height)
      : PronRequest(RQ_CREATE_WINDOW) {
    this->id = id;
    this->parent = parent;
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
  }

  unsigned int id; /**< Id of the window to create */
  int parent; /**< Id of the parent window */
  int x; /**< X-coordinate of the left-top corner of the window */
  int y; /**< Y-coordinate of the left-top corner of the window */
  int width; /**< Width of the window */
  int height; /**< Height of the window */
};

/**
 * CreateWindowExtended request.
 * Sent by a client to create a new window with extended attributes.
 */
struct RqCreateWindowExtended : public PronRequest {
  /**
   * Constructor.
   * @param id The id of the window to create
   * @param parent The id of the the parent window
   * @param attr The attributes of the new window
   * @param mask The mask that specifies the attributes to be set
   */
  RqCreateWindowExtended(unsigned int id, unsigned int parent,
      const PronWindowAttributes &attr, unsigned int mask)
      : PronRequest(RQ_CREATE_WINDOW_EXT) {
    this->id = id;
    this->parent = parent;
    this->attr = attr;
    this->mask = mask;
  }

  unsigned int id; /**< Id of the window to create */
  int parent; /**< Id of the parent window */
  PronWindowAttributes attr; /**< Attributes of the new window */
  unsigned int mask; /**< Specifies which attributes have to be set */
};

/**
 * ClearWindow request.
 * Sent by a client to clear the contents of a window.
 */
struct RqClearWindow : public PronRequest {
  /**
   * Constructor.
   * @param window The id of the window to clear
   */
  RqClearWindow(unsigned int window)
      : PronRequest(RQ_CLEAR_WINDOW) {
    this->window = window;
  }

  unsigned int window; /**< Id of the window to clear */
};

/**
 * MapWindow request.
 * Sent by a client to show a window on the screen.
 */
struct RqMapWindow : public PronRequest {
  /**
   * Constructor.
   * @param window The id of the window to map
   */
  RqMapWindow(unsigned int window)
      : PronRequest(RQ_MAP_WINDOW) {
    this->window = window;
  }

  unsigned int window; /**< Id of the window to map */
};

/**
 * UnmapWindow request.
 * Sent by a client to hide a window.
 */
struct RqUnmapWindow : public PronRequest {
  /**
   * Constructor.
   * @param window The id of the window to unmap
   */
  RqUnmapWindow(int window)
      : PronRequest(RQ_UNMAP_WINDOW) {
    this->window = window;
  }

  int window; /**< Id of the window to unmap */
};

/**
 * RaiseWindow request.
 * Sent by a client to raise a window on the screen.
 */
struct RqRaiseWindow : public PronRequest {
  /**
   * Constructor.
   * @param window The id of the window to raise
   */
  RqRaiseWindow(unsigned int window)
      : PronRequest(RQ_RAISE_WINDOW) {
    this->window = window;
  }

  unsigned int window; /**< Id of the window to raise */
};

/**
 * CreateGC request.
 * Sent by a client to create a new graphics context.
 */
struct RqCreateGC : public PronRequest {
  /**
   * Constructor.
   * @param id The id of the graphics context to create
   * @param values The values to set in the new GC
   * @param mask The mask that specifies the values to be set
   */
  RqCreateGC(unsigned int id, const PronGCValues &values, unsigned int mask)
      : PronRequest(RQ_CREATE_GC) {
    this->id = id;
    this->values = values;
    this->mask = mask;
  }

  unsigned int id; /**< Id of the graphics context to create */
  PronGCValues values; /**< GC values to set */
  unsigned int mask; /**< Specifies which values have to be set */
};

/**
 * GetGCValues request.
 * Sent by a client to get the values of the specified graphics context.
 */
struct RqGetGCValues : public PronRequest {
  /**
   * Constructor.
   * @param gc The graphics context whose values to get
   */
  RqGetGCValues(unsigned int gc)
      : PronRequest(RQ_GET_GC_VALUES) {
    this->gc = gc;
  }

  unsigned int gc; /**< GC whose values to get */
};

/**
 * ChangeGC request.
 * Sent by a client to change the values of a specified graphics context.
 */
struct RqChangeGC : public PronRequest {
  /**
   * Constructor.
   * @param gc The graphics context whose values to set
   * @param values The values to set
   * @param mask The mask that specifies the values to be set
   */
  RqChangeGC(unsigned int gc, const PronGCValues &values, unsigned int mask)
      : PronRequest(RQ_CHANGE_GC) {
    this->gc = gc;
    this->values = values;
    this->mask = mask;
  }

  unsigned int gc; /**< Id of the graphics context to change */
  PronGCValues values; /**< The values to set */
  unsigned int mask; /**< Specifies which values have to be set */
};

/**
 * FreeGC request.
 * Sent by a client to destroy a graphics context.
 */
struct RqFreeGC : public PronRequest {
  /**
   * Constructor.
   * @param gc The id of the graphics context to destroy
   */
  RqFreeGC(unsigned int gc)
      : PronRequest(RQ_FREE_GC) {
    this->gc = gc;
  }

  unsigned int gc; /**< Id of the graphics context to destroy */
};

/**
 * DrawPoint request.
 * Sent by a client to draw a point.
 */
struct RqDrawPoint : public PronRequest {
  /**
   * Constructor.
   * @param gc The graphics context to use
   * @param drawable The drawable in which to draw the line
   * @param x The x-coordinate of the point
   * @param y The y-coordinate of the point
   */
  RqDrawPoint(unsigned int gc, unsigned int drawable, int x, int y)
      : PronRequest(RQ_DRAW_POINT) {
    this->gc = gc;
    this->drawable = drawable;
    this->x = x;
    this->y = y;
  }

  unsigned int gc; /**< Id of the graphics context to use */
  unsigned int drawable; /**< Id of the drawable in which to draw */
  int x; /**< X-coordinate of the point */
  int y; /**< Y-coordinate of the point */
};

/**
 * DrawLine request.
 * Sent by a client to draw a line between two points.
 */
struct RqDrawLine : public PronRequest {
  /**
   * Constructor.
   * @param gc The graphics context to use
   * @param drawable The drawable in which to draw the line
   * @param x1 The x-coordinate of the first point to join
   * @param y1 The y-coordinate of the first point to join
   * @param x2 The x-coordinate of the second point to join
   * @param y2 The y-coordinate of the second point to join
   */
  RqDrawLine(unsigned int gc, unsigned int drawable, int x1, int y1, int x2, int y2)
      : PronRequest(RQ_DRAW_LINE) {
    this->gc = gc;
    this->drawable = drawable;
    this->x1 = x1;
    this->y1 = y1;
    this->x2 = x2;
    this->y2 = y2;
  }

  unsigned int gc; /**< Id of the graphics context to use */
  unsigned int drawable; /**< Id of the drawable in which to draw */
  int x1; /**< X-coordinate of the first point to join */
  int y1; /**< Y-coordinate of the first point to join */
  int x2; /**< X-coordinate of the second point to join */
  int y2; /**< Y-coordinate of the second point to join */
};

/**
 * DrawRect request.
 * Sent by a client to draw a rectangle to (x,y) width height sized.
 */
struct RqDrawRect : public PronRequest {
  /**
   * Constructor.
   * @param gc The graphics context to use
   * @param drawable The drawable in which to draw the rectangle
   * @param x The x-coordinate of the top-left corner of the rectangle
   * @param y The y-coordinate of the top-left corner of the rectangle
   * @param width of the rectangle
   * @param height of the rectangle
   */
  RqDrawRect(unsigned int gc, unsigned int drawable, int x, int y, int width, int height)
      : PronRequest(RQ_DRAW_RECT) {
    this->gc = gc;
    this->drawable = drawable;
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
  }

  unsigned int gc; /**< Id of the graphics context to use */
  unsigned int drawable; /**< Id of the drawable in which to draw */
  int x; /**< X-coordinate of the top-left corner of the rectangle */
  int y; /**< Y-coordinate of the top-left corner of the rectangle */
  int width; /**< Width of the rectangle */
  int height; /**< Height of the rectangle */
};

/**
 * FillRectangle request.
 * Sent by a client to fill a rectangle to (x,y) width height sized.
 */
struct RqFillRectangle : public PronRequest {
  /**
   * Constructor.
   * @param gc The graphics context to use
   * @param drawable The drawable in which to draw the rectangle
   * @param x The x-coordinate of the top-left corner of the rectangle
   * @param y The y-coordinate of the top-left corner of the rectangle
   * @param width of the rectangle
   * @param height of the rectangle
   */
  RqFillRectangle(unsigned int gc, unsigned int drawable, int x, int y, int width, int height)
      : PronRequest(RQ_FILL_RECTANGLE) {
    this->gc = gc;
    this->drawable = drawable;
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
  }

  unsigned int gc; /**< Id of the graphics context to use */
  unsigned int drawable; /**< Id of the drawable in which to draw */
  int x; /**< X-coordinate of the top-left corner of the rectangle */
  int y; /**< Y-coordinate of the top-left corner of the rectangle */
  int width; /**< Width of the rectangle */
  int height; /**< Height of the rectangle */
};

/**
 * DrawEllipse request.
 * Sent by a client to draw an ellipse with given center (x0,y0)
 * and radiuses a and b.
 */
struct RqDrawEllipse : public PronRequest {
  /**
   * Constructor.
   * @param gc The graphics context to use
   * @param drawable The drawable in which to draw the ellipse
   * @param x0 The x-coordinate of the center of the ellipse
   * @param y0 The y-coordinate of the center of the ellipse
   * @param a The longest radius of the ellipse
   * @param b The shortest radius of the ellipse
   */
  RqDrawEllipse(unsigned int gc, unsigned int drawable, int x0, int y0,
      int a, int b)
      : PronRequest(RQ_DRAW_ELLIPSE) {
    this->gc = gc;
    this->drawable = drawable;
    this->x0 = x0;
    this->y0 = y0;
    this->a = a;
    this->b = b;
  }

  unsigned int gc; /**< Id of the graphics context to use */
  unsigned int drawable; /**< Id of the drawable in which to draw */
  int x0; /**< X-coordinate of the center of the ellipse */
  int y0; /**< X-coordinate of the center of the ellipse */
  int a; /**< Longest radius of the ellipse */
  int b; /**< Shortest radius of the ellipse */
};

/**
 * FillEllipse request.
 * Sent by a client to draw a filled ellipse with given center (x0,y0)
 * and radiuses a and b.
 */
struct RqFillEllipse : public PronRequest {
  /**
   * Constructor.
   * @param gc The graphics context to use
   * @param drawable The drawable in which to draw the ellipse
   * @param x0 The x-coordinate of the center of the ellipse
   * @param y0 The y-coordinate of the center of the ellipse
   * @param a The longest radius of the ellipse
   * @param b The shortest radius of the ellipse
   */
  RqFillEllipse(unsigned int gc, unsigned int drawable, int x0, int y0,
      int a, int b)
      : PronRequest(RQ_FILL_ELLIPSE) {
    this->gc = gc;
    this->drawable = drawable;
    this->x0 = x0;
    this->y0 = y0;
    this->a = a;
    this->b = b;
  }

  unsigned int gc; /**< Id of the graphics context to use */
  unsigned int drawable; /**< Id of the drawable in which to draw */
  int x0; /**< X-coordinate of the center of the ellipse */
  int y0; /**< X-coordinate of the center of the ellipse */
  int a; /**< Longest radius of the ellipse */
  int b; /**< Shortest radius of the ellipse */
};

/**
 * GetWindowAttributes request.
 * Sent by a client to get the attributes of a window.
 */
struct RqGetWindowAttributes : public PronRequest {
  /**
   * Constructor.
   * @param window The window whose attributes to get
   */
  RqGetWindowAttributes(unsigned int window)
      : PronRequest(RQ_GET_WINDOW_ATTRIBUTES) {
    this->window = window;
  }

  unsigned int window; /**< Id of the window whose attributes to get */
};

/**
 * SetWindowAttributes request.
 * Sent by a client to set the attributes of a window.
 */
struct RqSetWindowAttributes : public PronRequest {
  /**
   * Constructor.
   * @param window The window whose attributes to set
   * @param newAttr The new attributes to set
   * @param mask The mask that specifies the attributes to be set
   */
  RqSetWindowAttributes(unsigned int window, const PronWindowAttributes &newAttr, unsigned int mask)
      : PronRequest(RQ_SET_WINDOW_ATTRIBUTES) {
    this->window = window;
    this->newAttr = newAttr;
    this->mask = mask;
  }

  unsigned int window; /**< Id of the window whose attributes to set */
  PronWindowAttributes newAttr; /**< New attributes to set */
  unsigned int mask; /**< Specifies which attributes have to be set */
};

/**
 * SelectInput request.
 * Sent by a client to subscribe to the events associated with
 * the given event mask.
 */
struct RqSelectInput : public PronRequest {
  /**
   * Constructor.
   * @param window The window whose events to subscribe
   * @param eventMask The event mask
   */
  RqSelectInput(unsigned int window, unsigned int eventMask)
      : PronRequest(RQ_SELECT_INPUT) {
    this->window = window;
    this->eventMask = eventMask;
  }

  unsigned int window; /**< Id of the window whose events to subscribe */
  unsigned int eventMask; /**< Specifies the events to subscribe */
};

/**
 * DontPropagate request.
 * Sent by a client to tell pron not to propagate the events associated with
 * the given event mask.
 */
struct RqDontPropagate : public PronRequest {
  /**
   * Constructor.
   * @param window The window whose events not to propagate
   * @param eventMask The event mask
   */
  RqDontPropagate(unsigned int window, unsigned int eventMask)
      : PronRequest(RQ_DONT_PROPAGATE) {
    this->window = window;
    this->eventMask = eventMask;
  }

  unsigned int window; /**< Id of the window whose events not to propagate */
  unsigned int eventMask; /**< Specifies the events not to propagate */
};

/**
 * Reparent request.
 * Sent by a client to set a new parent to a window.
 */
struct RqReparent : public PronRequest {
  /**
   * Constructor.
   * @param window The window to reparent
   * @param newParent The id of the new parent window
   */
  RqReparent(unsigned int window, unsigned int newParent)
      : PronRequest(RQ_REPARENT) {
    this->window    = window;
    this->newParent = newParent;
  }

  unsigned int window; /**< Id of the window to reparent */
  unsigned int newParent; /**< Id of the new parent Window */
};

/**
 * DestroyWindow request.
 * Sent by a client to close a window.
 */
struct RqDestroyWindow : public PronRequest {
  /**
   * Constructor.
   * @param window The window to destroy
   */
  RqDestroyWindow(unsigned int window)
      : PronRequest(RQ_DESTROY_WINDOW) {
    this->window    = window;
  }

  unsigned int window; /**< Id of the window to destroy */
};

/**
 * MoveWindow request.
 * Sent by a client to move a window of x pixels on the x-axis
 * and y pixels on the y-axis.
 */
struct RqMoveWindow : public PronRequest {
  /**
   * Constructor.
   * @param window The window to move
   * @param x The relative move on the x-axis
   * @param y The relative move on the y-axis
   */
  RqMoveWindow(unsigned int window, int x, int y)
      : PronRequest(RQ_MOVE_WINDOW) {
    this->window    = window;
    this->x = x;
    this->y = y;
  }

  unsigned int window; /**< Id of the window to move */
  int x; /**< Relative move on the x-axis */
  int y; /**< Relative move on the y-axis */
};

/**
 * MoveWindowTo request.
 * Sent by a client to move a window at an absolute position.
 */
struct RqMoveWindowTo : public PronRequest {
  /**
   * Constructor.
   * @param window The window to move
   * @param x The new x-coordinate
   * @param y The new y-coordinate
   */
  RqMoveWindowTo(unsigned int window, int x, int y)
      : PronRequest(RQ_MOVE_WINDOW_TO) {
    this->window    = window;
    this->x = x;
    this->y = y;
  }

  unsigned int window; /**< Id of the window to move */
  int x; /**< New x-coordinate */
  int y; /**< New y-coordinate */
};

/**
 * PutImage request.
 * Sent by a client to put an image into a window or a pixmap.
 */
struct RqPutImage : public PronRequest {
  /**
   * Constructor.
   * @param drawable The drawable in which to put the image
   * @param gc The graphics context to use
   * @param x The destination top-left corner x-coordinate
   * @param y The destination top-left corner y-coordinate
   * @param width The width of the image
   * @param height The height of the image
   * @param format The format of the image (XYBitmap, XYPixmap, ZPixmap)
   * @param depth The depth of the image (8, 16 or 24 bits)
   * @param bytesPerPixel The bytes per pixel of the image (1, 2 or 3 bpp)
   * @param offset The offset of the current image fragment (in bytes)
   * @param size The size of the current image fragment (in bytes)
   */
  RqPutImage(unsigned int drawable, unsigned int gc, int x, int y,
      int width, int height, int format, int depth, int bytesPerPixel,
      int offset, int size)
      : PronRequest(RQ_PUT_IMAGE) {
    this->drawable = drawable;
    this->gc = gc;
    this->width = width; 
    this->height = height;
    this->x = x;
    this->y = y;
    this->depth = depth;
    this->format = format;
    this->bytesPerPixel = bytesPerPixel;
    this->offset = offset;
    this->size = size;
  }

  unsigned int drawable; /**< Id of the drawable in which to put the image */
  unsigned int gc; /**< Id of the graphics context to use */
  int width; /**< Width of the image */
  int height; /**< Height of the image */
  int x; /**< Destination top-left corner x-coordinate */
  int y; /**< Destination top-left corner y-coordinate */
  int depth; /**< Depth of the image (8, 16, 24 bits) */
  int format; /**< Format of the image (XYBitmap, XYPixmap, ZPixmap) */
  int bytesPerPixel; /**< Bytes per pixel of the image (1, 2, 3 bpp) */
  int offset; /**< Byte offset of the current part of the image */
  int size; /**< Byte size of the current part of the image */
};

/**
 * ResizeWindow request.
 * Sent by a client to resize a window.
 */
struct RqResizeWindow : public PronRequest {
  /**
   * Constructor.
   * @param window The window to resize
   * @param width The new width
   * @param height The new height
   */
  RqResizeWindow(unsigned int window, int width, int height)
      : PronRequest(RQ_RESIZE_WINDOW) {
    this->window = window;
    this->width  = width;
    this->height = height;
  }

  unsigned int window; /**< Id of the window to resize */
  int width; /**< New width to set */
  int height; /**< New height to set */
};

/**
 * CreatePixmap request.
 * Sent by a client to create a pixmap.
 */
struct RqCreatePixmap : public PronRequest {
  /**
   * Constructor.
   * @param id The id of the pixmap to create
   * @param width The width of the pixmap
   * @param height The height of the pixmap
   * @param depth The depth of the pixmap
   */
  RqCreatePixmap(unsigned int id, int width, int height, int depth)
      : PronRequest(RQ_CREATE_PIXMAP) {
    this->id = id;
    this->width = width;
    this->height = height;
    this->depth = depth;
  }

  unsigned int id; /**< Id of the pixmap to create */
  int width; /**< Width of the pixmap */
  int height; /**< Height of the pixmap */
  int depth; /**< Depth of the pixmap */
};

/**
 * FreePixmap request.
 * Sent by a client to destroy a pixmap.
 */
struct RqFreePixmap : public PronRequest {
  /**
   * Constructor.
   * @param pixmap The pixmap to free
   */
  RqFreePixmap(unsigned int pixmap)
      : PronRequest(RQ_FREE_PIXMAP) {
    this->pixmap    = pixmap;
  }

  unsigned int pixmap; /**< Id of the pixmap to free */
};

/**
 * RqSetCursor request.
 * Sent by a client to set a cursor to a window.
 */
struct RqSetCursor : public PronRequest {
  /**
   * Constructor.
   * @param pixmap The pixmap containing the cursor to set
   * @param window The window for which to set the cursor
   */
  RqSetCursor(unsigned int pixmap, unsigned int window)
      : PronRequest(RQ_SET_CURSOR) {
    this->pixmap    = pixmap;
    this->window    = window;
  }

  unsigned int pixmap; /**< Id of the pixmap */
  unsigned int window; /**< Id of the window */
};

/**
 * CopyArea request.
 * Sent by a client to copy an area from a drawable to another one.
 */
struct RqCopyArea : public PronRequest {
  /**
   * Constructor.
   * @param src The source drawable
   * @param dest The destination drawable
   * @param gc The graphics context to use during the copy
   * @param srcX The x-coordinate of the top-left corner of the
   * source drawable area to copy
   * @param srcY The y-coordinate of the top-left corner of the
   * source drawable area to copy
   * @param width The width of the source drawable area to copy
   * @param height The height of the source drawable area to copy
   * @param destX The destination top-left corner x-coordinate
   * @param destY The destination top-left corner y-coordinate
   */
  RqCopyArea(unsigned int src, unsigned int dest,
      unsigned int gc, int srcX, int srcY, int width,
      int height, int destX, int destY)
      : PronRequest(RQ_COPY_AREA) {
    this->src = src;
    this->dest = dest;
    this->gc = gc;
    this->srcX = srcX;
    this->srcY = srcY;
    this->width = width;
    this->height = height;
    this->destX = destX;
    this->destY = destY;
  }

  unsigned int src; /**< Id of the source drawable */
  unsigned int dest; /**< Id of the destination drawable */
  unsigned int gc; /**< Id of the graphics context to use */
  int srcX; /**< Source top-left corner x-coordinate */
  int srcY; /**< Source top-left corner x-coordinate */
  int width; /**< Width to copy */
  int height; /**< Height to copy */
  int destX; /**< Destination top-left corner x-coordinate */
  int destY; /**< Destination top-left corner y-coordinate */
};

/**
 * DrawText request.
 * Sent by a client to draw a text.
 */
struct RqDrawText : public PronRequest {
  /**
   * Constructor.
   * @param gc The graphics context to use
   * @param drawable The drawable in which to draw the text
   * @param x The x-coordinate of the origin of the first character
   * @param y The y-coordinate of the origin of the first character
   * @param text The text to draw
   * @param length The length of the text
   * @param hpos The horizontal position of the text
   * @param vpos The vertical position of the text
   */
  RqDrawText(unsigned int gc, unsigned int drawable, int x, int y,
      const char *text, int length, HPosition hpos, VPosition vpos)
      : PronRequest(RQ_DRAW_TEXT) {
    this->gc = gc;
    this->drawable = drawable;
    this->x = x;
    this->y = y;
    memcpy(this->text, text, length);
    this->length = length;
    this->hpos = hpos;
    this->vpos = vpos;
  }

  unsigned int gc; /**< Id of the graphics context to use */
  unsigned int drawable; /**< Id of the drawable in which to draw */
  int x; /**< X-coordinate of the origin of the first character */
  int y; /**< Y-coordinate of the origin of the first character */
  char text[512]; /**< Text to draw */
  int length; /**< Length of the text */
  HPosition hpos; /**< Horizontal position of the text */
  VPosition vpos; /**< Vertical position of the text */
};

/**
 * TextSize request.
 * Sent by a client to get the width and the height of a string.
 */
struct RqTextSize : public PronRequest {
  /**
   * Constructor.
   * @param gc The graphics context to use
   * @param text The text to get the size
   * @param length The length of the text
   */
  RqTextSize(unsigned int gc, const char *text, int length)
      : PronRequest(RQ_TEXT_SIZE) {
    this->gc = gc;
    memcpy(this->text, text, length);
    this->length = length;
  }

  unsigned int gc; /**< Id of the graphics context to use */
  char text[512]; /**< Text to get the size */
  int length; /**< Length of the text */
};

} // namespace pron

#endif // __PRONPROTO_REQUESTS_H__
