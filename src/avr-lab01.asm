; *****************
; *               *
; *   AVR-lab01   *
; *               *
; *****************
; author: Krzysztof Grzeslo
; date: 24 August 2010

.nolist
.include "m8def.inc"
.list

.equ SYSFREQ = 16		; system frequency set to 16 MHz
.equ LEDDIR = DDRB		; direction of LED port register
.equ LEDOUT = PORTB		; output of LED port register
.equ KEYDIR = DDRD		; direction of keyboard port register
.equ KEYIN = PORTD		; input of keyboard port register
.equ KEYPIN = PIND		; input pin register
.equ KEYPRESSED	= 3		; line number for the "key pressed" line
.equ LCDDIR = DDRC		; direction of LCD port register
.equ LCDOUT = PORTC		; output of LCD port register
.equ RS = 5			; output of RS line for controlling LCD
.equ ENABLE = 4			; output of ENABLE line for LCD
.equ LCDDB4 = 3			; LCD interface line DB4
.equ LCDDB5 = 2			; LCD interface line DB5
.equ LCDDB6 = 1			; LCD interface line DB6
.equ LCDDB7 = 0			; LCD interface line DB7
.equ MENU_BEGIN_MARK = 0x00	; menu begin mark
.equ MENU_END_MARK = 0xFF	; menu end mark
.equ MENU_ITEMS_COUNT = 0x08	; number of menu entries

.def TEMP = R16			; R16 aka TEMP register
.def TEMPH = R17		; R17 aka TEMPH register
.def DELAY1 = R18		; R18 aka DELAY1 for delay loops
.def DELAY2 = R19		; R19 aka DELAY2 for delay loops
.def DELAY3 = R20		; R20 aka DELAY3 for delay loops
.def DELAY4 = R21		; R21 aka DELAY4 for delay loops
.def MENUL = R22		; R22 aka MENUL for displaying menu
.def MENUH = R23		; R23 aka MENUH for displaying menu

.dseg				; data segment (RAM)
.org 0x0060			; beginning of SRAM
KeyInfo: .BYTE 1		; variable KeyInfo in SRAM
MenuInfo: .BYTE 1		; vatiable MenuInfo in SRAM

.cseg				; program segment (Flash)
.org 0
	rjmp	Reset		; Reset vector
.org INT1addr
	rjmp	Keyboard	; keyboard vector (INT1)

Greeting:	.db	"avr-lab01", END_MARK
Menu1a:		.db	"lab 1a:bit move", END_MARK
Menu1b:		.db	"lab 1b:multiply", END_MARK
Menu1c:		.db	"lab 1c:rotate", END_MARK
Menu2:		.db	"lab 2: table", END_MARK
Menu3:		.db	"lab 3: key event", END_MARK
Menu4:		.db	"lab 4: T/C CTC", END_MARK
Menu5:		.db	"lab 5: T/C PWM", END_MARK
Menu6:		.db	"lab 6: LCD", END_MARK
; the number of items between begin and end mark must match MENU_ITEMS_COUNT
MenuItems:	.db	MENU_BEGIN_MARK, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, MENU_END_MARK

; initialization
Reset:
	ldi	TEMPH, high(RAMEND)
	ldi	TEMP, low(RAMEND)
	out	SPH, TEMPH
	out	SPL, TEMP	; initialize stack pointer

	rcall	Keyboard_init	; initialize keyboard
	rcall	LED_init	; initialize LEDs
	rcall	LCD_init	; initialize LCD display

	ldi ZH, high(Greeting<<1)
	ldi ZL, low(Greeting<<1)
	rcall LCD_show_string	; show greeting

	ldi	TEMPH, 100
	ldi	TEMP, 10
	rcall	Wait
	rcall	LCD_clear	; wait 1s and clear the display

	ldi	TEMP, 0x00	; turn on LEDs
	out	LEDOUT, TEMP
	ldi	TEMPH, 100
	ldi	TEMP, 10
	rcall	Wait		; wait 1s
	com	TEMP
	out	LEDOUT, TEMP	; turn LEDs off

	ldi	MENUL, 0x01
	sts	MenuInfo, MENUL	; initialize menu counter

	sei			; enable global interrupts

; main loop
Main:
; below command is uncommented, because MENUL has already the value of MenuInfo as 7 lines above
; it is left intentionally not to forget about it in case something new would be added before this line
;	lds	MENUL, MenuInfo	; load menu counter to register MENU
;	rcall	LCD_display_MenuInfo	; after start display current menuinfo
	rcall	ReadMenuItems	; after start display current menu item
Keypress:
	lds	TEMP, KeyInfo
	sbrs	TEMP, 7
	rjmp Keypress		; wait for key press

	lds	TEMP, KeyInfo	; save value of key in TEMP
	cbr	TEMP, 1<<7	; and clear key press bit

	cpi	TEMP, 0x01	; start of program
	brne	Loop1		; compare key value and
				; take proper action on key press
; action for key 1
	inc	MENUL		; add 1 to MENUL register
	rcall	ReadMenuItems	; call ReadMenuItems subroutine
	sts	MenuInfo, MENUL	; save the current menu item in the Menuinfo variable
	rcall	End

Loop1:
	cpi	TEMP, 0x02
	brne	Loop2
; action for key 2
	dec	MENUL		; substract 1 from MENUL register
	rcall	ReadMenuItems	; call ReadMenuItems subroutine
	sts	MenuInfo, MENUL	; save the current menu item in the Menuinfo variable
	rcall	End

Loop2:
	cpi	TEMP, 0x04
	brne	Loop3
; action for key 3
; ... put action here
	rcall	End

Loop3:
	cpi TEMP, 0x08
	brne	End
; action for key 4
; ... put action here
	rcall	End

End:
	lds	TEMP, KeyInfo
	sbrc	TEMP, 7
	rjmp	End		; wait until key is released
	rjmp	Main		; loop the entire program

.include "keyboard.inc"
.include "led.inc"
.include "lcd.inc"
.include "delay.inc"
.include "menu.inc"
