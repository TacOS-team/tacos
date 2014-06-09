#ifndef __FONT_H__
#define __FONT_H__

/**
 * @file font.h
 * Font class definition.
 */

#include <drawable.h>

/**
 * Font.
 * Describes a font loaded by the server.
 */
class Font {
public:
	static const int MAX_HEIGHT = 16; /**< Maximum height of the font, in pixels */
	
  /**
   * Describes a glyph of the font.
   */
	struct glyph_t {
		int width; /**< Width of the glyph */
		unsigned short bitmap[MAX_HEIGHT]; /**< Bitmap describing the glyph (one unsighed short = 16 bits per line) */
	};

  /**
   * Constructor.
   * @param name The name of the font
   * @param height The height of the font
   * @param glyphs The glyphs of the font (256 max)
   */
	Font(char *name, int height, glyph_t glyphs[256]);

  /**
   * Gets the width and the height of a string.
   * @param text The text to get the size
   * @param length The length of the text
   * @param width The width of the text (output)
   * @param height The height of the text (output)
   */
  void textSize(const char *text, int length, int *width, int *height);

  /**
   * Draws the given character.
   * @param d The drawable in which to draw the character
   * @param x The x-coordinate of the origin of the character
   * @param y The y-coordinate of the origin of the character
   * @param car The character to draw
   */

	void drawChar(Drawable *d, int x, int y, unsigned char car);

  /**
   * Draws the given text.
   * @param d The drawable in which to draw the text
   * @param x The x-coordinate of the origin of the first character
   * @param y The y-coordinate of the origin of the first character
   * @param text The text to draw
   * @param length The length of the text
   * @param hpos The horizontal position of the text
   * @param vpos The vertical position of the text
   */
	void drawText(Drawable *d, int x, int y, const char *text, int length,
      HPosition hpos = LEFT, VPosition vpos = MIDDLE);

  /**
   * Prints informations about the font (for debugging purposes).
   */
	void printInfo();

private:
	char *name; /**< Font name */ 
	int height; /**< Font height */ 
	glyph_t glyphs[256]; /**< Font glyphs (max 256) */
};

#endif
