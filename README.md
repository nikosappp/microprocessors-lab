# AVR Lab Exercises – ATmega328PB 

Solutions for the 7th‑semester Microprocessors Lab course at the National Technical University of Athens (2025–26).


This repository contains codes for a series of AVR microcontroller lab applications using the ATmega328PB on the ntuAboard_G1 platform.

<img width="828" height="592" alt="image" src="https://github.com/user-attachments/assets/b57bd34f-9e6d-458a-8c3b-b69cacd37afb" />

## Contributors
- Nikolaos Apostolopoulos
- Nektarios Vasiliou

## Exercise 1 – Basic Assembly & Timing

Wrote AVR assembly for the ATmega328PB to initialize the stack and I/O, and implemented delay routines and simple LED timing patterns.

## Exercise 2 – External Interrupts & Debouncing

Configured external interrupts (INT0/INT1) to respond to button presses, added debouncing, and built small automation behaviors that control LEDs from ISRs in Assembly and C.

## Exercise 3 – Timers, PWM & ADC

Used Timer1 to generate PWM on PB1 for LED brightness control and enabled the 10‑bit ADC to read analog inputs, linking sensor values to LED indications and brightness modes.

## Exercise 4 – LCD 2x16 & CO Sensor

Interfaced a 2x16 LCD and the ADC to display measured voltages from POT4 and a CO sensor and implemented a CO monitoring routine with threshold‑based LED and LCD alerts.

## Exercise 5 – TWI (I²C) & PCA9555

Set up TWI (I²C) to communicate with the PCA9555 I/O expander, computed logic functions on PORTB inputs, drove LEDs via expander pins, and used the expander to control an LCD.

## Exercise 6 – 4x4 Keypad via PCA9555

Implemented a debounced 4x4 keypad driver over the PCA9555, mapped key presses to ASCII, controlled LEDs based on keys, and built an electronic lock using a two‑digit code.

## Exercise 7 – DS1820 Temperature Sensor (1‑Wire)

Developed a 1‑wire driver for the DS1820/DS18B20 temperature sensor on PD4, read raw temperature values, and displayed signed temperatures or a “NO Device” message on the LCD.

## Exercise 8 – UART, ESP8266 & IoT Scenario

Combined UART communication with an ESP8266 and previous sensor modules to build an IoT “hospital” demo that forms a payload with vital signs and status and sends it to a server over WiFi.


