/**
 * @file pron/window.cpp
 * Window class implementation.
 */

#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <client.h>
#include <drawable.h>
#include <mouse.h>
#include <screen.h>
#include <window.h>

Window::Window(Screen *screen, int id, Client *creator, Window *parent,
    PronWindowAttributes *attributes, unsigned int mask)
    : Drawable(D_WINDOW, screen, id, creator, attributes->width, attributes->height) {
  this->init(parent, attributes->x, attributes->y);
  this->setAttributes(attributes, mask);
  
  if (this->parent != NULL) {
    // Send an event notifiying the window creation
    EventWindowCreated eventCreated(this->getId(), this->parent->getId(),
        this->getAttributes());
    this->deliverWindowEvent(&eventCreated, sizeof(eventCreated));
  }
}

Window::Window(Screen *screen, int id, Client *creator, Window *parent,
    int x, int y, int width, int height) 
    : Drawable(D_WINDOW, screen, id, creator, width, height) {
  this->init(parent, x, y);
  
  if (this->parent != NULL) {
    // Send an event notifiying the window creation
    EventWindowCreated eventCreated(this->getId(), this->parent->getId(),
        this->getAttributes());
    this->deliverWindowEvent(&eventCreated, sizeof(eventCreated));
  }
}

void Window::init(Window *parent, int x, int y) {
  this->pointer = 6663;
  this->x = x;
  this->y = y;
  memset(&this->bgColor, 0, sizeof(this->bgColor));
  this->bgColor = (parent == NULL) ? Color(0x20, 0x20, 0x20) : Color(0xdb, 0xdb, 0xdf);
  this->eventMask = 0;
  this->dontPropagateMask = 0;
  this->mapped = false;
  this->isResizable = true;
  this->maxWidth = -1; // -1 means there is no limit
  this->maxHeight = -1;
  this->minWidth = -1;
  this->minHeight = -1;
  this->wm_decorate = true;
  sprintf(this->wm_title, "Window %x", this->getId());

  if (parent != NULL) {
    this->unmappedParents = (parent->mapped ? 0 : 1) + parent->unmappedParents;
  } else {
    this->unmappedParents = 0;
  }

  // Père
  this->parent = parent;
  // Fils
  this->firstChild = this->lastChild = NULL;
  // Frères
  if (this->parent != NULL) {
    this->prevSibling = parent->lastChild;
    this->nextSibling = NULL;
  } else {
    this->prevSibling = this->nextSibling = NULL;
  }
  // Maj du père
  if (this->parent != NULL) {
    if (this->parent->lastChild == NULL) {
      this->parent->firstChild = this->parent->lastChild = this;
    } else {
      this->parent->lastChild->nextSibling = this;
      this->parent->lastChild = this;
    }
  }
}

// Destructor : delete all of the childs
Window::~Window() {
  for (Window *child = this->firstChild, *nextChild; child != NULL; child = nextChild) {
    nextChild = child->nextSibling;
    delete child;
  }
}

void Window::unmap() {
  // Can't unmap root window
  if (this->parent == NULL) {
    return;
  }

  // Already unmapped
  if (!this->mapped) {
    return;
  }

  this->mapped = false;

  Window *mouseWin = this->getScreen()->getMouseWin();
  Window *clipWin = this->getScreen()->getClipWin();
  Window *focusWin = this->getScreen()->getFocusWin();

  for (WindowsTree::IteratorBFS it = this->getScreen()->tree->beginBFS(this); it != this->getScreen()->tree->endBFS(); it++) {
    // Update clipWin/mouseWin/focusWin
    if (clipWin != NULL && *clipWin == *it) {
      this->getScreen()->setClipWin(NULL);
    }

    if (mouseWin != NULL && *mouseWin == *it) {
      Mouse::getInstance()->updateMouseWin();
    }

    if (focusWin != NULL && *focusWin == *it) {
      Mouse::getInstance()->updateFocusWin();
    }

    // Update all children
    if (*it != *this) {
      it->unmappedParents++;
    }
  }

  if (this->parent->realized()) {
    // Clear the area of the parent window occupied by this window and send exposure event
    if (this->parent == this->getScreen()->getRoot()) {
      this->parent->clear(this->x - parent->x, this->y - parent->y, this->getWidth(), this->getHeight());
    } else {
      // Test: only send exposure event to improve performances
      //printf("expose parent %x\n", this->parent->getId());
      EventExpose expose(this->parent->getId(), this->x - parent->x, this->y - parent->y, this->getWidth(), this->getHeight());
      this->parent->deliverEvent(&expose, sizeof(expose));
    }

    // Redraw covered lower siblings
    for (Window *sib = this->prevSibling; sib != NULL; sib = sib->prevSibling) {
      if (this->overlaps(sib)) {
        sib->exposeArea(this->x, this->y, this->getWidth(), this->getHeight()); 
      }
    }
  }
}

