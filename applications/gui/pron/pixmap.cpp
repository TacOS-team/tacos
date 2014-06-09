/**
 * @file pixmap.cpp
 * Pixmap class implementation.
 */

#include <algorithm>
#include <stdio.h>
#include <string.h>

#include <drawable.h>
#include <pixmap.h>
#include <screen.h>

using namespace std;

Pixmap::Pixmap(Screen *screen, int id, Client *creator, int width, int height, int depth) 
    : Drawable(D_PIXMAP, screen, id, creator, width, height) {
  this->buf = (char*) malloc(this->getScreen()->bytesPerPixel * width * height);
  this->depth = depth;
}

Pixmap::~Pixmap() {
  free(this->buf);
}

void* Pixmap::pixelAddr(int x, int y) {
  return this->buf + (y * this->getWidth() + x) * this->getScreen()->bytesPerPixel;
}

bool Pixmap::isValid(int x, int y) {
  return (x >= 0 && x < this->getWidth() && y >= 0 && y < this->getHeight());
}
