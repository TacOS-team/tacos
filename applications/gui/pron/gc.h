#ifndef __GC_H__
#define __GC_H__

/**
 * @file pron/gc.h
 * GC class definition.
 */

#include <proto/bits/gc.h>
#include <vector>

using namespace pron;

/**
 * Graphics context.
 * @todo XXX public members bourrin
 */
class GC {
 private:
  Color bg; /**< Background color */
  Color fg; /**< Foreground color */

 public:
  unsigned int id; /**< GC id */
  unsigned int bgValue; /**< Value of the background color for the screen depth */
  unsigned int fgValue; /**< Value of the foreground color for the screen depth */
  int font_num; /**< Id of the current font */

  /**
   * Returns the queried graphics context.
   * @param id The graphics context to get
   * @return the queried graphics context
   */
  static GC* getGC(unsigned int id);

  /**
   * Default constructor.
   * Creates a graphics context with default values.
   */
  GC();

  /**
   * Constructor.
   * @param id The id of the graphics context
   * @param values The values to set in the new GC
   * @param mask The mask that specifies the values to be set
   */
  GC(unsigned int id, const PronGCValues &values, unsigned int mask);

  /**
   * Returns the values of the graphics context.
   * @return the values of the graphics context
   */
  PronGCValues getValues();

  /** 
   * Sets the values of the graphics context.
   * @param values The values to set
   * @param mask The mask that specifies the values to be set
   */
  void setValues(const PronGCValues &values, unsigned int mask);

  /**
   * Sets the background color and computes its value
   * according to the screen depth.
   * @param c The color to set
   */
  void setBg(Color c);
  
  /**
   * Sets the foreground color and computes its value
   * according to the screen depth.
   * @param c The color to set
   */
  void setFg(Color c);
  
  /**
   * Returns the background color.
   * @return The background color
   */
  Color getBg() {
    return this->bg;
  }
  
  /**
   * Returns the foreground color.
   * @return The foreground color
   */
  Color getFg() {
    return this->fg;
  }

  /**
   * Destroys the graphics context.
   */
  void destroy();

 private:
  static std::vector<GC*> gcs; /**< List of all graphics contexts */
};

#endif
