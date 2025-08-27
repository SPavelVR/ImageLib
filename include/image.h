
#ifndef __IMAGE_H
#define __IMAGE_H

#include "defineImage.h"

typedef struct Image
{
    unsigned char* data;
    int width, height, channel;
} Image;

Image* read_image_file(const char* fname);
void free_image(Image* image);

int write_image(Image* image, const char* fname);
int write_image_in_image(Image* dst, Image* src,    int posDstX, int posDstY,
                                                    int posSrcX, int posSrcY,
                                                    int copyW, int copyH);

#endif