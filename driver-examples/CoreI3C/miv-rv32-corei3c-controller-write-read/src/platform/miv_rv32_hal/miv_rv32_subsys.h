/*******************************************************************************
 * Copyright 2023 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file miv_rv32_subsys.h
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief Mi-V soft processor SUBSYS regsiter description and API fuctions.
 *
 */
#ifndef MIV_RV32_SUBSYS_H
#define MIV_RV32_SUBSYS_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MIV_LEGACY_RV32
#define MGEUI                   16U
#define MGECI                   17U
#define MSYS_EI0                24U
#define MSYS_EI1                25U
#define MSYS_EI2                26U
#define MSYS_EI3                27U
#define MSYS_EI4                28U
#define MSYS_EI5                29U

#define MGEUI_MEIP              (1u << MGEUI)
#define MGECI_MEIP              (1u << MGECI)
#define MSYS_EI0IP              (1u << MSYS_EI0)
#define MSYS_EI1IP              (1u << MSYS_EI1)
#define MSYS_EI2IP              (1u << MSYS_EI2)
#define MSYS_EI3IP              (1u << MSYS_EI3)
#define MSYS_EI4IP              (1u << MSYS_EI4)
#define MSYS_EI5IP              (1u << MSYS_EI5)

#define MIV_LOCAL_IRQ_MIN         16
#ifndef MIV_RV32_V3_0 /*For MIV_RV32 v3.1*/
#define SUBSYSR                 22U
#define SUBSYS_EI               23U
#define MSYS_EI6                30U
#define MSYS_EI7                31U
#define MIV_SUBSYSR             (1u << SUBSYSR)
#define MIV_SUBSYS              (1u << SUBSYS_EI)
#define MSYS_EI6IP              (1u << MSYS_EI6)
#define MSYS_EI7IP              (1u << MSYS_EI7)
#define MIV_LOCAL_IRQ_MAX         31

#else /* MIV_RV32_V3_0 */
#define MIV_LOCAL_IRQ_MAX         29
#define SUBSYS_EI               30U
#define MIP_SUBSYS_REG          (1u << SUBSYS_EI)

#endif /* MIV_RV32_V3_0 */
#endif /* MIV_LEGACY_RV32 */

#ifdef MIV_LEGACY_RV32
#define MIV_LOCAL_IRQ_MAX         0U
#define MIV_LOCAL_IRQ_MIN         0U
#endif /* MIV_LEGACY_RV32 */

#ifdef __cplusplus
}
#endif

/*-------------------------------------------------------------------------*//**
  SUBSYS Register Configuration
  =======================================
  For the SUBSYS registers configutation, the following definitions are used in 
  the SUBSYS API functions. For example, to raise soft interrupts, enable parity
  checks, soft reset, and so on.

  |      Configuration       | Value |                   Description                  |  
  |--------------------------|-------|------------------------------------------------|  
  | SUBSYS_SOFT_REG_GRP_DED  | 0x04 | Mask for the Core GPR DED Reset Register        |
  | SUBSYS_CFG_PARITY_CHECK  | 0x01 | Use to set or clear the parity check on the TCM |
  | SUBSYS_SOFT_RESET        | 0x01 | Use the SUBSYS soft reset the MIV_RV32 IP core  |
  | SUBSYS_SOFT_IRQ          | 0x02 | Use to raise a software interrupt through SUBSYS|
 */
/*Mask for the Core GPR DED Reset Register*/
#define SUBSYS_SOFT_REG_GRP_DED                             0x04U

/*Use to set or clear the parity check on the TCM*/
#define SUBSYS_CFG_PARITY_CHECK                             0x01U

/*Use the SUBSYS soft reset the MIV_RV32 IP core*/
#define SUBSYS_SOFT_RESET                                   0x01U

/*Use to raise a software interrupt through SUBSYS*/
#define SUBSYS_SOFT_IRQ                                     0x02U

