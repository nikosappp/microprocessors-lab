.include "m328pbdef.inc"

.equ FOSC_MHZ = 16                  ; Microcontroller operating frequency in MHz         
.equ DEL_4000   = FOSC_MHZ * 4000   
.equ DEL_1000    = FOSC_MHZ * 1000
.equ DEL_3000    = FOSC_MHZ * 3000
.equ DEL_5 =FOSC_MHZ* 5

.equ PD3 = 3
.def output = r17

.org 0x0
rjmp reset
.org 0x4
pop r16      ; remove from the stack the return address of the interrupt
pop r16
rcall ISR1   ; now rcall INT1. 
rjmp waiting

; The reason we popped the address that the hardware pushed by the time it caught the interrupt,
; is because we are implementing a nested interrupt too. If we do not do this, when the nested interrupt 
; finishes, it would return to the previous ISR instead of resuming to the main program. 
; This will lead to continue executing leftover code from the earlier ISR, resulting in longer led activation times.

; we use rcall and not jmp because we lost the return address, 
; so when it will finish it will return here and get to "waiting"

reset:

	; Initialize Stack Pointer

	ldi r24, LOW(RAMEND)
	out SPL, r24
	ldi r24, HIGH(RAMEND)
	out SPH, r24

	; Initialize PORTB as output

	ser r24
	out DDRB, r24

	; Init PD3 as input        
	
	cbi DDRD, PD3 

	; Interrupt on rising edge 

	ldi r24, (1 << ISC11) | (1 << ISC10)
	sts EICRA,r24

	; Enable the INT1 interrupt (pd3)

	ldi r24, (1 << INT1)
	out EIMSK, r24
	sei

; now i want a waiting function. a simple loop that does almost nothing but expecting an interrupt

waiting:
	clr output
	out portb, output
	sei                    ;we have to re-enable the interrupts because we returned with ret and not reti
	rjmp waiting

ISR1:
	; bouncing handler
	eifr_check:
	ldi r24,(1 << INTF1)
	out EIFR,r24           ; clear eifr

	ldi r24,low(DEL_5)
	ldi r25,high(DEL_5)    ; prepare 5ms delay
	rcall delay_ms	       ; call 5ms delay

	in r25, EIFR           ; check if it is still 1. if yes go back, if not continue 
	sbic EIFR, INTF1
	rjmp eifr_check

	sei                    ; I need to reset the interrupts in order to catch the second signal
	cpi output,0b00001000    
	breq update            ; if we are within the 4 seconds we need to update the timer
	cpi output,0b00111111      
	breq update            ; if we are within the 1 second where PB0-PB5 are on we need to update the 

	ldi r24,LOW(DEL_4000)  
	ldi r25,HIGH(DEL_4000)   ; if the code does not brach prepare teh 4s delay 
	ldi output,0b00001000     
	out portb, output        ; activate PB3
	rcall delay_ms           ; for 4 seconds
	ret

update:

	ldi r24,LOW(DEL_1000)   
	ldi r25,HIGH(DEL_1000)   
	ldi output,0b00111111     
	out portb, output         ; turn PB0-PB5 on
	rcall delay_ms            ; for 1 second

	; and now turn on only PB3 again for 4-1=3 seconds
	ldi r24,LOW(DEL_3000)   
	ldi r25,HIGH(DEL_3000)
	ldi output,0b00001000
	out portb, output         ; turn on PB3
	rcall delay_ms            ; for 3 seconds
	ret  
	                   
; we use ret and not reti because reti is searching the stack for an addres pushed by the interrupt,
; which we removed by the time the interrupt got caught



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

