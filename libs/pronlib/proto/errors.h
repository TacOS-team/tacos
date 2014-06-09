#ifndef __PRONPROTO_ERRORS_H__
#define __PRONPROTO_ERRORS_H__

#include <proto/base.h>

namespace pron {

/**
 * Base class representing a pron error message.
 */
struct PronError : public PronMessage {
  /**
   * Constructor.
   * @param type The message type
   */
  PronError(MessageType type)
      : PronMessage(type) {
  }
};

} // namespace pron

#endif // __PRONPROTO_ERRORS_H__
