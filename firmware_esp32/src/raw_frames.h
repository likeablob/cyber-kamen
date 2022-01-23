#pragma once

#include <stdint.h>

const uint8_t default_face[] = {0b0,        0b11000011, 0b11000011, 0b11000011,
                                0b11000011, 0b11000011, 0b11000011, 0b0};
const uint8_t blink_frame_0[] = {0b0,        0b0,        0b11000011, 0b11000011,
                                 0b11000011, 0b11000011, 0b0,        0b0};
const uint8_t blink_frame_1[] = {0b0,        0b0, 0b0, 0b11000011,
                                 0b11000011, 0b0, 0b0, 0b0};
const uint8_t blink_frame_2[] = {0b0, 0b0, 0b0, 0b11000011, 0b0, 0b0, 0b0, 0b0};
