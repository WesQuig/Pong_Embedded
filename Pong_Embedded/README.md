# RFID Pong Game with MAX7219 LED Display

This project is an embedded system game combining an RFID reader, a MAX7219-controlled LED matrix display, JoyStick, and a simple Pong game logic. The game is implemented in C++ and runs on a microcontroller, using various peripherals for interaction and display.

## Features


- **Interactive Pong Game**: Controlled through RFID tag scans.
- **RFID Integration**: Players identified by RFID tags.
- **JoySticks**: Controls the player paddles.
- **LED Matrix Display**: Real-time rendering of the game using the MAX7219 driver.

---

## File Overview

### `main.cpp`
The central execution point of the application. Initializes the MAX7219 display, RFID module, JoyStick, and starts the Pong game loop. It handles setting up hardware interfaces and updating the display continuously.

### `pong.cpp` / `pong.h`
Implements the core logic of the Pong game:
- Ball and paddle physics
- Collision detection
- Scoring
- Game state updates

### `rfid.cpp` / `rfid.h`
Manages communication with the RFID reader module:
- Reads tag data
- Interprets tag values for player input
- Differentiates between multiple RFID cards
- USART1 used to communicate authorized user 

### `joystick.cpp` / `joystick.h`
Manages the movement of the paddles for players:
- Enables ADC conversion
- Reads analog logic input for both players
- Checks if start sequence (RFID) is ready to start
- USART1 to communivate authorized user

### `MAX7219.cpp` / `MAX7219.h`
Driver for the MAX7219 display chip, enabling:
- 24x32 matrix LED rendering
- Sending commands and data to the display
- Drawing pixels and custom characters

---

## Hardware Requirements

- **Microcontroller** (e.g., STM32, Arduino, etc.)
- **MAX7219 LED Matrix Display**
- **RFID Reader (e.g., MFRC522)**
- **RFID Tags**
- **2x JoySticks**
- **SPI-compatible wiring**

---

## Setup Instructions

1. **Connect Hardware**:
   - Use [wiring diagram](#wiring-diagrams)


   - Wire the MAX7219 display and RFID reader to the microcontroller’s SPI bus.
   - Power the devices appropriately (typically 3.3V or 5V depending on components).

3. **Build the Project**:
   Use PlatformIO, extension downloaded from VSCode to build and run the project

4. **Run the Game**:
   The run portion of this would be running the Upload and Monitor command in PlatformIO.

---

## Game Play Instructions

- **Power on the device.**
- **Scan RFID tags to control game start**
- **Game will begin start sequence**
- **LED Matrices will show user game area and default positions**
- **Play game**

## Actuators & Sensors

1. **Actuator**:
   - (BUZZER) Upon a player scoring a point, the buzzer will be notified to turn on.
   - (LED DISPLAYS) 8x32 LED Matrices will be a visual implementation of driving LEDs high from an input received from the joysticks.

2. **Sensor**:
   - (JOYSTICK) Helps control digital paddles that will be represented on the LED display.
   - (RFID) Reader detects and reads the tag and sends a signal to the other board to authenticate the start of the game.

## Team Responsibilities

1. **Eric**:
   - RFID
   - 8x32 LED Matrices
   - Joystick

2. **Wesley**:
   - Buzzer
   - Game Logic
     
# Wiring Diagrams
   **RFID SENSOR**
   
   ![image](https://github.com/user-attachments/assets/a87d07dc-bf03-4e9f-b200-05eda09fa16c)


   **DISPLAY / JOYSTICK / BUZZER / USART BETWEEN BOARDS**
   
   ![image](https://github.com/user-attachments/assets/6629fcbb-f3de-40af-9957-3a5f11f2aefa)

