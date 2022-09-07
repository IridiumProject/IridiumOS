#ifndef UAPI_VIDEO_SCREEN_H
#define UAPI_VIDEO_SCREEN_H

enum {
    SCREENUAPI_CLEARSCREEN = 0,
    SCREENUAPI_PUTPIXEL = 1,
};

void screen_uapi_init(void);



#endif
