# AVR Lab Exercises – ATmega328PB (2025–26)

This repository contains solutions and support code for a series of AVR microcontroller lab exercises using the ATmega328PB on the ntuAboard_G1 platform.
<img width="828" height="592" alt="image" src="https://github.com/user-attachments/assets/b57bd34f-9e6d-458a-8c3b-b69cacd37afb" />

## Exercise 1 – Basic Assembly & Timing

Worked with AVR assembly on the ATmega328 to initialize the stack, manipulate registers and ports, and implement millisecond delay routines used in simple LED timing applications.

## Exercise 2 – External Interrupts & Debouncing

Configured external interrupts (INT0/INT1) on PD2/PD3 and wrote ISRs in assembly and C to control LEDs, including basic button debouncing for reliable input handling.

## Exercise 3 – Timers, PWM & ADC

Used Timer1 for time delays and PWM generation, and configured the 10‑bit ADC to read analog inputs (potentiometers) and use these values to influence LED behavior and PWM duty cycle.

## Exercise 4 – LCD 2x16 & CO Sensor

Interfaced a 2x16 character LCD in 4‑bit mode to display data, and used the ADC to measure voltages from a potentiometer and a CO sensor, presenting the results on the LCD and LEDs.

## Exercise 5 – TWI (I²C) & PCA9555

Set up the TWI (I²C) interface and communicated with the PCA9555 I/O expander to configure pins and drive external LEDs and signals on the ntuAboard_G1.

## Exercise 6 – 4x4 Keypad via PCA9555

Connected a 4x4 keypad through the PCA9555 over TWI, implemented keypad scanning and key detection, and used key presses to control LED patterns and simple application logic.

## Exercise 7 – DS1820 Temperature Sensor (1‑Wire)

Implemented the 1‑wire protocol on a GPIO pin to communicate with the DS1820 temperature sensor and read, process, and display temperature measurements.

## Exercise 8 – UART, ESP8266 & IoT Scenario

Configured UART communication with an ESP8266 WiFi module and built a small IoT “hospital” demo that reads local sensor data, formats a payload, and transmits it to a remote HTTP server while reporting status on the LCD.
