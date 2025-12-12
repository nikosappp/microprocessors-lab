.include "m328PBdef.inc"    ; ATmega328P microcontroller definitions

.equ FOSC_MHZ = 16           ; Microcontroller operating frequency in MHz
.equ DEL_MS   = 500          ; Delay in MS (valid number from 1 to 4095)
.equ DEL_NU   = FOSC_MHZ * DEL_MS   ; delay_mS routine: (1000*DEL_NU+6) cycles
.equ DEL_20    = FOSC_MHZ * 5


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
	; Initialize Stack Pointer
	ldi r24, LOW(RAMEND)
	out SPL, r24
	ldi r24, HIGH(RAMEND)
	out SPH, r24

	; Initialize PORTB, PORTC as output
	ser r26
	out DDRB, r26
	out DDRC, r26
	; Initialize PD1, PD3 as input 
	cbi DDRD, PD1        
	cbi DDRD, PD3
	sbi PORTD, PD1    ; enable pull up ressistors
	sbi PORTD, PD2 

	;Initialize counter to zero
	ldi counter,0

	; Interrupt activation on rising edge 
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

	; bouncing handler

	eifr_check:
	ldi r24,(1 << INTF1)
	out EIFR,r24           ; clear eifr
	ldi r24,low(DEL_20)
	ldi r25,high(DEL_20)   ; prepare 5ms delay
	rcall delay_ms	       ; call 5ms delay            
	sbic EIFR, INTF1       ; skip if EIFR is clear (continue)
	rjmp eifr_check        ; if EIFR not clear loop back, because bouncing effect is not handled

	; ---extra handling of the pin that causes the interrupt---

	sbis PIND, PD3       ; if the pin is released skip
        rjmp eifr_check      ; if bit is still pressed go loop back to the debouncer

	; --- this additional check fixes an issue observed during testing where a single interrupt was counted multiple times---

	; check if the PD1 is pressed

	sbis PIND,PD1			; skip next instruction if PD1 bit in PIN D is not pressed. 
	rjmp do_not_count		; if not pressed then do not count this interrupt
	
	; if PD1 is not pressed increase the counter and check if it esxceeds 31, then move to "leds" routine

	inc counter
	cpi counter,32
	brne leds
	clr counter

; activates the leds that in binary repressentation display the counter of interrupts

leds:
	mov r25,counter
	lsl r25
	out PORTC,r25
	pop r24
	out SREG,r24
	pop r24
	pop r25
	reti

; the program gets here only if the PD1 is pressed 
; and keeps the leds as they are since we do count the interrupt in this case

do_not_count:
	nop
	pop r24
	out SREG,r24
	pop r24
	pop r25
	reti


; code given by the exercise, that counts from 0 to 15 and displays the number to PB3-PB0 with 500ms delay

loop1:
    clr r26
loop2:
    out PORTB, r26

    ldi r24, low(DEL_NU)      
    ldi r25, high(DEL_NU)     ; prepare delay of 500ms
	rcall delay_ms
    inc r26

    cpi r26, 16               ; compare r26 with 16 
    breq loop1                ; if they are equal loop to "loop1" to start counting from 0 again
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
