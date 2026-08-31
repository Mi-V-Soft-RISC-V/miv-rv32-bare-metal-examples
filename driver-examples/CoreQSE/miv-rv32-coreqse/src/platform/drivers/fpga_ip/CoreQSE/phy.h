/*******************************************************************************
 * Copyright 2026 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file phy.h
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief PHY abstraction layer for CoreQSE designs that use an external
 *        Ethernet PHY managed over CoreMDIO_APB.
 *
 * PURPOSE
 * -------
 * The IEEE 802.3 Clause 22 register set is standard only for registers 0x00 to
 * 0x0F. Speed and duplex resolution, LED control, cable diagnostics and page
 * selection are all VENDOR SPECIFIC and differ between PHY parts.
 *
 * Rather than accumulate vendor conditionals inside the MDIO driver, this file
 * declares a small interface that each supported PHY implements in its own
 * source file. The same approach is used by the CoreTSE driver in phy.h.
 *
 * ONLY ONE IMPLEMENTATION SHIPS WITH THIS EXAMPLE
 * -----------------------------------------------
 *   null_phy.c    generic IEEE 802.3 Clause 22, no vendor registers
 *
 * That is sufficient to bring up a 1000BASE-T link on any conforming PHY, and
 * is what the 1 Gbps reference design uses. Vendor specific drivers are
 * deliberately NOT included here; see "adding a vendor PHY" below.
 *
 * ADDING A VENDOR PHY
 * -------------------
 * The CoreTSE driver ships tested implementations behind this same interface:
 *
 *   vsc8575.c         Microchip VSC8575
 *   m88e1340_phy.c    Marvell M88E1340
 *   m88e1111_phy.c    Marvell M88E1111
 *
 * To use one with CoreQSE:
 *
 *   1. Copy the file into this directory.
 *   2. Replace TSE_read_phy_reg and TSE_write_phy_reg with MDIO_read and
 *      MDIO_write. Note the different signatures: the MDIO functions return a
 *      status and deliver data through a pointer.
 *   3. Rename the TSE_phy_* entry points to the PHY_* names declared below, and
 *      add the wait_ms and progress_cb parameters to PHY_autonegotiate().
 *   4. Define the corresponding selection macro in fpga_design_config.h, for
 *      example VSC8575_PHY or M88E1340_PHY.
 *
 * null_phy.c guards itself out when either macro is defined, so exactly one
 * implementation is ever compiled.
 *
 * A vendor driver is worth the effort when you need the resolved link speed and
 * duplex read directly from the PHY, LED control, cable diagnostics, or SERDES
 * and MAC interface configuration. None of those are reachable through the
 * standard Clause 22 registers.
 *
 * DESIGNS WITH NO PHY
 * -------------------
 * The 2.5 Gbps reference design connects the transceiver to an SMA cable
 * directly and has no PHY at all. The application simply does not call
 * QSE_attach_mdio(), and the QSE_phy_* functions then report
 * QSE_ERR_NOT_SUPPORTED.
 *
 * WHY A NULL PHY MATTERS HERE
 * ---------------------------
 * The two CoreQSE reference designs differ:
 *
 *   2.5 Gbps   COREQSE -> PF_XCVR_ERM -> SMA cable. NO PHY, no MDIO instance.
 *   1   Gbps   COREQSE -> PF_XCVR_ERM -> external PHY -> RJ45 -> test set.
 *
 * Without a null implementation the 2.5 Gbps build would fail to link against
 * PHY functions that have no hardware behind them.
 *
 * RELATIONSHIP TO core_mdio.h
 * ---------------------------
 * core_mdio.h provides the TRANSPORT: raw Clause 22 register read and write
 * over CoreMDIO_APB, plus the interface coherency self test. This file provides
 * the POLICY: what to write, in what order, and how to interpret what comes
 * back, for a particular PHY part.
 */

#ifndef PHY_H_
#define PHY_H_

