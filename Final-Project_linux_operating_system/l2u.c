
#include "ucode.c"

int main(int argc, char *argv[])
{
	int file, dest, n, c;
	char buf[1024], *cp;

	printf("entering Joseph's l2u\n");
	
	if(argc == 1)
	{ //convert by typing
		c = getc();
		while(c != EOF)//while read c 1 byte in entrees
		{
			//c &= 0x7F;
			if(c >= 'a' && c <= 'z')//if char has int value a-z
				mputc(c - 'a' + 'A');//minus a and add A
			else
				mputc(c);
			if(c == '\r')
				mputc('\n');
			c = getc();
		}
		exit(0);
	}
	if(argc < 3)//2 arguements not allowed
	{
		printf("usage: l2u f1 f2\n");
		exit(1);
	}//when argc = 3 open for read
	file = open(argv[1], O_RDONLY);
	if(file < 0)
	{
		printf("error open failed\n");
		exit(1);
	}
	dest = open(argv[2], O_WRONLY | O_CREAT);// creat new file to copy

	while(n = read(file, buf, 1024))//read then convert to capital letters
	{
		cp = buf;
		while(cp < buf + n)//pointer cp will traverse through buffer
		{
			if(*cp >= 'a' && *cp <= 'z')
			{
				*cp = *cp - 'a' + 'A';
			}
			cp++;
		}
		write(dest, buf, n);
	}
	printf("done\n");
}
