;This program counts from 10 to 0
	.ORIG x3000   	
	LEA R0,THOOA		;This instruction will be loaded into memory location x3000
	ADD R1,R1,#-1
	STB R1,R0,#0  		;expected 11 in x300A
	LEA R0, THOOD
	STW R1, R0,#0  		;expected 1111 in x300D, x300C
THOOA	.FILL x0000
THOOB	.FILL x0000
THOOC	.FILL x0000
THOOD	.FILL x0000
	.END			;The pseudo-op, delimiting the source program