#include "color.h"

Color::Color(float r, float g, float b)
    : r(r), g(g), b(b){
}

Color::Color(int r, int g, int b) {
  this->r = (float) r / 255.;
  this->g = (float) g / 255.;
  this->b = (float) b / 255.;
}

float Color::getR() {
  return this->r;
}

float Color::getG() {
  return this->g;
}

float Color::getB() {
  return this->b;
}

void Color::setR(float r) {
  this->r = r;
}

void Color::setG(float g) {
  this->g = g;
}

void Color::setB(float b) {
  this->b = b;
}

int Color::getRGB(int depth) {
  switch (depth) {
    case 24:
      return getRGB24();
    default:
      return getRGB24();
  }
}

int Color::getRGB24() {
  int pixel = 0;
  // Red
  pixel |= ((int)(this->r * 0xFF)) << 16;
  // Green
  pixel |= ((int)(this->g * 0xFF)) << 8;
  // Blue
  pixel |= ((int)(this->b * 0xFF)) << 0;
  // The rest of the pixel
  pixel &= 0x00FFFFFF;
  return pixel;
}
