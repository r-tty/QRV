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

/*
 * Macros shared between kernel/proc/kdebug.
 */

#define procmgr_prp		procnto_prp
#define sysmgr_prp		procnto_prp
#define pathmgr_prp		procnto_prp

/*
 * Consistency check number between procnto and modules
 *
 * old values:
 * #define LIBMOD_VERSION_CHECK	42	// trinity rc5
 * #define LIBMOD_VERSION_CHECK	43	// trinity rc6
 * #define LIBMOD_VERSION_CHECK	44	// trinity 2/pre-gcc
 */
#define LIBMOD_VERSION_CHECK	45  // trinity 2

/* Total number of priorities (including IDLE which users can not use) */
#ifndef NUM_PRI
#define NUM_PRI			256
#endif

#define _NTO_SF_NOJOBCTL	0x00000001

#define COF_NETCON		0x0001
#define COF_VCONNECT		0x0002
#define COF_ASYNC		0x0400
#define COF_ASYNC_QUEUED	0x4000

/*
 * Define the private flags of a thread tThread.flags
 * The low order 16 bits are reserved to trigger special actions when
 * we return from the kernel to a thread. Actions are performed low
 * bit first. It is important that the relative placement of QRV_FLG_THR_SIG_ACTIVE,
 * QRV_FLG_THR_SIGSUSPEND, QRV_FLG_THR_ACQUIRE_MUTEX  not be changed.
 *
 * Anything that is higher than QRV_FLG_THR_RCVINFO and QRV_FLG_THR_SHORT_MSG must
 * check and make sure there is not a Receiver thread that is ready, but
 * not finished running nano_specret()
 *
 * @@@ For now make sure public and private bits don't conflict
 */
#define QRV_FLG_THR_SPECRET_MASK	0x0000ffff

#define QRV_FLG_THR_KILLSELF	0x00000001
#define QRV_FLG_THR_TO_BE_STOPPED	0x00000002
#define QRV_FLG_THR_RCVINFO		0x00000004
#define QRV_FLG_THR_SHORT_MSG	0x00000008
#define QRV_FLG_THR_NANOSLEEP	0x00000010
#define QRV_FLG_THR_JOIN		0x00000020
#define QRV_FLG_THR_WAAA		0x00000040
#define QRV_FLG_THR_SIGWAITINFO	0x00000080
#define QRV_FLG_THR_SIG_ACTIVE	0x00000100
#define QRV_FLG_THR_SIGSUSPEND	0x00000200
#define QRV_FLG_THR_ACQUIRE_MUTEX	0x00000400
#define QRV_FLG_THR_CANCELSELF	0x00000800
#define QRV_FLG_THR_PULSE		0x00001000
#define QRV_FLG_THR_MUTEX_CEILING	0x00002000
#define QRV_FLG_THR_ASYNC_RECEIVE	0x00004000

//public QRV_FLG_THR_INTR_PENDING   0x00010000
//public QRV_FLG_THR_DETACHED       0x00020000
//unused                        0x00040000
#define QRV_FLG_THR_KCALL_ACTIVE	0x00080000
//public QRV_FLG_THR_THREADS_HOLD   0x00100000
#define QRV_FLG_THR_ONLYME		 	0x00200000
//public QRV_FLG_THR_UNBLOCK_REQ    0x00400000
#define QRV_FLG_THR_KERERR_SET		0x00800000
//public QRV_FLG_THR_ALIGN_FAULT    0x01000000
//public QRV_FLG_THR_SSTEP          0x02000000
//public QRV_FLG_THR_ALLOCED_STACK  0x04000000
//public QRV_FLG_THR_NOMULTISIG     0x08000000
//public QRV_FLG_THR_FROZEN         0x10000000
#define QRV_FLG_THR_V86				0x20000000
#define QRV_FLG_THR_BUFF_MSG		0x40000000
//public QRV_FLG_THR_IOPRIV         0x80000000
#define QRV_FLG_THR_PUBLIC_MASK		(QRV_FLG_THR_INTR_PENDING | \
								QRV_FLG_THR_DETACHED | \
								QRV_FLG_THR_THREADS_HOLD | \
								QRV_FLG_THR_UNBLOCK_REQ | \
								QRV_FLG_THR_ALIGN_FAULT | \
								QRV_FLG_THR_SSTEP | \
								QRV_FLG_THR_ALLOCED_STACK | \
								QRV_FLG_THR_NOMULTISIG | \
								QRV_FLG_THR_FROZEN | \
								QRV_FLG_THR_IOPRIV)

