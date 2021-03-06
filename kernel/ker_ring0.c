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

#include "externs.h"

/**
 * \brief Execute a given function in supervisor level.
 */
int kdecl ker_ring0(tThread * act, struct kerargs_ring0 *kap)
{

    if ((act->process->flags & QRV_FLG_PROC_RING0) == 0) {
        return EPERM;
    }

    (*kap->func) (kap->arg);
    return ENOERROR;
}

/*
 * This is used for async_flags processing in the SMP kernel
 */
int kdecl ker_nop(tThread * act, struct kerargs_null *kap)
{
    return ENOERROR;
}
