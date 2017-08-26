/* Dump binary data as program text either for BASIC or assembly or C or text dump
 */

/* $Id: code.h,v 1.1 2014/04/08 17:25:50 kjenning Exp $
 *
 * $Log: code.h,v $
 * Revision 1.1  2014/04/08 17:25:50  kjenning
 * Initial revision
 *
 *
 */

#ifndef _B2C_CODE_INCL_
#define _B2C_CODE_INCL_

#ifndef _B2C_IO_INCL_
#include "io.h"
#endif

char * code_rcs_id( void );

int outputData( MyBuf * thisMyBuf ); 

#endif /* _B2C_CODE_INCL_ */


