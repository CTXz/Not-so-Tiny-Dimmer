#pragma once

#include "config.h"
#include "strip.h"

#define RGB_ARRAY(...) __VA_ARGS__ 

#define PATCH_SET_ALL(R, G, B) \
        RGB_t rgb = {R, G, B}; \
        rgb_apply_brightness(rgb, pot()); \
        strip_set_all(rgb);

#define PATCH_DISTRIBUTE(RGB_ARR) \
        RGB_t rgb[] = { \
                RGB_ARR \
        }; \
        uint8_t brightness = pot(); \
        for (uint16_t i = 0; i < sizeof(rgb)/sizeof(RGB_t); i++) \
                rgb_apply_brightness(rgb[i], brightness); \
        strip_distribute_rgb(rgb, sizeof(rgb)/sizeof(RGB_t));

#define PATCH_ANIMATION_FADE_RGB(STEP_SIZE) strip_fade_rgb(STEP_SIZE, pot())
#define PATCH_ANIMATION_FADE_RGB_POT_CTRL strip_fade_rgb(glob_rgb, pot(), 255)

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
                        strip_distribute_rgb(rgb, sizeof(rgb)/sizeof(RGB_t)); \
                } else { \
                        RGB_t rgb[] = { \
                                {RSH, GSH, BSH}, {RFH, GFH, BFH} \
                        }; \
                        strip_distribute_rgb(rgb, sizeof(rgb)/sizeof(RGB_t)); \
                } \
                swap = !swap; \
                reset_timer(); \
        }

