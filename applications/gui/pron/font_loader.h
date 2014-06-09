#ifndef __FONT_LOADER_H__
#define __FONT_LOADER_H__

/**
 * @file font_loader.h
 * FontLoader class definition.
 */

#include <font.h>
#include <stdio.h>

/**
 * Font loader.
 * Reads a BDF font description file and returns a Font class.
 */
class FontLoader {
public:
  /**
   * Loads the queried font.
   * @param fileName The name of the BDF file describing the font
   * @return the loaded font
   */
	static Font* load(char *fileName);

private:
  /**
   * Reads the "PROPERTIES" section of the BDF file.
   * @param nb The number of properties to read
   */
	static void readProperties(int nb);

  /**
   * Reads the "CHARS" section of the BDF file.
   * @param nb The number of characters to read
   */
  static void readChars(int nb);

  /**
   * Returns true if the two strings are equal.
   * @param a The first string to compare
   * @param b The second string to compare
   * @return true if the two strings are equal
   */
	static bool eq(const char *a, const char *b);

	static FILE *fd; /**< File descriptor used to read the BDF file */
	static char kw[100]; /**< Buffer used to store the currently read keyword */
	static char fontName[256]; /**< Buffer used to store the font name read */
	static Font::glyph_t fontGlyphs[256]; /**< Buffer used to store the glyphs read */
	static int pixelSize; /**< Pixel size */
  static int fontAscent; /**< Font ascent (number of lines above baseline) */
  static int fontDescent; /**< Font descent (number of lines below baseline) */
  static int fontHeight; /**< Font height */
  static int defaultChar; /**< Default character */
};

#endif
