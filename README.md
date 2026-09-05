# Reaction-Game

A reaction time game developed in C for the Raspberry Pi Pico (RP2040), combining embedded software, digital electronics, user input, timing, randomization, and multiplexed seven-segment display control.

The project was developed as part of the Digital Electronics coursework at the **University of Antioquia (UdeA)**.

## Overview

Reaction-Game challenges the player to respond as quickly as possible to a randomly selected LED.

The system uses three LEDs as visual targets and three corresponding push buttons as inputs. Once the game starts, the LEDs perform a countdown sequence followed by a randomly generated delay. After the delay, one LED is randomly selected and activated. The player must press the corresponding button before the reaction timer reaches 9999 ms.

The measured reaction time is displayed on a four-digit seven-segment display. Pressing an incorrect button adds a 1000 ms penalty to the current time.

The project combines GPIO control, timing logic, pseudo-random behavior, input handling, display multiplexing, and basic switch debouncing.

<img width="" height="360" alt="reaction_demo" src="https://github.com/user-attachments/assets/13e59df2-9296-44c9-8a96-1f919378f67c" />

## Features

* Raspberry Pi Pico / RP2040 embedded application
* C firmware using the Raspberry Pi Pico SDK
* Three LED targets with corresponding push buttons
* Dedicated START button
* Random delay between 1 and 10 seconds
* Random selection of the target LED
* Reaction-time measurement up to 9999 ms
* 1000 ms penalty for incorrect button presses
* Four-digit seven-segment display
* Multiplexed display control
* NPN transistor-based digit selection
* Push-button edge detection to reduce the effects of switch bouncing
* USB serial output for runtime debugging

## Game Operation

A game round follows the logic represented in the following system flowchart:

<img width="719" height="711" alt="game_logic" src="https://github.com/user-attachments/assets/909c0efa-928c-4648-86b3-98e2377552c0" />

The game begins when the START button is pressed. The LEDs then perform a countdown sequence. After a pseudo-random delay, one of the three LEDs is selected as the reaction target.

The reaction timer starts when the target LED is activated. The player must press the corresponding button before the timer reaches 9999 ms.

An incorrect button press adds a 1000 ms penalty. A correct button press stops the timer and leaves the final reaction time displayed.

If the timeout condition is reached without a correct response, the round ends with the maximum displayed value.

## Hardware

### Main Components

| Component                  | Quantity | Purpose                          |
| -------------------------- | -------: | -------------------------------- |
| Raspberry Pi Pico (RP2040) |        1 | Main microcontroller             |
| 7-segment display          | 4 digits | Reaction-time display            |
| LEDs                       |        3 | Visual reaction targets          |
| Push buttons               |        4 | Three color inputs + START       |
| NPN transistors            |        4 | Seven-segment digit selection    |
| Breadboard                 |        1 | Hardware assembly                |
| Resistors and jumper wires |        — | Current limiting and connections |

<img width="480" height="" alt="wokwi_simulation" src="https://github.com/user-attachments/assets/4ca03aad-ad33-46c7-9f95-7ccbf043e8a7" />

### Seven-Segment Display

The four seven-segment digits share their seven segment lines. Each digit is independently enabled through an NPN transistor.

This allows the four digits to be driven using:

* 7 GPIO lines for segments A–G
* 4 GPIO lines for digit selection

Instead of requiring 28 independent control lines, the display is refreshed sequentially using **multiplexing**.

During operation, one digit is activated at a time, its corresponding segment pattern is written, and the next digit is selected. The rapid repetition produces the perception of a continuously illuminated four-digit display.

<img width="960" height="377" alt="display_multiplexing_tinkercad" src="https://github.com/user-attachments/assets/18fdde8a-d3ca-42fa-a72d-d52c8b46914a" />

The Tinkercad schematic uses an Arduino as a simulation reference because a Raspberry Pi Pico was not available in the Tinkercad component library. The display-driving logic and multiplexing concept correspond to the Pico implementation.

## GPIO Assignment

The firmware currently defines the following GPIO assignments:

| GPIO | Function                  |
| ---: | ------------------------- |
|  2–8 | Seven-segment lines A–G   |
|   12 | Digit-select transistor 1 |
|   13 | Digit-select transistor 2 |
|   14 | Digit-select transistor 3 |
|   15 | Digit-select transistor 4 |
|   16 | Red LED                   |
|   17 | Blue LED                  |
|   18 | Yellow LED                |
|   19 | START button              |
|   20 | Red button                |
|   21 | Blue button               |
|   22 | Yellow button             |

The three color buttons use internal pull-up resistors and are interpreted as active-low inputs.

## Software

The firmware is implemented in a single C source file:

`src/reflex_test.c`

The implementation contains the following logical subsystems.

### LED Control

Controls the three target LEDs, including the initial countdown and random target selection.

### Button Input

Reads the START and color buttons using GPIO inputs and internal pull-up resistors.

The color-button logic keeps track of the previous button state to detect a transition and avoid repeatedly processing a button while it remains pressed.

