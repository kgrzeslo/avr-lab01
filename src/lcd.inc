; SUBROUTINES FOR LCD IN 4-BIT MODE
; Rafal Baranowski (C) 2004
; modified by Krzysztof Grzeslo
;
; procedures labels:
; 	LCD_init		- display initialization
;	LCD_instruction	- send instruction to display
;	LCD_save		- send data to display
;	CGRAM_save	- copy table to CGRAM
;	LCD_clear		- clear display
;	LCD_cursor_return_home	- cursor back
;	LCD_space		- generate space
;	LCD_show_byte	- show byte value
;	LCD_show_string	- show string
;
; obligatory constants:
;	LCDDIR		- direction of LCD port register
;	LCDOUT		- output of LCD port register 
;	RS		- output of RS line for controlling LCD
;
;	LCD_ENABLE	- output of ENABLE line for LCD
;	LCD_LCDDB4	- LCD interface line DB4
;	LCD_LCDDB5	- LCD interface line DB5
;	LCD_LCDDB6	- LCD interface line DB6
;	LCD_LCDDB7	- LCD interface line DB7
;
; obligatory additional subroutines:
;	Wait_us, Wait	- from file delay.inc

;****************************************************************
; Initialize LCD subroutine
;
LCD_init:
	push	TEMP
	push	TEMPH

	sbi	LCDDIR, RS	; LCD ports are output
	sbi	LCDDIR, ENABLE
	sbi	LCDDIR, LCDDB4
	sbi	LCDDIR, LCDDB5
	sbi	LCDDIR, LCDDB6
	sbi	LCDDIR, LCDDB7
		
	cbi	LCDOUT, RS	; sending instructions
	cbi	LCDOUT, ENABLE

	ldi	TEMP, 45
	ldi	TEMPH, 1
	rcall	Wait		; waiting 45 ms in case power
				; is just turned on

	sbi	LCDOUT, ENABLE
	sbi	LCDOUT, LCDDB4
	sbi	LCDOUT, LCDDB5
	cbi	LCDOUT, LCDDB6
	cbi	LCDOUT, LCDDB7
	nop
	nop
	cbi	LCDOUT, ENABLE	; set DB7..4=0011 and confirm with
				; with falling edge on ENABLE
	ldi	TEMP, 5
	ldi	TEMPH, 1
	rcall	Wait		; waiting 5 ms

	ldi	TEMPH, 2
LCD_init_1:
	sbi	LCDOUT, ENABLE
	ldi	TEMP, 1
	rcall	Wait_us
	cbi	LCDOUT, ENABLE	
	ldi	TEMP, 20
	rcall	Wait_us
	dec	TEMPH
	brne	LCD_init_1		; retransmission of value (DB7..4=0011)
       					; with 200 us delay
					; (program initialization of display)

	sbi	LCDOUT, ENABLE
	cbi	LCDOUT, LCDDB4
	ldi	TEMP, 1
	rcall	Wait_us
	cbi	LCDOUT, ENABLE	
	ldi	TEMP, 10
	rcall	Wait_us		; sending DB7..4=0010
				; (4-bit interface)

	ldi	TEMP, (1<<LCD_SET)|(1<<LCD_SET_4BIT)|(1<<LCD_SET_2LINES)|(1<<LCD_SET_5x8)
	rcall	LCD_instruction	; 4-bit interface, 2 lines, 5x8 dots

	ldi	TEMP, (1<<LCD_MODE)|(1<<LCD_M_INC)
	rcall	LCD_instruction	; incrementing mode while saving

	ldi	TEMP, (1<<LCD_CONTROL)|(1<<LCD_C_DISPLAY_ON); |(1<<LCD_C_CURSOR_ON)|(1<<LCD_C_BLINKING_ON)
	rcall	LCD_instruction	; display on, inactive and not blinking cursor - commented out

	sbi	LCDOUT, RS	; sending data

	rcall	LCD_clear

	pop	TEMPH
	pop	TEMP
	ret