#include <stdint.h>
#include "core_mdio.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
 *              Generic IEEE 802.3 Clause 22 register numbers
 *
 * Registers 0x00 to 0x0F are standardised. Anything above that is vendor
 * specific and belongs in a per-PHY source file, not here.
 *============================================================================*/
#define MII_BMCR                    ( 0x00u ) /*!< Basic mode control.        */
#define MII_BMSR                    ( 0x01u ) /*!< Basic mode status.         */
#define MII_PHYSID1                 ( 0x02u ) /*!< PHY identifier 1.          */
#define MII_PHYSID2                 ( 0x03u ) /*!< PHY identifier 2.          */
#define MII_ADVERTISE               ( 0x04u ) /*!< Auto-neg advertisement.    */
#define MII_LPA                     ( 0x05u ) /*!< Link partner ability.      */
#define MII_EXPANSION               ( 0x06u ) /*!< Auto-neg expansion.        */
#define MII_CTRL1000                ( 0x09u ) /*!< 1000BASE-T control.        */
#define MII_STAT1000                ( 0x0Au ) /*!< 1000BASE-T status.         */
#define MII_ESTATUS                 ( 0x0Fu ) /*!< Extended status.           */

/*==============================================================================
 *                    Basic mode control register, 0x00
 *============================================================================*/
#define BMCR_SPEED1000              ( 0x0040u ) /*!< Speed select MSB.        */
#define BMCR_FULLDPLX               ( 0x0100u ) /*!< Full duplex.             */
#define BMCR_ANRESTART              ( 0x0200u ) /*!< Restart auto-neg.        */
#define BMCR_ISOLATE                ( 0x0400u ) /*!< Isolate from MII.        */
#define BMCR_PDOWN                  ( 0x0800u ) /*!< Power down.              */
#define BMCR_ANENABLE               ( 0x1000u ) /*!< Enable auto-neg.         */
#define BMCR_SPEED100               ( 0x2000u ) /*!< Speed select LSB.        */
#define BMCR_LOOPBACK               ( 0x4000u ) /*!< Internal loopback.       */
#define BMCR_RESET                  ( 0x8000u ) /*!< Software reset.          */

/*==============================================================================
 *                    Basic mode status register, 0x01
 *============================================================================*/
#define BMSR_ERCAP                  ( 0x0001u ) /*!< Extended capability.     */
#define BMSR_JCD                    ( 0x0002u ) /*!< Jabber detected.         */
#define BMSR_LSTATUS                ( 0x0004u ) /*!< Link up. LATCHING LOW:
                                                     read twice.             */
#define BMSR_ANEGCAPABLE            ( 0x0008u ) /*!< Auto-neg capable.        */
#define BMSR_RFAULT                 ( 0x0010u ) /*!< Remote fault.            */
#define BMSR_ANEGCOMPLETE           ( 0x0020u ) /*!< Auto-neg complete.       */
#define BMSR_ESTATEN                ( 0x0100u ) /*!< Extended status in 0x0F. */
#define BMSR_10HALF                 ( 0x0800u )
#define BMSR_10FULL                 ( 0x1000u )
#define BMSR_100HALF                ( 0x2000u )
#define BMSR_100FULL                ( 0x4000u )

/*==============================================================================
 *                 Auto-negotiation advertisement, 0x04
 *
 * NOTE: 1000 Mbps ability is NOT advertised here. It is advertised in register
 * 0x09. Writing only register 0x00 to request a gigabit link is not sufficient.
 *============================================================================*/
#define ADVERTISE_CSMA              ( 0x0001u ) /*!< Selector, always set.    */
#define ADVERTISE_10HALF            ( 0x0020u )
#define ADVERTISE_10FULL            ( 0x0040u )
#define ADVERTISE_100HALF           ( 0x0080u )
#define ADVERTISE_100FULL           ( 0x0100u )
#define ADVERTISE_PAUSE_CAP         ( 0x0400u )
#define ADVERTISE_PAUSE_ASYM        ( 0x0800u )

