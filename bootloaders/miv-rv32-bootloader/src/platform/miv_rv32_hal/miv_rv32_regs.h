/*******************************************************************************
 * Copyright 2019-2023 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file miv_rv32_regs.h
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief Mi-V soft processor register bit mask and shift constants encodings.
 *
 */
#ifndef MIV_RV32_REGS_H
#define MIV_RV32_REGS_H

#ifdef __cplusplus
extern "C" {
#endif

#define MSTATUS_UIE             0x00000001UL
#define MSTATUS_SIE             0x00000002UL
#define MSTATUS_HIE             0x00000004UL
#define MSTATUS_MIE             0x00000008UL
#define MSTATUS_UPIE            0x00000010UL
#define MSTATUS_SPIE            0x00000020UL
#define MSTATUS_HPIE            0x00000040UL
#define MSTATUS_MPIE            0x00000080UL
#define MSTATUS_SPP             0x00000100UL
#define MSTATUS_HPP             0x00000600UL
#define MSTATUS_MPP             0x00001800UL
#define MSTATUS_FS              0x00006000UL
#define MSTATUS_XS              0x00018000UL
#define MSTATUS_MPRV            0x00020000UL
#define MSTATUS_SUM             0x00040000UL
#define MSTATUS_MXR             0x00080000UL
#define MSTATUS_TVM             0x00100000UL
#define MSTATUS_TW              0x00200000UL
#define MSTATUS_TSR             0x00400000UL
#define MSTATUS_RES             0x7F800000UL
#define MSTATUS32_SD            0x80000000UL
#define MSTATUS64_SD            0x8000000000000000UL

#define MCAUSE32_CAUSE          0x7FFFFFFFUL
#define MCAUSE64_CAUSE          0x7FFFFFFFFFFFFFFFUL
#define MCAUSE32_INT            0x80000000UL
#define MCAUSE64_INT            0x8000000000000000UL

#define MIP_SSIP                (1u << IRQ_S_SOFT)
#define MIP_HSIP                (1u << IRQ_H_SOFT)
#define MIP_MSIP                (1u << IRQ_M_SOFT)
#define MIP_STIP                (1u << IRQ_S_TIMER)
#define MIP_HTIP                (1u << IRQ_H_TIMER)
#define MIP_MTIP                (1u << IRQ_M_TIMER)
#define MIP_SEIP                (1u << IRQ_S_EXT)
#define MIP_HEIP                (1u << IRQ_H_EXT)
#define MIP_MEIP                (1u << IRQ_M_EXT)

#define PRV_M                   3U

#define VM_MBARE                0U
#define VM_MBB                  1U
#define VM_MBBID                2U
#define VM_SV32                 8U
#define VM_SV39                 9U
#define VM_SV48                 10U

#define IRQ_S_SOFT              1U
#define IRQ_H_SOFT              2U
#define IRQ_M_SOFT              3U
#define IRQ_S_TIMER             5U
#define IRQ_H_TIMER             6U
#define IRQ_M_TIMER             7U
#define IRQ_S_EXT               9U
#define IRQ_H_EXT               10U
#define IRQ_M_EXT               11U

#define DEFAULT_RSTVEC          0x00001000
#define DEFAULT_NMIVEC          0x00001004
#define DEFAULT_MTVEC           0x00001010
#define CONFIG_STRING_ADDR      0x0000100C
#define EXT_IO_BASE             0x40000000
#define DRAM_BASE               0x80000000

#ifdef __riscv

#if __riscv_xlen == 64
# define MSTATUS_SD             MSTATUS64_SD
# define SSTATUS_SD             SSTATUS64_SD
# define MCAUSE_INT             MCAUSE64_INT
# define MCAUSE_CAUSE           MCAUSE64_CAUSE
# define RISCV_PGLEVEL_BITS     9
#else
# define MSTATUS_SD             MSTATUS32_SD
# define SSTATUS_SD             SSTATUS32_SD
# define RISCV_PGLEVEL_BITS     10
# define MCAUSE_INT             MCAUSE32_INT
# define MCAUSE_CAUSE           MCAUSE32_CAUSE
#endif

#define RISCV_PGSHIFT           12U
#define RISCV_PGSIZE            (1U << RISCV_PGSHIFT)

#ifndef __ASSEMBLER__

#ifdef __GNUC__

#define read_csr(reg) ({ unsigned long __tmp; \
  __asm__ volatile ("csrr %0, " #reg : "=r"(__tmp)); \
  __tmp; })

#define write_csr(reg, val) ({ \
  __asm__ volatile ("csrw " #reg ", %0" :: "rK"(val)); })

#define swap_csr(reg, val) ({ unsigned long __tmp; \
  __asm__ volatile ("csrrw %0, " #reg ", %1" : "=r"(__tmp) : "rK"(val)); \
  __tmp; })

#define set_csr(reg, bit) ({ unsigned long __tmp; \
  __asm__ volatile ("csrrs %0, " #reg ", %1" : "=r"(__tmp) : "rK"(bit)); \
  __tmp; })

