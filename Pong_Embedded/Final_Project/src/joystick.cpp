#include "joystick.h"



void initADC() {
    // pg. 281 -> setting this bit high (REFS0) means using the internal voltage as a baseline and that we have an external capacitor hooked up 
    ADMUX = (1 << REFS0);   // DO NOT RUN WITHOUT EXTERNAL CAPACITOR TO SMOOTH SIGNAL (I think value is 0.1uF but will have to look into this) // 

    // pg. 271 talks about this:
    // So we're converting an analog signal to digital and need 50kHz-200kHz
    // This will set 16MHz / 128 to get a value of 125 kHz
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    // if we want to use interrupts  ADIF (bit 4) & ADIE (bit 3) are used in ADCSRA
    // ADIF is ADC interrupt flag -> set within respective vector interrupt (and cleared)
    // ADIE is ADC interrupt enable -> also have I-bit in SREG set, enables interrupts for this
}


    // start with clearing the lower 5 bits of the ADMUX
    // initate the conversion from A->D
    // there's flags to indicate the conversion is complete
    // there are also two ADC data registers (pg. 286) where the 
    // conversion data is stored. I think this will be the place to 
    // read in the data but not sure how this interfaces with the 
    // interrupts yet so haven't written any code for this.

// lesson learned - gotta be a 16 bit value returning because ADC returns two bytes

uint16_t readPlayer1() {

    // PF1 - keeps upper 4 bits low
    ADMUX = ( ADMUX & 0xF0) | PF1;
    // Start Conversion
    ADCSRA |= (1 << ADSC);

    while( ADCSRA & ( 1 << ADSC));

    return ADC;
}

uint16_t readPlayer2() {

    // PF2 - keeps upper 4 bits low
    ADMUX = ( ADMUX & 0xF0) | PF2;
    // Start Conversion
    ADCSRA |= (1 << ADSC);

    while( ADCSRA & ( 1 << ADSC));

    return ADC;
}


// ------ Comms with other Arduino -------- //
// USART1 : Board to board comms
// USART0 : Serial Monitor
// Note: It is significantly easier using USART1 for board to board comms
void initUSART1() {
    uint16_t ubrr = (F_CPU / (16UL * 9600)) - 1;
    UBRR1H = (unsigned char)(ubrr >> 8);
    UBRR1L = (unsigned char)ubrr;
    UCSR1B = (1 << RXEN1);
    UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);
}

char rx_USART1() {

    while ( !(UCSR1A & (1 << RXC1)));
    return UDR1;
}

void initUSART0() {

    uint16_t ubrr = (F_CPU / (16UL * 9600)) - 1;
    UBRR0H = (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)ubrr;
    UCSR0B = (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void tx_USART0( char c) {

    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;
}

void printString_USART0(const char* str) {

    while (*str) {
        tx_USART0(*str++);
    }
}

// have to check recieve like this otherwise it waits in main and
// stops the bouncy
bool checkReady() {

    if (checkUSART()) {

        char c = rx_USART1();   
        tx_USART0(c);            
        return c == 'S';         
    }

    return false;                
}



bool checkUSART() {

    return ( UCSR1A & ( 1 << RXC1));
}