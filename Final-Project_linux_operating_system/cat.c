
#include "ucode.c"

char newline = '\n';
char carriage = '\r';

int main(int argc, char *argv[])
{
	int fd, i, n;
	char buf[1024], dummy;
	char *cp;
	fd = 0;
	if(argc < 2)//if arguement is less than 2 than its a pipe or stdio
	{
		while (gets(buf))//use gets and print
		{
			printf("%s", buf);
		}
		exit(0);
	}
	else//we need to open a file
	{
		fd = open(argv[1], O_RDONLY);
		if(fd < 0)
		{
			printf("cat %s error\n", argv[1]);
			exit(0);
		}
	
		while((n = read(fd, buf, 1024)))
		{
			buf[n] = 0;
			for(i = 0; i < n; i++)
			{
				write(1, &buf[i], 1);
				if(buf[i] == '\n')//if a \n occurs write carrige
					write(2, &carriage, 1);
			}
		}
		close(fd);
		exit(0);
	}
}
