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

// queue.c file

int enqueue(PROC **queue, PROC *p)
{
  PROC *q  = *queue;
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
}

PROC *dequeue(PROC **queue)
{
  PROC *p = *queue;
  if (p)
    *queue = p->next;
  return p;
}

int printList(char *name, PROC *p)
{
  printf("%s=", name);
  while(p){
    printf("[%d%d]->", p->pid, p->priority);
    p = p->next;
  }
  printf("NULL\n");
}

int printsleepList(PROC *p)
{
  printf("sleepList=");
  while(p){
    printf("[%devent=%x]->", p->pid, p->event);
    p = p->next;
  }
  printf("NULL\n");
}

int printChildList(PROC *parent){
    PROC *p = parent->child;
    printf("childList=");
    while (p){
        printf("[%d%d]->", p->pid, p->priority);
        p = p->sibling;
    }
    printf("NULL\n");
}

TQE *tq = 0;

//Timer Queue Functions
int tenqueue (TQE *newtqe, int new) {
    TQE *ttq = tq;

    // insert in beginning
    if(!tq || newtqe->tremain < ttq->tremain || !new){
        tq = newtqe;
        newtqe->next = ttq;
        
        // Need to decrement the time of of all tqe after insertion point,
        // since they will have to wait extra time (only if new insertion)
        TQE *trail = newtqe; 
        while(ttq && new){
            ttq->tremain -= trail->tremain;
            ttq = ttq->next;
            trail = trail->next;
        }
        return;
    }
    
    //insert in middle or end
    while (ttq->next && ttq->tremain < newtqe->tremain)
    {
        newtqe->tremain -= ttq->tremain;
        ttq = ttq->next;
    }
    
    TQE *temp = ttq->next;
    ttq->next = newtqe;
    newtqe->next = temp;

    // Need to decrement the time of of all tqe after insertion point,
    // since they will have to wait extra time (only if new insertion)
    TQE *trail = newtqe;
    while (temp && new){
        temp->tremain -= trail->tremain;
        temp = temp->next;
        trail = trail->next;
    }
}

TQE * tdequeue () {
    TQE *tqe = tq;
    if (tqe){
        tq = tqe->next;
    }

    return tqe;
}

// prints the timer queue in the top corner of the screen
int printTQ () {
    TQE *ttq = tq;
    char s[8 * 9 + 7]; // room for "TQ=" + 9 procs + null at the end
    int i = 0;

    mstrcpy(s, "TQ=");
    for (; ttq; i++, ttq = ttq->next){
        s[8 * i + 3] = '[';
        s[8 * i + 4] = '0' + ttq->tremain / 10;
        s[8 * i + 5] = '0' + ttq->tremain % 10;
        s[8 * i + 6] = ' ';
        s[8 * i + 7] = '0' + ttq->pproc->pid; // between 0 and 9
        s[8 * i + 8] = ']';
        s[8 * i + 9] = '-';
        s[8 * i + 10] = '>';
    }
    s[8 * i + 3] = 'N';
    s[8 * i + 4] = 'U';
    s[8 * i + 5] = 'L';
    s[8 * i + 6] = 'L';

    for (i = 0; i < strlen(s); i++){
        kpchar(s[i], 1, 79 - strlen(s) + i);
    }
}

// int handleTQ () {
//     TQE *tqe = tdequeue();

//     tqe->tremain--;
//     if (tqe->tremain <= 0){ // pause time expired
//         kwakeup(tqe->pproc);
//     }
//     else
//         tenqueue(tqe, 0);
// }