/*******************************************************************************
 * Copyright 2026 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 * @file core_i3c.c
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief CoreI3C IP bare metal driver implementation.
 *        See core_i3c.h for API description.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "core_i3c.h"
#include "corei3c_regs.h"

/*=============================================================================
 * Private constants
 *===========================================================================*/

#define NULL_INSTANCE       ((i3c_instance_t *) 0)
#define NULL_BUFFER         ((uint8_t *) 0)
#define NULL_CONST_BUFFER   ((const uint8_t *) 0)
#define NULL_LENGTH         ((uint32_t *) 0)
#define NULL_CFG            ((const i3c_cfg_t *) 0)
#define DISABLE             0u
#define ENABLE              1u

/* Soft-reset poll timeout — number of read iterations before giving up */
#define I3C_RESET_TIMEOUT_COUNT     (10000u)

/* Command ready poll timeout */
#define I3C_CMD_POLL_TIMEOUT        (100000u)

/* Maximum bytes per private command descriptor (6-bit length field) */
#define I3C_MAX_CMD_XFER_LEN        (63u)

/* Transaction ID rolls over at 4 bits */
#define I3C_TID_MAX                 (15u)

/* Poll loop iteration limit for private read/write operations */
#define I3C_PRIVATE_POLL_TIMEOUT    (5000000u)

/* Private command descriptor bit fields */
#define I3C_PRIV_CMD_SPEED_MASK     (0x00000008u)
#define I3C_PRIV_CMD_RD_FLAG        (0x20000000u)
#define I3C_PRIV_CMD_DEV_IDX_SHIFT  (16u)
#define I3C_PRIV_CMD_LEN_SHIFT      (16u)

/*******************************************************************************
 * I3C_init_controller()
 * See "core_i3c.h" file for details of how to use this function.
 */
i3c_status_t
I3C_init_controller
(
    i3c_instance_t       * this_i3c,
    addr_t                 base_addr,
    uint8_t                ser_address,
    const i3c_cfg_t      * cfg
)
{
    i3c_status_t status = I3C_ERR_PARAM;

    HAL_ASSERT( this_i3c != NULL_INSTANCE );
    HAL_ASSERT( base_addr != 0u );
    HAL_ASSERT( cfg != NULL_CFG );

    if( (this_i3c != NULL_INSTANCE) && (base_addr != 0u) && (cfg != NULL_CFG) )
    {
        uint32_t reg_val;

        /* Store instance fields */
        this_i3c->base_addr   = base_addr;
        this_i3c->ibi_capable = cfg->ibi_capable;
        this_i3c->tid         = 0u;

        /* Disable bus before reset */
        reg_val  = HAL_get_32bit_reg( this_i3c->base_addr, COREI3C_I3C_CNTRL );
        reg_val &= ~COREI3C_I3C_CNTRL_BUS_ENABLE_MASK;
        HAL_set_32bit_reg( this_i3c->base_addr, COREI3C_I3C_CNTRL, reg_val );

        /* Assert soft reset — auto-clears on completion */
        HAL_set_32bit_reg( this_i3c->base_addr, COREI3C_RESET_CNTRL,
                           COREI3C_RESET_CNTRL_SOFT_RST_MASK );

        /* Poll until SOFT_RST auto-clears */
        uint32_t timeout = I3C_RESET_TIMEOUT_COUNT;
        while( timeout > 0u )
        {
            reg_val = HAL_get_32bit_reg( this_i3c->base_addr,
                                         COREI3C_RESET_CNTRL );
            if( 0u == (reg_val & COREI3C_RESET_CNTRL_SOFT_RST_MASK) )
            {
                break;
            }
            --timeout;
        }

        if( 0u == timeout )
        {
            status = I3C_ERR_TIMEOUT;
        }

        /* Reset all FIFOs */
        if( I3C_ERR_TIMEOUT != status )
        {
            HAL_set_32bit_reg( this_i3c->base_addr, COREI3C_RESET_CNTRL,
                               COREI3C_RESET_CNTRL_ALL_FIFOS_MASK );

            /* Poll until all FIFO resets auto-clear */
            timeout = I3C_RESET_TIMEOUT_COUNT;
            while( timeout > 0u )
            {
                reg_val = HAL_get_32bit_reg( this_i3c->base_addr,
                                             COREI3C_RESET_CNTRL );
                if( 0u == (reg_val & COREI3C_RESET_CNTRL_ALL_FIFOS_MASK) )
                {
                    break;
                }
                --timeout;
            }

            if( 0u == timeout )
            {
                status = I3C_ERR_TIMEOUT;
            }
        }

        /* Set controller dynamic address */
        {
            uint32_t static_addr = 0x0u;
            reg_val = COREI3C_CNTRL_DEVICE_ADDR_VALID_MASK |
                      (((uint32_t)(ser_address) &
                        COREI3C_CNTRL_DEVICE_ADDR_DYNADDR_NS_MASK)
                       << COREI3C_CNTRL_DEVICE_ADDR_DYNADDR_SHIFT) |
                      ((uint32_t)static_addr &
                       COREI3C_CNTRL_DEVICE_ADDR_DYNADDR_NS_MASK);

            HAL_set_32bit_reg( this_i3c->base_addr,
                               COREI3C_CNTRL_DEV_ADDR, reg_val );
        }

        /* Program SCL open-drain timing */
        reg_val = ((uint32_t)(cfg->scl_od_high_cycles) <<
                   COREI3C_SCL_OD_HIGH_SHIFT) |
                  ((uint32_t)(cfg->scl_od_low_cycles) <<
                   COREI3C_SCL_OD_LOW_SHIFT);
        HAL_set_32bit_reg( this_i3c->base_addr, COREI3C_SCL_OD_TIME, reg_val );

        /* Program SCL push-pull timing */
        reg_val = ((uint32_t)(cfg->scl_pp_high_cycles) <<
                   COREI3C_SCL_PP_HIGH_SHIFT) |
                  ((uint32_t)(cfg->scl_pp_low_cycles) <<
                   COREI3C_SCL_PP_LOW_SHIFT);
        HAL_set_32bit_reg( this_i3c->base_addr, COREI3C_SCL_PP_TIME, reg_val );

        /* Enable CMD RESPNS interrupt status */
        reg_val = COREI3C_CMND_RSPNS_ALL_STAT_MASK;
        HAL_set_32bit_reg( this_i3c->base_addr,
                           COREI3C_CR_INTR_STAT_EN, reg_val );

        /* Write DAT entries from init table */
        reg_val = COREI3C_DAT1_INIT_WORD0;
        HAL_set_32bit_reg( this_i3c->base_addr,
                           COREI3C_DAT_BASE_OFFSET, reg_val );

        reg_val = COREI3C_DAT0_INIT_WORD1;
        HAL_set_32bit_reg( this_i3c->base_addr,
                           COREI3C_DAT_BASE_OFFSET1, reg_val );

        status = I3C_SUCCESS;
    }

    return (status);
}

/*******************************************************************************
 * I3C_wait_cmd_complete()
 * See "core_i3c.h" file for details of how to use this function.
 */
