	.cdecls C, list, "msp430.h"	; this allows us to use C headers
	
	.text						; locates code in 'text' section
	
	.global add					; declares 'add' as global
	.global comp				; 'comp' is now global
	
comp:
	cmp r12, r13
	jl lower 			;first 
	
higher:
	bis.b #0x80, &P9OUT ; P9.7 goes high
	bic.b #0x01, &P1OUT ; P1.0 goes low
	reta
	
lower:
	bis.b #0x001, &P1OUT ; P1.0 goes high
	bic.b #0x080, &P9OUT ; P9.7 goes low
	reta
	
add:							; this is the function itself
	; if bit 15 is set, then jump to add_neg
	bit.w	#32768, r12
	jc		add_neg
	
add_pos:
	bit.w	#32768, r13
	jc		add_pos_neg
	
add_pos_pos:					; this label isn't declared global, so it is only available locally
	add.w	r13, r12
	jn		add_pos_pos_sat
	reta

add_neg:
	bit.w	#32768, r13
	jc		add_neg_neg
	
add_pos_neg:
	add.w	r13, r12
	reta
	
add_pos_pos_sat:
	mov		#32767, r12
	reta
	
add_neg_neg:
	add.w	r13, r12	
	
	; result should be negative
	bit.w	#32768, r12
	jnc		add_neg_neg_sat
	reta
	
add_neg_neg_sat:
	mov		#32768, r12
	reta
	
	
	
	
	
	.end
	