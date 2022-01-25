IOCON_P1_18 EQU 0x4002C0C8 		; P1[18] IOCON_P1_18 R/W 0x030 0x4002 C0C8 D (tables 83, 84)	LED1
IOCON_P0_13 EQU 0x4002C034 		; P0[13] IOCON_P0_13 R/W 0x030 0x4002 C034 D (tables 83, 84)	LED2
IOCON_P1_13 EQU 0x4002C0B4 		; P1[13] IOCON_P1_13 R/W 0x030 0x4002 C0B4						LED3
IOCON_P2_19 EQU 0x4002C14C	 	; P0[13] IOCON_P0_13 R/W 0x030 0x4002 C14C D (tables 83, 84)	LED4
	
	
; This is essentially Table 94 from UM10562
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

bit13					EQU (1 << 13) ; 0x00002000
bit18					EQU (1 << 18) ; 0x00040000
bit19					EQU (1 << 19)
		
		AREA Lab02, CODE, READONLY
		ENTRY
		EXPORT			__main
__main

LOOP
		; setup LED#1 on port1[18]
		; setup the, IOCON, I/O configuration - default is 0x30, setup to 0x00
		; setup for GPIO and inactive and no hysteresis
		LDR		R1, =IOCON_P1_18 			; Pg 126 
		LDR 	R2, =0x00000000
		STR 	R2,[R1,#0x00]
		
		LDR 	R1,=LPC4088QSB_P1			; pointer to base reg of port1 - Pg 146
		LDR 	R3,=bit18 					; set LED 1 port pin to output - pin 18
		STR 	R3,[R1,#DIR_REG_OFFSET] 	; set DIRECTION bits
		
		LDR 	R1,=LPC4088QSB_P1 			; pointer to base reg of port1 
		LDR		R3,=0x00000000 				; MSK register setting
		STR 	R3,[R1,#MSK_REG_OFFSET] 	; set MASK bits

		LDR 	R1,=LPC4088QSB_P1 			; pointer to base reg of port1
		LDR 	R3,=bit18 					; CLR register setting
		STR 	R3,[R1,#CLR_REG_OFFSET] 	; set CLEAR bits - This turns ON the LED
		
		bl delay
		
		LDR 	R1,=LPC4088QSB_P1 			; pointer to base reg of port1
		LDR 	R3,=bit18 					; SET register setting
		STR 	R3,[R1,#SET_REG_OFFSET] 	; set SET bits - This turns OFF the LED

		bl delay
		
		; setup LED#2 on port0[13]
		; setup the, IOCON, I/O configuration - default is 0x30, setup to 0x00
		; setup for GPIO and inactive and no hysteresis
		LDR		R4, =IOCON_P0_13 			; Pg 126 
		LDR 	R5, =0x00000000
		STR 	R6,[R4,#0x00]
		
		LDR 	R4,=LPC4088QSB_P0			; pointer to base reg of port1 - Pg 146
		LDR 	R6,=bit13 					; set LED 1 port pin to output - pin 18
		STR 	R6,[R4,#DIR_REG_OFFSET] 	; set DIRECTION bits
		
		LDR 	R4,=LPC4088QSB_P0 			; pointer to base reg of port1 
		LDR		R6,=0x00000000 				; MSK register setting
		STR 	R6,[R4,#MSK_REG_OFFSET] 	; set MASK bits

		LDR 	R4,=LPC4088QSB_P0 			; pointer to base reg of port1
		LDR 	R6,=bit13 					; CLR register setting
		STR 	R6,[R4,#CLR_REG_OFFSET] 	; set CLEAR bits - This turns ON the LED
		
		bl delay

		LDR 	R4,=LPC4088QSB_P0 			; pointer to base reg of port1
		LDR 	R6,=bit13 					; SET register setting
		STR 	R6,[R4,#SET_REG_OFFSET] 	; set SET bits - This turns OFF the LED
		
		bl delay
		
		; setup LED#3 on port1[13]
		; setup the, IOCON, I/O configuration - default is 0x30, setup to 0x00
		; setup for GPIO and inactive and no hysteresis
		LDR		R7, =IOCON_P1_13 			; Pg 126 
		LDR 	R8, =0x00000000
		STR 	R9,[R7,#0x00]
		
		LDR 	R7,=LPC4088QSB_P1			; pointer to base reg of port1 - Pg 146
		LDR 	R9,=bit13 					; set LED 1 port pin to output - pin 18
		STR 	R9,[R7,#DIR_REG_OFFSET] 	; set DIRECTION bits
		
		LDR 	R7,=LPC4088QSB_P1 			; pointer to base reg of port1 
		LDR		R9,=0x00000000 				; MSK register setting
		STR 	R9,[R7,#MSK_REG_OFFSET] 	; set MASK bits
		
		LDR 	R7,=LPC4088QSB_P1 			; pointer to base reg of port1
		LDR 	R9,=bit13 					; SET register setting
		STR 	R9,[R7,#SET_REG_OFFSET] 	; set SET bits - This turns ON the LED
		
		bl delay

		LDR 	R7,=LPC4088QSB_P1 			; pointer to base reg of port1
		LDR 	R9,=bit13 					; CLR register setting
		STR 	R9,[R7,#CLR_REG_OFFSET] 	; set CLEAR bits - This turns OFF the LED
		
		bl delay
		
		; setup LED#4 on port2[19]
		; setup the, IOCON, I/O configuration - default is 0x30, setup to 0x00
		; setup for GPIO and inactive and no hysteresis
		LDR		R10, =IOCON_P2_19 			; Pg 126 
		LDR 	R11, =0x00000000
		STR 	R12,[R10,#0x00]
		
		LDR 	R10,=LPC4088QSB_P2			; pointer to base reg of port1 - Pg 146
		LDR 	R12,=bit19 					; set LED 1 port pin to output - pin 18
		STR 	R12,[R10,#DIR_REG_OFFSET] 	; set DIRECTION bits
		
		LDR 	R10,=LPC4088QSB_P2 			; pointer to base reg of port1 
		LDR		R12,=0x00000000 			; MSK register setting
		STR 	R12,[R10,#MSK_REG_OFFSET] 	; set MASK bits

		LDR 	R10,=LPC4088QSB_P2 			; pointer to base reg of port1
		LDR 	R12,=bit19 					; SET register setting
		STR 	R12,[R10,#SET_REG_OFFSET] 	; set SET bits - This turns ON the LED
		
		bl delay

		LDR 	R10,=LPC4088QSB_P2 			; pointer to base reg of port1
		LDR 	R12,=bit19 					; CLR register setting
		STR 	R12,[R10,#CLR_REG_OFFSET] 	; set CLEAR bits - This turns OFF the LED
		
		bl delay

		;# LEDs 1 & 2 are NEGATIVE logic, 
		;# SET turns it OFF
		;# CLR turns it ON


		

		

		
		;# LEDs 3 & 4 are POSITIVE logic, 
		;# SET turns it ON
		;# CLR turns it OFF


		

		
		B	LOOP
		
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