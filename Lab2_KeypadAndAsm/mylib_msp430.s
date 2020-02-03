	.cdecls C, list, "msp430.h"	; this allows us to use C headers
	
	.text						; locates code in 'text' section
	
	.global multiply				; 'multiply' is now global
	
multiply:
	;TODO: use MPY32 to multiply r12 & r13
	mov.w r12, &MPY ; load first arg
	mov.w r13, &OP2 ; load second arg & trigger multiplication
;mov.w #0x0200, &MPY32CTL0 ; //setup multiplier
;	mov.w &OP2L, r12; move bot 16 bits to r12 to return
;	mov.w &OP2H, r13; move top 16 bits to r13 to return
	mov.w &RES0, r12;
	mov.w &RES1, r13;
	;TODO: other? returning value together? regs: RES0(lower 16 bits) & RES1(upper 16 bits)
	;mov.w r12,r13 
	reta ;return that value
	
	
	
	.end
	