#include <Arduino.h>
#include "max7219.h"
#include "pong.h"
#include "joystick.h"


int main(void) {


srand(micros());  


initUSART0();    
initUSART1();
displayStartup();    
initADC();    
beep(880);   
_delay_ms(200);
beep(1046);  



for( int j = 8; j >= 0; j--) {
    gameDisplay("SCANCARD", j);
    flushGameBuffer();
    _delay_ms(150);
} 
 

Bouncy ball = {11, 6, 1, 1}; 

while (1) {
    bool gameOver = false; 
    updateDispBall( &ball);       
    flushDisplayBuffer();     
    _delay_ms(40);
    
    if( checkReady() && !gameOver) {

        introMessage();

        Game game = {
            {2, 8, 1, 1},    // Ball
            {1, 1},          // Paddle 1
            {30, 1},         // Paddle 2
            0,               // Ball timer
            0,               // Score1
            0                // Score2
        };

        displayScore(4, game.score1);
        displayScore(1, game.score2);
        flushDisplayBuffer();

        while (1) {
            updateGame(game);
            
            _delay_ms(25);      //*****  UPDATE THIS FOR REFRESH RATE ******/

            // end game
            if( game.score1 >= 3 || game.score2 >= 3) {
                showWinAnimation( game);
                clearGameBuffer();
                for( int j = 8; j >= 0; j--) {
                    gameDisplay("SCANCARD", j);
                    flushGameBuffer();
                    _delay_ms(250);
                } 
                gameOver = true;
                break;
            }

            }
        }  

    }


}