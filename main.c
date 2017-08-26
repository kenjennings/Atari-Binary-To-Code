/* Dump binary data as program text either for BASIC or assembly or C or text dump
 */

/* $Id: main.c,v 1.3 2017/08/26 18:25:20 kenjen Exp $
 *
 * $Log: main.c,v $
 * Revision 1.3  2017/08/26 18:25:20  kenjen
 * Fixed typo in comments/usage docs.
 *
 * Revision 1.2  2014/04/08 20:26:32  kjenning
 * Looks like everything more or less works as advertized.
 * So, here's a baseline check-in.
 *
 * Revision 1.1  2014/04/08 17:25:50  kjenning
 * Initial revision
 *
 *
 */

/* 
bin2code

Dump binary data in various human-readable and program language formats.  The
program is geared toward data for use on an Atari 8-bit computer, though other
computers could also use the output from the program.  Data should be less than
64K, but the program will not stop for longer data, though in some output
options this could result in corrupted display of address.  Realistically, it is
not usually practical to build programs on an 8-bit platform using more than 64K
of data.
The program outputs the information as data statements for programming languages
-- DATA for BASIC, a number of directives for Assembly, an unsigned char array
for C, and a plain text dump.
Data may be output as hex or decimal values.  Line numbers may or may not be
included. A "picture" of the bits may also be included inside comments with
characters representing 2 or 4 color pixels.


Usage:

bin2code [options]

file          filename == Providing filename 'stdin' (default) will use stdio
                          instead of reading a file.

linenum                == Include line numbers. (default for BASIC and
                          Assembly). The C format always always overrides this
                          to turn off line numbers.

nolinenum              == Do not include line numbers. (default for text dump).
                          And C format always turns off line numbers.

start              num == Starting line number less than 32000 (default 1000).

inc                num == Line number increment. (default 10).

asm                    == Output for Assembly. (default) Example output using
                          Line Size 1 and Pixel Line size 1 and the bits comment
                          merged with the data lines:
                          1000  *=$0000
                          1010  .BYTE 00 ; . . . . . . . .
                          1020  .BYTE 18 ; . . . # # . . .
                          1030  .BYTE 3c ; . . # # # # . .
                          1040  .BYTE 7e ; . # # # # # # .

basic                  == Output for BASIC using DATA statements.  If the bitmap
                          comment is included it always appears on the following
                          lines.  This example uses Line Size 4 and Pixel Line
                          size 1:
                          1000 DATA 0,24,60,126
                          1010 REM . . . . . . . .
                          1020 REM . . . # # . . .
                          1030 REM . . # # # # . .
                          1040 REM . # # # # # # .

cc                     == Output for C as an unsigned char array. Line numbers
                          are always disabled. Example output using Group Size
                          2, Line Size 2, and Pixel Line size 2 and the bits
                          comment merged with the data lines:
                          unsigned char bytes[] =
                          {
                              0x00,0x18, / * . . . . . . . . . . . # # . . . * /
                              0x3c,0x7e  / * . . # # # # . . . # # # # # # . * /
                          };

text                   == Output as a text dump showing address, value. An
                          address is assumed, starting at 0, for the bytes.  The
                          example output uses line numbers, hex format for the
                          address and byte values, and the bits comment merged
                          with the data lines.  Group and Line Size are 1:
                          1000 0000: 00: . . . . . . . .
                          1010 0001: 18: . . . # # . . .
                          1020 0002: 3c: . . # # # # . .
                          1030 0003: 7e: . # # # # # # .
                          Example output of the address and values in decimal
                          format. Group, Line Size, and Pixel Line size are 2:
                          1000 00000: 000 024: . . . . . . . . . . . # # . . .
                          1010 00001: 060 126: . . # # # # . . . # # # # # # .

org                num == Starting address for Assembly output. (default $0000)

hex                    == Output data using hex values. ($00 or $0000) (default)

dec                    == Output data using decimal format.

byte                   == Output bytes as single byte values. (default)

dbyte                  == Outputs pairs of bytes as word values low-endian.
                          ($1234 == $12 $34)Note that this does not affect the
                          bits comment which is always output in byte order.

word                   == Outputs pairs of bytes as word values high-endian.
                          ($1234 == $34 $12)

groupsize              == Defines the number of bytes output in a group of
                          successive lines before a comment line will be output
                          (default 1).  This must be greater than or equal to
                          the line size, but need not be an exact multiple of
                          line size.  If this value does not agree with the
                          Word/DByte options, then the group size will be
                          incremented by 1 byte.

linesize               == Defines the number of bytes output per line. (default
                          1). Maximum value 64.  If this does not agree with the
                          Word/DByte options, then the line size will be
                          incremented by 1 byte.

bitscomment            == After the block of character bytes output a series of
                          lines of text representing the character bitmap as it
                          would be rendered according to the "pixeltype" value
                          using the "pixel" character values to represent the
                          font pixels, according to the "pixelsize".
                          Example shown for 8 bytes as Pixel Type 2 in C format
                          for Hex data:
                          / * 0x00:  . . . . . . . .  * /
                          / * 0x18:  . . . # # . . .  * /
                          / * 0x3c:  . . # # # # . .  * /
                          / * 0x7e:  . # # # # # # .  * /
                          / * 0x7e:  . # # # # # # .  * /
                          / * 0x18:  . . . # # . . .  * /
                          / * 0x3c:  . . # # # # . .  * /
                          / * 0x00:  . . . . . . . .  * /

nobitscomment          == Do not output the character bitmap comments. (default)

merge                  == If all other variables agree then merge the bits
                          comment with the same line as the data. The length of
                          the output is not checked, so it is possible to output
                          lines for Assembly that are too long to be entered as
                          program text.  Merge works when the output format is
                          NOT BASIC, the Group Size is a multiple of Line Size,
                          and Line Size and Pixel Line size are the same value.
                          If any of those parameters are violated then merge is
                          silently disabled. If the actual data does not agree
                          with the various Size specifications, then the last
                          line is still output with less data.

nomerge                == Do not attempt to merge the bits comment with the
                          data. (default)

pixelline              == Defines the number of bytes output per line in the
                          bits comments. (default 1). Maximum value 64.

pixeltype          num == Specify the pixel rendering type when the bitmap
                          comment is produced. The value is 2 for 2-color bit
                          interpretation (1 bit per pixel) , and 4 for 4-color
                          bit interpretation (2 bits per pixel).  (default is 2)

pixelsize          num == Specifies how many characters wide a "pixel" is when
                          the bitmap comment is produced. Valid values are 1
                          through 4. (default 2)

pixelfill          num == Specifies how many times the character(s) representing
                          a "pixel" will be written out within the pixel field
                          (specified by "pixelsize") when the bitmap comment is
                          produced.  If the size  of "pixelfill" is less than
                          "pixelsize" then the difference will be made up by
                          blank spaces. Valid values are 1 through 4. (default
                          1)

pixel0         char(s) == Specifies the character to use to represent pixel
                          value 0 when the bitmap comment is produced. Note that
                          this and all other "pixel" values can be a string of
                          multiple characters which can be used to simulate
                          screen aspect ratios when data represents actual
                          graphics. (default ".")

pixel1         char(s) == Specifies the character to use to represent pixel
                          value 1 when the bitmap comment is produced. (default
                          "#")

pixel2         char(s) == Specifies the character to use to represent pixel
                          value 2 when the bitmap comment is produced. (default
                          "O")

pixel3         char(s) == Specifies the character to use to represent pixel
                          value 3 when the bitmap comment is produced. (default
                          "X")

Later options will likely override earlier options on the command line, so
conflicting options will usually not result in an error. As an example: If a
line includes "asm" followed by "basic" then the output format will be DATA
statements for BASIC.
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <libgen.h>

/* All the global values for command line options.  */
#ifndef _MY_GLOBALS_INCL_
#include "myglobals.h"  
#endif

