//  pin 53 -> SDA
//  pin 52 -> SCK
//  pin 51 -> MOSI
//  pin 50 -> MISO
//  pin 7  -> RESET
//
//  SPI REQUIRES 3.3V PINS: (used for SDA, SCK, MOSI and RESET)
//  from the board: -----[5k1]------- RFID
//                              |
//                            [10k]
//                              |
//                              V
 
// DATA SHEET: https://www.nxp.com/docs/en/data-sheet/MFRC522.pdf
 
#ifndef RFID_H
#define RFID_H
 
#include <avr/io.h>
#include <util/delay.h>
#include <Arduino.h>
 
// Registers p.36
// -> 0x0X: Command & Status
#define COMMAND_REG           0x01      // starts and stops command execution
#define COMM_IRQ_REG          0x04      // interrupt request bits
#define DIV_IRQ_Reg           0x05      // interrupt request bits
#define ERROR_REG             0x06      // error bits showing the error status of the last command executed
#define FIFO_DATA_REG         0x09      // I/O of 64 byte FIFO buffer
#define FIFO_LEVEL_REG        0x0A      // # of bytes stored in FIFO buffer
#define BIT_FRAMING_REG       0x0D      // adjustments for bit-orientated frames
#define COLL_REG              0x0E      // bit position of the first bit-collision detected on the RF Interface
 
// -> 0x1X: Command
#define MODE_REG              0x11      // defines general modes for transmitting and receiving
#define TX_MODE_REG           0x12
#define RX_MODE_REG           0x13
#define TX_CNTRL_REG          0x14      // controls logical behavior of antenna driver pins ( TX 1 & TX 2)
#define TX_ASK_REG            0x15      // contyrols setting of transmission modulation
 
// -> 0x2X: Configuration
#define CRC_RESULT_REGH       0x21      // shows the MSB and of the CRC calculation
#define CRC_RESULT_REGL       0x22      // shows the LSB values of the CRC calculation
#define MOD_WIDTH_REG         0x24
#define T_MODE_REG            0x2A      // internal timer settings - pg. 61
#define T_PRESCALE_REG        0x2B      // internal timer settings
#define T_RELOAD_REGL         0x2C      // defines 16-bit timer reload value
#define T_RELOAD_REGH         0x2D      // defines 16-bit timer reload value
 
// The PCD / PICC were not found in the datasheet, but in an RFID Arduino repo
// I believe they come from the ISO standard that is not accessiable
// PCD Commands -> board //
#define PCD_IDLE              0x00      // do nothing
#define PCD_TRANSCEIVE        0x0C      // communication command - sends data and listens for response
#define PCD_CALCCRC           0x03      // some math way of checking for bit collision i think
 
//  PICC Commands -> rfid tag  //
#define PICC_REQIDL           0x26      // request used to detect tags
#define PICC_SEL_CL1          0x93      // Cascade lvl 1 - used for type A cards and detecting collisions
 
//  Pin Setup //
#define MOSI               PB2
#define SCK                PB1
#define SS_PIN             PB0
 
 
// SPI Functions
void initSPI();
uint8_t tx_SPI( uint8_t data);
 
// USART Functions
void init_USART1();
void tx_USART1(unsigned char data);
void printString_USART(const char* str);
void printHex_USART(uint8_t num);
 
// RC522 Functions
void init_RFID();
void write_RFID(uint8_t reg, uint8_t value);
uint8_t read_RFID(uint8_t reg);
uint8_t request_RFID(uint8_t *tagType);
uint8_t anticollision_RFID(uint8_t *uid);
 
 
#endif