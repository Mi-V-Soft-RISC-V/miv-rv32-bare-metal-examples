/*******************************************************************************
 * Copyright 2026 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file core_mdio.h
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief CoreMDIO_APB bare metal driver, IEEE 802.3 Clause 22 access.
 *
 * Used by the CoreQSE 1 Gbps reference design to manage the external Ethernet
 * PHY. The 2.5 Gbps design has no PHY, since its loopback is a direct SMA
 * cable, and therefore no MDIO instance.
 *
 * Register offsets and the busy-polling protocol were taken from the reference
 * application at Application/CoreQSE_1G/application/main.c, which accesses the
 * peripheral directly. This driver wraps that access with parameter checking
 * and bounded waits: the reference code spins on the status register with no
 * timeout, so an absent or unresponsive PHY hangs the processor permanently.
 *
 * CoreQSE itself has no MDIO interface. PHY management is entirely separate
 * from MAC configuration; this driver does not touch CoreQSE registers.
 */

#ifndef CORE_MDIO_H_
#define CORE_MDIO_H_

#include <stdint.h>

#ifndef LEGACY_DIR_STRUCTURE
#include "hal/hal.h"
#include "hal/hal_assert.h"
#else
#include "hal.h"
#include "hal_assert.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------------
 * Return codes.
 */
#define MDIO_OK                     ( 0u )  /*!< Operation successful.        */
#define MDIO_ERR_PARAM              ( 1u )  /*!< Invalid parameter.           */
#define MDIO_ERR_TIMEOUT            ( 2u )  /*!< PHY did not become ready.    */

/*! Every register read returned the SAME value, so the interface is returning a
 *  constant rather than register specific data. Reported by MDIO_self_test().
 *  This is the signature of a stalled DATAOUT register or of transactions that
 *  never actually complete.
 */
#define MDIO_ERR_CONSTANT           ( 3u )

/*! No PHY is responding: reads return all ones, which is the idle state of the
 *  MDIO line, or the PHY identifier is all zeroes or all ones.
 */
#define MDIO_ERR_NO_PHY             ( 4u )

/*------------------------------------------------------------------------------
 * Standard IEEE 802.3 Clause 22 PHY registers.
 */
#define MDIO_PHY_REG_CONTROL        ( 0x00u ) /*!< Basic control.             */
#define MDIO_PHY_REG_STATUS         ( 0x01u ) /*!< Basic status.              */
#define MDIO_PHY_REG_ID1            ( 0x02u ) /*!< PHY identifier 1.          */
#define MDIO_PHY_REG_ID2            ( 0x03u ) /*!< PHY identifier 2.          */
#define MDIO_PHY_REG_AN_ADV         ( 0x04u ) /*!< Auto-neg advertisement.    */
#define MDIO_PHY_REG_AN_LP_ABILITY  ( 0x05u ) /*!< Link partner ability.      */
#define MDIO_PHY_REG_1000T_CTRL     ( 0x09u ) /*!< 1000BASE-T control.        */
#define MDIO_PHY_REG_1000T_STATUS   ( 0x0Au ) /*!< 1000BASE-T status.         */
#define MDIO_PHY_REG_PAGE_SELECT    ( 0x16u ) /*!< Vendor page select.        */

/*------------------------------------------------------------------------------
 * PHY basic control register bits, register 0x00.
 */
#define MDIO_CTRL_RESET             ( 1u << 15u ) /*!< Software reset.        */
#define MDIO_CTRL_LOOPBACK          ( 1u << 14u ) /*!< Internal loopback.     */
#define MDIO_CTRL_SPEED_LSB         ( 1u << 13u ) /*!< Speed select LSB.      */
#define MDIO_CTRL_AN_ENABLE         ( 1u << 12u ) /*!< Auto-neg enable.       */
#define MDIO_CTRL_POWER_DOWN        ( 1u << 11u ) /*!< Power down.            */
#define MDIO_CTRL_ISOLATE           ( 1u << 10u ) /*!< Electrical isolate.    */
#define MDIO_CTRL_AN_RESTART        ( 1u <<  9u ) /*!< Restart auto-neg.      */
#define MDIO_CTRL_FULL_DUPLEX       ( 1u <<  8u ) /*!< Full duplex.           */
#define MDIO_CTRL_SPEED_MSB         ( 1u <<  6u ) /*!< Speed select MSB.      */

/*! 1000 Mbps, full duplex, auto-negotiation enabled.
 *  Speed is encoded as {MSB, LSB} = {1,0}. This is the 0x1140 value the
 *  reference application writes.
 */
