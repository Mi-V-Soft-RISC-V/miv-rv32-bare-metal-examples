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
 * @file core_i3c.h
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief CoreI3C IP bare metal driver public API.
 *
 */
/*=========================================================================*//**
  @mainpage CoreI3C Bare Metal Driver

  ==============================================================================
  Introduction
  ==============================================================================
  The CoreI3C IP implements the MIPI I3C Basic specification in controller mode,
  providing an AXI4 host interface for FPGA-based SoC designs. This driver
  provides a bare-metal C API for all supported CoreI3C v2.0 features.

  The driver is designed for bare-metal embedded systems where no operating
  system is available. All instances are statically allocated by the caller.
  No dynamic memory allocation is used.

  ==============================================================================
  Supported Features (v2.0)
  ==============================================================================
  - I3C Controller mode (SDR) up to 12.5 Mbps
  - Dynamic Address Assignment (DAA) via ENTDAA / SETDASA procedures
  - Broadcast and Direct Common Command Codes (CCC)
  - Private write and read transfers
  - Write-then-read combined transfers
  - In-Band Interrupt (IBI) reception and descriptor readout
  - FIFO-based data path: Command, Write, Read, and Response FIFOs
  - Full interrupt subsystem: enable, disable, clear, and status read
  - Present state and capability reporting

  ==============================================================================
  Unsupported Features (planned for future releases)
  ==============================================================================
  - HDR modes
  - Hot-Join
  - Secondary Controller
  - Target mode
  - Multi-Lane

  ==============================================================================
  Driver Configuration
  ==============================================================================
  Call I3C_init_controller() once per CoreI3C hardware instance before any
  other driver function. The caller must provide a statically allocated
  i3c_instance_t and a populated i3c_cfg_t configuration structure.

  Example:
  @code
      #include "core_i3c.h"

      static i3c_instance_t g_i3c;

      int main(void)
      {
          i3c_cfg_t cfg = {
              .scl_od_high_cycles  = 2u,
              .scl_od_low_cycles   = 6u,
              .scl_pp_high_cycles  = 2u,
              .scl_pp_low_cycles   = 2u,
              .i2c_dev_present     = 0u,
              .iba_include         = 1u,
              .halt_on_timeout     = 1u,
              .ibi_capable         = 1u
          };

          I3C_init_controller(&g_i3c, 0x44000000u, 0x08u, &cfg);

          // Run DAA
          i3c_address_t addr_list[4];
          I3C_do_daa(&g_i3c, addr_list, 4u);

          // Write 4 bytes to target at dynamic address 0x12
          uint8_t tx[4] = {0xA0, 0xB1, 0xC2, 0xD3};
          I3C_write(&g_i3c, 0x12u, tx, 4u, I3C_OPT_EMIT_STOP);

          return 0;
      }
  @endcode

  ==============================================================================
  Theory of Operation
  ==============================================================================
  All commands are submitted by writing a 32-bit descriptor into the Command
  FIFO (I3C_CMND_FIFO). Transmit data is placed in the Write FIFO before or
  after the command descriptor depending on the transfer type. On transfer
  completion the core places a response descriptor in the Status FIFO.

  The driver follows a polling or interrupt-driven model:
  - Polling: call I3C_get_irq_status() to check completion bits.
  - Interrupt-driven: configure INT_O pin via I3C_enable_irq(), implement ISR,
    call I3C_get_irq_status() / I3C_clear_irq() inside the ISR.

  @note This driver is NOT re-entrant. If shared between an ISR and a main-loop
        context, the caller must use a critical section (disable/enable IRQ)
        around any driver call.

*//*=========================================================================*/

#ifndef CORE_I3C_H_
#define CORE_I3C_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef LEGACY_DIR_STRUCTURE
#include "hal/hal.h"
#include "hal/i3c_slave_sensors.h"
#else
#include "hal.h"
#include "hal_assert.h"
#endif

/*=============================================================================
 * Return status codes
 *===========================================================================*/
