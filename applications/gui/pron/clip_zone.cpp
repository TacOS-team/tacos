/**
 * @file clip_zone.cpp
 * ClipZone class implementation.
 */

#include <stdio.h>

#include <limits.h>
#include <clip_zone.h>
#include <screen.h>
#include <window.h>

ClipZone::ClipZone(int x, int y, int width, int height)
    : cache(0, 0, 0, 0), cacheRes(false) /* empty cache */ {
  // Clipping zone containing only 1 rectangle
  ClipRect *clip = new ClipRect(x, y, width, height);
  this->clipRects.push_back(clip);
}

ClipZone::ClipZone(Window *w)
    : cache(0, 0, 0, 0), cacheRes(false) /* empty cache */ {
  // Initial rectangle
  ClipRect *clip = new ClipRect(w);
  this->clipRects.push_back(clip);

  // Restrict this rectangle to the part visible through all its parents
  Window *parent, *sibling;
  parent = w;
  while (parent != w->getScreen()->tree->getRoot()) {
    parent = parent->parent;

    // X1
    if (parent->getX() > clip->x) {
      clip->width -= (parent->getX() - clip->x);
      clip->x = parent->getX();
    }

    // Y1
    if (parent->getY() > clip->y) {
      clip->height -= (parent->getY() - clip->y);
      clip->y = parent->getY();
    }

    // X2
    if (parent->getX() + parent->getWidth() < clip->x + clip->width) {
      clip->width -= (clip->x + clip->width - (parent->getX() + parent->getWidth()));
    }

    // Y2
    if (parent->getY() + parent->getHeight() < clip->y + clip->height) {
      clip->height -= (clip->y + clip->height - (parent->getY() + parent->getHeight()));
    }
  }

  // If it's still visible
  if (clip->width > 0 || clip->height > 0) {
    // Break this rectangle into pieces not obscured by other windows
    parent = w;
    while (parent != w->getScreen()->tree->getRoot()) {
      w = parent;
      parent = w->parent;
      sibling = w->nextSibling;
      
      for (; sibling != NULL; sibling = sibling->nextSibling) {
        if (sibling->realized()) {
          ClipRect obscurer(sibling);
          vector<ClipRect*> oldClipRects = this->clipRects;
          this->clipRects.clear();
          for (unsigned int i = 0; i < oldClipRects.size(); i++) {
            vector<ClipRect*> splittedRect = oldClipRects[i]->split(&obscurer);
            this->clipRects.insert(this->clipRects.begin(), splittedRect.begin(), splittedRect.end());
            delete oldClipRects[i];
          }
        }
      }
    }
  }
}

bool ClipZone::contains(int x, int y) {
  bool inZone = false;

  // Check result in cache
  if (x >= this->cache.x && y >= this->cache.y &&
      x < this->cache.x + this->cache.width && y < this->cache.y + this->cache.height) {
    inZone = this->cacheRes;
  } else if (false) {
    /**
     * @todo optimization: points outside the screen => return false,
     * cache => half-plane outside the screen
     */
  } else {
    // Scan all clip rectangles
    for (unsigned int i = 0; i < this->clipRects.size() && !inZone; i++) {
      if (this->clipRects[i]->contains(x, y)) {
        inZone = true;
        this->cache = *(this->clipRects[i]);
      }
    }

    if (!inZone) {
      // Search the maximum non-visible rectangle containing this point
      int cacheX1 = SHRT_MIN, cacheY1 = SHRT_MIN;
      int cacheX2 = SHRT_MAX, cacheY2 = SHRT_MAX;

      for (unsigned int i = 0; i < this->clipRects.size(); i++) {
        ClipRect *rect = this->clipRects[i];
        if (x < rect->x && rect->x < cacheX2) {
          cacheX2 = rect->x - 1;
        }
        if (y < rect->y && rect->y < cacheY2) {
          cacheY2 = rect->y - 1;
        }
        if (x >= rect->x + rect->width && rect->x + rect->width > cacheX1) {
          cacheX1 = rect->x + rect->width;
        }
        if (y >= rect->y + rect->height && rect->y + rect->height > cacheY1) {
          cacheY1 = rect->y + rect->height;
        }
      }

      this->cache.x = cacheX1;
      this->cache.y = cacheY1;
      this->cache.width = cacheX2 - cacheX1 + 1;
      this->cache.height = cacheY2 - cacheY1 + 1;
    }

    this->cacheRes = inZone;
  }

  return inZone; 
}

ClipState ClipZone::checkArea(int x1, int y1, int x2, int y2) {
  ClipState state = INVISIBLE;

  if (this->contains(x1, y1)) {
    state = this->cache.contains(x2, y2) ? VISIBLE : PARTIAL;
  } else {
    state = this->cache.contains(x2, y2) ? INVISIBLE : PARTIAL;
  }

  return state;
}

void ClipZone::print() {
  printf("########## CLIP ZONE ##########\n");
  for (unsigned int i = 0; i < this->clipRects.size(); i++) {
    printf("x: %d  y: %d  w: %d  h: %d\n", this->clipRects[i]->x, this->clipRects[i]->y, this->clipRects[i]->width, this->clipRects[i]->height);
  }
  printf("###############################\n");
}
