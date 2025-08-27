

#ifndef __ATLAS_H
#define __ATLAS_H

#include "image.h"

typedef struct Atlas Atlas;

Atlas* init_atlas(int width, int height, int channel);
void free_atlas(Atlas* atlas);

int atlas_resize(Atlas* atlas, int new_width, int new_height);

Image* atlas_get_image(Atlas* atlas);

int atlas_append_file(Atlas* atlas, const char* fname, int *i_x, int *i_y, int *i_width, int *i_height);
int atlas_append_image(Atlas* atlas, Image* image, int *i_x, int *i_y);

#endif