/***************************************************************************//**
  Return status codes used by all CoreI3C driver functions.

  | Constant              | Value | Description                              |
  |-----------------------|-------|------------------------------------------|
  | I3C_SUCCESS           | 0     | Operation completed successfully         |
  | I3C_ERR_PARAM         | 1     | Invalid parameter                        |
  | I3C_ERR_FIFO_FULL     | 2     | Target FIFO is full; cannot queue data   |
  | I3C_ERR_FIFO_EMPTY    | 3     | Source FIFO is empty; no data available  |
  | I3C_ERR_NOT_CTRL      | 4     | Not the active controller on the bus     |
  | I3C_ERR_NACK          | 5     | Target responded with NACK               |
  | I3C_ERR_TIMEOUT       | 6     | Command sequence timeout                 |
  | I3C_ERR_ABORTED       | 7     | Transfer was aborted                     |
  | I3C_ERR_INTERNAL      | 8     | Non-recoverable internal core error      |
  | I3C_ERR_BUS_BUSY      | 9     | Bus is currently busy                    |
  | I3C_ERR_DAA_FAILED    | 10    | DAA procedure failed (no devices found)  |
*/
#define I3C_SUCCESS              (0u)
#define I3C_ERR_PARAM            (1u)
#define I3C_ERR_FIFO_FULL        (2u)
#define I3C_ERR_FIFO_EMPTY       (3u)
#define I3C_ERR_NOT_CTRL         (4u)
#define I3C_ERR_NACK             (5u)
#define I3C_ERR_TIMEOUT          (6u)
#define I3C_ERR_ABORTED          (7u)
#define I3C_ERR_INTERNAL         (8u)
#define I3C_ERR_BUS_BUSY         (9u)
#define I3C_ERR_DAA_FAILED       (10u)

/*=============================================================================
 * Transfer options flags (or'd into the options parameter)
 *===========================================================================*/
/***************************************************************************//**
  Transfer option flags used in I3C_write(), I3C_read(), I3C_write_read(),
  I3C_write_ccc(), and I3C_read_ccc().

  | Constant            | Description                                        |
  |---------------------|----------------------------------------------------|
  | I3C_OPT_NONE        | No options: repeated-START, no response needed    |
  | I3C_OPT_EMIT_STOP   | Emit a STOP condition after this transfer          |
  | I3C_OPT_RESPONSE    | Request a response descriptor in the Status FIFO   |
  | I3C_OPT_IBA_SKIP    | Do not include I3C broadcast address for this tx   |
*/
#define I3C_OPT_NONE             (0x00u)
#define I3C_OPT_EMIT_STOP        (0x01u)
#define I3C_OPT_RESPONSE         (0x02u)
#define I3C_OPT_IBA_SKIP         (0x04u)

 /*
  * Data length word (CMND_FIFO word 1, bits [31:16])
  */
 #define I3C_CMD_DATA_LEN(len)           (((uint32_t)(len)) << 16u)

/*=============================================================================
 * Interrupt mask constants
 *===========================================================================*/
/***************************************************************************//**
  Interrupt source mask constants for I3C_enable_irq(), I3C_disable_irq(),
  I3C_clear_irq(), and I3C_get_irq_status().

  These constants may be OR'd together.

  General interrupt sources (mapped to INTR_STAT register):
  | Constant                | Description                                  |
  |-------------------------|----------------------------------------------|
  | I3C_IRQ_CMD_TIMEOUT     | Command sequence timeout error               |
  | I3C_IRQ_CMD_STALL       | Command sequence stall warning               |
  | I3C_IRQ_SEQ_CANCEL      | Transaction sequence was cancelled           |
  | I3C_IRQ_INTERNAL_ERR    | Non-recoverable internal error               |
  | I3C_IRQ_BUS_CTRL_CHG    | Controller role changed                      |
  | I3C_IRQ_TRGT_IBI_REQ    | Target IBI request handled                   |
  | I3C_IRQ_DYN_ADDR        | Target dynamic address assigned              |

  Command/response interrupt sources (mapped to CMND_RSPNS_INTR_STAT register,
  encoded in bits [31:16] of the irq_mask):
  | Constant                | Description                                  |
  |-------------------------|----------------------------------------------|
  | I3C_CR_IRQ_XFER_ERR     | Transfer error on bus                        |
  | I3C_CR_IRQ_XFER_ABRT    | Transfer aborted                             |
  | I3C_CR_IRQ_RESP_RDY     | Response FIFO has at least one entry         |
  | I3C_CR_IRQ_CMD_RDY      | Command execution completed                  |
  | I3C_CR_IRQ_IBI_RECV     | IBI FIFO has at least one entry              |
  | I3C_CR_IRQ_WRT_AVAIL    | Write FIFO has at least one entry            |
  | I3C_CR_IRQ_RD_AVAIL     | Read FIFO has at least one entry             |
*/
#include "corei3c_regs.h"

