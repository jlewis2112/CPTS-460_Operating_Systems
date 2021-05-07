// wait.c file

extern PROC *running;
extern PROC *sleepList;

int kexit(int exitValue)  // SIMPLE kexit() for process to terminate
{
  int i;
  PROC *p;
  kprintf("proc %d in kexit(), value= %d\n", running->pid, exitValue);

  if(running->pid == 1)
  {
    kprintf("runing->pid 1 shall not die\n");
    return -1;
  }

  for( i = 0; i < NPROC; i++)
  {
    p = &proc[i];
    if( p->status != FREE && p->ppid == running->pid)
    {
       p->ppid = 1;
       p->parent = &proc[i];
    }
  }

  running->exitCode = exitValue;
  running->status = ZOMBIE;

  kwakeup(proc[1].pid);
  kwakeup(running->parent);

  tswitch();
}

int ksleep(int event)
{
  //printf("proc%d sleep on event=%x\n", running->pid, event);
  int SR = int_off();  
  running->event = event;
  running->status = SLEEP;
  enqueue(&sleepList, running);
  //printf("sleepList = "); printQ(sleepList);
  tswitch();
  int_on(SR);
}

int kwakeup(int event)
{
  PROC *p, *tmp=0;
  int SR = int_off();
  while((p = dequeue(&sleepList))!=0){
    printf("hi \n");
    if (p->event==event){
      if (running->pid)
         printf("proc%d wakeup %d\n", running->pid, p->pid);
      p->status = READY;
      enqueue(&readyQueue, p);
      //kprintf("enqueue complete\n");
    }
    else{
      //kprintf("wrong event %d vs %d\n", p->event, event);
      enqueue(&tmp, p);
    }
  }
  sleepList = tmp;
  int_on(SR);
}

int kwait(int *status)
{
 int i;
 for(i=0; i<NPROC; i++)
 {
   if(proc[i].ppid == running->pid && proc[i].status != FREE && proc[i].status != ZOMBIE) break;
 }

 //if no children stop
 if(i == NPROC)
 {
   kprintf("there are no children here!\n");
   return -1;
 }

while(1)
{
    for(i = 0; i<NPROC; i++)
    {
	if(proc[i].status == ZOMBIE && proc[i].ppid == running->pid)
	{
	  *status = proc[i].exitCode;
	  proc[i].status = FREE;
	  enqueue(&freeList, &proc[i]);
	  RemoveFree(running->child);
	  return proc[i].pid;
	}
    }
    ksleep(running->pid);
}

}
