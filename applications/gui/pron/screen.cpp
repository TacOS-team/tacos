/**
 * @file screen.cpp
 * Screen class implementation.
 */

#include <dirent.h>
#include <fcntl.h>
//#include <math.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <sys/ioctl.h>

#include <font_loader.h>
#include <screen.h>
#include <window.h>

using namespace std;

Screen* Screen::instance = NULL;

Screen::Screen(int width, int height, int bitsPerPixel) {
  this->width = width;
  this->height = height;
  this->bitsPerPixel = bitsPerPixel;
  this->bytesPerPixel = bitsPerPixel/8;

  this->vesa_fd = open("/dev/vesa", O_RDONLY);
  struct vesa_setmode_req req = { this->width, this->height, this->bitsPerPixel };
  ioctl(this->vesa_fd, SETMODE, &req);
  ioctl(this->vesa_fd, GETVIDEOADDR, &this->videoBuffer);

  // HACK(julienm): TacOS doesn't support scandir/alphasort.
  /*struct dirent **namelist;
  int n = scandir("resources/fonts", &namelist, 0, alphasort);
  if (n < 0) {
    perror("scandir");
    fprintf(stderr, "Could not open the fonts directory.\n");
    exit(1);
  }
  else {
    for (int i = 0; i < n; i++) {
      if (namelist[i]->d_type == DT_REG) {
        char fileName[256] = "resources/fonts/";
        strcat(fileName, namelist[i]->d_name);
        this->fonts.push_back(FontLoader::load(fileName));
      }
      free(namelist[i]);
    }
  }
  free(namelist);*/
  DIR *dp;
  struct dirent *ep;
  dp = opendir("fonts");
  if (dp != NULL) {
    while ((ep = readdir(dp))) {
      if (ep->d_type == DT_REG) {
        char fileName[256] = "fonts/";
        strcat(fileName, ep->d_name);
        this->fonts.push_back(FontLoader::load(fileName));
      }
    }
    closedir (dp);
  } else {
    fprintf(stderr, "Could not open the fonts directory.\n");
    exit(1);
  }

  this->clipWin = NULL;
  this->clipZone = new ClipZone(0, 0, this->width, this->height);
  this->clippingCheck = true;

  this->gc = &this->defaultGC;

  this->mouseWin = NULL;
  this->focusWin = NULL;
  this->grabWin = NULL;

  this->tree = new WindowsTree();
}

Drawable* Screen::getDrawable(unsigned int id, int drawableType) {
  for (unsigned int i = 0; i < this->drawables.size(); i++) {
    if (drawables[i]->getId() == id) {
      return (drawables[i]->getType() == drawableType) ? drawables[i] : NULL;
    }
  }
  return NULL;
}

Drawable* Screen::getDrawable(unsigned int id) {
  for (unsigned int i = 0; i < this->drawables.size(); i++) {
    if (drawables[i]->getId() == id) {
      return drawables[i];
    }
  }
  return NULL;
}

void Screen::setClipWin(Window *w) {
  if (w != this->clipWin) {
    // Update clipzone
    if (this->clipZone != NULL) {
      delete this->clipZone;
    }
    if (w == NULL) {
      // Special case : whole screen
      this->clipZone = new ClipZone(0, 0, this->width, this->height);
    } else {
      this->clipZone = new ClipZone(w);
    }
    this->clipWin = w;
  }
}

bool Screen::prepareDrawing(Drawable *d, GC *gc) {
  if (d->getType() == D_WINDOW) {
    Window *w = (Window*) d;

    if (!w->realized()) {
      // Can't draw in unrealized window
      return false;
    }

    this->setClipWin(w);
    this->setClippingCheck(true);
  }

  if (gc != NULL) {
    this->gc = gc;
  }

  return true;
}

void Screen::traceWindows() {
  this->tree->getRoot()->traceWindowsRec("");
  /*debug("TraceWindow : ");
  for (WindowsTree::IteratorDFS it = tree->beginDFS() ; it != tree->endDFS(); it++) {
    debug("%d ", it->getId());
  }
  */
  //debug("\n");
}

void Screen::destroy(Window * w) {
  for (WindowsTree::IteratorBFS it = this->tree->beginBFS(w); it != this->tree->endBFS(); it++) {
    EventDestroyWindow eventDestroyWindow(it->getId());
    it->deliverWindowEvent(&eventDestroyWindow, sizeof(eventDestroyWindow));
  }
}

void Screen::reparent(Window * child, Window * newParent) {
  child->reparent(newParent);
  this->setClipWin(NULL);
  //this->traceWindows();
}
