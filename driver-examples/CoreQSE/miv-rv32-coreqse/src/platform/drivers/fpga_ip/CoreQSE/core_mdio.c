/*******************************************************************************
 * Copyright 2026 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file core_mdio.c
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief CoreMDIO_APB bare metal driver implementation.
 *
 * See file "core_mdio.h" for a description of the functions implemented here.
 */

#include "core_mdio.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * CoreMDIO_APB register offsets.
 *
 * VERIFIED against the IP RTL at
 *   component/Actel/DirectCore/COREMDIO_APB/3.0.111/rtl/vlog/core/CoreMDIO_APB.v
 *
 * The RTL decodes PADDR[4:2], so registers are 4 bytes apart:
 *   localparam ADDRESS_REG      = 3'b000;   offset 0x00
 *   localparam PHYADDR_REG      = 3'b001;   offset 0x04
 *   localparam CONTROL_REG      = 3'b010;   offset 0x08
 *   localparam STATUS_REG       = 3'b011;   offset 0x0C
 *   localparam DATAIN_REG       = 3'b100;   offset 0x10
 *   localparam DATAOUT_REG      = 3'b101;   offset 0x14
 *   localparam CLKPRESCALER_REG = 3'b110;   offset 0x18
 *   localparam DEVTYPE_REG      = 3'b111;   offset 0x1C
 *
 * Note also: reg_wr requires PADDR[1:0] == 2'b00, so all accesses must be
 * 32-bit aligned.
 */
#define MDIO_ADDR_REG               ( 0x00u ) /*!< PHY register address.       */
#define MDIO_PHYID_REG              ( 0x04u ) /*!< PHY device address.         */
#define MDIO_CTRL_REG               ( 0x08u ) /*!< Control, starts a cycle.    */
#define MDIO_STAT_REG               ( 0x0Cu ) /*!< Status, bit 0 is busy.      */
#define MDIO_DATA_WRITE_REG         ( 0x10u ) /*!< DATAIN, write data.         */
#define MDIO_DATA_READ_REG          ( 0x14u ) /*!< DATAOUT, read data.         */
#define MDIO_PRE_SCALAR_REG         ( 0x18u ) /*!< MDC clock prescaler.        */
#define MDIO_DEVTYPE_REG            ( 0x1Cu ) /*!< Clause 45 device type.      */

/*******************************************************************************
 * Control register, 3 bits wide. From the RTL:
 *   assign clause_sel = ctrl_reg[2];    0 = Clause 22, 1 = Clause 45
 *   assign mdio_wr    = ctrl_reg[1];    0 = read,      1 = write
 *   ctrl_reg[0]                         starts the transaction
 *
 * IMPORTANT: the whole register must be written in ONE access with the start
 * bit included. The RTL triggers on ctrl_reg_wr, that is the APB write itself,
 * and loads the bit counter from PWDATA[2] at that instant. Read-modify-write
 * of this register is therefore wrong: it can re-trigger a transaction with a
 * stale direction bit.
 */
#define MDIO_CTRL_START             ( 0x1u )  /*!< Bit 0, start.               */
#define MDIO_CTRL_WRITE             ( 0x2u )  /*!< Bit 1, 1 = write.           */
#define MDIO_CTRL_CLAUSE45          ( 0x4u )  /*!< Bit 2, 1 = Clause 45.       */

/*! Clause 22 read: start, direction read, Clause 22. */
#define MDIO_CTRL_READ_OP           ( MDIO_CTRL_START )

/*! Clause 22 write: start, direction write, Clause 22. */
#define MDIO_CTRL_WRITE_OP          ( MDIO_CTRL_START | MDIO_CTRL_WRITE )

/*******************************************************************************
 * Status register. From the RTL, sts_reg is a SINGLE bit reflecting mdio_busy,
 * read back as { 31'd0, sts_reg }. Bit 0 set means a transaction is in
 * progress.
 */
#define MDIO_STAT_BUSY_MASK         ( 0x1u )

