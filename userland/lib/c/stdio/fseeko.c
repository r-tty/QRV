/*
 * $QNXtpLicenseC:
 * Copyright 2007, QNX Software Systems. All Rights Reserved.
 *
 * You must obtain a written license from and pay applicable license fees to QNX
 * Software Systems before you may reproduce, modify or distribute this software,
 * or any work that includes all or part of this software.   Free development
 * licenses are available for evaluation and non-commercial purposes.  For more
 * information visit http://licensing.qnx.com or email licensing@qnx.com.
 *
 * This file may contain contributions from others.  Please review this entire
 * file for other proprietary rights or license notices, as well as the QNX
 * Development Suite License Guide at http://licensing.qnx.com/license-guide/
 * for other information.
 * $
 */



/*




Also copyright P.J. Plauger - see bottom of file for details.
*/

/* fseeko function */
#include "xstdio.h"
_STD_BEGIN

int (fseeko)(FILE *str, off_t off, int smode)
	{	/* set seek offset for stream */
	int ans;

	_Lockfileatomic(str);
	ans = _Fspos(str, 0, off, smode);
	_Unlockfileatomic(str);
	return (ans);
	}
_STD_END

/*
 * Copyright (c) 1992-2006 by P.J. Plauger.  ALL RIGHTS RESERVED.
 * Consult your license regarding permissions and restrictions.
V5.00:1296 */

__SRCVERSION("fseeko.c $Rev: 153052 $");
