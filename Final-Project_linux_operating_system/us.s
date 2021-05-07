   .global u_entry, main0, syscall, getmysp, getcsr, getmyaddress
   .global mexit, getulr, getFP, getusp
   .global setjmp, longjmp
	.text
.code 32
// upon entry, bl main0 => r0 contains pointer to the string in ustack

u_entry:
	mov r0, r1
	bl main0
// at VA=4: syscall to exit
	mov r0, #9
	mov r1, #128
	swi #0        // syscall(0, 128) to kexit(128) 
	
//	mov r0, #0
//	bl  mexit
        	
// if main0() ever retrun: syscall to exit(0)
	
@ user process issues int syscall(a,b,c,d) ==> a,b,c,d are in r0-r3	
syscall:

//   mov r4, sp  // r4 = usp
//   mov r5, pc  // r5 = return PC
   swi #0
   mov pc, lr

getmysp:
   mov r0, sp
   mov pc, lr

getulr:
	mov r0, lr
	mov pc, lr
	
getFP:	mov r0, fp
	mov pc, lr

getusp:	mov r0, sp
	mov pc, lr
getcsr:
   mrs r0, cpsr
   mov pc, lr
getmyaddress:
   ldr r0, =main0
   mov pc, lr
	
setjmp:	// int setjmp(int env[2])
	stmfd sp!, {fp,lr}
	add   fp, sp, #4
	ldr   r1, [fp]
	str   r1, [r0]
	ldr   r1, [fp, #-4]
	str   r1, [r0, #4]
	mov   r0, #0
	sub   sp, fp, #4
	ldmfd sp!, {fp,pc}

longjmp: // int longjmp(int env[2])
	stmfd sp!, {fp,lr}
	add   fp, sp, #4
	ldr   r2, [r0]
	str   r2, [fp]
	ldr   r2, [r0, #4]
	str   r2, [fp, #-4]
	mov   r0, r1
	sub   sp, fp, #4
	ldmfd sp!,{fp,pc}
	
