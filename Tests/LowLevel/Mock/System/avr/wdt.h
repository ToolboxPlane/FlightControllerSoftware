/**
 * @file wdt.h
 * @author Paul Nykiel
 * @date 22.12.22
 * @brief Description here TODO
 */
#ifndef FLIGHTCONTROLLER_WDT_H
#define FLIGHTCONTROLLER_WDT_H

typedef enum {
    WDTO_15MS = 0,
    WDTO_30MS = 1,
    WDTO_60MS = 2,
    WDTO_120MS = 3,
    WDTO_250MS = 4,
    WDTO_500MS = 5,
    WDTO_1S = 6,
    WDTO_2S = 7,
    WDTO_4S = 8,
    WDTO_8S = 9,
} wdt_t;

void wdt_enable(wdt_t wdt);
void wdt_reset(void);

#endif // FLIGHTCONTROLLER_WDT_H
