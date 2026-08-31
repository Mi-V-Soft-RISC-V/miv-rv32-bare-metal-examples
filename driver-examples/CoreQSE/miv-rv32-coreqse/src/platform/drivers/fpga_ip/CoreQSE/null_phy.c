/*******************************************************************************
 * Copyright 2026 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file null_phy.c
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief Generic IEEE 802.3 Clause 22 PHY implementation of the phy.h
 *        interface.
 *
 * WHAT THIS PROVIDES
 * ------------------
 * A vendor neutral PHY implementation using ONLY the standardised Clause 22
 * registers, 0x00 to 0x0F. That is sufficient to bring up a 1000BASE-T link on
 * any conforming PHY:
 *
 *   register 0x00   reset, power down, auto-negotiation enable and restart
 *   register 0x01   link status, auto-negotiation complete
 *   register 0x04   advertise 10 and 100 Mbps abilities
 *   register 0x05   link partner abilities
 *   register 0x09   advertise 1000 Mbps ability
 *   register 0x0A   link partner 1000BASE-T abilities
 *
 * Confirmed on hardware: the 1 Gbps reference design negotiates a 1000 Mbps
 * full duplex link in about 1735 ms using this sequence.
 *
 * WHAT IT DOES NOT PROVIDE
 * ------------------------
 * The resolved speed and duplex are reported by a VENDOR specific status
 * register, not by any standard one. Register 0x01 only says link up or down.
 * PHY_get_link_status() therefore INFERS the rate from the 1000BASE-T status
 * register and the link partner ability register. That is correct in the common
 * cases but is not a direct read of the negotiated rate.
 *
 * Also absent, because all of it is vendor specific: LED control, cable
 * diagnostics, paged register access beyond the harmless page 0 select, SERDES
 * and MAC interface configuration, and loopback modes.
 *
 * IF YOU NEED VENDOR SPECIFIC BEHAVIOUR
 * -------------------------------------
 * The CoreTSE driver ships tested implementations for several parts, each behind
 * the same small interface:
 *
 *   vsc8575.c         Microchip VSC8575
 *   m88e1340_phy.c    Marvell M88E1340
 *   m88e1111_phy.c    Marvell M88E1111
 *
 * To adapt one for CoreQSE: replace TSE_read_phy_reg and TSE_write_phy_reg with
 * MDIO_read and MDIO_write, rename the TSE_phy_* entry points to PHY_*, define a
 * selection macro such as VSC8575_PHY in fpga_design_config.h, and this file
 * guards itself out. See phy.h.
 *
 * DESIGNS WITH NO PHY AT ALL
 * --------------------------
 * The 2.5 Gbps reference design connects the transceiver to an SMA cable
 * directly. It has no PHY and no COREMDIO_APB instance, so the application does
 * not call QSE_attach_mdio() and nothing here is reached. The QSE_phy_*
 * functions then return QSE_ERR_NOT_SUPPORTED, and link state must be inferred
 * from the PCS counters using QSE_pcs_get_sync_status().
 */

#include "phy.h"
#include "core_mdio.h"

#ifndef LEGACY_DIR_STRUCTURE
#include "fpga_design_config/fpga_design_config.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(VSC8575_PHY) && !defined(M88E1340_PHY)

/*! Iterations allowed for a self clearing bit to clear. Bounded: an unbounded
 *  wait would hang the processor if the PHY never responds.
 */
#define PHY_SELFCLEAR_LIMIT         ( 100000u )

/*! Delay loops between asserting and releasing PHY reset. */
#define PHY_RESET_HOLD_LOOPS        ( 10000u )

/*! Page select register. Vendor specific on paged parts, but reserved and
 *  therefore harmless on parts without paging. Writing 0 selects the standard
 *  register set on both.
 */
#define PHY_PAGE_SELECT_REG         ( 0x16u )
#define PHY_PAGE_0                  ( 0x0000u )

/*------------------------------------------------------------------------------
 * Wait for the reset bit in register 0x00 to self clear.
 */
