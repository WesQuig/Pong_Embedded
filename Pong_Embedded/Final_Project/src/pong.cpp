#include "pong.h"
#include "max7219.h"
#include "joystick.h"

int wholeRow = 0b11111111;


// brains 
void updateGame(Game &game) {

    for (uint8_t m = 0; m < GAME_MATRIX; m++) {

        for (uint8_t r = 0; r < 8; r++) {
            // add a delay in here to do a slow refresh
            gameBuffer[m][r] = 0;
        }
    }

    game.ballTimer++;
    if (game.ballTimer >= 3) {

        int nextX = game.ball.x + game.ball.dx;
        int nextY = game.ball.y + game.ball.dy;

        // top and bottom bounce
        if (nextY < 0 || nextY > 15) {
            game.ball.dy *= -1;
            nextY = game.ball.y + game.ball.dy;
            beep(880);   

            
        }

        // Paddle 1 collision
        if (nextX == game.paddle1.x &&  nextY >= game.paddle1.y && nextY < game.paddle1.y + 3) {

            game.ball.dx *= -1;
            beep(1200);  

        // ball will change direction depending on where it hits the paddle
        int hitPos = nextY - game.paddle1.y;
        if (hitPos == 0)      game.ball.dy = -1;  // up
        else if (hitPos == 1) game.ball.dy = 0;   // straight
        else if (hitPos == 2) game.ball.dy = 1;   // down

         nextX = game.ball.x + game.ball.dx;
        }

        // Paddle 2 collision - same logic as paddle 1
        else if (nextX == game.paddle2.x && nextY >= game.paddle2.y && nextY < game.paddle2.y + 3) {

        game.ball.dx *= -1;
        beep(1200); 

        int hitPos = nextY - game.paddle2.y;
        if (hitPos == 0)      game.ball.dy = -1;
        else if (hitPos == 1) game.ball.dy = 0;
        else if (hitPos == 2) game.ball.dy = 1;

        nextX = game.ball.x + game.ball.dx;
        }

        // scoring checks
        if (nextX < 0) {
            beep( 1250);
            
            game.score1++;
            displayScore(4, game.score1);
            displayScore(1, game.score2);
            flushDisplayBuffer();
            
            flashScore(4, game.score1, 4);
            resetBall(game, 1);
            
            game.ballTimer = 0;
            return;
        }
        
        if (nextX > 31) {
            beep( 1250);
            game.score2++;
            displayScore(4, game.score1);
            displayScore(1, game.score2);
            flushDisplayBuffer();
            
            flashScore(1, game.score2, 3);
            resetBall(game, -1);
            
            game.ballTimer = 0;
            return;
        }
        
        game.ball.x = nextX;
        game.ball.y = nextY;
        game.ballTimer = 0;
    }

    // adc function values
    uint16_t p1 = readPlayer1();
    uint16_t p2 = readPlayer2();

    // paddle updates after ball -- maybe try other way?
    
    if (p1 < 450 && game.paddle1.y > 0) {
        game.paddle1.y--;

    } 
    else if (p1 > 570 && game.paddle1.y < 13) {
        game.paddle1.y++;
    }

    if (p2 < 450 && game.paddle2.y > 0) {
        game.paddle2.y--;

    } 
    else if (p2 > 570 && game.paddle2.y < 13) {
        game.paddle2.y++;
    }

    // this is really fast - maybe add difficulty setting?
    for (int i = 0; i < 3; i++) {
        setPixelGameBuffer(game.paddle1.x, game.paddle1.y + i, true);
        setPixelGameBuffer(game.paddle2.x, game.paddle2.y + i, true);
    }

    setPixelGameBuffer(game.ball.x, game.ball.y, true);
    flushGameBuffer();
}



// sets a coordinate system for the game matrices
void setPixelGameBuffer(uint8_t x, uint8_t y, bool on) {

    if (x >= 32 || y >= 16) return; 

    uint8_t matrix = x / 8;      // which 8x8 matrix horizontally
    uint8_t localX = x % 8;
    uint8_t localY = y % 8;
    
    uint8_t row = (y / 8) * 4;              // 4 matrices per row
    uint8_t matrixIndex = row + matrix;     // probably a better way to do this but it works

    if (on)
        gameBuffer[matrixIndex][localY] |= (1 << (7 - localX));
    else
        gameBuffer[matrixIndex][localY] &= ~(1 << (7 - localX));
}

// update the game - much smoother using this then just writing
void flushGameBuffer() {

    for (uint8_t row = 0; row < 8; row++) {

        PORTB &= ~(1 << CS_2);

        for (uint8_t m = 0; m < GAME_MATRIX; m++) {
            writeByte_2(row + 1);
            writeByte_2(gameBuffer[m][row]);
        }

        PORTB |= (1 << CS_2);
    }
}

