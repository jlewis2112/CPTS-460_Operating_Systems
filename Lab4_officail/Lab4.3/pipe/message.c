

#define NMBUF 10
struct semaphore nmbuf, mlock;
MBUF mbuf[NMBUF], *mbufList;




int menqueue(MBUF **queue, MBUF *p)
{
  //kprintf("enter queue\n");
  MBUF *q  = *queue;
  if (q==0 || p->priority > q->priority){
    *queue = p;
    p->next = q;
    return;
  }
  while (q->next && p->priority <= q->next->priority){
    q = q->next;
  }
  p->next = q->next;
  q->next = p;
  //kprintf("value entered\n");
}

MBUF *mdequeue(MBUF **queue)
{
  //kprintf("enter dequeue\n");
  MBUF *p = *queue;
  if (p)
    *queue = p->next;
  //kprintf("deque complete\n");
  return p;
}

int msg_init()
{
	int i; MBUF *mp;
	printf("message_init()\n");
	mbufList = 0;
	for(i=0; i<NMBUF; i++)
	{
		menqueue(&mbufList, &mbuf[i]);
	}
	nmbuf.value = NMBUF; nmbuf.queue = 0;
	mlock.value = 1; mlock.queue = 0;
}

MBUF *get_mbuf()
{
	P(&nmbuf);
	P(&mlock);
	MBUF *mp = mdequeue(&mbufList);
	V(&mlock);
	return mp;
}

int put_mbuf(MBUF *mp)
{
	P(&mlock);
	menqueue(&mbufList, mp);
	V(&mlock);
	V(&nmbuf);
}

int send(char *msg, int pid)
{
	//if(pid != 4)
	//	return -1;
	kprintf("message sender\n");
	PROC *p = &proc[pid];
	MBUF *mp = get_mbuf();
	mp->pid = running->pid;
	mp->priority = 1;
	strcpy(mp->contents, msg);
	P(&p->mQlock);
	menqueue(&p->mQueue, mp);
	V(&p->mQlock);
	V(&p->nmsg);
	return 0;
}

int recv(char *msg)
{
	kprintf("message reciever");
	P(&running->nmsg);
	P(&running->mQlock);
	MBUF *mp = mdequeue(&running->mQueue);
	V(&running->mQlock);
	strcpy(msg, mp->contents);
	int sender = mp->pid;
	put_mbuf(mp);
	return sender;
}