i3c_status_t
I3C_wait_cmd_complete
(
    i3c_instance_t * this_i3c,
    uint32_t timeout
)
{
    uint32_t count = 0u;
    i3c_status_t result = I3C_ERR_TIMEOUT;

    HAL_ASSERT( this_i3c != NULL_INSTANCE );

    if( this_i3c == NULL_INSTANCE )
    {
        return I3C_ERR_PARAM;
    }

    while( count < timeout )
    {
        uint32_t status = HAL_get_32bit_reg( this_i3c->base_addr,
                                             COREI3C_CR_INTR_STAT );

        if( (status & COREI3C_CR_STAT_RESP_RDY_MASK) != 0u )
        {
            /*
             * Clear the CMND_RDY_STAT bit by writing 1 to it (W1C).
             */
            HAL_set_32bit_reg( this_i3c->base_addr, COREI3C_CR_INTR_STAT,
                               COREI3C_CR_STAT_RESP_RDY_MASK );

            /*
             * Check for transfer error or abort.
             */
            if( (status & COREI3C_CR_STAT_XFER_ERR_MASK) != 0u )
            {
                HAL_set_32bit_reg( this_i3c->base_addr,
                                   COREI3C_CR_INTR_STAT,
                                   COREI3C_CR_STAT_XFER_ERR_MASK );
                result = I3C_ERR_FIFO_FULL;
            }
            else if( (status & COREI3C_CR_STAT_XFER_ABRT_MASK) != 0u )
            {
                HAL_set_32bit_reg( this_i3c->base_addr,
                                   COREI3C_CR_INTR_STAT,
                                   COREI3C_CR_STAT_XFER_ABRT_MASK );
                result = I3C_ERR_ABORTED;
            }
            else
            {
                result = I3C_SUCCESS;
            }

            if( (status & COREI3C_CMND_RSPNS_INTR_STAT_WRT_MASK) != 0u )
            {
                HAL_set_32bit_reg( this_i3c->base_addr,
                                   COREI3C_CR_INTR_STAT,
                                   COREI3C_CMND_RSPNS_INTR_STAT_WRT_MASK );
            }

            break;
        }

        count++;
    }

    return result;
}

/*******************************************************************************
 * I3C_drain_all_fifos()
 *
 * Full FIFO cleanup before raw_private_read().
 *
 * This is the comprehensive drain that should be called before any
 * raw_private_read() sequence.  It handles:
 *
 *   1. STATUS_FIFO drain (RSPNS_RDY_STAT entries from prior commands)
 *   2. IBI residue drain (IBI_RECV_STAT + IBI_PORT + IBI data in RD_FIFO)
 *   3. RD_FIFO flush (stale data bytes from prior reads or IBI payloads)
 *   4. Clear all W1C bits in CMND_RSPNS_INTR_STAT
 *
 * IBI residue is the primary source of FIFO corruption across TIDs.
 * When an IBI arrives (e.g. during TID-8), the controller places:
 *   - An IBI descriptor in STATUS_FIFO
 *   - IBI address + MDB in IBI_PORT
 *   - Optional IBI payload bytes in RD_FIFO
 *   - Sets IBI_RECV_STAT (bit 2) in CMND_RSPNS_INTR_STAT
 * If not fully consumed, this residue offsets subsequent raw_private_read()
 * data, causing sensor ID mismatches in TID-6, 11, 13, 14.
 *
 * @param this_i3c    Pointer to I3C instance.
 * @param max_drain   Maximum STATUS_FIFO entries to drain.
 * @param rd_flush    Number of RD_FIFO words to flush.
 */
void
I3C_drain_all_fifos
(
    i3c_instance_t  * this_i3c,
    uint8_t max_drain,
    uint16_t rd_flush
)
{
    uint8_t n;
    uint32_t stat;

    HAL_ASSERT( this_i3c != NULL_INSTANCE );

    if( this_i3c == NULL_INSTANCE )
    {
        return;
    }

    /* Drain STATUS_FIFO (RSPNS_RDY_STAT entries) */
    for( n = 0u; n < max_drain; n++ )
    {
        stat = HAL_get_32bit_reg( this_i3c->base_addr,
                                  COREI3C_CMND_RSPNS_INTR_STAT );

        if( (stat & COREI3C_CMND_RSPNS_INTR_STAT_WRT_MASK) == 0u )
        {
            break;
        }

        (void)HAL_get_32bit_reg( this_i3c->base_addr,
                                 COREI3C_I3C_STATUS_FIFO );
        HAL_set_32bit_reg( this_i3c->base_addr, COREI3C_STATUS_FIFO,
                           COREI3C_CMND_RSPNS_INTR_STAT_WRT_MASK );
    }

    /* Drain IBI residue if IBI_RECV_STAT (bit 2) is set */
    stat = HAL_get_32bit_reg( this_i3c->base_addr,
                              COREI3C_CMND_RSPNS_INTR_STAT );

    if( (stat & COREI3C_CMND_RSPNS_INTR_STAT_EN_IBI_RECV_MASK) != 0u )
    {
        /* Read IBI_PORT to consume the IBI descriptor */
        (void)HAL_get_32bit_reg( this_i3c->base_addr, COREI3C_IBI_PORT );

        /* IBI may have left payload bytes in RD_FIFO — flush extra */
        rd_flush += 4u;

        /* Clear only IBI_RECV_STAT (W1C) — do NOT touch CMND_RDY_STAT
         * or RSPNS_RDY_STAT as they reflect live hardware state */
        HAL_set_32bit_reg( this_i3c->base_addr,
                           COREI3C_CMND_RSPNS_INTR_STAT,
                           COREI3C_CMND_RSPNS_INTR_STAT_RD_MASK );
    }

    /* Flush RD_FIFO (stale data bytes).
     * Only read RD_FIFO while RD_STAT (bit 0) is set.  Reading an empty
     * Fabric FIFO (FIFO_TYPE=0) stalls the AXI4 read channel because
     * RVALID never asserts, hanging the firmware.  Guard every read with
     * an RD_STAT check to avoid this. */
    {
        uint16_t f;
        for( f = 0u; f < rd_flush; f++ )
        {
            uint32_t rd_chk = HAL_get_32bit_reg( this_i3c->base_addr,
                                                 COREI3C_CMND_RSPNS_INTR_STAT );
            if( (rd_chk & COREI3C_CMND_RSPNS_INTR_STAT_EN_RD_MASK) == 0u )
            {
                break;  /* RD_FIFO empty — stop flushing */
            }
            (void)HAL_get_32bit_reg( this_i3c->base_addr,
                                     COREI3C_I3C_RD_FIFO );
            HAL_set_32bit_reg( this_i3c->base_addr,
                               COREI3C_CMND_RSPNS_INTR_STAT,
                               COREI3C_CMND_RSPNS_INTR_STAT_RD_MASK );
        }
    }
}

/*******************************************************************************
 * I3C_private_read()
 *
 * Perform a two-phase private read transaction:
 *   Phase 1: Write the target register address.
 *   Phase 2: Read N data bytes from the target.
 *
 * @param this_i3c    Pointer to I3C instance.
 * @param dct_index   Device index in the DAT table.
 * @param reg_addr    Target register address to read from.
 * @param p_data      Pointer to buffer to store read data.
 * @param num_bytes   Number of bytes to read.
 * @return            I3C_SUCCESS or I3C_ERR_TIMEOUT / I3C_ERR_PARAM.
 */