/* Macros to extract signal, code and fault from sigcode generated for an excption */
#define SIGCODE_SIGNO(sigcode)		((sigcode) & 0xff)
#define SIGCODE_CODE(sigcode)		(((sigcode) >> 8) & 0xff)
#define SIGCODE_FAULT(sigcode)		(((sigcode) >> 16) & 0xff)
#define MAKE_SIGCODE(signo,code,fault) ((signo) | ((code)<<8) | ((fault)<<16))
#define SIGCODE_FLAGS_MASK			0xff000000
#define SIGCODE_FATAL				0x01000000
#define SIGCODE_INXFER				0x01000000  /* for use in fault handling */
#define SIGCODE_INTR				0x02000000
#define SIGCODE_KEREXIT				0x02000000  /* for use in fault handling */
#define SIGCODE_KERNEL				0x04000000
#define SIGCODE_USER				0x08000000
#define SIGCODE_PROC				0x10000000
#define SIGCODE_BDSLOT				0x20000000
#define SIGCODE_STORE				0x80000000
#define SIGCODE_SSTEP				0x80000000  /* for use with F.P. emulator */

/* flags for soul entries */
#define	SOUL_CRITICAL				0x01
#define	SOUL_NOFREE				0x02
#define	SOUL_PROC				0x04

/* flags for memmgr.vaddrinfo() */
#define VI_NORMAL				0x00
#define VI_PGTBL				0x01
#define VI_INIT					0x02
#define VI_KDEBUG				0x04

/*
 * These macros allow to process a singly linked list, keeping track
 * of the previous element. Thus a new member can be unlinked or added easily.
 * The macros make the code easier to read.
 */

#define LINK1_BEG(_queue, _object, _type) {\
		(_object)->next = (_queue);\
		(_queue) = (_object);\
		}

#define LINK1_REM(_queue, _object, _type) {\
		_type **_prev, *_cur;\
		for(_prev = &(_queue); (_cur = *_prev); _prev = &_cur->next) {\
			if(_cur == _object) {\
				*_prev = _cur->next;\
				break;\
				}\
			}\
		}

/*
 * These macros allow to process a doubly linked list, keeping track
 * of the previous element. Thus a new member can be unlinked or added easily.
 */
#define LINK2_BEG(_queue, _object, _type) {\
		if(((_object)->next = (_queue)))\
			(_queue)->prev = &(_object)->next;\
		(_object)->prev = &(_queue);\
		(_queue) = (_object);\
		}

#define LINK2_REM(_object) {\
		if((*((_object)->prev) = (_object)->next))\
			(_object)->next->prev = (_object)->prev;\
		}


/*
 * This variant of the LINK2 macros make use of a listhdr structure
 * (defined in objects.h) that enables fast insertion at the end
 * of the list (the listhdr has head/tail pointers). These macros
 * are used in nano_sched.c with the dispatch table.  The list is
 * still just a doubly linked list but the listhdr structure has
 * head/tail pointers.
 */
#define LINK3_BEG(_queue, _object, _type) {\
	if((((LINK3_NODE *)(_object))->next = (_queue).head))\
			(_queue).head->prev = &((LINK3_NODE *)(_object))->next;\
        else \
            (_queue).tail = (LINK3_NODE *)(_object); \
		((LINK3_NODE *)(_object))->prev = &(_queue).head;\
		(_queue).head = (LINK3_NODE *)(_object);\
	}

