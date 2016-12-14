#include <stdio.h>
#include <stdlib.h>
#include "read_ppm.h"

int read_ppm(const char *filename, Image *image) {
  FILE *fd;
  int  k, i, j, n, m, size;
  char c, str[100];
  int red, green, blue;

  fd = fopen(filename, "r");
  if (fd == NULL) {
    fprintf(stderr, "No such file %s\n", filename); 
    return FALSE;
  }
  fscanf(fd,"%[^\n] ",str);
  if(str[0]!='P'|| str[1] != '3') {
    printf("%s is not a P3 PPM file!\n", filename); 
    return FALSE;
  }
  //printf("%s is a P3 PPM file\n",filename);
  fscanf(fd, "%c",&c);
  while(c == '#') {
    fscanf(fd, "%[^\n] ", str);
    printf("%s\n",str);
    fscanf(fd, "%c",&c);
  }
  ungetc(c,fd); 
  fscanf(fd, "%d %d %d", &n, &m, &k);
  
  //printf("%d rows  %d columns 3 colors max value= %d\n",n,m,k);
  image->sizeX = n;
  image->sizeY = m;
  image->n = 3;
  size = image->sizeX * image->sizeY * 3;
  
  image->data = (unsigned char *) malloc(size);
  if (image->data == NULL) {
    printf("Error allocating memory for color-corrected image data");
    return FALSE;       
  }

  for(i=m-1;i>=0;i--) {
    for (j=0; j<n; j++) {
      fscanf(fd,"%d %d %d",&red, &green, &blue );
      if (k > 255) { 
        image->data[i*n*3+j*3]=(char) red*255.0/k;
        image->data[i*n*3+j*3+1]=(char) green*255.0/k;
        image->data[i*n*3+j*3+2]=(char) blue*255.0/k;
      }
      else {
        image->data[i*n*3+j*3]=(char) red;
        image->data[i*n*3+j*3+1]=(char) green;
        image->data[i*n*3+j*3+2]=(char) blue;
      }
    }
  }
  //printf("read image\n");
  return TRUE;
}

int read_ppm_bin(const char *filename, Image *image) {
  FILE *fd;
  int  k, n, m, nm;
  float s;
  char c;
  int i;
  char b[100];
  char *tmp;
        
  fd = fopen(filename, "rb");   // flag b may be not necessary on Unix
  if (fd == NULL) {
    fprintf(stderr, "No such file %s\n", b); 
    return FALSE;
  }
  fscanf(fd,"%[^\n] ", b);
  if(b[0]!='P'|| b[1] != '6') { // P6 is binary encoded, P3 is ASCII
    printf("%s is not a P6 PPM file!\n", b); 
    return FALSE;
  }
  printf("%s is a P6 PPM file\n",b);
  fscanf(fd, "%c",&c);
  while(c == '#') {
    fscanf(fd, "%[^\n] ", b);
    printf("%s\n",b);
    fscanf(fd, "%c",&c);
  }
  ungetc(c,fd); 
  fscanf(fd, "%d %d %d ", &n, &m, &k);
  printf("%d rows  %d columns 3 colors max value= %d\n",n,m,k);
  nm = n*m;
  image->sizeX = n;
  image->sizeY = m;
  image->n = 3;
  image->data= (unsigned char *) malloc(3 * sizeof(char) * nm);
  s = 255.0/k;

  tmp= (char *) malloc(3 * sizeof(char) * nm);
  fread(tmp, sizeof(char), 3 * nm, fd);
        
  // The image in tmp is not in the correct order...
  //
  for(i=0;i<nm;i++) {
    if (k>225) {
      image->data[3*nm-3*i-3]=tmp[0]*s;
      image->data[3*nm-3*i-2]=tmp[1]*s;
      image->data[3*nm-3*i-1]=tmp[2]*s;
    }
    else {
      image->data[3*nm-3*i-3]=tmp[0];
      image->data[3*nm-3*i-2]=tmp[1];
      image->data[3*nm-3*i-1]=tmp[2];
    }
      tmp+=3;
  }
  tmp-=3*nm;
  free(tmp);
  
  printf("read image\n");
  return TRUE;
}
