
#include "ucode.c"


/*init.c Joseph Lewis
This code is referrenced my KCW BOOK 
Design and Implementation of the MTX operating system
chapter 13.3.2*/


int main(int argc, char *argv[])
{
	char buf[1024], entername[64], enterpass[64];//buf entered values
	char copyline[64], argvc[64];
	char *cp, *cq, *cpp, *cqq;
	int i, n;

	int in, out, err;
	int fd;
	char curtty[64];

	char *username;
	char *password;
	char *gidx;
	char *uidx;
	char *homedir;
	char *program;
	char *fullname;
	char *valuex;

	close(0);//close file descriptors 1, 0
	close(1);

	strcpy(argvc, argv[1]);//open in out err in argv[1]
	in = open(argvc, 0);
	out = open(argvc, 1);
	err = open(argvc, 2);

	settty(argvc);
	gettty(curtty);

	printf("login: tty=%s\n", curtty);
	printf("open %s as stdin= %d stdout= %d stderr=%d\n", argvc, in, out, err);

	//open the password file
	fd = open("/etc/passwd", 0);
	if(fd < 0)
	{
		printf("error no password file\n");
		exit(1);
	}

	while(1)
	{
		n = read(fd, buf, 1024);
		buf[n] = 0;
		cp = cq = buf;
		
		//enter user name and password
		printf("login:"); gets(entername);
		printf("password:"); gets(enterpass);

		//reading the file and splitting the needed char values
		//we tokenize by : 
		while(cp < &buf[n])
		{
			while(*cq != '\n')
			{
				if(*cq == ' ')
					*cq = '-';
				if(*cq == ':')
					*cq = ' ';
				cq++;
			}
			*cq = 0;

			strcpy(copyline, cp);
			cpp = cqq = copyline;
			i = 0;

			while(*cqq)
			{
				if(*cqq == ' ')
				{
					*cqq = 0;
					switch(i){
						case 0: username = cpp; break;
						case 1: password = cpp; break;
						case 2: gidx = cpp; break; 
						case 3: uidx = cpp; break;
						case 4: fullname = cpp; break;
						case 5: homedir = cpp; break;
						case 6: program = cpp; break;
						case 7: valuex = cpp; break;
						default: break;
					}
					i++;
					cqq++;
					cpp = cqq;
					continue;
				}
				cqq++;
			}
			//determine if the password is correct
			if(strcmp(entername, username) == 0 && strcmp(enterpass, password) == 0)
			{
				//if it is cd to /, change uid, close
				printf("LOGIN: welcome Joseph\n");
				printf("cd to HOME %s\n", homedir);
				chdir(homedir);
				printf("change uid to %d\n", atoi(uidx));
				chuid(atoi(uidx), 0);
				printf("exec to sh\n");
				close(fd);
				exec("sh jksh start");
			}
			cq++;
			cp = cq;
		}
		//if not close fd and do the loop again.
		printf("login failed, try again\n");	
		close(fd);
		fd = open("/etc/passwd", 0);
	}
}


	