#define LINK3_END(_queue, _object, _type) {\
        if ((_queue).tail) { \
            ((LINK3_NODE *)(_object))->prev = &(_queue).tail->next;\
            (_queue).tail->next = (LINK3_NODE *)(_object);\
        } else {\
            ((LINK3_NODE *)(_object))->prev = &(_queue).head;\
            (_queue).head = (LINK3_NODE *)(_object);\
        }\
        (_queue).tail = (LINK3_NODE *)(_object);\
        ((LINK3_NODE *)(_object))->next = NULL;\
    }

#define LINK3_REM(_queue, _object, _type) {\
        if((*(((LINK3_NODE *)(_object))->prev) = ((LINK3_NODE*)(_object))->next)) { \
            ((LINK3_NODE *)(_object))->next->prev = ((LINK3_NODE *)(_object))->prev;\
        }\
        if ((_queue).tail == (LINK3_NODE *)(_object)) {\
            if ((_queue).head == NULL) { \
                (_queue).tail = NULL; \
            } else {\
                (_queue).tail = (LINK3_NODE *)((LINK3_NODE *)(_object))->prev;\
            }\
        }\
    }

// The LINK4_xxx macros are LINK3_xxx macros with the addition of a 'count'
// field in the header. The only reason that these macros were created were to
// preserve the LINK3_xx implementation since although the inc/dec operation is
// likely a relatively small impact, the LINK3_xxx macros are used in the
// scheduler, a sensitive part of the kernel

#define LINK4_BEG(_queue, _object, _type) {\
		LINK3_BEG(_queue, _object, _type);\
		++(_queue).count;\
	}

#define LINK4_END(_queue, _object, _type) {\
		LINK3_END(_queue, _object, _type);\
	    ++(_queue).count;\
    }

#define LINK4_REM(_queue, _object, _type) {\
		if (!(((_object)->next == NULL) && ((_object)->prev == NULL))) { \
			LINK3_REM(_queue, _object, _type);\
			(_object)->next = NULL, (_object)->prev = NULL; \
			--(_queue).count;\
		}\
    }

#define LINK4_INIT(_queue) \
		((_queue).count = ((unsigned int)((_queue).head = (_queue).tail = NULL)))

/*
 * General utility macros
 */
#define NUM_ELTS(array)	(sizeof(array) / sizeof(array[0]))

/* Rounding down and up */
#define ROUNDDOWN(val, round)	(((val)) & ~((round)-1))
#define ROUNDUP(val, round)	ROUNDDOWN((val) + ((round)-1), round)

/* Same as above, but arguments typecasted to paddr_t */
#define TRUNC(_x,_a)  ((paddr_t)(_x) & ~(((paddr_t)(_a))-1))
#define ROUND(_x,_a)  TRUNC(((paddr_t)(_x)) + ((paddr_t)(_a)-1), (paddr_t)(_a))

/* Same as above, but truncating/rounding the address on the page boundary */
#define TRUNCPG(_x)	TRUNC((_x), __PAGESIZE)
#define ROUNDPG(_x)	ROUND((_x), __PAGESIZE)

#define TOPTR(x)		((void *)(x))

#define PTR_DIFF(a,b)	((uint8_t *)(a) - (uint8_t *)(b))

#define KILO(k)		((k) * 1024UL)
#define MEG(m)		((m) * (1024UL * 1024UL))
#define GIG(m)		((m) * (1024UL * 1024UL * 1024UL))

#ifdef NDEBUG
#define CRASHCHECK(e)
#elif defined(_lint)
#include <stdlib.h>
#define CRASHCHECK(e)	do{if(e)exit(-1);}while(0)
#else
#define CRASHCHECK(e)	if(e) crash()
#endif

/* Macros for setting/resetting/testing for QRV_GLOBAL_CHANNEL */
#define SET_GLOBAL_CHAN(var) (var) |= (int)QRV_GLOBAL_CHANNEL
#define CLEAR_GLOBAL_CHAN(var) (var) &= (int)(~QRV_GLOBAL_CHANNEL)
#define IS_GLOBAL_CHAN_SET(var) (var) & (int)QRV_GLOBAL_CHANNEL
