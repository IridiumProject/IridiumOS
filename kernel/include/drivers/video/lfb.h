#ifndef VIDEO_LFB_H
#define VIDEO_LFB_H

#include <stddef.h>
#include <stdint.h>


size_t lfb_get_width(void);
size_t lfb_get_height(void);
void lfb_put_pixel(uint32_t x, uint32_t y, uint32_t color);
void lfb_clear_screen(uint32_t color);


#endif