#define MDIO_CTRL_1000_FD_AN        ( MDIO_CTRL_SPEED_MSB    | \
                                      MDIO_CTRL_AN_ENABLE    | \
                                      MDIO_CTRL_FULL_DUPLEX )

/*------------------------------------------------------------------------------
 * PHY basic status register bits, register 0x01.
 */
#define MDIO_STAT_100T4             ( 1u << 15u )
#define MDIO_STAT_100_FD            ( 1u << 14u )
#define MDIO_STAT_100_HD            ( 1u << 13u )
#define MDIO_STAT_10_FD             ( 1u << 12u )
#define MDIO_STAT_10_HD             ( 1u << 11u )
#define MDIO_STAT_EXT_STATUS        ( 1u <<  8u )
#define MDIO_STAT_AN_COMPLETE       ( 1u <<  5u ) /*!< Auto-neg complete.     */
#define MDIO_STAT_REMOTE_FAULT      ( 1u <<  4u )
#define MDIO_STAT_AN_ABILITY        ( 1u <<  3u )
#define MDIO_STAT_LINK_UP           ( 1u <<  2u ) /*!< Link status. Latching
                                                       low: read twice.       */
#define MDIO_STAT_JABBER            ( 1u <<  1u )
#define MDIO_STAT_EXT_CAPABILITY    ( 1u <<  0u )

/*------------------------------------------------------------------------------
 * Auto-negotiation advertisement register bits, register 0x04.
 *
 * These advertise the 10 and 100 Mbps abilities. 1000 Mbps is advertised in
 * register 0x09 instead, not here.
 */
#define MDIO_AN_ADV_100_FD          ( 1u <<  8u ) /*!< 100BASE-TX full duplex.*/
#define MDIO_AN_ADV_100_HD          ( 1u <<  7u ) /*!< 100BASE-TX half duplex.*/
#define MDIO_AN_ADV_10_FD           ( 1u <<  6u ) /*!< 10BASE-T full duplex.  */
#define MDIO_AN_ADV_10_HD           ( 1u <<  5u ) /*!< 10BASE-T half duplex.  */

/*------------------------------------------------------------------------------
 * 1000BASE-T control register bits, register 0x09.
 *
 * This is where 1000 Mbps ability is advertised. If neither bit is set the PHY
 * will NOT negotiate a gigabit link regardless of what register 0x00 requests.
 */
#define MDIO_1000T_CTRL_ADV_1000_FD ( 1u <<  9u ) /*!< Advertise 1000 full.   */
#define MDIO_1000T_CTRL_ADV_1000_HD ( 1u <<  8u ) /*!< Advertise 1000 half.   */

/*------------------------------------------------------------------------------
 * 1000BASE-T status register bits, register 0x0A.
 */
#define MDIO_1000T_LP_1000_FD       ( 1u << 11u ) /*!< Partner 1000 full dup. */
#define MDIO_1000T_LP_1000_HD       ( 1u << 10u ) /*!< Partner 1000 half dup. */
#define MDIO_1000T_LOCAL_RX_OK      ( 1u << 13u )
#define MDIO_1000T_REMOTE_RX_OK     ( 1u << 12u )

/*------------------------------------------------------------------------------
 * Driver instance.
 */
typedef struct MDIO_instance
{
    addr_t base_address;    /*!< Base address of the CoreMDIO_APB instance.   */
} MDIO_instance_t;

/*------------------------------------------------------------------------------
 * Progress callback for MDIO_wait_link_up().
 *
 * Auto-negotiation takes seconds, so a silent wait looks like a hang. This
 * callback lets the application report progress as it happens, which also makes
 * a wrong cable or a wrong RJ45 socket obvious: the status word changes the
 * moment a real link partner is present.
 *
 * @param elapsed_ms
 *   Approximate milliseconds waited so far.
 *
 * @param status_reg
 *   Raw contents of PHY register 0x01.
 *
 * @param link_up
 *   1 when the link status bit is set.
 *
 * @param an_complete
 *   1 when the auto-negotiation complete bit is set.
 */
typedef void (*MDIO_link_progress_t)( uint32_t elapsed_ms,
                                      uint16_t status_reg,
                                      uint8_t  link_up,
                                      uint8_t  an_complete );

