/**
 * @file font.cpp
 * Font class implementation.
 */

#include <font.h>
#include <stdio.h>
#include <string.h>
#include <proto/bits/text.h>

Font::Font(char *name, int height, glyph_t glyphs[256]) {
  this->name = strdup(name);
  this->height = height;
  memcpy(this->glyphs, glyphs, sizeof(glyph_t[256]));

  //this->printInfo();
}

void Font::textSize(const char *text, int length, int *width, int *height) {
  *height = this->height;
  *width = 0;
  for (int i = 0; i < length; i++) {
    unsigned char car = text[i];
    *width += this->glyphs[car].width;
  }
}

void Font::drawChar(Drawable *d, int x, int y, unsigned char car) {
  for (int l = 0; l < this->height; l++) {
    for (int c = 0; c < this->glyphs[car].width; c++) {
      int ci = 16 - 1 - c;
      if (this->glyphs[car].bitmap[l] & (1 << ci)) {
        d->drawPoint(x + c, y + l);
      }
    }
  }
}

void Font::drawText(Drawable *d, int x, int y, const char *text, int length,
    HPosition hpos, VPosition vpos) {
  int textWidth, textHeight;
  this->textSize(text, length, &textWidth, &textHeight);

  if (hpos == CENTER) {
    x -= textWidth / 2;
  } else if (hpos == RIGHT) {
    x -= textWidth;
  }

  if (vpos == MIDDLE) {
    y -= textHeight / 2;
  } else if (vpos == BOTTOM) {
    y -= textHeight;
  }

  for (int i = 0; i < length; i++) {
    unsigned char car = text[i];
    this->drawChar(d, x, y, car);
    x += this->glyphs[car].width;
  }
}

void Font::printInfo() {
  printf("Font name: %s, height: %d\n", this->name, this->height);
  printf("Glyphs:\n");
  for (int i = 0; i < 255; i++) {
    printf("Char %d:\n", i);
    for (int l = 0; l < this->height; l++) {
      for (int c = 15; c >= 0; c--) {
        if (this->glyphs[i].bitmap[l] & (1 << c)) {
          printf("*");
        } else {
          printf(" ");
        }
      }
      printf("\n");
    }
    printf("\n");
  }
}
