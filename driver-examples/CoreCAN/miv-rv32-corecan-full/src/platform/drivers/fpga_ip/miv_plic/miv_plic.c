/*******************************************************************************
 * Copyright 2022 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * MI-V Soft IP fabric bare-metal driver for Mi-V PLIC module. This module is
 * delivered as a part of Mi-V Extended Sub System(MIV_ESS).
 * Please refer to miv_plic.h file for more information.
 */
 
#include "miv_plic.h"

/***************************************************************************//**
 * Mi-V PLIC interrupt handler function declaration.
 * These functions are called by the external interrupt handler of the MIV_RV32
 * core base on the PLIC source causing the interrupt.
 */
uint8_t Invalid_IRQHandler(void);
uint8_t MIV_PLIC_EXT0_IRQHandler(void);
uint8_t MIV_PLIC_EXT1_IRQHandler(void);
uint8_t MIV_PLIC_EXT2_IRQHandler(void);
uint8_t MIV_PLIC_EXT3_IRQHandler(void);
uint8_t MIV_PLIC_EXT4_IRQHandler(void);
uint8_t MIV_PLIC_EXT5_IRQHandler(void);
uint8_t MIV_PLIC_EXT6_IRQHandler(void);
uint8_t MIV_PLIC_EXT7_IRQHandler(void);
uint8_t MIV_PLIC_EXT8_IRQHandler(void);
uint8_t MIV_PLIC_EXT9_IRQHandler(void);
uint8_t MIV_PLIC_EXT10_IRQHandler(void);
uint8_t MIV_PLIC_EXT11_IRQHandler(void);
uint8_t MIV_PLIC_EXT12_IRQHandler(void);
uint8_t MIV_PLIC_EXT13_IRQHandler(void);
uint8_t MIV_PLIC_EXT14_IRQHandler(void);
uint8_t MIV_PLIC_EXT15_IRQHandler(void);
uint8_t MIV_PLIC_EXT16_IRQHandler(void);
uint8_t MIV_PLIC_EXT17_IRQHandler(void);
uint8_t MIV_PLIC_EXT18_IRQHandler(void);
uint8_t MIV_PLIC_EXT19_IRQHandler(void);
uint8_t MIV_PLIC_EXT20_IRQHandler(void);
uint8_t MIV_PLIC_EXT21_IRQHandler(void);
uint8_t MIV_PLIC_EXT22_IRQHandler(void);
uint8_t MIV_PLIC_EXT23_IRQHandler(void);
uint8_t MIV_PLIC_EXT24_IRQHandler(void);
uint8_t MIV_PLIC_EXT25_IRQHandler(void);
uint8_t MIV_PLIC_EXT26_IRQHandler(void);
uint8_t MIV_PLIC_EXT27_IRQHandler(void);
uint8_t MIV_PLIC_EXT28_IRQHandler(void);
uint8_t MIV_PLIC_EXT29_IRQHandler(void);
uint8_t MIV_PLIC_EXT30_IRQHandler(void);

/***************************************************************************//**
 * MIV_PLIC interrupt handler for external interrupts.
 * The array of the function pointers pointing to the weak handler of the Mi-V
 * PLIC interrupt handlers.
 * These functions are called by the external interrupt handler of the MIV_RV32
 * core base on the PLIC source causing the interrupt.
 */