/*==============================================================================
 *                 Link partner ability register, 0x05
 *
 * What the far end advertised during auto-negotiation. Note that 1000 Mbps
 * partner ability is reported in register 0x0A, not here.
 *============================================================================*/
#define LPA_10HALF                  ( 0x0020u )
#define LPA_10FULL                  ( 0x0040u )
#define LPA_100HALF                 ( 0x0080u )
#define LPA_100FULL                 ( 0x0100u )
#define LPA_PAUSE_CAP               ( 0x0400u )
#define LPA_PAUSE_ASYM              ( 0x0800u )
#define LPA_RFAULT                  ( 0x2000u ) /*!< Partner reports a fault. */

/*==============================================================================
 *                   1000BASE-T control register, 0x09
 *
 * This is where gigabit ability is advertised.
 *============================================================================*/
#define ADVERTISE_1000FULL          ( 0x0200u ) /*!< Advertise 1000 full dup. */
#define ADVERTISE_1000HALF          ( 0x0100u ) /*!< Advertise 1000 half dup. */

/*==============================================================================
 *                   1000BASE-T status register, 0x0A
 *============================================================================*/
#define LPA_1000LOCALRXOK           ( 0x2000u ) /*!< Local receiver OK.       */
#define LPA_1000REMRXOK             ( 0x1000u ) /*!< Remote receiver OK.      */
#define LPA_1000FULL                ( 0x0800u ) /*!< Partner 1000 full dup.   */
#define LPA_1000HALF                ( 0x0400u ) /*!< Partner 1000 half dup.   */

/*==============================================================================
 *              Speed and duplex selection, passed to
 *              PHY_set_link_speed() as a bit mask
 *============================================================================*/
#define PHY_ANEG_10M_HD             ( 0x00000001uL )
#define PHY_ANEG_10M_FD             ( 0x00000002uL )
#define PHY_ANEG_100M_HD            ( 0x00000004uL )
#define PHY_ANEG_100M_FD            ( 0x00000008uL )
#define PHY_ANEG_1000M_FD           ( 0x00000010uL )

/*! Advertise every ability the PHY supports. */
#define PHY_ANEG_ALL                ( PHY_ANEG_10M_HD  | PHY_ANEG_10M_FD  | \
                                      PHY_ANEG_100M_HD | PHY_ANEG_100M_FD | \
                                      PHY_ANEG_1000M_FD )

/*! Gigabit full duplex only. This is what the CoreQSE 1 Gbps design needs, and
 *  it matches what the reference application requests.
 */
#define PHY_ANEG_1000M_FD_ONLY      ( PHY_ANEG_1000M_FD )

/*==============================================================================
 *                          Link state
 *============================================================================*/
#define PHY_LINK_DOWN               ( 0u )
#define PHY_LINK_UP                 ( 1u )

#define PHY_HALF_DUPLEX             ( 0u )
#define PHY_FULL_DUPLEX             ( 1u )

/*! Negotiated link speed, reported by PHY_get_link_status(). */
typedef enum PHY_speed
{
    PHY_SPEED_10MBPS   = 0u,
    PHY_SPEED_100MBPS  = 1u,
    PHY_SPEED_1000MBPS = 2u,
    PHY_SPEED_INVALID  = 3u
} PHY_speed_t;

/*==============================================================================
 *                    Interface implemented per PHY
 *============================================================================*/

/***************************************************************************//**
  PHY_init() brings the PHY out of reset and out of power down.

  On several parts, including the Marvell family used on the SGMII daughter
  board, the PHY comes up with the power down bit SET. Until it is cleared the
  PHY answers MDIO but drives nothing onto the copper, which looks exactly like
  a dead cable.

  @param this_mdio
    Points to an initialized MDIO_instance_t.

  @param phy_addr
    PHY address on the MDIO bus, 0 to 31.

  @return
    MDIO_OK, or an MDIO_ERR_* code from the underlying transport.
 */
