	.cdecls C, list, "msp430.h"	; this allows us to use C headers
	
	.text						; locates code in 'text' section
	
	.global multiply				; 'multiply' is now global
	
multiply:
	;TODO: use MPY32 to multiply r12 & r13
	mov.w r12, &MPY ; load first arg
	mov.w r13, &OP2 ; load second arg & trigger multiplication
	mov.w #0x0200, &MPY32CTL0 ; //setup multiplier
	;TODO: other? returning value together? regs: RES0(lower 16 bits) & RES1(upper 16 bits)
	;mov.w r12,r13 
	reta ;return that value
	
	
	
	.end
	