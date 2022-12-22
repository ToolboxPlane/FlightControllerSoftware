/**
 * @file wdt.h
 * @author Paul Nykiel
 * @date 22.12.22
 * @brief Description here TODO
 */
#ifndef FLIGHTCONTROLLER_WDT_H
#define FLIGHTCONTROLLER_WDT_H

typedef enum {
    WDTO_1S
} wdt_t;

void wdt_enable(wdt_t wdt);
void wdt_reset(void);

#endif // FLIGHTCONTROLLER_WDT_H
