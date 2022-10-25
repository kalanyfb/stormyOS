	AREA handle_pend,CODE,READONLY	;defines new "area"- way for linker to know what exists in this file (code that shouldn't be overwritten)
	GLOBAL PendSV_Handler	;declares function
	PRESERVE8	;tells linker that stack will always lie on 8 byte boundary
PendSV_Handler
	MOV LR,#0xFFFFFFFD	;loads LR with constant val
	BX LR;returns from function
	END
	