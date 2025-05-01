# Arduino Pong with LED Matrix Display

This project implements a simple version of the classic game **Pong**, played on two 8x8 LED matrices using joysticks for control. The game also features a startup animation and serial-based authentication using a "START" message.

## Features

- Dual MAX7219-controlled 8x8 LED displays
- Joystick input via ADC for paddle control
- Serial communication (USART) to check for a "START" signal
- Simple bouncing ball animation while waiting for game start
- Score tracking for two players
- Clean modular code structure

## Hardware Requirements

- 2 × 8x8 LED matrices with MAX7219 drivers
- 2 × Analog joysticks (or potentiometers)
- 2 × Arduino-compatible boards (optional for multiplayer over serial)
- 1 × Capacitor (~0.1µF) for ADC reference smoothing
- SPI wiring for LED matrices
- USB or serial connection for "START" signal input

## Pin Configuration

| Component     | Arduino Pin        |
|---------------|--------------------|
| LED Matrix 1  | SPI (MOSI, SCK, SS)|
| LED Matrix 2  | SPI (2nd set if used) |
| Joystick 1    | PF1 (ADC1)         |
| Joystick 2    | PF2 (ADC2)         |
| Serial RX     | RX0                |

> Note: Adjust `PF1`, `PF2`, and SPI pins in code if using different analog channels or displays.

## How to Run

1. Wire up both LED matrices, joysticks, and capacitor to your Arduino according to the pinout above.
2. Flash the project code to your Arduino.
3. Open a serial terminal and send the message `START\n`.
4. Use the joysticks to control the paddles and try to bounce the ball past your opponent!

## Notes

- ADC must be initialized properly with external capacitor to avoid noisy readings.
- The MAX7219 drivers handle the LED refresh internally.
- The `checkReady()` function waits for a newline-terminated `"START"` command before launching the game.

# JoyStick Analog Input & USART Communication

This project provides basic initialization and usage of the **Analog-to-Digital Converter (ADC)** and **USART serial communication** for AVR microcontrollers (e.g., ATmega328P, ATmega2560).

## Features

- Configure ADC to read analog inputs on `PF1` and `PF2`
- Read 10-bit ADC results using polling (no interrupts)
- Initialize USART at 9600 baud for serial communication
- Detect `"START"` message from another device (e.g., second Arduino)

## Requirements

- AVR microcontroller (e.g. ATmega series)
- External 0.1µF capacitor on AREF pin for stable ADC reference
- 16 MHz system clock (or modify prescaler math accordingly)
- External device sending `"START\n"` or `"START\r"` via serial

## Pin Usage

| Function      | Pin     | Description                      |
|---------------|---------|----------------------------------|
| Analog Input  | PF1     | Player 1 input (ADC1)            |
| Analog Input  | PF2     | Player 2 input (ADC2)            |
| USART RX      | RXD     | Receive pin for serial input     |

## Code Overview

### ADC Functions

```c
void initADC();
uint16_t readPlayer1();
uint16_t readPlayer2();
```

# MAX7219 Dual Matrix Display Control

This project interfaces with two 8x8 LED matrix displays driven by MAX7219 chips using an AVR microcontroller. It supports initializing and displaying messages on both a **display buffer** and a **game buffer**, with low-level control of each matrix.

## Features

- Support for two independent MAX7219-driven displays
- Buffering for both game state and messages
- Character display from a predefined font table
- Pixel inversion and flashing effects
- RFID input pin checking
- Modular setup for dual display configuration

## File: `max7219.c`

### Key Components

- `displayBuffer` and `gameBuffer`: Represent the state of each matrix group.
- `displayStartup()`: Initializes both displays and their ports.
- `initInput() / inputCheck()`: Configures and reads an input pin (e.g., for RFID).
- `initPorts()` / `initPorts_2()`: Set up the output control pins for both matrices.
- `initMAX7219()` / `initMAX7219_2()`: Sends initialization instructions to the displays.
- `writeByte()` / `writeByte_2()`: Sends a byte over SPI to the displays.
- `writeData()` / `writeData_2()`: Writes a byte to a specific matrix and address.
- `clearDisplay()` / `clearDisplay_2()`: Pushes the buffer content to the displays.
- `clearMAX7219()` / `clearMAX7219_2()`: Resets the internal buffers.
- `displayTable()` / `textToMessage()`: Converts characters or text into display data.
- `gameDisplay()` / `messageDisplay()`: Show messages on game or main display.
- `invertDisplay()`: Inverts pixels (used for effects like flashing).
- `flashDisplay()`: Temporarily flashes a message.

## Usage Example

To show a message on the game display:

```c
gameDisplay("HELLO");
```

# RFID Reader Firmware (C)

This firmware provides the low-level initialization and communication routines for an RFID reader using SPI and USART on an AVR microcontroller (e.g., ATmega). It includes functionality to initialize the required peripherals and interact with the RFID module through standard commands.

## Files

