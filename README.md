# 8051 Bull-Cow (Number Guessing) Game
This is a classic Bull-Cow (1A2B) number guessing mini-game developed for the 8051 microcontroller. The project utilizes a dual-device setup via UART communication, allowing one device to act as the question setter and the other as the guesser. It features matrix keypad input, LCD data display, and a robust data transmission protocol.

## Features
Dual-Device Communication: Utilizes 8051 UART (Serial Communication) for interactive gameplay between two development boards.

Data Integrity & Error Handling: Implements an XOR CheckSum mechanism along with ACK (O) / NACK (E) flags. Includes a timeout and automatic maximum 3-retry mechanism to ensure reliable data transmission.

Hardware Interfacing: Integrates a 4x4 matrix keypad for user input and a 16x2 LCD (HD44780 compatible) for real-time game status display.

## Hardware Configuration
Ensure your 8051 development board is wired as follows:

### LCD Display:

Data Pins (D0-D7): P2

RS Pin: P1.0

RW Pin: P1.1

E Pin: P1.2

### Matrix Keypad: 
Connected to P0

### UART Communication: 
Cross-connect TX and RX pins between the two 8051 boards (TX to RX, RX to TX), and ensure a common Ground (GND).

### Keypad Mapping and Controls

Based on the matrix keypad decoding in the code, the special control keys are defined as follows:

**Number Keys (0-9)**: Used for inputting your 4-digit unique numbers (either setting the question or guessing).

**D Key**: Start/Confirm Question. Used by the setter to finalize the 4-digit question and initiate the game session.

**E Key**: Backspace/Delete. Deletes the last entered digit if a mistake is made during input.

**F Key**: Send Guess. Used by the guesser to transmit the 4-digit guess packet (includes CheckSum verification) to the host.

## How to Play

Initialization: Power on both 8051 boards.

Setting the Question: The question setter inputs a unique 4-digit number using the keypad and presses D to start the game.

Guessing: The guesser inputs their 4-digit guess and presses F to send the guess.

Result: The LCD will display the result in the format ?A?B (where 'A' means correct number and position, and 'B' means correct number but wrong position).

## Future Work

The following improvements are planned for future updates:

State Lock for Guesser: Currently, there is no state lock implemented for the guesser's terminal. This allows players to continuously send new guess packets before the previous result is fully calculated and displayed, which can cause scrambled characters (garble) on the LCD. A state-blocking mechanism needs to be added during the "waiting for result" phase.

Multiplayer Support: The current architecture only supports a single question setter and a single guesser. Future updates will aim to overhaul the communication logic to support simultaneous guessing from two players (competitive two-player mode).
