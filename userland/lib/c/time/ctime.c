/**
 * \file ctime.h
 *
 * ctime() function
 *
 * Copyright (c) 1992-2006 by P.J. Plauger.  ALL RIGHTS RESERVED.
 * Consult your license regarding permissions and restrictions.
 * V5.00:1296
 */

#include "xtime.h"
_STD_BEGIN

char *(ctime)(const time_t *tod)
	{	/* convert calendar time to local text */
	return (asctime(localtime(tod)));
	}
_STD_END


