; addresses of the IOCON for the GPIO
IOCON_P1_13 EQU 0x4002C0B4 ; P1[13] IOCON_P1_13 R/W 0x030 0x4002C0B4
IOCON_P1_18 EQU 0x4002C0C8 ; P1[18] IOCON_P1_18 R/W 0x030 0x4002 C0C8 D (tables 83,    84)
IOCON_P0_13 EQU 0x4002C034 ; LED 2
IOCON_P2_19 EQU 0x4002C14C ; LED 4
IOCON_P1_2 EQU 0x4002C088 ; external LED
IOCON_P2_10 EQU 0x4002C128 ; user switch

; This is essentially Table 94 from UM10562 (starting addresses of the ports and the offsets for GPIO registers)
LPC4088QSB_P0 EQU 0x20098000
LPC4088QSB_P1 EQU 0x20098020
LPC4088QSB_P2 EQU 0x20098040
LPC4088QSB_P3 EQU 0x20098060
LPC4088QSB_P4 EQU 0x20098080
LPC4088QSB_P5 EQU 0x200980A0
DIR_REG_OFFSET EQU 0x00
MSK_REG_OFFSET EQU 0x10
PIN_REG_OFFSET EQU 0x14
SET_REG_OFFSET EQU 0x18
CLR_REG_OFFSET EQU 0x1C
; End Table 94

bit13 EQU (1 << 13) ; 0x00002000
bit18 EQU (1 << 18) ; 0x00040000
bit19 EQU (1 << 19)
bit2 EQU (1 << 2)
bit10 EQU (1 << 10)

		AREA my_asm, CODE, READONLY
		ENTRY
		EXPORT __main
__main

	; setting external LED as GPIO
	LDR R1, =IOCON_P1_2 ; Pg 126
	LDR R2, =0x00000000 ; we are saying a 0 offset bc the default is 0x30. what does the 0x30 offset do?
	STR R2,[R1,#0x00]

	; setting the external LED pin as an output
	LDR R1,=LPC4088QSB_P1
	LDR R3,=bit2
	STR R3,[R1,#DIR_REG_OFFSET]

	; Setting a mask on port 1 such that only the LED pin can be updated
	LDR R1,=LPC4088QSB_P1
	LDR R3,=bit2
	STR R3,[R1,#MSK_REG_OFFSET]

	; Start with the LED turned off
	LDR R1,=LPC4088QSB_P1
	LDR R3,=bit2
	STR R3,[R1,#CLR_REG_OFFSET]

	;set the switch as GPIO
	LDR R1, =IOCON_P2_10
	LDR R3, =0x00000000 ; we are saying a 0 offset bc the default is 0x30
	STR R3, [R1,#0x00]

	;set switch as input
	LDR R1, =LPC4088QSB_P2
	LDR R3, = 0x00000000 ; '0' sets the pin as input
	STR R3, [R1, #DIR_REG_OFFSET]

	;mask ~(bit10)
	LDR R1, =LPC4088QSB_P2
	LDR R3, =bit10
	STR R3, [R1, #MSK_REG_OFFSET]

	MOV R8, #0x00000000 ; Current LED value
	MOV R6, #0x00000001 ; LED on
	MOV R7, #0x00000000 ; LED off

Toggle

	;button pin value
	LDR R1, =LPC4088QSB_P2
	LDR R3, [R1, #PIN_REG_OFFSET] ; is it bc we masked the value therefore we can use the whole port 0x00000000
	LDR R4, =bit10
	ANDS R5, R4, R3 ; Z flag, '0' == to the button being pressed(equivalent) && '1' == to not pressed
	BEQ on2off ; branch to check which state to toggle too
	BNE Toggle ; check to see if the button was pressed again

on2off
	CMP R6, R8; R6 = 1 & R8 hold the value of the LED
	BNE turnon
	BEQ turnoff

turnon
	; turn on the LED
	LDR R1,=LPC4088QSB_P1
	LDR R3,=bit2
	STR R3,[R1,#SET_REG_OFFSET]

	MOV R8, #0x00000001; set the value of the LED as on
	bl Toggle

turnoff
	; Turn off the LED
	LDR R1,=LPC4088QSB_P1
	LDR R3,=bit2
	STR R3,[R1,#CLR_REG_OFFSET]

	MOV R8, #0x00000001
	bl Toggle

stp
  b stp

; ------------------------------------------
; Delay subroutine
delay
	PUSH {R0,LR}
	MOVS r0, #0x1000000 ;1 cycle
B1
	SUBS r0, r0, #1 ;1 cycle
	BNE B1 ;2 if taken, 1 otherwise
	POP {R0,PC}
		; end delay subroutine
; ------------------------------------------
	ALIGN
 END