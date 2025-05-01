#include "rfid.h"
 
 
// ------------- SPI --------------- //
 
void initSPI() {
 
    DDRB |= ( 1 << MOSI ) | ( 1 << SCK ) | ( 1 << SS_PIN );
    // since were setting master -> SS has to stay high
    PORTB |= ( 1 << SS_PIN );
    // Enables SPI, select MASTER, sets clock divider to F_CPU / 16 = 1 MHZ
    // I tried setting it to 4 MHz but it didn't like that
   
    SPCR = ( 1 << SPE ) | ( 1 << MSTR ) ;
    // SPCR = ( 1 << SPE ) | ( 1 << MSTR ) | ( 1 << SPR0);
}
 
// SPI sends a bit and recieves a bit
uint8_t tx_SPI( uint8_t data) {
 
    SPDR = data;
    // waits until tx complete
    while ( !(SPSR & (1 << SPIF)));
   
    return SPDR;
}
 
 
 
 
// --------------- USART -------------- //
// USART1 is way easier to use when communicating with another board versus USART0
 
void init_USART1() {
 
    uint16_t ubrr = (F_CPU / (16UL * 9600)) - 1;
    UBRR1H = (unsigned char)(ubrr >> 8);
    UBRR1L = (unsigned char)ubrr;
    UCSR1B = (1 << TXEN1);
    UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);
}
 
void tx_USART1(unsigned char data) {
    while (!(UCSR1A & (1 << UDRE1)));
    UDR1 = data;
}
 
 
void printString_USART( const char* str) {
 
    while ( *str) {
        tx_USART1( *str++);
    }
}
 
void printHex_USART( uint8_t num) {
 
    const char hexDigits[] = "0123456789ABCDEF";
 
    tx_USART1(hexDigits[(num >> 4) & 0x0F]);
    tx_USART1(hexDigits[num & 0x0F]);
}
 
// ------------- RFID ------------ //
 
// pg. 11 of datasheet
// writing requires the MSB to be zero -> LSB must be zero also so use 0x7E (0b0111 1110)
// gotta set SS low to initate communication
// because of the MSB/LSB -> the address is from bits 6:1 so you have to shift left one
void write_RFID( uint8_t ADDR, uint8_t DATA) {
 
    PORTB &= ~(1 << SS_PIN);
    _delay_us(10);
   
    tx_SPI( (ADDR << 1) & 0x7E );   // i dont think the 0x7E is required but left it anyway
    tx_SPI( DATA);
   
    _delay_us(10);
    PORTB |= ( 1 << SS_PIN );  
    _delay_us(10);
}
 
// reading requires MSB to be one -> LSB still zero
// same as above -> the only valid bits are 6:1
uint8_t read_RFID( uint8_t REG) {
 
    PORTB &= ~( 1 << SS_PIN );
 
    tx_SPI( ((REG << 1) & 0x7E) | 0x80);
    uint8_t value = tx_SPI(0x00);   // send garbage
 
    PORTB |= ( 1 << SS_PIN );  
    return value;
}
 
