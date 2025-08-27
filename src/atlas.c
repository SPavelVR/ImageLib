
#include <stdlib.h>

#include "atlas.h"

#define DIRACTION_NONE 0
#define DIRACTION_RIGHT 1
#define DIRACTION_DOWN 2

// Стурктура для хронения линии записи
typedef struct AtlasWriter {
    int upLine, rightLine, downLine, leftLine, endLine;
    int diraction;
} AtlasWriter;

typedef struct Atlas
{
    Image* atl_image;
    AtlasWriter* writers;
    int countAWriters;
} Atlas;

//      SUB FUNCTION

int push_atlas_writer(Atlas* atlas, AtlasWriter aw)
{
    if (atlas->writers == NULL)
    {
        atlas->writers = calloc(1, sizeof(AtlasWriter));

        if (atlas->writers == NULL) return 1;

        atlas->writers[0] = aw;
        atlas->countAWriters = 1;
    }
    else
    {
        AtlasWriter* aw_arr = realloc(atlas->writers, (atlas->countAWriters + 1) * sizeof(AtlasWriter));

        if (aw_arr == NULL) return 1;

        atlas->writers[atlas->countAWriters] = aw;
        atlas->countAWriters++;
    }

    return 0;
}

int case_atlas_write(AtlasWriter aw, int w, int h)
{
    switch (aw.diraction)
    {
    case DIRACTION_RIGHT:
        if (aw.rightLine + w <= aw.endLine && aw.downLine - aw.upLine >= h) return 1;
        break;
    case DIRACTION_DOWN:
        if (aw.downLine + h <= aw.endLine && aw.rightLine - aw.leftLine >= w) return 1;
        break;
    default:
        return 0;
        break;
    }
    return 0;
}

void shift_atlas_write(AtlasWriter* aw_arr, int index, int w, int h)
{
    if (aw_arr == NULL) return  ;

    switch (aw_arr[index].diraction)
    {
    case DIRACTION_RIGHT:
        aw_arr[index].rightLine += w;
        break;
    case DIRACTION_DOWN:
        aw_arr[index].downLine += h;
        break;
    default:
        return ;
        break;
    }
}

int case_get_start_pixel_y(AtlasWriter aw)
{
    switch (aw.diraction)
    {
    case DIRACTION_RIGHT:
        return aw.upLine;
        break;
    case DIRACTION_DOWN:
        return aw.downLine;
        break;
    default:
        return 0;
        break;
    }
}

int case_get_start_pixel_x(AtlasWriter aw)
{
    switch (aw.diraction)
    {
    case DIRACTION_RIGHT:
        return aw.rightLine;
        break;
    case DIRACTION_DOWN:
        return aw.leftLine;
        break;
    default:
        return 0;
        break;
    }
}

int find_atlas_writer(AtlasWriter* aw_arr, int countAW, int w, int h)
{
    if (aw_arr == NULL) return -1;

    int min_w = -1, min_h = -1, res = -1;

    for (int i = 0; i < countAW; i++)
    {
        AtlasWriter aw = aw_arr[i];

        if (case_atlas_write(aw, w, h))
        {
            if (min_w == -1 || min_h == -1)
            {
                min_w = aw.rightLine - aw.leftLine;
                min_h = aw.downLine - aw.upLine;
                res = i;
            }

            if (((aw.rightLine - aw.leftLine) <= min_w && (aw.downLine - aw.upLine) <= min_h) || (aw.downLine - aw.upLine) < min_h)
            {
                min_w = ((aw.rightLine - aw.leftLine) < min_w) ? (aw.rightLine - aw.leftLine) : min_w;
                min_h = ((aw.downLine - aw.upLine) < min_h) ? (aw.downLine - aw.upLine) : min_h;
                res = i;
            }
        }
    }

    return res;
}