/*******************************************************************************
 * Clock prescaler. From the RTL, clk_div_reg is 3 bits and only values 2 to 7
 * select a defined divider:
 *   2 -> 3,  3 -> 4,  4 -> 5,  5 -> 7,  6 -> 10,  7 -> 14
 *   anything else -> 2
 *
 * MDC = PCLK / divider. IEEE 802.3 limits MDC to 2.5 MHz, so at a 50 MHz PCLK
 * the divider must be at least 20. NO legal prescaler value achieves that:
 * the largest divider is 14, giving MDC = 3.57 MHz, which is over the limit.
 * Value 7 is the closest available and is what this driver recommends.
 */
#define MDIO_PRESCALER_MIN          ( 2u )
#define MDIO_PRESCALER_MAX          ( 7u )

/*******************************************************************************
 * Local limits.
 */
#define NULL_INSTANCE               (( MDIO_instance_t * ) 0)
#define NULL_U16                    (( uint16_t * ) 0)
#define NULL_U32                    (( uint32_t * ) 0)
#define NULL_U8                     (( uint8_t * ) 0)

#define MDIO_ADDR_MAX               ( 31u )   /*!< 5-bit PHY and register.     */
#define MDIO_DATA_MASK              ( 0xFFFFu )

/*! Bound on the busy wait. The reference application spins without a limit,
 *  which hangs the processor when no PHY responds.
 */
#define MDIO_BUSY_TIMEOUT           ( 100000u )

/*! Settling delay after the busy flag clears, before reading DATAOUT.
 *
 *  The RTL registers PRDATA on the clock edge following the APB address phase,
 *  and data_out_reg is loaded from the serial shift register. A short delay
 *  after busy de-asserts avoids sampling DATAOUT before it is stable.
 */
#define MDIO_DATA_SETTLE_LOOPS      ( 64u )

/*! Busy wait iterations that approximate the remainder of one millisecond in
 *  MDIO_wait_link_up(), after the two status transactions have been performed.
 *
 *  At a 50 MHz processor clock a simple empty loop iteration costs on the order
 *  of a few clock cycles, so a few thousand iterations is roughly a
 *  millisecond. This does not need to be precise: it only has to make the total
 *  wait comfortably longer than the 1 to 3 seconds auto-negotiation requires,
 *  and the elapsed count is reported so the caller can see what was actually
 *  waited.
 */
#define MDIO_ONE_MS_PAD_LOOPS       ( 4000u )

/*! Delay loops after asserting PHY reset, before releasing it. */
#define MDIO_RESET_DELAY_LOOPS      ( 10000u )

/*! Iterations allowed for the PHY reset bit to self clear. */
#define MDIO_RESET_POLL_LIMIT       ( 1000u )

/*******************************************************************************
 * Direct register access. The peripheral is a simple APB slave, so the generic
 * HAL 32-bit accessors are used with explicit offsets.
 */
#define MDIO_WRITE_REG( BASE, OFFSET, VALUE )                                 \
    HW_set_32bit_reg( ( (BASE) + (OFFSET) ), (uint32_t)(VALUE) )

#define MDIO_READ_REG( BASE, OFFSET )                                         \
    HW_get_32bit_reg( ( (BASE) + (OFFSET) ) )

/*******************************************************************************
 * Wait for the MDIO status register to report not busy.
 *
 * Returns MDIO_OK when idle, MDIO_ERR_TIMEOUT when the budget is exhausted.
 */
static uint8_t mdio_wait_not_busy( addr_t base )
{
    uint32_t attempts;

    for( attempts = 0u; attempts < MDIO_BUSY_TIMEOUT; attempts++ )
    {
        /* Test bit 0 only. sts_reg is a single bit in the RTL and the upper
         * bits read as zero, but masking makes the intent explicit and keeps
         * the driver correct if the register gains fields later.
         */
        if( ( MDIO_READ_REG( base, MDIO_STAT_REG ) & MDIO_STAT_BUSY_MASK )
                == 0u )
        {
            return ( MDIO_OK );
        }
    }

    return ( MDIO_ERR_TIMEOUT );
}

/*******************************************************************************
 * MDIO_init()
 */
