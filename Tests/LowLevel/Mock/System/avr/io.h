/**
 * @file io.h
 * @author paul
 * @date 27.11.22
 * Description here TODO
 */
#ifndef FLIGHTCONTROLLER_IO_H
#define FLIGHTCONTROLLER_IO_H

#include <stdint.h>

uint8_t DDRA, DDRB, DDRC, DDRD, DDRE, DDRH, DDRL;
uint8_t PORTA, PORTB, PORTC, PORTD, PORTE, PORTH, PORTL;
uint8_t MCUSR;

enum { WDRF, BORF };

#endif // FLIGHTCONTROLLER_IO_H
