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




#include <unistd.h>
#include <errno.h>
#include <sys/iofunc.h>

int iofunc_read_default(resmgr_context_t *ctp, io_read_t *msg, iofunc_ocb_t *ocb) {
	int					status;

	// Check we have the correct perms
	if((status = iofunc_read_verify(ctp, msg, ocb, 0)) != EOK) {
		return status;
	}

	switch(msg->i.xtype & _IO_XTYPE_MASK) {
	case _IO_XTYPE_NONE:
		break;
	default:
		// We don't handle this xtype, return error
		return ENOSYS;
	}

	// Return nbytes as zero
	_IO_SET_READ_NBYTES(ctp, 0);
	return EOK;
}

__SRCVERSION("iofunc_read_default.c $Rev: 153052 $");
