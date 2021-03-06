/**
 * \file lzo16bit.h
 *
 * Configuration for the strict 16-bit memory model.
 *
 * This file is part of the LZO real-time data compression library.
 *
 * \copyright (C) 1998-99 Markus Franz Xaver Johannes Oberhumer
 *                        <markus.oberhumer@jk.uni-linz.ac.at>
 *                        http://wildsau.idv.uni-linz.ac.at/mfx/lzo.html
 *
 * \license GNU GPL 2.0
 *
 * \note The strict 16-bit memory model is *not* officially supported.
 *       This file is only included for the sake of completeness.
 */

#ifndef __LZOCONF_H
#  include <lzoconf.h>
#endif

#ifndef __LZO16BIT_H
#define __LZO16BIT_H

#if defined(__LZO_STRICT_16BIT)
#if (UINT_MAX < LZO_0xffffffffL)

#ifdef __cplusplus
extern "C" {
#endif


/***********************************************************************
//
************************************************************************/

#ifndef LZO_99_UNSUPPORTED
#define LZO_99_UNSUPPORTED
#endif
#ifndef LZO_999_UNSUPPORTED
#define LZO_999_UNSUPPORTED
#endif

typedef unsigned int        lzo_uint;
typedef int                 lzo_int;
#define LZO_UINT_MAX        UINT_MAX
#define LZO_INT_MAX         INT_MAX

#define lzo_sizeof_dict_t   sizeof(lzo_uint)


/***********************************************************************
//
************************************************************************/

#if defined(__LZO_DOS16) || defined(__LZO_WIN16)

#if 0
#define __LZO_MMODEL        __far
#else
#define __LZO_MMODEL
#endif

#endif /* defined(__LZO_DOS16) || defined(__LZO_WIN16) */


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* (UINT_MAX < LZO_0xffffffffL) */
#endif /* defined(__LZO_STRICT_16BIT) */

#endif /* already included */
