// kernel.c file

#define NPROC 9
PROC proc[NPROC], *running, *freeList, *readyQueue;
PROC *sleepList, *timeList;
int procsize = sizeof(PROC);
int body();
char *status[] = {"FREE", "READY", "SLEEP", "BLOCK", "ZOMBIE"};
int tqe_buf[200];

int init()
{
  int i, j; 
  PROC *p;
  kprintf("kernel_init()\n");
  for (i=0; i<NPROC; i++){
    p = &proc[i];
    p->pid = i;
    p->status = FREE;
    p->priority = 0;
    p->tvalue = 0;
    p->next = p + 1;
  }
  proc[NPROC-1].next = 0; // circular proc list

  freeList = &proc[0];
  readyQueue = 0;
  sleepList = 0;
  timeList = 0;
 
  //tq->tvalue = 0;
  //tq->next = 0;
  //tq->global = 0;
  //entree->tvalue = 0;
  //entree->next = 0;
  //entree->global = 0;
  
  p = dequeue(&freeList);
  p->status = READY;
  p->priority = 0;
  p->ppid = 0;
  p->tvalue = 0;
  running = p;

  kprintf("running = %d\n", running->pid);
  printList("freeList", freeList);
}

  
int kfork(int func, int priority)
{
  int i;
  PROC *p = dequeue(&freeList);
  
  if (p==0){
    printf("kfork failed\n");
    return -1;
  }
  p->status = READY;
  p->priority = priority;
  p->ppid = running->pid;
  p->parent = running;


  if(p->parent->child == 0)//if no child
  {
    p->parent->child = p;
  }
  else//child make a sibling
  {
    PROC *temp = p->parent->child;

    while(temp->sibling != 0)
    {
      temp = temp->sibling;
    }
    temp->sibling = p;
  }

  p->sibling = 0;
  p->child = 0;

  kprintf("\nrunning pid: %d\n", running->pid);
  kprintf("running ppid: %d\n", running->ppid);
	PROC *temp = running->child;
  if(running->child != 0)
  {
     //kprintf("running children pid: [%d, %d] ", running->child->pid, running->child->status);
  }
 
  while(temp->sibling != 0)
  {
    //kprintf("[%d, %d] ", temp->sibling->pid, running->child->status);
    temp = temp->sibling;
  }
    kprintf(" \n");
  
  // set kstack for new proc to resume to func()
  // stack = r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r14
  //         1  2  3  4  5  6  7  8  9  10 11  12  13  14
  for (i=1; i<15; i++)
      p->kstack[SSIZE-i] = 0;

  p->kstack[SSIZE-1] = (int)func;  // in dec reg=address ORDER !!!
  p->ksp = &(p->kstack[SSIZE-14]);

  enqueue(&readyQueue, p);

  printf("proc %d kforked a child %d\n", running->pid, p->pid);
  printList("readyQueue", readyQueue);
  return p->pid;
}

int scheduler()
{
  //  kprintf("proc %d in scheduler ", running->pid);
  if (running->status == READY)
     enqueue(&readyQueue, running);
  running = dequeue(&readyQueue);
  //kprintf("next running = %d\n", running->pid);
  if (running->pid){
    color = running->pid;
  }
}  


int RemoveFree(PROC* p)
{
  if(p == 0)
  {
    return 0;
  }
  else
  {
    if(p->status == 0)
    {
        p = p->sibling;
    }
    printProcTree(p->child);
    printProcTree(p->sibling);
  }
  return 1;
}


int printProcTree(PROC* p)
{
  if(p == 0)
  {
    return 0;
  }
  else
  {
    if(p->status != 0)
    {
    kprintf("[%d, %s] ->", p->pid, status[p->status]);
    }
    printProcTree(p->child);
    printProcTree(p->sibling);
  }
  return 1;
}


int add_TQE(int value)
{
  running->tvalue = value;
  running->ttemp = 0;
  enqueuex(&timeList, running);
  ksleep(running->pid);
}



int printtimeList(PROC *p)
{
  int printpid[20];
  int printvalue[20];
  int i=0, j=0, k=0, l=0;
  int global_minus = 0;
  for(j=0; j<20; j++)
  {
	printpid[j]=0;
	printvalue[j]=0;
  }
  kprintf("timeList=");
  while(p){
    i = 0;
    while(p->tvalue > printvalue[i] && i < 20 && printpid[i] != 0)
    {	
      i++;
    }
    if(printpid[i] != 0)
    {
	for(k = 19; k > i ; k--)
	{
		printpid[k] = printpid[k-1];
		printvalue[k] = printvalue[k-1];
	}
    }
    printpid[i] = p->pid;
    printvalue[i] = p->tvalue;
	p = p->next;
  }
  //kprintf("i= %d", i);
  for(l=0; l<20 && printpid[l] != 0; l++)
  {
   int z = (printvalue[l] - global_minus);
   kprintf("[%d, %d]->", z, printpid[l]);
   global_minus += z;
  } 
  printf("NULL\n");
}


int count_TQE()
{
  PROC *p, *tmp=0;
  while((p = dequeue(&timeList))!=0){
    p->tvalue--;
    if (p->tvalue==0){
      kprintf("process %d woke up\n", p->pid);
      kwakeup(p->pid);
    }
    else{
      enqueue(&tmp, p);
    }
  }
  timeList = tmp;
  if(timeList)
  	printtimeList(timeList);
}

int body()
{
  char c, cmd[64];

  kprintf("proc %d resume to body()\n", running->pid);
  while(1){
    printf("-------- proc %d running -----------\n", running->pid);
    
    //printList("freeList  ", freeList);
    //printList("readyQueue", readyQueue);
    //printsleepList(sleepList);
    //printf("childList = ");
    //printProcTree(running->child);
    //kprintf("NULL \n");
	
    printf("Enter a command [switch|fork|exit|sleep|wakeup|wait|t] : ",
	   running->pid);
    kgets(cmd);
    kprintf("\n");
    if (strcmp(cmd, "switch")==0)
       tswitch();
    else if (strcmp(cmd, "fork")==0)
       kfork((int)body, 1);
    else if (strcmp(cmd, "exit")==0){
       kprintf("enter an exit value: ");
       kexit(geti());
    }
    else if (strcmp(cmd, "sleep")==0){
    	kprintf("enter an event value to sleep on : ");
        ksleep(geti());
    }
    else if (strcmp(cmd, "wakeup")==0){
	kprintf("enter an event value to wakeup with : ");
	kwakeup(geti());
    }
    else if (strcmp(cmd, "wait")==0){
	int runner = 0;
	int *status = &runner;
	int pid = kwait(status);
	kprintf("\n FOUND ZOMBIE pid = %d\n", pid); 
    }
    else if (strcmp(cmd, "t")==0){
      kprintf("\n%d enter time: ", running->pid);
      add_TQE(geti());
    }
    
  kprintf("\n");
  }
}
