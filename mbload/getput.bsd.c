/*
 * Get characters from a file.   Get and put characters from an arbitrary
 * fd.
 */

#include "xmodem.h"

/*

 *      Get a byte from the specified file.  Buffer the read so we don't
 *      have to use a system call for each character.
 *
 */
getbyte(fildes, ch)		/* Buffered disk read */
int fildes;
char *ch;

{
    static char buf[BUFSIZ];	/* Remember buffer */
    static char *bufp = buf;	/* Remember where we are in buffer */

    if (nbchr == 0) {		/* Buffer exausted; read some more */
	if ((nbchr = read(fildes, buf, BUFSIZ)) < 0)
	    error("File Read Error", TRUE);
	bufp = buf;		/* Set pointer to start of array */
    }
    if (--nbchr >= 0) {
	*ch = *bufp++;
	return (0);
    } else {
	return (EOF);
    }
}


/* Count the number of newlines in a file so we know the REAL file size */

long countnl(fd)
int fd;
{
    char buf[BUFSIZ];
    char *bufp;
    long nltot = 0;
    int numchar;
    long lseek();

    while (numchar = read(fd, buf, BUFSIZ))	/* cycle through file */
	for (bufp = buf; numchar--; bufp++)
	    if (*bufp == '\n')
		nltot++;

    (void) lseek(fd, 0l, 0);	/* rewind file */
    return (nltot);
}


/*   CRC-16 constant array...
   from Usenet contribution by Mark G. Mendel, Network Systems Corp.
   (ihnp4!umn-cs!hyper!mark)
 */

/* crctab as calculated by initcrctab() */
unsigned short crctab[1 << B] =
{
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
    0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
    0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
    0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
    0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
    0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
    0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
    0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
    0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
    0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
    0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
    0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
    0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
    0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
    0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
    0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
    0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
    0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
    0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
    0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
    0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
    0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};


/* get a byte from data stream -- timeout if "seconds" elapses */
/* This routine is VERY 4.2 specific */

int readbyte(seconds)
int seconds;
{
    int readfd;
    char c;
    struct timeval tmout;

    tmout.tv_sec = seconds;
    tmout.tv_usec = 0;

    readfd = 1 << 0;

    if ((select(1, &readfd, (int *) 0, (int *) 0, &tmout)) == 0)
	return (TIMEOUT);

    read(0, &c, 1);

    return (c & 0xff);		/* return the char */
}

/* flush input stream by reading pending characters */

flushin()
{
    int readfd;
    char inbuf[BBUFSIZ];
    struct timeval tmout;

    while (1) {
	/* set up a usec timeout on stdin */
	tmout.tv_sec = 0;
	tmout.tv_usec = 1000;
	readfd = 1 << 0;

	/* any characters pending?; return if none */

	if ((select(1, &readfd, (int *) 0, (int *) 0, &tmout)) == 0) {
	    return;
	}
	/* read the characters to flush them (assume there are fewer than
	   BBUFSIZ */

	(void) read(0, inbuf, BBUFSIZ);
    }
}

/* send a byte to data stream */

sendbyte(data)
char data;
{
    if (write(1, &data, 1) != 1)	/* write the byte (assume it goes NOW; no
					   * flushing needed) */
	error("Write error on stream", TRUE);
    return;
}

/* send a buffer to data stream */

writebuf(buffer, nbytes)
char *buffer;
int nbytes;
{
    if (write(1, buffer, nbytes) != nbytes)	/* write the buffer (assume
						   * no TIOCFLUSH needed) */
	error("Write error on stream", TRUE);
    return;
}


/*
 * "nap" for specified time -- VERY 4.2BSD specific
 */

napms(milliseconds)
int milliseconds;
{
    struct timeval timeout;
    int readfd;

    if (milliseconds == 0)
	return;
    timeout.tv_sec = 0;
    timeout.tv_usec = milliseconds * 1000;
    readfd = 0;

    (void) select(1, &readfd, (int *) 0, (int *) 0, &timeout);
}



/* set and restore tty modes for XMODEM transfers */
/* These routines are 4.2/v7(?) specific */

struct sgttyb ttys, ttysnew;	/* for stty terminal mode calls */
struct stat statbuf;		/* for terminal message on/off control */

int wason;			/* holds status of tty read write/modes */
char *tty;			/* current tty name */


setmodes(ttyspeed)
int ttyspeed;
{
    char *ttyname();

    int n;
    int speedIx;
    static int speeds[] =
    {
	0, 50, 75, 110, 134, 150, 200, 300, 600, 1200, 1800, 2400, 4800,
	9600, 19200, 38400, -1
    };
    extern onintr();

    /* Validate the input speed is valid.  If not return false.  We
       validate 0 as a speed.  It means leave the speed alone. */

    for (speedIx = 0; speeds[speedIx] != ttyspeed; ++speedIx) {
	if (speeds[speedIx] < 0) {
	    return FALSE;
	}
    }
    sleep(2);			/* let the output appear */
    if (ioctl(0, TIOCGETP, &ttys) < 0)	/* get tty params [V7] */
	error("Can't get TTY Parameters", TRUE);

    tty = ttyname(0);		/* identify current tty */

    if (speedIx) {
	ttysnew.sg_ispeed = speedIx;
	ttysnew.sg_ospeed = speedIx;
    }
    ttysnew.sg_flags |= RAW;	/* set for RAW Mode */
    ttysnew.sg_flags &= ~ECHO;	/* set for no echoing */
    ttysnew.sg_flags &= ~TANDEM;	/* turn off flow control */

    /* set new paramters */
    if (ioctl(0, TIOCSETP, &ttysnew) < 0)
	error("Can't set new TTY Parameters", TRUE);

    /* Flush characters waiting for read or write */
    n = 0;
    if (ioctl(0, TIOCFLUSH, &n) < 0)
	error("Can't flush terminal queue", TRUE);

    /* get tty status */
    if (stat(tty, &statbuf) < 0)
	error("Can't get your TTY Status", TRUE);

#if 0
    if (statbuf.st_mode & 022)	/* Need to turn messages off */
	if (chmod(tty, (int) statbuf.st_mode & ~022) < 0)
	    error("Can't change  TTY mode", TRUE);
	else
	    wason = TRUE;
    else
#endif
	wason = FALSE;

    /* set up signal catcher to restore tty state if we are KILLed */

    if (signal(SIGTERM, SIG_IGN) != SIG_IGN)
	signal(SIGTERM, onintr);
    return TRUE;
}


/* restore normal tty modes */

restoremodes(errcall)
int errcall;
{
    if (wason)
	if (chmod(tty, (int) statbuf.st_mode | 022) < 0)
	    error("Can't change TTY mode", FALSE);
    if (ioctl(0, TIOCSETP, &ttys) < 0) {
	if (!errcall)
	    error("RESET - Can't restore normal TTY Params", FALSE);
	else
	    printf("RESET - Can't restore normal TTY Params\n");
    }
    if (signal(SIGTERM, SIG_IGN) != SIG_IGN)
	signal(SIGTERM, SIG_DFL);
    return;
}




/* signal catcher */
onintr()
{
    error("Kill signal; bailing out", TRUE);
}


/* create string with a timestamp for log file */

char *
 stamptime()
{
    char *asctime();		/* stuff to get timestamp */
    struct tm *localtime(), *tp;
    struct timeval tv;
    struct timezone tz;

    gettimeofday(&tv, &tz);	/* fill in timestamp */
    tp = localtime((time_t *) & tv.tv_sec);
    return (asctime(tp));
}
