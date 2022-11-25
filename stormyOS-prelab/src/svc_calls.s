	AREA handle_pend,CODE,READONLY	;defines new "area"- way for linker to know what exists in this file (code that shouldn't be overwritten)
	EXTERN task_switch	;calls a C function to handle the switching
	GLOBAL PendSV_Handler	;declares function
	GLOBAL SVC_Handler	;declares new function 
	PRESERVE8	;tells linker that stack will always lie on 8 byte boundary
PendSV_Handler

	MRS r0, PSP	;loads curr val of PSP into r0
	
	STMDB r0!,{r4-r11}	;stores the registers
	
	BL task_switch	;calls kernel to switch task
	
	MRS r0,PSP	;new task stack
	MOV LR,#0xFFFFFFFD	;loads LR with constant val
	
	LDMIA r0!,{r4-r11}	; opposite of STMDB, means LoaD Multiple Increment After - basically undos the stack pushes done prior
	
	MSR PSP,r0	; reload PSP
	
	BX LR;returns from function
	;END
	
SVC_Handler
	EXTERN SVC_Handler_Main	;defines external function called SVC_Handler_Main
		
	;tests the value stored in LR
	;tells chip whether it was called from thread/handler mode & whether PSP/MSP was used
	TST LR,#4	
	
	ITE EQ	;an if-then-else construct that will load r0 depending on test result
	
	MRSEQ r0, MSP	;loads r0 with MSP depending on the test result
	MRSNE r0, PSP	;loads r0 with PSP depending on the test result
	
	B SVC_Handler_Main
	END
	
	