i3c_status_t
I3C_private_read
(
    i3c_instance_t  * this_i3c,
    uint8_t dct_index,
    uint8_t reg_addr,
    uint8_t * p_data,
    uint8_t num_bytes
)
{
    uint32_t cmd_w0_wr;
    uint32_t cmd_w0_rd;
    uint32_t cmd_w1_rd;
    uint32_t poll_count;

    HAL_ASSERT( this_i3c != NULL_INSTANCE );
    HAL_ASSERT( p_data != NULL_BUFFER );

    if( (this_i3c == NULL_INSTANCE) || (p_data == NULL_BUFFER) )
    {
        return I3C_ERR_PARAM;
    }

    cmd_w0_wr = I3C_PRIV_CMD_SPEED_MASK |
                ((uint32_t)dct_index << I3C_PRIV_CMD_DEV_IDX_SHIFT);
    cmd_w0_rd = I3C_PRIV_CMD_RD_FLAG |
                I3C_PRIV_CMD_SPEED_MASK |
                ((uint32_t)dct_index << I3C_PRIV_CMD_DEV_IDX_SHIFT);
    cmd_w1_rd = (uint32_t)num_bytes << I3C_PRIV_CMD_LEN_SHIFT;

    /* Phase 1: Private write - send register address */
    HAL_set_32bit_reg( this_i3c->base_addr, COREI3C_I3C_WRT_FIFO,
                       (uint32_t)reg_addr );
    HAL_set_32bit_reg( this_i3c->base_addr, COREI3C_I3C_CMND_FIFO,
                       cmd_w0_wr );
    HAL_set_32bit_reg( this_i3c->base_addr, COREI3C_I3C_CMND_FIFO,
                       0x00010000u ); /* 1 byte */

    /* Wait for Phase 1 write completion by polling CMND_RDY_STAT (bit 3).
     * A blind raw_delay is not reliable — the I3C bus transaction takes
     * variable time.  Issuing Phase 2 before Phase 1 completes corrupts
     * CMND_FIFO state and hangs the firmware. */
    for( poll_count = 0u; poll_count < I3C_PRIVATE_POLL_TIMEOUT;
         poll_count++ )
    {
        uint32_t ph1_stat = HAL_get_32bit_reg( this_i3c->base_addr,
                                COREI3C_CMND_RSPNS_INTR_STAT );
        if( (ph1_stat & COREI3C_CMND_RSPNS_INTR_STAT_CMD_RDY_MASK) != 0u )
        {
            /* Clear CMND_RDY_STAT (W1C) */
            HAL_set_32bit_reg( this_i3c->base_addr,
                               COREI3C_CMND_RSPNS_INTR_STAT,
                               COREI3C_CMND_RSPNS_INTR_STAT_CMD_RDY_MASK );
            break;
        }
    }

    if( poll_count >= I3C_PRIVATE_POLL_TIMEOUT )
    {
        return I3C_ERR_TIMEOUT;
    }

    /* Intra-read drain: consume Phase 1 write-completion residue from
     * STATUS_FIFO and RD_FIFO BEFORE issuing the Phase 2 read command.
     * Without this, the RTL's write-phase STATUS_FIFO entry can leave
     * stale bytes in RD_FIFO that offset the subsequent read data
     * (e.g. LPS22DFTR WHO_AM_I returns 0x02 instead of 0xB4). */
    {
        uint8_t drain_n;

        /* Drain STATUS_FIFO entries from Phase 1 completion */
        for( drain_n = 0u; drain_n < 4u; drain_n++ )
        {
            uint32_t drain_stat = HAL_get_32bit_reg( this_i3c->base_addr,
                                      COREI3C_CMND_RSPNS_INTR_STAT );
            if( (drain_stat &
                 COREI3C_CMND_RSPNS_INTR_STAT_RESP_RDY_MASK) == 0u )
            {
                break;
            }
            (void)HAL_get_32bit_reg( this_i3c->base_addr,
                                     COREI3C_STATUS_FIFO );
            HAL_set_32bit_reg( this_i3c->base_addr,
                               COREI3C_CMND_RSPNS_INTR_STAT,
                               COREI3C_CMND_RSPNS_INTR_STAT_RESP_RDY_MASK );
        }

        /* Flush any stale RD_FIFO words left by Phase 1 — but only if
         * RD_STAT (bit 0) is set.  A private write never puts data in
         * RD_FIFO; reading an empty FIFO when FIFO_TYPE=0 (Fabric FIFO)
         * stalls the AXI4 read transaction because RVALID never asserts,
         * hanging the firmware.  Guard the flush so it only runs when
         * data is genuinely present. */
        {
            uint32_t rd_stat_val = HAL_get_32bit_reg( this_i3c->base_addr,
                                       COREI3C_CMND_RSPNS_INTR_STAT );
            if( (rd_stat_val &
                 COREI3C_CMND_RSPNS_INTR_STAT_RD_MASK) != 0u )
            {
                (void)HAL_get_32bit_reg( this_i3c->base_addr,
                                         COREI3C_RD_FIFO );
                /* Clear RD_STAT (W1C) */
                HAL_set_32bit_reg( this_i3c->base_addr,
                                   COREI3C_CMND_RSPNS_INTR_STAT,
                                   COREI3C_CMND_RSPNS_INTR_STAT_RD_MASK );
            }
        }
    }

    /* Phase 2: Private read - read N data bytes */
    HAL_set_32bit_reg( this_i3c->base_addr, COREI3C_I3C_CMND_FIFO,
                       cmd_w0_rd );
    HAL_set_32bit_reg( this_i3c->base_addr, COREI3C_I3C_CMND_FIFO,
                       cmd_w1_rd );

    /* Wait for RD_STAT (bit 0) to confirm read data is in RD_FIFO before
     * reading it.  A blind raw_delay risks reading an empty FIFO if the
     * I3C bus transaction has not yet completed, which stalls the AXI4
     * read channel (RVALID never asserts) and hangs the firmware. */
    for( poll_count = 0u; poll_count < I3C_PRIVATE_POLL_TIMEOUT;
         poll_count++ )
    {
        uint32_t rd_stat = HAL_get_32bit_reg( this_i3c->base_addr,
                               COREI3C_CMND_RSPNS_INTR_STAT );
        if( (rd_stat & COREI3C_CMND_RSPNS_INTR_STAT_RD_MASK) != 0u )
        {
            break;
        }
    }

    if( poll_count >= I3C_PRIVATE_POLL_TIMEOUT )
    {
        return I3C_ERR_TIMEOUT;
    }

    /* Read data from RD_FIFO.
     * RWIDTH=32: each 32-bit read returns up to 4 bytes packed little-endian.
     * Read one 32-bit word per 4 bytes, extract each byte individually. */
    {
        uint8_t i;
        uint32_t rd_word = 0u;
        for( i = 0u; i < num_bytes; i++ )
        {
            if( (i % 4u) == 0u )
            {
                rd_word = HAL_get_32bit_reg( this_i3c->base_addr,
                                             COREI3C_I3C_RD_FIFO );
            }
            p_data[i] = (uint8_t)((rd_word >> ((i % 4u) * 8u)) & 0xFFu);
        }
    }

    /* Clear RD_STAT (W1C) after draining RD_FIFO */
    HAL_set_32bit_reg( this_i3c->base_addr, COREI3C_CMND_RSPNS_INTR_STAT,
                       COREI3C_CMND_RSPNS_INTR_STAT_RD_MASK );

    return I3C_SUCCESS;
}

/*******************************************************************************
 * I3C_private_write()
 *
 * Perform a private write transaction: write a register address followed
 * by data bytes to the target device.
 *
 * @param this_i3c    Pointer to I3C instance.
 * @param dct_index   Device index in the DAT table.
 * @param reg_addr    Target register address to write to.
 * @param p_data      Pointer to data bytes to write.
 * @param num_bytes   Number of data bytes to write.
 * @return            I3C_SUCCESS or error code.
 */
