				ORG 00H			;Starting at 0000H address	
				MOV A,#04H		;Moving Y to the accumulator
				JZ DIVZERO		;Checking for division by zero
				MOV B,A			;Loading value of Y to B register
				MOV A,#010H		;Loading X to the accumulator
				ADD A,#02H		;Calculating X+2
				JC OVRFLOW		;Checking for carry during addition
				MOV 20H,A		;Storing sum X+2 to 0x20 IRAM 
				MOV R3,#0H		;Setting R3 register to zero
				CLR C			;Clearing the carry bit
REP:			SUBB A,B		;Subtract with borrow Accumulator-B register
				JC LESS			;Branch instruction for handling nagtive result during subtraction
				JZ ZERO			;Branch instruction for handling zero during subtraction
				INC R3			;Incrementing R3 register to obtain the final quotient
				SJMP REP		;Looping until zero or negative result in accumulator
LESS:			ADD A,B			;Adding accumulator and B register to obtain the remainder
				MOV 22H,A		;Storing remainder at 0x22 IRAM
				SJMP OUT		;Branching out of the loop
ZERO:			INC R3			;Increment R3 register for final count of quotient
				MOV 22H,#00H	;Storing the remainder at 0x22 IRAM
OUT:			MOV A,R3		;Loading the accumulator with the final quotient
				MOV 21H,A		;Storing the quotient to 0x21 IRAM
				CLR C			;Clearing the carry bit
				RLC A			;Rotating accumulator contents left through carry for multiplication by 2
				RLC A			;Rotating accumulator contents left through carry for multiplication by 2
				JC OVERFLOWMUL	;Handling carry during multiplication
				MOV 23H,A		;Storing final result Z at 0x23 IRAM
				MOV 30H,#00H	;Writing error code to 0x30 IRAM
				SJMP ENDLOOP	;Moving to infinite loop
OVRFLOW:		MOV 30H,#01H	;Writing error code to 0x30 IRAM
				SJMP ENDLOOP	;To infinite loop
DIVZERO:		MOV 30H,#02H	;Writing error code to 0x30 IRAM
				SJMP ENDLOOP
OVERFLOWMUL:	MOV 30H,#03H	;Writing error code to 0x30 IRAM
				SJMP ENDLOOP	;To infinite loop
ENDLOOP:		SJMP ENDLOOP	;Program ends here
