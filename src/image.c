
#include <stdlib.h>
#include <string.h>

#include "wrapSTB.h"
#include "image.h"


Image* read_image_file(const char* fname)
{
    int t_width, t_height, t_channel;
    unsigned char* img = stbi_load(fname, &t_width, &t_height, &t_channel, 0);

    if (img == NULL) return NULL;

    Image* image = calloc(1, sizeof(Image));

    if (image == NULL)
    {
        free(img);
        return NULL;
    }

    image->width = t_width;
    image->height = t_height;
    image->channel = t_channel;
    image->data = img;

    return image;
}

void free_image(Image* image)
{
    if (image == NULL) return   ;

    if (image->data != NULL) free(image->data);

    free(image);
}

int write_image(Image* image, const char* fname)
{
    if (image == NULL) return IMAGE_NONE_INIT;

    if (!stbi_write_png(fname, image->width, image->height, image->channel, image->data, image->width * image->channel)) return WRITE_IMAGE_NO_WRITE;

    return IMAGE_SUCCESS;
}

int write_image_in_image(Image* dst, Image* src,    int posDstX, int posDstY,
                                                    int posSrcX, int posSrcY,
                                                    int copyW, int copyH)
{
    if (dst == NULL || dst->data == NULL || src == NULL || src->data == NULL) return IMAGE_NONE_INIT;

    if (copyW <= 0 || copyW > ((dst->width - posDstX < src->width - posSrcX) ? dst->width - posDstX : src->width - posSrcX))
    {
        copyW = ((dst->width - posDstX < src->width - posSrcX) ? dst->width - posDstX : src->width - posSrcX);
    }

    if (copyH <= 0 || copyH > ((dst->height - posDstY < src->height - posSrcY) ? dst->height - posDstY : src->height - posSrcY))
    {
        copyH = ((dst->height - posDstY < src->height - posSrcY) ? dst->height - posDstY : src->height - posSrcY);
    }

    if (copyW == 0 || copyH == 0) return WRITE_IMAGE_NO_PLACE;

    if (dst->channel != src->channel) return COPY_IMAGE_DIFFERENT_CHANNELS;

    
    for (int y = 0; y < copyH; y++)
    {
        for (int x = 0; x < copyW * dst->channel; x++)
        {
            if (dst->channel == 4 && x % 4 == 0 && src->data[(posSrcY + y) * src->width * src->channel + posSrcX * src->channel + x + 3] <= NO_WRITE_ALPHA)
            {
                x += 4;
                continue;
            }
            dst->data[(posDstY + y) * dst->width * dst->channel + posDstX * dst->channel + x] = src->data[(posSrcY + y) * src->width * src->channel + posSrcX * src->channel + x];
        }
        
    }
    
    return IMAGE_SUCCESS;
}