uint8_t (* const ext_irq_handler_table[32]) (void) =
{
    Invalid_IRQHandler,
    MIV_PLIC_EXT0_IRQHandler,
    MIV_PLIC_EXT1_IRQHandler,
    MIV_PLIC_EXT2_IRQHandler,
    MIV_PLIC_EXT3_IRQHandler,
    MIV_PLIC_EXT4_IRQHandler,
    MIV_PLIC_EXT5_IRQHandler,
    MIV_PLIC_EXT6_IRQHandler,
    MIV_PLIC_EXT7_IRQHandler,
    MIV_PLIC_EXT8_IRQHandler,
    MIV_PLIC_EXT9_IRQHandler,
    MIV_PLIC_EXT10_IRQHandler,
    MIV_PLIC_EXT11_IRQHandler,
    MIV_PLIC_EXT12_IRQHandler,
    MIV_PLIC_EXT13_IRQHandler,
    MIV_PLIC_EXT14_IRQHandler,
    MIV_PLIC_EXT15_IRQHandler,
    MIV_PLIC_EXT16_IRQHandler,
    MIV_PLIC_EXT17_IRQHandler,
    MIV_PLIC_EXT18_IRQHandler,
    MIV_PLIC_EXT19_IRQHandler,
    MIV_PLIC_EXT20_IRQHandler,
    MIV_PLIC_EXT21_IRQHandler,
    MIV_PLIC_EXT22_IRQHandler,
    MIV_PLIC_EXT23_IRQHandler,
    MIV_PLIC_EXT24_IRQHandler,
    MIV_PLIC_EXT25_IRQHandler,
    MIV_PLIC_EXT26_IRQHandler,
    MIV_PLIC_EXT27_IRQHandler,
    MIV_PLIC_EXT28_IRQHandler,
    MIV_PLIC_EXT29_IRQHandler,
    MIV_PLIC_EXT30_IRQHandler
};

/* Mi-V PLIC interrupt weak handlers */
__attribute__((weak)) uint8_t Invalid_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t MIV_PLIC_EXT0_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t MIV_PLIC_EXT1_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t MIV_PLIC_EXT2_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t MIV_PLIC_EXT3_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t MIV_PLIC_EXT4_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t MIV_PLIC_EXT5_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t MIV_PLIC_EXT6_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t MIV_PLIC_EXT7_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t MIV_PLIC_EXT8_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t MIV_PLIC_EXT9_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t MIV_PLIC_EXT10_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t MIV_PLIC_EXT11_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t MIV_PLIC_EXT12_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t MIV_PLIC_EXT13_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t MIV_PLIC_EXT14_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t MIV_PLIC_EXT15_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t MIV_PLIC_EXT16_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t MIV_PLIC_EXT17_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t MIV_PLIC_EXT18_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t MIV_PLIC_EXT19_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t MIV_PLIC_EXT20_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t MIV_PLIC_EXT21_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t MIV_PLIC_EXT22_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t MIV_PLIC_EXT23_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t MIV_PLIC_EXT24_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t MIV_PLIC_EXT25_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t MIV_PLIC_EXT26_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t MIV_PLIC_EXT27_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t MIV_PLIC_EXT28_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t MIV_PLIC_EXT29_IRQHandler(void)
{
    return(0U); /* Default handler */
}

__attribute__((weak)) uint8_t MIV_PLIC_EXT30_IRQHandler(void)
{
    return(0U); /* Default handler */
}

/*-------------------------------------------------------------------------*//**
 * Please refer to miv_plic.h for more information about this function.
*/
void 
MIV_PLIC_isr
(
    miv_plic_instance_t *this_plic
)
{
    unsigned long hart_id = read_csr(mhartid);

    /* claim the interrupt from PLIC controller */

    uint32_t int_num = HAL_get_32bit_reg(this_plic->base_addr +
                                        (0x1000 * hart_id), INT_CLAIM_COMPLETE);

    uint8_t disable = EXT_IRQ_KEEP_ENABLED;
    disable = ext_irq_handler_table[int_num]();

    /* Indicate the PLIC controller that the interrupt is processed and claim is
     * complete. */
    HAL_set_32bit_reg(this_plic->base_addr +
                                (0x1000 * hart_id), INT_CLAIM_COMPLETE, int_num);

    if (EXT_IRQ_DISABLE == disable)
    {
        MIV_PLIC_disable_irq(this_plic, (miv_plic_irq_num_t)int_num);
    }
}
