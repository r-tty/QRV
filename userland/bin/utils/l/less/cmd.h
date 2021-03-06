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
 * Copyright (c) 1984,1985,1989,1994,1995,1996  Mark Nudelman
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice in the documentation and/or other materials provided with
 *    the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#define	MAX_USERCMD		500
#define	MAX_CMDLEN		16

#define	A_B_LINE		2
#define	A_B_SCREEN		3
#define	A_B_SCROLL		4
#define	A_B_SEARCH		5
#define	A_DIGIT			6
#define	A_DISP_OPTION		7
#define	A_DEBUG			8
#define	A_EXAMINE		9
#define	A_FIRSTCMD		10
#define	A_FREPAINT		11
#define	A_F_LINE		12
#define	A_F_SCREEN		13
#define	A_F_SCROLL		14
#define	A_F_SEARCH		15
#define	A_GOEND			16
#define	A_GOLINE		17
#define	A_GOMARK		18
#define	A_HELP			19
#define	A_NEXT_FILE		20
#define	A_PERCENT		21
#define	A_PREFIX		22
#define	A_PREV_FILE		23
#define	A_QUIT			24
#define	A_REPAINT		25
#define	A_SETMARK		26
#define	A_SHELL			27
#define	A_STAT			28
#define	A_FF_LINE		29
#define	A_BF_LINE		30
#define	A_VERSION		31
#define	A_VISUAL		32
#define	A_F_WINDOW		33
#define	A_B_WINDOW		34
#define	A_F_BRACKET		35
#define	A_B_BRACKET		36
#define	A_PIPE			37
#define	A_INDEX_FILE		38
#define	A_UNDO_SEARCH		39
#define	A_FF_SCREEN		40
#define	A_LSHIFT		41
#define	A_RSHIFT		42
#define	A_AGAIN_SEARCH		43
#define	A_T_AGAIN_SEARCH	44
#define	A_REVERSE_SEARCH	45
#define	A_T_REVERSE_SEARCH	46
#define	A_OPT_TOGGLE		47
#define	A_OPT_SET		48
#define	A_OPT_UNSET		49
#define	A_F_FOREVER		50
#define	A_GOPOS			51

#define	A_INVALID		100
#define	A_NOACTION		101
#define	A_UINVALID		102
#define	A_END_LIST		103

#define	A_EXTRA			0200


/* Line editting characters */

#define	EC_BACKSPACE	1
#define	EC_LINEKILL	2
#define	EC_RIGHT	3
#define	EC_LEFT		4
#define	EC_W_LEFT	5
#define	EC_W_RIGHT	6
#define	EC_INSERT 	7
#define	EC_DELETE	8
#define	EC_HOME		9
#define	EC_END		10
#define	EC_W_BACKSPACE	11
#define	EC_W_DELETE	12
#define	EC_UP		13
#define	EC_DOWN		14
#define	EC_EXPAND	15
#define	EC_F_COMPLETE	17
#define	EC_B_COMPLETE	18
#define	EC_LITERAL	19

#define	EC_UINVALID	102

/* Flags for editchar() */
#define	EC_PEEK		01
#define	EC_NOHISTORY	02
#define	EC_NOCOMPLETE	04

/* Environment variable stuff */
#define	EV_OK		01
