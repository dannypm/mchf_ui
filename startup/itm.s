/************************************************************************************
**                                                                                 **
**                             mcHF Pro QRP Transceiver                            **
**                         Krassi Atanassov - M0NKA 2012-2019                      **
**                            mail: djchrismarc@gmail.com                          **
**                                 twitter: @M0NKA_                                **
**---------------------------------------------------------------------------------**
**                                                                                 **
**  File name:                                                                     **
**  Description:                                                                   **
**  Last Modified:                                                                 **
**  Licence:                                                                       **
**          The mcHF project is released for radio amateurs experimentation,       **
**          non-commercial use only. All source files under GPL-3.0, unless        **
**          third party drivers specifies otherwise. Thank you!                    **
************************************************************************************/
	.syntax unified
	.cpu cortex-m7
	.fpu softvfp
	.thumb

	.section		.text.ItmInitAsm
	.global			ItmInitAsm
	.type			ItmInitAsm, %function

ItmInitAsm:

			PUSH    {R1-R5,LR}
			LDR     R3, reg1
			MOV.W   R2, #0x700000
			LDR     R1, unlk
			MOVS    R0, #0xC7
			STR     R2, [R3,#4]
			ADD.W   R3, R3, #0x20C0
			LDR.W   LR, reg3
			MOV.W   R10, #0x100
			LDR     R2, reg4
			ADDS    R3, #0x30
			STR.W   R1, [LR]
			SUB.W   LR, LR, #0xFB0
			STR     R0, [R2]
			STR     R5, [R3]
			LDR.W   R2, [LR]
			LDR     R3, reg5
			ORR.W   R2, R2, #1
			LDR     R5, reg6
			STR.W   R2, [LR]
 			LDR.W   R2, [R4,#0xE0]
			STR     R1, [R5]
 			ORR.W   R2, R2, #2
 			STR.W   R2, [R4,#0xE0]
			LDR     R2, [R3]
			BIC.W   R2, R2, #0xC0
			STR     R2, [R3]
			LDR     R2, [R3]
			ORR.W   R2, R2, #0x80
			STR     R2, [R3]
			LDR     R2, [R3,#8]
			BIC.W   R2, R2, #0xC0
			STR     R2, [R3,#8]
			LDR     R2, [R3,#8]
			ORR.W   R2, R2, #0x80
			STR     R2, [R3,#8]
			LDR     R2, [R3,#0x20]
			BIC.W   R2, R2, #0xF000
			STR     R2, [R3,#0x20]
			b		all_done

reg1:		.word   0x5C001000
unlk:		.word  	0xC5ACCE55
reg3:		.word	0x5C004FB0
reg4:		.word  	0x5C003010
reg5:		.word  	0x58020400
reg6:		.word  	0x5C003FB0

all_done:
	POP     {R1-R5,PC}

	.size	ItmInitAsm,	.-ItmInitAsm
