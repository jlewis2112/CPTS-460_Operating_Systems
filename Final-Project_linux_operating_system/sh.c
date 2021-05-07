#include "ucode.c"
#define BLK 1024

char *name[16];
int num_en;

char buf[1024];
int color = 0x00C;

/*init.c Joseph Lewis
This code is referrenced my KCW BOOK 
Design and Implementation of the MTX operating system
chapter 13.3.2 I also looked at chapter 8 in the other text book*/




//function used to determine the last IO redirection location
//also does IO redirection
int IO_red(int entries)
{
	int i;
	int Ilast = entries;
	for (i = 0; i < entries; i++)//scan for input
	{
		if(strcmp(name[i], "<") == 0)
		{
			printf("proc %d redirect input %s\n", getpid(), name[i+1]);
			if(Ilast > i)
				Ilast = i;
			if(name[i+1] == 0)
			{
				printf("error invalid < in command line\n");
				exit(1);
			}
			close(0);
			if(open(name[i+1], 0) < 0)
			{
				printf("error no such input file\n");
				exit(2);
			}
			break;
		}
	}

	for (i = 0; i < entries; i++)
	{
		if(strcmp(name[i], ">") == 0)//scan for output
		{
			printf("proc %d redirect output %s\n", getpid(), name[i+1]);
			if(Ilast > i)
				Ilast = i;
			if(name[i+1] == 0)
			{
				printf("error invalid > in command line\n");
				exit(3);
			}
			close(1);
			open(name[i+1], O_WRONLY | O_CREAT);
			break;
		}
	}

	for (i = 0; i < entries; i++)
	{
		if(strcmp(name[i], ">>") == 0)//scan for append
		{
			printf("proc %d append output %s\n", getpid(), name[i+1]);
			if(Ilast > i)
				Ilast = i;
			if(name[i+1] == 0)
			{
				printf("error invalid >> in command line\n");
				exit(4);
			}
			close(1);
			open(name[i+1], O_WRONLY | O_CREAT | O_APPEND);
			break;
		}
	}
	return Ilast;
}

//function used to execute the command
int command(char *s)
{
	char tbuf[64];
	int i, j, comp, ip;
	char cmdline[64];

	strcpy(tbuf, s);
	comp = eatpath(tbuf, name);

	ip = comp;

	ip = IO_red(comp);	

	strcpy(cmdline, name[0]);//copy the command and cat the arguements

	for(j = 1; j < ip; j++)
	{
		strcat(cmdline, " ");
		strcat(cmdline, name[j]);
	}

	if (getpid() < 9)//then exec to the needed file
	{
		if(exec(cmdline) < 0)
			exit(1);
	}

	while(1)
	{
		printf("%d : enter a key : ", getpid());
		getc();
	}
	return 1;
}


int scan(buf, tail) char *buf;
char **tail;
{
	char *cp;
	cp = buf;
	*tail = 0;
	while(*cp)//scan till end line
		cp++;
	while( cp != buf && *cp != '|')//scan backward until |
		cp--;
	if(cp == buf)
		return 0;
	*cp = 0;
	cp++;
	while(*cp == ' ')// skip spacings over tail
		cp++;
	*tail = cp;//change tail pointer to p
	return 1;
}


int do_pipe(char *buf, int *pd)
{
	int ifPipe, pid;
	char *tail;
	int lpd[2];//pipe being created

	if(pd)
	{
		close(pd[0]);
		close(1);
		dup(pd[1]);
		close(pd[1]);
	}

	ifPipe = scan(buf, &tail);//scan the head and tail

	if (ifPipe)
	{
		if(pipe(lpd) < 0)//creat pipe
		{
			printf("proc %d pipe call failed\n", getpid());
			exit(1);
		}
		pid = fork();
		if(pid < 0)
		{
			printf("proc %d fork failed\n", getpid());
			exit(1);
		}

		if (pid)
		{
			close(lpd[1]);
			close(0);
			dup(lpd[0]);
			printf("proc %d exec %s\n", getpid(), tail);
			command(tail);
		}
		else
		{
			do_pipe(buf, lpd);
		}
	}
	else
	{//no pipe symbol run command
		command(buf);
	}
	return 1;
}



//function that executes build functions
int exec_built()
{
	int i;
	if(strcmp(name[0], "cd")==0)//if cd do cd
	{
		if(name[1] == 0)
			chdir("/");
		else
			chdir(name[1]);
		return 1;
	}
		
	if(strcmp(name[0], "pwd") == 0)//if pwd do pwd
	{
		pwd();
		return 1;
	}

	if(strcmp(name[0], "echo") == 0) //if echo print the echoed string
	{
		for(i = 1; i < num_en; i++)
		{
			printf("%s ", name[i]);
		}
		return 1;
	}

	//if help display help menue
	if(strcmp(name[0], "?") == 0 || strcmp(name[0], "help") == 0)
	{
		printf("*************************\n");
		printf("ls cd pwd cat cp mv ps   \n");
		printf("mkdir rmdir creat rm chmod more grep\n");
		printf("lpr (I/O and Pipe) : > >> < | \n");
		printf("*************************\n");
		return 1;
	}

	//if chname call chname
	if(strcmp(name[0], "chname") == 0)
	{
		chname();
		return 1;
	}

	//if logout exit and chage dir to /
	if(strcmp(name[0], "logout") == 0)
	{
		printf("*********************\n");
		printf("LOGING OUT GOODBYE   \n");
		printf("*********************\n");
		chdir("/");
		exit(0);
	}

	if(strcmp(name[0], "exit") == 0)
	{
		exit(0);
	}
	return 0;
}


int main(int argc, char *argv)
{
	int pid, status, i, x, isbuilt;
	char buf[256], token_buf[256], *mop, *broom;

	signal(2,1);
	color = getpid() + 0x000A;
	printf("Joseph's sh running\n");

	while(1)
	{
		printf("sh %d#", getpid());
		gets(buf);
		if(buf[0] != 0)
		{
		printf("\n input = %s\n", buf);
		}
		else
		{
			continue;
		}

		broom = buf;
		while(*broom == ' ')//clean all useless spaces in the command
			broom++;		
		mop = broom;
		while(*mop)
			mop++;
		mop--;
		while(*mop == ' ')
		{
			*mop = 0;
			mop--;
		}

		if(broom[0] == 0)//if the entree only had spaces
			continue;
		
		//copy the new bufs and tokenize by ' '
		strcpy(token_buf, broom);
		strcpy(buf, token_buf);
		strcpy(token_buf, buf);
		num_en = eatpath(token_buf, name);

		isbuilt = exec_built();
		if(isbuilt == 1)
		{
			continue;
		}

		
		printf("parent sh %d: forked a child\n", getpid());
		pid = fork();

		if(pid)//if waiting for a child to die
		{
			printf("parent sh %d: wait for child %d to die\n", getpid(), pid);
			pid = wait(&status);
			printf("sh %d: child %d exit status = %x\n", getpid(), pid, status);
			continue;
		}
		else
		{
			printf("child sh %d running: cmd=%s\n", getpid(), buf);
			do_pipe(buf, 0);
		}
	}
}