void Window::map() {
  // Already mapped
  if (this->mapped) {
    return;
  }

  this->mapped = true;

  /** @todo update clipwin/mousewin/focuswin */

  // Clear the window and send exposure event
  //this->clear();
  // Test: only send exposure event to improve performances
  if (this->realized()) {
    EventExpose expose(this->getId(), 0, 0, this->getWidth(), this->getHeight());
    this->deliverEvent(&expose, sizeof(expose));
  }

  // Update all children
  for (WindowsTree::IteratorBFS it = ++(this->getScreen()->tree->beginBFS(this)); it != this->getScreen()->tree->endBFS(); it++) {
    it->unmappedParents--;

    if (it->realized()) {
      EventExpose expose(it->getId(), 0, 0, it->getWidth(), it->getHeight());
      it->deliverEvent(&expose, sizeof(expose));
    }
  }

  Mouse::getInstance()->updateMouseWin();
}

void Window::clear(int x, int y, int width, int height, bool sendExposureEvent) {
  Color oldFg = this->getScreen()->getGC()->getFg();
  this->getScreen()->getGC()->setFg(this->bgColor);
  this->getScreen()->setClipWin(this);
  if (x == 0 && y == 0 && width == this->getWidth() && height == this->getHeight()) {
    vector<ClipRect*> clipRects = this->getScreen()->getClipZone()->getClipRects();
    for (unsigned int i = 0; i < clipRects.size(); i++) {
      this->getScreen()->getRoot()->fillRectangle(clipRects[i]->x, clipRects[i]->y, clipRects[i]->width, clipRects[i]->height);
    }
  } else {
    this->reduce(x, y, width, height);
    this->fillRectangle(x, y, width, height);
  }
  this->getScreen()->getGC()->setFg(oldFg);

  if (sendExposureEvent && this->realized()) {
    // Send exposure event
    EventExpose expose(this->getId(), x, y, width, height);
    this->deliverEvent(&expose, sizeof(expose));
  }
}

PronWindowAttributes Window::getAttributes() {
  PronWindowAttributes attr;

  attr.x = this->x;
  attr.y = this->y;
  attr.width = this->getWidth();
  attr.height = this->getHeight();
  attr.bgColor = this->bgColor;
  attr.isResizable = this->isResizable;
  attr.maxWidth = this->maxWidth;
  attr.maxHeight = this->maxHeight;
  attr.minWidth = this->minWidth;
  attr.minHeight = this->minHeight;
  attr.wm_decorate = this->wm_decorate;
  strcpy(attr.wm_title, this->wm_title);

  return attr;
}

void Window::setAttributes(PronWindowAttributes *newAttr, unsigned int mask) {
  if (mask & WIN_ATTR_X) {
    this->x = newAttr->x;
  }
  if (mask & WIN_ATTR_Y) {
    this->y = newAttr->y;
  }
  if (mask & WIN_ATTR_WIDTH) {
    this->setWidth(newAttr->width);
  }
  if (mask & WIN_ATTR_HEIGHT) {
    this->setHeight(newAttr->height);
  }
  if (mask & WIN_ATTR_BG_COLOR) {
    this->bgColor = newAttr->bgColor;
  }
  if (mask & WIN_ATTR_IS_RESIZABLE) {
    this->isResizable = newAttr->isResizable;
  }
  if (mask & WIN_ATTR_MAX_WIDTH) {
    this->maxWidth = newAttr->maxWidth;
  }
  if (mask & WIN_ATTR_MAX_HEIGHT) {
    this->maxHeight = newAttr->maxHeight;
  }
  if (mask & WIN_ATTR_MIN_WIDTH) {
    this->minWidth = newAttr->minWidth;
  }
  if (mask & WIN_ATTR_MIN_HEIGHT) {
    this->minHeight = newAttr->minHeight;
  }
  if (mask & WIN_ATTR_WM_DECORATE) {
    this->wm_decorate = newAttr->wm_decorate;
  }
  if (mask & WIN_ATTR_WM_TITLE) {
    strncpy(this->wm_title, newAttr->wm_title, WM_TITLE_MAX_LEN - 1);
    this->wm_title[WM_TITLE_MAX_LEN - 1] = '\0';
  }
}

