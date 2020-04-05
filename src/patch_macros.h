#pragma once

#include "config.h"
#include "strip.h"

#define RGB_ARRAY(...) __VA_ARGS__ 

#define PATCH_SET_ALL(R, G, B) strip_set_all((RGB_t){R, G, B}, pot(), WS2812_PIXELS)

#define PATCH_DISTRIBUTE(RGB_ARR) \
        RGB_t rgb[] = { \
                RGB_ARR \
        }; \
        strip_distribute_rgb(rgb, sizeof(rgb)/sizeof(RGB_t), pot(), WS2812_PIXELS);

#define PATCH_ANIMATION_FADE_RGB(STEP_SIZE) strip_fade_rgb(STEP_SIZE, pot(), WS2812_PIXELS)
#define PATCH_ANIMATION_FADE_RGB_POT_CTRL strip_fade_rgb(pot(), 255, WS2812_PIXELS)

#define PATCH_ANIMATION_SWAP(RFH, GFH, BFH, RSH, GSH, BSH, SWAP_TIME) \
        static bool swap = false; \
        if (ms_passed() >= SWAP_TIME) { \
                if (swap) { \
                        PATCH_DISTRIBUTE(RGB_ARRAY({RFH, GFH, BFH}, {RSH, GSH, BSH})); \
                } else { \
                        PATCH_DISTRIBUTE(RGB_ARRAY({RSH, GSH, BSH}, {RFH, GFH, BFH})); \
                } \
                swap = !swap; \
                reset_timer(); \
        }

#define PATCH_ANIMATION_SWAP_POT_CTRL(RFH, GFH, BFH, RSH, GSH, BSH) \
        static bool swap = false; \
        if (ms_passed() >= (1020 - (pot() << 2))) { \
                if (swap) { \
                        RGB_t rgb[] = { \
                                {RFH, GFH, BFH}, {RSH, GSH, BSH} \
                        }; \
                        strip_distribute_rgb(rgb, sizeof(rgb)/sizeof(RGB_t), 255, WS2812_PIXELS); \
                } else { \
                        RGB_t rgb[] = { \
                                {RSH, GSH, BSH}, {RFH, GFH, BFH} \
                        }; \
                        strip_distribute_rgb(rgb, sizeof(rgb)/sizeof(RGB_t), 255, WS2812_PIXELS); \
                } \
                swap = !swap; \
                reset_timer(); \
        }