;****************************************************************
;	SENDING INSTRUCTION TO DISPLAY SUBROUTINE
;
; arguments:
;	TEMP	- instruction code
;
; example:
;				; turn on cursor
;	ldi	TEMP, (1<<LCD_CONTROL)|(1<<LCD_C_DISPLAY_ON)|(1<<LCD_C_CURSOR_ON)|(1<<LCD_C_BLINKING_OFF)
;	rcall	LCD_instruction
;

				; symbolic labels for bits
				; of instructions' codes
.EQU	LCD_DISPLAY_CLEAR	= 0	; clear display, cursor return home
.EQU	LCD_DISPLAY_RETURN_HOME	= 1	; cursor return home

.EQU	LCD_MODE	= 2	; change memory save mode
.EQU	LCD_M_INC	= 1	; increment address with each save
.EQU	LCD_M_DEC	= 2	; decrement address with each save
.EQU	LCD_M_MOVE_WINDOW	= 0	; move window with each save

.EQU	LCD_CONTROL	= 3	; display control
.EQU	LCD_C_DISPLAY_ON	= 2	; display on
.EQU	LCD_C_DISPLAY_OFF	= 3	; display off
.EQU	LCD_C_CURSOR_ON	= 1	; cursor on
.EQU	LCD_C_CURSOR_OFF	= 3	; cursor off
.EQU	LCD_C_BLINKING_ON	= 0	; cursor blinking on
.EQU	LCD_C_BLINKING_OFF	= 3	; cursor blinging off

.EQU	LCD_MOVE	= 4	; move
.EQU	LCD_MOVE_CURSOR	= 4	; move cursor
.EQU	LCD_MOVE_WINDOW	= 3	; move window
.EQU	LCD_LEFT	= 4	; move left
.EQU	LCD_RIGHT	= 2	; move right

.EQU	LCD_SET	= 5	; interface and display settings
.EQU	LCD_SET_8BIT	= 4	; 8-bit interface
.EQU	LCD_SET_4BIT	= 5	; 4-bit interface
.EQU	LCD_SET_2LINES	= 3	; 2-line display
.EQU	LCD_SET_1LINE	= 5	; 1-line display
.EQU	LCD_SET_5x10	= 2	; characters 5x10 dots
.EQU	LCD_SET_5x8	= 5	; characters 5x8 dots

.EQU	LCD_CGADR	= 6	; CGRAM address change
.EQU	LCD_DDADR	= 7	; DDRAM address change

LCD_instruction:
	cbi	LCDOUT, RS	; sending instructions
	rcall	LCD_save	; send instruction from TEMP register
	sbi	LCDOUT, RS	; sending data
	ret


;***************************************************************
; Sending byte subroutine
;
; arguments:
;	TEMP	- data to be sent (e.g. character code)
;
LCD_save:
	sbi	LCDOUT, ENABLE	; ENABLE is high
	cbi	LCDOUT, LCDDB4
	cbi	LCDOUT, LCDDB5
	cbi	LCDOUT, LCDDB6
	cbi	LCDOUT, LCDDB7	; DB7..4 are low
	sbrc	TEMP, 7
	sbi	LCDOUT, LCDDB7 
	sbrc	TEMP, 6
	sbi	LCDOUT, LCDDB6 
	sbrc	TEMP, 5
	sbi	LCDOUT, LCDDB5
	sbrc	TEMP, 4
	sbi	LCDOUT, LCDDB4	; setting these lines of DB7..4,
	nop			; that corespond to the high values
				; of bits 7..4 in TEMP register
	cbi	LCDOUT, ENABLE	; ENABLE is low - confirmation
				;   (sent Most Significant Nibble)

	nop
	nop			; delay (2 clock cycles)

	sbi	LCDOUT, ENABLE
	cbi	LCDOUT, LCDDB4
	cbi	LCDOUT, LCDDB5
	cbi	LCDOUT, LCDDB6
	cbi	LCDOUT, LCDDB7
	sbrc	TEMP, 3
	sbi	LCDOUT, LCDDB7 
	sbrc	TEMP, 2
	sbi	LCDOUT, LCDDB6
	sbrc	TEMP, 1
	sbi	LCDOUT, LCDDB5
	sbrc	TEMP, 0
	sbi	LCDOUT, LCDDB4	; setting these lines of DB7..4,
	nop			; that corespond to the high values
				; of bits 3..0 in TEMP register
	cbi	LCDOUT, ENABLE	; ENABLE is low - confirmation
				;   (sent LS Nibble)
		
	ldi	TEMP, 5
	rcall	Wait_us		; delay 50 us

	ret