uint8_t
MDIO_init
(
    MDIO_instance_t * this_mdio,
    addr_t            base_addr,
    uint32_t          prescaler
)
{
    uint8_t status = MDIO_ERR_PARAM;

    HAL_ASSERT( this_mdio != NULL_INSTANCE );
    HAL_ASSERT( base_addr != 0u );

    if( ( this_mdio != NULL_INSTANCE ) && ( base_addr != 0u ) )
    {
        this_mdio->base_address = base_addr;

        /* Clamp to a value the RTL actually decodes. clk_div_reg is 3 bits and
         * only 2 to 7 select a defined divider; anything else falls through to
         * the default of 2, which is the FASTEST setting and the most likely to
         * exceed the PHY's MDC limit.
         */
        if( prescaler < MDIO_PRESCALER_MIN )
        {
            prescaler = MDIO_PRESCALER_MIN;
        }
        else if( prescaler > MDIO_PRESCALER_MAX )
        {
            prescaler = MDIO_PRESCALER_MAX;
        }
        else
        {
            /* In range. */
        }

        MDIO_WRITE_REG( base_addr, MDIO_PRE_SCALAR_REG, prescaler );

        /* Ensure the control register is idle. If a previous run left the
         * direction bit set, the first read would otherwise run as a write.
         */
        MDIO_WRITE_REG( base_addr, MDIO_CTRL_REG, 0u );

        status = MDIO_OK;
    }

    return ( status );
}

/*******************************************************************************
 * MDIO_read()
 */
uint8_t
MDIO_read
(
    const MDIO_instance_t * this_mdio,
    uint8_t                 phy_addr,
    uint8_t                 reg_addr,
    uint16_t              * data
)
{
    uint8_t           status = MDIO_ERR_PARAM;
    volatile uint32_t settle;

    HAL_ASSERT( this_mdio != NULL_INSTANCE );
    HAL_ASSERT( data != NULL_U16 );

    if( ( this_mdio != NULL_INSTANCE ) && ( data != NULL_U16 ) &&
        ( phy_addr <= MDIO_ADDR_MAX ) && ( reg_addr <= MDIO_ADDR_MAX ) )
    {
        /* Any previous transaction must have finished first. */
        status = mdio_wait_not_busy( this_mdio->base_address );

        if( status == MDIO_OK )
        {
            MDIO_WRITE_REG( this_mdio->base_address, MDIO_ADDR_REG, reg_addr );
            MDIO_WRITE_REG( this_mdio->base_address, MDIO_PHYID_REG, phy_addr );

            /* Write the CONTROL register OUTRIGHT, never read-modify-write.
             *
             * The previous implementation did:
             *     ctrl = read(CONTROL); ctrl |= START; write(CONTROL, ctrl);
             *
             * That is wrong on this IP. ctrl_reg[1] is the direction bit and it
             * persists after a transaction, so once any write had been
             * performed, every subsequent "read" re-asserted the write bit and
             * ran as a WRITE cycle. No read data was ever fetched, and DATAOUT
             * retained whatever it last held, which is why every register
             * appeared to return the same value.
             *
             * Writing the whole register also matters because the RTL loads its
             * bit counter from PWDATA[2] at the instant of the APB write:
             *     else if(ctrl_reg_wr) begin
             *       if(PWDATA[2]) mdio_cnt <= 8'd128; else mdio_cnt <= 8'd63;
             */
            MDIO_WRITE_REG( this_mdio->base_address, MDIO_CTRL_REG,
                            MDIO_CTRL_READ_OP );

            status = mdio_wait_not_busy( this_mdio->base_address );

            if( status == MDIO_OK )
            {
                /* Let DATAOUT settle. The RTL registers PRDATA a cycle after
                 * the address phase and loads data_out_reg from the shift
                 * register, so an immediate read can catch stale data.
                 */
                for( settle = 0u; settle < MDIO_DATA_SETTLE_LOOPS; settle++ )
                {
                    /* Busy wait. */
                }

                *data = (uint16_t)( MDIO_READ_REG( this_mdio->base_address,
                                        MDIO_DATA_READ_REG ) & MDIO_DATA_MASK );
            }
        }
    }

    return ( status );
}

/*******************************************************************************
 * MDIO_write()
 */