i3c_status_t
I3C_private_write
(
    i3c_instance_t  * this_i3c,
    uint8_t dct_index,
    uint8_t reg_addr,
    const uint8_t * p_data,
    uint8_t num_bytes
)
{
    uint32_t cmd_w0;
    uint32_t total_len;
    uint32_t cmd_w1;

    HAL_ASSERT( this_i3c != NULL_INSTANCE );
    HAL_ASSERT( p_data != NULL_CONST_BUFFER );

    if( (this_i3c == NULL_INSTANCE) || (p_data == NULL_CONST_BUFFER) )
    {
        return I3C_ERR_PARAM;
    }

    cmd_w0 = I3C_PRIV_CMD_SPEED_MASK |
             ((uint32_t)dct_index << I3C_PRIV_CMD_DEV_IDX_SHIFT);
    total_len = (uint32_t)num_bytes + 1u; /* reg_addr + data bytes */
    cmd_w1 = total_len << I3C_PRIV_CMD_LEN_SHIFT;

    /* Load WRT_FIFO: register address first, then data bytes.
     * WWIDTH=32: pack up to 4 bytes per 32-bit write, little-endian.
     * Payload = { reg_addr, p_data[0], p_data[1], ... } packed sequentially. */
    {
        uint32_t  wr_word = 0u;
        uint8_t   byte_idx = 0u;
        uint16_t  total = (uint16_t)num_bytes + 1u; /* reg_addr + data bytes */
        uint16_t  b;

        for( b = 0u; b < total; b++ )
        {
            uint8_t byte_val = (b == 0u) ? reg_addr : p_data[b - 1u];
            wr_word |= ((uint32_t)byte_val << (byte_idx * 8u));
            byte_idx++;
            if( (byte_idx == 4u) || (b == (total - 1u)) )
            {
                HAL_set_32bit_reg( this_i3c->base_addr,
                                   COREI3C_I3C_WRT_FIFO, wr_word );
                wr_word  = 0u;
                byte_idx = 0u;
            }
        }
    }

    /* Issue command */
    HAL_set_32bit_reg( this_i3c->base_addr, COREI3C_I3C_CMND_FIFO, cmd_w0 );
    HAL_set_32bit_reg( this_i3c->base_addr, COREI3C_I3C_CMND_FIFO, cmd_w1 );

    /* Wait for command completion by polling CMND_RDY_STAT (bit 3).
     * A blind raw_delay is not sufficient — the I3C bus transaction
     * (arbitration + bit-clocking + ACK) takes variable time depending
     * on SCL frequency and slave response.  Issuing the next command
     * before this one completes corrupts the CMND_FIFO state and hangs
     * the firmware.  Poll with a safety timeout to avoid infinite loops
     * on bus errors. */
    {
        uint32_t poll_count;
        uint32_t wr_stat;
        for( poll_count = 0u; poll_count < I3C_PRIVATE_POLL_TIMEOUT;
             poll_count++ )
        {
            wr_stat = HAL_get_32bit_reg( this_i3c->base_addr,
                                         COREI3C_CMND_RSPNS_INTR_STAT );
            if( (wr_stat &
                 COREI3C_CMND_RSPNS_INTR_STAT_CMD_RDY_MASK) != 0u )
            {
                /* Clear CMND_RDY_STAT (W1C) */
                HAL_set_32bit_reg( this_i3c->base_addr,
                                   COREI3C_CMND_RSPNS_INTR_STAT,
                                   COREI3C_CMND_RSPNS_INTR_STAT_CMD_RDY_MASK );
                break;
            }
        }

        if( poll_count >= I3C_PRIVATE_POLL_TIMEOUT )
        {
            return I3C_ERR_TIMEOUT;
        }

        /* Drain STATUS_FIFO entry left by the completed write command */
        wr_stat = HAL_get_32bit_reg( this_i3c->base_addr,
                                     COREI3C_CMND_RSPNS_INTR_STAT );
        if( (wr_stat & COREI3C_CMND_RSPNS_INTR_STAT_WRT_MASK) != 0u )
        {
            (void)HAL_get_32bit_reg( this_i3c->base_addr,
                                     COREI3C_I3C_STATUS_FIFO );
            HAL_set_32bit_reg( this_i3c->base_addr,
                               COREI3C_CMND_RSPNS_INTR_STAT,
                               COREI3C_CMND_RSPNS_INTR_STAT_WRT_MASK );
        }
    }

    return I3C_SUCCESS;
}

/*******************************************************************************
 * I3C_do_daa()
 * See "core_i3c.h" file for details of how to use this function.
 */
i3c_status_t
I3C_do_daa
(
    i3c_instance_t  * this_i3c,
    i3c_address_t   * addr_list,
    uint32_t          max_devices
)
{
    i3c_status_t status = I3C_ERR_NOT_CTRL;

    HAL_ASSERT( this_i3c != NULL_INSTANCE );

    /* Suppress unused parameter warnings — reserved for future use */
    (void)addr_list;
    (void)max_devices;

    if( this_i3c != NULL_INSTANCE )
    {
        uint32_t cmd;
        uint32_t cmd1;
        uint32_t reg_val;
        uint32_t dev_count = 1u;

        /* Enable bus */
        reg_val  = HAL_get_32bit_reg( this_i3c->base_addr, COREI3C_CNTRL );
        reg_val |= COREI3C_I3C_CNTRL_BUS_ENABLE_MASK;
        HAL_set_32bit_reg( this_i3c->base_addr, COREI3C_CNTRL, reg_val );

        /*
         * Build the broadcast CCC command descriptor.
         *
         * Word 0: CCC_FLAG | BCAST | CCC(code) | TID(3) | CMD_ATTR_TRANSFER
         * Word 1: DATA_LEN
         *
         * Verified: ENEC(0x00) produces 0x80008018, DISEC(0x01) produces
         * 0x80008098 which match proven reference firmware values.
         */
        cmd = I3C_CMD_CCC_FLAG |
              I3C_CMD_BCAST |
              I3C_CMD_CCC(0) |
              I3C_CMD_TID(3u) |
              I3C_CMD_ATTR_TRANSFER;
        cmd1 = I3C_CMD_DATA_LEN(0);
        HAL_set_32bit_reg( this_i3c->base_addr, COREI3C_CMND_FIFO, cmd );
        HAL_set_32bit_reg( this_i3c->base_addr, COREI3C_CMND_FIFO, cmd1 );

        /* Poll for command completion (CMD_RDY bit) */
        status = I3C_wait_cmd_complete( this_i3c, I3C_CMD_POLL_TIMEOUT );

        if( status != I3C_SUCCESS )
        {
            return status;
        }

        /* DAT write */
        reg_val = COREI3C_DAT1_INIT_WORD0;
        HAL_set_32bit_reg( this_i3c->base_addr,
                           COREI3C_DAT_BASE_OFFSET, reg_val );
        HAL_set_32bit_reg( this_i3c->base_addr,
                           COREI3C_DAT_BASE_OFFSET1, 0x0u );

        /* Set broadcast RSTDAA */
        cmd = I3C_CMD_CCC_FLAG |
              I3C_CMD_BCAST |
              I3C_CMD_CCC(I3C_CCC_RSTDAA) |
              I3C_CMD_TID(2u) |
              I3C_CMD_ATTR_TRANSFER;

        /* Enable bus */
        reg_val  = HAL_get_32bit_reg( this_i3c->base_addr, COREI3C_CNTRL );
        reg_val |= COREI3C_I3C_CNTRL_BUS_ENABLE_MASK;
        HAL_set_32bit_reg( this_i3c->base_addr, COREI3C_CNTRL, reg_val );

        HAL_set_32bit_reg( this_i3c->base_addr, COREI3C_CMND_FIFO, cmd );
        HAL_set_32bit_reg( this_i3c->base_addr, COREI3C_CMND_FIFO, 0x0u );

        /* Poll for command completion (CMD_RDY bit) */
        status = I3C_wait_cmd_complete( this_i3c, I3C_CMD_POLL_TIMEOUT );

        if( status != I3C_SUCCESS )
        {
            return status;
        }

        /* Bus disable */
        HAL_set_32bit_reg( this_i3c->base_addr, COREI3C_CNTRL, 0x0u );

        /* Build and submit ENTDAA command */
        HAL_set_32bit_reg( this_i3c->base_addr, COREI3C_SCL_PP_TIME,
                           ((uint32_t)0x6u << 16u) | (uint32_t)0x005u );
        cmd = I3C_CMD_DEV_CNT(dev_count) |
              I3C_CMD_CCC(I3C_CCC_ENTDAA) |
              I3C_CMD_TID(1u) |
              I3C_CMD_ATTR_ADDR_ASSIGN;

        /* Write command descriptor to CMND_FIFO (two 32-bit words). */
        HAL_set_32bit_reg( this_i3c->base_addr, COREI3C_CMND_FIFO, cmd );
        HAL_set_32bit_reg( this_i3c->base_addr, COREI3C_CMND_FIFO, 0x0u );

        /* Enable bus */
        reg_val  = HAL_get_32bit_reg( this_i3c->base_addr, COREI3C_CNTRL );
        reg_val |= COREI3C_I3C_CNTRL_BUS_ENABLE_MASK;
        HAL_set_32bit_reg( this_i3c->base_addr, COREI3C_CNTRL, reg_val );

        /* Wait for DAA completion */
        status = I3C_wait_cmd_complete( this_i3c, I3C_CMD_POLL_TIMEOUT );

        /* Restore fast PP for normal SDR transfers */
        HAL_set_32bit_reg( this_i3c->base_addr, COREI3C_SCL_PP_TIME,
                           ((uint32_t)3u << 16u) | (uint32_t)4u );
    }

    return status;
}

/*******************************************************************************
 * I3C_abort()
 * See "core_i3c.h" file for details of how to use this function.
 */
