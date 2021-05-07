
#include "ucode.c"

char *m1 = "xwrxwrxwr------";
char *m2 = "---------------";

//ls a file entree
void ls_file(STAT *sp, char *name, char *path)
{
	u16 mode;
    int mask, k, len;
    int t = 0;
    char fullname[32], linkname[60];

    mode = sp->st_mode;//determine file type
    if ((mode & 0xF000) == 0x4000)
        mputc('d');
    if ((mode & 0xF000) == 0xA000)
        mputc('l');
    else if ((mode & 0xF000) == 0x8000)
        mputc('-');

	//print all permissions
	mask = 000400;
    for (k = 0; k < 3; k++)
    {
        if (mode & mask)
            mputc(m1[t]);
        else
            mputc(m2[t]);
        mask = mask >> 1;
	t++;
        if (mode & mask)
            mputc(m1[t]);
        else
            mputc(m2[t]);
        mask = mask >> 1;
	t++;
        if (mode & mask)
            mputc(m1[t]);
        else
            mputc(m2[t]);
	t++;
        mask = mask >> 1;
    }

    if (sp->st_nlink < 10)//link count
        printf("  %d ", sp->st_nlink);
    else
        printf(" %d ", sp->st_nlink);

    printf(" %d  %d", sp->st_uid, sp->st_gid);//size uid gid
    printf("%d ", sp->st_size);

    printf("%s", name);//name

    if ((mode & 0xF000) == 0xA000)//if link print the link
    {
        strcpy(fullname, path);
        strcat(fullname, "/");
        strcat(fullname, name);
        // symlink file: get its linked string
        len = readlink(fullname, linkname);
        printf(" -> %s", linkname);
    }

    printf("\n\r");
}



int main(int argc, char *argv[])
{
	STAT utat, *sp;
	int fd, n;
	char file[64];
	DIR *dp;
	char f[32], cwdname[64];
	char buf[1024];

	char *cp;
    sp = &utat;

    STAT dstat, *dsp;
    long size;
    char temp[32];
    int r;

	
    
    printf("**Now Running Joseph's ls**\n\r");
	//if argc file is ./ by defualt which is cwd
    if (argc == 1)
    { 
        strcpy(file, "./");
    }
    else
    {
        strcpy(file, argv[1]);
    }
	//stat file
    if (stat(file, sp) < 0)
    {
        printf("cannot stat %s\n", file);
        exit(2);
    }

    if ((sp->st_mode & 0xF000) == 0x8000)//if reg file just print
    {
        ls_file(sp, file, file);
    }
    else
    {
        if ((sp->st_mode & 0xF000) == 0x4000)
        {
        	size = sp->st_size;
    		fd = open(file, O_RDONLY); /* open dir file for READ */
			if(!fd)
			{
				printf("error fd not open\n");
				exit(0);
			}
    		while ((n = read(fd, buf, 1024)))//same as 360 traverse the dir
    		{
        		cp = buf;
        		dp = (DIR *)buf;

        		while (cp < buf + 1024)
        		{
            			dsp = &dstat;
            			strncpy(temp, dp->name, dp->name_len);
            			temp[dp->name_len] = 0;
            			f[0] = 0;
           		 		strcpy(f, file);
            			strcat(f, "/");
            			strcat(f, temp);
            			if (stat(f, dsp) >= 0)
                			ls_file(dsp, temp, file);
            			cp += dp->rec_len;
            			dp = (DIR *)cp;
 		       }
 			}
    		close(fd);
        }
    }

    exit(0);
}
