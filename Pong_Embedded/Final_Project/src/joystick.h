// From what I got so far the joystick has predefined x/y values that change with movement
// So we can determine which way the movement is based off something like if( x > some value) then move left/right
// the joysticks convert analog to digital which can be tricky to set up, but we should minimize the length of wire 
// running between the joystick and the board and add a smoothing capacitor to ground to reduce any noisy signals
#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <avr/io.h>
#include <Arduino.h>
#include <avr/interrupt.h>

// Function Definitions
void initADC(); 
uint16_t readPlayer1();
uint16_t readPlayer2();

// comms with other arduino
void initUSART0();
void initUSART1();
char rx_USART1();
void tx_USART0( char c);
void printString_USART0(const char* str);
bool checkReady();
bool checkUSART();



#endif