# AVR Lab Exercises – ATmega328PB (2025–26)

This repository contains solutions and support code for a series of AVR microcontroller lab exercises using the ATmega328PB on the ntuAboard_G1 platform.[file:1][file:2][file:3][file:4][file:5][file:6][file:7][file:9]

## Technologies

- ATmega328 / ATmega328PB microcontroller.[file:1][file:2]  
- ntuAboard_G1 development board (16 MHz).[file:2][file:4]  
- Assembly (MPLAB X) and C (AVR-GCC / avr-libc).[file:1][file:3]  
- On-board peripherals: LEDs, DIP switches, potentiometers, LCD 2x16, CO sensor, 4x4 keypad, DS1820 temperature sensor, ESP8266 WiFi module, PCA9555 I/O expander.[file:4][file:5][file:6][file:7][file:9]

## Exercise 1 – Basic Assembly & Timing

- Implemented millisecond delay routines in assembly using 16‑bit counters (r24/r25) and `sbiw`-based timing loops.[file:1]  
- Built simple applications such as a stopwatch and logical operations to drive LEDs on PORTD.[file:1]  
- Practiced stack initialization, register usage, and bitwise manipulation on ATmega328.[file:1]

## Exercise 2 – External Interrupts & Debouncing

- Configured INT0 and INT1 on PD2/PD3 using EICRA, EIMSK, and EIFR registers.[file:3]  
- Wrote assembly and C ISRs to toggle LEDs on PORTB/PORTC with 500 ms and 2 s delays.[file:3]  
- Implemented debouncing for a push button on INT0 using a small delay and interrupt flag handling.[file:3]

## Exercise 3 – Timers, PWM & ADC

- Configured Timer1 (16‑bit) with prescalers via TCCR1B and TOIE1 in TIMSK1 to generate 3 s delays and overflow interrupts.[file:2]  
- Set up Fast PWM on OC1A (PB1) using OCR1A, TCCR1A/B to control LED brightness and duty cycle.[file:2]  
- Enabled the 10‑bit ADC, configured ADMUX (MUX bits, REFS) and ADCSRA/ADCSRB for proper sampling frequency, and read analog inputs (POTs) to drive LEDs and PWM duty cycle.[file:2]

## Exercise 4 – LCD 2x16 & CO Sensor

- Drove a 2x16 character LCD in 4‑bit mode using PORTD (data and control lines) with `lcdinit`, `lcdcommand`, and `lcddata` routines.[file:4]  
- Implemented the LCD initialization sequence (8‑bit to 4‑bit mode, function set, display on, clear, entry mode).[file:4]  
- Used the ADC to measure voltage from POT4 and a CO sensor, converting the 10‑bit ADC result to meaningful display values on the LCD and LEDs.[file:4]

## Exercise 5 – TWI (I²C) & PCA9555 I/O Expander

- Configured the TWI interface (I²C) on PC4 (SDA) and PC5 (SCL) using TWBR0, TWSR0, and TWCR0 for 100 kHz operation.[file:6]  
- Implemented low‑level TWI functions: `twi_init`, `twi_start`, `twi_rep_start`, `twi_write`, `twi_readAck`, `twi_readNak`, and `twi_stop` with status checking via TWSR0.[file:6]  
- Controlled the PCA9555 I/O expander by writing to configuration and output registers (REGCONFIGURATION0/1, REGOUTPUT0/1) to drive external LEDs and interfaces on ntuAboard_G1.[file:6]

## Exercise 6 – 4x4 Keypad via PCA9555

- Used the PCA9555 as a keypad interface, scanning rows and columns over TWI from the ATmega328PB.[file:5]  
- Implemented `scan_row`, `scan_keypad`, and rising‑edge detection logic to detect key presses reliably with 10–20 ms polling intervals.[file:5]  
- Mapped key codes to ASCII and built simple applications that control LEDs on PORTB based on keypad input (for example, enabling different LED patterns or modes).[file:5]

## Exercise 7 – DS1820 Temperature Sensor (1‑Wire)

- Interfaced the DS1820 temperature sensor on PD4 (J14, THERMOMETER switch) using the 1‑wire protocol.[file:7]  
- Implemented low‑level 1‑wire routines: `onewire_reset`, `onewire_transmit_bit/byte`, and `onewire_receive_bit/byte` with precise microsecond timing slots.[file:7]  
- Issued ROM and function commands (e.g., Skip ROM, Convert T, Read Scratchpad) to read the 16‑bit temperature value, processed sign and resolution, and displayed the temperature on LCD (with “NO Device” handling when absent).[file:7]

## Exercise 8 – UART, ESP8266 & IoT Hospital Scenario

- Configured UART (USART0) for 9600 baud, 8N1, using UBRR0H/UBRR0L and UCSR0A/B/C, and implemented `usart_init`, `usart_transmit`, and `usart_receive` in C.[file:9]  
- Connected the ESP8266 WiFi module via UART and controlled it using a simple command protocol (`ssid`, `password`, `baudrate`, `payload`, `url`, `connect`, `transmit`, `restart`, `debug`, `help`).[file:9]  
- Built an IoT “hospital” application:  
  - Read temperature from DS18B20 and pressure from POT0 (0–20 cm H₂O) on ntuAboard.[file:9]  
  - Formed JSON‑style payloads with fields like `temperature`, `pressure`, `team`, and `status` (for example, `NURSE CALL`, `CHECK PRESSURE`, `CHECK TEMP`, `OK`) depending on sensor thresholds.[file:9]  
  - Sent payloads to an HTTP endpoint (e.g., `http://192.168.1.250:5000/data`) via ESP8266 and displayed “Success/Fail” states on the LCD based on ESP responses and HTTP 200 OK status.[file:9]
