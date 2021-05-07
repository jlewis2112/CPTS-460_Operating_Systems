/********************************************************************
Copyright 2010-2017 K.C. Wang, <kwang@eecs.wsu.edu>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/
//#include "type.h"
extern PROC proc[], *running;
extern int tswitch();

int ktswitch()
{
  tswitch();
}

int kgetpid()
{
  return running->pid;
}

int kgetppid()
{
  return running->ppid;
}

char *pstatus[]={"FREE   ","READY  ","SLEEP  ","BLOCK  ","ZOMBIE", " RUN  "};
int kps()
{
  int i; PROC *p;
  for(i=0; i<NPROC; i++)
  {
    p=&proc[i];
    kprintf("proc[%d]: pid=%d ppid=%d ", i, p->pid, p->ppid);
    if(p==running)
	printf("%s ", pstatus[5]);
    else
	printf("%s ", pstatus[p->status]);
    printf("name=%s\n", p->name);
  }
}

int kchname(char *s)
{ 
  kprintf("kchname: name=%s\n", s);
  strcpy(running->name, s);
  return 123;
}

int kgetPA()
{
  return running->pgdir[2048] & 0xFFFF0000;
}

int fork()
{
	int i, r;
	char *PA, *CA;
	int *ptable, pentry;
	PROC *p = getproc(&freeList);
	if(p==0) {printf("fork failed\n"); return -1;}
	p->ppid = running->pid;
	p->parent = running;
	p->status = READY;
	p->priority = 1;

	p->pgdir = (int *)(0x600000 + (p->pid - 1)*0x4000);
	ptable = p->pgdir;
	for(i=0; i<4096; i++)
		ptable[i] = 0;
  pentry = 0x412;
	for(i=0; i<258; i++){
		ptable[i] = pentry;
		pentry += 0x100000;
	}

	ptable[2048] = 0x800000 + (p->pid -1)*0x100000|0xC32;
  PA = running->pgdir[2048] & 0xFFFF0000;
	CA = p->pgdir[2048] & 0xFFFF0000;
	memcpy(CA, PA, 0x100000);

	for(i=1; i<=14; i++){
		p->kstack[SSIZE-i] = running->kstack[SSIZE-i];
	}

	p->kstack[SSIZE-14] = 0;
  p->kstack[SSIZE-15] = (int)goUmode;
	p->ksp = &(p->kstack[SSIZE-28]);
	p->usp = running->usp;
	p->cpsr = running->cpsr;
	enqueue(&readyQueue, p);
	printQ(readyQueue);
	return p->pid;
}

int exec(char *cmdline)
{
	int i, upa, usp;
	char *cp, kline[128], file[32], filename[32];
	PROC *p = running;
	strcpy(kline, cmdline);
	cp = kline; i = 0;
	while(*cp != ' ' && *cp != '\0'){
		filename[i] = *cp;
		cp++; i++;
	}
	filename[i] = 0;
	file[0] = 0;
	kstrcat(file, filename);
	upa = p->pgdir[2048] & 0xFFFF0000;
	if(!load(file, p))
		return -1;
	usp = upa + 0x100000 - 128;
	strcpy((char *)usp, kline);
	p->usp = (int *)VA(0x100000 - 128);
	for(i=2; i<14; i++)
		p->kstack[SSIZE - i] = 0;
	p->kstack[SSIZE-1] = (int)VA(0);
	return (int)p->usp;
}


int svc_handler(int a, int b, int c, int d)
{
  int r;
  
  // printf("svc_handler: a=%d b=%d c=%d d=%d\n",a,b,c,d);

  switch(a){ // a is the call number
    
     case 0: r = kgetpid();          break;
     case 1: r = kgetppid();         break;
     case 2: r = kps();              break;
     case 3: r = kchname((char *)b); break;
     case 5: r = ktswitch();         break;
		 case 6: r = kwait(b);           break;
		 case 7: r = kexit(b);           break;
		 case 9: r = fork();             break;
     case 10: r = exec((char *)b);   break;
     case 90: r = kgetc();           break;
     case 91: r = kputc(b);          break;
     case 92: r = kgetPA();          break;
     default: printf("invalid syscall %d\n", a);
  }

  return r; //return to goUmode in ts.s, which will replace r0 in Kstack with r
}

