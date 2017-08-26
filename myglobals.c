/* Dump binary data as program text either for BASIC or assembly or C or text dump.
 */

/* $Id: myglobals.c,v 1.1 2014/04/08 17:25:50 kjenning Exp $
 *
 * $Log: myglobals.c,v $
 * Revision 1.1  2014/04/08 17:25:50  kjenning
 * Initial revision
 *
 */

#ifndef _GLOBALS_INCL_
#include "globals.h"
#endif

#define CKP( STR ) fprintf(stderr, "Checkpoint in %s at %d: ",__FILE__,__LINE__); fprintf(stderr, STR); fputc( '\n', stderr ); fflush(NULL);


static char * rcs_id = "$Id: myglobals.c,v 1.1 2014/04/08 17:25:50 kjenning Exp $";

char * myglobal_rcs_id( void ) /* report file version for myglobals.o */
{
	return rcs_id;
}


/* Program globals...  mostly from command line. 
 */
int gDoDebug       = 0;       /* Turn on debug to stderr. 0 == off.  1 == on*/

int gDoLines       = 1;       /* include line numbers or not. 1 == lines */
int gLine          = 1000;    /* start at line number */
int gIncr          = 10;      /* increment line numbers */

int gDoAsmOrBasiCT = 0;       /* 0 == asm, 1 == BASIC, 2 == C, 3 == Text Dump */
int gAsmOrg        = 0x0000;  /* Starting assembly address.  default ($0000). */
int gDoHexOrDec    = 0;       /* 0 == hex, 1 == dec */
int gDoBDW         = 0;       /* 0 == byte, 1 == dbyte, 2 == word */

int gGroupSize     = 8;       /* Number of bytes in a group (default 1) */
int gLineSize      = 8;       /* Number of bytes per line (default 1) */

int gBitsComment   = 0;       /* Comment block for the bitmap? 0 == off. 1 == on */
int gMerge         = 0;       /* If all other variables agree, put the bits comment on the same line as data */

int gPixelLine     = 1;       /* Number of bytes to output in one line of the bitmap comment. */
int gPixelType     = 2;       /* Bitmap interpretation.  2 (2 color) or 4  (4 color ) */
int gPixelSize     = 2;       /* Bitmap Field width (chars output) PER Bit (2) */
int gPixelFill     = 1;       /* Number of times to output gPixel inside of PixelSize */
char gPixels[4]    = { '.', '#', 'X', 'O' }; /* Chars to represent bits or pairs 0, 1, 2, 3 */

char * gFileName   = NULL;    /* Name of file to read */
char * gStdin      = "stdin"; /* fallback if no filename provided */


/* A little bit of Tedium.  Documentation arrays must be declared. 
 * But at least this is better than hand editing a giant printf()
 * in the usage() every time a new option is added.
 */
static char * gdFile[] = { "filename",
						   "Providing filename 'stdin' (default) will use stdio instead.",
						   NULL };
static char * gdLines[] = { "",
						    "Include line numbers. (default for BASIC and Assembly). The C format always always overrides this to turn off line numbers.",
						    NULL };
static char * gdNoLines[] = { "",
						      "Do not include line numbers. (default for text dump).  And C format always turns off line numbers.",
						      NULL };	
static char * gdStart[] = { "num",
						    "Starting line number less than 32000 (default 1000).",
						    NULL };	
static char * gdInc[] = { "num",
						  "Line number increment. (default 10).",
						  NULL };	
static char * gdAsm[] = { "",
						  "Output for Assembly. (default) Example output using Line Size 1 "
						  "and Pixel Line size 1 and the bits comment merged with the data lines:\n"
						  "1000  *=$0000\n"
						  "1010  .BYTE 00 ; . . . . . . . . \n"
						  "1020  .BYTE 18 ; . . . # # . . . \n"
						  "1030  .BYTE 3c ; . . # # # # . . \n"
						  "1040  .BYTE 7e ; . # # # # # # . ",
						  NULL };	
