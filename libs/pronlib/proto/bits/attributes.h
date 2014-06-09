#ifndef __PRONPROTO_ATTRIBUTES_H__
#define __PRONPROTO_ATTRIBUTES_H__

#include <color.h>

#define WM_TITLE_MAX_LEN 128

namespace pron {

/**
 * Pron window attributes fields for mask on get/set attributes
 */
enum WindowAttributeField {
  WIN_ATTR_X            = (1 << 0), 
  WIN_ATTR_Y            = (1 << 1), 
  WIN_ATTR_WIDTH        = (1 << 2), 
  WIN_ATTR_HEIGHT       = (1 << 3),
  WIN_ATTR_BG_COLOR     = (1 << 4),
  WIN_ATTR_IS_RESIZABLE = (1 << 5),
  WIN_ATTR_MAX_WIDTH    = (1 << 6),
  WIN_ATTR_MAX_HEIGHT   = (1 << 7),
  WIN_ATTR_MIN_WIDTH    = (1 << 8),
  WIN_ATTR_MIN_HEIGHT   = (1 << 9),
  WIN_ATTR_WM_DECORATE  = (1 << 10),
  WIN_ATTR_WM_TITLE     = (1 << 11),
};

/**
 * Structure for pron window attributes
 */
struct PronWindowAttributes {
  int x; /**< X-coordinate of the top-left corner */
  int y; /**< Y-coordinate of the top-left corner */
  int width; /**< Width */
  int height; /**< Height */
  Color bgColor; /**< Background color */
  bool isResizable; /**< Whether the window is resizable or not */
  int maxWidth; /**< Maximum width */
  int maxHeight; /**< Maximum height */
  int minWidth; /**< Minimum width */
  int minHeight; /**< Minimum height */
  bool wm_decorate; /**< Whether the window manager should decorate the window or not */
  char wm_title[128]; /**< Title displayed by the window manager */
};

} // namespace pron

#endif // __PRONPROTO_ATTRIBUTES_H__