/***************************************************************************//**
  MDIO_init() initializes the driver and programs the MDIO clock prescaler.

  The prescaler divides the APB clock to produce MDC. IEEE 802.3 limits MDC to
  2.5 MHz. The reference application uses a value of 7.

  @param this_mdio
    Points to the MDIO_instance_t structure to initialize.

  @param base_addr
    Base address of the CoreMDIO_APB instance. 0x60000000 in the 1 Gbps design.

  @param prescaler
    MDIO clock prescaler value.

  @return
    MDIO_OK, or MDIO_ERR_PARAM if this_mdio is NULL or base_addr is 0.
 */
uint8_t MDIO_init( MDIO_instance_t * this_mdio,
                   addr_t            base_addr,
                   uint32_t          prescaler );

/***************************************************************************//**
  MDIO_read() performs a Clause 22 read of a PHY register.

  @warning
  The reference application polls the status register with an unbounded loop,
  which hangs the processor if the PHY does not respond. This function applies a
  bounded wait and reports MDIO_ERR_TIMEOUT instead.

  @param this_mdio
    Points to an initialized MDIO_instance_t.

  @param phy_addr
    PHY address, 0 to 31.

  @param reg_addr
    PHY register address, 0 to 31.

  @param data
    Receives the 16-bit register value. Unmodified on failure.

  @return
    MDIO_OK,
    MDIO_ERR_PARAM if a pointer is NULL or an address is out of range,
    MDIO_ERR_TIMEOUT if the PHY did not complete the transaction.

  @code
  uint16_t id1;
  if (MDIO_read(&g_mdio, 0u, MDIO_PHY_REG_ID1, &id1) == MDIO_OK) {
      // id1 holds the upper PHY identifier
  }
  @endcode
 */
uint8_t MDIO_read( const MDIO_instance_t * this_mdio,
                   uint8_t                 phy_addr,
                   uint8_t                 reg_addr,
                   uint16_t              * data );

/***************************************************************************//**
  MDIO_write() performs a Clause 22 write to a PHY register.

  @param this_mdio
    Points to an initialized MDIO_instance_t.

  @param phy_addr
    PHY address, 0 to 31.

  @param reg_addr
    PHY register address, 0 to 31.

  @param data
    16-bit value to write.

  @return
    MDIO_OK,
    MDIO_ERR_PARAM if this_mdio is NULL or an address is out of range,
    MDIO_ERR_TIMEOUT if the PHY did not complete the transaction.
 */
uint8_t MDIO_write( const MDIO_instance_t * this_mdio,
                    uint8_t                 phy_addr,
                    uint8_t                 reg_addr,
                    uint16_t                data );

/***************************************************************************//**
  MDIO_self_test() verifies that the MDIO interface returns register specific
  data, and must be called before trusting any other read.

  It reads four registers that cannot legitimately hold the same value: CONTROL,
  STATUS, ID1 and ID2. If all four return an identical value, the interface is
  returning a constant and MDIO_ERR_CONSTANT is reported.

  This check exists because a constant read is easy to mistake for success. A
  value such as 0x0DC0 has plausible looking bits set, so individual field
  checks on it can appear to pass while carrying no information whatsoever about
  the PHY. Calling this function first turns that into one unambiguous failure.

  @param this_mdio
    Points to an initialized MDIO_instance_t.

  @param phy_addr
    PHY address, 0 to 31.

  @param readings
    Optional four element array receiving the raw values read from CONTROL,
    STATUS, ID1 and ID2 in that order, so the caller can print them. Zeroed if
    a read failed. May be NULL.

  @return
    MDIO_OK if the four registers differ and the identifier is plausible,
    MDIO_ERR_CONSTANT if all four returned the same value,
    MDIO_ERR_NO_PHY if nothing is driving the line or the identifier is
    all zeroes or all ones,
    MDIO_ERR_TIMEOUT or MDIO_ERR_PARAM as for MDIO_read().

  @code
  uint16_t regs[4];
  uint8_t  st = MDIO_self_test(&g_mdio, 0u, regs);

  if (st == MDIO_ERR_CONSTANT) {
      // MDIO is not functioning; do not interpret any PHY register
  }
  @endcode
 */
uint8_t MDIO_self_test( const MDIO_instance_t * this_mdio,
                        uint8_t                 phy_addr,
                        uint16_t                readings[4] );

/***************************************************************************//**
  MDIO_get_phy_id() reads the two PHY identifier registers and returns the
  combined 32-bit identifier, ID1 in the upper half and ID2 in the lower.

  A result of 0x00000000 or 0xFFFFFFFF means no PHY responded at that address,
  which usually indicates a wrong PHY address, an unpowered PHY, or MDIO wiring
  that is not connected.

  @param this_mdio
    Points to an initialized MDIO_instance_t.

  @param phy_addr
    PHY address, 0 to 31.

  @param phy_id
    Receives the combined identifier.

  @return
    MDIO_OK, MDIO_ERR_PARAM, or MDIO_ERR_TIMEOUT.
 */