- `rfid.h`: Header file (not shown here, assumed to contain constants and function declarations).
- `rfid.c`: Implementation file containing the following:
  - `startUp()`: High-level function to initialize SPI, USART, and RFID interfaces.
  - SPI communication functions.
  - USART functions for serial output and debugging.
  - RFID functions to send and receive data, perform card detection (`request_RFID`), and UID anticollision (`anticollision_RFID`).

## Functions Overview

### Initialization
- `initSPI()`: Sets up SPI as master with specific clock settings.
- `init_USART()`: Initializes USART for serial communication at 9600 baud.
- `init_RFID()`: Performs hardware-level setup of the RFID module registers and powers on the antenna.

### SPI Communication
- `tx_SPI(uint8_t data)`: Sends and receives one byte via SPI.

### USART Communication
- `tx_USART(unsigned char data)`: Transmits a byte over serial.
- `printString_USART(const char* str)`: Sends a null-terminated string.
- `printHex_USART(uint8_t num)`: Sends a byte as two hexadecimal characters.

### RFID Communication
- `write_RFID(uint8_t ADDR, uint8_t DATA)`: Writes to a register on the RFID module.
- `read_RFID(uint8_t REG)`: Reads from a register on the RFID module.
- `request_RFID(uint8_t *tagType)`: Sends a REQA command to detect nearby RFID tags.
- `anticollision_RFID(uint8_t *uid)`: Requests and stores the UID of the detected RFID tag, handling basic anti-collision.

## Usage Notes

- Ensure your `rfid.h` header defines necessary constants such as register addresses and SPI/USART pin assignments.
- This code assumes a 16 MHz clock (`F_CPU`) and uses `_delay_us()` and `_delay_ms()` functions from `<util/delay.h>`.
- You can connect this firmware to a serial monitor to view responses like ATQA or UID values via USART.

# Pong Game with LED Display and Joystick Control

This project is a simple Pong game with a LED display for showing the game board and scores. The game features two paddles, a bouncing ball, and player control via a joystick.

## Requirements

- A microcontroller (e.g., Arduino, Atmega32)
- MAX7219 LED matrix display
- Joystick for paddle control
- Buzzer for sound effects

## File Overview

### pong.h
Contains the necessary structures and declarations for the game, including the Game object, ball, and paddles.

### max7219.h
Includes functions to control the MAX7219 LED matrix for displaying the game and score.

### joystick.h
Contains functions to read joystick input for controlling the paddles.

## Key Features

- **Game Logic**: Ball bounces off paddles and walls. When the ball passes a paddle, the opposing player scores.
- **Joystick Control**: Each player controls their paddle with a joystick, moving it up or down.
- **LED Display**: The game board and score are displayed on a 4x8 matrix LED display.
- **Sound Effects**: A buzzer sounds when a point is scored.

## Functions

### `updateGame(Game &game)`
Updates the game state: moves the ball, checks for collisions, updates paddles, and handles scoring.

### `setPixelGameBuffer(uint8_t x, uint8_t y, bool on)`
Sets a pixel on the LED matrix for the game board at position `(x, y)`.

### `flushGameArray()`
Flushes the current game state to the LED display.

### `flushDisplayBuffer()`
Flushes the display buffer to the LED matrix.

### `displayScore(uint8_t matrix, uint8_t digit)`
Displays a single digit score on a specified matrix.

### `resetBall(Game &game, int8_t dir)`
Resets the ball to the center of the board with a random direction.

### `flashScore(uint8_t matrix, uint8_t digit, uint8_t flashes = 3)`
Flashes the score on the LED display.

### `randomFillDisplay(uint8_t steps)`
Randomly fills the display with pixels to create a chaotic visual effect.

### `initBuzzer()`
Initializes the buzzer.

### `beepBuzzer(uint8_t times, uint16_t buzzer_delay)`
Makes the buzzer beep a specified number of times with a delay.

### `swirlIntoWIN()`
Creates a swirling animation on the display leading to the word "WIN".

### `showWinAnimation()`
Displays the "WIN" animation when a player wins.

### `initWholeBoard()`
Initializes the game board and user display.

### `initUserDisplay()`
Initializes the display for player scores and labels.

### `borderAnimation()`
Creates a border animation for visual effects.

### `testFunctionality()`
Tests the LED display by lighting up rows in sequence.

## Setup Instructions

1. **Hardware Setup**:
   - Connect the MAX7219 LED matrix to your microcontroller.
   - Connect the joystick to the microcontroller to control the paddles.
   - Connect the buzzer for sound effects.

2. **Code Setup**:
   - Include the necessary header files in your project:
     - `pong.h`
     - `max7219.h`
     - `joystick.h`
   - Ensure that the microcontroller is configured to run the game loop and read joystick inputs.

3. **Game Logic**:
   - The game is controlled by two players, each using a joystick to move their paddle up and down.
   - The ball bounces off the paddles and the top/bottom edges of the screen.
   - When the ball passes a paddle, the other player scores a point.

## Notes

- The game is designed to run on a microcontroller with limited resources. Optimization may be needed for different setups.
- Adjust the joystick input thresholds as needed for your hardware.
- The MAX7219 LED matrix library should be compatible with the hardware being used.
