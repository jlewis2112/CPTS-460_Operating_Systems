

#include "ucode.c"

//char space = ' ';
char carriage = '\r';
char newline = '\n';


int main(int argc, char *argv[])
{
	char buf[1024];
	char line[2048];
	int out=0;
	int infile=0;
	char tty[64];
	int n, i;
	char cur, pos;
	int nlines = 0;
	gettty(tty);//open the current port
	out = open(tty, 0);
	printf("entering Joseph's more\n");
	if(argv[1])//more from file
	{
		infile = open(argv[1], 0);//open file
		if(infile < 0)
			exit(1);
		n = read(infile, buf, 1024);//begin reading
		while(n > 0)
		{
			for(i=0; i<n; i++)//write to the console unilt 25 line
			{
				cur = buf[i];
				write(1, &cur, 1);
				if (cur == '\n')//if \n increase line count by 1
				{
					nlines++;
					write(2, &carriage, 1);
				}
				if(nlines > 25)//once pass 25 line
				{
					read(out, &pos, 1);//wait for a read from tty
					//if enter deduct line count by 1
					if (pos == carriage || pos == newline)
						nlines--;
					if (pos == space)//if space count = 0 new 25 lines
						nlines = 0;
				}
			}
			n = read(infile, buf, 1024);
		}
	}
	else//more from pipe
	{
		while(getline(line))
		{
			printf("%s\r",line);
			nlines++;
			if(nlines > 25)
			{
				n = read(out, &pos, 1);
				if(pos == carriage || pos == newline)
					nlines--;
				if(pos == space)
					nlines = 0;
			}
		}
	}
}