void Window::selectInput(Client *client, unsigned int mask) {
  if (client == this->getCreator()) {
    this->eventMask = mask;
  } else {
    unsigned int i;
    for (i = 0; i < this->otherClients.size(); i++) {
      if (this->otherClients[i].client == client) {
        this->otherClients[i].mask = mask;
        break;
      }
    }
    if (i == this->otherClients.size()) { // Not found, add it
      this->otherClients.push_back(OtherClient(client, mask));
    }
  }
}

void Window::discardInputs(Client *client) {
  for (vector<OtherClient>::iterator it = this->otherClients.begin(); it != this->otherClients.end(); ++it) {
    if (it->client == client) {
      this->otherClients.erase(it);
      break;
    }
  }
}

bool Window::acceptsEvents(int eventMask) {
  if (this->eventMask & eventMask) {
    return true;
  }

  for (unsigned int i = 0; i < this->otherClients.size(); i++) {
    if (this->otherClients[i].mask & eventMask) {
      return true;
    }
  }

  return false;
}

void Window::deliverEvent(PronEvent *e, unsigned int size) {
  unsigned int eventMask = PRON_EVENTMASK(e->type);
  
  // Deliver to creator
  if (this != this->getScreen()->tree->getRoot() && (this->eventMask & eventMask)) {
    this->getCreator()->send(e, size);
  }

  // Deliver to other clients
  for (unsigned int i = 0; i < this->otherClients.size(); i++) {
    if (this->otherClients[i].mask & eventMask) {
      this->otherClients[i].client->send(e, size);
    }
  }
}

void Window::deliverWindowEvent(PronEvent *e, unsigned int size) {
  this->deliverEvent(e, size);
  e->window = this->getId();
  printf("deliver window event to %x\n", e->window);
  if (this->parent) {
    this->parent->deliverEvent(e, size);
  }
}

void Window::deliverDeviceEvent(PronEvent *e, unsigned int size) {
  unsigned int eventMask = PRON_EVENTMASK(e->type);

  // Set the event window
  e->window = this->getId();
  printf("deliver device event to %x\n", e->window);
  this->deliverEvent(e, size);

  if (this->parent && !(this->dontPropagateMask & eventMask)) {
    this->parent->deliverDeviceEvent(e, size);
  }
}

void Window::raise() {
  // Do nothing if we are the root window
  if (this->parent == NULL) {
    return;
  }

  // Do nothing if we are already the last child of our parent
  if (this->parent->lastChild != this) {
    Window *sibling = this->nextSibling;
    bool overlap = false;
    for (; sibling != NULL; sibling = sibling->nextSibling) {
      overlap = overlap || this->overlaps(sibling);
    }

    if (this->prevSibling == NULL) {
      this->parent->firstChild = this->nextSibling;
    } else {
      this->prevSibling->nextSibling = this->nextSibling;  
    }
    if (this->nextSibling != NULL) {
      this->nextSibling->prevSibling = this->prevSibling;
    }
    this->prevSibling = this->parent->lastChild;
    this->nextSibling = NULL;
    this->parent->lastChild->nextSibling = this;
    this->parent->lastChild = this;

    if (overlap) {
      this->exposeArea(this->x, this->y, this->getWidth(), this->getHeight());
    }
  }

  this->parent->raise();
}

void Window::exposeArea(int x, int y, int width, int height) {
  for (WindowsTree::IteratorBFS it = this->getScreen()->tree->beginBFS(this); it != this->getScreen()->tree->endBFS(); it++) {
    //it->clear(x - it->x, y - it->y, width, height);
    // Test: only send exposure event to improve performances
    if (it->realized()) {
      int myX = x - it->x;
      int myY = y - it->y;
      int myW = width;
      int myH = height;
      it->reduce(myX, myY, myW, myH);
      if (myW > 0 && myH > 0) {
        EventExpose expose(it->getId(), myX, myY, myW, myH);
        it->deliverEvent(&expose, sizeof(expose));
      }
    }
  }
}