uint8_t
MDIO_write
(
    const MDIO_instance_t * this_mdio,
    uint8_t                 phy_addr,
    uint8_t                 reg_addr,
    uint16_t                data
)
{
    uint8_t status = MDIO_ERR_PARAM;

    HAL_ASSERT( this_mdio != NULL_INSTANCE );

    if( ( this_mdio != NULL_INSTANCE ) &&
        ( phy_addr <= MDIO_ADDR_MAX ) && ( reg_addr <= MDIO_ADDR_MAX ) )
    {
        status = mdio_wait_not_busy( this_mdio->base_address );

        if( status == MDIO_OK )
        {
            MDIO_WRITE_REG( this_mdio->base_address, MDIO_ADDR_REG, reg_addr );
            MDIO_WRITE_REG( this_mdio->base_address, MDIO_PHYID_REG, phy_addr );
            MDIO_WRITE_REG( this_mdio->base_address, MDIO_DATA_WRITE_REG, data );

            /* Write CONTROL outright, never read-modify-write. See the detailed
             * comment in MDIO_read().
             */
            MDIO_WRITE_REG( this_mdio->base_address, MDIO_CTRL_REG,
                            MDIO_CTRL_WRITE_OP );

            status = mdio_wait_not_busy( this_mdio->base_address );
        }
    }

    return ( status );
}

/*******************************************************************************
 * MDIO_self_test()
 */
uint8_t
MDIO_self_test
(
    const MDIO_instance_t * this_mdio,
    uint8_t                 phy_addr,
    uint16_t                readings[4]
)
{
    uint8_t  status;
    uint16_t local[4];
    uint8_t  i;
    int      all_same;

    /* Read four registers that cannot legitimately hold the same value:
     *   0x00 CONTROL   contains speed, duplex and auto-negotiation bits
     *   0x01 STATUS    contains link, capability and auto-negotiation bits
     *   0x02 ID1       upper half of the PHY OUI
     *   0x03 ID2       lower OUI bits plus model and revision
     */
    status = MDIO_read( this_mdio, phy_addr, MDIO_PHY_REG_CONTROL, &local[0] );

    if( status == MDIO_OK )
    {
        status = MDIO_read( this_mdio, phy_addr, MDIO_PHY_REG_STATUS,
                            &local[1] );
    }

    if( status == MDIO_OK )
    {
        status = MDIO_read( this_mdio, phy_addr, MDIO_PHY_REG_ID1, &local[2] );
    }

    if( status == MDIO_OK )
    {
        status = MDIO_read( this_mdio, phy_addr, MDIO_PHY_REG_ID2, &local[3] );
    }

    if( readings != NULL_U16 )
    {
        for( i = 0u; i < 4u; i++ )
        {
            readings[ i ] = ( status == MDIO_OK ) ? local[ i ] : 0u;
        }
    }

    if( status != MDIO_OK )
    {
        return ( status );
    }

    /* All four identical means the interface is returning a constant rather
     * than register specific data. This is the signature of a stalled DATAOUT
     * register or of transactions that never complete, and it must NOT be
     * mistaken for a working PHY.
     */
    all_same = 1;
    for( i = 1u; i < 4u; i++ )
    {
        if( local[ i ] != local[ 0 ] )
        {
            all_same = 0;
        }
    }

    if( all_same != 0 )
    {
        return ( MDIO_ERR_CONSTANT );
    }

    /* All ones on every register means no PHY is driving the line: MDIO idles
     * high, so an absent or unpowered PHY reads 0xFFFF.
     */
    if( ( local[0] == 0xFFFFu ) && ( local[1] == 0xFFFFu ) &&
        ( local[2] == 0xFFFFu ) && ( local[3] == 0xFFFFu ) )
    {
        return ( MDIO_ERR_NO_PHY );
    }

    /* A valid PHY identifier must not be all zeroes or all ones. */
    if( ( ( local[2] == 0x0000u ) && ( local[3] == 0x0000u ) ) ||
        ( ( local[2] == 0xFFFFu ) && ( local[3] == 0xFFFFu ) ) )
    {
        return ( MDIO_ERR_NO_PHY );
    }

    return ( MDIO_OK );
}

/*******************************************************************************
 * MDIO_get_phy_id()
 */
