/**
 * @file io.h
 * @author paul
 * @date 27.11.22
 * Description here TODO
 */
#ifndef MOCK_AVR_IO_H
#define MOCK_AVR_IO_H

#include <stdint.h>

extern uint8_t DDRA;
extern uint8_t DDRB;
extern uint8_t DDRC;
extern uint8_t DDRD;
extern uint8_t DDRE;
extern uint8_t DDRH;
extern uint8_t DDRL;
extern uint8_t PORTA;
extern uint8_t PORTB;
extern uint8_t PORTC;
extern uint8_t PORTD;
extern uint8_t PORTE;
extern uint8_t PORTH;
extern uint8_t PORTL;
extern uint8_t MCUSR;

enum { WDRF, BORF };

#endif // MOCK_AVR_IO_H