static char * gdBasic[] = { "",
						    "Output for BASIC using DATA statements.  If the bitmap comment is included "
							"it always appears on the following lines.  This example uses Line Size 4 "
							"and Pixel Line size 1:\n"
						    "1000 DATA 0,24,60,126\n"
							"1010 REM . . . . . . . . \n"
							"1020 REM . . . # # . . . \n"
							"1030 REM . . # # # # . . \n"
							"1040 REM . # # # # # # . ",
						    NULL };							
static char * gdCC[] = { "",
					     "Output for C as an unsigned char array. Line numbers are always disabled. "
						 "Example output using Group Size 2, Line Size 2, and Pixel Line size 2 and "
						 "the bits comment merged with the data lines:\n"
						 "unsigned char bytes[] =\n"
						 "{\n"
						 "    0x00,0x18, /* . . . . . . . . . . . # # . . . */\n"
						 "    0x3c,0x7e  /* . . # # # # . . . # # # # # # . */",
					     "};\n",
					     NULL };	
static char * gdText[] = { "",
					       "Output as a text dump showing address, value. An address is assumed, starting at 0, for "
						   "the bytes.  The example output uses line numbers, hex format for the address "
						   "and byte values, and the bits comment merged with the data lines.  Group and Line Size are 1:\n"
						   "1000 0000: 00: . . . . . . . . \n"
						   "1010 0001: 18: . . . # # . . . \n"
						   "1020 0002: 3c: . . # # # # . . \n"
						   "1030 0003: 7e: . # # # # # # . \n"
						   "Example output of the address and values in decimal format. "
						   "Group, Line Size, and Pixel Line size are 2:\n"
						   "1000 00000: 000 024: . . . . . . . . . . . # # . . . \n"
						   "1010 00001: 060 126: . . # # # # . . . # # # # # # . ",
					       NULL };	
static char * gdAsmOrg[] = { "num",
							 "Starting address for Assembly output. (default $0000)",
							 NULL };								
static char * gdDec[] = { "",
					      "Output data using decimal format.",
					      NULL };	
static char * gdHex[] = { "",
					      "Output data using hex values. ($00 or $0000) (default)",
					      NULL };	
static char * gdByte[] = { "",
					       "Output bytes as single byte values. (default)",
					       NULL };	
static char * gdDbyte[] = { "",
					        "Outputs pairs of bytes as word values low-endian. ($1234 == $12 $34)"
							"Note that this does not affect the bits comment which is always output "
							"in byte order.",
					        NULL };	
static char * gdWord[] = { "",
					       "Outputs pairs of bytes as word values high-endian. ($1234 == $34 $12)",
					       NULL };	
static char * gdGSize[] = { "",
					        "Defines the number of bytes output in a group of successive lines before a "
							"comment line will be output (default 1).  This must be greater than or equal "
							"to the line size, but need not be an exact multiple of line size.  If this "
							"value does not agree with the Word/DByte options, then the group size will "
							"be incremented by 1 byte. ",
					        NULL };
static char * gdLSize[] = { "",
					        "Defines the number of bytes output per line. (default 1). Maximum value 64.  If this does not agree "
							"with the Word/DByte options, then the line size will be incremented by 1 byte. ",
					        NULL };			
static char * gdPxLine[] = { "",
					         "Defines the number of bytes output per line in the bits comments. (default 1). Maximum value 64.",
					         NULL };									
static char * gdBCom[] = { "",
						   "After the block of character bytes output a series of lines of text "
						   "representing the character bitmap as it would be rendered according to the "
						   "\"pixeltype\" value using the \"pixel\" character values to represent the "
						   "font pixels, according to the \"pixelsize\".\n"
						   "Example shown for 8 bytes as Pixel Type 2 in C format for Hex data:\n"
						   "/* 0x00:  . . . . . . . .  */\n"
						   "/* 0x18:  . . . # # . . .  */\n"
						   "/* 0x3c:  . . # # # # . .  */\n"
						   "/* 0x7e:  . # # # # # # .  */\n"
						   "/* 0x7e:  . # # # # # # .  */\n"
						   "/* 0x18:  . . . # # . . .  */\n"
						   "/* 0x3c:  . . # # # # . .  */\n"
						   "/* 0x00:  . . . . . . . .  */",
						   NULL };
