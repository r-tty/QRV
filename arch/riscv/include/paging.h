/**
 * \file paging.h
 *
 * RISC-V paging definitions.
 *
 * \license MIT (https://opensource.org/licenses/mit-license.php)
 *
 * \copyright (c) 2006-2020 Frans Kaashoek, Robert Morris, Russ Cox,
 *                          Massachusetts Institute of Technology
 */

#ifndef _RISCV_PAGING_H
#define _RISCV_PAGING_H

#define PGSIZE 4096             // bytes per page
#define PGSHIFT 12              // bits of offset within a page

#define PGROUNDUP(sz)  (((sz)+PGSIZE-1) & ~(PGSIZE-1))
#define PGROUNDDOWN(a) (((a)) & ~(PGSIZE-1))

#define PTE_V (1L << 0)         // valid
#define PTE_R (1L << 1)
#define PTE_W (1L << 2)
#define PTE_X (1L << 3)
#define PTE_U (1L << 4)         // 1 -> user can access

// shift a physical address to the right place for a PTE.
#define PA2PTE(pa) ((((uint64_t)pa) >> 12) << 10)

#define PTE2PA(pte) (((pte) >> 10) << 12)

#define PTE_FLAGS(pte) ((pte) & 0x3FF)

// extract the three 9-bit page table indices from a virtual address.
#define PXMASK          0x1FF   // 9 bits
#define PXSHIFT(level)  (PGSHIFT+(9*(level)))
#define PX(level, va) ((((uint64_t) (va)) >> PXSHIFT(level)) & PXMASK)

// one beyond the highest possible virtual address.
// MAXVA is actually one bit less than the max allowed by
// Sv39, to avoid having to sign-extend virtual addresses
// that have the high bit set.
#define MAXVA (1L << (9 + 9 + 9 + 12 - 1))

typedef uint64_t pde_t;
typedef uint64_t pte_t;
typedef uint64_t *pagetable_t;    // 512 PTEs

#endif
