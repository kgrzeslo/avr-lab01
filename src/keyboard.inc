; *****************
; *               *
; * keyboard.inc  *
; *               *
; *****************
; author: Krzysztof Grzeslo
; date: 29 September 2010

Keyboard_init:			; initialize keyboard
	push	TEMP
	ldi	TEMP, 0x00		; KEYIN is input
	out	KEYDIR, TEMP
	ldi	TEMP, 0xF0		; Pins 4..7 (keyboard) pull-up
	out	KEYIN, TEMP

	; enable INT1 for keyboard
	in	TEMP, MCUCR 	; copy MCUCR to TEMP register for modifications
	cbr	TEMP, 1<<ISC11	; any logical change on int1 triggers interrupt
	sbr	TEMP, 1<<ISC10
	out	MCUCR, TEMP	; write modifications to MCUCR

	ldi	TEMP, 1<<INTF1	; clear INT1 interrupt request flag
	out	GIFR, TEMP

	in	TEMP, GICR
	sbr	TEMP, 1<<INT1
	out	GICR, TEMP	; enable INT1 interrupt
	pop	TEMP
	ret

; main part of the program for keyboard
Keyboard:
	; test loop for checking keyboard initialization
	push	TEMP		; put TEMP, TEMPH, SREG on stack
	push	TEMPH
	in	TEMP, SREG
	push	TEMP

	ldi	TEMP, 20	; wait 20 ms to see if key is pressed
	ldi	TEMPH, 1
	rcall	Wait

	sbic	KEYPIN, KEYPRESSED
	rjmp	KeyReleased	; go to the end if key is released

	ldi	TEMP, 20	; 2nd check whether key is pressed
	ldi	TEMPH, 1	; another 20 ms
	rcall	Wait

	sbic	KEYPIN, KEYPRESSED
	rjmp	KeyReleased	; go to the end if key is released

	sbis	KEYPIN, 4	; check keys one by one
	ldi	TEMP, 1		; and assign proper value
	sbis	KEYPIN, 5	; key 1 is on PIND4
	ldi	TEMP, 2		; key 2 is on PIND5
	sbis	KEYPIN, 6	; key 3 is on PIND6
	ldi	TEMP, 4		; key 4 is on PIND7
	sbis	KEYPIN, 7
	ldi	TEMP, 8
	sbr	TEMP, 1<<7	; set MSB of KeyInfo - "key pressed"
	sts	KeyInfo, TEMP
	rjmp	KeyReadEnd

KeyReleased:
	lds	TEMP, KeyInfo
	cbr	TEMP, 1<<7	; if key released clear MSB
	sts	KeyInfo, TEMP	; of KeyInfo - "key released"

KeyReadEnd:
	rcall Keyboard_init	; initialize keyboard again

	pop	TEMP
	out	SREG, TEMP
	pop	TEMPH
	pop	TEMP		; take SREG, TEMPH and TEMP from stack
	reti
