#include "pmm.h"

#define NULL 0

#define FRAME_SIZE 4096
#define FRAMES_PER_BYTE 8

static uint8_t* bitmap;
static uint64_t total_frames;
static uint64_t used_frames;
static uint64_t bitmap_size;

void pmm_init(uint64_t mem_size) {
    total_frames = mem_size / FRAME_SIZE;
    bitmap_size = total_frames / FRAMES_PER_BYTE;

    // Place bitmap at a known location - adjust as needed
    bitmap = (uint8_t*)0x100000;

    // Clear bitmap
    for (uint64_t i = 0; i < bitmap_size; i++) {
        bitmap[i] = 0;
    }

    used_frames = 0;
}

static void pmm_set_frame(uint64_t frame_addr) {
    uint64_t frame = frame_addr / FRAME_SIZE;
    uint64_t idx = frame / FRAMES_PER_BYTE;
    uint64_t bit = frame % FRAMES_PER_BYTE;
    bitmap[idx] |= (1 << bit);
}

static void pmm_clear_frame(uint64_t frame_addr) {
    uint64_t frame = frame_addr / FRAME_SIZE;
    uint64_t idx = frame / FRAMES_PER_BYTE;
    uint64_t bit = frame % FRAMES_PER_BYTE;
    bitmap[idx] &= ~(1 << bit);
}

static uint8_t pmm_test_frame(uint64_t frame_addr) {
    uint64_t frame = frame_addr / FRAME_SIZE;
    uint64_t idx = frame / FRAMES_PER_BYTE;
    uint64_t bit = frame % FRAMES_PER_BYTE;
    return bitmap[idx] & (1 << bit);
}

void* pmm_alloc_frame(void) {
    if (used_frames >= total_frames) {
        return NULL;
    }

    // Find first free frame
    for (uint64_t i = 0; i < total_frames; i++) {
        if (!pmm_test_frame(i * FRAME_SIZE)) {
            pmm_set_frame(i * FRAME_SIZE);
            used_frames++;
            return (void*)(i * FRAME_SIZE);
        }
    }
    return NULL;
}

void pmm_free_frame(void* frame) {
    pmm_clear_frame((uint64_t)frame);
    used_frames--;
}
