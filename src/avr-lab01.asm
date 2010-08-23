; *****************
; *               *
; *   AVR-lab01   *
; *               *
; *****************
; author: Krzysztof Grzeslo
; date: 24.08.2010

.include "m8def.inc"

.def temp=R16

forever:
	nop ; infinite loop
	rjmp forever