/*=============================================================================
 * Data types and structures
 *===========================================================================*/

/***************************************************************************//**
  The i3c_status_t type represents a driver return status. All public functions
  that can fail return this type. A value of I3C_SUCCESS (0) indicates no error.
*/
typedef uint8_t i3c_status_t;

/***************************************************************************//**
  The i3c_address_t structure holds the dynamic address and associated
  identification information for one I3C target device discovered during DAA.

  | Field          | Description                                          |
  |----------------|------------------------------------------------------|
  | dynamic_addr   | 7-bit dynamic address assigned during DAA            |
  | static_addr    | 7-bit static address (0 if none)                     |
  | pid_high       | Provisional ID upper 16 bits (manufacturer ID)       |
  | pid_low        | Provisional ID lower 32 bits                         |
  | bcr            | Bus Characteristic Register value                    |
  | dcr            | Device Characteristic Register value                 |
*/
typedef struct i3c_address
{
    uint8_t  dynamic_addr;
    uint8_t  static_addr;
    uint16_t pid_high;
    uint32_t pid_low;
    uint8_t  bcr;
    uint8_t  dcr;
} i3c_address_t;

/***************************************************************************//**
  The i3c_cfg_t structure holds all configuration parameters passed to
  I3C_init_controller(). The caller must populate this structure before calling
  init.

  | Field               | Description                                       |
  |---------------------|---------------------------------------------------|
  | scl_od_high_cycles  | ACLK cycles for SCL HIGH in open-drain mode        |
  | scl_od_low_cycles   | ACLK cycles for SCL LOW in open-drain mode         |
  | scl_pp_high_cycles  | ACLK cycles for SCL HIGH in push-pull mode         |
  | scl_pp_low_cycles   | ACLK cycles for SCL LOW in push-pull mode          |
  | i2c_dev_present     | 1 if legacy I2C devices are present on the bus     |
  | iba_include         | 1 to include broadcast address for private xfers   |
  | halt_on_timeout     | 1 to halt the core on command sequence timeout     |
  | ibi_capable         | 1 if IBI_CAP_EN parameter was set at synthesis     |

  SCL timing example for 50 MHz ACLK, targeting ~3.5 Mbps OD / 12.5 Mbps PP:
  @code
      i3c_cfg_t cfg = {
          .scl_od_high_cycles = 4u,   // 80 ns HIGH
          .scl_od_low_cycles  = 10u,  // 200 ns LOW => ~3.5 Mbps OD
          .scl_pp_high_cycles = 2u,   // 40 ns HIGH
          .scl_pp_low_cycles  = 2u,   // 40 ns LOW  => 12.5 Mbps PP
          .i2c_dev_present    = 0u,
          .iba_include        = 1u,
          .halt_on_timeout    = 1u,
          .ibi_capable        = 1u
      };
  @endcode
*/
typedef struct i3c_cfg
{
    uint16_t scl_od_high_cycles;
    uint16_t scl_od_low_cycles;
    uint16_t scl_pp_high_cycles;
    uint16_t scl_pp_low_cycles;
    uint8_t  i2c_dev_present;
    uint8_t  iba_include;
    uint8_t  halt_on_timeout;
    uint8_t  ibi_capable;
} i3c_cfg_t;

/***************************************************************************//**
  The i3c_instance_t structure identifies a CoreI3C hardware instance.
  I3C_init_controller() initializes this structure. A pointer to an initialized
  instance must be provided as the first parameter to all driver functions.

  @note Do not modify the contents of this structure directly. Use only the
        driver API functions.
*/
typedef struct i3c_instance
{
    addr_t   base_addr;       /* AXI4 base address of this CoreI3C instance     */
    uint8_t  ibi_capable;     /* 1 if IBI hardware support is enabled           */
    uint8_t  tid;             /* Rolling transaction ID counter (0-15)          */
    uint8_t  dat_offset;      /* Cached DAT table byte offset (from DAT reg)    */
    uint8_t  dat_size;        /* Number of DAT entries                          */
    uint8_t  dct_offset;      /* Cached DCT table byte offset (from DCT reg)    */
    uint8_t  dct_size;        /* Number of DCT entries                          */
} i3c_instance_t;

