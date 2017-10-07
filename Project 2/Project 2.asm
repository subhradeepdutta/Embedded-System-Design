;Author:Subhradeep Dutta
;University of Colorado Boulder
;Date 09/23/2016
;This program uses the Timer 0 interrupt
;to toggle an LED on P1.0 for a duration of 455 ms

ORG 00H     ;Program starts at 00H address
LJMP MAIN   ;Jump to main part of the code

ORG 000BH   ;Address to the Timer 0 ISR
LJMP ISR

ISR:SETB P1.1 ;P1.1 used to calculate execution time of ISR
CLR TF0       ;Clear overflow flag
SETB TR0      ;Start Timer0
MOV TH0,#05CH 
MOV TL0,#031H
DJNZ R0,END1  ;Jumps to end of ISR until R0 is zero
CPL P1.0      ;P1.0 is connected to LED and used to toggle
MOV R0,#0AH   ;Reloading R0 value once it is zero
END1:CLR P1.1 ;Clearing P1.1 to mark end of ISR
RETI          ;End of ISR

MAIN: MOV TMOD, #01H  ;Enabling Timer 0 in mode 01
MOV IE, #82H          ;Enable Timer 0 Interrupt
MOV R0, #0AH          ;Loading R0 with 0AH
MOV TH0,#05CH
MOV TL0,#031H
SETB TR0              ;Start Timer0
HERE:SJMP HERE        ;Infinite Loop until Interrupt
END
