/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *		 BeBox port Copyright 1997 by Olaf Seibert.
 *		 All Rights Reserved.
 *
 * This file is made available to you only for compilation for and/or
 * execution on non-Intel target CPUs. You are expressly forbidden to
 * compile this code with the intent, effect or purpose to run it on
 * Intel-designed and/or 80x86 compatible CPUs. You are also expressly
 * forbidden to modify this code with that intent, effect or purpose.
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */
/*
 * os_beos.c  Additional stuff for BeOS (rest is in os_unix.c)
 */

#include <float.h>
//#include <termios.h>
#include <kernel/OS.h>
#include "vim.h"

#if __INTEL__

#error "I told you that you were not permitted to compile this code for this CPU!"

#else

    int
check_for_bebox(void)
{
    static int checked = 0;
    static int is_bebox = 0;

    if (!checked) {
	system_info si;

	checked = 1;

	get_system_info(&si);
#if 0
	printf("\r\ncpu_count = %d\r\n", si.cpu_count);
	printf("cpu_type = %d\r\n", si.cpu_type);
	printf("cpu_clock_speed = %f\r\n", si.cpu_clock_speed);
	printf("bus_clock_speed = %f\r\n", si.bus_clock_speed);
#endif
	/*
	 * Measured values on my machine at some time were:
	 * cpu_clock_speed = 66434214
	 * bus_clock_speed = 33238632
	 */
	if (si.cpu_count == 2 &&
	    ((si.cpu_type == B_CPU_PPC_603  && si.cpu_clock_speed <=  66666666) ||
	     (si.cpu_type == B_CPU_PPC_603e && si.cpu_clock_speed <= 133333333)) &&
	    si.bus_clock_speed <= 33333333) {
	    is_bebox = 1;
	}
	if (si.platform_type == B_AT_CLONE_PLATFORM ||
		si.cpu_type == B_CPU_PPC_686 ||
		si.cpu_type == B_CPU_X86) {
	    emsg((char_u *)"This program does not work on this CPU type.");
	    getout(1);
	}
	if (!is_bebox) {
	    msg((char_u *)"This program works better on a real BeBox.");
	}
    }
    return is_bebox;
}

#if USE_THREAD_FOR_INPUT_WITH_TIMEOUT

#ifdef PROTO	    /* making prototypes on Unix */
#define sem_id int
#define thread_id int
#endif

int charbuf;
sem_id character_present;
sem_id character_wanted;
thread_id read_thread_id;

#define TRY_ABORT	0

#if TRY_ABORT
    static void
mostly_ignore(int sig)
{
}
#endif

    static long
read_thread(void *dummy)
{
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
#if TRY_ABORT
    signal(SIGUSR1, mostly_ignore);
#endif

    for (;;) {
	if (acquire_sem(character_wanted) != B_NO_ERROR)
	    break;
	charbuf = 0;
	if (read(read_cmd_fd, &charbuf, 1) > 0)
	    charbuf++;		// insure non-zero to indicate valid
	release_sem(character_present);
    }

    return 0;
}

    void
beos_cleanup_read_thread(void)
{
    if (character_present > 0)
	delete_sem(character_present);
    character_present = 0;
    if (read_thread_id > 0)
	kill_thread(read_thread_id);
    read_thread_id = 0;
}

#endif

/*
 * select() emulation. Hopefully, in DR9 there will be something
 * useful supplied by the system. ... Alas, not.
 */

    int
beos_select(int nbits,
       struct fd_set *rbits,
       struct fd_set *wbits,
       struct fd_set *ebits,
       struct timeval *timeout)
{
    double tmo;

    check_for_bebox();
    if (nbits == 0) {
	/* select is purely being used for delay */
	snooze(timeout->tv_sec * 1e6 + timeout->tv_usec);
	return 0;
    }
#if 0
    /*
     * This does not seem to work either. Reads here are not supposed to
     * block indefinitely, yet they do. This is most annoying.
     */
    if (FD_ISSET(0, rbits)) {
	char cbuf[1];
	int count;
	struct termios told;
	struct termios tnew;
	tcgetattr(0, &told);
	tnew = told;
	tnew.c_lflag &= ~ICANON;
	tnew.c_cc[VMIN] = 0;
	tnew.c_cc[VTIME] = timeout->tv_sec * 10 + timeout->tv_usec / 100000;
	tcsetattr(0, TCSANOW, &tnew);

	count = read(0, &cbuf, sizeof(cbuf));
	tcsetattr(0, TCSANOW, &told);
	if (count > 0) {
	    add_to_input_buf(&cbuf[0], count);
	    return 1;
	}
	return 0;
    }
#endif
#if USE_THREAD_FOR_INPUT_WITH_TIMEOUT
    /*
     * Check if the operation is really on stdin...
     */
    if (FD_ISSET(read_cmd_fd, rbits))
    {
	int acquired;

	/*
	 * Is this the first time through?
	 * Then start up the thread and initialise the semaphores.
	 */
	if (character_present == 0) {
	    character_present = create_sem(0, "vim character_present");
	    character_wanted = create_sem(1, "vim character_wanted");
	    read_thread_id = spawn_thread(read_thread, "vim async read",
		    B_NORMAL_PRIORITY, NULL);
	    atexit(beos_cleanup_read_thread);
	    resume_thread(read_thread_id);
	}

	/* timeout == NULL means "indefinitely" */
	if (timeout) {
	    tmo = timeout->tv_sec * 1e6 + timeout->tv_usec;
	    /* 0 means "don't wait, which is impossible to do exactly. */
	    if (tmo == 0)
		tmo = 1.0;
	} else {
	    tmo = FLT_MAX;
	}
#if TRY_ABORT
	release_sem(character_wanted);
#endif
	acquired = acquire_sem_etc(character_present, 1, B_TIMEOUT, tmo);
	if (acquired == B_NO_ERROR) {
	    if (charbuf) {
		add_to_input_buf((char_u *)&charbuf, 1);
#if !TRY_ABORT
		release_sem(character_wanted);
#endif

		return 1;
	    } else {
#if !TRY_ABORT
		release_sem(character_wanted);
#endif

		return 0;
	    }
	}
#if TRY_ABORT
	else {
	    /*
	     * Timeout occurred. Break the read() call by sending
	     * a signal. Problem: it may be just read()ing it now.
	     * Therefore we still have to finish the handshake with
	     * the thread and maybe remember the character.
	     */
	    kill(read_thread_id, SIGUSR1);
	    /*
	     *	If some other error occurred, don't hang now.
	     * (We will most likely hang later anyway...)
	     */
	    if (acquired == B_TIMED_OUT)
		acquire_sem(character_present);
	    if (charbuf) {
		add_to_input_buf((char_u *)&charbuf, 1);
		return 1;
	    }
	    return 0;
	}
#endif
    }
#endif

#if USE_GUI_BEOS
    /*
     * If not reading from terminal, pretend there is input.
     * This makes the pty reading (for the GUI) "work" for
     * :!ls but not for :r !ls ... weird.
     */
    if (gui.in_use && State == EXTERNCMD)
	return 1;
#endif
    return 0;
}

#endif