static char * gdNoBCom[] = { "",
						     "Do not output the character bitmap comments. (default)",
						     NULL };
static char * gdMerge[] = { "",
						    "If all other variables agree then merge the bits comment with the same line as the data. "
							"The length of the output is not checked, so it is possible to output lines for Assembly "
							"that are too long to be entered as program text.  Merge works when the output format is "
							"NOT BASIC, the Group Size is a multiple of Line Size, and Line Size and Pixel Line size are "
							"the same value.  If any of those parameters are violated then merge is silently disabled. "
							"If the actual data does not agree with the various Size specifications, then the last line "
							"is still output with less data.",
						   NULL };
static char * gdNoMerge[] = { "",
						      "Do not attempt to merge the bits comment with the data. (default)",
						      NULL };
static char * gdPxType[] = { "num",
						     "Specify the pixel rendering type when the bitmap comment is produced. The value is 2 for "
						     "2-color bit interpretation (1 bit per pixel) , and 4 for 4-color bit interpretation (2 bits "
							 "per pixel).  (default is 2)",
						     NULL };
static char * gdPxSize[] = { "num",
						     "Specifies how many characters wide a \"pixel\" is when the bitmap comment is produced. "
						     "Valid values are 1 through 4. (default 2)",
						     NULL };
static char * gdPxFill[] = { "num",
						     "Specifies how many times the character(s) representing a \"pixel\" will be written out within "
						     "the pixel field (specified by \"pixelsize\") when the bitmap comment is produced.  If the size "
						     " of \"pixelfill\" is less than \"pixelsize\" then the difference will be made up by blank "
						     "spaces. Valid values are 1 through 4. (default 1)",
						     NULL };
static char * gdPixel0[] = { "char(s)",
						     "Specifies the character to use to represent pixel value 0 when the bitmap comment is produced. "
						     "Note that this and all other \"pixel\" values can be a string of multiple characters which can "
						     "be used to simulate screen aspect ratios when data represents actual graphics. (default \".\")",
						     NULL };
static char * gdPixel1[] = { "char(s)",
						     "Specifies the character to use to represent pixel value 1 when the bitmap comment is produced. "
						     "(default \"#\")",
						     NULL };
static char * gdPixel2[] = { "char(s)",
						     "Specifies the character to use to represent pixel value 2 when the bitmap comment is produced. "
						     "(default \"O\")",
						     NULL };
static char * gdPixel3[] = { "char(s)",
						     "Specifies the character to use to represent pixel value 3 when the bitmap comment is produced. "
						     "(default \"X\")",
						     NULL };

						 
/* Though individual declaration of each struct is tedious, this
 * allows the remaining code to use a direct name instead of
 * a subscript in the GlobalArgs array, and so, subscript references
 * do not need to be revised every time a new command is inserted
 * in the array.
 *
 * Debug is "secret".  Shhhhhhh.  The doc entry is NULL, so it is
 * never described in the usage.
 */
static GlobalArg gaNoDebug = { "nodebug",       NULL,      0, NULL, GA_IFLAG,  0, 0,     (void *)&gDoDebug,       0, "0",       NULL,       NULL                };
static GlobalArg gaDebug   = { "debug",         NULL,      0, NULL, GA_IFLAG,  1, 1,     (void *)&gDoDebug,       1, "1",       &gaNoDebug, NULL                }; 

static GlobalArg gaFile    = { "file",          gdFile,    1, NULL, GA_STRING, 0, 0,     (void *)&gFileName,      0, "stdin",   NULL,       NULL                };
 
static GlobalArg gaLines   = { "linenum",       gdLines,   0, NULL, GA_IFLAG,  1, 1,     (void *)&gDoLines,       0, "1",       NULL,       NULL                };
static GlobalArg gaNoLines = { "nolinenum",     gdNoLines, 0, NULL, GA_IFLAG,  0, 0,     (void *)&gDoLines,       1, "0",       &gaLines,   NULL                }; 

static GlobalArg gaStart   = { "start",         gdStart,   1, NULL, GA_INT,    1, 32000, (void *)&gLine,          0, "1000",    NULL,       NULL                };

