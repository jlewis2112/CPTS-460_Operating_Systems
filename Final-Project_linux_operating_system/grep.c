#include "ucode.c"

char *cp, mtty[32];
char uline[2048], buf[1024], zero;
char pattern[2048];

int instr(char *src, char *target)//determine if pattern is true 
{
	int i;
	for(i = 0; i<strlen(src) - strlen(target); i++)
	{
		if(strncmp(&src[i], target, strlen(target)) == 0)
		{
			return 1;
		}
	}
	return 0;
}



int main(int argc, char *argv[])
{
	int fd, n, count, cr, i, j, newline, backspace;
	int lineread;

	STAT st0, st1, sttty;
	int redirect, red1;

	printf("Now entering Joseph's grep\n");

	cr = '\r';
	newline = '\n';
	gettty(mytty);
	fstat(0, &st0);
	fstat(1, &st1);
	redirect = 1;
	
	if(st0.st_dev != 0 && st0.st_ino != 0)//int dev and ino are 0 then pipe
		redirect = 0;

	if(argc < 2)//if one entree
	{
		printf("usage : grep pattern filename\n");
		exit(1);
	}

	if(argc == 2)//if 2 entrees
	{
		//grep for piping
		if(redirect)
		{
			lineread = 1;
			while(lineread)
			{
				lineread = getline(uline);
				if(instr(uline, argv[1]))
					printf("%s", uline);
			}
		}
		else //grep for stdin
		{
			while(gets(uline))
			{
				strcpy(pattern, argv[1]);
				pattern[strlen(pattern)-1] = 0;
				uline[strlen(uline)] = 0;
				if(instr(uline, pattern))
					printf("%s\n", uline);
			}
		}
		
	}
	else//grep for a file
	{
		printf("open %s for read\n", argv[2]);
		fd = open(argv[2], O_RDONLY);
		if(fd < 0)
		{
			printf("error open file failed\n");
			exit(2);
		}

		count = 0;
		while((n = read(fd, buf, 1024)))//read file
		{
			buf[n] = 0;
			uline[0] = 0;
			j = 0;
			for(i=0; i < n; i++)//traverse copy buffer until \n
			{
				if(buf[i] == '\n' || buf[i] == '\r')
					break;
				uline[j++] = buf[i];
				count++;
			}
			uline[j] = 0;
			count++;
			//determine if the pattern instr
			if(instr(uline, argv[1]))
				printf("%s\n", uline);

			lseek(fd, (long)count, 0);//change the read pointer
		}
	}
}
