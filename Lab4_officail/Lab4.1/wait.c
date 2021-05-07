// wait.c file

extern PROC *running;
extern PROC *sleepList;

int kexit()  // SIMPLE kexit() for process to terminate
{
  printf("proc %d exit\n", running->pid);
  running->status = ZOMBIE;
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
    if (p->event==event){
      if (running->pid)
         //printf("proc%d wakeup %d\n", running->pid, p->pid);
      p->status = READY;
      enqueue(&readyQueue, p);
    }
    else{
      enqueue(&tmp, p);
    }
  }
  sleepList = tmp;
  int_on(SR);
}