static GlobalArg gaInc     = { "inc",           gdInc,     1, NULL, GA_INT,    1, 1000,  (void *)&gIncr,          0, "10",      NULL,       NULL                };

static GlobalArg gaAsm     = { "asm",           gdAsm,     0, NULL, GA_IFLAG,  0, 0,     (void *)&gDoAsmOrBasiCT, 0, "0",       NULL,       NULL                };
static GlobalArg gaBasic   = { "basic",         gdBasic,   0, NULL, GA_IFLAG,  1, 1,     (void *)&gDoAsmOrBasiCT, 1, "1",       &gaAsm,     NULL                };
static GlobalArg gaCC      = { "cc",            gdCC,      0, NULL, GA_IFLAG,  2, 2,     (void *)&gDoAsmOrBasiCT, 1, "2",       &gaAsm,     NULL                };
static GlobalArg gaText    = { "text",          gdText,    0, NULL, GA_IFLAG,  3, 3,     (void *)&gDoAsmOrBasiCT, 1, "3",       &gaAsm,     NULL                };

static GlobalArg gaAsmOrg  = { "org",           gdAsmOrg,  1, NULL, GA_INT,    1, 65534, (void *)&gAsmOrg,        0, "0x0000",  NULL,       NULL                };

static GlobalArg gaHex     = { "hex",           gdHex,     0, NULL, GA_IFLAG,  0, 0,     (void *)&gDoHexOrDec,    0, "0",       NULL,       NULL                };
static GlobalArg gaDec     = { "dec",           gdDec,     0, NULL, GA_IFLAG,  1, 1,     (void *)&gDoHexOrDec,    1, "1",       &gaHex,     NULL                };

static GlobalArg gaByte    = { "byte",          gdByte,    0, NULL, GA_IFLAG,  0, 0,     (void *)&gDoBDW,         0, "0",       NULL,       NULL                };
static GlobalArg gaDbyte   = { "dbyte",         gdDbyte,   0, NULL, GA_IFLAG,  1, 1,     (void *)&gDoBDW,         1, "1",       &gaByte,    NULL                };
static GlobalArg gaWord    = { "word",          gdWord,    0, NULL, GA_IFLAG,  2, 2,     (void *)&gDoBDW,         1, "2",       &gaByte,    NULL                };

static GlobalArg gaGSize   = { "groupsize",     gdGSize,   1, NULL, GA_INT,    1, 65536, (void *)&gGroupSize,     0, "1",       NULL,       NULL                };

static GlobalArg gaLSize   = { "linesize",      gdLSize,   1, NULL, GA_INT,    1, 64,    (void *)&gLineSize,      0, "1",       NULL,       NULL                };

static GlobalArg gaNoBCom  = { "nobitscomment", gdNoBCom,  0, NULL, GA_IFLAG,  0, 0,     (void *)&gBitsComment,   0, "0",       NULL,       NULL                }; 
static GlobalArg gaBCom    = { "bitscomment",   gdBCom,    0, NULL, GA_IFLAG,  1, 1,     (void *)&gBitsComment,   1, "1",       &gaNoBCom,  NULL                };

static GlobalArg gaNoMerge = { "nomerge",       gdNoMerge,  0, NULL, GA_IFLAG,  0, 0,     (void *)&gMerge,        0, "0",       NULL,       NULL                }; 
static GlobalArg gaMerge   = { "merge",         gdMerge,    0, NULL, GA_IFLAG,  1, 1,     (void *)&gMerge,        1, "1",       &gaNoMerge, NULL                };

static GlobalArg gaPxLine  = { "pixelline",     gdPxLine,  1, NULL, GA_INT,    1, 64,    (void *)&gPixelLine,     0, "1",       NULL,       NULL                };
static GlobalArg gaPxType  = { "pixeltype",     gdPxType,  1, NULL, GA_INT,    1, 16,    (void *)&gPixelType,     0, "2",       NULL,       NULL                };
static GlobalArg gaPxSize  = { "pixelsize",     gdPxSize,  1, NULL, GA_INT,    1, 4,     (void *)&gPixelSize,     0, "2",       NULL,       NULL                };
static GlobalArg gaPxFill  = { "pixelfill",     gdPxFill,  1, NULL, GA_INT,    1, 4,     (void *)&gPixelFill,     0, "1",       NULL,       NULL                };

