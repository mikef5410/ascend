/*
 *  Multiband X-modem loader based upon a PD xmodem from
 *      {ihnp4,ncar}!noao!grandi   grandi@noao.arizona.edu
 *
 *  Severely modified by marc@dumbcat.sf.ca.us
 */

#include <ctype.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#if defined( SYSV )
#include <termios.h>
#include <unistd.h>
#else
/* # include <sgtty.h> */
#include <termios.h>
#endif
#include <signal.h>


#define VERSION		51	/* Version Number */
#define FALSE	 	0
#define TRUE		(! FALSE )

/*  ASCII Constants  */
#define      SOH	001
#define	     STX	002
#define	     ETX	003
#define      EOT	004
#define	     ENQ	005
#define      ACK	006
#define	     LF		012	/* Unix LF/NL */
#define	     CR		015
#define      NAK	025
#define	     SYN	026
#define	     CAN	030
#define	     ESC	033

/*  XMODEM Constants  */
#define      TIMEOUT	-1
#define      ERRORMAX	5	/* maximum errors tolerated while
				   transferring a packet */
#define      STERRORMAX	60	/* maximum "errors" tolerated in read startup */
#define      NAKMAX	120	/* maximum times to wait for initial NAK
				   when sending */
#define      EOTMAX	10	/* maximum times sender will send an EOT to
				   end transfer */
#define	     BBUFSIZ	1024	/* buffer size */
#define	     CTRLZ	032	/* CP/M EOF for text (usually!) */
#define      CRCCHR	'C'	/* CRC request character */
#define      KCHR	'K'	/* 1K block request character */

#define      CREATMODE 0644	/* mode for created files */

/* GLOBAL VARIABLES */

int ttyspeed;			/* tty speed (bits per second) */
unsigned char buff[BBUFSIZ];	/* buffer for data */
int nbchr;			/* number of chars read so far for
				   buffered read */
long filelength;		/* length specified in YMODEM header */
long fileread;			/* characters actually read so far in file */
char filename[256];		/* place to construct filenames */
char *ttydev;
FILE *ttyfp;
int ttyin, ttyout;

/*   CRC-16 constants.   From Usenet contribution by Mark G. Mendel, 
   Network Systems Corp.  (ihnp4!umn-cs!hyper!mark)
 */

    /* the CRC polynomial. */
#define	P	0x1021

    /* number of bits in CRC */
#define W	16

    /* the number of bits per char */
#define B	8
