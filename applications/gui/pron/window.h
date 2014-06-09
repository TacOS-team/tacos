#ifndef __WINDOW_H__
#define __WINDOW_H__

/**
 * @file pron/window.h
 * Window class definition.
 */

#include <proto/events.h>
#include <vector>
#include <drawable.h>
#include <screen.h>
#include <string>

using namespace std;
using namespace pron;

class Client;
class Pixmap;

/**
 * Client who is not the creator but has selected events on the window.
 */
struct OtherClient {
  /**
   * Constructor.
   * @param client The client
   * @param mask The mask that specifies the selected events
   */
  OtherClient(Client *client, unsigned int mask) {
    this->client = client;
    this->mask = mask;
  }

  Client *client; /**< Client */
  unsigned int mask; /**< Mask that specifies the selected events */
};

/**
 * Window (drawable displayed on the screen).
 * @todo XXX public members bourrin
 */
class Window : public Drawable {
 public:
  int eventMask; /**< Events selected by the creator of the window */
  int dontPropagateMask; /**< Events that the window must not propagate */

  unsigned int pointer; /**< The mouse pointeur of the window */
  
  Window *parent; /**< Parent window */
  Window *prevSibling; /**< Previous sibling (linked list) */
  Window *nextSibling; /**< Next sibling (linked list) */
  Window *firstChild; /**< First child (linked list) */
  Window *lastChild; /**< Last child (linked list) */

  /**
   * Constructor.
   * @param screen The screen the window belongs to
   * @param id The id of the window
   * @param creator The client who has created the window
   * @param parent The parent window
   * @param x The x-coordinate of the top-left corner of the window
   * @param y The y-coordinate of the top-left corner of the window
   * @param width The width of the window
   * @param height The height of the window
   */
  Window(Screen *screen, int id, Client *creator, Window *parent,
      int x, int y, int width, int height);
  
  /**
   * Extended constructor.
   * @param screen The screen the window belongs to
   * @param id The id of the window
   * @param creator The client who has created the window
   * @param parent The parent window
   * @param attributes The attributes of the new window
   * @param mask The mask that specifies the attributes to be set
   */
  Window(Screen *screen, int id, Client *creator, Window *parent,
      PronWindowAttributes *attributes, unsigned int mask);

  /**
   * Destructor.
   */
  ~Window();

  /**
   * Initializes a window.
   * Used by the constructors.
   * @param parent The parent window
   * @param x The x-coordinate of the top-left corner of the window
   * @param y The y-coordinate of the top-left corner of the window
   */
  void init(Window *parent, int x, int y);
  
  /**
   * Gets the address of the pixel (x, y) in memory.
   * @param x The x-coordinate of the pixel
   * @param y The y-coordinate of the pixel
   */ 
  void* pixelAddr(int x, int y) {
    return this->getScreen()->pixelAddr(this->x + x, this->y + y);
  }
  
  /**
   * Returns true if we can draw at position (x, y).
   * @param x The x-coordinate of the point to check
   * @param y The y-coordinate of the point to check
   * @return true if we can draw at position (x, y)
   */
  bool isValid(int x, int y) {
    return this->getScreen()->isValid(this->x + x, this->y + y);
  }

  /**
   * Callback function called before drawing.
   * @param x1 The minimum x-coordinate of the area where to draw
   * @param y1 The minimum y-coordinate of the area where to fraw
   * @param x2 The maximum x-coordinate of the area where to draw
   * @param y2 The maximum y-coordinate of the area where to draw
   * @return the state of the area (fully visible, fully invisible
   * or partially visible)
   */
  ClipState beforeDrawing(int x1, int y1, int x2, int y2);

  /**
   * Callback function called before drawing.
   * @param x1 The minimum x-coordinate of the drawn area
   * @param y1 The minimum y-coordinate of the drawn area
   * @param x2 The maximum x-coordinate of the drawn area
   * @param y2 The maximum y-coordinate of the drawn area
   */
  void afterDrawing(int x1, int y1, int x2, int y2);

  /**
   * Operator ==.
   * @param w The window to compare with
   * @return true if the windows are the same
   */
  bool operator==(const Window &w) const {
    return this->getId() == w.getId();
  }

  /**
   * Operator !=.
   * @param w The window to compare with
   * @return true if the windows are the different
   */
  bool operator!=(const Window &w) const {
    return !(*this == w);
  }

  /**
   * Maps this window (shows it on the screen).
   */
  void map();

  /**
   * Unmaps this (hides it from the screen).
   */
  void unmap();

  /**
   * Clears this window.
   * @param sendExposureEvent Whether to send an exposure event or not
   */
  void clear(bool sendExposureEvent = true) {
    this->clear(0, 0, this->getWidth(), this->getHeight(), sendExposureEvent);
  }

  /**
   * Clears an area of this window.
   * @param x The x-coordinate of the top-left corner of the area to clear
   * @param y The y-coordinate of the top-left corner of the area to clear
   * @param width The width of the area to clear
   * @param height The height of the area to clear
   * @param sendExposureEvent Whether to send an exposure event or not
   */
  void clear(int x, int y, int width, int height, bool sendExposureEvent = true);

  /**
   * Returns the attributes of this window.
   * @return the attributes of this window
   */
  PronWindowAttributes getAttributes();

