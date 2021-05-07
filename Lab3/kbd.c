#include "keymap2"


// KBD registers from base address
#define KCNTL 0x00    
#define KSTAT 0x04
#define KDATA 0x08
#define KCLK  0x0C
#define KISTA 0x10

typedef struct kbd{
  char *base;
  char buf[128];
  int head, tail, data, room;
}KBD;

KBD kbd;

int kputc(char);

int shifted = 0; 
int release = 0;
int control = 0;

int kbd_init()
{
  KBD *kp = &kbd;
  kp->base = (char *)0x10006000;
  *(kp->base + KCNTL) = 0x11;    // bit4=Enable   bit0=INT on
  *(kp->base + KCLK)  = 8;       // KBD internal clock setting 
  kp->head = kp->tail = 0;
  kp->data = 0; kp->room = 128;

  release = 0;  // key release flag

  shifted = 0;  // shift key down flag
  control = 0;  // control key down flag
}

// kbd_handler() for scan code set 2

/*******************************************
  key press    =>        scanCode
  key release  =>   0xF0 scanCode
Example: 
 press   'a'   =>          0x1C
 release 'a'   =>   0xF0   0x1C
******************************************/

void kbd_handler()
{
  u8 scode, c;

  KBD *kp = &kbd;
  color = YELLOW;
  scode = *(kp->base + KDATA);  // get scan code from KDATA reg => clear IRQ

  //printf("scanCode = %x\n", scode);

  if (scode == 0xF0){       // key release 
     release = 1;           // set flag
     return;
  }

  if (scode == 0x12 || scode == 0x59)
  {
      shifted = !shifted;
      release = 0;
      return;
  }

  if(scode == 0x14)
  {
    control = !control;
    if(!control)
    {
      release = 0;
    }
    return;
  }

  if(control && release)
  {
    if(scode == 0x21)
    {
        kprintf("Control-C key\n");
    }
    
    if(scode == 0x23)
    {
      c = 0;
      c = 0x04;
      //printf("inside control d c=%x %c\n", c, c);
		kprintf("Control-D key\n");
      kp->buf[kp->head++] = c;
      kp->head %= 128;
      kp->data++; kp->room--; 
    }
    release = 0;
    return;
  }
  //above statement must come before, because release reset
  if (release && scode){    // next scan code following key release
     release = 0;           // clear flag 
     return;
  }


  else if (!shifted)            
     c = ltab[scode];
  else               // ONLY IF YOU can catch LEFT or RIGHT shift key
     c = utab[scode];
  
  kprintf("c=%x %c\n", c, c);
  
   if(!control)
   {
      kp->buf[kp->head++] = c;
      kp->head %= 128;
      kp->data++; kp->room--; 
   }

}

int kgetc()
{
  char c;
  KBD *kp = &kbd;
  //printf("%d in kgetc\n",running->pid); 

  while(kp->data == 0);

  lock();
    c = kp->buf[kp->tail++];
    kp->tail %= 128;
    kp->data--; kp->room++;
  unlock();
  return c;
}

int kgets(char s[ ])
{
  char c;
  while( (c = kgetc()) != '\r'){
    if (c=='\b'){
      s--;
      continue;
    }
    *s++ = c;
  }
  *s = 0;
  return strlen(s);
}


