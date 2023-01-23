/*******************************************************************************
 * Copyright 2022 Microchip FPGA Embedded Systems Solutions.
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
 * This file contains the application programming interface for the MI-V Soft IP
 * uDMA module driver. This module is delivered as a part of Mi-V Extended
 * Sub-System(MIV_ESS).
 */

/*=========================================================================*//**
  @mainpage Mi-V uDMA Bare Metal Driver.
  The Mi-V uDMA bare metal software driver.

  @section intro_sec Introduction
  The Mi-V uDMA driver provides a set of functions to control the Mi-V uDMA
  module in the Mi-V Extended Sub System(MIV_ESS) soft-IP. The Mi-V uDMA module
  allows peripherals with AHB interfaces to transfer data independently of the
  MIV_RV32 RISC-V processor.

  The major features provided by the Mi-V uDMA driver are:
     - Initialization and configuration
     - Start and reset the transaction

  This driver can be used as part of a bare metal system where no operating
  system  is available. The driver can be adapted for use as part of an
  operating system, but the implementation of the adaptation layer between the
  driver and  the operating system's driver model is outside the scope of this
  driver.

  @section hw_dependencies Hardware Flow Dependency
  The application software should initialize and configure the Mi-V uDMA through
  the call to MIV_uDMA_init(), MIV_uDMA_config() function for Mi-V uDMA
  instance in the design.
  The μDMA can operate in two possible transfer configurations:

   AHBL Read –> AHBL Write:
   In this configuration, the μDMA reads data from the source memory over an
   AHBL (Mirrored Main/Initiator) read interface and writes data to the
   destination memory over an AHBL (Mirrored Main/Initiator) write interface.

   AHBL Read –> TAS Write:
   In this configuration, the μDMA reads data from the source memory over an
   AHBL (Mirrored Main/Initiator) read interface and writes data to the
   destination memory over the TAS (Mirrored Main/Initiator) write interface.

   The AHBL Read -> TAS Write configuration is out of scope for this driver.

   @section theory_op Theory of Operation
   The uDMA module in the Mi-V Extended Sub System(MIV_ESS) is a single channel
   uDMA module that allows peripherals to perform read write operations between
   source and destination memory. The Mi-V uDMA driver is generally used in
   interrupt driven mode and uses the Mi-V uDMA IRQ signal to drive the
   interrupt service routine(ISR) which signifies a transfer has completed.
   The status is checked in the ISR to ensure the transfer is completed
   successfully.
   The reset operation in the ISR will reset the Mi-V uDMA controller.
   Once the Mi-V uDMA transfer completes, Mi-V uDMA retires. To
   initiate another transaction, Mi-V uDMA will have to be configured again.

   The operation of the Mi-V uDMA driver can be divided into following
   categories:
       - Initialization
       - Configuration
       - Start and reset transfer

   ### Initialization and configuration
   Mi-V uDMA is first initialized by a call to MIV_uDMA_init(). This function
   initializes the instance of Mi-V uDMA with base address. The MIV_uDMA_init()
   function must be called before calling any other Mi-V uDMA driver functions.

   The Mi-V uDMA is configured by a call to MIV_uDMA_config(). This function
   will configure the source_addr and dest_addr registers of the Mi-V uDMA with
   source and destination addresses for Mi-V uDMA transfers.
   This function also configures the transfer size and interrupt preference for
   successful transfer of Mi-V uDMA.

   ### Start and Reset transfer
   Once the Mi-V uDMA is configured, the transfers can be started with a call to
   MIV_uDMA_start(). Once the Mi-V uDMA transfer is started, it can not be
   aborted and the status of the transfer should be read from the ISR by a call
   to MIV_uDMA_read_status().

   The Mi-V uDMA can be reset to the default state by calling MIV_uDMA_reset()
   function. After performing reset operation, the Mi-V uDMA should be
   re-configured to perform transfer since MIV_uDMA_reset() resets the Mi-V uDMA
   controller.
 */

#ifndef MIV_uDMA_H_
#define MIV_uDMA_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef LEGACY_DIR_STRUCTURE
#include "hal/hal.h"
#include "hal/cpu_types.h"

#else
#include "hal.h"
#include "cpu_types.h"
#endif

/*-------------------------------------------------------------------------*//**
 MIV_uDMA_CTRL_IRQ_CONFIG
 =====================

 The MIV_uDMA_CTRL_IRQ_CONFIG macro is used to assert the uDMA IRQ when an error
 occurs during a uDMA transfer or on the completion of a uDMA transfer.
 */
#define MIV_uDMA_CTRL_IRQ_CONFIG         1u

