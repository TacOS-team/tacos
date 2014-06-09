#ifndef __PRONPROTO_IMAGE_H__
#define __PRONPROTO_IMAGE_H__

#include <stdlib.h>

namespace pron {

/**
 * Pixmap types.
 * @todo Describe the different types.
 */
enum PixmapType {
  XYBitmap = 0x0000, 
  XYPixmap = 0x0001, 
  ZPixmap  = 0X0002,
};

/**
 * Structure for pron Image
 * This structure is free data pointer
 */
struct PronImage {
  /**
   * PronImage constructor.
   * @param width The width of the image
   * @param height The height of the image
   * @param format The format of the image (XYBitmap, XYPixmap, ZPixmap)
   * @param data The image data
   * @param depth The depth of the image (8, 16 or 24 bits)
   * @param bytesPerPixel The bytes per pixel of the image (1, 2 or 3 bpp)
   * @param freeBuffer Whether we have to free the data buffer or not
   */
  PronImage(int width, int height, int format, char *data, int depth, int bytesPerPixel, bool freeBuffer = true) {
    this->width = width;
    this->height = height;
    this->format = format;
    this->data = data;
    this->depth = depth;
    this->freeBuffer = freeBuffer;
    this->bytesPerPixel = bytesPerPixel;
  }

  /**
   * PronImage destructor.
   * If freeBuffer is true, the image data is freed.
   */
  ~PronImage() {
    if (this->freeBuffer) {
      free(this->data);
    }
  }

  int width; /**< Width of the image */
  int height; /**< Height of the image */
  int format; /**< Format of the image (XYBitmap, XYPixmap, ZPixmap) */
  char *data; /**< Pointer to the image data */
  int depth; /**< Depth of the image (8, 16 or 24 bits) */
  bool freeBuffer; /**< Whether we have to free the data buffer or not */
  int bytesPerPixel; /**< Bytes per pixel of the image (1, 2 or 3 bpp) */
};

} // namespace pron

#endif // __PRONPROTO_IMAGE_H__
