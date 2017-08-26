/* Dump binary data as program text either for BASIC or assembly or C or text dump
 */

/* $Id: io.c,v 1.8 2014/04/08 17:28:43 kjenning Exp $
 *
 * $Log: io.c,v $
 * Revision 1.8  2014/04/08 17:28:43  kjenning
 * Updated for use in Binary 2 Code program.
 *
 * Revision 1.7  2014/03/31 20:53:36  kjenning
 * Added Assembly origin command line argument.
 * Cleaned up assemby output.
 * Probably made the code even cruftier.
 * Rearranged some of the help text.
 *
 * Revision 1.6  2014/03/26 20:44:11  kjenning
 * Slight cleanup of bloaty cruftiness.   A little modularity
 * inflicted on the I/O section.  Probably fixed the C
 * output bug that resulted in a trailing comma at the end
 * of data when character comments were used.
 *
 * Revision 1.5  2014/03/18 19:47:11  kjenning
 * Bells and Whistles implemented and some sanity
 * testing completed.  This seems to be working
 * well enough, so here's a check-in baseline.
 *
 * Revision 1.4  2014/03/13 20:41:56  kjenning
 * Interim check-in after myglobals and documentation
 * text formatting were revised and tested.   Still
 * more features to add per the configs.
 *
 * Revision 1.3  2013/11/07 21:38:45  kjenning
 * Added command line controls for outputting
 * decorative comments with the data.  The
 * actual code to do the decorating is not
 * included here. Wait for a future version.
 *
 * Revision 1.2  2013/10/21 20:48:50  kjenning
 * Temporary Checkin.
 * Most functionality had been working.
 * Next unecessary revision of the globals due
 * to begin after this version.
 *
 * Revision 1.1  2013/10/17 20:36:08  kjenning
 * Initial revision
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>

#ifndef _MY_GLOBALS_INCL_
#include "myglobals.h"
#endif

#ifndef _B2C_IO_INCL_
#include "io.h"
#endif

#define CKP( STR ) fprintf(stderr, "Checkpoint in %s at %d: ",__FILE__,__LINE__); fprintf(stderr, STR); fputc( '\n', stderr ); fflush(NULL);


/* I/O here is different from the previous atf2code program.  That program
 * expected to read the complete font into memory first, and then output the 
 * all the buffered data.
 * This program reads from input in chunks defined by the Group Size.
 * As each chunk is read, then it is written as formatted output.
 * Also, correct language syntax in some cases needs to know if more data
 * follows the current data.  This is managed by always reading ahead to 
 * determine if there is additional content.  The "old", previously read data
 * is then output.  Then the "new" data from the look-ahead read is copied 
 * to the "old" buffer and then a new look-ahead read is performed.  
 * Therefore, the program always knows the next state of the file while 
 * working with current data.
 */


static char * rcs_id = "$Id: io.c,v 1.8 2014/04/08 17:28:43 kjenning Exp $";

char * io_rcs_id( void ) /* report file version for io.o */
{
	return rcs_id;
}


/* A thing to manage the input file, data read from the file
 * and a read-ahead buffer for the next block.
 */
