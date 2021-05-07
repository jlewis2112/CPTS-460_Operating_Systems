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
#include "defines.h"
#include "type.h"

char *tab = "0123456789ABCDEF";

int kprintf(char *fmt, ...);
int kputc(char);

#include "string.c"
#include "queue.c"
#include "vid.c"

#include "kbd.c"
#include "uart.c"
#include "exceptions.c"
#include "kernel.c"
#include "message.c"



void copy_vectors(void) {
    extern u32 vectors_start;
    extern u32 vectors_end;
    u32 *vectors_src = &vectors_start;
    u32 *vectors_dst = (u32 *)0;

    while(vectors_src < &vectors_end)
       *vectors_dst++ = *vectors_src++;
}

// IRQ interrupts handler entry point
void IRQ_handler()
{
    int vicstatus, sicstatus;

    // read VIC SIC status reg to find out which interrupt
    vicstatus = VIC_STATUS;
    sicstatus = SIC_STATUS;  

    if (vicstatus & (1<<31)){ // SIC interrupts=bit_31=>KBD at bit 3 
      if (sicstatus & (1<<3)){
          kbd_handler();
       }
    }
}





  
int main()
{ 
	int i; 
   char line[128]; 
   //   u8 kbdstatus, key, scode;

   color = WHITE;
   row = col = 0; 

   fbuf_init();
   kbd_init();
   
   // allow KBD interrupts   
   VIC_INTENABLE |= (1<<31); // allow VIC IRQ31

   // enable KBD IRQ 
   SIC_ENSET = 1<<3;     // KBD int=3 on SIC
   SIC_PICENSET = 1<<3;  // KBD int=3 on SIC

   
    init();
	
	msg_init();
	kprintf("PO kfork tasks\n");
	kfork((int)sender, 1);
	kfork((int)receiver, 1);
	while(1){
		if(readyQueue)
			tswitch();
	} 
}
