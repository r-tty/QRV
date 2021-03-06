/*
 * $QNXLicenseC:
 * Copyright 2008, QNX Software Systems.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You
 * may not reproduce, modify or distribute this software except in
 * compliance with the License. You may obtain a copy of the License
 * at: http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OF ANY KIND, either express or implied.
 *
 * This file may contain contributions from others, either as
 * contributors under the License or as licensors under other terms.
 * Please review this entire file for other proprietary rights or license
 * notices, as well as the QNX Development Suite License Guide at
 * http://licensing.qnx.com/license-guide/ for other information.
 * $
 */

#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <malloc.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>
#include <devctl.h>
#include <sys/dcmd_chr.h>
#include <sys/iomsg.h>
#include <atomic.h>
#include <hw/inout.h>
#include <arm/omap.h>
#include <sys/io-char.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>

#include <sys/trace.h>

char *user_parm;

#define FIFO_TRIG_8     1
#define FIFO_TRIG_16    2
#define FIFO_TRIG_32    3
#define FIFO_TRIG_56    4
#define FIFO_TRIG_60    5

#define FIFO_SIZE       64

typedef struct dev_riscv_qemu_virt {
    TTYDEV tty;
    struct dev_omap *next;
    unsigned intr;
    int iid;
    unsigned clk;
    unsigned div;
    unsigned char fifo;
    uintptr_t port[OMAP_UART_SIZE];

    unsigned kick_maxim;

    unsigned brd;               /* Baud rate divisor */
    unsigned lcr;
    unsigned efr;
    unsigned baud;

    unsigned auto_rts_enable;
    unsigned no_msr_int;        /* Do not enable MSR interrupt */
} DEV_RISCV_QEMU_VIRT;

typedef struct ttyinit_omap {
    TTYINIT tty;
    unsigned loopback;          /*loopback mode */
    unsigned auto_rts_enable;
    unsigned no_msr_int;        /* Do not enable MSR interrupt */
} TTYINIT_RISCV_QEMU_VIRT;

EXT TTYCTRL ttyctrl;

#include "proto.h"
#include <variant.h>
