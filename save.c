/*
 * save and restore routines
 *
 * @(#)save.c	3.5 (Berkeley) 4/16/81
 */

#include <curses.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

typedef struct stat STAT;

extern char version[];
extern int wizard;
extern WINDOW *notes;

char *sbrk();

STAT sbuf;
char file_name[32];

save_game()
{
    register int *savef;
    char buf[80];

    /*
     * get file name
     */
	if (!*file_name)
		strcpy(file_name, "midway.save");
	sprintf(buf, "Save file (\"%s\")? ", file_name);
	inform(buf, 0);
	wreadstr(notes, buf);
	if (*buf == 'y' || *buf == 'Y')
	    goto gotfile;

    do
    {
	inform("File name: ", 0);
	wreadstr(notes, file_name);

gotfile:
	if ((savef = open(file_name, 1)) == NULL)
	    inform("Bad file number.", 0); 
    } while (savef == NULL);

    /*
     * write out encrpyted file (after a stat)
     * The fwrite is to force allocation of the buffer before the write
     */
    save_file(savef);
}

/*
 * write the saved game on the file
 */
save_file(savef)
register FILE *savef;
{
    /*
     * the fwrite of junk is to force the allocation of the stdio file
     * buffer for savef before getting sbrk(0), so it will be right
     */
    write(savef, "junk", 5);
    lseek(savef, 0L, 0);
    fstat(savef, &sbuf);
    write(savef, version, (unsigned) (sbrk(0) - version));
    mvcur(0, COLS -1, LINES -1, 0);
    echo();
    nocrmode();
    exit(0);
}

restore(file, envp)
register char *file;
char **envp;
{
    register int inf;
    int interrupt();
    char buf[80];
    STAT sbuf2;

    if ((inf = open(file, 0)) < 0)
    {
	perror(file);
	exit(1);
    }

    fflush(stdout);
    read(inf, buf, (unsigned) (strlen(version) + 1));
    if (strcmp(buf, version) != 0)
    {
	printf("Sorry, saved game is out of date.\n");
	exit(1);
    }

    fstat(inf, &sbuf2);
    fflush(stdout);
    if (brk(version + sbuf2.st_size) < 0)
	perror("brk");
    lseek(inf, 0L, 0);
    read(inf, version, (unsigned) sbuf2.st_size);
    /*
     * we do not close the file so that we will have a hold of the
     * inode for as long as possible
     */

    if (!wizard)
	if (sbuf2.st_ino != sbuf.st_ino || sbuf2.st_dev != sbuf.st_dev)
	{
	    printf("Sorry, saved game is not in the same file.\n");
	    exit(1);
	}
    /*
     * defeat multiple restarting from the same place
     */
    if (!wizard)
	if (sbuf2.st_nlink != 1)
	{
	    printf("Cannot restore from a linked file\n");
	    exit(1);
	}
	else if (unlink(file) < 0)
	{
	    printf("Cannot unlink file\n");
	    exit(1);
	}

    if (!My_term && isatty(2))
    {
	register char	*sp;

	_tty_ch = 2;
	gettmode();
	if ((sp = getenv("TERM")) == NULL)
	    sp = Def_term;
	setterm(sp);
    }
    else
	setterm(Def_term);
    strcpy(file_name, file);
    srand(getpid());
    redraw();
    inform("Game saved from", 0);
    inform(ctime(&sbuf2.st_ctime), 0);
    signal(SIGINT, interrupt);
    crmode();
    noecho();
    playit();
    /*NOTREACHED*/
}