i3c_status_t
I3C_abort
(
    i3c_instance_t * this_i3c
)
{
    i3c_status_t status = I3C_ERR_PARAM;

    HAL_ASSERT( this_i3c != NULL_INSTANCE );

    if( this_i3c != NULL_INSTANCE )
    {
        uint32_t reg_val = HAL_get_32bit_reg( this_i3c->base_addr,
                                              COREI3C_CNTRL );
        reg_val |= COREI3C_I3C_CNTRL_ABORT_MASK;
        HAL_set_32bit_reg( this_i3c->base_addr, COREI3C_CNTRL, reg_val );
        status = I3C_SUCCESS;
    }
    return (status);
}

/*******************************************************************************
 * I3C_resume()
 * See "core_i3c.h" file for details of how to use this function.
 */
i3c_status_t
I3C_resume
(
    i3c_instance_t * this_i3c
)
{
    i3c_status_t status = I3C_ERR_PARAM;
    HAL_ASSERT( this_i3c != NULL_INSTANCE );
    if( this_i3c != NULL_INSTANCE )
    {
        /* Write 1 to RESUME bit (RW1C) — auto-clears once core resumes */
        uint32_t reg_val = HAL_get_32bit_reg( this_i3c->base_addr,
                                              COREI3C_CNTRL );
        reg_val |= COREI3C_I3C_CNTRL_RESUME_MASK;
        HAL_set_32bit_reg( this_i3c->base_addr, COREI3C_CNTRL, reg_val );
        status = I3C_SUCCESS;
    }
    return (status);
}

/*******************************************************************************
 * I3C_write_ccc()
 * See "core_i3c.h" file for details of how to use this function.
 */
i3c_status_t
I3C_write_ccc
(
    i3c_instance_t       * this_i3c,
    uint8_t                serial_addr,
    uint8_t                ccc_byte,
    const uint8_t        * write_buffer,
    uint16_t               write_size,
    uint32_t               options
)
{
    i3c_status_t status = I3C_ERR_PARAM;
    HAL_ASSERT( this_i3c != NULL_INSTANCE );
    if( this_i3c != NULL_INSTANCE )
    {
        /* Must be active controller */
        uint32_t state = HAL_get_32bit_reg( this_i3c->base_addr,
                                            COREI3C_PRESENT_STATE );
        if( (state & COREI3C_PRESENT_STATE_CURRENT_CTRL_MASK) != 0u )
        {
            uint32_t cmd;
            uint8_t  tid;
            uint8_t  is_broadcast =
                (serial_addr == I3C_BROADCAST_ADDR) ? 1u : 0u;

            /* Increment rolling TID */
            tid = this_i3c->tid;
            this_i3c->tid = (this_i3c->tid >= I3C_TID_MAX) ? 0u :
                            (this_i3c->tid + 1u);

            /* Write payload bytes into Write FIFO (32-bit packing,
             * little-endian) */
            if( (write_buffer != NULL_CONST_BUFFER) && (write_size > 0u) )
            {
                uint16_t i = 0u;
                /* Full 32-bit words */
                while( (write_size - i) >= 4u )
                {
                    uint32_t dword;
                    dword = ((uint32_t)write_buffer[i])            |
                            ((uint32_t)write_buffer[i + 1u] << 8u) |
                            ((uint32_t)write_buffer[i + 2u] << 16u)|
                            ((uint32_t)write_buffer[i + 3u] << 24u);
                    HAL_set_32bit_reg( this_i3c->base_addr,
                                       COREI3C_WRT_FIFO, dword );
                    i += 4u;
                }
                /* Remaining bytes */
                if( i < write_size )
                {
                    uint32_t dword = 0u;
                    switch( write_size - i )
                    {
                        case 3u:
                            dword |= ((uint32_t)write_buffer[i + 2u] << 16u);
                            /* fallthrough */
                        case 2u:
                            dword |= ((uint32_t)write_buffer[i + 1u] << 8u);
                            /* fallthrough */
                        case 1u:
                            dword |= ((uint32_t)write_buffer[i]);
                            break;
                        default:
                            break;
                    }
                    HAL_set_32bit_reg( this_i3c->base_addr,
                                       COREI3C_WRT_FIFO, dword );
                }
            }

            /* Build CCC command descriptor */
            cmd  = ((uint32_t)(ccc_byte & COREI3C_CCC_CODE_NS_MASK)
                    << COREI3C_CCC_CODE_SHIFT);
            if( is_broadcast != 0u )
            {
                cmd |= COREI3C_CCC_BROADCAST_MASK;
            }
            else
            {
                cmd |= ((uint32_t)(serial_addr & COREI3C_CCC_TGT_ADDR_NS_MASK)
                        << COREI3C_CCC_TGT_ADDR_SHIFT);
            }
            cmd |= ((uint32_t)((write_size &
                    (uint16_t)COREI3C_CCC_DATA_LEN_NS_MASK))
                    << COREI3C_CCC_DATA_LEN_SHIFT);

            cmd |= ((uint32_t)(tid & COREI3C_CMD_TID_NS_MASK)
                    << COREI3C_CMD_TID_SHIFT);

            if( (options & I3C_OPT_RESPONSE) != 0u )
            {
                cmd |= COREI3C_CMD_ROC_MASK;
            }

            if( (options & I3C_OPT_EMIT_STOP) != 0u )
            {
                cmd |= COREI3C_CMD_TOC_MASK;
            }

            cmd |= COREI3C_CMD_CP_MASK;

            HAL_set_32bit_reg( this_i3c->base_addr,
                               COREI3C_CMND_FIFO, cmd );

            status = I3C_SUCCESS;
        }
        else
        {
            status = I3C_ERR_NOT_CTRL;
        }
    }

    return (status);
}

/*******************************************************************************
 * I3C_read_ccc()
 * See "core_i3c.h" file for details of how to use this function.
 */