;****************************************************************
; Subroutine for copying table to CGRAM
;
; Note by Krzysztof Grzeslo: this procedure is not used in AVR-lab01 project
;
; arguments:
;	Z	- address of first cell in table in program memory
;
;CGRAM_save:
;	push	TEMP
;	push	TEMPH
;
;	ldi	TEMP, 0|(1<<LCD_CGADR)
;	rcall	LCD_instruction	; enter CGRAM addressing mode
;				; starting with address zeto
;
;	ldi	TEMPH, 64		; initialize loop counter (64 times)
;CGRAM_save_1:
;	lpm	TEMP, Z+
;	rcall	LCD_save		; saving in CGRAM next values of
;					; table in program memory
;					; pointed by intex register Z
;	dec	TEMPH
;	brne	CGRAM_save_1	; end of loop
;
;	ldi	TEMP, 0|(1<<LCD_DDADR)
;	rcall	LCD_instruction	; enter DDRAM addressing mode
;				; (setting cursor position (1,1))
;
;	pop	TEMPH
;	pop	TEMP
;	ret
;
;			; polish characters
;Polskie_znaki:
;.db 0b00000000,0b00000000,0b00001110,0b00000001,0b00001111,0b00010001,0b00001111,0b00000011 ; "¹"
;.db 0b00000010,0b00000100,0b00001110,0b00010000,0b00010000,0b00010001,0b00001110,0b00000000 ; "æ"
;.db 0b00000000,0b00000000,0b00001110,0b00010001,0b00011111,0b00010000,0b00001110,0b00000011 ; "ê"
;.db 0b00001100,0b00000100,0b00000110,0b00001100,0b00000100,0b00000100,0b00001110,0b00000000 ; "³"
;.db 0b00000010,0b00000100,0b00010110,0b00011001,0b00010001,0b00010001,0b00010001,0b00000000 ; "ñ"
;.db 0b00000010,0b00000100,0b00001110,0b00010001,0b00010001,0b00010001,0b00001110,0b00000000 ; "ó"
;.db 0b00000010,0b00000100,0b00001110,0b00010000,0b00001110,0b00000001,0b00011110,0b00000000 ; "œ"
;.db 0b00000100,0b00000000,0b00011111,0b00000010,0b00000100,0b00001000,0b00011111,0b00000000 ; "¿"
;
;			; labels for polish characters after loading
;			; CGRAM with values of table above
;.EQU POL_A	= 0	;   "¹"
;.EQU POL_C	= 1	;   "æ"
;.EQU POL_E	= 2	;   "ê"
;.EQU POL_L	= 3	;   "³"
;.EQU POL_N	= 4	;   "ñ"
;.EQU POL_O	= 5	;   "ó"
;.EQU POL_S	= 6	;   "œ"
;.EQU POL_Z	= 7	;   "¿"


;***************************************************************
; Cubroutine for cleaning display
;
LCD_clear:
	push	TEMP

	ldi	TEMP, (1<<LCD_DISPLAY_CLEAR)
	rcall	LCD_instruction	; clear display and return home
	ldi	TEMP, 180
	rcall	Wait_us		; delay 1,8 ms

	pop	TEMP
	ret


