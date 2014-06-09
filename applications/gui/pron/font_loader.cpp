/**
 * @file font_loader.cpp
 * FontLoader class implementation.
 */

#include <font.h>
#include <font_loader.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE* FontLoader::fd;
char FontLoader::kw[100];
char FontLoader::fontName[256];
Font::glyph_t FontLoader::fontGlyphs[256];
int FontLoader::pixelSize, FontLoader::fontAscent, FontLoader::fontDescent, FontLoader::fontHeight, FontLoader::defaultChar;

bool FontLoader::eq(const char *a, const char *b) {
  return (strcmp(a, b) == 0);
}

void FontLoader::readProperties(int nb) {
  for (int i = 0; i < nb; i++) {
    fscanf(fd, "%s", kw);
    //fprintf(stderr, "Read %s\n", kw);
    if (eq(kw, "PIXEL_SIZE")) {
      fscanf(fd, "%d", &pixelSize);
    } else if (eq(kw, "FONT_ASCENT")) {
      fscanf(fd, "%d", &fontAscent);
    } else if (eq(kw, "FONT_DESCENT")) {
      fscanf(fd, "%d", &fontDescent);
    } else if (eq(kw, "DEFAULT_CHAR")) {
      fscanf(fd, "%d", &defaultChar);
    } else {
      //fprintf(stderr, "Ignored keyword: %s\n", kw);
      while (fgetc(fd) != '\n');
    }
  }

  fontHeight = fontAscent + fontDescent;

  fscanf(fd, "%s", kw);
  if (!eq(kw, "ENDPROPERTIES")) {
    fprintf(stderr, "Fatal: expected ENDPROPERTIES, got %s.", kw);
    exit(2);
  }
}

void FontLoader::readChars(int nb) {
  memset(fontGlyphs, 0, sizeof(fontGlyphs));

  for (int i = 0; i < nb; i++) {
    char glyphName[100];
    fscanf(fd, "%s %s", kw, glyphName);
    if (!eq(kw, "STARTCHAR")) {
      fprintf(stderr, "Fatal: no STARTCHAR found.\n");
      exit(2);
    }
    bool end = false;
    int encoding = -1, width = -1, bbw = -1, bbh = -1, bbx = 0, bby = 0, line, unused;
    char line_str[8 + 1];
    while (!end) {
      fscanf(fd, "%s", kw);
      if (eq(kw, "ENCODING")) {
        fscanf(fd, "%d", &encoding);
      } else if (eq(kw, "DWIDTH")) {
        fscanf(fd, "%d %d", &width, &unused);
      } else if (eq(kw, "BBX")) {
        fscanf(fd, "%d %d %d %d", &bbw, &bbh, &bbx, &bby);
      } else if (eq(kw, "BITMAP")) {
        if (encoding == -1) {
          //fprintf(stderr, "No encoding set - skipping character.\n");
          continue;
        } else if (bbw == -1 || bbh == -1) {
          //fprintf(stderr, "No bounding box set - skipping character.\n");
          continue;
        } else if (width == -1) {
          //fprintf(stderr, "No width set - skipping character.\n");
          continue;
        }

        width -= bbx;
        fontGlyphs[encoding].width = width;

        if (bbx < 0) {
          bbx = 0;
        }

        for (int i = 0; i < bbh; i++) {
          /*fscanf(fd, "%x", &line);
          int nbDigits = 0;
          while (line >> (4 * (++nbDigits)));*/

          fscanf(fd, "%s", line_str);
          int nbDigits = strlen(line_str);
          line = strtol(line_str, NULL, 16); 

          int idx = fontHeight - fontDescent - bbh - bby + i;
          if (idx < 0) {
            idx += fontHeight;
          }
          fontGlyphs[encoding].bitmap[idx] = line << (16 - 4 * nbDigits - bbx);
        }
      } else if (eq(kw, "ENDCHAR")) {
        end = true;
      } else {
        //fprintf(stderr, "Ignored keyword: %s\n", kw);
        while (fgetc(fd) != '\n');
      }
    }
  }

  for (int c = 0; c < 256; c++) {
    if (fontGlyphs[c].width == 0) {
      memcpy(&fontGlyphs[c], &fontGlyphs[defaultChar], sizeof(fontGlyphs[defaultChar]));
    }
  }
}

Font* FontLoader::load(char *fileName) {
  Font *font = NULL;
  
  fd = fopen(fileName, "r");
  if (fd == NULL) {
    fprintf(stderr, "Unable to open file.\n");
  } else {
    char version[10];
    if (fscanf(fd, "STARTFONT %s", version) <= 0) {
      fprintf(stderr, "Fatal: no STARTFONT found.\n");
    } else {
      printf("Reading font v%s\n", version);
      bool end = false;
      while (!end) {
        fscanf(fd, "%s", kw);
        if (eq(kw, "FONT")) {
          fscanf(fd, "%s", fontName);
        } else if (eq(kw, "STARTPROPERTIES")) {
          int nbProperties;
          fscanf(fd, "%d", &nbProperties);
          readProperties(nbProperties);
        } else if (eq(kw, "CHARS")) {
          int nbChars;
          fscanf(fd, "%d", &nbChars);
          readChars(nbChars);
        } else if (eq(kw, "ENDFONT")) {
          end = true;
        } else {
          //fprintf(stderr, "Ignored keyword: %s\n", kw);
          while (fgetc(fd) != '\n');
        }
      }

      font = new Font(fontName, fontHeight, fontGlyphs);
    }
  }

  return font;
}
