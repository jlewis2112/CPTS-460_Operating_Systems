
#define TLOAD	0x0
#define TVALUE	0x1
#define TCNTL	0x2
#define TINTCLR	0x3
#define TRIS	0x4
#define TMIS	0x5
#define	TBGLOAD	0x6


typedef volatile struct timer{
	u32 *base;
	int tick, hh, mm, ss;
	char clock[16];
}TIMER;

volatile TIMER timer;


void timer_init()
{

	int i;
	TIMER *tp;
	kprintf("timer_init()\n");
	tp = &timer;
	tp->base = (u32 *)0x101E2000;

	*(tp->base+TLOAD) = 0x0;
	*(tp->base+TVALUE) = 0xFFFFFFFF;
	*(tp->base+TRIS) = 0x0;
	*(tp->base+TMIS) = 0x0;
	*(tp->base+TLOAD) = 0x100;
	*(tp->base+TCNTL) = 0x66;
	*(tp->base+TBGLOAD) = 0x1C00;
	//*(tp->base+TBGLOAD) = 0x2C00;   

	tp->tick = tp->hh = tp->mm = tp->ss = 0;
	tp->clock[0] = '0';
	tp->clock[1] = '0';
	tp->clock[2] = ':';
	tp->clock[3] = '0';
	tp->clock[4] = '0';
	tp->clock[5] = ':';
	tp->clock[6] = '0';
	tp->clock[7] = '0';
}

void timer_handler()
{
	u32 ris, mis, value, load, bload, i;
	TIMER *t = &timer;

	ris = *(t->base+TRIS);
	mis = *(t->base+TMIS);
	value = *(t->base+TVALUE);
	load = *(t->base+TLOAD);
	bload = *(t->base+TBGLOAD);

	t->tick++; 

	if(t->tick == 10){
		t->tick=0; t->ss++;
		count_TQE();
		if(t->ss == 60)
		{
			t->ss=0; t->mm++;
			if(t->mm == 60)
			{
				t->mm=0; t->hh++; t->hh %= 24;
			}
		}
	}
	if(t->tick == 0){
	for(i=0; i<8; i++){
		kpchar(t->clock[i], 0, 70+i);
	}

	t->clock[7] = '0'+(t->ss%10); t->clock[6] = '0'+(t->ss/10);
	t->clock[4] = '0'+(t->mm%10); t->clock[3] = '0'+(t->mm/10);
	t->clock[1] = '0'+(t->hh%10); t->clock[0] = '0'+(t->hh/10);

 	for(i=0; i<8; i++){
		kpchar(t->clock[i], 0, 70+i);
	}
	}

	timer_clearInterrupt();
	return;
}

void timer_start()
{
	TIMER *tp = &timer;

	kprintf("timer_start base= %x \n", tp->base);
	*(tp->base+TCNTL) |= 0x80;
}

int timer_clearInterrupt()
{
	TIMER *tp = &timer;
	*(tp->base+TINTCLR) = 0xFFFFFFFF;
}

void timer_stop()
{
	TIMER *tp = &timer;
	*(tp->base+TCNTL) &= 0x7F;
}