;***************************************************************
; Subroutine for cursor returning home
;
LCD_cursor_return_home:
	push	TEMP

	ldi	TEMP, (1<<LCD_DISPLAY_RETURN_HOME)
	rcall	LCD_instruction	; cursor return home
	ldi	TEMP, 180
	rcall	Wait_us		; delay 1,8 ms

	pop	TEMP
	ret


;***************************************************************
; Subroutine for spaces
;
; Parametry:
;	TEMP	- spaces to be displayed count
;
LCD_space:
	push	TEMPH

	mov	TEMPH, TEMP	; copy count of spaces to TEMPH
				; (loop counter initialization)
LCD_space_1:
	ldi	TEMP, ' '	; TEMP contains ASCII code of space
	rcall	LCD_save	; display empty character (character in TEMP)
	dec	TEMPH		; decrease loop counter.
	brne	LCD_space_1	; loop

	pop	TEMPH
	ret


;***************************************************************
; Subroutine for displaying byte
;
; arguments:
;	TEMP	- value to be displayed
;
LCD_show_byte:
	push	TEMP
	push	TEMPH

	mov	TEMPH, TEMP		; copy displayed value to TEMPH
	clt				; clear T flag, that is set when
					; the first non-zero digit is hit

	clr	TEMP			; loop for counting digit of hundreds
LCD_s_b_1:
	subi	TEMPH, 100
	brlo	LCD_s_b_2
	inc	TEMP
	rjmp	LCD_s_b_1
LCD_s_b_2:
	subi	TEMPH, -100
	tst	TEMP
	breq	LCD_s_b_3		; if hundreds !=0 then
	set				; set the T flag (further digits
					; must be displayed even if are zeroes)
	ori	TEMP, 0b00110000	; convert it to ASCII code (+48)
	rcall	LCD_save		; and display on LCD

	clr	TEMP			; loop for counting digit of tens
LCD_s_b_3:
	subi	TEMPH, 10
	brlo	LCD_s_b_4
	inc	TEMP
	rjmp	LCD_s_b_3
LCD_s_b_4:
	subi	TEMPH, -10
	tst	TEMP
	brne	LCD_s_b_5		; if tens = 0 then ..
	brtc	LCD_s_b_6		; if T flag is not set then ..
LCD_s_b_5:
	set				; set T flag (further digits
					; must be displayed even if are zeroes)
	ori	TEMP, 0b00110000	; convert it to ASCII code (+48)
	rcall	LCD_save		; and display on LCD 

LCD_s_b_6:
	mov	TEMP, TEMPH		; remaining is the last digit of ones
	ori	TEMP, 0b00110000	; convert it to ASCII code (+48)
	rcall	LCD_save		; and display on LCD

	pop	TEMPH
	pop	TEMP
	ret


;***************************************************************
; Subroutine for displaying string
;
; arguments:
;	Z	- address first character of string in program memory

.EQU END_MARK	= 255 ; label for end of string mark

LCD_show_string:
	push	TEMP
	push	TEMPH

LCD_s_s_1:		; loop for displaying next characters of string
	lpm	TEMP, Z+	; read character from program memory from address 
				; pointed by index register Z
	cpi	TEMP, END_MARK
	breq	LCD_s_s_2	; if it is end of string mark then stop
	rcall	LCD_save	; otherwise display it on LCD
	rjmp	LCD_s_s_1	; and read next character

LCD_s_s_2:
	pop	TEMPH
	pop	TEMP
	ret


;***************************************************************
; Subroutine for displaying MenuInfo on the LCD (testing)
;
; using LCD_show_byte and LCD_show_string

LCD_Display_MenuInfo:

	push	TEMP
	push	MENUL
	mov	TEMP, MENUL
	rcall	LCD_clear
	rcall	LCD_show_byte
	pop	MENUL
	pop	TEMP
	ret
