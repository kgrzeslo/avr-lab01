; **************
; * delay      *
; * subroutine *
; **************
; author: Rafal Baranowski (C) 2004
; modified by: Krzysztof Grzeslo
; date: 29 September 2010
; 
; Arguments for Wait subroutine:
; TEMP - delay value in ms (256 ms for zero) - R16
; TEMPH - delay multiplier (256x for zero) - R17
;
; Arguments for Wait_us subroutine:
; TEMP - delay value in 10x us ( 2560 us for zero)
; Obligatory constants:
; SYSFREQ - system frequency in MHz
;
; usage (delay 2*25 ms = 50 ms):
; ldi TEMP, 25
; ldi TEMPH, 2
; rcall Wait
;
; usage (delay 300 us):
; ldi TEMP, 30
; rcall Wait

Wait:
	push	DELAY1
	push	DELAY2
	push	DELAY3
	push	DELAY4

	mov	DELAY3, TEMP
	mov	DELAY4, TEMPH
	ldi	DELAY1, SYSFREQ

Wait0:
	mov	TEMPH, DELAY4
Wait1:
	mov	TEMP, DELAY3
Wait2:
	ldi	DELAY2, 249
	nop
Wait3:
	nop
	dec	DELAY2
	brne	Wait3
	dec	TEMP
	brne	Wait2
	dec	TEMPH
	brne	Wait1
	dec	DELAY1
	brne	Wait0

	pop	DELAY4
	pop	DELAY3
	pop	DELAY2
	pop	DELAY1
	ret

Wait_us:
	push	TEMPH
	push	DELAY1

	mov	DELAY1, TEMP
	ldi	TEMPH, SYSFREQ

Wait_us0:
	mov	TEMP, DELAY1
Wait_us1:
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	dec	TEMP
	brne	Wait_us1
	dec	TEMPH
	brne	Wait_us0

	pop	DELAY1
	pop	TEMPH
	ret
