/*
 * $QNXLicenseC:
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




#include <time.h>
#include <sys/neutrino.h>

int clock_getres(clockid_t clock_id, struct timespec *res) {
	struct _clockperiod				clockl;

	if(ClockPeriod(clock_id, 0, &clockl, 0) == -1)
		return -1;

	nsec2timespec(res, clockl.nsec);
	return 0;
}

__SRCVERSION("clock_getres.c $Rev: 153052 $");
