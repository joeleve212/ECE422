	.cdecls C, list, "msp430.h"	; this allows us to use C headers
	
	.text						; locates code in 'text' section
	
	.global multiply				; 'multiply' is now global
	
multiply: ; use MPY32 to multiply r12 & r13
	mov.w r12, &MPY ; load first arg
	mov.w r13, &OP2 ; load second arg & trigger multiplication
	mov.w &RES0, r12; move bottom 16 bits so they can be returned
	mov.w &RES1, r13; move top 16 bits so they can be returned
	reta ;return that value
	
	.end
	