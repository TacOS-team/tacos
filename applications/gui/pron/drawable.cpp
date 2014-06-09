/**
 * @file drawable.cpp
 * Drawable class implementation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <client.h>
#include <drawable.h>
#include <screen.h>
#include <window.h>

#undef FAST_BPP32

Drawable::Drawable(int type, Screen *screen, int id, Client *creator, int width, int height) {
  this->type = type;
  this->screen = screen;
  this->id = id;
  this->creator = creator;
  this->width = width;
  this->height = height;

  // Add the new drawable to the screen's list of drawables
  screen->addDrawable(this);
}

Drawable::~Drawable() {
  // Destruction
  this->getScreen()->removeDrawable(this);
}

ClipState Drawable::beforeDrawing(int x1 __attribute__((unused)),
    int y1 __attribute__((unused)), int x2 __attribute__((unused)),
    int y2 __attribute__((unused))) {
  return VISIBLE;
}

void Drawable::afterDrawing(int x1 __attribute__((unused)),
    int y1 __attribute__((unused)), int x2 __attribute__((unused)),
    int y2 __attribute__((unused))) {
}

void Drawable::reduce(int &x, int &y, int &width, int &height) {
  if (x < 0) {
    width += x;
    x = 0;
  }

  if (y < 0) {
    height += y;
    y = 0;
  }

  if (x + width > this->getWidth()) {
    width = this->getWidth() - x;
  }

  if (y + height > this->getHeight()) {
    height = this->getHeight() - y;
  }
}

void Drawable::drawPoint(int x, int y) {
  if (this->isValid(x, y)) {
#ifdef FAST_BPP32
    *((int*) this->pixelAddr(x, y)) = this->getScreen()->getGC()->fgValue;
#else
    memcpy(this->pixelAddr(x, y), &this->getScreen()->getGC()->fgValue, this->getScreen()->bytesPerPixel);
#endif
  }
}

void Drawable::drawHorizLine(int x, int y, int width) {
  switch (this->beforeDrawing(x, y, x + width, y)) {
    case VISIBLE: {
#ifdef FAST_BPP32
      int line[width];
      for (int i = 0; i < width; i++) {
        line[i] = this->getScreen()->getGC()->fgValue;
      }
      memcpy(this->pixelAddr(x, y), &line, sizeof(line));
#endif
      for (int c = 0; c < width; c++) {
        this->drawPoint(x + c, y);
      }
      this->afterDrawing(x, y, x + width, y);
    }
    case PARTIAL: {
      for (int c = 0; c < width; c++) {
        this->drawPoint(x + c, y);
      }
      this->afterDrawing(x, y, x + width, y);
      break;
    }
    case INVISIBLE: {
      break;
    }
  }
}

void Drawable::drawVertLine(int x, int y, int height) {
  if (this->beforeDrawing(x, y, x, y + height)) {
    for (int l = 0; l < height; l++) {
      this->drawPoint(x, y + l);
    }

    this->afterDrawing(x, y, x, y + height);
  }
}

void Drawable::drawLine(int x1, int y1, int x2, int y2) {
  if (this->beforeDrawing(x1, y1, x2, y2)) {
    if (x1 == x2) {
      // Vertical line
      if (y1 > y2) {
        int tmp;
        tmp = y1;
        y1 = y2;
        y2 = tmp;
      }
      this->drawVertLine(x1, y1, (y2 - y1 + 1));
    } else if (y1 == y2) {
      // Horizontal line
      if (x1 > x2) {
        int tmp;
        tmp = x1;
        x1 = x2;
        x2 = tmp;
      }
      this->drawHorizLine(x1, y1, (x2 - x1 + 1));
    } else {
      int i, dx, dy, sdx, sdy, dxabs, dyabs, x, y, px, py;

      dx = x2 - x1; 
      dy = y2 - y1;
      dxabs = (dx >= 0 ? dx : -dx);
      dyabs = (dy >= 0 ? dy : -dy);
      sdx = (dx >= 0 ? 1 : -1);
      sdy = (dy >= 0 ? 1 : -1);
      x = dyabs >> 1;
      y = dxabs >> 1;
      px = x1;
      py = y1;

      this->drawPoint(px, py);

      if (dxabs >= dyabs) {
        for (i = 0; i < dxabs; i++) {
          y += dyabs;
          if (y >= dxabs) {
            y -= dxabs;
            py += sdy;
          }
          px += sdx;
          this->drawPoint(px, py);
        }
      } else { 
        for (i = 0; i < dyabs; i++) {
          x += dxabs;
          if (x >= dyabs) {
            x -= dyabs;
            px += sdx;
          }
          py += sdy;
          this->drawPoint(px, py);
        }
      }
    }

    this->afterDrawing(x1, y1, x2, y2);
  }
}

void Drawable::drawRect(int x, int y, int width, int height) {
  if (this->beforeDrawing(x, y, x + width - 1, y + height - 1)) {
    drawHorizLine(x, y, width);
    drawHorizLine(x, y + height - 1, width);
    drawVertLine(x, y + 1, height - 2);
    drawVertLine(x + width - 1, y + 1, height - 2);

    this->afterDrawing(x, y, x + width - 1, y + height - 1);
  }
}

void Drawable::fillRectangle(int x, int y, int width, int height) {
  if (this->beforeDrawing(x, y, x + width - 1, y + height - 1)) {
    for (int l = 0; l < height; l++) {
      drawHorizLine(x, y + l, width);
    }

    this->afterDrawing(x, y, x + width - 1, y + height - 1);
  }
}

void Drawable::drawEllipse(int x0, int y0, int a, int b) {
  int x = 0, y = b;
  long aSquared = a * a, twoAsquared = 2 * aSquared;
  long bSquared = b * b, twoBsquared = 2 * bSquared;
  long d = bSquared - aSquared * b + aSquared / 4L;
  long dx = 0, dy = twoAsquared * b;

  while (dx < dy) {
    this->drawPoint(x0 + x, y0 + y);
    this->drawPoint(x0 + x, y0 - y);
    this->drawPoint(x0 - x, y0 + y);
    this->drawPoint(x0 - x, y0 - y);
    if (d > 0) {
      --y;
      dy -= twoAsquared;
      d -= dy;
    }
    ++x;
    dx += twoBsquared;
    d += bSquared + dx;
  }
  d += (3L * (aSquared - bSquared) / 2L - (dx + dy)) / 2L;
  while (y >= 0) {
    this->drawPoint(x0 + x, y0 + y);
    this->drawPoint(x0 + x, y0 - y);
    this->drawPoint(x0 - x, y0 + y);
    this->drawPoint(x0 - x, y0 - y);
    if (d < 0) {
      ++x;
      dx += twoBsquared;
      d += dx;
    }
    --y;
    dy -= twoAsquared;
    d += aSquared - dy;
  }
}

void Drawable::fillEllipse(int x0, int y0, int a, int b) {
  int x = 0, y = b;
  long aSquared = a * a, twoAsquared = 2 * aSquared;
  long bSquared = b * b, twoBsquared = 2 * bSquared;
  long d = bSquared - aSquared * b + aSquared / 4L;
  long dx = 0, dy = twoAsquared * b;

  while (dx < dy) {
    this->drawHorizLine(x0 - x, y0 + y, 2 * x + 1);
    this->drawHorizLine(x0 - x, y0 - y, 2 * x + 1);
    if (d > 0) {
      --y;
      dy -= twoAsquared;
      d -= dy;
    }
    ++x;
    dx += twoBsquared;
    d += bSquared + dx;
  }
  d += (3L * (aSquared - bSquared) / 2L - (dx + dy)) / 2L;
  while (y >= 0) {
    this->drawHorizLine(x0 - x, y0 + y, 2 * x + 1);
    this->drawHorizLine(x0 - x, y0 - y, 2 * x + 1);
    if (d < 0) {
      ++x;
      dx += twoBsquared;
      d += dx;
    }
    --y;
    dy -= twoAsquared;
    d += aSquared - dy;
  }
}

int Drawable::getPixel(int x, int y) {
  if (this->isValid(x, y)) {
    int ret = 0;
#ifdef FAST_BPP32
    ret = *((int*) this->pixelAddr(x, y));
#else
    memcpy(&ret, this->pixelAddr(x, y), this->getScreen()->bytesPerPixel);
#endif
    return ret;
  }
  return -1;
}

void Drawable::setPixel(int x, int y, int pixel) {
  if (this->isValid(x, y)) {
#ifdef FAST_BPP32
    *((int*) this->pixelAddr(x, y)) = pixel;
#else
    memcpy(this->pixelAddr(x, y), &pixel, this->getScreen()->bytesPerPixel);
#endif
  }
}

void Drawable::putImage(PronImage *image, int x, int y) {
  if (this->beforeDrawing(x, y, x + image->width - 1, y + image->height - 1)) {
    // We have to test if the image and the drawable have the same depth
    if (image->depth == this->getScreen()->bitsPerPixel) {
      // Copy the image in the video memory
      for (int srcY = 0; srcY < image->height; srcY++) {
        for (int srcX = 0; srcX < image->width; srcX++) {
          if (this->isValid(srcX + x, srcY + y)) {
            // Computing the buffer pointers
            void *src = image->data + (srcX + srcY * image->width) * image->bytesPerPixel;
            void *dest = this->pixelAddr(x + srcX, y + srcY);
#ifdef FAST_BPP32
            *((int*) dest) = *((int*) src);
#else
            memcpy(dest, src, image->bytesPerPixel);
#endif
          }
        }
      }
    }
    
    this->afterDrawing(x, y, x + image->width - 1, y + image->height - 1);
  }
}

void Drawable::putImage(PronImage *image, int x, int y, int offset, int size) {
  if (this->beforeDrawing(x, y, x + image->width - 1, y + image->height - 1)) {
    // We have to test if the image and the drawable have the same depth
    if (image->depth == this->getScreen()->bitsPerPixel) {
      // We compute using the offsets where we are in the image
      int srcY = offset / (image->width * image->bytesPerPixel) ;
      int srcX = (offset - (srcY * image->width * image->bytesPerPixel)) / image->bytesPerPixel;
      int currentOffset = 0;
      while (1) {
        // On a fini de copier le segment de l'image
        if (currentOffset >= size) {
          break;
        }
        if (this->isValid(srcX + x, srcY + y)) {
          // Computing the buffer pointers
          void *src = image->data + (srcX + srcY * image->width) * image->bytesPerPixel - offset;
          void *dest = this->pixelAddr(x + srcX, y + srcY);
#ifdef FAST_BPP32
          *((int*) dest) = *((int*) src);
#else
          memcpy(dest, src, image->bytesPerPixel);
#endif
        }
        // Iterate on the image
        if (srcX == image->width - 1) {
          srcY++;
          srcX = 0;
        } else {
          srcX++;
        }
        // Increment the current offset
        currentOffset += image->bytesPerPixel;
      }
    }
    
    this->afterDrawing(x, y, x + image->width - 1, y + image->height - 1);
  }
}

void Drawable::copyArea(int dstX, int dstY, Drawable *d, int srcX, int srcY, int width, int height) {
  if (this->beforeDrawing(dstX, dstY, dstX + width - 1, dstY + height - 1)) {
    /** @todo XXX Bourrin à revoir (problème de depth et de byte per pixel de la pixmap et de l'écran) */
    for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++) {
        this->setPixel(x + dstX, y + dstY, d->getPixel(x + srcX, y + srcY));
      }
    }

    this->afterDrawing(dstX, dstY, dstX + width - 1, dstY + height - 1);
  }
}

void Drawable::drawText(int x, int y, const char *text, int length,
    HPosition hpos, VPosition vpos) {
  Font *font = this->getScreen()->getFont(this->getScreen()->getGC()->font_num);
  
  int textWidth, textHeight;
  font->textSize(text, length, &textWidth, &textHeight);

  /** @todo XXX Fix rapide mais sale car code dupliqué */
  int textX = x, textY = y;
  if (hpos == CENTER) {
    textX -= textWidth / 2;
  } else if (hpos == RIGHT) {
    textX -= textWidth;
  }

  if (vpos == MIDDLE) {
    textY -= textHeight / 2;
  } else if (vpos == BOTTOM) {
    textY -= textHeight;
  }

  if (this->beforeDrawing(textX, textY, textX + textWidth - 1, textY + textHeight - 1)) {
    font->drawText(this, x, y, text, length, hpos, vpos);
    this->afterDrawing(textX, textY, textX + textWidth - 1, textY + textHeight - 1);
  }
}