  /**
   * Sets the attributes of this window.
   * @param newAttributes The new attributes structure
   * @param mask The mask that specifies the attributes to be set
   */
  void setAttributes(PronWindowAttributes *newAttributes, unsigned int mask);

  /**
   * Subscribes to the events associated with the specified event mask
   * for the specified client.
   * @param client The client who subscribes to the events
   * @param mask The event mask
   */
  void selectInput(Client *client, unsigned int mask);

  /**
   * Discards all events previously selected by the specified client.
   * @param client The client who discards the events
   */
  void discardInputs(Client *client);

  /**
   * Raises this window (moves it on top of the screen).
   */
  void raise();

  /**
   * Destroys this window.
   */
  void destroy();

  /**
   * Moves this window of x pixels on the x-axis and y pixels on the y-axis.
   * @param dx The relative move on the x-axis
   * @param dy The relative move on the y-axis
   */
  void move(int dx, int dy);

  /**
   * Moves this window at (x,y) position, relative to its parent.
   * @param x The new x-coordinate, relative to the parent window
   * @param y The new y-coordinate, relative to the parent window
   */
  void moveTo(int x, int y) {
    this->move(this->parent->x + x - this->x, this->parent->y + y - this->y);
  }

  /**
   * Resizes a window.
   * @param width The new width
   * @param height The new height
   */
  void resize(int width, int height);

  /**
   * Handles the exposure of a given area of this window.
   * This window and all its children that overlap the area will be delivered
   * an exposure avent.
   * @param x The x-coordinate of the top-left corner of the exposed area (absolute)
   * @param y The y-coordinate of the top-left corner of the exposed area (absolute)
   * @param width The width of the exposed area
   * @param height The height of the exposed area
   */
  void exposeArea(int x, int y, int width, int height);

  /**
   * Delivers an event to this window.
   * @param e The event to deliver
   * @param size The size of the event
   */
  void deliverEvent(PronEvent *e, unsigned int size);

  /**
   * Delivers an event to this window and its immediate parent.
   * Suitable for window events (EV_WINDOW_CREATED, EV_EXPOSE...).
   * @param e The event to deliver
   * @param size The size of the event
   */
  void deliverWindowEvent(PronEvent *e, unsigned int size);

  /**
   * Delivers a event that needs to propagate up the window tree.
   * Suitable for device events (EV_POINTER_MOVED, EV_KEY_PRESSED...).
   * @param e The event to deliver
   * @param size The size of the event
   */
  void deliverDeviceEvent(PronEvent *e, unsigned int size);

  /**
   * Returns true when the window contains (x,y)
   * @param x The x-coordinate of the point to check
   * @param y The y-coordinate of the point to check
   * @return true when the window contains (x,y)
   */
  bool contains(int x, int y) {
    return this->getX() <= x &&
      this->getY() <= y &&
      this->getX() + this->getWidth() > x &&
      this->getY() + this->getHeight() > y;
  }

  /**
   * Returns true when the window and all its parents are mapped.
   * @return true if the window and all its parent are mapped.
   */
  bool realized() {
    return this->mapped && this->unmappedParents == 0;
  }

  /**
   * Returns the x-coordinate of the top-left corner of the window.
   * @return the x-coordinate of the top-left corner of the window
   */
  int getX() {
    return this->x;
  }

  /**
   * Sets the x-coordinate of the top-left corner of the window.
   * @param x The x-coordinate of the top-left corner of the window
   */
  void setX(int x) {
    this->x = x;
  }

  /**
   * Returns the y-coordinate of the top-left corner of the window.
   * @return the y-coordinate of the top-left corner of the window.
   */
  int getY() {
    return this->y;
  }

  /**
   * Sets the y-coordinate of the top-left corner of the window.
   * @param y The y-coordinate of the top-left corner of the window
   */
  void setY(int y) {
    this->y = y;
  }

  /**
   * Sets a new parent to the window.
   * @param w The new parent window
   */
  void reparent(Window *w);

  /**
   * Returns true if the window accepts at least one of the given events
   * @param eventMask The event mask to check
   * @return true if the window accepts one of the events
   */
  bool acceptsEvents(int eventMask);

  /**
   * Utility function used by Screen::traceWindows.
   * @param prefix The current prefix to show
   */
  void traceWindowsRec(string prefix); 

 private: 
  int x; /**< Top-left corner x-coordinate */
  int y; /**< Top-left corner y-coordinate */
  Color bgColor; /**< Background color */
  bool isResizable; /**< Wheter the window is resizable or not */
  int maxWidth; /**< Maximum width */
  int maxHeight; /**< Maximum height */
  int minWidth; /**< Minimum width */
  int minHeight; /**< Minimum height */
  bool wm_decorate; /**< Whether the window manager should decorate the window or not */
  char wm_title[128]; /**< Title displayed by the window manager */
  bool mapped; /**< Whether the window is mapped (showable on the screen) or not */
  int unmappedParents; /**< Number of unmapped parents. */
  vector<OtherClient> otherClients; /**< Other clients who have selected events on this window */

  /**
   * Returns true if w overlaps this window.
   * @param w The window to check
   * @return true if w overlaps this window
   */
  bool overlaps(Window *w) {
    return !(w->x >= this->x + this->getWidth() || w->y >= this->y + this->getHeight() || w->x + w->getWidth() <= this->x || w->y + w->getHeight() <= this->y);
  }
};

#endif