uint8_t PHY_init( const MDIO_instance_t * this_mdio, uint8_t phy_addr );

/***************************************************************************//**
  PHY_set_link_speed() configures which speeds and duplex modes the PHY
  advertises during auto-negotiation.

  @warning
  This function writes BOTH register 0x04 and register 0x09. That is the whole
  reason it exists as a separate step. Register 0x00 only REQUESTS a speed; the
  abilities actually offered to the link partner live in 0x04 for 10 and 100
  Mbps and in 0x09 for 1000 Mbps. An implementation that writes only 0x00 will
  appear to work whenever the PHY happens to default to advertising gigabit,
  and will silently negotiate 100 Mbps or fail when it does not.

  @param this_mdio
    Points to an initialized MDIO_instance_t.

  @param phy_addr
    PHY address, 0 to 31.

  @param speed_duplex_select
    Bitwise OR of PHY_ANEG_* values. Use PHY_ANEG_1000M_FD_ONLY for the CoreQSE
    1 Gbps design.

  @return
    MDIO_OK, or an MDIO_ERR_* code.
 */
uint8_t PHY_set_link_speed( const MDIO_instance_t * this_mdio,
                            uint8_t                 phy_addr,
                            uint32_t                speed_duplex_select );

/***************************************************************************//**
  PHY_autonegotiate() enables and restarts auto-negotiation, then waits for it
  to complete.

  @warning
  Auto-negotiation for 1000BASE-T takes 1 to 3 SECONDS. IEEE 802.3 defines a
  break_link_timer of 1.2 to 1.5 s before the page exchange and master/slave
  resolution even begin. Allow at least 3000 ms.

  @param this_mdio
    Points to an initialized MDIO_instance_t.

  @param phy_addr
    PHY address, 0 to 31.

  @param wait_ms
    Approximate maximum wait in milliseconds. Use 5000 for 1000BASE-T.

  @param progress_cb
    Called periodically with the current status so the application can show
    progress rather than appearing to hang. May be NULL.

  @return
    MDIO_OK if auto-negotiation completed, MDIO_ERR_TIMEOUT if it did not.
 */
uint8_t PHY_autonegotiate( const MDIO_instance_t * this_mdio,
                           uint8_t                 phy_addr,
                           uint32_t                wait_ms,
                           MDIO_link_progress_t    progress_cb );

/***************************************************************************//**
  PHY_get_link_status() reports link state and, when the link is up, the
  NEGOTIATED speed and duplex.

  The generic status register only reports link up or down. Speed and duplex
  resolution is vendor specific, typically in a vendor status register, so each
  PHY implementation reads its own.

  Reporting the negotiated speed matters: it is the difference between "we asked
  for gigabit" and "we actually got gigabit". A link that silently falls back to
  100 Mbps would otherwise be indistinguishable from success.

  @param this_mdio
    Points to an initialized MDIO_instance_t.

  @param phy_addr
    PHY address, 0 to 31.

  @param speed
    Receives the negotiated speed. May be NULL.

  @param fullduplex
    Receives PHY_FULL_DUPLEX or PHY_HALF_DUPLEX. May be NULL.

  @return
    PHY_LINK_UP or PHY_LINK_DOWN.
 */
uint8_t PHY_get_link_status( const MDIO_instance_t * this_mdio,
                             uint8_t                 phy_addr,
                             PHY_speed_t           * speed,
                             uint8_t               * fullduplex );

/***************************************************************************//**
  PHY_get_name() returns a short human readable name for the compiled PHY
  implementation, for example "VSC8575" or "none". Useful in test logs so that
  the build configuration is visible.

  @return
    Pointer to a static string. Never NULL.
 */
const char * PHY_get_name( void );

#ifdef __cplusplus
}
#endif

#endif /* PHY_H_ */