static GlobalArg gaPixel0  = { "pixel0",        gdPixel0,  1, NULL, GA_CHAR,   0, 0,     (void *)&gPixels[0],     0, ".",       NULL,       NULL                };
static GlobalArg gaPixel1  = { "pixel1",        gdPixel1,  1, NULL, GA_CHAR,   0, 0,     (void *)&gPixels[1],     0, "#",       NULL,       NULL                };
static GlobalArg gaPixel2  = { "pixel2",        gdPixel2,  1, NULL, GA_CHAR,   0, 0,     (void *)&gPixels[2],     0, "O",       NULL,       NULL                };
static GlobalArg gaPixel3  = { "pixel3",        gdPixel3,  1, NULL, GA_CHAR,   0, 0,     (void *)&gPixels[3],     0, "X",       NULL,       NULL                };


/* Control structure for command line entries.
 */
static GlobalArg * GlobalArgs[] = 
{
	&gaDebug,
	&gaNoDebug,
	&gaFile,
	&gaLines,
	&gaNoLines,
	&gaStart,
	&gaInc,
	&gaAsm,
	&gaBasic,
	&gaCC,
	&gaText,
	&gaAsmOrg,
	&gaHex,
	&gaDec,
	&gaByte,
	&gaDbyte,
	&gaWord,
	&gaGSize,
	&gaLSize,
	&gaBCom,
	&gaNoBCom,
	&gaMerge,
	&gaNoMerge,
	&gaPxLine,
	&gaPxType,
	&gaPxSize,
	&gaPxFill,
	&gaPixel0,
	&gaPixel1,
	&gaPixel2,
	&gaPixel3,
	NULL
};



int dumpMyArgs( void ) /* Debugging.  Output the Globals. */
{
	if ( !gDoDebug )
		return 0;
		
	dumpArgs(); /* Generic Globals -- dump array of structures */

	fprintf(stderr," gdump: gDoDebug       = %d\n",    gDoDebug       ); /* debug to stderr. 1 == yes */
	fprintf(stderr," gdump: gDoLines       = %d\n",    gDoLines       ); /* include line numbers or not. 1 == lines */
	fprintf(stderr," gdump: gLine          = %d\n",    gLine          ); /* start at line number */
	fprintf(stderr," gdump: gIncr          = %d\n",    gIncr          ); /* increment line numbers */
	fprintf(stderr," gdump: gDoAsmOrBasiCT = %d\n",    gDoAsmOrBasiCT ); /* 0 == asm, 1 == BASIC, 2 == C, 3 == Text */
	fprintf(stderr," gdump: gAsmOrg        = $%04x\n", gAsmOrg        ); /* Starting assembly address.  default ($0000). */
	fprintf(stderr," gdump: gDoHexOrDec    = %d\n",    gDoHexOrDec    ); /* 0 == hex, 1 == dec */
	fprintf(stderr," gdump: gDoBDW         = %d\n",    gDoBDW         ); /* 0 == byte, 1 == dbyte, 2 == word */
	fprintf(stderr," gdump: gGroupSize     = %d\n",    gGroupSize     ); /* Number of bytes in a group (default 1) */
	fprintf(stderr," gdump: gLineSize      = %d\n",    gLineSize      ); /* Number of bytes per line (default 1) */
	fprintf(stderr," gdump: gBitsComment   = %d\n",    gBitsComment   ); /* Comment block for the bitmap? 0 == off. 1 == on */
	fprintf(stderr," gdump: gMerge         = %d\n",    gMerge         ); /* If all other variables agree, put the bits comment on the same line as data */
	fprintf(stderr," gdump: gPixelLine     = %d\n",    gPixelLine     ); /* Number of bytes to output in one line of the bitmap comment. */
	fprintf(stderr," gdump: gPixelType     = %d\n",    gPixelType     ); /* Bitmap interpretation.  2 (2 color) or 4  (4 color ) */	
	fprintf(stderr," gdump: gPixelSize     = %d\n",    gPixelSize     ); /* Bitmap Field width (chars output) PER Bit (2) */
	fprintf(stderr," gdump: gPixelFill     = %d\n",    gPixelFill     ); /* Number of times to output gPixel inside of PixelSize */
	fprintf(stderr," gdump: gPixels[]      = { '%c', '%c', '%c', '%c' }\n", gPixels[0],gPixels[1],gPixels[2],gPixels[3]); /* which 256 byte pages to output */
	fprintf(stderr," gdump: * gFileName    = %p: \"%s\"\n", gFileName, gFileName ? gFileName : "NULL") ; /* Filename... or stdin */	
	fprintf(stderr," gdump: ** gFileName   = %p \n", &gFileName ); /* Checking what goes in GlobalsArray */
	fprintf(stderr," gdump: * gStdin       = %p: \"%s\"\n", gStdin, gStdin ? gStdin : "NULL") ; 
	fflush(NULL);
	
	return 0;
}



