/***************************************************************************//**
 * Copyright 2019-2021 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 * 
 * Legacy interrupt control functions for the Microchip driver library hardware
 * abstraction layer.
 *
 * SVN $Revision: 13158 $
 * SVN $Date: 2021-01-31 10:57:57 +0530 (Sun, 31 Jan 2021) $
 */
#include "hal.h"
#include "miv_rv32_hal/miv_rv32_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------------
 * 
 */
#define HW_get_uint32_reg(BASE_ADDR, REG_OFFSET) (*((uint32_t volatile *)(BASE_ADDR + REG_OFFSET##_REG_OFFSET)))

#define HW_set_uint32_reg(BASE_ADDR, REG_OFFSET, VALUE) (*((uint32_t volatile *)(BASE_ADDR + REG_OFFSET##_REG_OFFSET)) = (VALUE))

#define HW_set_uint32_reg_field(BASE_ADDR, FIELD, VALUE) \
            (*((uint32_t volatile *)(BASE_ADDR + FIELD##_OFFSET)) =  \
                ( \
                    (uint32_t) \
                    ( \
                    (*((uint32_t volatile *)(BASE_ADDR + FIELD##_OFFSET))) & ~FIELD##_MASK) | \
                    (uint32_t)(((VALUE) << FIELD##_SHIFT) & FIELD##_MASK) \
                ) \
            )

#define HW_get_uint32_reg_field( BASE_ADDR, FIELD ) \
            (( (*((uint32_t volatile *)(BASE_ADDR + FIELD##_OFFSET))) & FIELD##_MASK) >> FIELD##_SHIFT)

/*------------------------------------------------------------------------------
 * 32 bits memory access:
 */
#define HW_get_uint32(BASE_ADDR) (*((uint32_t volatile *)(BASE_ADDR)))

#define HW_set_uint32(BASE_ADDR, VALUE) (*((uint32_t volatile *)(BASE_ADDR)) = (VALUE))

/*------------------------------------------------------------------------------
 * 16 bits registers access:
 */
#define HW_get_uint16_reg(BASE_ADDR, REG_OFFSET) (*((uint16_t volatile *)(BASE_ADDR + REG_OFFSET##_REG_OFFSET)))

#define HW_set_uint16_reg(BASE_ADDR, REG_OFFSET, VALUE) (*((uint16_t volatile *)(BASE_ADDR + REG_OFFSET##_REG_OFFSET)) = (VALUE))

#define HW_set_uint16_reg_field(BASE_ADDR, FIELD, VALUE) \
            (*((uint16_t volatile *)(BASE_ADDR + FIELD##_OFFSET)) =  \
                ( \
                    (uint16_t) \
                    ( \
                    (*((uint16_t volatile *)(BASE_ADDR + FIELD##_OFFSET))) & ~FIELD##_MASK) | \
                    (uint16_t)(((VALUE) << FIELD##_SHIFT) & FIELD##_MASK) \
                ) \
            )

#define HW_get_uint16_reg_field( BASE_ADDR, FIELD ) \
            (( (*((uint16_t volatile *)(BASE_ADDR + FIELD##_OFFSET))) & FIELD##_MASK) >> FIELD##_SHIFT)

/*------------------------------------------------------------------------------
 * 8 bits registers access:
 */
#define HW_get_uint8_reg(BASE_ADDR, REG_OFFSET) (*((uint8_t volatile *)(BASE_ADDR + REG_OFFSET##_REG_OFFSET)))

#define HW_set_uint8_reg(BASE_ADDR, REG_OFFSET, VALUE) (*((uint8_t volatile *)(BASE_ADDR + REG_OFFSET##_REG_OFFSET)) = (VALUE))
 
#define HW_set_uint8_reg_field(BASE_ADDR, FIELD, VALUE) \
            (*((uint8_t volatile *)(BASE_ADDR + FIELD##_OFFSET)) =  \
                ( \
                    (uint8_t) \
                    ( \
                    (*((uint8_t volatile *)(BASE_ADDR + FIELD##_OFFSET))) & ~FIELD##_MASK) | \
                    (uint8_t)(((VALUE) << FIELD##_SHIFT) & FIELD##_MASK) \
                ) \
            )

#define HW_get_uint8_reg_field( BASE_ADDR, FIELD ) \
            (( (*((uint8_t volatile *)(BASE_ADDR + FIELD##_OFFSET))) & FIELD##_MASK) >> FIELD##_SHIFT)

/*------------------------------------------------------------------------------
 * 8 bits memory access:
 */
#define HW_get_uint8(BASE_ADDR) (*((uint8_t volatile *)(BASE_ADDR)))

#define HW_set_uint8(BASE_ADDR, VALUE) (*((uint8_t volatile *)(BASE_ADDR)) = (VALUE))
 
#ifdef __cplusplus
extern "C" {
#endif

#endif  /* __HW_REGISTER_MACROS_H */

