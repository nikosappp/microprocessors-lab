.include "m328PBdef.inc"

; -----the delay will be 100 ms-----
; with frequency 16Mhz we get 16mips

.equ delay = 100
.equ f_cpu = 16000000
.equ iterations = (f_cpu / 4000) - 3

; stack
ldi r24,LOW(RAMEND)
out SPL,r24
ldi r24,HIGH(RAMEND)
out SPH,r24
clr r24

; code to visualize the delay with the leds
ser r16
out ddrd, r16 ; portd becomes output
out portd,r16

main:
ldi r24,LOW(delay)
ldi r25,HIGH(delay)    ; 100ms delay (100 loops of 1ms)
nop
rcall wait_x_msec
nop
com r16
out portd,r16
jmp main


wait_x_msec:
ldi r26,LOW(iterations)   ; 1 cycle
ldi r27,HIGH(iterations)  ; 1 cycle
loop: 
	sbiw r26,1  ; 2 cycles
	brne loop   ; 2 cycles or 1 if its the last loop
	; this loop spends 16987 cycles
sbiw r24,1         ; 2 cycles
breq last          ; 1 cycle if it continues, 2 if ti branches
nop                ; 1
nop                ; 1
nop                ; 1
nop                ; 1
nop                ; 1
nop                ; 1
brne wait_x_msec   ; 2 cycles if it goes back, otherwise 1

; if it goes back to wait_x_msec it spends 16000 cycles until this point
; if it goes to last it spends 16993 cycles until this point


last:              
nop                ; 1
nop                ; 1
nop                ; 1
ret                ; 4 cycles

; 7 cycles + 16993 = 16000