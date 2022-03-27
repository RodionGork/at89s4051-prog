ORG 0
        SJMP START ;lines until "START" may be removed, they are just for testing
        DB 0
        DB 1
        DB 2

ORG 11 
; Main Program
START:	MOV P1, #11110101b ; set bits on Port1
REP:	CALL WAIT
	    MOV A, P1   ; Take value from port
	    ;MOV R2, #1
	    ;ADD A, R2
	    ;MOV R2, #00001111b
	    ;ANL A, R2
	    ;MOV R2, #11100000b
	    ;ORL A, R2
	    CPL A
	    MOV P1, A 	; Write back to port
	    SJMP REP 	; Jump to START
WAIT:	MOV R2, #10 	; Load Register R2 with 10 (0x0A)
WAIT1:	MOV R3, #200 		; Load Register R3 with 10 (0xC8)
WAIT2:	MOV R4, #200 		; Load Register R4 with 10 (0xC8)
WAIT3:  DJNZ R4, WAIT3 	; Decrement R4 till it is 0
	    DJNZ R3, WAIT2 	; Decrement R3 till it is 0
	    DJNZ R2, WAIT1 	; Decrement R2 till it is 0
	    RET
END
