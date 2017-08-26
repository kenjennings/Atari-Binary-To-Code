/* Dump font data as program text either for BASIC or assembly or C
 */

/* $Id: code.c,v 1.2 2014/04/08 20:26:32 kjenning Exp $
 *
 * $Log: code.c,v $
 * Revision 1.2  2014/04/08 20:26:32  kjenning
 * Looks like everything more or less works as advertized.
 * So, here's a baseline check-in.
 *
 * Revision 1.1  2014/04/08 17:25:50  kjenning
 * Initial revision
 *
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

#ifndef _B2C_CODE_INCL_
#include "code.h"
#endif

#define CKP( STR ) fprintf(stderr, "Checkpoint in %s at %d: ",__FILE__,__LINE__); fprintf(stderr, STR); fputc( '\n', stderr ); fflush(NULL);

static char * rcs_id = "$Id: code.c,v 1.2 2014/04/08 20:26:32 kjenning Exp $";

/* Controlling C (and Asm) output... */
static int zPreamble = 0;   /* flag that C/Asm init declarations have been output. */
static int zEndamble = 0;   /* Flag that C end of array declaration is output*/

char * code_rcs_id( void ) /* report file version for io.o */
{
	return rcs_id;
}
	


static void startComment( void )
{
	char * sComment[] = { "; ", "REM ", "/* ", "", NULL };
	
	fprintf(stdout, "%s", sComment[ gDoAsmOrBasiCT ] );
}


static void endComment( void ) 
{
	char * eComment[] = { "", "", " */", "", NULL };
	
	fprintf(stdout, "%s\n", eComment[ gDoAsmOrBasiCT ] );
}



/* Output a byte (or word) as hex or decimal in a format based on the 
 * type or program source being output.  A value in decimal may require 
 * padding to 3 digits. However, the valid sizes for hex are 2 or 4.  
 * To allow the caller not to make a decision any odd value is turned 
 * even for hex output.
 */
static void decOrHex( unsigned int size,
					  unsigned int val )
{
	if ( !gDoHexOrDec   &&                   /* Size check is valid for Hex output */
	    ( ( size <  1 ) || ( size > 5 ) )  ) /* Maybe 5 could be valid for dec. ?*/
		return;

	
	if ( gDoHexOrDec ) /* Decimal */
	{
		if ( gDoAsmOrBasiCT == 3 ) /* Forcing 0 padded output for Text display */
			size++;
		
		if ( size == 3 ) /* special case output 0 padded for decimal */
			fprintf(stdout, "%03d", val );
		else if ( size == 5 ) /* another special case output 0 padded for decimal */
			fprintf(stdout, "%05d", val );
		else /* otherwise, as is without padding */
			fprintf(stdout, "%d", val );
	}
	else               /* then this must be hex. */
	{
		size = size & 0x06; /* 3 for decimal, 2 or 4 for hex */
		
		if ( gDoAsmOrBasiCT == 2 ) /* C is different */
			fprintf(stdout, "0x%0*x", size, val );
		else                      /* asm and BASIC. */
			fprintf(stdout, "$%0*X", size, val );
	}
}



static void doDataType( void )
{
	char * dtype[]    = { "  .BYTE", "  .DBYTE", "  .WORD", NULL };
			
	switch ( gDoAsmOrBasiCT ) /* Start data with BASIC or ASM directive. */
	{
		case 0: /* Asm  */
			fprintf(stdout, "%s ", dtype[ gDoBDW ] );
			break;
					
		case 1: /* Basic */
			fprintf(stdout, "DATA ");
			break;
					
		case 2: /* C */
				fputc( '\t', stdout );
			break;
		
		case 3: /* Text */
			decOrHex( 4, gAsmOrg );
			fprintf(stdout, ": " );
			break;
	}
}



/* Output a line number if it is needed. 
 * Yes, everything this function needs is global. 
 */
static void doLine( void )
{
	if ( gDoLines && ( gDoAsmOrBasiCT != 2 ) ) /* Output Line number except for C */
	{
		fprintf( stdout, "%d ", gLine );

		gLine += gIncr;
	}
}


/* Force data declaration for C and Assembly if needed.
 */