void Window::reparent(Window *w) {
  if (this->prevSibling != NULL) {
    this->prevSibling->nextSibling = this->nextSibling;
  } else {
    this->parent->firstChild = this->nextSibling;
  }
  if (this->nextSibling != NULL) {
    this->nextSibling->prevSibling = this->prevSibling;
  } else {
    this->parent->lastChild = this->prevSibling;
  }

  if (w->lastChild != NULL) {
    w->lastChild->nextSibling = this;
    this->prevSibling = w->lastChild;
    w->lastChild = this;
  } else {
    w->firstChild = this;
    w->lastChild = this;
    this->prevSibling = NULL;
  }
  this->nextSibling = NULL;
  
  this->parent = w;

  for (WindowsTree::IteratorBFS it = this->getScreen()->tree->beginBFS(this); it != this->getScreen()->tree->endBFS(); it++) {
    it->unmappedParents = (it->parent->mapped ? 0 : 1) + it->parent->unmappedParents;
  }
}

void Window::destroy() {
  printf("window::destroy(%x) (@%p)\n", this->getId(), this);
  this->unmap();
  
  // We remove the window from the tree 
  if (this->prevSibling != NULL) {
    this->prevSibling->nextSibling = this->nextSibling;
  } else {
    this->parent->firstChild = this->nextSibling;
  }
  if (this->nextSibling != NULL) {
    this->nextSibling->prevSibling = this->prevSibling;
  } else {
    this->parent->lastChild = this->prevSibling;
  }

  for (WindowsTree::IteratorBFS it = this->getScreen()->tree->beginBFS(this); it != this->getScreen()->tree->endBFS(); ++it) {
    if (this->getScreen()->getGrabWin() == &(*it)) {
      this->getScreen()->setGrabWin(NULL);
    }
  }
  
  this->getScreen()->destroy(this);
  delete this;
}

void Window::move(int dx, int dy) {
  bool isMapped = this->realized();

  if (isMapped) {
    this->unmap();
  }

  for (WindowsTree::IteratorBFS it = this->getScreen()->tree->beginBFS(this); it != this->getScreen()->tree->endBFS(); it++) {
    it->x += dx;
    it->y += dy;
  }

  if (isMapped) {
    this->map();
  }
}

void Window::resize(int width, int height) {
  bool isMapped = this->realized();

  if (isMapped) {
    this->unmap();
  }

  this->setWidth(width);
  this->setHeight(height);

  if (isMapped) {
    this->map();
  }

  // Send resize event
  EventResizeWindow eventResizeWindow(width, height);
  eventResizeWindow.window = this->getId();
  this->deliverWindowEvent(&eventResizeWindow, sizeof(eventResizeWindow));
}

ClipState Window::beforeDrawing(int x1, int y1, int x2, int y2) {
  ClipState state = this->getScreen()->getClipZone()->checkArea(
      this->x + x1, this->y + y1, this->x + x2, this->y + y2);

  if (state == VISIBLE) {
    this->getScreen()->setClippingCheck(false);
  } else {
    this->getScreen()->setClippingCheck(true);
  }

  if (Mouse::getInstance()->overlapsPointer(this->x + x1, this->y + y1, this->x + x2, this->y + y2)) {
    Mouse::getInstance()->hidePointer();
  }

  return state;
}

void Window::afterDrawing(int x1 __attribute__((unused)),
    int y1 __attribute__((unused)), int x2 __attribute__((unused)),
    int y2 __attribute__((unused))) {
  // Will only show the pointer if it has been hidden
  Mouse::getInstance()->showPointer();
}

void Window::traceWindowsRec(string prefix __attribute__((unused))) {
  /*printf("%s%-6x (p: %-6x, fc: %-6x, lc: %-6x, ps: %-6x, ns: %-6x\n",
        prefix.c_str(),
        this->getId(),
        this->parent == NULL ? 0 : this->parent->getId(),
        this->firstChild == NULL ? 0 : this->firstChild->getId(),
        this->lastChild == NULL ? 0 : this->lastChild->getId(),
        this->prevSibling == NULL ? 0 : this->prevSibling->getId(),
        this->nextSibling == NULL ? 0 : this->nextSibling->getId());
  printf("%s        x: %-6d, y : %-6d, w : %-6d, h : %-6d, r : %s\n",
        prefix.c_str(),
        this->getX(),
        this->getY(),
        this->getWidth(),
        this->getHeight(),
        this->realized() ? "y" : "n");
  printf("%s        m: %-6d, up: %-6d)\n",
        prefix.c_str(),
        this->mapped,
        this->unmappedParents);
  for (Window *currentChild = this->firstChild; currentChild != NULL; currentChild = currentChild->nextSibling) {
    currentChild->traceWindowsRec(prefix + "--");
  }*/
}