MyBuf * newMyBuf( char * fileName,    /* stdin or a filename */
				  int    blockSize )  /* number of bytes to group together */
{
	MyBuf * thisMyBuf = NULL;

/* CKP( "newMyBuf()" );
 * fprintf(stderr, "filename = %p %s\n", fileName, fileName ? fileName : "NULL" );fflush(NULL);
 */ 
	if ( ( blockSize < 1 ) || ( blockSize > 65536 ) )
	{
		fprintf( stderr,
				 "Error: newMyBuf(): blockSize( %d ) is invalid\n", 
				 blockSize );
		return NULL;
	}

	thisMyBuf = (MyBuf *)malloc( sizeof( MyBuf ) );
	if ( !thisMyBuf )
	{
		fprintf( stderr,
				 "Error: newMyBuf(): unable to malloc( sizeof( MyBuf ) );\n" );
		return NULL;
	}

	/* By initializing everything to a known state the program can
	 * now use the delete() function to cleanup instead of a localized,
	 * custom cleanup for each step later in this function.
	 */
	thisMyBuf->myFile = NULL;      /* Reading data from file or stdin */
	thisMyBuf->eof    = 0;         /* Flag indicates reading it over */
	thisMyBuf->size   = blockSize; /* size allocated to each buf */
	thisMyBuf->buf    = NULL;      /* alloc'd buffer */
	thisMyBuf->len    = 0;         /* actual length of buf */
	thisMyBuf->nbuf   = NULL;      /* alloc'd look ahead buffer */
	thisMyBuf->nlen   = 0;         /* actual length of nbuf */
	
	thisMyBuf->buf = (unsigned char *)malloc( blockSize );
	if ( !thisMyBuf->buf )
	{
		deleteMyBuf( thisMyBuf );
		fprintf( stderr,
				 "Error: newMyBuf(): unable to malloc( blockSize ); for MyBuf.buf\n" );
		return NULL;
	}
	
	thisMyBuf->nbuf = (unsigned char *)malloc( blockSize );
	if ( !thisMyBuf->nbuf )
	{
		deleteMyBuf( thisMyBuf );
		fprintf( stderr,
				 "Error: newMyBuf(): unable to malloc( blockSize ); for MyBuf.nbuf\n" );
		return NULL;
	}

	/* Filename defined and is not "stdin" ? */
	if ( fileName && *fileName && strcmp( fileName, "stdin" ) )
	{
		thisMyBuf->myFile = fopen( fileName, "r" );
		if ( !thisMyBuf->myFile )
		{
			deleteMyBuf( thisMyBuf );
			fprintf( stderr,
					 "Error: newMyBuf(): unable to fopen(\"%s\", \"r\");\n",
					 fileName );
			return NULL;
		}
	}
	else /* reading stdin */
		thisMyBuf->myFile = stdin;
		
	return thisMyBuf;
}


/* Anything that's been allocated has to be freed.
 * The only way this could be more thorough is to 
 * accept a double indirect **, so the delete 
 * function could also NULL the caller's pointer.
*/
int deleteMyBuf( MyBuf * thisMyBuf )
{
	if ( !thisMyBuf )
		return -1;
	
	if ( thisMyBuf->buf )
		free( thisMyBuf->buf );

	if ( thisMyBuf->nbuf )
		free( thisMyBuf->nbuf );

	if ( thisMyBuf->myFile && ( thisMyBuf->myFile != stdin ) )
		fclose( thisMyBuf->myFile );
		
	free( thisMyBuf );

	return 0;
}



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
int readFile( MyBuf * thisMyBuf )
{
	unsigned char * temp = NULL;
	int ch               = 0;
	int done             = 0;
	
	if ( INVALID_THISMYBUF )
	{
		fprintf( stderr, "Error: readFile( thisMyBuf ): thisMyBuf is not valid\n" );
		return -1;
	}
		
	/* "copy" read ahead buf to current buf. */
	temp            = thisMyBuf->buf;  /* Most would have decided to */
	thisMyBuf->buf  = thisMyBuf->nbuf; /* memcpy(), but this looks a */
	thisMyBuf->nbuf = temp;            /* bit faster. */
	thisMyBuf->len  = thisMyBuf->nlen; /* Don't forget to transfer length, too. */
	
	/* Prep to refill read ahead buf. */
	thisMyBuf->nlen = 0;
	
	/* add characters to nbuf via temp and keep track of the length
	 * until EOF or we match the size, whichever comes first.  The 
	 * EOF flag check here prevents cycling on no bytes.  Note that
	 * on EOF we do want to process and exit as if a normal read 
	 * occurred, so that the prior, read-ahead buffer becomes the 
	 * current buffer.  Code using the buffer knows there is no more
	 * data based on the zero size of thisMyBuf->nlen
	 */
	while ( !done && !thisMyBuf->eof )
	{
		if ( thisMyBuf->nlen == thisMyBuf->size )
		{
			done++;
			continue;
		}
		
		ch = fgetc( thisMyBuf->myFile );

		if ( ch == EOF ) 
		{
			thisMyBuf->eof++;
			done++;
			continue;
		}
		
		*temp++ = ch;
		thisMyBuf->nlen++;
	}
	
/* CKP("ReadFile():");
 * fprintf(stderr,"Read %d bytes \n", 	thisMyBuf->nlen );
 */
	return 0;
}
