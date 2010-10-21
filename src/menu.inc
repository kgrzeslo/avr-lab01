; *****************
; *               *
; *   menu.inc    *
; *               *
; *****************
; author: Krzysztof Grzeslo
; date: 21 October 2010
; this file is event handler for the menus we use

ReadMenuItems:
	push	TEMP
	clr	MENUH		; clear the higher menu register
	ldi	ZH, high(MenuItems<<1)	; address the MenuItems table
	ldi	ZL, low(MenuItems<<1)	; "
	add	ZL, MENUL	; add menu counter offset
	add	ZH, MENUH	; add nothing to higher byte of the index register
	lpm	TEMP, Z
	cpi	TEMP, MENU_BEGIN_MARK	; check for menu begin mark and branch
	breq	ReadMenuItemsBegin
	cpi	TEMP, MENU_END_MARK	; check for menu end mark and branch
	breq	ReadMenuItemsEnd
	rjmp	MenuEnd
ReadMenuItemsBegin:
	ldi	MENUL, MENU_ITEMS_COUNT	; when beginning is reached go to the end
	add	ZL, MENUL	; of the menu
	rjmp	MenuEnd
ReadMenuItemsEnd:
	ldi	MENUL, MENU_ITEMS_COUNT	; when end is reached go to the beginning
	sub	ZL, MENUL	; of the menu

MenuEnd:
	pop	TEMP
	lpm	MENUL, Z	; read the value of the menu item from table

; running the labs from menu
	cpi	MENUL, 0x01	; check the value of menu we are currently in
	breq	Lab1a		; and branch to the correct lab label
	cpi	MENUL, 0x02
	breq	Lab1b
	cpi	MENUL, 0x03
	breq	Lab1c
	cpi	MENUL, 0x04
	breq	Lab2
	cpi	MENUL, 0x05
	breq	Lab3
	cpi	MENUL, 0x06
	breq	Lab4
	cpi	MENUL, 0x07
	breq	Lab5
	cpi	MENUL, 0x08
	breq	Lab6
	rjmp	Finish		; jump to finish just in case something goes wring
Lab1a:				; labels created for the branch commands above 
	rjmp	Lab1aHandle	; to work correctly due to +-64 words branches
	rjmp	Finish		; jump to finish
Lab1b:
	rjmp	Lab1bHandle
	rjmp	Finish		; jump to finish
Lab1c:
	rjmp	Lab1cHandle
	rjmp	Finish		; jump to finish
Lab2:
	rjmp	Lab2Handle
	rjmp	Finish		; jump to finish
Lab3:
	rjmp	Lab3Handle
	rjmp	Finish		; jump to finish
Lab4:
	rjmp	Lab4Handle
	rjmp	Finish		; jump to finish
Lab5:
	rjmp	Lab5Handle
	rjmp	Finish		; jump to finish
Lab6:
	rjmp	Lab6Handle
	rjmp	Finish		; jump to Finish just in case something goes wrong

Lab1aHandle:			; subroutine for lab1a
	push	ZH			
	push	ZL
	push	TEMP
	push	TEMPH
	rcall	LCD_Clear
	ldi ZH, high(Menu1a<<1)
	ldi ZL, low(Menu1a<<1)	; display "menu 1a"
	rcall LCD_show_string
; put here the procedure of the current lab
; ...
Lab1aLoop:
	ldi	TEMP, 0x01
Lab1aLoop1:
	com	TEMP
	out	LEDOUT, TEMP
	rcall	Wait1s
	com	TEMP
	lsl	TEMP
	cpi	TEMP, 0x40
	brne	Lab1aLoop1
	breq	Lab1aLoop
	pop	TEMPH
	pop	TEMP
	pop	ZL
	pop	ZH
	ret

Lab1bHandle:
	push	ZH			
	push	ZL
	push	TEMP
	push	TEMPH
	rcall	LCD_Clear
	ldi ZH, high(Menu1b<<1)
	ldi ZL, low(Menu1b<<1)	; display "menu 1b"
	rcall LCD_show_string
; put here the procedure of the current lab
; ...
	pop	TEMPH
	pop	TEMP
	pop	ZL
	pop	ZH
	ret

Lab1cHandle:
	push	ZH			
	push	ZL
	rcall	LCD_Clear
	ldi ZH, high(Menu1c<<1)
	ldi ZL, low(Menu1c<<1)	; display "menu 1c"
	rcall LCD_show_string
; put here the procedure of the current lab
; ...
	pop	ZL
	pop	ZH
	ret

Lab2Handle:
	push	ZH			
	push	ZL
	rcall	LCD_Clear
	ldi ZH, high(Menu2<<1)
	ldi ZL, low(Menu2<<1)	; display "menu 2"
	rcall LCD_show_string
; put here the procedure of the current lab
; ...
	pop	ZL
	pop	ZH
	ret

Lab3Handle:
	push	ZH			
	push	ZL
	rcall	LCD_Clear
	ldi ZH, high(Menu3<<1)
	ldi ZL, low(Menu3<<1)	; display "menu 3"
	rcall LCD_show_string
; put here the procedure of the current lab
; ...
	pop	ZL
	pop	ZH
	ret

Lab4Handle:
	push	ZH			
	push	ZL
	rcall	LCD_Clear
	ldi ZH, high(Menu4<<1)
	ldi ZL, low(Menu4<<1)	; display "menu 4"
	rcall LCD_show_string
; put here the procedure of the current lab
; ...
	pop	ZL
	pop	ZH
	ret

Lab5Handle:
	push	ZH			
	push	ZL
	rcall	LCD_Clear
	ldi ZH, high(Menu5<<1)
	ldi ZL, low(Menu5<<1)	; display "menu 5"
	rcall LCD_show_string
; put here the procedure of the current lab
; ...
	pop	ZL
	pop	ZH
	ret

Lab6Handle:
	push	ZH			
	push	ZL
	rcall	LCD_Clear
	ldi ZH, high(Menu6<<1)
	ldi ZL, low(Menu6<<1)	; display "menu 6"
	rcall LCD_show_string
; put here the procedure of the current lab
; ...
	pop	ZL
	pop	ZH
	ret

Finish:
	ret
