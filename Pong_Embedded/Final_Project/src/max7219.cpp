#include "max7219.h"

byte displayBuffer[DISPLAY_MATRIX][8] = {0};
byte gameBuffer[GAME_MATRIX][8] = {0};


void displayStartup() {

  initPorts();
  initMAX7219();
  clearDisplayBuffer();
  flushDisplayBuffer();
  _delay_ms(1);

  initPorts_2();
  initMAX7219_2();
  clearGameBuffer();
  flushGameBuffer();

}

// check header for pin #'s
void initPorts() {
    DDRH |= (1 << CLK) | (1 << DIN) | (1 << CS);
    PORTH |= (1 << CS);
}

void initPorts_2() {
    DDRB |= (1 << CLK_2) | (1 << DIN_2) | (1 << CS_2);
    PORTB |= (1 << CS_2);
}

// same as lab 4
void writeByte( unsigned char DATA) {

    for( byte i = 0; i < 8; i++) {

        PORTH &= ~(1 << CLK);
        if( DATA & 0x80) {
            PORTH |= (1 << DIN);
        }
        else {
            PORTH &= ~(1 << DIN);
        }

        DATA <<= 1;
        PORTH |= (1 << CLK);
    }
}

// same as lab 4
void writeByte_2( unsigned char DATA) {

    for( byte i = 0; i < 8; i++) {

        PORTB &= ~(1 << CLK_2);
        if( DATA & 0x80) {
            PORTB |= (1 << DIN_2);
        }
        else {
            PORTB &= ~(1 << DIN_2);
        }

        DATA <<= 1;
        PORTB |= (1 << CLK_2);
    }
}


void initMAX7219() {
    // instruction array
    const byte instructions[][2] = 
    {
        {0x09, 0x00},   // decode mode
        {0x0A, 0x08},   // brightness (maybe later we could define a value or use a rotary decoder to set?)
        {0x0B, 0x07},   // scan limit -> something about how many rows
        {0x0C, 0x01},   // normal operation 
        {0x0F, 0x00}    // display test
    };

    // hardcoded to 5 -> if you add any instructions to initalize you gotta change this
    for( byte i = 0; i < 5; i++) {

        PORTH &= ~(1 << CS); // i think this can go here, but might have to go in loop?
        for( byte j = 0; j < 4; j++) {
            //PORTH &= ~(1 << CS);
            writeByte( instructions[i][0]);    // address
            writeByte( instructions[i][1]);    // data
            //PORTH |= (1 << CS);
        }
        PORTH |= (1 << CS);
        
    }
}


void initMAX7219_2() {
    // instruction array
    const byte instructions[][2] = 
    {
        {0x09, 0x00},   // decode mode
        {0x0A, 0x08},   // brightness (maybe later we could define a value or use a rotary decoder to set?)
        {0x0B, 0x07},   // scan limit -> something about how many rows
        {0x0C, 0x01},   // normal operation 
        {0x0F, 0x00}    // display test
    };

    // hardcoded to 5 -> if you add any instructions to initalize you gotta change this
    for( byte i = 0; i < 5; i++) {

        PORTB &= ~(1 << CS_2); // i think this can go here, but might have to go in loop?
        for( byte j = 0; j < 8; j++) {
            //PORTH &= ~(1 << CS);
            writeByte_2( instructions[i][0]);    // address
            writeByte_2( instructions[i][1]);    // data
            //PORTH |= (1 << CS);
        }
        PORTB |= (1 << CS_2);
        
    }
}


// this works backwords from what I thought but it works
void writeData(byte matrix, byte addr, byte data) {
    
    PORTH &= ~(1 << CS);
    for (byte i = 1; i <= DISPLAY_MATRIX; i++) {
        
        if (i == matrix) {
            writeByte(addr);
            writeByte(data);
        } 
        else {
            writeByte(0x00);
            writeByte(0x00);
        }
        
    }
    PORTH |= (1 << CS);
    
}

void writeData_2(byte matrix, byte addr, byte data) {
    
    PORTB &= ~(1 << CS_2);
    for (byte i = 1; i <= GAME_MATRIX; i++) {
        
        if (i == matrix) {
            writeByte_2(addr);
            writeByte_2(data);
        } 
        else {
            writeByte_2(0x00);
            writeByte_2(0x00);
        }
        
    }
    PORTB |= (1 << CS_2);
    
}



void clearDisplayTop() {
    for (byte row = 0; row < 8; row++) {
        PORTH &= ~(1 << CS);

        for (byte m = 1; m < DISPLAY_MATRIX; m++) {
            writeByte(row + 1);                     
            writeByte(displayBuffer[m][row]);      
        }

        PORTH |= (1 << CS);
    }
}

