#include <common/font.h>
#include <common/panic.h>
#include <common/asm.h>
#include <common/log.h>
#include <common/string.h>
#include <fs/initrd.h>
#include <mm/kheap.h>
#include <drivers/video/lfb.h>
#include <stdint.h>

#define PSF1_MAGIC0 0x36
#define PSF1_MAGIC1 0x04

static uint8_t init = 0;

static struct FontHeader {
    unsigned char magic[2];
    unsigned char mode;
    unsigned char chsize;
} *font_header = NULL;

static void* font = NULL;

void init_font(void) {
    int8_t* font_data = (int8_t*)initrd_open("initrd/font.psf");

    if (font_data == NULL) {
        kprintf(KERN_PANIC "Could not locate builtin font (font.psf)!\n");
        CLI; HLT;
    }

    font_header = kmalloc(sizeof(struct FontHeader));

    if (font_header == NULL) {
        kprintf(KERN_PANIC "Failed to allocate builtin font's header (font.psf)!\n");
        CLI; HLT;
    }

    uint8_t* header_ptr = (uint8_t*)font_header;

    for (uint16_t i = 0; i < sizeof(struct FontHeader); ++i) {
        header_ptr[i] = font_data[i];
    }

    if (font_header->magic[0] != PSF1_MAGIC0 && font_header->magic[1] != PSF1_MAGIC1) {
        kprintf(KERN_PANIC "Builtin font magic invalid!\n");
        CLI; HLT;
    }

    uint64_t glyph_buffer_size = font_header->mode == 1 ? font_header->chsize * 512 : font_header->chsize * 256;
    font = kmalloc(glyph_buffer_size);

    if (font == NULL) {
        kprintf(KERN_PANIC "Failed to allocate glpyhs for builtin font (font.psf)!\n");
        CLI; HLT;
    }

    uint8_t* glyph_buffer_ptr = (uint8_t*)font;
    for (uint64_t i = sizeof(struct FontHeader), j = 0; i < sizeof(struct FontHeader) + glyph_buffer_size; ++i, ++j) {
        glyph_buffer_ptr[j] = font_data[i];
    }
}


void putchar(uint32_t x, uint32_t y, uint32_t color, char c) {
    char* fontPtr = (char*)(font + (c * font_header->chsize));
    for (uint64_t cy = y; cy < y + 16; ++cy) {
        for (uint64_t cx = x; cx < x + 8; ++cx) {
            // if (cy < y+2) continue;     // To remove artifacts on top of letter.
            if ((*fontPtr & (0b10000000 >> (cx - x))) > 0) {
                lfb_put_pixel(cx, cy, color);
            }

        }

        ++fontPtr;
    }
}


void write_string(const char* string, uint32_t color) {
    static uint32_t x = 0;
    static uint32_t y = 0;
    const uint32_t X_INC = 8;
    const uint32_t Y_INC = 10; 

    for (int i = 0; i < strlen(string); ++i) {
        if (string[i] == '\n') {
            y += Y_INC;
            x = 0;
            continue;
        }

        putchar(x, y, color, string[i]);
        x += X_INC;

        if (x >= lfb_get_width() - 1) {
            y += Y_INC;
            x = 0;
        }

        if (x >= lfb_get_height() - 1) {
            lfb_clear_screen(0x000000);
        }
    }
}
