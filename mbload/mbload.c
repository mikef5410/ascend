/*
 *  Multiband X-modem loader based upon a PD xmodem from
 *      {ihnp4,ncar}!noao!grandi   grandi@noao.arizona.edu
 *
 *  Severely modified by marc@dumbcat.sf.ca.us
 */

/* Modified again by Mike Ferrara mikef@sr.hp.com to support HP-UX */
#include "xmodem.h"

main(int argc,
     char **argv)
{
    extern int optind;
    extern char *optarg;
    int opt;
    int escape = FALSE;
    int mbplus = FALSE;
    int nindy = FALSE;
    struct stat filestatbuf;

    fprintf(stderr, "\nMBLOAD Version %d.%d\n", VERSION / 10, VERSION % 10);

    /* The default bit rate, 0, signifies no change.  can be overriden */
    ttyspeed = 0;
    while ((opt = getopt(argc, argv, "enps:t:")) != -1) {
	switch (opt) {
	case 'e':
	    escape = TRUE;
	    break;

	case 'n':
	    nindy = TRUE;
	    break;

	case 'p':
	    mbplus = TRUE;
	    break;

	case 's':
	    ttyspeed = atoi(optarg);
	    break;

	case 't':
	    ttydev = optarg;
	    break;

	case '?':
	    fprintf(stderr, "%s [-e[p]|n] [-s speed] [-t ttydev] file", argv[0]);
	    return 1;
	}
    }
    ttyfp = fopen(ttydev, "r+");
    if (ttyfp != NULL) {
	ttyin = fileno(ttyfp);
	ttyout = fileno(ttyfp);
    } else {
	ttyin = 0;
	ttyout = 1;
    }
    tcflush(ttyin, TCIOFLUSH);
    tcflush(ttyout, TCIOFLUSH);
    if (optind < argc) {
	if (stat(argv[optind], &filestatbuf) < 0)
	    error("Can't find requested file", FALSE);
	if (ttyspeed) {
	    fprintf(stderr, "File %s\nReady to SEND at %d bit/s\n",
		    argv[optind], ttyspeed);
	} else {
	    fprintf(stderr, "File %s\nReady to SEND at current bit rate\n",
		    argv[optind], ttyspeed);
	}
	fprintf(stderr, "Estimated File Size %ldK (%ld Bytes)\n",
		(filestatbuf.st_size / 1024) + 1, filestatbuf.st_size);

	if (!setmodes(ttyspeed)) {
	    error("unknown speed selected", FALSE);
	}
	sleep(2);
	if (escape) {
	    sendbyte(ESC);
	    sendbyte('[');
	    sendbyte(ESC);
	    sendbyte('-');
	    /*      writebuf("\n\n[-",6); */
	}
	if (nindy) {
	    sendbyte('\r');
	    sendbyte('d');
	    sendbyte('f');
	    sendbyte('\r');
	}
	if (mbplus) {
	    sendbyte('\r');
	    sendbyte('D');
	    flushin();
	    sendbyte('\r');
	    sendbyte('\r');
	}
	sfile(argv[optind]);
	flushin();
	restoremodes(FALSE);
    } else {
	error("Fatal - No file specified to send", FALSE);
    }
    sleep(2);
    exit(0);
}

/* print error message and exit; if mode == TRUE, restore normal tty modes */
error(char *msg,
      int mode)
{
    if (mode) {
	restoremodes(TRUE);
    }
    fprintf(stderr, "\r\n%s\n", msg);
    exit(-1);
}