void flushDisplayBuffer() {

    for (uint8_t row = 0; row < 8; row++) {

        PORTH &= ~(1 << CS);

        for (uint8_t m = 0; m < DISPLAY_MATRIX; m++) {

            writeByte(row + 1);
            writeByte(displayBuffer[m][row]);
        }

        PORTH |= (1 << CS);
    }
}


void displayScore(uint8_t matrix, uint8_t digit) {

    if (digit > 9) return;
    for (uint8_t row = 0; row < 8; row++) {
        displayBuffer[matrix - 1][row] = scoreDigits[digit][row];
    }
}


// updated this to draw the ball in right away ( it was pretty fast after a score )
// now it draws that ball in right away and has small pause
void resetBall(Game &game, int8_t dir) {

    game.ball.x = 16;
    game.ball.y = 8;
    game.ball.dx = dir;
    game.ball.dy = (rand() % 3) - 1;  

    
    setPixelGameBuffer(game.ball.x, game.ball.y, true);
    flushGameBuffer();

    _delay_ms(500); 
}


void flashScore(uint8_t matrix, uint8_t digit, uint8_t flashes = 3) {
    if (digit > 9) return;

    for (uint8_t i = 0; i < flashes; i++) {
        // Clear digit (blank)
        for (uint8_t row = 0; row < 8; row++) {
            displayBuffer[matrix - 1][row] = 0x00;
        }
        flushDisplayBuffer();
        _delay_ms(150);

        // Restore digit
        for (uint8_t row = 0; row < 8; row++) {
            displayBuffer[matrix - 1][row] = scoreDigits[digit][row];
        }
        flushDisplayBuffer();
        _delay_ms(150);
    }
}


void randomFillDisplay(uint8_t steps) {

    for (uint8_t i = 0; i < steps; i++) {

        uint8_t matrix = random(0, 4);  // 0–3 (matrices 4, 3, 2, 1)
        uint8_t row = random(0, 8);
        uint8_t col = random(0, 8);

        displayBuffer[matrix][row] |= (1 << col);
        flushDisplayBuffer();
        _delay_ms(10);
    }
}

void initPWM(uint16_t frequency) {
   
    DDRE |= (1 << PE3);

    // gets the top value for frequency
    uint16_t top = (F_CPU / (2UL * frequency)) - 1;

    // toggle OC3A on compare match
    TCCR3A = (1 << COM3A0);                
    TCCR3B = (1 << WGM32) | (1 << CS30);   

    OCR3A = top;
}

// turn off beep
void silence() {

    TCCR3A = 0;
    TCCR3B = 0;
    PORTE &= ~(1 << PE3); 
}

void beep( uint16_t freq) {

    initPWM( freq);
    _delay_ms( 10);
    silence();
}

void beepScore( uint16_t freq) {

    for( int i = 0; i < 1; i++) {
        initPWM( freq);
        _delay_ms(25);
    }

}




void showWinAnimation(Game &game) {

    clearDisplayBuffer();
    clearGameBuffer();

    messageDisplay("WIN", 0);  // Show WIN on top display
    invertDisplay();
    flashDisplay("WIN", 4, 150);
    messageDisplay(" WIN", 0);  // Show WIN on top display
    invertDisplay();
    flashDisplay(" WIN", 3, 150);


 

    for (int i = 0; i < 10; i++) {
        
        //clearDisplayBuffer();
        clearGameBuffer();
        flushDisplayBuffer();
        flushGameBuffer();

        int winner = (game.score1 > game.score2) ? 2 : 1;

        
        beepScore(880);
        _delay_ms(100);
        beepScore(440);
        _delay_ms(100);
        beepScore(1046);
        _delay_ms(50);
        silence();
        // random matrix 
        uint8_t baseMatrix = (random() % 7) + 1;

    
        for (uint8_t row = 0; row < 8; row++) {
            // messageDisplay("WIN", row);
            // _delay_ms(100);
            gameBuffer[baseMatrix - 1][row] = WIN[0][row];                 // Always 'P'
            gameBuffer[baseMatrix][row]     = WIN[winner][row];            // winner = 1 or 2
        }
        

        flushGameBuffer();
        _delay_ms(150);
    }
}



// clearly this was well planned
void introMessage() { 

    clearDisplayBuffer();
    clearGameBuffer();
    flushGameBuffer();
    messageDisplay("PONG", 0);
    invertDisplay();
    flashDisplay("PONG", 7, 250);
    flushDisplayBuffer();
    clearDisplayBuffer();

    messageDisplay("PLAY", 0);
    gameDisplay(" TO NINE", 0);
    flushGameBuffer();
    flushDisplayBuffer();
    _delay_ms(2200);
    clearDisplayBuffer();
}