/* Sanity check the pages and sizes. 
 * The number of pages being output cannot be more than the 
 * number of pages available.
 *		 
 * So, If the starting ROM page is not zero, then the number of pages,  
 * and the max page number change.
 * --pages 3,2,1,0 is perfectly legal... but
 * --diff 2 means a 512 byte font and pages 3 and 2 are now illegal.  
 * Also, if the FileSize is 512 bytes a similar situation occurs where
 * only pages 0 and 1 are valid.
 *
 * Note that --pages are always relative to whatever gRom states is 
 * the first page of the font.
 *
 * Rather than error out on every silly combination, the pages list will 
 * be truncated if too long and then the page list will be checked.
 * 
 * Also Sanity check glyph/pixel settings.
 * Glyph type may only be 2 or 4.
 * Pixel fill must be <= Pixel size.
 */
 int sanityCheck( void ) /* All values are global */
 {
	if ( ( gDoBDW == 1 ) || (gDoBDW == 2 ) ) /* Word and Dbyte require 2 bytes */
	{
		if ( gGroupSize & 0x01 ) /* If Group is odd, then */
			gGroupSize++;        /* size up to even. */
		
		if ( gLineSize & 0x01 ) /* If Line is odd, then */
			gLineSize++;        /* size up to even. */

		if ( gPixelLine & 0x01 ) /* If Pixels per line is odd, then */
			gPixelLine++;        /* size up to even. */			
	}
/* CKP( "SanityCheck()" );
 * fprintf(stderr, "1) gMerge = %d, gGroupSize = %d, gLineSize = %d, gPixelLine = %d, gDoAsmOrBasiCT = %d\n", gMerge, gGroupSize, gLineSize, gPixelLine, gDoAsmOrBasiCT );fflush(NULL);
 */
	if ( gMerge &&
	     ( ( gGroupSize != ( ( gGroupSize / gLineSize ) * gLineSize ) ) || /* Group must be a multiple of Line */
		   ( gPixelLine != gLineSize ) ||                                  /* Pixels must be same as line */
		   ( gDoAsmOrBasiCT == 1 ) ) )                                     /* BASIC cannot merge comments */
		gMerge = 0;	
/* fprintf(stderr, "2) gMerge = %d, gGroupSize = %d, gLineSize = %d, gPixelLine = %d, gDoAsmOrBasiCT = %d\n", gMerge, gGroupSize, gLineSize, gPixelLine, gDoAsmOrBasiCT );fflush(NULL);
 */
	if ( gDoLines && ( gDoAsmOrBasiCT == 2 ) ) /* C will not do line numbers */
		gDoLines = 0;

	if ( ( gPixelType != 2 ) && (gPixelType != 4) ) /* must be 2 color or 4 color */
		gPixelType = 2;
		
	if ( gPixelFill > gPixelSize )  /* can't fill more pixels than allowed by the field size */
		gPixelFill = gPixelSize;
			
	return 0;
}



/* Return the Global Args, so it can be passed to doArgs */

GlobalArg ** get_GA( void )
{
	return GlobalArgs;
}