uint8_t
MDIO_get_phy_id
(
    const MDIO_instance_t * this_mdio,
    uint8_t                 phy_addr,
    uint32_t              * phy_id
)
{
    uint8_t  status = MDIO_ERR_PARAM;
    uint16_t id1 = 0u;
    uint16_t id2 = 0u;

    HAL_ASSERT( phy_id != NULL_U32 );

    if( phy_id != NULL_U32 )
    {
        status = MDIO_read( this_mdio, phy_addr, MDIO_PHY_REG_ID1, &id1 );

        if( status == MDIO_OK )
        {
            status = MDIO_read( this_mdio, phy_addr, MDIO_PHY_REG_ID2, &id2 );
        }

        if( status == MDIO_OK )
        {
            *phy_id = ( (uint32_t)id1 << 16u ) | (uint32_t)id2;
        }
    }

    return ( status );
}

/*******************************************************************************
 * MDIO_phy_reset()
 */
uint8_t
MDIO_phy_reset
(
    const MDIO_instance_t * this_mdio,
    uint8_t                 phy_addr
)
{
    uint8_t           status;
    uint16_t          ctrl = 0u;
    uint32_t          attempts;
    volatile uint32_t loop;

    status = MDIO_write( this_mdio, phy_addr, MDIO_PHY_REG_CONTROL,
                         (uint16_t)MDIO_CTRL_RESET );

    if( status == MDIO_OK )
    {
        for( loop = 0u; loop < MDIO_RESET_DELAY_LOOPS; loop++ )
        {
            /* Allow the PHY to begin its reset. */
        }

        /* The reset bit is self clearing. */
        status = MDIO_ERR_TIMEOUT;

        for( attempts = 0u; attempts < MDIO_RESET_POLL_LIMIT; attempts++ )
        {
            if( MDIO_read( this_mdio, phy_addr, MDIO_PHY_REG_CONTROL, &ctrl )
                    != MDIO_OK )
            {
                status = MDIO_ERR_TIMEOUT;
                break;
            }

            if( ( ctrl & MDIO_CTRL_RESET ) == 0u )
            {
                status = MDIO_OK;
                break;
            }
        }
    }

    return ( status );
}

/*******************************************************************************
 * MDIO_phy_config_1000_fd()
 *
 * Follows the sequence used by the reference application:
 *   write 0x16 = 0x0000   select page 0
 *   write 0x00 = 0x9140   reset with 1000 Mbps full duplex auto-neg
 *   delay
 *   write 0x00 = 0x1140   release reset, same speed settings
 */
uint8_t
MDIO_phy_config_1000_fd
(
    const MDIO_instance_t * this_mdio,
    uint8_t                 phy_addr
)
{
    uint8_t           status;
    volatile uint32_t loop;

    /* Select register page 0. Vendor specific but harmless on PHYs without
     * paging, since register 0x16 is reserved there.
     */
    status = MDIO_write( this_mdio, phy_addr, MDIO_PHY_REG_PAGE_SELECT, 0x0000u );

    if( status == MDIO_OK )
    {
        /* Reset together with the 1000 Mbps full duplex auto-neg settings. */
        status = MDIO_write( this_mdio, phy_addr, MDIO_PHY_REG_CONTROL,
                             (uint16_t)( MDIO_CTRL_RESET |
                                         MDIO_CTRL_1000_FD_AN ) );
    }

    if( status == MDIO_OK )
    {
        for( loop = 0u; loop < MDIO_RESET_DELAY_LOOPS; loop++ )
        {
            /* Hold reset briefly. */
        }

        /* Release reset, retaining the speed and duplex selection. */
        status = MDIO_write( this_mdio, phy_addr, MDIO_PHY_REG_CONTROL,
                             (uint16_t)MDIO_CTRL_1000_FD_AN );
    }

    return ( status );
}

/*******************************************************************************
 * MDIO_get_link_status()
 */
