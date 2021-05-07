
#include "defines.h"
#include "string.c"
void timer_handler();

char *tab = "0123456789ABCDEF";
int color;

#include "vid.c"
#include "kbd.c"
#include "timer.c"
#include "exceptions.c"


void copy_vectors(void) {
    extern u32 vectors_start;
    extern u32 vectors_end;
    u32 *vectors_src = &vectors_start;
    u32 *vectors_dst = (u32 *)0;

    while(vectors_src < &vectors_end)
       *vectors_dst++ = *vectors_src++;
}

int kprintf(char *fmt, ...);

void IRQ_handler()
{
    int vicstatus, sicstatus;
    int ustatus, kstatus;

    // read VIC SIV status registers to find out which interrupt
    vicstatus = VIC_STATUS;
    sicstatus = SIC_STATUS;  

    if (vicstatus & (1<<31)){
      if (sicstatus & (1<<3)){
          kbd_handler();
       }
    }
}

int main()
{
	int i;
	char line[128];
	color = RED;
	fbuf_init();

	//enable VIC interupts: timer0 at IRQ3, SIC at IRQ31
	VIC_INTENABLE = 0;
	VIC_INTENABLE |= (1<<4);
	VIC_INTENABLE |= (1<<5);
	VIC_INTENABLE |= (1<<31);
	//enable kbd irq on sic
	SIC_INTENABLE = 0;
	SIC_INTENABLE |= (1<<3);
	timer_init();
	timer_start(0);
	kbd_init();
	kprintf("C3.2 start: test KBD and TIMER drivers\n");
	unlock();
	while(1){
		color = CYAN;
		kprintf("Enter a line from KBD\n");
		kgets(line);
		kprintf("line = %s\n", line);
	}
}
