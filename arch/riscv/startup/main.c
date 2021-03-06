/*
 * Copyright 2013, QNX Software Systems.
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
 */

#include <startup.h>
#include <platform/qemu_virt.h>

#define STARTUP_DEBUG_LEVEL 4

extern void init_qtime(void);
extern void init_clocks(void);

int cpu_handle_common_option(int opt)
{
    return 0;
}

void *startup_memory_map(unsigned size, paddr_t phys, unsigned prot_flags)
{
    return (void *)((uintptr_t)phys);
}

void startup_memory_unmap(void *p)
{
}

void init_clocks()
{
    kprintf("%s\n", __func__);
}

void init_cpuinfo()
{
    kprintf("%s\n", __func__);
}

void init_hwinfo()
{
    kprintf("%s\n", __func__);
}

void init_qtime()
{
    kprintf("%s\n", __func__);
}

void uartintr() {kprintf("@");}
void virtio_disk_intr() {kprintf("$");}


extern void rvq_putc_ser_dbg(int c);

const struct debug_device debug_devices[] = {
    {   .name = "sbi",
        .init = NULL,
        .put = rvq_putc_ser_dbg,
    },
};

/*
 * main()
 *  Startup program executing out of RAM
 *
 * 1. It gathers information about the system and places it in a structure
 *    called the system page. The kernel references this structure to
 *    determine everything it needs to know about the system. This structure
 *    is also available to user programs (read only if protection is on)
 *    via _syspage->.
 *
 * 2. It (optionally) turns on the MMU and starts the next program
 *    in the image file system.
 */
int main(int argc, char **argv, char **envv)
{
    int opt;

    while ((opt = getopt(argc, argv, COMMON_OPTIONS_STRING "d")) != -1) {
        switch (opt) {
          default:
            handle_common_option(opt);
            break;
        }
    }

    /*
     * Initialize debugging output.
     * kprintf will not work before this point.
     */
    select_debug(debug_devices, sizeof(debug_devices));

    /*
     * Initialize clocks
     */
    init_clocks();

    init_raminfo();

    /*
     * Set CPU frequency
     */
    if (cpu_freq == 0) {
        pr_warn("cpu_freq = 0\n");
    }

    /* Claim the RAM used by kernel and RAM-disk if any */
    alloc_ram((paddr_t)_start, _end - _start, 1);
    if (ramdisk_phys_start)
        alloc_ram(ramdisk_phys_start, ramdisk_phys_end-ramdisk_phys_start, 1);

    /*
     * Initialize CPU sections in the syspage.
     * This must be done before init_mmu().
     */
    init_smp();

    /*
     * Initialize the MMU
     */
    init_mmu();

    init_intrinfo();

    init_qtime();

    init_cpuinfo();

    init_hwinfo();

    add_typed_string(_CS_MACHINE, "RISC-V virt");

    /*
     * Load bootstrap executables in the image file system and initialize
     * various syspage pointers. This must be the _last_ initialization done
     * before transferring control to the next program.
     */
    init_system_private();

    /*
     * This is handy for debugging a new version of the startup program.
     * Commenting this line out will save a great deal of code.
     */
    print_syspage();

    return 0;
}