i3c_status_t
I3C_read_ccc
(
    i3c_instance_t  * this_i3c,
    uint8_t           serial_addr,
    uint8_t           ccc_byte,
    uint8_t           defining_byte,
    uint8_t         * read_buffer,
    uint16_t          read_size,
    uint32_t        * read_len,
    uint32_t          options
)
{
    i3c_status_t status = I3C_ERR_PARAM;

    HAL_ASSERT( this_i3c != NULL_INSTANCE );
    HAL_ASSERT( read_buffer != NULL_BUFFER );
    HAL_ASSERT( read_len != NULL_LENGTH );

    if( (this_i3c != NULL_INSTANCE)   &&
        (read_buffer != NULL_BUFFER)   &&
        (read_len != NULL_LENGTH) )
    {
        /* Must be active controller */
        uint32_t state = HAL_get_32bit_reg( this_i3c->base_addr,
                                            COREI3C_PRESENT_STATE );
        if( (state & COREI3C_PRESENT_STATE_CURRENT_CTRL_MASK) != 0u )
        {
            uint32_t cmd;
            uint32_t reg_val;
            uint32_t timeout;
            uint8_t  tid;

            *read_len = 0u;

            /* Increment rolling TID */
            tid = this_i3c->tid;
            this_i3c->tid = (this_i3c->tid >= I3C_TID_MAX) ? 0u :
                            (this_i3c->tid + 1u);
            /*
             * If a defining byte is needed, push it into the Write FIFO
             * first — the core sends it as the first write byte of the
             * combined CCC.
             */
            if( defining_byte != 0u )
            {
                HAL_set_32bit_reg( this_i3c->base_addr, COREI3C_WRT_FIFO,
                                   (uint32_t)defining_byte );
            }

            /* Build direct read CCC command descriptor */
            cmd  = ((uint32_t)(ccc_byte & COREI3C_CCC_CODE_NS_MASK)
                    << COREI3C_CCC_CODE_SHIFT);
            cmd |= ((uint32_t)(serial_addr & COREI3C_CCC_TGT_ADDR_NS_MASK)
                    << COREI3C_CCC_TGT_ADDR_SHIFT);
            cmd |= ((uint32_t)((read_size &
                    (uint16_t)COREI3C_CCC_DATA_LEN_NS_MASK))
                    << COREI3C_CCC_DATA_LEN_SHIFT);
            cmd |= ((uint32_t)(tid & COREI3C_CMD_TID_NS_MASK)
                    << COREI3C_CMD_TID_SHIFT);
            /* Mark as read direction */
            cmd |= COREI3C_CMD_RNW_MASK;

            if( (options & I3C_OPT_RESPONSE) != 0u )
            {
                cmd |= COREI3C_CMD_ROC_MASK;
            }
            if( (options & I3C_OPT_EMIT_STOP) != 0u )
            {
                cmd |= COREI3C_CMD_TOC_MASK;
            }
            cmd |= COREI3C_CMD_CP_MASK;
            HAL_set_32bit_reg( this_i3c->base_addr,
                               COREI3C_CMND_FIFO, cmd );

            /* Poll until RD_STAT set (Read FIFO has data) or transfer
             * error */
            timeout = I3C_CMD_POLL_TIMEOUT;
            status  = I3C_ERR_TIMEOUT;
            while( timeout > 0u )
            {
                reg_val = HAL_get_32bit_reg( this_i3c->base_addr,
                                             COREI3C_CR_INTR_STAT );
                if( (reg_val & COREI3C_CR_STAT_RD_MASK) != 0u )
                {
                    status = I3C_SUCCESS;
                    break;
                }
                if( (reg_val & COREI3C_CR_STAT_XFER_ERR_MASK) != 0u )
                {
                    HAL_set_32bit_reg( this_i3c->base_addr,
                                       COREI3C_CR_INTR_STAT,
                                       COREI3C_CR_STAT_XFER_ERR_MASK );
                    status = I3C_ERR_NACK;
                    break;
                }
                --timeout;
            }

            if( status == I3C_SUCCESS )
            {
                /* Drain Read FIFO into caller buffer */
                uint16_t bytes_read = 0u;
                uint16_t remaining  = read_size;

                while( remaining > 0u )
                {
                    reg_val = HAL_get_32bit_reg( this_i3c->base_addr,
                                                 COREI3C_CR_INTR_STAT );
                    if( (reg_val & COREI3C_CR_STAT_RD_MASK) == 0u )
                    {
                        break;  /* FIFO empty */
                    }
                    uint32_t dword;
                    dword = HAL_get_32bit_reg( this_i3c->base_addr,
                                               COREI3C_RD_FIFO );
                    if( remaining >= 4u )
                    {
                        read_buffer[bytes_read]      =
                            (uint8_t)((dword)        & 0xFFu);
                        read_buffer[bytes_read + 1u] =
                            (uint8_t)((dword >> 8u)  & 0xFFu);
                        read_buffer[bytes_read + 2u] =
                            (uint8_t)((dword >> 16u) & 0xFFu);
                        read_buffer[bytes_read + 3u] =
                            (uint8_t)((dword >> 24u) & 0xFFu);
                        bytes_read += 4u;
                        remaining  -= 4u;
                    }
                    else
                    {
                        uint16_t tail = remaining;
                        switch( tail )
                        {
                            case 3u:
                                read_buffer[bytes_read + 2u] =
                                    (uint8_t)((dword >> 16u) & 0xFFu);
                                /* fallthrough */
                            case 2u:
                                read_buffer[bytes_read + 1u] =
                                    (uint8_t)((dword >> 8u)  & 0xFFu);
                                /* fallthrough */
                            case 1u:
                                read_buffer[bytes_read] =
                                    (uint8_t)((dword)        & 0xFFu);
                                break;
                            default:
                                break;
                        }
                        bytes_read += tail;
                        remaining   = 0u;
                    }
                }
                *read_len = (uint32_t)bytes_read;
            }
        }
        else
        {
            status = I3C_ERR_NOT_CTRL;
        }
    }
    return (status);
}

/*******************************************************************************
 * I3C_write()
 * See "core_i3c.h" file for details of how to use this function.
 */
i3c_status_t
I3C_write
(
    i3c_instance_t       * this_i3c,
    uint8_t                serial_addr,
    const uint8_t        * write_buffer,
    uint16_t               write_size,
    uint32_t               options
)
{
    i3c_status_t status = I3C_ERR_PARAM;
    HAL_ASSERT( this_i3c != NULL_INSTANCE );
    if( this_i3c != NULL_INSTANCE )
    {
        if( (write_buffer != NULL_CONST_BUFFER) || (write_size == 0u) )
        {
            /* Must be active controller */
            uint32_t state = HAL_get_32bit_reg( this_i3c->base_addr,
                                                COREI3C_PRESENT_STATE );
            if( (state & COREI3C_PRESENT_STATE_CURRENT_CTRL_MASK) != 0u )
            {
                uint16_t bytes_remaining = write_size;
                uint16_t chunk;
                uint16_t offset = 0u;

                /*
                 * For transfers larger than I3C_MAX_CMD_XFER_LEN (63 bytes),
                 * split into multiple command descriptors. Intermediate
                 * chunks use repeated-START (no TOC/STOP). Only the final
                 * chunk uses the caller's options.
                 */
                while( bytes_remaining > 0u )
                {
                    uint32_t cmd;
                    uint32_t chunk_options;
                    uint16_t i;
                    uint8_t  tid;
                    if( bytes_remaining > (uint16_t)I3C_MAX_CMD_XFER_LEN )
                    {
                        chunk         = (uint16_t)I3C_MAX_CMD_XFER_LEN;
                        chunk_options = I3C_OPT_NONE;
                    }
                    else
                    {
                        chunk         = bytes_remaining;
                        chunk_options = options;
                    }
                    /* Increment rolling TID */
                    tid = this_i3c->tid;
                    this_i3c->tid = (this_i3c->tid >= I3C_TID_MAX) ? 0u :
                                    (this_i3c->tid + 1u);
                    /* Pack chunk bytes into Write FIFO */
                    i = 0u;
                    while( (chunk - i) >= 4u )
                    {
                        uint32_t dword =
                            ((uint32_t)write_buffer[offset + i])             |
                            ((uint32_t)write_buffer[offset + i + 1u] << 8u)  |
                            ((uint32_t)write_buffer[offset + i + 2u] << 16u) |
                            ((uint32_t)write_buffer[offset + i + 3u] << 24u);
                        HAL_set_32bit_reg( this_i3c->base_addr,
                                           COREI3C_WRT_FIFO, dword );
                        i += 4u;
                    }
                    if( i < chunk )
                    {
                        uint32_t dword = 0u;
                        switch( chunk - i )
                        {
                            case 3u:
                                dword |= ((uint32_t)write_buffer[
                                           offset + i + 2u] << 16u);
                                /* fallthrough */
                            case 2u:
                                dword |= ((uint32_t)write_buffer[
                                           offset + i + 1u] << 8u);
                                /* fallthrough */
                            case 1u:
                                dword |= ((uint32_t)write_buffer[
                                           offset + i]);
                                break;
                            default:
                                break;
                        }
                        HAL_set_32bit_reg( this_i3c->base_addr,
                                           COREI3C_WRT_FIFO, dword );
                    }
                    /* Build private write command descriptor (RnW = 0) */
                    cmd  = ((uint32_t)(serial_addr &
                            COREI3C_CMD_DEV_ADDR_NS_MASK)
                            << COREI3C_CMD_DEV_ADDR_SHIFT);
                    cmd |= ((uint32_t)((chunk &
                            (uint16_t)COREI3C_CMD_XFER_LEN_NS_MASK))
                            << COREI3C_CMD_XFER_LEN_SHIFT);
                    cmd |= ((uint32_t)(tid & COREI3C_CMD_TID_NS_MASK)
                            << COREI3C_CMD_TID_SHIFT);
                    if( (chunk_options & I3C_OPT_RESPONSE) != 0u )
                    {
                        cmd |= COREI3C_CMD_ROC_MASK;
                    }
                    if( (chunk_options & I3C_OPT_EMIT_STOP) != 0u )
                    {
                        cmd |= COREI3C_CMD_TOC_MASK;
                    }
                    cmd |= COREI3C_CMD_CP_MASK;
                    HAL_set_32bit_reg( this_i3c->base_addr,
                                       COREI3C_CMND_FIFO, cmd );
                    offset          += chunk;
                    bytes_remaining -= chunk;
                }
                status = I3C_SUCCESS;
            }
            else
            {
                status = I3C_ERR_NOT_CTRL;
            }
        }
    }

    return (status);
}