uint8_t MDIO_get_phy_id( const MDIO_instance_t * this_mdio,
                         uint8_t                 phy_addr,
                         uint32_t              * phy_id );

/***************************************************************************//**
  MDIO_phy_reset() issues a software reset to the PHY and waits for the reset
  bit to self clear.

  @param this_mdio
    Points to an initialized MDIO_instance_t.

  @param phy_addr
    PHY address, 0 to 31.

  @return
    MDIO_OK, MDIO_ERR_PARAM, or MDIO_ERR_TIMEOUT if the reset bit did not
    clear.
 */
uint8_t MDIO_phy_reset( const MDIO_instance_t * this_mdio,
                        uint8_t                 phy_addr );

/***************************************************************************//**
  MDIO_phy_config_1000_fd() configures the PHY for 1000 Mbps full duplex with
  auto-negotiation, matching the sequence used by the reference application:
  select page 0, reset with the 1000 Mbps setting, then release reset.

  @param this_mdio
    Points to an initialized MDIO_instance_t.

  @param phy_addr
    PHY address, 0 to 31.

  @return
    MDIO_OK, MDIO_ERR_PARAM, or MDIO_ERR_TIMEOUT.
 */
uint8_t MDIO_phy_config_1000_fd( const MDIO_instance_t * this_mdio,
                                 uint8_t                 phy_addr );

/***************************************************************************//**
  MDIO_get_link_status() reports whether the PHY link is up and whether
  auto-negotiation has completed.

  @note
  The link status bit in register 0x01 latches low, so a transient loss is
  reported until the register is read. This function reads the register twice
  and returns the second value, which reflects the current state.

  @param this_mdio
    Points to an initialized MDIO_instance_t.

  @param phy_addr
    PHY address, 0 to 31.

  @param link_up
    Receives 1 when the link is up, 0 otherwise. May be NULL.

  @param an_complete
    Receives 1 when auto-negotiation has completed, 0 otherwise. May be NULL.

  @return
    MDIO_OK, MDIO_ERR_PARAM, or MDIO_ERR_TIMEOUT.
 */
uint8_t MDIO_get_link_status( const MDIO_instance_t * this_mdio,
                             uint8_t                 phy_addr,
                             uint8_t               * link_up,
                             uint8_t               * an_complete );

/***************************************************************************//**
  MDIO_wait_link_up() polls the PHY until the link comes up or the wait expires.

  @warning
  Auto-negotiation for 1000BASE-T takes 1 to 3 SECONDS. IEEE 802.3 defines a
  break_link_timer of 1.2 to 1.5 s on its own, before the page exchange and
  master/slave resolution. Allow at least 3000 ms, and preferably 5000 ms.

  An earlier version of this function took a raw iteration count, and callers
  passed 200. At a 50 MHz PCLK with prescaler 7 that is only about 7 ms, roughly
  400 times too short: a perfectly working link could never pass. The parameter
  is now expressed in milliseconds so the unit matches the specification.

  @param this_mdio
    Points to an initialized MDIO_instance_t.

  @param phy_addr
    PHY address, 0 to 31.

  @param wait_ms
    Approximate maximum wait in milliseconds. Use 5000 for 1000BASE-T.

  @param progress_cb
    Called about every 250 ms with the current status, so the application can
    show that negotiation is progressing rather than appearing to hang. Also
    called once on exit. May be NULL.

  @return
    MDIO_OK if the link came up, MDIO_ERR_TIMEOUT if it did not within wait_ms,
    MDIO_ERR_PARAM on a bad parameter.

  @code
  static void on_progress(uint32_t ms, uint16_t stat, uint8_t up, uint8_t an)
  {
      printf("  %lu ms STATUS=0x%04X link=%u an=%u\r\n", ms, stat, up, an);
  }

  MDIO_wait_link_up(&g_mdio, 0u, 5000u, on_progress);
  @endcode
 */
uint8_t MDIO_wait_link_up( const MDIO_instance_t * this_mdio,
                           uint8_t                 phy_addr,
                           uint32_t                wait_ms,
                           MDIO_link_progress_t    progress_cb );

#ifdef __cplusplus
}
#endif

#endif /* CORE_MDIO_H_ */
