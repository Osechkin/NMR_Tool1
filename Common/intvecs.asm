;*-----------------------------------------------------------------------------
;* Project: UART_interrupt_loopback_dspL138
;* File: intvecs.asm
;*
;* Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/
;*
;*  Redistribution and use in source and binary forms, with or without
;*  modification, are permitted provided that the following conditions
;*  are met:
;*
;*    Redistributions of source code must retain the above copyright
;*    notice, this list of conditions and the following disclaimer.
;*
;*    Redistributions in binary form must reproduce the above copyright
;*    notice, this list of conditions and the following disclaimer in the
;*    documentation and/or other materials provided with the
;*    distribution.
;*
;*    Neither the name of Texas Instruments Incorporated nor the names of
;*    its contributors may be used to endorse or promote products derived
;*    from this software without specific prior written permission.
;*
;*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
;*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
;*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
;*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
;*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
;*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
;*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
;*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
;*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
;*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
;*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;* 
;*-----------------------------------------------------------------------------
;* 
;*	intvecs.asm
;*	-----------
;*
;*	Description:
;*		This file includes a generic setup to map the DSP interrupt service 
;*		table (IST) to the corresponding interrupt service routines (ISR).
;*
;*-----------------------------------------------------------------------------

; Global symbols defined here
	.global _intcVectorTable
	.global _c_int00
	.global _TIMER0_12_isr
	.global _UART_isr
	.global _upp_isr
	.global _UART_Dielec_isr
	.global _UART_Telemitric_isr

;******************************************************************************
;* VEC_ENTRY: Macro that instantiates one entry in the interrupt service table.
;******************************************************************************
VEC_ENTRY .macro addr
	STW   B0,*--B15
	MVKL  addr,B0
	MVKH  addr,B0
	B     B0
	LDW   *B15++,B0
	NOP   2
	NOP
	NOP
	.endm

;******************************************************************************
;* vec_dummy: Dummy interrupt service routine used to initialize the IST.
;******************************************************************************
_vec_dummy:
	B    B3
	NOP  5

;***************************************************************************************
;* Map interrupt service table (IST) to corresponding interrupt service routines (ISR)
;***************************************************************************************
 .sect ".vecs"
 .align 1024

_intcVectorTable:
_vector0:	VEC_ENTRY _c_int00			;RESET
_vector1:	VEC_ENTRY _vec_dummy		;NMI
_vector2:	VEC_ENTRY _vec_dummy		;RSVD
_vector3:	VEC_ENTRY _vec_dummy		;RSVD
_vector4:	VEC_ENTRY _TIMER0_12_isr	;DSP Maskable INT4 : Mapped to func 'TIMER0_12_isr'
_vector5:	VEC_ENTRY _UART_isr			;DSP Maskable INT5 : Mapped to func 'UART_isr'
;_vector5:	VEC_ENTRY _vec_dummy		;DSP Maskable INT5 : Empty
_vector6:	VEC_ENTRY _upp_isr			;DSP Maskable INT6 : Mapped to func 'upp_isr'
;_vector6:	VEC_ENTRY _vec_dummy		;DSP Maskable INT6 : Empty
_vector7:	VEC_ENTRY _UART_Dielec_isr	;DSP Maskable INT7 : Mapped to func 'UART_Dielec_isr'
_vector8:	VEC_ENTRY _UART_Telemitric_isr		;DSP Maskable INT8 : Mapped to func UART_Telemitric_isr
_vector9:	VEC_ENTRY _vec_dummy		;DSP Maskable INT9 : Empty
_vector10:	VEC_ENTRY _vec_dummy		;DSP Maskable INT10: Empty
_vector11:	VEC_ENTRY _vec_dummy		;DSP Maskable INT11: Empty
_vector12:	VEC_ENTRY _vec_dummy		;DSP Maskable INT12: Empty
_vector13:	VEC_ENTRY _vec_dummy		;DSP Maskable INT13: Empty
_vector14:	VEC_ENTRY _vec_dummy		;DSP Maskable INT14: Empty
_vector15:	VEC_ENTRY _vec_dummy		;DSP Maskable INT15: Empty