static uint8_t wait_reset_clear( const MDIO_instance_t * this_mdio,
                                 uint8_t phy_addr )
{
    uint16_t reg = BMCR_RESET;
    uint32_t attempts;

    for( attempts = 0u; attempts < PHY_SELFCLEAR_LIMIT; attempts++ )
    {
        if( MDIO_read( this_mdio, phy_addr, MII_BMCR, &reg ) != MDIO_OK )
        {
            return ( MDIO_ERR_TIMEOUT );
        }

        if( ( reg & BMCR_RESET ) == 0u )
        {
            return ( MDIO_OK );
        }
    }

    return ( MDIO_ERR_TIMEOUT );
}

/*******************************************************************************
 * PHY_init()
 *
 * Software reset, then clear power down and isolate.
 *
 * The power down step matters. Several parts come out of reset with the power
 * down bit SET, in which case the PHY answers MDIO normally but drives nothing
 * onto the copper. That presents exactly like a disconnected cable and the RJ45
 * LEDs stay dark. On the 1 Gbps reference design the PHY was measured at
 * CONTROL = 0x1940 on entry, with power down asserted, and became 0x1140 after
 * this function ran.
 */
uint8_t
PHY_init( const MDIO_instance_t * this_mdio, uint8_t phy_addr )
{
    uint8_t           status;
    uint16_t          reg = 0u;
    volatile uint32_t loop;

    /* Select the standard register page. */
    status = MDIO_write( this_mdio, phy_addr, PHY_PAGE_SELECT_REG,
                         PHY_PAGE_0 );

    /* Software reset. */
    if( status == MDIO_OK )
    {
        status = MDIO_read( this_mdio, phy_addr, MII_BMCR, &reg );
    }

    if( status == MDIO_OK )
    {
        reg |= BMCR_RESET;
        status = MDIO_write( this_mdio, phy_addr, MII_BMCR, reg );
    }

    if( status == MDIO_OK )
    {
        for( loop = 0u; loop < PHY_RESET_HOLD_LOOPS; loop++ )
        {
            /* Hold reset briefly before polling. */
        }

        status = wait_reset_clear( this_mdio, phy_addr );
    }

    /* Clear power down and isolate so the PHY drives the line. */
    if( status == MDIO_OK )
    {
        status = MDIO_read( this_mdio, phy_addr, MII_BMCR, &reg );
    }

    if( status == MDIO_OK )
    {
        reg &= (uint16_t)~( BMCR_PDOWN | BMCR_ISOLATE );
        status = MDIO_write( this_mdio, phy_addr, MII_BMCR, reg );
    }

    return ( status );
}

/*******************************************************************************
 * PHY_set_link_speed()
 *
 * Writes BOTH register 0x04 and register 0x09.
 *
 * This is the whole reason the function exists as a separate step. Register 0x00
 * only REQUESTS a speed; the abilities actually offered to the link partner live
 * in 0x04 for 10 and 100 Mbps and in 0x09 for 1000 Mbps. An implementation that
 * writes only 0x00 appears to work whenever the PHY happens to default to
 * advertising gigabit, and silently negotiates 100 Mbps when it does not.
 */