static void doPreamble( void )
{
	if ( zPreamble )
		return;

	if ( gDoAsmOrBasiCT == 0 ) /* Last chance.  Output ORG for Assembly if needed */
	{
		doLine(); /* (end prior text if needed and) output Line number if needed  */
		fprintf( stdout, "  *=$%04X\n", gAsmOrg);
	}
		
	if ( gDoAsmOrBasiCT == 2 ) /* Last chance.  Output declaration for C if needed */
	{
		doLine(); /*  end prior text if needed and (ouput Line number if needed */
		fprintf( stdout, "unsigned %s cset[] =\n{\n", ( gDoBDW == 0 ) ? "char" : "int" );
	}

	zPreamble++;
}


void doTheEnd( void )
{
	if ( !zPreamble || zEndamble)
		return;
		
	if ( gDoAsmOrBasiCT == 2 ) /* If C, then close the array... */
		fprintf( stdout, "};\n" );
		
	zEndamble++;
}




/* Output one byte representing the bitmap at text patterns.  
 * Monochrome and 4 color (bit pairs) are supported.  
 */
static void charPicture( unsigned char val )
{
	int    bits     = 0;    /* masked, shifted bits to the low bits position. */
	int    pixS     = 0;    /* from gPixelSize */
	int    pixF     = 0 ;   /* from gPixelFill */
	int  * mask     = NULL;
	int  * shift    = NULL;
	int    mask2[]  = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01, 0x00 };
	int    shift2[] = { 7, 6, 5, 4, 3, 2, 1, 0, -1 };
	int    mask4[]  = { 0xC0, 0x30, 0x0C, 0x03, 0x00 };
	int    shift4[] = { 6, 4, 2, 0, -1 };

	
	if (  gPixelType == 4 ) /* Color bits */
	{
		mask  = mask4;
		shift = shift4;
	}
	else /* Monochrome bitmap */
	{
		mask  = mask2;
		shift = shift2;
	}
				
	while ( *shift >= 0 ) /* Negative shift means we have output all the bits */
	{
		bits = ( val & ( *mask ) ) >> *shift; /* Extract bits, shift down to low bits position */
			
		pixS = gPixelSize; /* The number of characters in a "pixel" */
		pixF = gPixelFill; /* The number of times the "graphics" character is duplicated in the "pixel". */
						
		if ( pixS < pixF ) /* The Fill count can't be bigger than the Size */
			pixS = pixF;
							
		while ( pixF ) /* Output pixel up to fill size */
		{
			fprintf(stdout, "%c", gPixels[ bits ] ) ;
			pixS--;
			pixF--;
		}
						
		while ( pixS-- ) /* If pixel width continues, pad spaces */
			fputc( ' ', stdout );
						 				
		mask++;
		shift++;
	} /* Output next bit(s) in this byte */
}					


/* Write the data as code-formatted text.
 * Most of the ugliness occurs here.
 * Shamefully, nearly everything is in global vars. 
 */