/*=============================================================================
 * Public function declarations
 *===========================================================================*/

/***************************************************************************//**
  The I3C_init_controller() function initializes the driver. It sets the base
  address for the CoreI3C instance, initializes the i3c_instance_t data
  structure, applies the configuration provided in the cfg parameter including
  SCL timing and operating settings, and enables the I3C bus. This function
  must be called before calling any other CoreI3C driver functions.

  The initialization sequence is:
    1. Soft-reset the core and wait for completion.
    2. Reset all FIFOs.
    3. Program SCL open-drain and push-pull timing registers.
    4. Set the controller dynamic address.
    5. Apply control register options (halt-on-timeout, I2C presence, IBA).
    6. Enable selected interrupt sources.
    7. Assert BUS_ENABLE.

  @param this_i3c
    Pointer to an i3c_instance_t structure that holds all data related to the
    CoreI3C instance being initialized. This pointer is passed to all subsequent
    driver function calls.

  @param base_addr
    Base address of the CoreI3C hardware instance registers in the processor
    memory map.

  @param ser_address
    7-bit dynamic address the controller assigns to itself as the active
    primary controller. This address is written to CNTRL_DEVICE_ADDR.

  @param cfg
    Pointer to an i3c_cfg_t structure containing the configuration to apply.
    Must not be NULL.

  @return
    Returns I3C_SUCCESS on successful initialization.
    Returns I3C_ERR_PARAM if cfg is NULL.

  @example
  @code
      static i3c_instance_t g_i3c;

      i3c_cfg_t cfg = {
          .scl_od_high_cycles = 4u,
          .scl_od_low_cycles  = 10u,
          .scl_pp_high_cycles = 2u,
          .scl_pp_low_cycles  = 2u,
          .i2c_dev_present    = 0u,
          .iba_include        = 1u,
          .halt_on_timeout    = 1u,
          .ibi_capable        = 1u
      };

      I3C_init_controller(&g_i3c, 0x44000000u, 0x08u, &cfg);
  @endcode
*/
i3c_status_t
I3C_init_controller
(
    i3c_instance_t       * this_i3c,
    addr_t                 base_addr,
    uint8_t                ser_address,
    const i3c_cfg_t      * cfg
);

/***************************************************************************//**
  The I3C_do_daa() function initiates the Dynamic Address Assignment procedure
  on the I3C bus. It performs the ENTDAA operation to assign 7-bit dynamic
  addresses to all unaddressed I3C target devices on the bus. If the addr_list
  parameter is non-NULL, the function also fills it with the PID, BCR, DCR,
  and assigned dynamic address of each discovered target by reading the DCT
  table after the procedure completes.

  @param this_i3c
    Pointer to an initialized i3c_instance_t structure.

  @param addr_list
    Pointer to an array of i3c_address_t structures to be filled with the
    discovered target information. May be NULL if caller does not need this
    information. Array must have at least max_devices entries.

  @param max_devices
    Maximum number of entries in addr_list. The function stops after
    discovering this many devices. Unused if addr_list is NULL.

  @return
    Returns I3C_SUCCESS if at least one device was assigned an address.
    Returns I3C_ERR_DAA_FAILED if no devices responded to ENTDAA.
    Returns I3C_ERR_NOT_CTRL if this instance is not the active controller.

  @example
  @code
      i3c_address_t devs[8];
      i3c_status_t rc = I3C_do_daa(&g_i3c, devs, 8u);
      if (rc == I3C_SUCCESS)
      {
          // devs[0].dynamic_addr holds the first assigned address
      }
  @endcode
*/
i3c_status_t
I3C_do_daa
(
    i3c_instance_t  * this_i3c,
    i3c_address_t   * addr_list,
    uint32_t          max_devices
);

/***************************************************************************//**
  The I3C_abort() function aborts the current in-progress transfer on the
  CoreI3C bus. The CoreI3C controller terminates the transaction at the nearest
  opportunity after the current data byte completes.

  @param this_i3c
    Pointer to an initialized i3c_instance_t structure.

  @return
    Returns I3C_SUCCESS.

  @example
  @code
      I3C_abort(&g_i3c);
  @endcode
*/
i3c_status_t
I3C_abort
(
    i3c_instance_t * this_i3c
);

