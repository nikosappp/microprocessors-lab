.include "m328pbdef.inc"

; T=0 right shift
; T=1 left_shift

; stack
ldi r24,LOW(RAMEND)
out SPL,r24
ldi r24,HIGH(RAMEND)
out SPH,r24
clr r24

start:
ldi r24,LOW(20)
ldi r25,HIGH(20)    ; 2000ms delay
ser r16
out ddrd,r16
ldi r16,0b00000001   ; or 0xfe
bset 6               ; start with left shift 
bclr 0               ; clear carry

left_shift:
out portd,r16
nop
rcall wait_x_msec    ; call 2 sec delay
nop
ldi r24,LOW(20)
ldi r25,HIGH(20)
rol r16
brcs change_to_right
rjmp left_shift

change_to_right:
bclr 6               ; change direction to right
bclr 0               ;clear carry
ldi r24,LOW(10)
ldi r25,HIGH(10) 
nop   
rcall wait_x_msec    ;  another 1000ms delay
nop
ldi r16,0b01000000
ldi r24,LOW(20)
ldi r25,HIGH(20)   ; bring back the 2000ms delay
rjmp right_shift

right_shift:
out portd,r16
nop
rcall wait_x_msec
nop
ldi r24,LOW(20)
ldi r25,HIGH(20)
ror r16
brcs change_to_left
rjmp right_shift

change_to_left:
bset 6               ; change direction to left
bclr 0               ; clear carry
ldi r24,LOW(10)
ldi r25,HIGH(10) 
nop   
rcall wait_x_msec    ;  another 1000ms delay
nop
ldi r16,0b00000010
ldi r24,LOW(20)
ldi r25,HIGH(20)   ; bring back the 2000ms delay
rjmp right_shift


; --- delay routine ---

wait_x_msec:
ldi r26,LOW(15988)   ; 1 cycle
ldi r27,HIGH(15988)  ; 1 cycle
loop: 
	sbiw r26,4  ; 2 cycles
	brne loop   ; 2 cycles or 1 if its the last loop
	; this loop spends 7987 cycles
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
; if it goes to last it spends 15993 cycles until this point
last:              
nop                ; 1
nop                ; 1
nop                ; 1
ret                ; 4 cycles
; 7 cycles + 15993 = 16000











