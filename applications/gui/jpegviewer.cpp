/**
 * @file jpegviewer.cpp
 * Sample JPEG viewer application. Tests image sending with the pronlib.
 * @see http://www.cim.mcgill.ca/~junaed/libjpeg.php
 */

#include <stdio.h>
#include <jpeglib.h>
#include <stdlib.h>
#include <pronlib.h>

using namespace pron;

/* we will be using this uninitialized pointer later to store raw, uncompressd image */
char *raw_image = NULL;

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s filename.jpg\n", argv[0]);
    exit(1);
  }

  char *filename = argv[1];

  /* these are standard libjpeg structures for reading (decompression) */
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  /* libjpeg data structure for storing one row, that is, scanline of an image */
  JSAMPROW row_pointer[1];

  FILE *infile = fopen(filename, "rb");
  unsigned long location = 0;

  if (infile == NULL) {
    fprintf(stderr, "Error opening jpeg file %s.\n", filename);
    exit(2);
  }

  /* here we set up the standard libjpeg error handler */
  cinfo.err = jpeg_std_error(&jerr);
  /* setup decompression process and source, then read JPEG header */
  jpeg_create_decompress(&cinfo);
  /* this makes the library read from infile */
  jpeg_stdio_src(&cinfo, infile);
  /* reading the image header which contains image information */
  jpeg_read_header(&cinfo, TRUE);

  /* Uncomment the following to output image information, if needed. */
  //*--
  printf("JPEG File Information: \n");
  printf("Image width and height: %d pixels and %d pixels.\n", cinfo.image_width, cinfo.image_height);
  printf("Color components per pixel: %d.\n", cinfo.num_components);
  printf("Color space: %d.\n", cinfo.jpeg_color_space);
  //--*/
  
  int width = cinfo.image_width;
  int height = cinfo.image_height;

  /* Start decompression jpeg here */
  jpeg_start_decompress(&cinfo);

  /* allocate memory to hold the uncompressed image */
  raw_image = (char*) malloc(cinfo.output_width * cinfo.output_height * cinfo.num_components);
  /* now actually read the jpeg into the raw buffer */
  row_pointer[0] = (unsigned char *) malloc(cinfo.output_width * cinfo.num_components);
  /* read one scan line at a time */
  while (cinfo.output_scanline < cinfo.image_height) {
    jpeg_read_scanlines(&cinfo, row_pointer, 1);
    for (unsigned int i = 0; i < cinfo.image_width * cinfo.num_components; i += cinfo.num_components) {
      //for (unsigned int j = 0; j < cinfo.num_components; j++) {
      for (int j = cinfo.num_components - 1; j >= 0; j--) {
        raw_image[location++] = row_pointer[0][i + j];
      }
    }
  }

  /* wrap up decompression, destroy objects, free pointers and close open files */
  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);
  free(row_pointer[0]);
  fclose(infile);
  
  /* Connect to pron */
  Display *d = pronConnect();
  if (d == NULL) {
    fprintf(stderr, "Unable to connect to pron.\n");
    exit(1);
  }

  /* Create window */
  Window w = pronCreateWindow(d, d->rootWindow, 0, 0, width, height);
  
  /* Subscribe to events */
  pronSelectInput(d, w, PRON_EVENTMASK(EV_EXPOSE));
  
  /* Map the window */
  pronMapWindow(d, w);

  /* Create PronImage */
  PronImage image(width, height, ZPixmap, raw_image, 24, 3);

  PronEvent *e = getPronEvent();

  while (1) {
    if (!pronNextEvent(d, e)) {
      fprintf(stderr, "pron has closed the connection.\n");
      exit(1);
    } else {
      switch (e->type) {
        case EV_EXPOSE:
          pronPutImage(d, w, d->defaultGC, &image, 0, 0, width, height, 0, 0);
          break;
        default:
          break;
      }
    }
  }

  pronDisconnect(d);

  /* yup, we succeeded! */
  return 0;
}
