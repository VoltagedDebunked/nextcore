#ifndef TYPES_H
#define TYPES_H

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef unsigned long size_t;

typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;
typedef signed long long int64_t;

// Boolean type
typedef enum {
    false = 0,
    true = 1
} bool;

// Mouse state structure
typedef struct {
    int32_t x;
    int32_t y;
    uint8_t buttons;
    bool has_wheel;
    bool has_extra_buttons;
} mouse_state_t;

#endif