### Randomized Game Timing

After the LED countdown, the firmware generates a pseudo-random delay before selecting the target LED.

A second pseudo-random value determines which of the three LEDs is activated.

### Reaction-Time Measurement

Once the target LED is activated, the firmware starts incrementing the reaction-time counter.

The counter is represented as four decimal digits and updated while the player is responding.

### Penalty System

Pressing a button that does not correspond to the active LED adds 1000 ms to the current reaction time.

If the penalty causes the game to reach the timeout condition, the round ends and the display is set to `9999`.

### Display Multiplexing

The reaction time is represented by four decimal digits and displayed through a multiplexed seven-segment arrangement.

The firmware cycles through the four digit-select transistors and applies the corresponding segment bit mask for each digit.

## Input Handling and Debouncing

The project includes dedicated logic for handling push-button transitions and reducing unwanted repeated detections caused by mechanical switch bouncing.

<img width="544" height="482" alt="anti_bounce_push_button" src="https://github.com/user-attachments/assets/350f4fa8-6d7c-4e8f-9ca4-beb53d6066d9" />

The debounce behavior is documented separately in `diagrams/anti_bounce_push_button.png`.

## System Architecture

The overall hardware/software interaction is documented in the following system diagram:

<img width="831" height="889" alt="system" src="https://github.com/user-attachments/assets/68429425-25d4-4970-a594-1b688ff44063" />

The diagram provides a higher-level representation of the interaction between the Raspberry Pi Pico, input devices, LEDs, and display system.

## Development and Validation

Several representations were used during development to validate the hardware and software behavior.

### Wokwi

Wokwi was used to validate the Raspberry Pi Pico logic and basic input/output behavior.

The simulation is intentionally simplified because the required transistor-based display switching could not be fully reproduced with the available components.

<img width="480" height="" alt="wokwi_simulation" src="https://github.com/user-attachments/assets/bf8e3efb-5f26-48ed-ac33-6dc41b557b47" />

### Tinkercad

Tinkercad was used to document and visualize the seven-segment multiplexing circuit.

An Arduino was used as a simulation substitute because the Raspberry Pi Pico was not available in the Tinkercad component library. The relevant display-driving logic and multiplexing arrangement remain representative of the physical implementation.

### Physical Implementation

The final system was assembled on a breadboard using:

* Raspberry Pi Pico
* Three LEDs
* Four push buttons
* Four seven-segment digits
* Four NPN transistors for digit selection
* Current-limiting resistors and wiring

The physical implementation integrates the complete game logic with the electronic interface.

## Repository Structure

```text
Reaction-Game/
├── diagrams/
│   ├── anti_bounce_push_button.png
│   ├── game_logic.png
│   └── system.png
│
├── media/
│   ├── reaction_game_demo.mp4
│   └── timeout_demo.mp4
│
├── schematics/
│   ├── display_multiplexing_tinkercad.png
│   └── wokwi_simulation.jpeg
│
├── src/
│   ├── CMakeLists.txt
│   ├── pico_sdk_import.cmake
│   └── reflex_test.c
│
├── .gitignore
├── LICENSE
└── README.md
```

## Building

The project uses **CMake** and the **Raspberry Pi Pico SDK**.

The build configuration is located in `src/CMakeLists.txt` and defines the `reflex_test` executable.

From the `src/` directory, a typical CMake build can be performed with:

```bash
cmake -S . -B build
cmake --build build
```

The resulting UF2 firmware can then be transferred to the Raspberry Pi Pico using the standard RP2040 UF2 bootloader process.

A properly configured Raspberry Pi Pico SDK environment is required.

## Demonstrations

### Reaction Game

`media/reaction_game_demo.mp4`

Demonstrates the complete reaction-game operation, including the countdown, target selection, player response, and reaction-time display.

<img width="202" height="360" alt="reaction_game_demo" src="https://github.com/user-attachments/assets/00269a2f-9e84-49b9-a863-2d097983950b" />

### Timeout

`media/timeout_demo.mp4`

Demonstrates the timeout behavior when the player does not respond before the reaction counter reaches the maximum displayed value.

<img width="360" height="422" alt="timeout_demo_3" src="https://github.com/user-attachments/assets/b1e524cc-a29f-4482-8d6f-4e082ba7271a" />

## Project Documentation

The repository includes the original design material used during development:

* `diagrams/anti_bounce_push_button.png` — push-button input and debounce logic
* `diagrams/game_logic.png` — game logic flowchart
* `diagrams/system.png` — overall system architecture
* `schematics/display_multiplexing_tinkercad.png` — seven-segment multiplexing circuit
* `schematics/wokwi_simulation.jpeg` — Raspberry Pi Pico simulation setup

These materials complement the source code by documenting both the logical behavior and the hardware implementation.

## Authors

**Santiago Giraldo Tabares**

**Ana María Velasco Montenegro**

Electronics Engineering
University of Antioquia (UdeA)

## License

This project is licensed under the MIT License. See [`LICENSE`](LICENSE) for details.
