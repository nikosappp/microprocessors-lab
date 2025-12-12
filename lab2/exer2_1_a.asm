.include "m328PBdef.inc"    ; ATmega328P microcontroller definitions

.equ FOSC_MHZ = 16           ; Microcontroller operating frequency in MHz
.equ DEL_MS   = 50          ; Delay in MS (valid number from 1 to 4095)
.equ DEL_NU   = FOSC_MHZ * DEL_MS   ; delay_mS routine: (1000*DEL_NU+6) cycles

.def counter = r16
.equ PD0 = 0
.equ PD1 = 1
.equ PD2 = 2
.equ PD3 = 3

.org 0x0
rjmp reset
.org 0x4
rjmp ISR1


reset:
; Init Stack Pointer
ldi r24, LOW(RAMEND)
out SPL, r24
ldi r24, HIGH(RAMEND)
out SPH, r24

; Init PORTB, PORTC as output
ser r26
out DDRB, r26
out DDRC, r26
; Init PD1, PD3 as input 
cbi DDRD, PD1        
cbi DDRD, PD3 

;Init counter to zero
ldi counter,0

; Interrupt on rising edge 
ldi r24, (1 << ISC11) | (1 << ISC10)
sts EICRA,r24
; Enable the INT1 interrupt (pd3)
ldi r24, (1 << INT1)
out EIMSK, r24
sei
rjmp loop1

ISR1:
; save registers 
push r25
push r24
in r24,SREG
push r24
; check if the PD1 is pressed
sbic PIND,PD1			; skip next instruction if PD1 bit in PIN D is pressed. 
rjmp do_not_count		; if not pressed then do not count this interrupt
; if PD1 is not pressed increase the counter and check if it esxceeds 31
inc counter
cpi counter,32
brne leds
clr counter

leds:
mov r18,counter
lsl r18
out PORTC,r18
pop r24
out SREG,r24
pop r24
pop r25
reti

do_not_count:
pop r24
out SREG,r24
pop r24
pop r25
reti


; code given by the exercise that counts from 0 to 15 and displays the number to PB3-PB0 with 500ms delay
loop1:
    clr r26
loop2:
    out PORTB, r26

    ldi r24, low(DEL_NU)      ; Set delay (number of cycles)
    ldi r25, high(DEL_NU)
	nop
	rcall delay_ms
	nop
    inc r26

    cpi r26, 16               ; compare r26 with 16
    breq loop1
    rjmp loop2

; delay of 1000*F1+6 cycles (almost equal to 1000*F1 cycles)
delay_ms:

; total delay of next 4 instruction group = 1+(249*4-1) = 996 cycles
    ldi r23, 249             ; (1 cycle)
loop_inn:
    dec r23                  ; 1 cycle
    nop                      ; 1 cycle
    brne loop_inn            ; 1 or 2 cycles

    sbiw r24, 1              ; 2 cycles
    brne delay_ms            ; 1 or 2 cycles

    ret                      ; 4 cycles