/***************************************************************************//**
  The I3C_resume() function resumes CoreI3C bus operation after the controller
  has entered the halt state. The CoreI3C enters the halt state when a transfer
  error occurs. This function writes the RESUME bit to allow the next queued
  command to proceed.

  @param this_i3c
    Pointer to an initialized i3c_instance_t structure.

  @return
    Returns I3C_SUCCESS.

  @example
  @code
      // Inside an ISR or main loop after detecting I3C_IRQ_CMD_TIMEOUT:
      I3C_resume(&g_i3c);
  @endcode
*/
i3c_status_t
I3C_resume
(
    i3c_instance_t * this_i3c
);

/***************************************************************************//**
  The I3C_write_ccc() function issues a write-type Common Command Code (CCC)
  to a specific target device (direct CCC) or to all devices on the bus
  (broadcast CCC). If serial_addr is I3C_BROADCAST_ADDR (0x7E), the command
  is issued as a broadcast; otherwise it is issued as a direct write CCC to
  the specified target.

  @param this_i3c
    Pointer to an initialized i3c_instance_t structure.

  @param serial_addr
    7-bit dynamic address of the target for a direct CCC, or I3C_BROADCAST_ADDR
    (0x7E) for a broadcast CCC.

  @param ccc_byte
    The CCC command byte to issue (e.g. I3C_CCC_RSTDAA, I3C_CCC_SETMWL).

  @param write_buffer
    Pointer to the data payload to transmit after the CCC byte. May be NULL
    if write_size is 0.

  @param write_size
    Number of bytes in write_buffer.

  @param options
    Transfer option flags. OR of I3C_OPT_EMIT_STOP, I3C_OPT_RESPONSE, etc.

  @return
    Returns I3C_SUCCESS on successful command submission.
    Returns I3C_ERR_FIFO_FULL if the Command or Write FIFO is full.
    Returns I3C_ERR_NOT_CTRL if this instance is not the active controller.

  @example
  @code
      // Broadcast RSTDAA to reset all dynamic addresses
      I3C_write_ccc(&g_i3c, I3C_BROADCAST_ADDR, I3C_CCC_RSTDAA,
                    NULL, 0u, I3C_OPT_EMIT_STOP | I3C_OPT_RESPONSE);
  @endcode
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
);

/***************************************************************************//**
  The I3C_read_ccc() function issues a read-type Direct CCC to a specific
  target device and reads the response into read_buffer.

  @param this_i3c
    Pointer to an initialized i3c_instance_t structure.

  @param serial_addr
    7-bit dynamic address of the target device.

  @param ccc_byte
    The CCC command byte to issue (e.g. I3C_CCC_GETBCR, I3C_CCC_GETPID).

  @param defining_byte
    Defining byte for CCCs that require one (e.g. GETMXDS). Set to 0 if not
    applicable.

  @param read_buffer
    Pointer to a buffer to receive the CCC response data.

  @param read_size
    Maximum number of bytes to read into read_buffer.

  @param read_len
    Pointer to a uint32_t that receives the actual number of bytes read.
    Must not be NULL.

  @param options
    Transfer option flags.

  @return
    Returns I3C_SUCCESS on successful command submission.
    Returns I3C_ERR_FIFO_FULL if the Command FIFO is full.
    Returns I3C_ERR_NOT_CTRL if this instance is not the active controller.

  @example
  @code
      uint8_t bcr_val;
      uint32_t rx_len = 0u;
      I3C_read_ccc(&g_i3c, 0x12u, I3C_CCC_GETBCR, 0u,
                   &bcr_val, 1u, &rx_len, I3C_OPT_EMIT_STOP | I3C_OPT_RESPONSE);
  @endcode
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
);

/***************************************************************************//**
  The I3C_write() function initiates an I3C controller private write
  transaction. It writes data from write_buffer to the CoreI3C Write FIFO and
  submits a private write command descriptor to the Command FIFO. The function
  returns after initiating the transfer. The caller must not modify or free
  write_buffer until the transfer is confirmed complete via the Status FIFO or
  the I3C_CR_IRQ_CMD_RDY interrupt.

  @param this_i3c
    Pointer to an initialized i3c_instance_t structure.

  @param serial_addr
    7-bit dynamic address of the target device.

  @param write_buffer
    Pointer to the data to transmit. The buffer must remain valid until
    transfer completion.

  @param write_size
    Number of bytes to transmit from write_buffer.

  @param options
    Transfer option flags (e.g. I3C_OPT_EMIT_STOP | I3C_OPT_RESPONSE).

  @return
    Returns I3C_SUCCESS on successful command submission.
    Returns I3C_ERR_FIFO_FULL if Write or Command FIFO is full.
    Returns I3C_ERR_NOT_CTRL if not the active controller.
    Returns I3C_ERR_PARAM if write_buffer is NULL with write_size > 0.

  @example
  @code
      uint8_t tx_buf[4] = {0xDE, 0xAD, 0xBE, 0xEF};
      I3C_write(&g_i3c, 0x12u, tx_buf, 4u,
                I3C_OPT_EMIT_STOP | I3C_OPT_RESPONSE);
  @endcode
*/
i3c_status_t
I3C_write
(
    i3c_instance_t       * this_i3c,
    uint8_t                serial_addr,
    const uint8_t        * write_buffer,
    uint16_t               write_size,
    uint32_t               options
);