/* The actual moving machinery for reading the data. */
#ifndef _B2C_IO_INCL_
#include "io.h"
#endif

/* Writing data as code */
#ifndef _B2C_CODE_INCL_
#include "code.h"
#endif

#define CKP( STR ) fprintf(stderr, "Checkpoint in %s at %d: ",__FILE__,__LINE__); fprintf(stderr, STR); fputc( '\n', stderr ); fflush(NULL);

static char * rcs_id = "$Id: main.c,v 1.3 2017/08/26 18:25:20 kenjen Exp $";


static void usage( char * prgName )
{
    struct winsize wWin;
	int            conWidth = 63; /* The minimum width for the example BASIC program to print correctly */
	char *         prg      = NULL;
	int            retVal   = 0;

	/* Yes, this officially makes the program not portable to Windoze.  
	 * Time for you to upgrade to a better OS. 
	 */
    ioctl( STDOUT_FILENO, TIOCGWINSZ, &wWin ); 
 
	if ( wWin.ws_col > conWidth )
		conWidth = wWin.ws_col -1;

/*	printf ("lines %d\n", wWin.ws_row);
 *	printf ("columns %d\n", wWin.ws_col); fflush(NULL);
 */	
	if ( prgName )
		prg = basename( prgName );
	
	if ( !prg )
		prg = "bin2code";
	
	fprintf( stderr,
			 "\n%s\n\n%s\n%s\n%s\n%s\n\n",
			 prg,
 			 rcs_id,
			 global_rcs_id(),
			 myglobal_rcs_id(),
			 io_rcs_id() );
	
	retVal = docWrap( "Dump binary data in various human-readable and program language formats.  The program "
					  "is geared toward data for use on an Atari 8-bit computer, though other computers "
					  "could also use the output from the program.  Data should be less than 64K, but the "
					  "program will not stop for longer data, though in some output options this could "
					  "result in corrupted display of address.  Realistically, it is not usually practical "
					  "to build programs on an 8-bit platform using more than 64K of data.\n"
					  "The program outputs the information as data statements for programming languages -- "
					  "DATA for BASIC, a number of directives for Assembly, an unsigned char array for C, and " 
					  "a plain text dump.\n"
					  "Data may be output as hex or decimal values.  Line numbers may or may not be included. "
					  "A \"picture\" of the bits may also be included inside comments with characters "
					  "representing 2 or 4 color pixels.",
					  conWidth, /* Line Length */
					  0,        /* Indenting, none */
					  0 );      /* N/A flag to indent first line */
					  
	if ( retVal )
		fprintf(stderr,"doDocs() (header block) failure %d\n\n", retVal );
			 
	fprintf( stderr, 
			 "Usage:\n\n%s [options]\n\n",
			 prg );

	retVal = doDocs( conWidth );
			
	if ( retVal )
		fprintf(stderr,"doDocs() failure %d\n\n", retVal );

	retVal = docWrap( "Later options will likely override earlier "
					  "options on the command line, so conflicting options "
					  "will usually not result in an error. "
					  "As an example: If a line includes \"asm\" followed by \"basic\" then the "
					  "output format will be DATA statements for BASIC.\n",
					  conWidth, /* Line Length */
					  0,       /* Indenting, none */
					  0 );     /* N/A flag to indent first line */

	if ( retVal )
		fprintf(stderr,"doDocs() (trailer block) failure %d\n\n", retVal );

	fflush( NULL );
}


 
int main( int argc, char ** argv )
{
	MyBuf * fileBuf  = NULL;
	
	if ( doArgs( argc, argv, get_GA() ) ) /* Over-engineered command line parser */
	{	
		usage( argv[0] );
		return -1;
	}

	if ( sanityCheck() ) /* Verify a few command line options agree */
	{	
		usage( argv[0] );
		return -2;
	}

	dumpMyArgs();	/* Does nothing if debug is not turned on */

	fileBuf = newMyBuf( gFileName, gGroupSize );
	if ( !fileBuf )
		return -3;

	while ( !readFile( fileBuf ) ) /* Read a file, or get data from stdio */
	{	
		if ( outputData( fileBuf ) ) /* write current buffer as code */
			break;
	}

	deleteMyBuf( fileBuf );
	
	return 0;
}