#define clear_csr(reg, bit) ({ unsigned long __tmp; \
  __asm__ volatile ("csrrc %0, " #reg ", %1" : "=r"(__tmp) : "rK"(bit)); \
  __tmp; })

#ifdef __riscv_atomic

#define MASK(nr)                    (1UL << nr)
#define MASK_NOT(nr)                (~(1UL << nr))

/**
 * atomic_read - read atomic variable
 * @v: pointer of type int
 *
 * Atomically reads the value of @v.
 */
static inline int atomic_read(const int *v)
{
    return *((volatile int *)(v));
}

/**
 * atomic_set - set atomic variable
 * @v: pointer of type int
 * @i: required value
 *
 * Atomically sets the value of @v to @i.
 */
static inline void atomic_set(int *v, int i)
{
    *v = i;
}

/**
 * atomic_add - add integer to atomic variable
 * @i: integer value to add
 * @v: pointer of type int
 *
 * Atomically adds @i to @v.
 */
static inline void atomic_add(int i, int *v)
{
    __asm__ __volatile__ (
        "amoadd.w zero, %1, %0"
        : "+A" (*v)
        : "r" (i));
}

static inline int atomic_fetch_add(unsigned int mask, int *v)
{
    int out;

    __asm__ __volatile__ (
        "amoadd.w %2, %1, %0"
        : "+A" (*v), "=r" (out)
        : "r" (mask));
    return out;
}

/**
 * atomic_sub - subtract integer from atomic variable
 * @i: integer value to subtract
 * @v: pointer of type int
 *
 * Atomically subtracts @i from @v.
 */
static inline void atomic_sub(int i, int *v)
{
    atomic_add(-i, v);
}

static inline int atomic_fetch_sub(unsigned int mask, int *v)
{
    int out;

    __asm__ __volatile__ (
        "amosub.w %2, %1, %0"
        : "+A" (*v), "=r" (out)
        : "r" (mask));
    return out;
}

/**
 * atomic_add_return - add integer to atomic variable
 * @i: integer value to add
 * @v: pointer of type int
 *
 * Atomically adds @i to @v and returns the result
 */
static inline int atomic_add_return(int i, int *v)
{
    register int c;
    __asm__ __volatile__ (
        "amoadd.w %0, %2, %1"
        : "=r" (c), "+A" (*v)
        : "r" (i));
    return (c + i);
}

/**
 * atomic_sub_return - subtract integer from atomic variable
 * @i: integer value to subtract
 * @v: pointer of type int
 *
 * Atomically subtracts @i from @v and returns the result
 */
static inline int atomic_sub_return(int i, int *v)
{
    return atomic_add_return(-i, v);
}

/**
 * atomic_inc - increment atomic variable
 * @v: pointer of type int
 *
 * Atomically increments @v by 1.
 */
static inline void atomic_inc(int *v)
{
    atomic_add(1, v);
}

/**
 * atomic_dec - decrement atomic variable
 * @v: pointer of type int
 *
 * Atomically decrements @v by 1.
 */
static inline void atomic_dec(int *v)
{
    atomic_add(-1, v);
}

static inline int atomic_inc_return(int *v)
{
    return atomic_add_return(1, v);
}

static inline int atomic_dec_return(int *v)
{
    return atomic_sub_return(1, v);
}

/**
 * atomic_sub_and_test - subtract value from variable and test result
 * @i: integer value to subtract
 * @v: pointer of type int
 *
 * Atomically subtracts @i from @v and returns
 * true if the result is zero, or false for all
 * other cases.
 */
static inline int atomic_sub_and_test(int i, int *v)
{
    return (atomic_sub_return(i, v) == 0);
}

/**
 * atomic_inc_and_test - increment and test
 * @v: pointer of type int
 *
 * Atomically increments @v by 1
 * and returns true if the result is zero, or false for all
 * other cases.
 */
static inline int atomic_inc_and_test(int *v)
{
    return (atomic_inc_return(v) == 0);
}

/**
 * atomic_dec_and_test - decrement and test
 * @v: pointer of type int
 *
 * Atomically decrements @v by 1 and
 * returns true if the result is 0, or false for all other
 * cases.
 */
static inline int atomic_dec_and_test(int *v)
{
    return (atomic_dec_return(v) == 0);
}

/**
 * atomic_add_negative - add and test if negative
 * @i: integer value to add
 * @v: pointer of type int
 *
 * Atomically adds @i to @v and returns true
 * if the result is negative, or false when
 * result is greater than or equal to zero.
 */
static inline int atomic_add_negative(int i, int *v)
{
    return (atomic_add_return(i, v) < 0);
}

static inline int atomic_xchg(int *v, int n)
{
    register int c;
    __asm__ __volatile__ (
        "amoswap.w %0, %2, %1"
        : "=r" (c), "+A" (*v)
        : "r" (n));
    return c;
}

/**
 * atomic_and - Atomically clear bits in atomic variable
 * @mask: Mask of the bits to be retained
 * @v: pointer of type int
 *
 * Atomically retains the bits set in @mask from @v
 */
