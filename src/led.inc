; *****************
; *               *
; *    led.inc    *
; *               *
; *****************
; author: Krzysztof Grzeslo
; date: 29 September 2010

LED_init:			; initialize LEDs
	push	TEMP
	ldi	TEMP, 0xFF	; LEDDIR is output
	out	LEDDIR, TEMP	; turn off all LEDs on LEDOUT
	out	LEDOUT, TEMP
	pop	TEMP
	ret
