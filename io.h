/* Dump binary data as program text either for BASIC or assembly or C or text dump
 */

/* $Id: io.h,v 1.3 2014/04/08 17:28:43 kjenning Exp $
 *
 * $Log: io.h,v $
 * Revision 1.3  2014/04/08 17:28:43  kjenning
 * Updated for use in Binary 2 Code program.
 *
 * Revision 1.2  2014/03/13 20:41:56  kjenning
 * Interim check-in after myglobals and documentation
 * text formatting were revised and tested.   Still
 * more features to add per the configs.
 *
 * Revision 1.1  2013/11/07 21:38:45  kjenning
 * Initial revision
 *
 */

#ifndef _B2C_IO_INCL_
#define _B2C_IO_INCL_

char * io_rcs_id( void );

typedef struct _mybuf_
{
	FILE          * myFile; /* Reading data from file or stdin */
	int             eof;    /* Flag EOF, so we don't pointlessly cycle */
	int             size;   /* size allocated to each buf */
	unsigned char * buf;    /* alloc'd buffer */
	int             len;    /* actual length of buf */
	unsigned char * nbuf;   /* alloc'd look ahead buffer */
	int             nlen;   /* actual length of nbuf */
} MyBuf;

#define INVALID_THISMYBUF ( !thisMyBuf || !thisMyBuf->myFile || !thisMyBuf->buf || !thisMyBuf->nbuf || ( thisMyBuf->size < 1 ) ) 

MyBuf * newMyBuf( char * fileName,    /* stdin or a filename */
				  int    blockSize ); /* number of bytes to group together */

int deleteMyBuf( MyBuf * thisMyBuf );

/* Read the data....
 * Technically, this only fails if the MyBuf is invalid.
 * Reading at EOF is still "successful" and shifts buffers
 * as described below.  Th difference is results are 0 length.
 *
 * First, copy the look ahead buffer to current buffer. 
 * Then fill up the look-ahead buffer up to the max size.
 * So, the results ....
 * On the first read, the current buffer is empty/zero size
 * which can be used as a flag for pre-amble output.
 * On subsequent reads the current buf should have data.
 * When the call to read finally results in an empty main 
 * buffer this means the read has really hit End Of File.
 */ 
int readFile( MyBuf * thisMyBuf );

#endif /* _B2C_IO_INCL_ */


