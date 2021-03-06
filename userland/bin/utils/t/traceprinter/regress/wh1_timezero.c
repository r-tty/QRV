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





/*****************************************************************************
*
*	File:		wh1_timezero.c
*
******************************************************************************
*
*   Contents:	This tests checks that a problem where a timestamp of 0
*				would show up in the logs no longer exists.
*
*	Date:		Aug. 14, 2001
*
*	Author:		Peter Graves
*
*	Notes:		This test must have the tracelogger available to it in it's
*				path.  If this is not available the tests will not be
*				run.
*
*****************************************************************************/

/*--------------------------------------------------------------------------*
 *								STANDARD HEADERS 							*
 *--------------------------------------------------------------------------*/
#include <process.h>
#include <sys/trace.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <pthread.h>
#include <sys/traceparser.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/kercalls.h>
#include <unistd.h>
#include <spawn.h>
#include <stdlib.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/neutrino.h>
#include <sys/procfs.h>
#include <stdio.h>
#include <fcntl.h>


/*--------------------------------------------------------------------------*
 *									LOCAL HEADERS 							*
 *--------------------------------------------------------------------------*/
#ifndef __QNXLOCALREGRESS__
#include "testpoint.h"
#else
#include <regress/testpoint.h>
#endif


/*--------------------------------------------------------------------------*
 *									DEFINES 								*
 *--------------------------------------------------------------------------*/
#define NUM_ITER  40
/*--------------------------------------------------------------------------*
 *									TYPES									*
 *--------------------------------------------------------------------------*/
typedef struct traceparser_state* tp_state_t;
typedef struct {
	procfs_debuginfo info;
	char             buff[_POSIX_PATH_MAX];
} proc_name;


/*--------------------------------------------------------------------------*
 *									GLOBALS 								*
 *--------------------------------------------------------------------------*/
/* This is a global used by the traceparser callback function to
 * tell the main thread that the values it got in the events were
 * correct
 */
static int correct_values;

/*--------------------------------------------------------------------------*
 *									ROUTINES								*
 *--------------------------------------------------------------------------*/
/****************************************************************************
*
*						Subroutine kill_tl
*
*	Purpose:	This function will read though /proc/<pid>/as and try to
*				find a copy of tracelogger running. If it is found it
*				will be killed.
*
*	Parameters: none
*
*	Returns:	-1 on failures, 0 if tracelogger is not found, and 1 when
*				tracelogger has been killed.
*
*****************************************************************************/
int kill_tl()
{
	proc_name name;
	char buf[200];
	int fd,rc, curpid,rval;
	DIR * mydir;
	struct dirent * curent;
	mydir=opendir("/proc/");
	assert(mydir!=NULL);
	rval=0;
	while ((curent=readdir(mydir))!=NULL) {
		curpid=atoi(curent->d_name);
		if (curpid>0) {
			memset(buf,0,sizeof(buf));
			snprintf(buf,sizeof(buf), "/proc/%d/as", curpid);
			fd=open(buf,O_RDONLY);
			if (fd==-1)
				continue;
			rc=devctl(fd, DCMD_PROC_MAPDEBUG_BASE, &name, sizeof name, 0);
			if (rc!=EOK)
				continue;
			if (strstr(name.info.path, "tracelogger")!=NULL) {
				/* This is tracelogger */
				kill(curpid, SIGINT);
				/* We should be able to exit here, but we will continue just to make
			 	 * sure there are no more traceloggers to kill
				 */
				rval=1;
			}

		}
	}
	closedir(mydir);
	return(rval);

}
/****************************************************************************
*
*						Subroutine parse_cb
*
*	Purpose: 	This is a simple parse call back that will just check that
*				the timestamp is not 0
*
*	Parameters:	header - event header
*				time   - time of the event
*				event_p - pointer to the event array
*				length  - length of the event array
*
*	Returns:	This function will set the value of correct values. It
*				will be set to -1 on failure, and 1 on success.
*
*****************************************************************************/
int parse_cb(tp_state_t  state, void * nothing, unsigned header, unsigned time, unsigned * event_p, unsigned length)
{

	/* This is an exceptionally simple parse callback that will just
	 * check to see if the timestamp is 0 or not.
	 */
	if (time==0)
		correct_values=-1;
	if (correct_values==0)
		correct_values=1;
	return(EOK);


}