static inline void atomic_and(unsigned int mask, int *v)
{
    __asm__ __volatile__ (
        "amoand.w zero, %1, %0"
        : "+A" (*v)
        : "r" (mask));
}

static inline int atomic_fetch_and(unsigned int mask, int *v)
{
    int out;
    __asm__ __volatile__ (
        "amoand.w %2, %1, %0"
        : "+A" (*v), "=r" (out)
        : "r" (mask));
    return out;
}

/**
 * atomic_or - Atomically set bits in atomic variable
 * @mask: Mask of the bits to be set
 * @v: pointer of type int
 *
 * Atomically sets the bits set in @mask in @v
 */
static inline void atomic_or(unsigned int mask, int *v)
{
    __asm__ __volatile__ (
        "amoor.w zero, %1, %0"
        : "+A" (*v)
        : "r" (mask));
}

static inline int atomic_fetch_or(unsigned int mask, int *v)
{
    int out;
    __asm__ __volatile__ (
        "amoor.w %2, %1, %0"
        : "+A" (*v), "=r" (out)
        : "r" (mask));
    return out;
}

/**
 * atomic_xor - Atomically flips bits in atomic variable
 * @mask: Mask of the bits to be flipped
 * @v: pointer of type int
 *
 * Atomically flips the bits set in @mask in @v
 */
static inline void atomic_xor(unsigned int mask, int *v)
{
    __asm__ __volatile__ (
        "amoxor.w zero, %1, %0"
        : "+A" (*v)
        : "r" (mask));
}

static inline int atomic_fetch_xor(unsigned int mask, int *v)
{
    int out;
    __asm__ __volatile__ (
        "amoxor.w %2, %1, %0"
        : "+A" (*v), "=r" (out)
        : "r" (mask));
    return out;
}

/**
 * test_and_set_bit - Set a bit and return its old value
 * @nr: Bit to set
 * @addr: Address to count from
 *
 * This operation is atomic and cannot be reordered.
 * It also implies a memory barrier.
 */
static inline int test_and_set_bit(int nr, volatile unsigned long *addr)
{
    unsigned long __res, __mask;
    __mask = MASK(nr);
    __asm__ __volatile__ (                \
        "amoor.w %0, %2, %1"            \
        : "=r" (__res), "+A" (*addr)    \
        : "r" (__mask));                \

        return ((__res & __mask) != 0);
}

/**
 * test_and_clear_bit - Clear a bit and return its old value
 * @nr: Bit to clear
 * @addr: Address to count from
 *
 * This operation is atomic and cannot be reordered.
 * It also implies a memory barrier.
 */
static inline int test_and_clear_bit(int nr, volatile unsigned long *addr)
{
    unsigned long __res, __mask;
    __mask = MASK_NOT(nr);
    __asm__ __volatile__ (                \
        "amoand.w %0, %2, %1"            \
        : "=r" (__res), "+A" (*addr)    \
        : "r" (__mask));                \

        return ((__res & __mask) != 0);
}

/**
 * test_and_change_bit - Change a bit and return its old value
 * @nr: Bit to change
 * @addr: Address to count from
 *
 * This operation is atomic and cannot be reordered.
 * It also implies a memory barrier.
 */
static inline int test_and_change_bit(int nr, volatile unsigned long *addr)
{

    unsigned long __res, __mask;
    __mask = MASK(nr);
    __asm__ __volatile__ (                \
        "amoxor.w %0, %2, %1"            \
        : "=r" (__res), "+A" (*addr)    \
        : "r" (__mask));                \

        return ((__res & __mask) != 0);
}

/**
 * set_bit - Atomically set a bit in memory
 * @nr: the bit to set
 * @addr: the address to start counting from
 *
 * This function is atomic and may not be reordered. 
 */

static inline void set_bit(int nr, volatile unsigned long *addr)
{
    __asm__ __volatile__ (                    \
        "AMOOR.w zero, %1, %0"            \
        : "+A" (*addr)            \
        : "r" (MASK(nr)));
}

/**
 * clear_bit - Clears a bit in memory
 * @nr: Bit to clear
 * @addr: Address to start counting from
 *
 * clear_bit() is atomic and may not be reordered.  
 */
static inline void clear_bit(int nr, volatile unsigned long *addr)
{
    __asm__ __volatile__ (                    \
        "AMOAND.w zero, %1, %0"            \
        : "+A" (*addr)            \
        : "r" (MASK_NOT(nr)));
}

/**
 * change_bit - Toggle a bit in memory
 * @nr: Bit to change
 * @addr: Address to start counting from
 *
 * change_bit() is atomic and may not be reordered.
 */
static inline void change_bit(int nr, volatile unsigned long *addr)
{
    __asm__ __volatile__ (                    \
            "AMOXOR.w zero, %1, %0"            \
            : "+A" (*addr)            \
            : "r" (MASK(nr)));
}

#endif /* __riscv_atomic */

#endif  /* __GNUC__ */

#endif  /* __ASSEMBLER__ */

#endif  /* __riscv */

#ifdef __cplusplus
}
#endif

#endif    /* RISCV_REGS_H */