int outputData( MyBuf * thisMyBuf ) 
{
	int sizeGroup         = 0;    /* Number of bytes expected in this buffer */
	int sizeLine          = 0;    /* Actual number of bytes possible on this line. */
	int loop              = 0;    /* 0 to line size loop for the individual bytes. */
	int hiB               = 0;    /* hi byte when pairing */
	int loB               = 0;    /* lo byte when pairing */
	int addr              = 0;    /* temporary working address for gAsmOrg */
	unsigned char * index = NULL;

	
	if ( INVALID_THISMYBUF )  /* If argument is invalid then exit */
	{
/* CKP("outputData() abort\n");	
 */
		return -1;
	}

	/* No data in the current buffer means this is the first time here, or 
	 * this is the last time here/no more data left in the file.
	 */
	if ( !thisMyBuf->len )
	{
		if ( !zPreamble ) /* The first time here... */
		{
			doPreamble(); /* Write C/Asm start of file declarations */
			return 0;
		}
		
		if ( !zEndamble ) /* The last time here... */
		{
			doTheEnd();   /* Write C end of array */
			return 0;
		}
		
		return -1; /* been here before.  Need not come back again. */
	}

	if ( thisMyBuf->len != gGroupSize )  /* Figure out how much data we can output here. */
		sizeGroup = thisMyBuf->len;      /* This should mean the buffer is shorter than the desired size of the group */
	else
		sizeGroup = gGroupSize;          /* Buffer is correct length.  Start with expected group size */
	
	addr  = gAsmOrg;
	index = thisMyBuf->buf;
	
	while ( sizeGroup )
	{
		if ( sizeGroup < gLineSize ) /* Figure how much data really fits on this line */
			sizeLine = sizeGroup;    /* This should mean we're on the (last) line shorter than expected */ 
		else
			sizeLine = gLineSize;    /* This Line is what we expect */
	
		doLine();     /* Line number printed if needed. */
		doDataType(); /* Data declaration printed. */

		for ( loop = 0; loop < sizeLine; loop++, sizeGroup--, addr++)  /* Output the data bytes */
		{
			loB = (int)*(index + loop); /* Get a (low) byte */
				
			switch ( gDoBDW ) /* How is output data grouped? bytes, or ints?*/
			{
				case 0: /* Byte */
					decOrHex( 2, loB);
					break;

				case 1: /* dbyte, low-endian*/
					hiB = loB; /* The low byte is really the high byte */
					loop++; sizeGroup--; addr++;
					loB = (int)*(index + loop);
						
					decOrHex( 4, ( hiB * 256 ) + loB );
					break;

				case 2: /* word, high-endian */
					loop++; sizeGroup--; addr++;
					hiB = (int)*(index + loop);
						
					decOrHex( 4, ( hiB * 256 ) + loB );
			} /* Switch output data grouping */

			if ( loop < ( sizeLine - 1 ) ) /* Comma out after the first X - 1 values */
			{
				if ( gDoAsmOrBasiCT == 3 ) 
					fputc( ' ', stdout );  /* Text gets a blank separator. */
				else
					fputc( ',', stdout );  /* Others get a comma */
			}
			else if ( ( gDoAsmOrBasiCT == 2 ) && /* If this is C format ... and */   
					  ( ( sizeGroup > 1 )     || /* more data follows in this group, or */
					    thisMyBuf->nlen ) )      /* more data follows in this file. */
				fputc( ',', stdout );
		} /* End loop next byte in this line */
				
		if ( gBitsComment && gMerge)
		{
			if ( gDoAsmOrBasiCT != 3 ) 
				fputc( ' ', stdout );    /* Non-text gets a blank */ 
			else 
				fprintf( stdout, ": " ); /* Text gets a colon+blank */ 
			
			startComment();
			for ( loop = 0; loop < sizeLine; loop++ )  /* Output the data bytes */
			{
				loB = (int)*(index + loop); /* Get a byte */
	
				charPicture( loB );
			}
			endComment();
		}
		else
			fputc( '\n', stdout );
		
		index   += sizeLine;  /* Move base pointer to the start of the next line */
		gAsmOrg += sizeLine ; /* Move assembly address same disatance */
	} /* End while more bytes in this group. */

	/* This space intentionally a waste of good bits due to ill-conceived design.
	 * Do it all again, but this time just output bits as pictures.
	*/ 
	if ( gBitsComment && !gMerge )
	{
		if ( thisMyBuf->len != gGroupSize )  /* Figure out how much data we can output here. */
			sizeGroup = thisMyBuf->len;      /* This should mean the buffer is shorter than the desired size of the group */
		else
			sizeGroup = gGroupSize;          /* Buffer is correct length.  Start with expected group size */
	
		index = thisMyBuf->buf;

		while ( sizeGroup )
		{
			if ( sizeGroup < gPixelLine ) /* Figure how much data really fits on this line */
				sizeLine = sizeGroup;    /* This should mean we're on the (last) line shorter than expected */ 
			else
				sizeLine = gPixelLine;    /* This Line is what we expect */

			if ( gDoAsmOrBasiCT == 2 ) /* C gets some tabbing */
				fprintf( stdout, "\t\t\t\t\t" );
				
			doLine();     /* Line number printed if needed. */				
			startComment();
			for ( loop = 0; loop < sizeLine; loop++, sizeGroup-- )  /* Output the data bytes */
			{
				loB = (int)*(index + loop); /* Get a byte */
	
				charPicture( loB );
			}
			endComment();
		
			index += sizeLine; /* Move base pointer to the start of the next line */
		} /* End while more bytes in this group. */
	}
	
	return 0;
}