/*******************************************************************************
 * I3C_read()
 * See "core_i3c.h" file for details of how to use this function.
 */
i3c_status_t
I3C_read
(
    i3c_instance_t  * this_i3c,
    uint8_t           serial_addr,
    uint8_t         * read_buffer,
    uint16_t          read_size,
    uint32_t        * read_len,
    uint32_t          options
)
{
    i3c_status_t status = I3C_ERR_PARAM;
    HAL_ASSERT( this_i3c != NULL_INSTANCE );
    HAL_ASSERT( read_buffer != NULL_BUFFER );
    HAL_ASSERT( read_len != NULL_LENGTH );
    if( (this_i3c != NULL_INSTANCE)   &&
        (read_buffer != NULL_BUFFER)   &&
        (read_len != NULL_LENGTH) )
    {
        /* Must be active controller */
        uint32_t state = HAL_get_32bit_reg( this_i3c->base_addr,
                                            COREI3C_PRESENT_STATE );
        if( (state & COREI3C_PRESENT_STATE_CURRENT_CTRL_MASK) != 0u )
        {
            uint32_t reg_val;
            uint16_t bytes_remaining;
            uint16_t chunk;
            uint16_t offset;
            uint16_t bytes_got;

            *read_len       = 0u;
            bytes_remaining = read_size;
            offset          = 0u;
            status          = I3C_SUCCESS;
            while( (bytes_remaining > 0u) && (status == I3C_SUCCESS) )
            {
                uint32_t cmd;
                uint32_t timeout;
                uint32_t chunk_options;
                uint8_t  tid;
                if( bytes_remaining > (uint16_t)I3C_MAX_CMD_XFER_LEN )
                {
                    chunk         = (uint16_t)I3C_MAX_CMD_XFER_LEN;
                    chunk_options = I3C_OPT_NONE;
                }
                else
                {
                    chunk         = bytes_remaining;
                    chunk_options = options;
                }
                /* Increment rolling TID */
                tid = this_i3c->tid;
                this_i3c->tid = (this_i3c->tid >= I3C_TID_MAX) ? 0u :
                                (this_i3c->tid + 1u);
                /* Build private read command descriptor (RnW = 1) */
                cmd  = ((uint32_t)(serial_addr &
                        COREI3C_CMD_DEV_ADDR_NS_MASK)
                        << COREI3C_CMD_DEV_ADDR_SHIFT);
                cmd |= COREI3C_CMD_RNW_MASK;
                cmd |= ((uint32_t)((chunk &
                        (uint16_t)COREI3C_CMD_XFER_LEN_NS_MASK))
                        << COREI3C_CMD_XFER_LEN_SHIFT);
                cmd |= ((uint32_t)(tid & COREI3C_CMD_TID_NS_MASK)
                        << COREI3C_CMD_TID_SHIFT);
                if( (chunk_options & I3C_OPT_RESPONSE) != 0u )
                {
                    cmd |= COREI3C_CMD_ROC_MASK;
                }
                if( (chunk_options & I3C_OPT_EMIT_STOP) != 0u )
                {
                    cmd |= COREI3C_CMD_TOC_MASK;
                }
                cmd |= COREI3C_CMD_CP_MASK;
                HAL_set_32bit_reg( this_i3c->base_addr,
                                   COREI3C_CMND_FIFO, cmd );

                /* Poll until RD_STAT set or transfer error */
                timeout = I3C_CMD_POLL_TIMEOUT;
                status  = I3C_ERR_TIMEOUT;
                while( timeout > 0u )
                {
                    reg_val = HAL_get_32bit_reg( this_i3c->base_addr,
                                                 COREI3C_CR_INTR_STAT );
                    if( (reg_val & COREI3C_CR_STAT_RD_MASK) != 0u )
                    {
                        status = I3C_SUCCESS;
                        break;
                    }
                    if( (reg_val & COREI3C_CR_STAT_XFER_ERR_MASK) != 0u )
                    {
                        HAL_set_32bit_reg( this_i3c->base_addr,
                                           COREI3C_CR_INTR_STAT,
                                           COREI3C_CR_STAT_XFER_ERR_MASK );
                        status = I3C_ERR_NACK;
                        break;
                    }
                    --timeout;
                }

                if( status == I3C_SUCCESS )
                {
                    /* Drain Read FIFO into caller buffer */
                    bytes_got = 0u;
                    {
                        uint16_t remaining = chunk;
                        while( remaining > 0u )
                        {
                            reg_val = HAL_get_32bit_reg(
                                this_i3c->base_addr,
                                COREI3C_CR_INTR_STAT );
                            if( (reg_val &
                                 COREI3C_CR_STAT_RD_MASK) == 0u )
                            {
                                break;
                            }
                            uint32_t dword = HAL_get_32bit_reg(
                                this_i3c->base_addr,
                                COREI3C_RD_FIFO );
                            if( remaining >= 4u )
                            {
                                read_buffer[offset + bytes_got] =
                                    (uint8_t)((dword) & 0xFFu);
                                read_buffer[offset + bytes_got + 1u] =
                                    (uint8_t)((dword >> 8u) & 0xFFu);
                                read_buffer[offset + bytes_got + 2u] =
                                    (uint8_t)((dword >> 16u) & 0xFFu);
                                read_buffer[offset + bytes_got + 3u] =
                                    (uint8_t)((dword >> 24u) & 0xFFu);
                                bytes_got += 4u;
                                remaining -= 4u;
                            }
                            else
                            {
                                uint16_t tail = remaining;
                                switch( tail )
                                {
                                    case 3u:
                                        read_buffer[offset +
                                            bytes_got + 2u] =
                                            (uint8_t)((dword >> 16u)
                                                      & 0xFFu);
                                        /* fallthrough */
                                    case 2u:
                                        read_buffer[offset +
                                            bytes_got + 1u] =
                                            (uint8_t)((dword >> 8u)
                                                      & 0xFFu);
                                        /* fallthrough */
                                    case 1u:
                                        read_buffer[offset +
                                            bytes_got] =
                                            (uint8_t)((dword) & 0xFFu);
                                        break;
                                    default:
                                        break;
                                }
                                bytes_got += tail;
                                remaining  = 0u;
                            }
                        }
                    }
                    *read_len       += (uint32_t)bytes_got;
                    offset          += bytes_got;
                    bytes_remaining -= bytes_got;
                }
            }
        }
        else
        {
            status = I3C_ERR_NOT_CTRL;
        }
    }
    return (status);
}

/*******************************************************************************
 * I3C_write_read()
 * See "core_i3c.h" file for details of how to use this function.
 */
i3c_status_t
I3C_write_read
(
    i3c_instance_t       * this_i3c,
    uint8_t                serial_addr,
    const uint8_t        * addr_offset,
    uint16_t               offset_size,
    uint8_t              * read_buffer,
    uint16_t               read_size,
    uint32_t             * read_len,
    uint32_t               options
)
{
    i3c_status_t status = I3C_ERR_PARAM;

    HAL_ASSERT( this_i3c != NULL_INSTANCE );
    HAL_ASSERT( addr_offset != NULL_CONST_BUFFER );
    HAL_ASSERT( read_buffer != NULL_BUFFER );
    HAL_ASSERT( read_len != NULL_LENGTH );

    if( (this_i3c != NULL_INSTANCE)        &&
        (addr_offset != NULL_CONST_BUFFER) &&
        (read_buffer != NULL_BUFFER)        &&
        (read_len != NULL_LENGTH) )
    {
        *read_len = 0u;

        /*
         * Step 1: Private write without STOP (repeated-START).
         *         Bus stays active for the following read.
         */
        status = I3C_write( this_i3c, serial_addr, addr_offset, offset_size,
                            I3C_OPT_NONE );

        if( status == I3C_SUCCESS )
        {
            /*
             * Step 2: Private read following repeated-START.
             *         Pass caller options (including any requested STOP).
             */
            status = I3C_read( this_i3c, serial_addr, read_buffer, read_size,
                               read_len, options );
        }
    }
    return (status);
}

/*******************************************************************************
 * I3C_read_response()
 * See "core_i3c.h" file for details of how to use this function.
 */