/*-------------------------------------------------------------------------*//**
  MIV_uDMA_STATUS_BUSY
  =====================

  The MIV_uDMA_STATUS_BUSY macro is used to indicate that the uDMA transfer is
  in progress.
  */
#define MIV_uDMA_STATUS_BUSY             1u

/*-------------------------------------------------------------------------*//**
  MIV_uDMA_STATUS_ERROR
  =====================

  The MIV_uDMA_STATUS_ERROR macro is used to indicate that the last uDMA
  transfer has caused an error.
  */
#define MIV_uDMA_STATUS_ERROR            2u

/***************************************************************************//**
 * Mi-V uDMA instant structure.
 * This structure will hold the base of Mi-V uDMA module which is used in the
 * other functions in the driver to access the uDMA registers.
 */
typedef struct miv_udma_instance
{
    addr_t base_address;
} miv_udma_instance_t;

/***************************************************************************//**
 * The MIV_uDMA_init() assigns the base address of Mi-V uDMA module to the
 * uDMA instance structure.
 * This address is used in later part of the driver to access the uDMA registers.
 *
 * @param this_udma
 *      This parameter is a pointer to the miv_udma_instance_t structure.
 *
 * @param base_addr
 *      Base address of the Mi-V uDMA module.
 *
 * @return
 *      This function does not return a value.
 */
void
MIV_uDMA_init
(
    miv_udma_instance_t* this_udma,
    addr_t base_addr
);

/***************************************************************************//**
 * The MIV_uDMA_config() is used to configure the Mi-V uDMA controller.
 * This function will set the source address, destination address, block size
 * and IRQ config register.
 *
 * @param this_udma
 *      This parameter is a pointer to the miv_udma_instance_t structure which
 *      holds the base address of Mi-V uDMA module.
 *
 * @param base_addr
 *      Base address of the Mi-V uDMA
 *
 * @param src_addr
 *      Source address of memory from which the uDMA will read the data.
 *
 * @param dest_addr
 *      Destination address where the data will be written from src_addr.
 *
 * @param transfer_size
 *      Number of 32-bit words to transfer
 *
 * @param irq_config
 *      uDMA IRQ Configuration.
 *
 *      When set, the IRQ is asserted when and error occurs during a uDMA
 *      transfer or on the completion of the uDMA transfer.
 *
 *      When clear, the IRQ is only asserted when an error occurs during a uDMA
 *      transfer.
 *
 * @return
 *      This function does not return any value.
 */
void
MIV_uDMA_config
(
    miv_udma_instance_t* this_udma,
    addr_t src_addr,
    addr_t dest_addr,
    uint32_t transfer_size,
    uint32_t irq_config
);

/***************************************************************************//**
 * The MIV_uDMA_start() is used to start the uDMA transfer.
 *
 * @param this_udma
 *      This parameter is a pointer to the miv_udma_instance_t structure which
 *      holds the base address of Mi-V uDMA module.
 *
 * @return
 *      This function does not return any value.
 */
void
MIV_uDMA_start
(
    miv_udma_instance_t* this_udma
);

/***************************************************************************//**
 * The MIV_uDMA_reset() is used to clear the uDMA interrupt and reset the uDMA
 * transfer.
 *
 * This function should be called from interrupt handler and it will reset the
 * values set during MIV_uDMA_config().
 *
 * @param this_udma
 *      This parameter is a pointer to the miv_udma_instance_t structure which
 *      holds the base address of Mi-V uDMA module.
 *
 * @return
 *      This function does not return any value.
 */
void
MIV_uDMA_reset
(
    miv_udma_instance_t* this_udma
);

/***************************************************************************//**
 * The MIV_uDMA_read_status() will be used to status of the uDMA transfer. When
 * Interrupt is enabled this function can be called from the interrupt handler
 * to know the reason of uDMA interrupt.
 *
 * @param this_udma
 *      This parameter is a pointer to the miv_udma_instance_t structure which
 *      holds the base address of Mi-V uDMA module.
 *
 * @return
 * Return value will indicate error of busy status of the uDMA channel.
 *
 * |Bit Number| Name    |           Description                                |
 * |----------|---------|------------------------------------------------------|
 * |   0      | Busy    |  When set indicates that uDMA transfer is in progress|
 * |   1      | Error   |  When set indicates that last uDMA transfer caused an|
 * |          |         |  error.                                              |
 */
uint32_t
MIV_uDMA_read_status
(
    miv_udma_instance_t* this_pdma
);

#ifdef __cplusplus
}
#endif

#endif  /* MIV_uDMA_H_ */
