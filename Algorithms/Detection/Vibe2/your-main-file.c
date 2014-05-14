/* 
   January 2010
   Olivier Barnich <o.barnich@ulg.ac.be> and
   Marc Van Droogenbroeck <m.vandroogenbroeck@ulg.ac.be>
*/

/*
  This file contains an example of a main functions that uses the ViBe algorithm
  implemented in vibe-background.{o, h}. You should read vibe-background.h for
  more information.

  vibe-background.o was compiled by gcc 4.0.3 using the following command
  $> gcc -ansi -Wall -Werror -pedantic -c vibe-background.c

  This file can be compiled using the following command
  $> gcc -o main -std=c99 your-main-file.c vibe-background.o
  
*/

#include "vibe-background.h"

static int32_t get_image_width(void *stream)
{
  /* put your own code here */
  return(640);
}

static int32_t get_image_height(void *stream)
{
  /* put your own code here */
  return(480);
}

static int32_t get_image_stride(void *stream)
{
  /* put your own code here */
  return(640);
}

static int32_t *acquire_grayscale_image(void *stream, uint8_t *image_data)
{
  /* put your own code here */
  return(0);
}

static int32_t finished(void *stream){
  /* put your own code here */
  static int32_t counter = 0;
  if (counter++ < 80)
    return(0);
  else
    return(1);
}

int main(int argc, char **argv)
{
  /* Your video stream */
  void *stream = NULL;
  
  /* Get a model data structure */
  vibeModel_t *model = libvibeModelNew();

  /* Get the dimensions of the images of your stream 
     nb: stride is te number of bytes from the start of one row of the image  
     to the start of the next row. */
  int32_t width = get_image_width(stream);
  int32_t height = get_image_height(stream);
  int32_t stride = get_image_stride(stream);

  /* Allocates memory to store the input images and the segmentation maps */
  uint8_t *segmentation_map = (uint8_t*) malloc(stride * height);
  uint8_t *image_data = (uint8_t*) malloc(stride * height);
  
  /* Acquires your first image */
  acquire_grayscale_image(stream, image_data);
    
  /* Allocates the model and initialize it with the first image */
  libvibeModelAllocInit_8u_C1R(model, image_data, width, height, stride);
  
  /* Processes all the following frames of your stream:
     results are stored in "segmentation_map" */
  while(!finished(stream)){
    fprintf(stderr, ".");
    acquire_grayscale_image(stream, image_data);
    libvibeModelUpdate_8u_C1R(model, image_data, segmentation_map);
  }
  fprintf(stderr, "\n");
 
  /* Cleanup allocated memory */
  libvibeModelFree(model);
  free(image_data);
  free(segmentation_map);
  
  return(0);
}
