#ifndef __PRONPROTO_BASE_H__
#define __PRONPROTO_BASE_H__

#define RQ_PREFIX (0 << 16)
#define RS_PREFIX (1 << 16)
#define EV_PREFIX (2 << 16)
#define ER_PREFIX (3 << 16)
#define CATEGORY_MASK (3 << 16)

namespace pron {

/**
 * Pron message types.
 */
enum MessageType {
  /* Requests */
  RQ_HELLO                    = (RQ_PREFIX |  1),
  RQ_CREATE_WINDOW            = (RQ_PREFIX |  2),
  RQ_CLEAR_WINDOW             = (RQ_PREFIX |  3),
  RQ_MAP_WINDOW               = (RQ_PREFIX |  4),
  RQ_CREATE_GC                = (RQ_PREFIX |  5),
  RQ_DRAW_LINE                = (RQ_PREFIX |  6),
  RQ_DRAW_RECT                = (RQ_PREFIX |  7),
  RQ_FILL_RECTANGLE           = (RQ_PREFIX |  8),
  RQ_DRAW_ELLIPSE             = (RQ_PREFIX |  9),
  RQ_FILL_ELLIPSE             = (RQ_PREFIX | 10),
  RQ_GET_WINDOW_ATTRIBUTES    = (RQ_PREFIX | 11),
  RQ_SET_WINDOW_ATTRIBUTES    = (RQ_PREFIX | 12),
  RQ_SELECT_INPUT             = (RQ_PREFIX | 13),
  RQ_RAISE_WINDOW             = (RQ_PREFIX | 14),
  RQ_REPARENT                 = (RQ_PREFIX | 15),
  RQ_UNMAP_WINDOW             = (RQ_PREFIX | 16),
  RQ_DRAW_POINT               = (RQ_PREFIX | 17),
  RQ_DESTROY_WINDOW           = (RQ_PREFIX | 18),
  RQ_MOVE_WINDOW              = (RQ_PREFIX | 19),
  RQ_MOVE_WINDOW_TO           = (RQ_PREFIX | 20),
  RQ_DONT_PROPAGATE           = (RQ_PREFIX | 21),
  RQ_GET_GC_VALUES            = (RQ_PREFIX | 22),
  RQ_CHANGE_GC                = (RQ_PREFIX | 23),
  RQ_FREE_GC                  = (RQ_PREFIX | 24),
  RQ_RESIZE_WINDOW            = (RQ_PREFIX | 25),
  RQ_GOODBYE                  = (RQ_PREFIX | 26),
  RQ_PUT_IMAGE                = (RQ_PREFIX | 27),
  RQ_CREATE_PIXMAP            = (RQ_PREFIX | 28),
  RQ_FREE_PIXMAP              = (RQ_PREFIX | 29),
  RQ_COPY_AREA                = (RQ_PREFIX | 30),
  RQ_DRAW_TEXT                = (RQ_PREFIX | 31),
  RQ_TEXT_SIZE                = (RQ_PREFIX | 32),
  RQ_CREATE_WINDOW_EXT        = (RQ_PREFIX | 33),
  RQ_SET_CURSOR               = (RQ_PREFIX | 34),

  /* Responses */
  RS_WELCOME                  = (RS_PREFIX |  1),
  RS_WINDOW_ATTRIBUTES        = (RS_PREFIX |  2),
  RS_GC_VALUES                = (RS_PREFIX |  3),
  RS_TEXT_SIZE                = (RS_PREFIX |  4),

  /* Events */
  EV_WINDOW_CREATED           = (EV_PREFIX |  1),
  EV_POINTER_MOVED            = (EV_PREFIX |  2),
  EV_EXPOSE                   = (EV_PREFIX |  3),
  EV_KEY_PRESSED              = (EV_PREFIX |  4),
  EV_KEY_RELEASED             = (EV_PREFIX |  5),
  EV_MOUSE_BUTTON             = (EV_PREFIX |  6),
  EV_DESTROY_WINDOW           = (EV_PREFIX |  7),
  EV_RESIZE_WINDOW            = (EV_PREFIX |  8),

  /* Errors */
};

/**
 * Base class representing a pron message.
 */
struct PronMessage {
  /**
   * Constructor.
   * @param type The message type
   */
  PronMessage(MessageType type) {
    this->type = type;
  }

  MessageType type; /**< Message type */
};

} // namespace pron

#endif // __PRONPROTO_BASE_H__