int creat_writer_atlas(Atlas* atlas, int w, int h)
{
    if (atlas == NULL || atlas->atl_image->data == NULL) return 1;

    int dL = 0;

    for (int i = 0; i < atlas->countAWriters; i++)
    {
        if (atlas->writers[i].downLine > dL) dL = atlas->writers[i].downLine;
    }

    if (dL + h > atlas->atl_image->height) return 1;

    AtlasWriter aw = {
        dL, 0, dL + h, 0, atlas->atl_image->width, DIRACTION_RIGHT
    };

    if (push_atlas_writer(atlas, aw)) return 1;

    return 0;
}

int creat_sub_write_atlas(Atlas* atlas, int index, int w, int h)
{
    if (atlas == NULL || atlas->writers == NULL) return 1;

    AtlasWriter aw = atlas->writers[index];

    if (aw.diraction == DIRACTION_DOWN || aw.diraction == DIRACTION_NONE) return 1;

    if ((aw.downLine - aw.upLine) < h * 2) return 1;
    
    AtlasWriter new_aw = {
        aw.upLine + h, aw.rightLine + w, aw.upLine + h, aw.rightLine, aw.downLine, DIRACTION_DOWN
    };

    push_atlas_writer(atlas, new_aw);
}

// END SUB FUNCTION

Atlas* init_atlas(int width, int height, int channel)
{
    Atlas* atlas = calloc(1, sizeof(Atlas));

    if (atlas == NULL) return   NULL;

    Image* image = calloc(1, sizeof(Image));

    if (image == NULL)
    {
        free(atlas);
        return NULL;
    }

    unsigned char* img = calloc(width * height * channel, sizeof(char));

    if (img == NULL)
    {
        free_image(image);
        free(atlas);
        return NULL;
    }

    image->data = img;
    image->width = width;
    image->height = height;
    image->channel = channel;

    atlas->atl_image = image;

    return atlas;
}

void free_atlas(Atlas* atlas)
{
    if (atlas == NULL) return   ;

    if (atlas->atl_image != NULL) free_image(atlas->atl_image);

    atlas->atl_image = NULL;

    free(atlas);
}

int atlas_resize(Atlas* atlas, int new_width, int new_height)
{
    
}

Image* atlas_get_image(Atlas* atlas)
{
    if (atlas == NULL) return NULL;
    return atlas->atl_image;
}

int atlas_append_file(Atlas* atlas, const char* fname, int *i_x, int *i_y, int *i_width, int *i_height)
{
    if (atlas == NULL) return IMAGE_NONE_INIT;

    Image* image = read_image_file(fname);

    if (image == NULL) return LOAD_IMAGE_NO_FIND_FILE;
    if (image->channel != atlas->atl_image->channel) return COPY_IMAGE_DIFFERENT_CHANNELS;


    if (i_width != NULL) *i_width = image->width;
    if (i_height != NULL) *i_height = image->height;

    int res = atlas_append_image(atlas, image, i_x, i_y);

    free_image(image);

    return res;
}

int atlas_append_image(Atlas* atlas, Image* image, int *i_x, int *i_y)
{
    if (atlas == NULL || image == NULL) return IMAGE_NONE_INIT;

    if (atlas->atl_image->channel != image->channel) return COPY_IMAGE_DIFFERENT_CHANNELS;

    int index = find_atlas_writer(atlas->writers, atlas->countAWriters, image->width, image->height);

    if (index == -1 && creat_writer_atlas(atlas, image->width, image->height))
    {
        return ATLAS_NO_PAST;
    }
    else if (index == -1) 
    {
        index = atlas->countAWriters - 1;
    }

    AtlasWriter aw = atlas->writers[index];

    int res = write_image_in_image(atlas->atl_image, image, case_get_start_pixel_x(aw), case_get_start_pixel_y(aw), 0, 0, image->width, image->height);

    if (res != IMAGE_SUCCESS) return res;

    if (i_x != NULL) *i_x = case_get_start_pixel_x(aw);
    if (i_y != NULL) *i_y = case_get_start_pixel_y(aw);

    creat_sub_write_atlas(atlas, index, image->width, image->height);

    shift_atlas_write(atlas->writers, index, image->width, image->height);

    return res;
}
