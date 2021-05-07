
#define TLOAD	0x0
#define TVALUE	0x1
#define TCNTL	0x2
#define TINTCLR	0x3
#define TRIS	0x4
#define TMIS	0x5
#define TBGLOAD	0x6

typedef volatile struct timer{
	u32 *base;
	int tick, hh, mm, ss;
	char clock[16];
}TIMER; 

volatile TIMER timer[4];

void timer_init()
{
	int i; TIMER *tp;
	kprintf("timer_init()\n");
	for(i=0; i<4; i++)
	{
		tp=&timer[i];
		if (i==0) tp->base = (u32 *) 0x101E2000;
		if (i==1) tp->base = (u32 *) 0x101E2020;
		if (i==2) tp->base = (u32 *) 0x101E3000;
		if (i==3) tp->base = (u32 *) 0x101E3020;
		*(tp->base+TLOAD) = 0x0;
		*(tp->base+TVALUE)= 0xFFFFFFFF;
		*(tp->base+TRIS) = 0x0;
		*(tp->base+TMIS) = 0x0;
		*(tp->base+TLOAD) = 0x100;

		*(tp->base+TCNTL) = 0x66;
		*(tp->base+TBGLOAD) = 0x1C00;
		tp->tick = tp->hh = tp->mm = tp->ss = 0;
		strcpy((char *)tp->clock, "00:00:00");
	}
}

void timer_handler(int n)
{
	int i;
	TIMER *t = &timer[n];
	t->tick++;
	if (t->tick == 120)
	{
		t->tick = 0; t->ss++;
		if(t->ss == 60)
		{
			t->ss = 0; t->mm++;
			if(t->mm == 60)
			{
				t->mm = 0; t->hh++;
			}
		}
		t->clock[7]='0'+(t->ss%10); t->clock[6]='0'+(t->ss/10);
		t->clock[4]='0'+(t->mm%10); t->clock[3]='0'+(t->mm/10);
		t->clock[1]='0'+(t->hh%10); t->clock[0]='0'+(t->hh/10);
	}
	color = n;
	for(i=0; i<8; i++){
		kpchar(t->clock[i], n, 70+i);
	}
	timer_clearInterrupt(n);
}

void timer_start (int n)
{
	TIMER *tp = &timer[n];
	kprintf("timer_start %d base=%x", n, tp->base);
	*(tp->base+TCNTL) |= 0x80;
}

int timer_clearInterrupt(int n)
{
	TIMER *tp = &timer[n];
	*(tp->base+TINTCLR) = 0xFFFFFFFF;
}

void timer_stop(int n)
{
	TIMER *tp = &timer[n];
	*(tp->base+TCNTL) &= 0x7F;
}
