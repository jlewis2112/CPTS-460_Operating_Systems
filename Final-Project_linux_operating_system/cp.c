

#include "ucode.c"



int main(int argc, char *argv[])
{
	int file, dest, n, count;
	char buf[1024], dummy = 0;
	count = 0;
	printf("Entering Joseph's cp\n");
	if (argc < 3)//if not enough arguements
	{
		printf("usage: cp src dest\n");
		exit(1);
	}
	else
	{
		file = open(argv[1], O_RDONLY);//open for read source
		if(file < 0)
		{
			printf("open src %s error\n", argv[1]);
			exit(2);
		}
		dest = open(argv[2], O_WRONLY | O_CREAT);//open for write or creat
		while ((n = read(file, buf, 1024)))//read loop back in 360 and chapter 8
		{
			printf("n=%d ", n);
			write(dest, buf, n);//write n amount to destination
			count += n;
		}
		printf("%d bytes copied\n", count);
	}
}

