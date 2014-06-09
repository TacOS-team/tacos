#ifndef __PRONPROTO_GC_H__
#define __PRONPROTO_GC_H__

#include <color.h>

namespace pron {

/**
 * Graphics context fields masks for get/set requests.
 */
enum GCValueField {
  GC_VAL_FG        = (1 << 0), 
  GC_VAL_BG        = (1 << 1),
  GC_VAL_FONTNUM   = (1 << 2),
};

/**
 * Graphics context values.
 */
struct PronGCValues {
  Color fg; /**< Foreground color */
  Color bg; /**< Background color */
  int font_num; /**< Font number */
};

} // namespace pron

#endif // __PRONPROTO_GC_H__