uint8_t
PHY_set_link_speed
(
    const MDIO_instance_t * this_mdio,
    uint8_t                 phy_addr,
    uint32_t                speed_duplex_select
)
{
    uint8_t  status;
    uint16_t reg = 0u;

    status = MDIO_write( this_mdio, phy_addr, PHY_PAGE_SELECT_REG,
                         PHY_PAGE_0 );

    /* Register 0x04: 10 and 100 Mbps abilities. */
    if( status == MDIO_OK )
    {
        status = MDIO_read( this_mdio, phy_addr, MII_ADVERTISE, &reg );
    }

    if( status == MDIO_OK )
    {
        reg &= (uint16_t)~( ADVERTISE_10HALF  | ADVERTISE_10FULL |
                            ADVERTISE_100HALF | ADVERTISE_100FULL );

        if( ( speed_duplex_select & PHY_ANEG_10M_HD ) != 0u )
        {
            reg |= ADVERTISE_10HALF;
        }
        if( ( speed_duplex_select & PHY_ANEG_10M_FD ) != 0u )
        {
            reg |= ADVERTISE_10FULL;
        }
        if( ( speed_duplex_select & PHY_ANEG_100M_HD ) != 0u )
        {
            reg |= ADVERTISE_100HALF;
        }
        if( ( speed_duplex_select & PHY_ANEG_100M_FD ) != 0u )
        {
            reg |= ADVERTISE_100FULL;
        }

        /* The selector field must always indicate CSMA/CD. */
        reg |= ADVERTISE_CSMA;

        status = MDIO_write( this_mdio, phy_addr, MII_ADVERTISE, reg );
    }

    /* Register 0x09: 1000 Mbps ability. */
    if( status == MDIO_OK )
    {
        status = MDIO_read( this_mdio, phy_addr, MII_CTRL1000, &reg );
    }

    if( status == MDIO_OK )
    {
        reg &= (uint16_t)~( ADVERTISE_1000FULL | ADVERTISE_1000HALF );

        if( ( speed_duplex_select & PHY_ANEG_1000M_FD ) != 0u )
        {
            reg |= ADVERTISE_1000FULL;
        }

        status = MDIO_write( this_mdio, phy_addr, MII_CTRL1000, reg );
    }

    /* Keep the speed and duplex request in register 0x00 consistent with what
     * is advertised, so the settings still make sense if the link partner does
     * not auto-negotiate.
     */
    if( status == MDIO_OK )
    {
        status = MDIO_read( this_mdio, phy_addr, MII_BMCR, &reg );
    }

    if( status == MDIO_OK )
    {
        reg &= (uint16_t)~( BMCR_SPEED100 | BMCR_SPEED1000 | BMCR_FULLDPLX );

        if( ( speed_duplex_select & PHY_ANEG_1000M_FD ) != 0u )
        {
            /* Speed is encoded as {MSB, LSB} = {1, 0} for 1000 Mbps. */
            reg |= ( BMCR_SPEED1000 | BMCR_FULLDPLX );
        }
        else if( ( speed_duplex_select &
                   ( PHY_ANEG_100M_FD | PHY_ANEG_100M_HD ) ) != 0u )
        {
            reg |= BMCR_SPEED100;

            if( ( speed_duplex_select & PHY_ANEG_100M_FD ) != 0u )
            {
                reg |= BMCR_FULLDPLX;
            }
        }
        else
        {
            if( ( speed_duplex_select & PHY_ANEG_10M_FD ) != 0u )
            {
                reg |= BMCR_FULLDPLX;
            }
        }

        status = MDIO_write( this_mdio, phy_addr, MII_BMCR, reg );
    }

    return ( status );
}

/*******************************************************************************
 * PHY_autonegotiate()
 *
 * Enables and restarts auto-negotiation, then waits for the link.
 *
 * 1000BASE-T auto-negotiation takes 1 to 3 SECONDS. IEEE 802.3 defines a
 * break_link_timer of 1.2 to 1.5 s before the page exchange and master/slave
 * resolution even begin, so wait_ms must be at least 3000.
 */
uint8_t
PHY_autonegotiate
(
    const MDIO_instance_t * this_mdio,
    uint8_t                 phy_addr,
    uint32_t                wait_ms,
    MDIO_link_progress_t    progress_cb
)
{
    uint8_t  status;
    uint16_t reg = 0u;

    status = MDIO_write( this_mdio, phy_addr, PHY_PAGE_SELECT_REG,
                         PHY_PAGE_0 );

    if( status == MDIO_OK )
    {
        status = MDIO_read( this_mdio, phy_addr, MII_BMCR, &reg );
    }

    if( status == MDIO_OK )
    {
        reg |= ( BMCR_ANENABLE | BMCR_ANRESTART );
        status = MDIO_write( this_mdio, phy_addr, MII_BMCR, reg );
    }

    if( status == MDIO_OK )
    {
        status = MDIO_wait_link_up( this_mdio, phy_addr, wait_ms, progress_cb );
    }

    return ( status );
}

