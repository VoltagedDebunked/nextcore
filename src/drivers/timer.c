#include "timer.h"
#include "../kernel/types.h"
#include "io.h"

#define PIT_CHANNEL0    0x40
#define PIT_COMMAND     0x43
#define PIT_FREQUENCY   1193182
#define TIMER_FREQ      100

static volatile unsigned long ticks = 0;

void timer_init(void) {
    uint16_t divisor = PIT_FREQUENCY / TIMER_FREQ;

    // Set command byte: Channel 0, lobyte/hibyte, mode 3 (square wave)
    outb(PIT_COMMAND, 0x36);

    // Send divisor - low byte first, then high byte
    outb(PIT_CHANNEL0, divisor & 0xFF);
    outb(PIT_CHANNEL0, (divisor >> 8) & 0xFF);
}

void timer_handler(void) {
    ticks++;
}
