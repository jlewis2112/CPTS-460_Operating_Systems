#include "ucode.c"
#define printk printf

int console, serial0, serial1;


/*init.c Joseph Lewis
This code is referrenced my KCW BOOK 
Design and Implementation of the MTX operating system
chapter 13.3.1*/

int main(int argc, char *argv[ ])
{
  int infile, outfile;
  int pid, status;
  
  //open infile outfile
  infile  = open("/dev/tty0", O_RDONLY);
  outfile = open("/dev/tty0", O_WRONLY);

  pid = getpid();

  printf("proc %d in Umode \n", pid);
  printf("argc= %d %s %s\n", argc, argv[0], argv[1]);

  printf("INIT: fork login task on console\n");
  printf("fork ttyo\n");
  console = fork();//fork console 
  if (console){ 
	printf("fork ttyS0\n");
	serial0 = fork();//fork first serail port
	if(serial0)
	{
		printf("fork ttyS1\n");
		serial1 = fork();//fork second serial port
		if(serial1)//entering parent
		{
			printf("INIT: waiting ...\n");
			while(1)
			{
				pid = wait(&status);//wait for pid
				//determine pid for console or serial port
				//fork process after that
				if (pid == console)
				{
					console = fork();
					if(console)
						continue;
					else
						exec("login /dev/tty0");
				}
				if (pid == serial0)
				{
					serial0 = fork();
					if(serial0)
						continue;
					else
						exec("login /dev/ttyS0");
				}
				if (pid == serial1)
				{
					serial1 = fork();
					if(serial1)
						continue;
					else
						exec("login /dev/ttyS1");
				}
				printf("INIT buried an orphan child task %d\n", pid);
			}
		}
		else
			exec("login /dev/ttyS1");
	}
	else
		exec("login /dev/ttyS0");
    }
	else
		exec("login /dev/tty0");
}




