#
# Common definition for all projects.
#

export ARCH = riscv
export VARIANT = 64
export ARCHVAR = $(ARCH)$(VARIANT)

# Tools
TOOLPREFIX = riscv64-linux-gnu-
CC = $(TOOLPREFIX)gcc
AS = $(TOOLPREFIX)gas
LD = $(TOOLPREFIX)ld
OBJCOPY = $(TOOLPREFIX)objcopy
OBJDUMP = $(TOOLPREFIX)objdump

# Flags
CPPFLAGS = -Wall -nostdinc -D_QNX_SOURCE
CPPFLAGS += -isystem /usr/lib/gcc-cross/riscv64-linux-gnu/10/include
ASFLAGS = -D__ASSEMBLY__ -fno-pie
CFLAGS = -DVARIANT_smp -D__QNXNTO__ -Wconversion
CFLAGS += -march=rv64g -mcmodel=medany -mno-relax
CFLAGS += -ffreestanding -fno-common -nostdlib
CFLAGS += -fno-pie -fno-builtin
