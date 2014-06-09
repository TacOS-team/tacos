#ifndef __COLOR_H__
#define __COLOR_H__
/**
 * @file color.h
 * Color class definition
 */

/**
 * Color class definition
 * @brief The Color
 */
class Color {
 private:
  float r; /**< Red component */
  float g; /**< Green component */
  float b; /**< Blue component */
 public:
  /**
   * Color contructor
   * @param r red component between 0. and 1.
   * @param g green component between 0. and 1.
   * @param b blue component between 0. and 1.
   */
  Color(float r = 0., float g = 0., float b = 0.);
  /**
   * Color RGB contructor
   * @param r red component from 0 to 255
   * @param g green component from 0 to 255
   * @param b blue component from 0 to 255
   */
  Color(int r, int g, int b);
  /**
   * Get red
   * @return component between 0. and 1.
   */
  float getR();
  /**
   * Get green
   * @return component between 0. and 1.
   */
  float getG();
  /**
   * Get blue
   * @return component between 0. and 1.
   */
  float getB();
  /**
   * Set red
   * @param component between 0. and 1.
   */
  void setR(float r);
    /**
   * Set green
   * @param component between 0. and 1.
   */
  void setG(float g);
  /**
   * Set blue
   * @param component between 0. and 1.
   */
  void setB(float b);
  /**
   * Get RGB 24
   * @return The RGB pixel with 24 bits depth
   */
  int getRGB24();
  /**
   * Get RGB
   * @param The pixel depth
   * @return The RGB pixel with 24 bits depth
   */
  int getRGB(int depth);
};

#endif//__COLOR_H__