/*-------------------------------------------------------------------------*//**
  SUBSYS Interrupt Request Masks
  =======================================
  The following values correspond to the bit value of the SUBSYS interrupt
  enable and interrupt pending register.

  |        Interrupt Mask      |  Value  |               Description           |
  |----------------------------|---------|-------------------------------------|
  | SUBSYS_TCM_ECC_CE_IRQ      |  0x01u  | TCM ECC controllable error IRQ enable |
  | SUBSYS_TCM_ECC_UCE_IRQ     |  0x02u  | TCM ECC uncontrollable error IRQ enable | 
  | SUBSYS_AXI_WR_RESP_IRQ     |  0x10u  | AXI write response error IRQ enable | 
  | SUBSYS_ICACHE_ECC_CE_IRQ   |  0x40u  | Icache ECC Correctable error IRQ    | 
  | SUBSYS_ICACHE_ECC_UCE_IRQ  |  0x80u  | Icache ECC Uncorrectable error IRQ  | 
  | SUBSYS_BASE_ADDR           | 0x6000u | Base address of the SUBSYS          |
 */
/* TCM ECC correctable error irq enable mask value */
#define SUBSYS_TCM_ECC_CE_IRQ            0x01u

/* TCMECC uncorrectable error irq enable */
#define SUBSYS_TCM_ECC_UCE_IRQ           0x02u

/* AXI write response error irq enable */
#define SUBSYS_AXI_WR_RESP_IRQ           0x10u

/*Icache ECC Correctable error irq*/
#define SUBSYS_ICACHE_ECC_CE_IRQ         0x40u

/*Icache ECC Uncorrectable error irq*/
#define SUBSYS_ICACHE_ECC_UCE_IRQ        0x80u

/*Base address of the SUBSYS*/
#define SUBSYS_BASE_ADDR                 0x00006000UL

/***************************************************************************//**
  Subsys contains interrupt enable, interrupt pending and Subsys registers
  which can be used to enable SUBSYS specific features such as ECC for vaious
  memories. For more available features refer to the MIV_RV32 User Guide
*/
typedef struct
{
    volatile uint32_t cfg;          	/*Parity is not supported by MIV_RV32 v3.1 and MIV_RV32 v3.0.100*/
    volatile uint32_t reserved0[3];
    volatile uint32_t irq_en;           /*offset 0x10*/
    volatile uint32_t irq_pend;
    volatile uint32_t reserved1[2];
    volatile uint32_t soft_reg;         /*offset 0x20*/
} SUBSYS_Type;

#define SUBSYS                           ((SUBSYS_Type *)SUBSYS_BASE_ADDR)

/***************************************************************************//**
  The MRV32_subsys_enable_irq() function initializes the SUBSYS interrupts. It 
  takes the logical OR of the following defined IRQ masks as a parameter.
  
  @param irq_mask
    |      irq_mask          | Value      |
    |------------------------|------------|
    | SUBSYS_TCM_ECC_CE_IRQ  |   0x01u    |
    | SUBSYS_TCM_ECC_UCE_IRQ  |   0x02u    |
    | SUBSYS_AXI_WR_RESP_IRQ  |   0x10u    |
    | SUBSYS_ICACHE_ECC_CE_IRQ  |   0x40u    |
    | SUBSYS_ICACHE_ECC_UCE_IRQ  |   0x80u    |
    Use logical OR values of one or more interrupts to enable them in the SUBSYS
  @return 
  This function does not return any value.
 */
static inline void MRV32_subsys_enable_irq(uint32_t irq_mask)
{
    SUBSYS->irq_en |= irq_mask;
}

/***************************************************************************//**
  The MRV32_subsys_disable_irq() function disables the SUBSYS interrupts. It 
  takes the logical OR of the following defined IRQ masks as a parameter.
  
  @param irq_mask
    |      irq_mask          | Value      |
    |------------------------|------------|
    | SUBSYS_TCM_ECC_CE_IRQ  |   0x01u    |
    | SUBSYS_TCM_ECC_UCE_IRQ  |   0x02u    |
    | SUBSYS_AXI_WR_RESP_IRQ  |   0x10u    |
    | SUBSYS_ICACHE_ECC_CE_IRQ  |   0x40u    |
    | SUBSYS_ICACHE_ECC_UCE_IRQ  |   0x80u    |
    Use logical OR values of one or more interrupts to disable them in the SUBSYS
  @return 
  This function does not return any value.
 */
