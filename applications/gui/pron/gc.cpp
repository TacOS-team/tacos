/**
 * @file gc.cpp
 * GC class implementation.
 */

#include <algorithm>
#include <gc.h>
#include <screen.h>

std::vector<GC*> GC::gcs;

GC::GC() {
  this->id = 0;

  // Default foreground color
  //this->setFg(Color(255, 255, 255));
  /** @todo xxx */
  this->fg = Color(255, 255, 255);
  this->fgValue = 0x00FFFFFF;

  // Default background color
  //this->setBg(Color(0, 0, 0));
  /** @todo xxx */
  this->bg = Color(0, 0, 0);
  this->bgValue = 0x00000000;

  // Default font
  this->font_num = 0;
}

GC::GC(unsigned int id, const PronGCValues &values, unsigned int mask) {
  this->id = id;

  // Default foreground color
  //this->setFg(Color(255, 255, 255));
  /** @todo xxx */
  this->fg = Color(255, 255, 255);
  this->fgValue = 0x00FFFFFF;

  // Default background color
  //this->setBg(Color(0, 0, 0));
  /** @todo xxx */
  this->bg = Color(0, 0, 0);
  this->bgValue = 0x00000000;

  // Default font
  this->font_num = 0;

  this->setValues(values, mask);
  GC::gcs.push_back(this);
}

GC* GC::getGC(unsigned int id) {
  for (unsigned int i = 0; i < GC::gcs.size(); i++) {
    if (GC::gcs[i]->id == id) {
      return GC::gcs[i];
    }
  }
  return NULL;
}

PronGCValues GC::getValues() {
  PronGCValues values;
  values.fg = this->fg;
  values.bg = this->bg;
  values.font_num = this->font_num;

  return values; 
}

void GC::setValues(const PronGCValues &values, unsigned int mask) {
  if (mask & GC_VAL_FG) {
    this->setFg(values.fg);
  }
  if (mask & GC_VAL_BG) {
    this->setBg(values.bg);
  }
  if (mask & GC_VAL_FONTNUM) {
    this->font_num = values.font_num;
  }
}
  
void GC::setBg(Color c) {
  this->bg = c;
  this->bgValue = c.getRGB(Screen::getInstance()->bitsPerPixel);
}

void GC::setFg(Color c) {
  this->fg = c;
  this->fgValue = c.getRGB(Screen::getInstance()->bitsPerPixel);
}

void GC::destroy() {
  GC::gcs.erase(std::find(GC::gcs.begin(), GC::gcs.end(), this));
  delete this;
}
