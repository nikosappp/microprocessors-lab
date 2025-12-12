# AVR Lab Exercises – ATmega328PB 

Solutions for the 7th‑semester Microprocessors Lab course at the National Technical University of Athens (2025–26).


This repository contains solutions and support code for a series of AVR microcontroller lab exercises using the ATmega328PB on the ntuAboard_G1 platform.
<img width="828" height="592" alt="image" src="https://github.com/user-attachments/assets/b57bd34f-9e6d-458a-8c3b-b69cacd37afb" />

## Exercise 1 – Basic Assembly & Timing

Worked with AVR assembly on the ATmega328PB to initialize the stack, manipulate registers and ports, and implement millisecond delay routines used in simple LED timing applications.

## Exercise 2 – External Interrupts & Debouncing

Focuses on configuring external interrupts (INT0/INT1), handling button inputs with debouncing, and implementing simple automation logic that controls LEDs and timing behavior using ISRs in both Assembly and C.

## Exercise 3 – Timers, PWM & ADC

Configures Timer1 to generate a high‑frequency PWM signal on PB1 to control LED brightness, then extends this with C code that samples analog signals via the 10‑bit ADC to both classify the measured value using PORTD LEDs and implement two brightness control modes: one using push buttons (PB4/PB5) and one using the POT1 potentiometer.

## Exercise 4 – LCD 2x16 & CO Sensor

Uses the ADC and a 2x16 LCD to periodically sample a voltage from POT4 (and later a CO sensor on A3), convert the raw 10‑bit ADC value into a voltage, and display it with two decimal digits, then extends this in C to implement a CO monitoring application that polls the sensor every ~100 ms and drives LEDs and LCD messages (GAS DETECTED / CLEAR) based on a 75 ppm threshold.

## Exercise 5 – TWI (I²C) & PCA9555

Uses TWI (I²C) to control the PCA9555 I/O expander from the ATmega328PB in C, implementing Boolean logic functions on PORTB inputs and driving IO0_x pins to LEDs, reading keypad-like inputs through IO1_x to light specific LEDs based on pressed keys, and finally driving a 2x16 LCD via the expander to display custom text (name and surname).

## Exercise 6 – 4x4 Keypad via PCA9555

Implements a full 4x4 keypad interface in C using the PCA9555 (row/column scanning, debounced rising‑edge detection and ASCII mapping), then uses it to control LEDs on PORTB, print the last pressed key on a 2x16 LCD, and build an “electronic lock” that checks a two‑digit team code and drives LEDs with different timing patterns for correct and incorrect entries.

## Exercise 7 – DS1820 Temperature Sensor (1‑Wire)

Implements a 1‑wire software driver in C for the DS1820/DS18B20 temperature sensor on PD4 (reset, transmit/receive bit and byte, ROM and function commands), provides a routine that returns the raw 16‑bit two’s‑complement temperature value or 0x8000 if no device is present, and then uses this routine to display the signed temperature (‑55 °C to +125 °C) or “NO Device” on a 2x16 LCD.

## Exercise 8 – UART, ESP8266 & IoT Scenario

Implements a C program that drives the ESP8266 over UART to step through the full IoT workflow: first sending `connect` and `url` commands and showing `1.Success` / `1.Fail` and `2.Success` / `2.Fail` on the LCD based on the module’s replies, then reading temperature from DS18B20 and pressure from POT0 to build a JSON‑style payload with fields for vital signs and a `status` field (`NURSE CALL`, `CHECK PRESSURE`, `CHECK TEMP`, `OK`) determined by keypad input and threshold checks, and finally issuing `payload` and `transmit` commands so the data is posted to the server while the LCD displays `3.Success` / `3.Fail` and a `4.Server response` message (e.g. HTTP 200 OK), before repeating the whole 8.1–8.3 sequence.
