#ifndef __PRONPROTO_RESPONSES_H__
#define __PRONPROTO_RESPONSES_H__

#include <proto/base.h>

namespace pron {

/**
 * Base class representing a pron response message.
 */
struct PronResponse : public PronMessage {
  /**
   * Constructor.
   * @param type The message type
   */
  PronResponse(MessageType type)
      : PronMessage(type) {}
};

/**
 * Welcome response.
 * Sent by the server in response to a Hello request.
 */
struct RespWelcome : public PronResponse {
  /**
   * Constructor.
   * @param rootWindow The id of the root window
   * @param startId The first usable resource id
   * @param endId The last usable resource id
   */
  RespWelcome(int rootWindow, int startId, int endId)
      : PronResponse(RS_WELCOME) {
    this->rootWindow = rootWindow;
    this->startId = startId;
    this->endId = endId;
  }

  int rootWindow; /**< Id of the root window */
  int startId; /**< First usable resource id */
  int endId; /**< Last usable resource id */
};

/**
 * WindowAttributes response.
 * Sent by the server in response to a GetWindowAttributes request.
 */
struct RespWindowAttributes : public PronResponse {
  /**
   * Constructor.
   * @param attributes The attributes of the requested window
   */
  RespWindowAttributes(const PronWindowAttributes &attributes)
      : PronResponse(RS_WINDOW_ATTRIBUTES) {
    this->attributes = attributes;
  }

  PronWindowAttributes attributes; /**< Attributes of the requested window */
};

/**
 * GCValues response.
 * Sent by the server in response to a GetGCValues request.
 */
struct RespGCValues : public PronResponse {
  /**
   * Constructor.
   * @param values The values of the requested graphics context
   */
  RespGCValues(const PronGCValues &values)
      : PronResponse(RS_GC_VALUES) {
    this->values = values;
  }

  PronGCValues values; /**< Values of the requested graphics context */
};

/**
 * TextSize response.
 * Sent by the server in response to a TextSize request.
 */
struct RespTextSize : public PronResponse {
  /**
   * Constructor.
   * @param width The width of the text
   * @param height The height of the text
   */
  RespTextSize(int &width, int &height)
      : PronResponse(RS_TEXT_SIZE) {
    this->width = width;
    this->height = height;
  }

  int width; /**< Text width */
  int height; /**< Text height */
};

} // namespace pron

#endif // __PRONPROTO_RESPONSES_H__