/*******************************************************************************
 * PHY_get_link_status()
 *
 * Reports link state, and INFERS speed and duplex from the standard registers.
 *
 * LIMITATION: the resolved speed and duplex are only directly readable from a
 * VENDOR status register, which this implementation deliberately avoids. The
 * inference below uses the 1000BASE-T status register to detect a gigabit link
 * and the link partner ability register for 10 and 100 Mbps. That is correct in
 * the common cases but is not a direct read of the negotiated rate.
 *
 * For an authoritative reading, adapt a vendor implementation from the CoreTSE
 * driver: it reads register 0x11 on Marvell parts and register 0x1C on the
 * VSC8575.
 */
uint8_t
PHY_get_link_status
(
    const MDIO_instance_t * this_mdio,
    uint8_t                 phy_addr,
    PHY_speed_t           * speed,
    uint8_t               * fullduplex
)
{
    uint16_t reg = 0u;

    /* The link bit latches low, so read twice and use the second value. */
    if( MDIO_read( this_mdio, phy_addr, MII_BMSR, &reg ) != MDIO_OK )
    {
        return ( PHY_LINK_DOWN );
    }

    if( MDIO_read( this_mdio, phy_addr, MII_BMSR, &reg ) != MDIO_OK )
    {
        return ( PHY_LINK_DOWN );
    }

    if( ( reg & BMSR_LSTATUS ) == 0u )
    {
        if( speed != (PHY_speed_t *)0 )
        {
            *speed = PHY_SPEED_INVALID;
        }
        return ( PHY_LINK_DOWN );
    }

    /* Link is up. Default to unknown until a rate is resolved. */
    if( speed != (PHY_speed_t *)0 )
    {
        *speed = PHY_SPEED_INVALID;
    }

    if( fullduplex != (uint8_t *)0 )
    {
        *fullduplex = PHY_HALF_DUPLEX;
    }

    /* 1000BASE-T status: if the partner resolved a gigabit ability, the link is
     * running at 1000 Mbps.
     */
    if( MDIO_read( this_mdio, phy_addr, MII_STAT1000, &reg ) == MDIO_OK )
    {
        if( ( reg & ( LPA_1000FULL | LPA_1000HALF ) ) != 0u )
        {
            if( speed != (PHY_speed_t *)0 )
            {
                *speed = PHY_SPEED_1000MBPS;
            }

            if( fullduplex != (uint8_t *)0 )
            {
                *fullduplex = ( ( reg & LPA_1000FULL ) != 0u )
                                ? PHY_FULL_DUPLEX : PHY_HALF_DUPLEX;
            }

            return ( PHY_LINK_UP );
        }
    }

    /* Not gigabit. Fall back to the link partner ability register. */
    if( MDIO_read( this_mdio, phy_addr, MII_LPA, &reg ) == MDIO_OK )
    {
        if( ( reg & ( LPA_100FULL | LPA_100HALF ) ) != 0u )
        {
            if( speed != (PHY_speed_t *)0 )
            {
                *speed = PHY_SPEED_100MBPS;
            }

            if( fullduplex != (uint8_t *)0 )
            {
                *fullduplex = ( ( reg & LPA_100FULL ) != 0u )
                                ? PHY_FULL_DUPLEX : PHY_HALF_DUPLEX;
            }
        }
        else if( ( reg & ( LPA_10FULL | LPA_10HALF ) ) != 0u )
        {
            if( speed != (PHY_speed_t *)0 )
            {
                *speed = PHY_SPEED_10MBPS;
            }

            if( fullduplex != (uint8_t *)0 )
            {
                *fullduplex = ( ( reg & LPA_10FULL ) != 0u )
                                ? PHY_FULL_DUPLEX : PHY_HALF_DUPLEX;
            }
        }
        else
        {
            /* Link is up but no ability was resolved. */
        }
    }

    return ( PHY_LINK_UP );
}

/*******************************************************************************
 * PHY_get_name()
 */
const char *
PHY_get_name( void )
{
    return ( "generic Clause 22" );
}

#endif /* !VSC8575_PHY && !M88E1340_PHY */

#ifdef __cplusplus
}
#endif