uint8_t
MDIO_get_link_status
(
    const MDIO_instance_t * this_mdio,
    uint8_t                 phy_addr,
    uint8_t               * link_up,
    uint8_t               * an_complete
)
{
    uint8_t  status;
    uint16_t stat = 0u;

    /* The link status bit latches low, so the first read may report a stale
     * transient loss. Read twice and use the second value.
     */
    status = MDIO_read( this_mdio, phy_addr, MDIO_PHY_REG_STATUS, &stat );

    if( status == MDIO_OK )
    {
        status = MDIO_read( this_mdio, phy_addr, MDIO_PHY_REG_STATUS, &stat );
    }

    if( status == MDIO_OK )
    {
        if( link_up != NULL_U8 )
        {
            *link_up = ( ( stat & MDIO_STAT_LINK_UP ) != 0u ) ? 1u : 0u;
        }

        if( an_complete != NULL_U8 )
        {
            *an_complete =
                ( ( stat & MDIO_STAT_AN_COMPLETE ) != 0u ) ? 1u : 0u;
        }
    }

    return ( status );
}

/*******************************************************************************
 * MDIO_wait_link_up()
 *
 * TIMING NOTE, and the reason this function was rewritten.
 *
 * 1000BASE-T auto-negotiation takes 1 to 3 SECONDS. IEEE 802.3 Clause 28 and 40
 * define a break_link_timer of 1.2 to 1.5 s on its own, and the full page
 * exchange plus master/slave resolution runs longer than that.
 *
 * An earlier version of this driver polled a fixed 200 iterations. At a 50 MHz
 * PCLK with prescaler 7 the MDC rate is 3.57 MHz, so one Clause 22 transaction
 * takes about 18 us and each status check performs two of them. 200 iterations
 * is therefore only about 7 ms, roughly 400 times too short. A perfectly
 * working link could never pass that test.
 *
 * This version takes a wait in MILLISECONDS and converts it internally, so the
 * caller expresses intent in a unit that matches the specification.
 */
uint8_t
MDIO_wait_link_up
(
    const MDIO_instance_t * this_mdio,
    uint8_t                 phy_addr,
    uint32_t                wait_ms,
    MDIO_link_progress_t    progress_cb
)
{
    uint8_t  status  = MDIO_ERR_TIMEOUT;
    uint8_t  link_up = 0u;
    uint8_t  an_done = 0u;
    uint16_t stat    = 0u;
    uint32_t elapsed_ms;
    uint32_t inner;

    for( elapsed_ms = 0u; elapsed_ms < wait_ms; elapsed_ms++ )
    {
        /* One outer iteration is approximately one millisecond. Each status
         * check is two MDIO transactions of about 18 us, so a handful of
         * checks plus a short pad approximates 1 ms without needing a timer.
         */
        if( MDIO_read( this_mdio, phy_addr, MDIO_PHY_REG_STATUS, &stat )
                != MDIO_OK )
        {
            return ( MDIO_ERR_TIMEOUT );
        }

        /* Read twice: the link bit latches low, so the first read may report a
         * stale transient loss.
         */
        if( MDIO_read( this_mdio, phy_addr, MDIO_PHY_REG_STATUS, &stat )
                != MDIO_OK )
        {
            return ( MDIO_ERR_TIMEOUT );
        }

        link_up = ( ( stat & MDIO_STAT_LINK_UP ) != 0u ) ? 1u : 0u;
        an_done = ( ( stat & MDIO_STAT_AN_COMPLETE ) != 0u ) ? 1u : 0u;

        /* Report progress roughly every 250 ms so the caller can show that
         * negotiation is being attempted rather than appearing to hang.
         */
        if( ( progress_cb != (MDIO_link_progress_t)0 ) &&
            ( ( elapsed_ms % 250u ) == 0u ) )
        {
            progress_cb( elapsed_ms, stat, link_up, an_done );
        }

        if( link_up != 0u )
        {
            status = MDIO_OK;
            break;
        }

        /* Pad the remainder of the millisecond. */
        for( inner = 0u; inner < MDIO_ONE_MS_PAD_LOOPS; inner++ )
        {
            /* Busy wait. */
        }
    }

    /* Final report so the caller always sees the terminating state. */
    if( progress_cb != (MDIO_link_progress_t)0 )
    {
        progress_cb( elapsed_ms, stat, link_up, an_done );
    }

    return ( status );
}

#ifdef __cplusplus
}
#endif