void clearGameDisplay() {
    for (byte row = 0; row < 8; row++) {
        PORTB &= ~(1 << CS_2);

        for (byte m = 0; m < GAME_MATRIX; m++) {
            writeByte_2(row + 1);                     
            writeByte_2(gameBuffer[m][row]);       
        }

        PORTB |= (1 << CS_2);
    }
}


void clearDisplayBuffer() {
    for (byte m = 0; m < DISPLAY_MATRIX; m++) {
        for (byte r = 0; r < 8; r++) {
            displayBuffer[m][r] = 0x00;
        }
    }
}

void clearGameBuffer() {
    for (byte m = 0; m < GAME_MATRIX; m++) {
        for (byte r = 0; r < 8; r++) {
            gameBuffer[m][r] = 0x00;
        }
    }
}


// --------- Display -> these are for both displays kinda

int getTableIndex(char c) {

    if (c >= '0' && c <= '9') return c - '0';         // nums
    if (c >= 'A' && c <= 'Z') return c - 'A' + 10;    // letters
    if (c == ' ') return -1;                          
    return -1;  
}

// for the look up table
void displayBits( byte buffer[][8], uint8_t matrix, char c, uint8_t xOffset) {
    
    int index = getTableIndex(c);
    if (index < 0 || matrix > GAME_MATRIX || matrix == 0) return;

    for (uint8_t row = 0; row < 8; row++) {

        uint8_t rowBits = pgm_read_byte(&characters[index][row]);
        uint8_t shiftedRow = (xOffset > 0) ? (rowBits >> xOffset) : rowBits;

        buffer[matrix - 1][row] = shiftedRow;  // Update buffer only
    }
}



void textToMessage( byte buffer[][8], const char *text, uint8_t num, uint8_t set_x) {

    clearGameBuffer();  // clear gameBuffer to zero

    uint8_t matrix = 1;    // Start at rightmost matrix
    uint8_t charIndex = 0;

    while (text[charIndex] != '\0' && matrix <= num) {
        displayBits(buffer, matrix, text[charIndex], set_x);  
        matrix++;
        charIndex++;
    }
}


void gameDisplay(const char *text, uint8_t offset) {

    clearGameBuffer();  // clear gameBuffer
    textToMessage( gameBuffer, text, GAME_MATRIX, offset);
    flushGameBuffer();
}

void messageDisplay(const char *text, uint8_t offset) {

    clearDisplayBuffer();  // clear displayBuffer
    textToMessage( displayBuffer, text, DISPLAY_MATRIX, offset);
    flushDisplayBuffer();
}

// this is just for the top display
void invertDisplay() {

    for (uint8_t m = 0; m < DISPLAY_MATRIX; m++) {

        for (uint8_t r = 0; r < 8; r++) {

            displayBuffer[m][r] ^= 0xFF;  // flip every bit
        }
    }
}


// just for top display
void flashDisplay( const char *message, byte flashes, uint16_t freq) {

    // flips pixels and flashes
    for( byte i = 0; i < flashes; i++) {

        invertDisplay();
        flushDisplayBuffer();
        _delay_ms( freq);

        invertDisplay();
        flushDisplayBuffer();
        _delay_ms( freq);
    }
}


// --------- Animations 
void setDisplayBall(uint8_t x, uint8_t y, bool on) {
    if (x >= 32 || y >= 8) return;

    uint8_t matrix = x / 8;        // Which of 4 matrices
    uint8_t localX = x % 8;

    if (on)
        displayBuffer[matrix][y] |= (1 << (7 - localX));
    else
        displayBuffer[matrix][y] &= ~(1 << (7 - localX));
}


void updateDispBall( Bouncy *ball) {
    // clear previous
    setDisplayBall(ball->x, ball->y, false);
    

    //  position
    ball->x += ball->dx;
    ball->y += ball->dy;

    // wall bounce
    if (ball->x <= 0 || ball->x >= 31) {
        
        ball->dx *= -1;
        if (rand() % 2) ball->dy = (rand() % 2) ? 1 : -1; // add some flavor to those bounces
    }
    if (ball->y <= 0 || ball->y >= 7) {

        ball->dy *= -1;
        if (rand() % 2) ball->dx = (rand() % 2) ? 1 : -1;
    }

    // draw next
    setDisplayBall(ball->x, ball->y, true);
}

void animate() {
    Bouncy ball = {11, 3, 1, 1}; 
    clearDisplayBuffer();  
    updateDispBall( &ball);
    flushDisplayBuffer();
    _delay_ms(250);
    
}