/****************************************************************************
*
*						Subroutine start_logger
*
*	Purpose: 	This function will start up the tracelogger, and
*				clear any filters that may be in place.
*
*	Returns: 	Pid of the tracelogger
*
*****************************************************************************/
int start_logger(void)
{
	int tlpid,rc;
	char buf[100];
	snprintf(buf, sizeof(buf), "-n%d", NUM_ITER);

	tlpid=spawnlp(P_NOWAIT, "tracelogger", "tracelogger",buf, "-d", NULL);
	if (tlpid==-1) {
		snprintf(buf,sizeof(buf), "Unable to start tracelogger (%s)", strerror(errno));
		testerror(buf);
		teststop("Bad");
		exit(0);
	}
	rc=TraceEvent(_NTO_TRACE_DELALLCLASSES);
	assert(rc!=-1);
	rc=TraceEvent(_NTO_TRACE_CLRCLASSPID, _NTO_TRACE_KERCALL);
	assert(rc!=-1);
	rc=TraceEvent(_NTO_TRACE_CLRCLASSTID, _NTO_TRACE_KERCALL);
	assert(rc!=-1);
	rc=TraceEvent(_NTO_TRACE_CLRCLASSPID, _NTO_TRACE_THREAD);
	assert(rc!=-1);
	rc=TraceEvent(_NTO_TRACE_CLRCLASSTID, _NTO_TRACE_THREAD);
	assert(rc!=-1);
	return(tlpid);
}
/****************************************************************************
*
*						Subroutine main
*
*****************************************************************************/
int main(int argc, char *argv[])
{
	int tlkilled,tlpid, rc,status;  /*tlpid=trace logger pid*/
	struct traceparser_state * tp_state;
	struct stat statbuf;

	/*
	 * Start the tests.
	 */
	teststart(argv[0]);

	/* Kill off the tracelogger if it is running */
	tlkilled=kill_tl();

	/***********************************************************************/

	/***********************************************************************/
	/*
	 * Make sure that timestamps never show up as 0 in wide mode
	 */
 	testpntbegin("timestamp is not 0 in wide mode");

	/* We need to start up the tracelogger in daemon mode, 1 itteration.
	 * We will then start logging, and create some system activity.
	 */
	tlpid=start_logger();
	sleep(1);
	/* Set the logger to wide emmiting mode */
	rc=TraceEvent(_NTO_TRACE_SETALLCLASSESWIDE);
	assert(rc!=-1);
	/* Add all classes */
	rc=TraceEvent(_NTO_TRACE_ADDALLCLASSES);
	assert(rc!=-1);

	rc=TraceEvent(_NTO_TRACE_STARTNOSTATE);
	assert(rc!=-1);
	/* Make sure there is some system activity going on */
	while(waitpid(tlpid, &status, WNOHANG)!=tlpid ) {
		stat("/proc/", &statbuf);

	}
	/* Now, setup the traceparser lib to pull out most of the events
	 */
	tp_state=traceparser_init(NULL);
	assert(tp_state!=NULL);
	traceparser_cs_range(tp_state, NULL, parse_cb, _NTO_TRACE_INTENTER, _NTO_TRACE_INTFIRST, _NTO_TRACE_INTLAST);
	traceparser_cs_range(tp_state, NULL, parse_cb, _NTO_TRACE_KERCALL, _NTO_TRACE_KERCALLFIRST,_NTO_TRACE_KERCALLLAST);
	traceparser_cs(tp_state, NULL, parse_cb,_NTO_TRACE_CONTROL,_NTO_TRACE_CONTROLTIME );
	traceparser_cs_range(tp_state, NULL, parse_cb,_NTO_TRACE_PROCESS, _NTO_TRACE_PROCCREATE, _NTO_TRACE_PROCDESTROY_NAME);
	traceparser_cs_range(tp_state, NULL, parse_cb,_NTO_TRACE_THREAD, _NTO_TRACE_THDEAD, _NTO_TRACE_THDESTROY);
	traceparser_cs_range(tp_state, NULL, parse_cb,_NTO_TRACE_VTHREAD, _NTO_TRACE_VTHDEAD, _NTO_TRACE_VTHDESTROY);

	/* Since we don't want a bunch of output being displayed in the
	 * middle of the tests, turn off verbose output.
	 */
	traceparser_debug(tp_state, stdout, _TRACEPARSER_DEBUG_NONE);
	/* Set correct_values to 0, so we can see if the callback actually
	 * got called.
	 */
	correct_values=0;
	/* And parse the tracebuffer */
	traceparser(tp_state, NULL, "/dev/shmem/tracebuffer");

	if (correct_values==0)
		testpntfail("Our callback never got called, no events?");
	else if (correct_values==-1)
		testpntfail("Wrong parameters in the event");
	else if (correct_values==1)
		testpntpass("Got the correct values");
	else
		testpntfail("This should not happen");

	traceparser_destroy(&tp_state);
 	testpntend();
	/***********************************************************************/

	/***********************************************************************/
	/*
	 * Make sure that timestamps never show up as 0 in fast mode
	 */
 	testpntbegin("timestamp is not 0 in fast mode");

	/* We need to start up the tracelogger in daemon mode, 1 itteration.
	 * We will then start logging, and create some system activity.
	 */
	tlpid=start_logger();
	sleep(1);
	/* Set the logger to wide emmiting mode */
	rc=TraceEvent(_NTO_TRACE_SETALLCLASSESFAST);
	assert(rc!=-1);
	/* Add all classes */
	rc=TraceEvent(_NTO_TRACE_ADDALLCLASSES);
	assert(rc!=-1);

	rc=TraceEvent(_NTO_TRACE_STARTNOSTATE);
	assert(rc!=-1);
	/* Make sure there is some system activity going on */
	while(waitpid(tlpid, &status, WNOHANG)!=tlpid ) {
		stat("/proc/", &statbuf);

	}


	/* Now, setup the traceparser lib to pull out all the events
	 */
	tp_state=traceparser_init(NULL);
	assert(tp_state!=NULL);
	traceparser_cs_range(tp_state, NULL, parse_cb, _NTO_TRACE_INTENTER, _NTO_TRACE_INTFIRST, _NTO_TRACE_INTLAST);
	traceparser_cs_range(tp_state, NULL, parse_cb, _NTO_TRACE_KERCALL, _NTO_TRACE_KERCALLFIRST,_NTO_TRACE_KERCALLLAST);
	traceparser_cs(tp_state, NULL, parse_cb,_NTO_TRACE_CONTROL,_NTO_TRACE_CONTROLTIME );
	traceparser_cs_range(tp_state, NULL, parse_cb,_NTO_TRACE_PROCESS, _NTO_TRACE_PROCCREATE, _NTO_TRACE_PROCDESTROY_NAME);
	traceparser_cs_range(tp_state, NULL, parse_cb,_NTO_TRACE_THREAD, _NTO_TRACE_THDEAD, _NTO_TRACE_THDESTROY);
	traceparser_cs_range(tp_state, NULL, parse_cb,_NTO_TRACE_VTHREAD, _NTO_TRACE_VTHDEAD, _NTO_TRACE_VTHDESTROY);

	/* Since we don't want a bunch of output being displayed in the
	 * middle of the tests, turn off verbose output.
	 */
	traceparser_debug(tp_state, stdout, _TRACEPARSER_DEBUG_NONE);
	/* Set correct_values to 0, so we can see if the callback actually
	 * got called.
	 */
	correct_values=0;
	/* And parse the tracebuffer */
	traceparser(tp_state, NULL, "/dev/shmem/tracebuffer");

	if (correct_values==0)
		testpntfail("Our callback never got called, no events?");
	else if (correct_values==-1)
		testpntfail("Wrong parameters in the event");
	else if (correct_values==1)
		testpntpass("Got the correct values");
	else
		testpntfail("This should not happen");

	traceparser_destroy(&tp_state);
 	testpntend();
	/***********************************************************************/
	/* If the tracelogger was killed, restart it */
	if (tlkilled==1)
		system("reopen /dev/null ; tracelogger -n0 -f /dev/null &");

	teststop(argv[0]);
	return 0;
}


