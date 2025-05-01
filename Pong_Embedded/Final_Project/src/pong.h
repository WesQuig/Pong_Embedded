//              Matrix set up 16x32
//                 Game Matrix
//     |================================|
//     |   1   |   2   |   3    |   4   | <-*
//     |================================|
//     |================================|
//  <-*|   5   |   6   |   7    |   8   | <- From Arduino
//     |================================| 
//
//
//
//           Display Coordinates
//  (y)
//   0 |   
//     |                            
//     | 
//     |
//     |   
//     |                            
//     | 
//  15 |------------------------> (x)
//     0                         31



#ifndef PONG_H
#define PONG_H

#include <avr/io.h>
#include <Arduino.h>
#include <avr/interrupt.h>


struct Ball {
  int8_t x;     // horizontal
  int8_t y;     // verticle
  int8_t dx;    // sideways
  int8_t dy;
};

struct Paddle {
  const uint8_t x;   // paddle is fixed in this col
  int8_t y;          // measured from top of paddle
};

struct Game {
  Ball ball;
  Paddle paddle1;
  Paddle paddle2;
  uint8_t ballTimer;
  uint8_t score1;
  uint8_t score2;
};

const int BUZZER = PH3;


// ------- Display Prototypes ------- //
void flushDisplayBuffer();
void displayScore( uint8_t matrix, uint8_t digit);
void flashScore( uint8_t matrix, uint8_t digit, uint8_t flashes);
void introMessage();

// -------- Pong Prototypes -------- //
void updateGame( Game &game);
void setPixelGameBuffer(uint8_t x, uint8_t y, bool on);
void flushGameBuffer();
void resetBall( Game &game, int8_t dir);

// ----------- Buzzer ---------- //
void initPWM( uint16_t freq);
void silence();
void beep( uint16_t freq);
void beepScore( uint16_t freq);


// ------- Dumb -------------- //
void randomFillDisplay( uint8_t steps);
void swirlIntoWIN();
void showWinAnimation( Game &game);

// ------------ Useable? 
void testFunctionality(void);
void initGameBoard(void);
void initWholeBoard(void);
void initUserDisplay(void);
void borderAnimation(void);


// place in progmem maybe
const byte scoreDigits[10][8] = {
  {0b00111100, 0b01100110, 0b01101110, 0b01110110, 0b01100110, 0b01100110, 0b00111100, 0b00000000}, // 0
  {0b00011000, 0b00111000, 0b00011000, 0b00011000, 0b00011000, 0b00011000, 0b01111110, 0b00000000}, // 1
  {0b00111100, 0b01100110, 0b00000110, 0b00001100, 0b00110000, 0b01100000, 0b01111110, 0b00000000}, // 2
  {0b00111100, 0b01100110, 0b00000110, 0b00011100, 0b00000110, 0b01100110, 0b00111100, 0b00000000}, // 3
  {0b00001100, 0b00011100, 0b00111100, 0b01101100, 0b01111110, 0b00001100, 0b00001100, 0b00000000}, // 4
  {0b01111110, 0b01100000, 0b01111100, 0b00000110, 0b00000110, 0b01100110, 0b00111100, 0b00000000}, // 5
  {0b00111100, 0b01100110, 0b01100000, 0b01111100, 0b01100110, 0b01100110, 0b00111100, 0b00000000}, // 6
  {0b01111110, 0b00000110, 0b00001100, 0b00011000, 0b00110000, 0b00110000, 0b00110000, 0b00000000}, // 7
  {0b00111100, 0b01100110, 0b01100110, 0b00111100, 0b01100110, 0b01100110, 0b00111100, 0b00000000}, // 8
  {0b00111100, 0b01100110, 0b01100110, 0b00111110, 0b00000110, 0b01100110, 0b00111100, 0b00000000}  // 9
};

// same
const uint8_t WIN_bitmap[8][3] = {
  {0b10000001, 0b10000001, 0b10000001},  
  {0b10000001, 0b10000001, 0b10000001},
  {0b10010001, 0b10000001, 0b10000001},
  {0b10101001, 0b10000001, 0b10000001},
  {0b10101001, 0b10000001, 0b10000001},
  {0b11000101, 0b10000001, 0b10000001},
  {0b10000001, 0b01111110, 0b01111110},  
  {0b10000001, 0b10000001, 0b10000001}
};

const uint8_t WIN[3][8] = {

  { 0x7C, 0x42, 0x42, 0x7C, 0x40, 0x40, 0x40, 0x00 }, // 'P'
  { 0x18, 0x38, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x00 }, // 1
  { 0x3C, 0x42, 0x02, 0x1C, 0x20, 0x42, 0x7E, 0x00 }, // 2
};



#endif