void init_RFID() {
 
    write_RFID( COMMAND_REG, 0x0F);        // Soft Reset pg. 70
    _delay_ms(50);
    write_RFID( T_MODE_REG, 0x8D);         // Timer settings are upper nybble (0x8 = Timer starts/stops automatically), (0xD) used below
    write_RFID( T_PRESCALE_REG, 0x3E);     // Prescaler is 12 bits - 0xD3E ~3390 -> 13.56MHz / (2 * 3390 + 2) = 2 kHz
    write_RFID( T_RELOAD_REGL, 0xE8);      // the high and low bytes here are actually : 0x1EE8 = 7912
    write_RFID( T_RELOAD_REGH, 0x1E);      // its what the clock divider uses to count to (4 seconds)
 
    write_RFID( TX_ASK_REG, 0x40);         // forces full strength (100% ASK modulation whatever that means)
     
 
    write_RFID( TX_MODE_REG, 0x00);        // buncha words in this both tx / rx
    write_RFID( RX_MODE_REG, 0x00);        // they're set to the same speed of 106 kBd ( dont think this is required)
    write_RFID( MOD_WIDTH_REG, 0x26);      // default value ( dont think this is required either )
    write_RFID( TX_ASK_REG, 0x40);         // full power
 
    write_RFID( MODE_REG, 0x30);           // pg. 48 - TxWaitRF,
 
    // antenna defaults to both off -- this turns them both on
    uint8_t temp = read_RFID( TX_CNTRL_REG);
 
    if ((temp & 0x03) != 0x03) {
        write_RFID( TX_CNTRL_REG, temp | 0x03);
    }
}
 
 
uint8_t request_RFID( uint8_t *tagType) {
    // this is the arduino library converted to C -> this is where all my problems were
    write_RFID(BIT_FRAMING_REG, 0x07);                      // Only 7 bits to send
 
    write_RFID(COMMAND_REG, 0x00);                          // Set IDLE
    write_RFID(FIFO_LEVEL_REG, 0x80);                       // Clear FIFO
 
    write_RFID(FIFO_DATA_REG, PICC_REQIDL);                 // Load REQA command
    write_RFID(COMMAND_REG, 0x0C);                          // Start Transceive
    write_RFID(BIT_FRAMING_REG, read_RFID(BIT_FRAMING_REG) | 0x80); // StartSend bit = 1
 
    //_delay_us(50);
    // Waits for response
    uint16_t timeout = 5000;
    while ( !(read_RFID( COMM_IRQ_REG) & 0x30)) {
 
        if ( --timeout == 0) {
            write_RFID( COMMAND_REG, 0x00);
            printString_USART("REQA: Timeout waiting for response\n");
            return 0;
        }
    }
 
    // Buffer overflow, Parity error, Protocol error
    uint8_t error = read_RFID( ERROR_REG);
 
    if ( error & 0x1B) {
        printString_USART("REQA: Error detected\n");
        return 0;
    }
 
 
    // All code below checks the Answer to Request Type A (ATQA)
    // card transmits this when it detects a request
    // response is 2 bytes -> our card is 0x0400
    timeout = 1000;
    while ( read_RFID( FIFO_LEVEL_REG) != 2) {  
        _delay_us(10);
        if (--timeout == 0) {
            printString_USART("Timeout waiting FIFOLevel == 2\n");
            return 0;
        }
    }
 
 
    tagType[0] = read_RFID( FIFO_DATA_REG);
    tagType[1] = read_RFID( FIFO_DATA_REG);
   
    printString_USART(" ATQA Response: ");
    printHex_USART( tagType[0]);
    printString_USART(" ");
    printHex_USART( tagType[1]);
    printString_USART("\n");
 
    return 1;
}
 
 
// Converted from arduino library to C. Function is used for multiple cards in proximity resulting in
// bit collisions. I think it follows the ISO 14444-3 Protocol for these types of devices. Apparently that
// costs money, so I used someone elses repo to figure this out. I'm sure this could be improved upon
uint8_t anticollision_RFID(uint8_t *uid) {
   
    write_RFID( BIT_FRAMING_REG, 0x00);
 
    write_RFID( COMMAND_REG, PCD_IDLE);
    write_RFID( FIFO_LEVEL_REG, 0x80);
 
    write_RFID( FIFO_DATA_REG, PICC_SEL_CL1);
    write_RFID( FIFO_DATA_REG, 0x20);
 
    write_RFID( COMMAND_REG, PCD_TRANSCEIVE);
    write_RFID( BIT_FRAMING_REG, 0x80);
    //write_RFID( BIT_FRAMING_REG, 0x80)
 
    uint16_t timeout = 5000;
    uint8_t irqValue;
    while ( !((irqValue = read_RFID( COMM_IRQ_REG)) & 0x30)) {
        if (--timeout == 0) {
            printString_USART("Anticollision timeout\n");
            write_RFID( COMMAND_REG, PCD_IDLE);
            return 0;
        }
        _delay_us(100);
    }
 
    uint8_t error = read_RFID( ERROR_REG);
    if ( error & 0x13) {
        printString_USART("Error Register:\n");
        printHex_USART( error);
        printString_USART("\n");
        write_RFID( COMMAND_REG, 0x00);
        return 0;
    }
 
    _delay_ms(1);
 
    // Timing issue here, unsure of why but would assume its from inital commands to RFID
 
    // Debugging Statements:
    //
    // uint8_t collReg = read_RFID( COLL_REG);
    // printString_USART( "Coll Reg: ");
    // printHex_USART( collReg);
    // printString_USART( "\n");
 
    // uint8_t fifoLevel = read_RFID( FIFO_LEVEL_REG);
    // printString_USART("FIFO Level: ");
    // printHex_USART( fifoLevel);
    // printString_USART("\n");
 
 
    // if ( fifoLevel < 5) {
 
    //     printString_USART(" Not getting all UID bytes\n");
    //     return 0;
    // }
 
    // Card Identifier - its possible to edit this but looks like alot of work
    printString_USART("UID: ");
    for (uint8_t i = 0; i < 4; i++) {
 
        uid[i] = read_RFID( FIFO_DATA_REG);
        printHex_USART( uid[i]);
        printString_USART(" ");
 
    }
 
    //the BCC value is the UID xor'd together
    byte getBCC = read_RFID( FIFO_DATA_REG);
    byte BCC = uid[0] ^ uid[1] ^ uid [2] ^ uid[3];
 
 
    printString_USART("\n");
    printString_USART("BCC:");
    printHex_USART(getBCC);
    printString_USART("\n Our BCC: ");
    printHex_USART( BCC);
    printString_USART(" \n");
 
    return 1;
}