static inline void MRV32_subsys_disable_irq(uint32_t irq_mask)
{
    SUBSYS->irq_en &= ~irq_mask;
}

/***************************************************************************//**
  The MRV32_subsys_clear_irq() function clears the SUBSYS interrupts, which was
  triggered. It takes the logical OR of the following defined IRQ masks as a 
  parameter.
  @param irq_mask
    |      irq_mask          | Value      |
    |------------------------|------------|
    | SUBSYS_TCM_ECC_CE_IRQ  |   0x01u    |
    | SUBSYS_TCM_ECC_UCE_IRQ  |   0x02u    |
    | SUBSYS_AXI_WR_RESP_IRQ  |   0x10u    |
    | SUBSYS_ICACHE_ECC_CE_IRQ  |   0x40u    |
    | SUBSYS_ICACHE_ECC_UCE_IRQ  |   0x80u    |
    Use logical OR values of one or more interrupts to disable them in the SUBSYS
  @return 
  This function does not return any value.
 */
static inline void MRV32_subsys_clear_irq(uint32_t irq_mask)
{
    SUBSYS->irq_pend |= irq_mask;
}

/***************************************************************************//**
  The MRV32_subsys_irq_cause() function returns the irq_pend register value which
  is present in the SUBSYS. This is be used to check which irq_mask value 
  caused the SUBSYS interrupt to occur.
  @param
  This function does not take any parameters

  @return
  This function returns the irq_pend regsiter value.
*/
static inline uint32_t MRV32_subsys_irq_cause()
{
    return SUBSYS->irq_pend;
}

/***************************************************************************//**
  The MRV32_is_gpr_ded() function returns the core_gpr_ded_reset_reg bit value.
  When ECC is enabled, the core_gpr_ded_reset_reg is set when the core was
  reset due to GPR DED error.
  @param
  This function does not take any parameters.

  @return
  This functions returns the CORE_GPR_DED_RESET_REG bit value.
 */
static inline uint32_t MRV32_is_gpr_ded(void)
{
    return((SUBSYS->soft_reg & SUBSYS_SOFT_REG_GRP_DED) >> 0x02u);
}

/***************************************************************************//**
  The MRV32_clear_gpr_ded() function must be used to clear the 
  core_gpr_ded_reset_reg bit. When ECC is enabled, the core_gpr_ded_reset_reg is
  set when the core was previously reset due to GPR DED error.
  @param
  This function does not take any parameters.

  @return
  This function does not return any value.
 */
static inline void MRV32_clear_gpr_ded(void)
{
    SUBSYS->soft_reg &= ~SUBSYS_SOFT_REG_GRP_DED;
}

/***************************************************************************//**
  The MRV32_enable_parity_check() function is used to enable parity check on
  the TCM and it's interface transactions. This feature is not available on
  MIV_RV32 v3.1 and MIV_RV32 v3.0.100
  @param
  This function does not take any parameters.

  @return
  This function does not return any value.
 */
static inline void MRV32_enable_parity_check(void)
{
    SUBSYS->cfg |= SUBSYS_CFG_PARITY_CHECK;
}

/***************************************************************************//**
  The MRV32_disable_parity_check() function is used to disable parity check on
  the TCM and it's interface transactions.
  @param
  This function does not take any parameters

  @return
  This function does not return any value.
  
 */
static inline void MRV32_disable_parity_check(void)
{
    SUBSYS->cfg &= ~SUBSYS_CFG_PARITY_CHECK;
}

/***************************************************************************//**
  The MRV32_cpu_soft_reset() function is used to cause a soft cpu reset on
  the MIV_RV32 soft processor core.
  @param
  This function does not take any parameters.

  @return
  This function does not return any value.
 */
static inline void MRV32_cpu_soft_reset(void)
{
    SUBSYS->soft_reg |= SUBSYS_SOFT_RESET;
}
#endif    /* MIV_RV32_SUBSYS_H */