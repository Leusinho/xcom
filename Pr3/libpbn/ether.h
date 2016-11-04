#ifndef ETHER_H
#define ETHER_H

#include <inttypes.h>
#include <stdbool.h>

/*
 * MO-DEM Morse
 * This module use modulator module for MODULATOR
 * and implements a DEModulator on PORTD pin2 by 
 * interrupt INT0
 */

void ether_init(void);
//void ether_put(uint8_t c);
//bool ether_can_read(void);
//uint8_t ether_get(void);

//nova API
typedef uint8_t *block_morse;
typedef void (*ether_callback_t)(void);

bool ether_can_put(void);
void ether_block_put(const block_morse b);

bool ether_can_get(void);
void ether_block_get(block_morse b);
void on_message_received(ether_callback_t m);
void on_finish_transmission(ether_callback_t f);

#endif
