/**
 * @file clip_rect.cpp
 * ClipRect class implementation.
 */

#include <clip_rect.h>
#include <window.h>

ClipRect::ClipRect(int x, int y, int width, int height) {
  this->x = x;
  this->y = y;
  this->width = width;
  this->height = height;
}

ClipRect::ClipRect(Window *w) {
  this->x = w->getX();
  this->y = w->getY();
  this->width = w->getWidth();
  this->height = w->getHeight();
}

vector<ClipRect*> ClipRect::split(ClipRect *obscurer) {
  vector<ClipRect*> splittedRect;

  bool visible_top = this->y < obscurer->y;
  bool visible_bottom = this->y + this->height > obscurer->y + obscurer->height;
  bool visible_left = this->x < obscurer->x;
  bool visible_right = this->x + this->width > obscurer->x + obscurer->width;

  // No overlap
  if (this->x >= obscurer->x + obscurer->width ||
      this->y >= obscurer->y + obscurer->height ||
      this->x + this->width <= obscurer->x ||
      this->y + this->height <= obscurer->y) {
    splittedRect.push_back(new ClipRect(this->x, this->y, this->width, this->height));
    return splittedRect;
  }

  // Determine the zones that are still visible
  if (visible_top) {
    // Visible zone on top
    splittedRect.push_back(new ClipRect(this->x, this->y, this->width, obscurer->y - this->y));
  }

  if (visible_bottom) {
    // Visible zone at bottom
    splittedRect.push_back(new ClipRect(this->x, obscurer->y + obscurer->height, this->width, this->y + this->height - (obscurer->y + obscurer->height)));
  }

  if (visible_left) {
    // Visible zone at left
    if (visible_top && visible_bottom) {
      splittedRect.push_back(new ClipRect(this->x, obscurer->y, obscurer->x - this->x, obscurer->height));
    } else if (visible_top) {
      splittedRect.push_back(new ClipRect(this->x, obscurer->y, obscurer->x - this->x, this->y + this->height - obscurer->y));
    } else if (visible_bottom) {
      splittedRect.push_back(new ClipRect(this->x, this->y, obscurer->x - this->x, obscurer->y + obscurer->height - this->y));
    } else {
      splittedRect.push_back(new ClipRect(this->x, this->y, obscurer->x - this->x, this->height));
    }
  }

  if (visible_right) {
    // Visible zone at right
    if (visible_top && visible_bottom) {
      splittedRect.push_back(new ClipRect(obscurer->x + obscurer->width, obscurer->y, this->x + this->width - (obscurer->x + obscurer->width), obscurer->height));
    } else if (visible_top) {
      splittedRect.push_back(new ClipRect(obscurer->x + obscurer->width, obscurer->y, this->x + this->width - (obscurer->x + obscurer->width), this->y + this->height - obscurer->y));
    } else if (visible_bottom) {
      splittedRect.push_back(new ClipRect(obscurer->x + obscurer->width, this->y, this->x + this->width - (obscurer->x + obscurer->width), obscurer->y + obscurer->height - this->y));
    } else {
      splittedRect.push_back(new ClipRect(obscurer->x + obscurer->width, this->y, this->x + this->width - (obscurer->x + obscurer->width), this->height));
    }
  }

  return splittedRect;
}