uint32_t
I3C_read_response
(
    i3c_instance_t * this_i3c,
    uint32_t       * response
)
{
    uint32_t result = 0u;

    HAL_ASSERT( this_i3c != NULL_INSTANCE );
    HAL_ASSERT( response != NULL_LENGTH );
    if( (this_i3c != NULL_INSTANCE) && (response != NULL_LENGTH) )
    {
        uint32_t stat = HAL_get_32bit_reg( this_i3c->base_addr,
                                           COREI3C_CR_INTR_STAT );

        if( (stat & COREI3C_CR_STAT_RESP_RDY_MASK) != 0u )
        {
            *response = HAL_get_32bit_reg( this_i3c->base_addr,
                                           COREI3C_STATUS_FIFO );
            result = 1u;
        }
    }
    return (result);
}

/*******************************************************************************
 * I3C_read_ibi()
 * See "core_i3c.h" file for details of how to use this function.
 */
uint32_t
I3C_read_ibi
(
    i3c_instance_t * this_i3c,
    uint32_t       * ibi_descriptor
)
{
    uint32_t result = 0u;

    HAL_ASSERT( this_i3c != NULL_INSTANCE );
    HAL_ASSERT( ibi_descriptor != NULL_LENGTH );

    if( (this_i3c != NULL_INSTANCE) && (ibi_descriptor != NULL_LENGTH) )
    {
        uint32_t stat = HAL_get_32bit_reg( this_i3c->base_addr,
                                           COREI3C_CR_INTR_STAT );

        if( (stat & COREI3C_CR_STAT_IBI_RECV_MASK) != 0u )
        {
            *ibi_descriptor = HAL_get_32bit_reg( this_i3c->base_addr,
                                                 COREI3C_IBI_PORT );
            result = 1u;
        }
    }
    return (result);
}

/*******************************************************************************
 * I3C_get_status()
 * See "core_i3c.h" file for details of how to use this function.
 */
uint32_t
I3C_get_status
(
    i3c_instance_t * this_i3c
)
{
    uint32_t result = 0u;

    HAL_ASSERT( this_i3c != NULL_INSTANCE );

    if( this_i3c != NULL_INSTANCE )
    {
        result = HAL_get_32bit_reg( this_i3c->base_addr,
                                    COREI3C_PRESENT_STATE );
    }
    return (result);
}

/*******************************************************************************
 * I3C_enable_irq()
 * See "core_i3c.h" file for details of how to use this function.
 */
void
I3C_enable_irq
(
    i3c_instance_t * this_i3c,
    uint32_t         irq_mask
)
{
    HAL_ASSERT( this_i3c != NULL_INSTANCE );
    if( this_i3c != NULL_INSTANCE )
    {
        /*
         * Bits [15:0]  of irq_mask -> general INTR_STAT_EN / INTR_SIG_EN
         * Bits [31:16] of irq_mask -> cmd/response CR_INTR_STAT_EN / SIG_EN
         */
        uint32_t general_mask = irq_mask & 0x0000FFFFu;
        uint32_t cr_mask      = (irq_mask >> 16u) & 0x0000FFFFu;
        uint32_t reg_val;

        if( general_mask != 0u )
        {
            reg_val  = HAL_get_32bit_reg( this_i3c->base_addr,
                                          COREI3C_INTR_STAT_EN );
            reg_val |= general_mask;
            HAL_set_32bit_reg( this_i3c->base_addr,
                               COREI3C_INTR_STAT_EN, reg_val );
            reg_val  = HAL_get_32bit_reg( this_i3c->base_addr,
                                          COREI3C_INTR_SIG_EN );
            reg_val |= general_mask;
            HAL_set_32bit_reg( this_i3c->base_addr,
                               COREI3C_INTR_SIG_EN, reg_val );
        }
        if( cr_mask != 0u )
        {
            reg_val  = HAL_get_32bit_reg( this_i3c->base_addr,
                                          COREI3C_CR_INTR_STAT_EN );
            reg_val |= cr_mask;
            HAL_set_32bit_reg( this_i3c->base_addr,
                               COREI3C_CR_INTR_STAT_EN, reg_val );
            reg_val  = HAL_get_32bit_reg( this_i3c->base_addr,
                                          COREI3C_CR_INTR_SIG_EN );
            reg_val |= cr_mask;
            HAL_set_32bit_reg( this_i3c->base_addr,
                               COREI3C_CR_INTR_SIG_EN, reg_val );
        }
    }
}

/*******************************************************************************
 * I3C_disable_irq()
 * See "core_i3c.h" file for details of how to use this function.
 */
void
I3C_disable_irq
(
    i3c_instance_t * this_i3c,
    uint32_t         irq_mask
)
{
    HAL_ASSERT( this_i3c != NULL_INSTANCE );
    if( this_i3c != NULL_INSTANCE )
    {
        uint32_t general_mask = irq_mask & 0x0000FFFFu;
        uint32_t cr_mask      = (irq_mask >> 16u) & 0x0000FFFFu;
        uint32_t reg_val;

        if( general_mask != 0u )
        {
            reg_val  = HAL_get_32bit_reg( this_i3c->base_addr,
                                          COREI3C_INTR_STAT_EN );
            reg_val &= ~general_mask;
            HAL_set_32bit_reg( this_i3c->base_addr,
                               COREI3C_INTR_STAT_EN, reg_val );

            reg_val  = HAL_get_32bit_reg( this_i3c->base_addr,
                                          COREI3C_INTR_SIG_EN );
            reg_val &= ~general_mask;
            HAL_set_32bit_reg( this_i3c->base_addr,
                               COREI3C_INTR_SIG_EN, reg_val );
        }
        if( cr_mask != 0u )
        {
            reg_val  = HAL_get_32bit_reg( this_i3c->base_addr,
                                          COREI3C_CR_INTR_STAT_EN );
            reg_val &= ~cr_mask;
            HAL_set_32bit_reg( this_i3c->base_addr,
                               COREI3C_CR_INTR_STAT_EN, reg_val );
            reg_val  = HAL_get_32bit_reg( this_i3c->base_addr,
                                          COREI3C_CR_INTR_SIG_EN );
            reg_val &= ~cr_mask;
            HAL_set_32bit_reg( this_i3c->base_addr,
                               COREI3C_CR_INTR_SIG_EN, reg_val );
        }
    }
}

/*******************************************************************************
 * I3C_clear_irq()
 * See "core_i3c.h" file for details of how to use this function.
 */
void
I3C_clear_irq
(
    i3c_instance_t * this_i3c,
    uint32_t         irq_mask
)
{
    HAL_ASSERT( this_i3c != NULL_INSTANCE );
    if( this_i3c != NULL_INSTANCE )
    {
        uint32_t general_mask = irq_mask & 0x0000FFFFu;
        uint32_t cr_mask      = (irq_mask >> 16u) & 0x0000FFFFu;

        /* Write 1 to RW1C bits to clear them */
        if( general_mask != 0u )
        {
            HAL_set_32bit_reg( this_i3c->base_addr,
                               COREI3C_INTR_STAT, general_mask );
        }
        if( cr_mask != 0u )
        {
            HAL_set_32bit_reg( this_i3c->base_addr,
                               COREI3C_CR_INTR_STAT, cr_mask );
        }
    }
}

/*******************************************************************************
 * I3C_get_irq_status()
 * See "core_i3c.h" file for details of how to use this function.
 */
uint32_t
I3C_get_irq_status
(
    i3c_instance_t * this_i3c
)
{
    uint32_t result = 0u;

    HAL_ASSERT( this_i3c != NULL_INSTANCE );
    if( this_i3c != NULL_INSTANCE )
    {
        uint32_t general_stat = HAL_get_32bit_reg( this_i3c->base_addr,
                                    COREI3C_INTR_STAT ) & 0x0000FFFFu;
        uint32_t cr_stat      = HAL_get_32bit_reg( this_i3c->base_addr,
                                    COREI3C_CR_INTR_STAT ) & 0x0000FFFFu;

        /* Pack: general bits in [15:0], CR bits in [31:16] */
        result = (general_stat | (cr_stat << 16u));
    }
    return (result);
}

#ifdef __cplusplus
}
#endif
