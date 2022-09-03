#include <drivers/video/lfb.h>
#include <limine.h>


static struct limine_framebuffer_request fb_req = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};


#define FRAMEBUFFER fb_req.response->framebuffers[0]



static inline size_t get_pixel_index(uint32_t x, uint32_t y) {
    return x + y * FRAMEBUFFER->width;
}


void lfb_put_pixel(uint32_t x, uint32_t y, uint32_t color) {
    if (x > FRAMEBUFFER->width - 1 || x < 0 || y > FRAMEBUFFER->height - 1 || y < 0) {
        return;
    }

    ((uint32_t*)FRAMEBUFFER->address)[get_pixel_index(x, y)] = color;
}


void lfb_clear_screen(uint32_t color) {
    for (uint32_t i = 0; i < FRAMEBUFFER->pitch * FRAMEBUFFER->height; ++i) {
        ((uint32_t*)FRAMEBUFFER->address)[i] = color;
    }
}


size_t lfb_get_width(void) {
    return fb_req.response->framebuffers[0]->width;
}

size_t lfb_get_height(void) {
    return fb_req.response->framebuffers[0]->height;
}