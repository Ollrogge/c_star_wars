#ifndef CONSTANTS_H
#define CONSTANTS_H

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define MS_PER_S 1000
// how many ticks per second
#define TICK_RATE 100

#define MS_TO_TICKS(ms) (((ms) * TICK_RATE) / MS_PER_S)
#define TICKS_TO_MS(ticks) ((ticks * MS_PER_S) / TICK_RATE)

#endif // CONSTANTS_H
