/*
 * send a file
 *
 * From X-modem loader based upon a PD xmodem from
 *      {ihnp4,ncar}!noao!grandi   grandi@noao.arizona.edu
 *
 * Rewrite by marc@dumbcat.sf.ca.us
 */

#include <string.h>
#include "xmodem.h"

extern unsigned short crctab[1 << B];	/* CRC-16 constant values,
					   see getput.c */


    /*
     * Read the first characters from the input stream and validate it.
     * Abort if not valid.  Set crcMode and buf1024.
     *
     *  crcMode:        set to 1 if crc mode selected, otherwise 0.
     *  bufsize:        set to 1024 or 128.
     */
static void getFirstChar(int *crcMode,
			 int *bufsize)
{
    int retries;
    int input;

    flushin();
    for (retries = 0; retries < NAKMAX; ++retries) {
	input = readbyte(3);
	if ((input & 0x7f) == CAN) {
	    input = readbyte(3);
	    if ((input & 0x7f) == CAN) {
		error("Send cancelled at user's request", TRUE);
	    }
	} else if (input == CRCCHR) {
	    fprintf(stderr, "CRC mode requested\n");
	    *crcMode = 1;
	    input = readbyte(1);
	    fprintf(stderr, "%s byte packets\n",
		    input == KCHR ? "1K" : "128");
	    *bufsize = (input == KCHR) ? 1024 : 128;
	    return;
	} else if (input == NAK) {
	    fprintf(stderr, "Checksum mode requested\n128 byte packets\n");
	    *crcMode = 0;
	    *bufsize = 128;
	    return;
	}
    }
    error("Remote System Not Responding", TRUE);
}


void sfile(char *name)
{
    register unsigned short checksum;
    register int bufctr;
    register int sectnum;
    char blockbuf[BBUFSIZ + 6];
    struct stat filestatbuf;
    int fd;
    int attempts;
    int sendfin;
    int bbufcnt;
    int bufsize;
    int sendresp;
    long sentsect;
    long expsect;
    char c;
    int errorMax;
    int crcMode;

    /* Check on NULL file name.  Abort transfer if NULL.  Open the file.
       Abort the transfer if we cant open the file. */

    if (strcmp(name, "") == 0) {
	sendbyte(CAN);
	sendbyte(CAN);
	sendbyte(CAN);
	error("NULL file name in send", TRUE);
    }
    fd = open(name, 0);
    if (fd < 0) {
	sendbyte(CAN);
	sendbyte(CAN);
	sendbyte(CAN);
	error("Can't open file for send", TRUE);
    }
    /* calculate the number of 'sectors' int the file. */

    stat(name, &filestatbuf);
    expsect = (filestatbuf.st_size + 127) / 128;
    sentsect = 0;
    sectnum = 1;

    /* Get the first character from the receiver.  It determins checksum/crc
       mode and 1K/128 byte block size. */

    getFirstChar(&crcMode, &bufsize);
    fprintf(stderr, "block %05d", 0);

    sendfin = FALSE;
    do {
	if ((bufsize == 1024) && ((expsect - sentsect) < 8)) {
	    fprintf(stderr,
		    "\nReducing packet size to 128 for tail end of file\n"
		    "Block %05d", sectnum);
	    bufsize = 128;
	}
	for (bufctr = 0; bufctr < bufsize;) {
	    if (getbyte(fd, &c) == EOF) {
		sendfin = TRUE;
		if (!bufctr) {
		    break;
		}
		buff[bufctr++] = CTRLZ;
		continue;
	    }
	    buff[bufctr++] = c;
	}

	if (!bufctr) {
	    break;
	}
	bbufcnt = 0;
	blockbuf[bbufcnt++] = (bufsize == 1024) ? STX : SOH;
	blockbuf[bbufcnt++] = sectnum;
	blockbuf[bbufcnt++] = ~sectnum;
	checksum = 0;
	for (bufctr = 0; bufctr < bufsize; bufctr++) {
	    blockbuf[bbufcnt++] = buff[bufctr];
	    if (crcMode) {
		checksum =
		    ((checksum << B) ^
		     crctab[(checksum >> (W - B)) ^ buff[bufctr]]);
	    } else {
		checksum = ((checksum + buff[bufctr]) & 0xff);
	    }
	}

	if (crcMode) {
	    checksum &= 0xffff;
	    blockbuf[bbufcnt++] = ((checksum >> 8) & 0xff);
	    blockbuf[bbufcnt++] = (checksum & 0xff);
	} else {
	    blockbuf[bbufcnt++] = checksum;
	}

	attempts = 0;
	if (sectnum == 1) {
	    flushin();
	    errorMax = STERRORMAX;
	} else {
	    errorMax = ERRORMAX;
	}
	fprintf(stderr, "\b\b\b\b\b%05d", sectnum);
	sendresp = NAK;
	do {
	    if ((sendresp == NAK) || (sendresp == TIMEOUT)) {
		if (attempts) {
		    fprintf(stderr, " r");
		}
		writebuf(blockbuf, bbufcnt);
		attempts++;
	    }
	    sendresp = readbyte(10);
	} while ((sendresp != ACK) && (attempts < errorMax));

	sectnum++;
	if (attempts > 1) {
	    fprintf(stderr, "\nblock %05d", sectnum);
	}
	sentsect += (bufsize == 128) ? 1 : 8;
    } while ((!sendfin) && (attempts < errorMax));

    if (attempts >= ERRORMAX) {
	sendbyte(CAN);
	sendbyte(CAN);
	sendbyte(CAN);
	sendbyte(CAN);
	sendbyte(CAN);
	error("Too many errors in transmission", TRUE);
    }
    attempts = 0;
    sendbyte(EOT);
    while ((readbyte(10) != ACK) && (attempts++ < EOTMAX)) {
	if (attempts > 1) {
	    fprintf(stderr, "\nEOT not ACKed, try %d", attempts);
	}
	sendbyte(EOT);
    }

    fprintf(stderr, "\nFile sent\n");
    if (attempts >= EOTMAX) {
	error("Remote System Not Responding on Completion", TRUE);
    }
    close(fd);
}
