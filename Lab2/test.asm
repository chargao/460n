;This program counts from 10 to 0
	.ORIG x3000   	
	LEA R0,ZEROS		;This instruction will be loaded into memory location x3000
  XOR R1,R0,#0  ;expected 1
  XOR R2, R0,R0  ;expected 1
ZEROS	.FILL x0000		;
	.END			;The pseudo-op, delimiting the source program