/***************************************************************************//**
  The I3C_read() function initiates an I3C controller private read transaction.
  It submits a read command descriptor to the Command FIFO and, after the
  transfer completes, drains the Read FIFO into read_buffer. The function
  blocks until the Read FIFO contains data or an error is detected.

  @param this_i3c
    Pointer to an initialized i3c_instance_t structure.

  @param serial_addr
    7-bit dynamic address of the target device.

  @param read_buffer
    Pointer to a buffer to receive the incoming data. Must remain valid until
    the transfer completes.

  @param read_size
    Maximum number of bytes to read into read_buffer.

  @param read_len
    Pointer to a uint32_t that receives the actual number of bytes placed into
    read_buffer. Must not be NULL.

  @param options
    Transfer option flags.

  @return
    Returns I3C_SUCCESS on successful completion.
    Returns I3C_ERR_FIFO_FULL if Command FIFO is full.
    Returns I3C_ERR_NOT_CTRL if not the active controller.
    Returns I3C_ERR_PARAM if read_buffer or read_len is NULL.

  @example
  @code
      uint8_t rx_buf[8];
      uint32_t rx_len = 0u;
      I3C_read(&g_i3c, 0x12u, rx_buf, 8u, &rx_len,
               I3C_OPT_EMIT_STOP | I3C_OPT_RESPONSE);
  @endcode
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
);

void
I3C_drain_all_fifos
(
    i3c_instance_t       * this_i3c,
    uint8_t max_drain,
    uint16_t rd_flush
);

i3c_status_t
I3C_private_read
(
    i3c_instance_t  * this_i3c,
    uint8_t dct_index,
    uint8_t reg_addr,
    uint8_t * p_data,
    uint8_t num_bytes
);

/***************************************************************************//**
  The I3C_private_write() function initiates an I3C controller private write
  transaction. It writes data from write_buffer to the CoreI3C Write FIFO and
  submits a private write command descriptor to the Command FIFO. The function
  returns after initiating the transfer. The caller must not modify or free
  write_buffer until the transfer is confirmed complete via the Status FIFO or
  the I3C_CR_IRQ_CMD_RDY interrupt.

  @param this_i3c
    Pointer to an initialized i3c_instance_t structure.

  @param dct_index
    DCT address of the target device.

  @param reg_addr
    Address offset.

  @param p_data
    Destination data buffer.

   @param num_bytes
    Number of bytes to transmit from write_buffer.

  @return
    Returns I3C_SUCCESS on successful command submission.
    Returns I3C_ERR_FIFO_FULL if Write or Command FIFO is full.
    Returns I3C_ERR_NOT_CTRL if not the active controller.
    Returns I3C_ERR_PARAM if write_buffer is NULL with write_size > 0.

  @example
  @code
      uint8_t tx_buf[4] = {0xDE, 0xAD, 0xBE, 0xEF};
      I3C_write(&g_i3c, 0x12u, tx_buf, 4u,
                I3C_OPT_EMIT_STOP | I3C_OPT_RESPONSE);
  @endcode
*/
i3c_status_t
I3C_private_write
(
    i3c_instance_t       * this_i3c,
    uint8_t dct_index,
    uint8_t reg_addr,
    const uint8_t * p_data,
    uint8_t num_bytes
);
/***************************************************************************//**
  The I3C_write_read() function initiates an I3C controller write-then-read
  transaction. It first writes the addr_offset bytes to the target, then
  issues a repeated-START and reads back read_size bytes into read_buffer.
  This is the standard register read access pattern for I3C target devices.

  @param this_i3c
    Pointer to an initialized i3c_instance_t structure.

  @param serial_addr
    7-bit dynamic address of the target device.

  @param addr_offset
    Pointer to the register address / offset bytes to write first.

  @param offset_size
    Number of bytes in addr_offset.

  @param read_buffer
    Pointer to a buffer to receive the read data.

  @param read_size
    Maximum number of bytes to read.

  @param read_len
    Pointer to a uint32_t that receives the actual number of bytes read.

  @param options
    Transfer option flags.

  @return
    Returns I3C_SUCCESS on successful completion.
    Returns I3C_ERR_PARAM if any required pointer is NULL.
    Returns I3C_ERR_FIFO_FULL if a FIFO is full.
    Returns I3C_ERR_NOT_CTRL if not the active controller.

  @example
  @code
      uint8_t reg_addr[1] = {0x20u};
      uint8_t reg_data[2];
      uint32_t rx_len = 0u;
      I3C_write_read(&g_i3c, 0x12u, reg_addr, 1u,
                     reg_data, 2u, &rx_len, I3C_OPT_EMIT_STOP);
  @endcode
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
);

/***************************************************************************//**
  The I3C_read_response() function reads one response descriptor from the
  CoreI3C Status FIFO. The response descriptor contains the transfer result,
  error type, transaction ID, and data length of the most recently completed
  command.

  Call this function after receiving the I3C_CR_IRQ_RESP_RDY interrupt or
  after polling I3C_get_irq_status() for the RESP_RDY bit.

  @param this_i3c
    Pointer to an initialized i3c_instance_t structure.

  @param response
    Pointer to a uint32_t to receive the raw 32-bit response descriptor.
    Must not be NULL.

  @return
    Returns the number of response entries read (0 or 1).
    Returns 0 if the Status FIFO is empty.

  @example
  @code
      uint32_t resp = 0u;
      uint32_t n = I3C_read_response(&g_i3c, &resp);
      if (n > 0u)
      {
          uint8_t err = (resp >> 28u) & 0xFu;
          uint8_t tid = (resp >> 24u) & 0xFu;
      }
  @endcode
*/
uint32_t
I3C_read_response
(
    i3c_instance_t * this_i3c,
    uint32_t       * response
);

