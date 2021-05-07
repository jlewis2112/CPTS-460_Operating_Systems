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

/********************
#define  SSIZE 1024
#define  NPROC  9
#define  FREE   0
#define  READY  1
#define  SLEEP  2
#define  BLOCK  3
#define  ZOMBIE 4
#define  printf  kprintf
 
typedef struct proc{
  struct proc *next;
  int    *ksp;
  int    status;
  int    pid;

  int    priority;
  int    ppid;
  struct proc *parent;
  int    event;
  int    exitCode;
  int    kstack[SSIZE];
}PROC;
***************************/
extern PROC *getproc();
extern PROC *dequeue();

#define NPROC 9
PROC proc[NPROC], *running, *freeList, *readyQueue, *sleepList;
int procsize = sizeof(PROC);

int init()
{
  int i, j; 
  PROC *p;
  kprintf("kernel_init()\n");
  for (i=0; i<NPROC; i++){
    p = &proc[i];
    p->pid = i;
    p->status = READY;
    p->next = p + 1;
  }
  proc[NPROC-1].next = 0; // circular proc list

  freeList = &proc[0];
  readyQueue = 0;
  sleepList = 0;
  
  p = dequeue(&freeList);
  p->priority = 0;
  p->ppid = 0;
  running = p;

  kprintf("running = %d\n", running->pid);

}



int scheduler()
{
  //kprintf("proc %d in scheduler\n", running->pid);
  if (running->status==READY)
     enqueue(&readyQueue, running);
  //printQ(readyQueue);
  running = dequeue(&readyQueue);
  //kprintf("next running = %d\n", running->pid);
  color = RED + running->pid;
}  

int ksleep(int event)
{
  printf("proc%d sleep on event=%x\n", running->pid, event);  
  running->event = event;
  running->status = SLEEP;
  enqueue(&sleepList, running);
  //printf("sleepList = "); printQ(sleepList);
  tswitch();
}

int kwakeup(int event)
{
  PROC *p, *tmp=0;
  while((p = dequeue(&sleepList))!=0){
    if (p->event==event){
      if (running->pid)
         printf("proc%d wakeup %d\n", running->pid, p->pid);
      p->status = READY;
      enqueue(&readyQueue, p);
    }
    else{
      enqueue(&tmp, p);
    }
  }
  sleepList = tmp;
}
int kexit()
{
  //  printf("proc %d kexit\n", running->pid);
  running->status = ZOMBIE;
  tswitch();
}
  
int kfork(int func, int priority)
{
  int i;
  PROC *p = dequeue(&freeList);
  if (p==0){
    printf("no more PROC, kfork failed\n");
    return 0;
  }
  p->status = READY;
  p->priority = priority;
  p->ppid = running->pid;
  p->parent = running;
  p->mQueue = 0;
  p->mQlock.value = 1;
  p->mQlock.queue = 0;
  p->nmsg.value = 0;
  p->nmsg.queue = 0;
  p->mQueue = 0;
  
  
  // set kstack for new proc to resume to func()
  // stack = r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r14
  //         1  2  3  4  5  6  7  8  9  10 11  12  13  14
  for (i=1; i<15; i++)
      p->kstack[SSIZE-i] = 0;
  p->kstack[SSIZE-1] = (int)func;  // in dec reg=address ORDER !!!
  p->ksp = &(p->kstack[SSIZE-14]);
  enqueue(&readyQueue, p);

  printf("proc %d kforked a child %d\n", running->pid, p->pid);
  return p->pid;
}

int block(struct semaphore *s)
{
	running->status = BLOCK;
	enqueue(&s->queue, running);
	tswitch();
}

int signal(struct semaphore *s)
{
	PROC *p = dequeue(&s->queue);
	p->status = READY;
	enqueue(&readyQueue, p);
}


int P(struct semaphore *s)
{
	int SR = int_off();
	s->value--;
	if(s->value < 0)
		block(s);
	int_on(SR);
}

int V(struct semaphore *s)
{
	int SR = int_off();
	s->value++;
	if(s->value <= 0)
		signal(s);
	int_on(SR);
}

int sender()
{
	struct uart *up = &uart[0];
	char line[128];
	while(1)
	{
		kprintf("enter a line\n");
		kgets(line);
		printf("task %d got a line = %s \n", running->pid, line);
		send(line, 2);
		printf("task %d send %s to pid=4\n", running->pid, line);
		tswitch();
	}
}

int receiver()
{
	char line[128];
	int pid;
	while(1)
	{
		printf("task %d try to recieve msg\n", running->pid);
		pid = recv(line);
		printf("task %d received: [%s] from task %d \n", running->pid, line, pid);
	}
}

