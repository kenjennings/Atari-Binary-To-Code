/* Dump binary data as program text either for BASIC or assembly or C or text dump
 */

/* $Id: myglobals.h,v 1.1 2014/04/08 17:25:50 kjenning Exp $
 *
 * $Log: myglobals.h,v $
 * Revision 1.1  2014/04/08 17:25:50  kjenning
 * Initial revision
 *
 *
 */

#ifndef _MY_GLOBALS_INCL_
#define _MY_GLOBALS_INCL_

#ifndef _GLOBALS_INCL_
#include "globals.h"
#endif

/* Program globals...  mostly from command line. 
 */
extern int  gDoLines;       /* include line numbers or not. 1 == lines */
extern int  gLine;          /* start at line number */
extern int  gIncr;          /* increment line numbers */
extern int  gDoAsmOrBasiCT; /* 0 == asm, 1 == BASIC, 2 == C, 3 == Dump */
extern int  gAsmOrg;        /* Starting assembly address.  default (0x0000). */
extern int  gDoHexOrDec;    /* 0 == hex, 1 == dec */
extern int  gDoBDW;         /* 0 == byte, 1 == dbyte, 2 == word */

extern int  gGroupSize;     /* Number of bytes in a group (default 8) */
extern int  gLineSize;      /* Number of bytes per line (default 8) */

extern int  gBitsComment;   /* Comment block for the bitmap? 0 == off. 1 == on */
extern int  gMerge;         /* If all other variables agree, put the bits comment on the same line as data */

extern int  gPixelLine;     /* Number of bytes to output in a line.  Need not match Line Size.*/
extern int  gPixelType;     /* Bitmap interpretation.  2 (2 color) or 4  (4 color ) */
extern int  gPixelSize;     /* Bitmap Field width (chars output) PER Bit (2) */
extern int  gPixelFill;     /* Number of times to output gPixel inside of PixelSize */

extern char gPixels[4];     /* Char to represent bits or pairs 0, 1, 2, 3 */

extern char * gFileName;    /* Required (not really) first argument */
extern char * gStdin;       /* fallback if no filename provided */


char * myglobal_rcs_id( void );

int dumpMyArgs( void );      /* Debugging.  Output these Globals then call the globals library debug */

int sanityCheck( void );     /* Verify some choices are compatible.  All values are global. */

GlobalArg ** get_GA( void ); /* Return the Global Args array, so it can be passed to doArgs */

#endif /* _MY_GLOBALS_INCL_ */