/***************************************************************************//**
  The I3C_read_ibi() function reads one In-Band Interrupt descriptor from the
  CoreI3C IBI port register. The IBI descriptor includes the requesting target
  address and any associated IBI payload data.

  Call this function after receiving the I3C_CR_IRQ_IBI_RECV interrupt or
  after polling I3C_get_irq_status() for the IBI_RECV bit.

  @param this_i3c
    Pointer to an initialized i3c_instance_t structure.

  @param ibi_descriptor
    Pointer to a uint32_t to receive the raw 32-bit IBI descriptor.
    Must not be NULL.

  @return
    Returns the number of IBI entries read (0 or 1).
    Returns 0 if no IBI is pending (IBI_RECV bit not set).

  @example
  @code
      uint32_t ibi = 0u;
      uint32_t n = I3C_read_ibi(&g_i3c, &ibi);
      if (n > 0u)
      {
          // Process IBI descriptor
      }
  @endcode
*/
uint32_t
I3C_read_ibi
(
    i3c_instance_t * this_i3c,
    uint32_t       * ibi_descriptor
);

/***************************************************************************//**
  The I3C_get_status() function returns the current operational state of the
  CoreI3C controller by reading the PRESENT_STATE and INTR_STAT registers. The
  returned value includes the active controller flag, command sequence timeout
  state, and internal error flags.

  @param this_i3c
    Pointer to an initialized i3c_instance_t structure.

  @return
    Returns the 32-bit PRESENT_STATE register value. The caller can inspect
    individual bits using the COREI3C_STATE_* constants from corei3c_regs.h.

  @example
  @code
      uint32_t state = I3C_get_status(&g_i3c);
      if (state & COREI3C_STATE_CURRENT_CTRL_MASK)
      {
          // This instance is the active controller
      }
  @endcode
*/
uint32_t
I3C_get_status
(
    i3c_instance_t * this_i3c
);

