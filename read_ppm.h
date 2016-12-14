#ifndef __READ_PPM_H__
#define __READ_PPM_H__

#define FALSE		0
#define TRUE		1

/* Image type - contains height, width, number of color components and data */
struct _image {
  int sizeX;
  int sizeY;
  int n;
  unsigned char *data;
};
typedef struct _image Image;

int read_ppm(const char *filename, Image *image);
int read_ppm_bin(const char *filename, Image *image);

#endif 