/***************************************************************************//**
  The I3C_enable_irq() function enables one or more CoreI3C interrupt sources
  for both status logging (visible in the status registers) and signal
  assertion (asserts the INT_O output pin to the host CPU). The irq_mask
  parameter specifies a bitmask of interrupt sources to enable using the
  I3C_IRQ_* and I3C_CR_IRQ_* constants defined in corei3c_regs.h.

  @param this_i3c
    Pointer to an initialized i3c_instance_t structure.

  @param irq_mask
    Bitmask of interrupt sources to enable. OR of I3C_IRQ_* and I3C_CR_IRQ_*
    constants. Bits [15:0] correspond to INTR_STAT_EN / INTR_SIG_EN. Bits
    [31:16] correspond to CMND_RSPNS_INTR_STAT_EN / CMND_RSPNS_INTR_SIG_EN.

  @return
    This function does not return a value.

  @example
  @code
      I3C_enable_irq(&g_i3c,
                     I3C_IRQ_CMD_TIMEOUT    |
                     I3C_IRQ_INTERNAL_ERR   |
                     I3C_CR_IRQ_CMD_RDY     |
                     I3C_CR_IRQ_RESP_RDY    |
                     I3C_CR_IRQ_IBI_RECV);
  @endcode
*/
void
I3C_enable_irq
(
    i3c_instance_t * this_i3c,
    uint32_t         irq_mask
);

/***************************************************************************//**
  The I3C_disable_irq() function disables one or more CoreI3C interrupt sources.
  Both status logging and INT_O signal assertion are disabled for the specified
  interrupt sources.

  @param this_i3c
    Pointer to an initialized i3c_instance_t structure.

  @param irq_mask
    Bitmask of interrupt sources to disable. OR of I3C_IRQ_* and I3C_CR_IRQ_*
    constants.

  @return
    This function does not return a value.

  @example
  @code
      I3C_disable_irq(&g_i3c, I3C_CR_IRQ_WRT_AVAIL | I3C_CR_IRQ_RD_AVAIL);
  @endcode
*/
void
I3C_disable_irq
(
    i3c_instance_t * this_i3c,
    uint32_t         irq_mask
);

/***************************************************************************//**
  The I3C_clear_irq() function clears one or more pending CoreI3C interrupt
  status bits by writing 1 to the corresponding RW1C bit positions in the
  INTR_STAT and CMND_RSPNS_INTR_STAT registers.

  @param this_i3c
    Pointer to an initialized i3c_instance_t structure.

  @param irq_mask
    Bitmask of interrupt sources to clear. OR of I3C_IRQ_* and I3C_CR_IRQ_*
    constants.

  @return
    This function does not return a value.

  @example
  @code
      void i3c_isr(void)
      {
          uint32_t pending = I3C_get_irq_status(&g_i3c);
          if (pending & I3C_CR_IRQ_CMD_RDY)
          {
              // handle command complete
              I3C_clear_irq(&g_i3c, I3C_CR_IRQ_CMD_RDY);
          }
      }
  @endcode
*/
void
I3C_clear_irq
(
    i3c_instance_t * this_i3c,
    uint32_t         irq_mask
);

/***************************************************************************//**
  The I3C_get_irq_status() function returns the current combined interrupt
  status of the CoreI3C controller. The return value encodes both the
  general interrupt status (INTR_STAT register, bits [15:0]) and the
  command/response interrupt status (CMND_RSPNS_INTR_STAT, bits [31:16]),
  matching the encoding used by irq_mask in I3C_enable_irq() / I3C_clear_irq().

  @param this_i3c
    Pointer to an initialized i3c_instance_t structure.

  @return
    Combined 32-bit interrupt status. Compare with I3C_IRQ_* and I3C_CR_IRQ_*
    constants to determine which sources are pending.

  @example
  @code
      uint32_t status = I3C_get_irq_status(&g_i3c);
      if (status & I3C_CR_IRQ_RESP_RDY)
      {
          uint32_t resp;
          I3C_read_response(&g_i3c, &resp);
      }
  @endcode
*/
uint32_t
I3C_get_irq_status
(
    i3c_instance_t * this_i3c
);

static inline void
I3C_raw_private_read
(
        i3c_instance_t  * this_i3c,
        uint8_t dct_index,
        uint8_t reg_addr,
        const uint8_t *p_data,
        uint8_t num_bytes
);


#ifdef __cplusplus
}
#endif

#endif /* CORE_I3C_H_ */
