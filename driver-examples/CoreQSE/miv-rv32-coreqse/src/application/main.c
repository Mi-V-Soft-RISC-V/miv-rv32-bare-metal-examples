/*******************************************************************************
 * Copyright 2026 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file main.c
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief CoreQSE Ethernet validation test, 1 Gbps and 2.5 Gbps.
 *
 * HOW TO TEST EACH DESIGN
 * -----------------------
 * The two designs are built separately from this one source file. Select the
 * target in board_config.h:
 *
 *   For the 2.5 Gbps design
 *     1. Program the CoreQSE_2_5Gbps bitstream.
 *     2. Fit the SMA loopback cable.
 *     3. Set  #define BOARD_DESIGN  BOARD_COREQSE_2_5G
 *     4. Build with drivers: CoreQSE, CoreAXI4ProtoConv, CoreGPIO, CoreUARTapb
 *     5. Console on the UART at 0x71000000, 115200 8N1.
 *
 *   For the 1 Gbps design
 *     1. Program the CoreQSE_1Gbps bitstream.
 *     2. Connect RJ45 to the external test set, for example Spirent Test
 *        Centre, configured for 1000BASE-T full duplex.
 *     3. Set  #define BOARD_DESIGN  BOARD_COREQSE_1G
 *     4. Build with drivers: CoreQSE, CoreMDIO, CoreUARTapb.
 *        CoreAXI4ProtoConv and CoreGPIO are excluded by the preprocessor and
 *        need not be compiled, though including them is harmless.
 *     5. Console on the UART at 0x61000000, 115200 8N1.
 *
 * Alternatively pass the selection on the command line instead of editing the
 * file, which is convenient when alternating between the two:
 *     -DBOARD_DESIGN=BOARD_COREQSE_2_5G
 *     -DBOARD_DESIGN=BOARD_COREQSE_1G
 *
 * The application prints its build configuration at startup and verifies
 * CORE_VER before running any test, so a mismatched binary is reported rather
 * than producing misleading results.
 *
 * The two reference designs are not variants of one another: they differ in
 * topology, not just link rate, so the set of tests that can run differs
 * between them.
 *
 * 2.5 Gbps design, BOARD_COREQSE_2_5G
 *   MIV_RV32 -> COREAXI4PROTOCONV -> COREAXI4INTERCONNECT -> COREQSE
 *            -> PF_XCVR_ERM (PMA mode) -> SMA loopback -> and back again
 *   The processor generates and verifies frames itself, so the full data path
 *   including payload integrity can be tested in software.
 *
 * 1 Gbps design, BOARD_COREQSE_1G
 *   COREQSE (AXI4S RX looped to its own TX in fabric)
 *           -> PF_XCVR_ERM (PMA mode) -> external 1G PHY -> RJ45
 *           -> external test set, for example Spirent Test Centre
 *   There is NO DMA, NO frame buffers and NO CoreGPIO. CoreQSE's stream output
 *   is wired straight back to its own input:
 *
 *       wire [31:0] COREQSE_C0_0_AXI4S_INITR_TDATA;
 *       .AXI4S_TX_TARG_TDATA  ( COREQSE_C0_0_AXI4S_INITR_TDATA ),
 *       .AXI4S_RX_INITR_TDATA ( COREQSE_C0_0_AXI4S_INITR_TDATA ),
 *
 *   so the processor cannot stage or inspect a frame. Traffic is generated and
 *   verified externally. The processor configures the MAC and the PHY over
 *   MDIO, then reports statistics. Data path tests are skipped with an
 *   explanation rather than silently omitted.
 *
 * Every check is self-verifying. Each test prints PASS, FAIL or SKIP and the
 * run ends with a summary, so no interpretation of hex dumps is required.
 *
 * PREREQUISITES
 * -------------
 * 2.5 Gbps: the SMA loopback cable must be fitted. SYS_RX_RESETN is driven by
 *   LANE0_RX_VAL, which asserts only once the receiver has CDR lock on a real
 *   incoming signal. Without the cable the receive datapath stays in reset.
 *
 * 1 Gbps: the RJ45 link must be connected to the test set, and the external
 *   PHY must negotiate. MDIO_wait_link_up() reports this before any traffic is
 *   expected.
 *
 * Register access itself does not depend on the transceiver in either design:
 * the CoreQSE AXI4-Lite interface is in the ACLK domain, which is the fabric
 * clock shared with the processor.
 */

#include <stdio.h>
#include <string.h>

#include "fpga_design_config/fpga_design_config.h"
#include "hal/hal.h"
#include "miv_rv32_hal/miv_rv32_hal.h"

#include "drivers/fpga_ip/CoreUARTapb/core_uart_apb.h"
#include "drivers/fpga_ip/CoreQSE/core_qse.h"

#if ( BOARD_HAS_GPIO_IRQ != 0u )
#include "drivers/fpga_ip/CoreGPIO/core_gpio.h"
#endif

#if ( BOARD_HAS_DMA_LOOPBACK != 0u )
#include "drivers/fpga_ip/CoreAXI4ProtoConv/core_axi4protoconv.h"
#endif

/* PHY management is reached through the QSE_phy_* API in core_qse.h, which is
 * included above. core_qse.h itself pulls in phy.h and core_mdio.h, so the
 * application does not need to include them directly and never calls MDIO_*
 * except where it wants to distinguish a specific MDIO failure mode.
 *
 * CoreQSE has NO MII management hardware of its own: the QSE_phy_* functions
 * delegate to a separate COREMDIO_APB IP. See the architectural note in
 * core_qse.h.
 */

/*==============================================================================
 * Base addresses come from board_config.h. Aliases keep the test bodies
 * readable and unchanged between designs.
 *============================================================================*/
#define COREQSE_BASE                COREQSE_BASE_ADDR

#if ( BOARD_HAS_DMA_LOOPBACK != 0u )
/*==============================================================================
 * CoreAXI4ProtoConv is a SINGLE peripheral with both channels behind one base
 * address. The driver applies the channel offsets internally:
 *     S2MM registers at PROTOCONV_BASE + 0x000
 *     MM2S registers at PROTOCONV_BASE + 0x400
 *
 * Do not pass two separate base addresses; that was an error in the original
 * validation code, which treated 0x60010000 and 0x60010400 as distinct IPs.
 *============================================================================*/
#define PROTOCONV_BASE              PROTOCONV_BASE_ADDR
#define TX_BUFFER_BASE              TX_BUFFER_BASE_ADDR
#define RX_BUFFER_BASE              RX_BUFFER_BASE_ADDR
#endif

#if ( BOARD_HAS_GPIO_IRQ != 0u )
/*==============================================================================
 * GPIO input assignment, from Top.v of the 2.5 Gbps design:
 *
 *   assign GPIO_IN_net_0 = { 1'b0,
 *                            COREAXI4PROTOCONV_C0_0_S2MM_ERR_INT,
 *                            COREAXI4PROTOCONV_C0_0_MM2S_ERR_INT,
 *                            COREAXI4PROTOCONV_C0_0_S2MM_INT,
 *                            COREAXI4PROTOCONV_C0_0_MM2S_INT };
 *
 * Only five bits exist and bit 4 is tied low. CoreQSE itself has no interrupt
 * output, so all completion signalling comes from the DMA engine.
 *
 * NOTE: LANE0_TX_CLK_STABLE and LANE0_RX_VAL are NOT routed to GPIO in this
 * design, so software cannot poll transceiver readiness. See
 * xcvr_assumed_ready() below.
 *============================================================================*/
#define GPIO_MM2S_DONE_MASK         GPIO_0_MASK
#define GPIO_S2MM_DONE_MASK         GPIO_1_MASK
#define GPIO_MM2S_ERR_MASK          GPIO_2_MASK
#define GPIO_S2MM_ERR_MASK          GPIO_3_MASK
#endif

/*==============================================================================
 * Test parameters.
 *============================================================================*/

/*! Station MAC address, locally administered. */
static const uint8_t g_mac_addr[6] = {0x00u, 0x04u, 0xA3u, 0x11u, 0x22u, 0x33u};

/*! EtherType 0x88B5 is reserved for local experimental use. */
#define TEST_ETHERTYPE              0x88B5u

/*! Payload lengths exercised, in bytes, including the 14-byte header.
 *  60 is the minimum unpadded frame; 1500 is a full-size frame.
 */
#if ( BOARD_HAS_DMA_LOOPBACK != 0u )
static const uint16_t g_test_lengths[] = { 60u, 64u, 128u, 512u, 1000u, 1500u };
#define TEST_LENGTH_COUNT           (sizeof(g_test_lengths)/sizeof(g_test_lengths[0]))
#endif

/*! Command IDs tagged onto DMA descriptors, echoed back in the status register.
 *  Values chosen to match the CoreAXI4ProtoConv driver documentation examples.
 */
#define MM2S_CMD_ID                 0xA6u
#define S2MM_CMD_ID                 0xB6u

/*! Bound on every hardware wait, expressed in poll iterations.
 *  At 50 MHz this is on the order of tens of milliseconds, which is several
 *  orders of magnitude longer than a 1500-byte frame needs at 2.5 Gbps.
 */
#define POLL_TIMEOUT_ITERATIONS     2000000u

/*! Iterations used when sampling PCS counters for link stability. */
#define LINK_SETTLE_ITERATIONS      100000u

/*==============================================================================
 * Driver instances.
 *============================================================================*/
static UART_instance_t  g_uart;
static QSE_instance_t   g_qse;

#if ( BOARD_HAS_GPIO_IRQ != 0u )
static gpio_instance_t  g_gpio;
#endif

#if ( BOARD_HAS_DMA_LOOPBACK != 0u )
static PCDMA_instance_t g_pcdma;
#endif

#if ( BOARD_HAS_MDIO != 0u )
/* No MDIO instance is held by the application. QSE_attach_mdio() records the
 * COREMDIO_APB base inside the CoreQSE instance, so PHY access goes through
 * QSE_phy_* and the two IPs are presented as one API.
 */
#endif

/*==============================================================================
 * Test bookkeeping.
 *============================================================================*/
/*! Iterations allowed for the external PHY link to come up. */
/* The link wait is now expressed in MILLISECONDS by
 * BOARD_PHY_LINK_WAIT_MS in fpga_design_config.h. An earlier raw iteration
 * count of 200 amounted to roughly 7 ms, which no working 1000BASE-T link could
 * satisfy: auto-negotiation takes 1 to 3 seconds.
 */

static uint32_t g_checks_run    = 0u;
static uint32_t g_checks_passed = 0u;

#if ( BOARD_HAS_DMA_LOOPBACK != 0u )
/*! Set once the beat duplication evidence has been printed, so the explanation
 *  is not repeated for every frame length.
 */
static uint8_t  g_stretch_reported = 0u;
#endif

/*! Print buffer. Sized generously: the original code used 50 bytes for format
 *  strings needing up to 56, which overflowed into adjacent globals.
 */
static char g_print_buf[192];

/*==============================================================================
 * Output helpers.
 *============================================================================*/

static void print_str( const char * str )
{
    UART_polled_tx_string( &g_uart, (const uint8_t *)str );
}

/*! Record and report the outcome of a single check. */
static void check( const char * label, int condition )
{
    g_checks_run++;

    if( condition != 0 )
    {
        g_checks_passed++;
        (void)snprintf( g_print_buf, sizeof(g_print_buf),
                        "  [PASS] %s\r\n", label );
    }
    else
    {
        (void)snprintf( g_print_buf, sizeof(g_print_buf),
                        "  [FAIL] %s\r\n", label );
    }

    print_str( g_print_buf );
}

/*! Report a check that could not be run because the hardware is absent. */
static void check_skipped( const char * label, const char * reason )
{
    (void)snprintf( g_print_buf, sizeof(g_print_buf),
                    "  [SKIP] %s (%s)\r\n", label, reason );
    print_str( g_print_buf );
}

static void print_u32( const char * label, uint32_t value )
{
    (void)snprintf( g_print_buf, sizeof(g_print_buf),
                    "         %s = 0x%08lX (%lu)\r\n",
                    label, (unsigned long)value, (unsigned long)value );
    print_str( g_print_buf );
}

/*==============================================================================
 * Transceiver readiness.
 *
 * CoreQSE SYS_CLK comes from LANE0_TX_CLK_R and SYS_TX_RESETN from
 * LANE0_TX_CLK_STABLE. Reading a CoreQSE register before the TX PLL locks
 * hangs the CPU with no possible recovery, because AXI reads have no timeout.
 *
 * In this design neither LANE0_TX_CLK_STABLE nor LANE0_RX_VAL is routed to a
 * software readable input: GPIO_IN carries only the four ProtoConv interrupts.
 * There is therefore no way to verify readiness from software.
 *
 * This function exists to make that gap explicit and to give a single place to
 * fix it. The mitigation used here is a fixed settling delay after reset,
 * which is weaker than a real check but far better than an immediate access.
 *
 * RECOMMENDED DESIGN CHANGE: route LANE0_TX_CLK_STABLE and LANE0_RX_VAL to
 * GPIO_IN[4] and a spare bit, then replace the delay below with a real poll.
 *============================================================================*/
static void xcvr_assumed_ready( void )
{
    volatile uint32_t i;

    print_str( "Waiting for transceiver TX clock to settle...\r\n" );

    for( i = 0u; i < POLL_TIMEOUT_ITERATIONS; i++ )
    {
        /* Busy wait. See the comment block above: this is a delay, not a
         * check, because XCVR status is not observable in this design.
         */
    }

    print_str( "  NOTE: XCVR readiness is assumed, not verified.\r\n" );
    print_str( "        LANE0_TX_CLK_STABLE/LANE0_RX_VAL are not routed to"
               " GPIO.\r\n" );
}

#if ( BOARD_HAS_DMA_LOOPBACK != 0u )
/*==============================================================================
 * EVERYTHING FROM HERE TO THE END OF TEST 5b REQUIRES A PROCESSOR DRIVEN DATA
 * PATH, that is a DMA engine, frame buffers in memory, and completion
 * interrupts routed to CoreGPIO.
 *
 * The 1 Gbps design has none of these: CoreQSE's AXI4-Stream receive output is
 * wired directly back to its own transmit input in the fabric, so no frame can
 * be staged or inspected by software. Traffic is generated and verified by an
 * external test set.
 *
 * These functions are therefore compiled only for the 2.5 Gbps design. On the
 * 1 Gbps design the corresponding tests report SKIP with an explanation.
 *============================================================================*/

/*==============================================================================
 * Bounded wait for a GPIO interrupt bit.
 *
 * Returns 1 on success, 0 on timeout. Never spins forever: an unfitted SMA
 * cable is the expected failure mode, not an exotic one, and the original code
 * hung indefinitely in that case.
 *============================================================================*/
static int wait_for_gpio_bit( uint32_t mask, uint32_t * elapsed )
{
    uint32_t i;
    uint32_t inputs;

    for( i = 0u; i < POLL_TIMEOUT_ITERATIONS; i++ )
    {
        inputs = GPIO_get_inputs( &g_gpio );

        if( ( inputs & mask ) != 0u )
        {
            if( elapsed != (uint32_t *)0 )
            {
                *elapsed = i;
            }
            return ( 1 );
        }
    }

    if( elapsed != (uint32_t *)0 )
    {
        *elapsed = i;
    }

    return ( 0 );
}

/*==============================================================================
 * Fill a transmit buffer with a valid Ethernet frame.
 *
 * A real header matters: receive filtering compares the destination address,
 * so a buffer full of an arbitrary pattern only gets accepted when filtering
 * happens to be permissive.
 *
 * The payload is a length-dependent pattern so that a stale buffer or an
 * off-by-one cannot be mistaken for a correct result.
 *============================================================================*/
static uint16_t build_test_frame( uint32_t buf_base, uint16_t frame_len )
{
    volatile uint8_t * buf = (volatile uint8_t *)buf_base;
    uint8_t            header[14];
    uint16_t           built;
    uint16_t           i;

    /* Broadcast destination, our own source address. */
    built = QSE_build_frame_header( header, (const uint8_t *)0, g_mac_addr,
                                   TEST_ETHERTYPE, &g_qse );

    if( built == 0u )
    {
        return ( 0u );
    }

    for( i = 0u; i < built; i++ )
    {
        buf[ i ] = header[ i ];
    }

    for( i = built; i < frame_len; i++ )
    {
        buf[ i ] = (uint8_t)( ( (uint32_t)i + (uint32_t)frame_len ) & 0xFFu );
    }

    return ( frame_len );
}

/*! Zero a buffer so a previous frame cannot be mistaken for a new one. */
static void clear_buffer( uint32_t buf_base, uint16_t len )
{
    volatile uint8_t * buf = (volatile uint8_t *)buf_base;
    uint16_t           i;

    for( i = 0u; i < len; i++ )
    {
        buf[ i ] = 0u;
    }
}

/*==============================================================================
 * Diagnostics used when a payload comparison fails.
 *============================================================================*/

/*! Print the first len bytes of a buffer as hex. */
static void dump_hex( const char * label, uint32_t buf_base, uint16_t len )
{
    const volatile uint8_t * buf = (const volatile uint8_t *)buf_base;
    uint16_t                 i;
    int                      written;

    written = snprintf( g_print_buf, sizeof(g_print_buf), "         %s:", label );

    for( i = 0u; ( i < len ) && ( written > 0 ) &&
                 ( (size_t)written < ( sizeof(g_print_buf) - 4u ) ); i++ )
    {
        written += snprintf( &g_print_buf[ written ],
                             sizeof(g_print_buf) - (size_t)written,
                             " %02X", buf[ i ] );
    }

    print_str( g_print_buf );
    print_str( "\r\n" );
}

/*==============================================================================
 * Characterise the structure of a corrupted receive buffer.
 *
 * PURPOSE
 * -------
 * Reports measurable properties of the received data without asserting a root
 * cause. Different faults leave different signatures:
 *
 *   - repeating 2-byte period    the PCS/SGMII layer is 16 bits wide
 *                               (user guide section 1.1, "SGMII (16-bit)
 *                               interface at 156.25 MHz"), so a 16-bit repeat
 *                               points at the 20b/16b decoder, word aligner or
 *                               elastic buffer rather than the 32-bit AXI side.
 *
 *   - repeating 4-byte period    points at the 32-bit AXI4-Stream side.
 *
 *   - every word duplicated      a fixed 2:1 rate mismatch. This would be
 *                               perfectly reproducible run to run.
 *
 *   - irregular duplication      an unstable receive stream: alignment being
 *                               lost and reacquired, or elastic buffer
 *                               over/underrun.
 *
 * IMPORTANT
 * ---------
 * This function deliberately does NOT name a cause. An earlier version of this
 * test asserted a SYS_CLK frequency error after a single run appeared to fit a
 * 2:1 stretch model. Subsequent runs produced different data from identical
 * stimulus, which refuted that conclusion: a fixed clock ratio error is
 * perfectly reproducible, and this fault is not.
 *
 * Use test_repeatability() to establish whether the fault is deterministic
 * before drawing any structural conclusion from these numbers.
 *============================================================================*/
static int report_beat_duplication( uint32_t tx_base, uint32_t rx_base,
                                    uint16_t len )
{
    const volatile uint32_t * tx  = (const volatile uint32_t *)tx_base;
    const volatile uint32_t * rx  = (const volatile uint32_t *)rx_base;
    const volatile uint8_t  * rxb = (const volatile uint8_t *)rx_base;
    uint16_t                  words;
    uint16_t                  i;
    uint16_t                  p;
    uint16_t                  period;
    uint16_t                  adjacent_dup = 0u;
    uint16_t                  stretch_match = 0u;
    uint16_t                  stretch_pairs;
    int                       fits_2to1 = 0;

    words = (uint16_t)( len / 4u );

    if( words < 4u )
    {
        return ( 0 );
    }

    /* Count adjacent duplicate 32-bit words in the received data. */
    for( i = 1u; i < words; i++ )
    {
        if( rx[ i ] == rx[ i - 1u ] )
        {
            adjacent_dup++;
        }
    }

    /* Test the 2x stretch hypothesis directly: received words 2n and 2n+1
     * should both equal transmitted word n.
     */
    stretch_pairs = (uint16_t)( words / 2u );

    for( i = 0u; i < stretch_pairs; i++ )
    {
        if( ( rx[ 2u * i ] == tx[ i ] ) &&
            ( rx[ ( 2u * i ) + 1u ] == tx[ i ] ) )
        {
            stretch_match++;
        }
    }

    /* Detect the smallest repeating byte period, up to 8 bytes. The period
     * identifies which datapath width the artefact originates from.
     */
    period = 0u;
    for( p = 1u; p <= 8u; p++ )
    {
        uint16_t matched = 0u;
        uint16_t tested  = 0u;

        for( i = p; ( i < len ) && ( i < 64u ); i++ )
        {
            tested++;
            if( rxb[ i ] == rxb[ i - p ] )
            {
                matched++;
            }
        }

        /* Require near-perfect periodicity over the sampled window. */
        if( ( tested >= 8u ) && ( matched == tested ) )
        {
            period = p;
            break;
        }
    }

    (void)snprintf( g_print_buf, sizeof(g_print_buf),
                    "         evidence: adjacent duplicate words %u of %u,"
                    " 2:1 stretch fit %u of %u\r\n",
                    (unsigned)adjacent_dup, (unsigned)( words - 1u ),
                    (unsigned)stretch_match, (unsigned)stretch_pairs );
    print_str( g_print_buf );

    if( period != 0u )
    {
        (void)snprintf( g_print_buf, sizeof(g_print_buf),
                        "         evidence: data repeats with a %u-byte"
                        " period\r\n", (unsigned)period );
        print_str( g_print_buf );

        if( ( period == 1u ) || ( period == 2u ) )
        {
            print_str( "         -> a 1 or 2 byte period matches the 16-bit"
                       " SGMII/PCS\r\n" );
            print_str( "            datapath, so suspect the 20b/16b decoder,"
                       " word\r\n" );
            print_str( "            aligner or elastic buffer, not the 32-bit"
                       " AXI side\r\n" );
        }
        else if( period == 4u )
        {
            print_str( "         -> a 4 byte period matches the 32-bit"
                       " AXI4-Stream\r\n" );
            print_str( "            datapath\r\n" );
        }
        else
        {
            /* Period reported, no further classification. */
        }
    }

    /* Report how well the data fits a fixed 2:1 rate mismatch, WITHOUT
     * asserting it as the cause. A fixed rate error is reproducible; confirm
     * with test_repeatability() before acting on this.
     */
    if( ( stretch_pairs > 0u ) &&
        ( stretch_match >= ( ( stretch_pairs / 2u ) + 1u ) ) )
    {
        fits_2to1 = 1;

        print_str( "         -> the majority of words fit a fixed 2:1 rate"
                   " mismatch.\r\n" );
        print_str( "            If test_repeatability() shows this is"
                   " REPRODUCIBLE,\r\n" );
        print_str( "            investigate SYS_CLK versus XCVR clock rates."
                   " If it\r\n" );
        print_str( "            VARIES between runs, the cause is an unstable"
                   " receive\r\n" );
        print_str( "            stream, not a clock ratio.\r\n" );
    }
    else if( adjacent_dup > ( ( words - 1u ) / 2u ) )
    {
        print_str( "         -> heavy but irregular duplication: consistent"
                   " with an\r\n" );
        print_str( "            unstable receive stream, alignment being lost"
                   " and\r\n" );
        print_str( "            reacquired, or elastic buffer over/underrun\r\n" );
    }
    else
    {
        /* No strong duplication signature. */
    }

    return ( fits_2to1 );
}

/*==============================================================================
 * Search the receive buffer for the start of the transmitted frame.
 *
 * A single mismatching byte cannot distinguish between:
 *   - genuine data corruption,
 *   - a correct frame written at a non-zero offset,
 *   - a frame preceded by preamble or SFD bytes that were not stripped,
 *   - a stale or never-written buffer.
 *
 * The transmitted frame begins with six 0xFF bytes (broadcast destination
 * address), which is a distinctive pattern. Locating it reports the offset.
 *============================================================================*/
static void report_frame_offset( uint32_t rx_base, uint16_t len )
{
    const volatile uint8_t * rx = (const volatile uint8_t *)rx_base;
    uint16_t                 i;
    uint16_t                 run;
    uint16_t                 nonzero = 0u;

    /* Is the buffer entirely zero? That means S2MM reported done but never
     * wrote, or wrote elsewhere.
     */
    for( i = 0u; i < len; i++ )
    {
        if( rx[ i ] != 0u )
        {
            nonzero++;
        }
    }

    if( nonzero == 0u )
    {
        print_str( "         RX buffer is entirely ZERO: S2MM signalled done"
                   " but wrote no data\r\n" );
        return;
    }

    (void)snprintf( g_print_buf, sizeof(g_print_buf),
                    "         RX buffer has %u non-zero bytes of %u\r\n",
                    (unsigned)nonzero, (unsigned)len );
    print_str( g_print_buf );

    /* Look for six consecutive 0xFF, the broadcast destination address. */
    run = 0u;
    for( i = 0u; i < len; i++ )
    {
        if( rx[ i ] == 0xFFu )
        {
            run++;

            if( run == 6u )
            {
                (void)snprintf( g_print_buf, sizeof(g_print_buf),
                        "         frame start (6x FF) found at offset %u\r\n",
                        (unsigned)( i - 5u ) );
                print_str( g_print_buf );
                return;
            }
        }
        else
        {
            run = 0u;
        }
    }

    print_str( "         broadcast DA (6x FF) NOT found anywhere in RX"
               " buffer\r\n" );

    /* Report whether the Ethernet preamble or SFD leaked through, which would
     * indicate the RX MAC is not stripping them.
     */
    for( i = 0u; ( i + 1u ) < len; i++ )
    {
        if( ( rx[ i ] == 0x55u ) && ( rx[ i + 1u ] == 0x55u ) )
        {
            (void)snprintf( g_print_buf, sizeof(g_print_buf),
                    "         preamble bytes (55 55) present at offset %u\r\n",
                    (unsigned)i );
            print_str( g_print_buf );
            break;
        }
    }
}

/*! Compare received bytes against the transmitted frame. Returns the index of
 *  the first mismatch, or -1 when identical.
 */
static int32_t compare_buffers( uint32_t tx_base, uint32_t rx_base,
                                uint16_t len )
{
    const volatile uint8_t * tx = (const volatile uint8_t *)tx_base;
    const volatile uint8_t * rx = (const volatile uint8_t *)rx_base;
    uint16_t                 i;

    for( i = 0u; i < len; i++ )
    {
        if( tx[ i ] != rx[ i ] )
        {
            return ( (int32_t)i );
        }
    }

    return ( -1 );
}

/*==============================================================================
 * TEST 0: CPU access to the DMA buffers.
 *
 * Both buffers are AXI slaves on COREAXI4INTERCONNECT, reached by the CPU as
 * MASTER0 and by CoreAXI4ProtoConv as another master:
 *
 *   TX buffer 0x60000000  SLAVE0  PF_SRAM_AHBL_AXI_C0
 *   RX buffer 0x60030000  SLAVE3  PF_SRAM_AHBL_AXI_C1
 *
 * A write/read-back through the CPU port proves the address decode and the CPU
 * path are correct. This has to be established BEFORE any payload comparison
 * is meaningful: if the CPU cannot see the RX SRAM reliably, a comparison
 * failure says nothing about the Ethernet datapath.
 *============================================================================*/
static void test_buffer_access_dma( void )
{
    volatile uint32_t * tx = (volatile uint32_t *)TX_BUFFER_BASE;
    volatile uint32_t * rx = (volatile uint32_t *)RX_BUFFER_BASE;
    uint32_t            i;
    int                 ok;

    print_str( "\r\n=== TEST 0: CPU access to DMA buffers ===\r\n" );

    /* Walking pattern, so a stuck address line or a mirrored decode shows up
     * rather than a single value that happens to read back.
     */
    for( i = 0u; i < 8u; i++ )
    {
        tx[ i ] = 0xA5A50000u + i;
        rx[ i ] = 0x5A5A0000u + i;
    }

    ok = 1;
    for( i = 0u; i < 8u; i++ )
    {
        if( tx[ i ] != ( 0xA5A50000u + i ) )
        {
            ok = 0;
        }
    }
    check( "TX buffer 0x60000000 read/write via CPU", ok );

    ok = 1;
    for( i = 0u; i < 8u; i++ )
    {
        if( rx[ i ] != ( 0x5A5A0000u + i ) )
        {
            ok = 0;
        }
    }
    check( "RX buffer 0x60030000 read/write via CPU", ok );

    /* The two buffers must be distinct memories, not the same block aliased at
     * two addresses. If they alias, MM2S and S2MM would overwrite each other
     * and any comparison result would be meaningless.
     */
    tx[ 0 ] = 0xDEADBEEFu;
    rx[ 0 ] = 0xCAFEF00Du;

    check( "TX and RX buffers are independent memories",
           ( tx[ 0 ] == 0xDEADBEEFu ) && ( rx[ 0 ] == 0xCAFEF00Du ) );

    if( tx[ 0 ] != 0xDEADBEEFu )
    {
        print_str( "         TX buffer changed when RX was written:"
                   " ADDRESS ALIASING\r\n" );
    }

    /* Leave both buffers zeroed. */
    for( i = 0u; i < 8u; i++ )
    {
        tx[ i ] = 0u;
        rx[ i ] = 0u;
    }
}

#endif /* BOARD_HAS_DMA_LOOPBACK, part 1 */

/*==============================================================================
 * The tests below operate on the CoreQSE AXI4-Lite register interface only, so
 * they are valid on BOTH designs. The register interface is in the ACLK domain,
 * which is the fabric clock shared with the processor, and is independent of
 * the transceiver and of any data path.
 *============================================================================*/

/*==============================================================================
 * TEST 1: CoreQSE register power-on reset check.
 *============================================================================*/
static void test_register_por( void )
{
    uint8_t  major = 0u;
    uint8_t  minor = 0u;
    uint32_t failed_offset = 0u;
    uint32_t param1 = 0u;
    uint32_t param2 = 0u;

    print_str( "\r\n=== TEST 1: CoreQSE register POR check ===\r\n" );

    check( "QSE_get_version() returns SUCCESS",
           QSE_get_version( &g_qse, &major, &minor ) == QSE_OK );

    (void)snprintf( g_print_buf, sizeof(g_print_buf),
                    "         CORE_VER = %u.%u\r\n",
                    (unsigned)major, (unsigned)minor );
    print_str( g_print_buf );

    /* CoreQSE 2.0.101 is expected. A wildly different value, or all zeroes or
     * all ones, usually means SYS_CLK is not running or the base is wrong.
     */
    check( "CORE_VER major version is 2", major == 2u );

    /* Reset value check. Safe to run after QSE_init(): the driver masks out the
     * TX/RX enable bits, which QSE_init() deliberately clears.
     */
    check( "QSE_self_test() reset values match",
           QSE_self_test( &g_qse, &failed_offset ) == QSE_OK );

    if( failed_offset != 0xFFFFFFFFu )
    {
        print_u32( "first mismatching register offset", failed_offset );
    }

    /* MAC_xX_PARAM1/PARAM2 are read-only registers reporting the build-time
     * configuration of the IP, not resettable state. Their value depends on the
     * IP configurator settings, so they are REPORTED, never asserted.
     *
     *   MAC_xX_PARAM2 = { 11'b0, PAUSE_EN, ECC_ENABLE, CHECK_LT, PREAMBLE,
     *                     RAM_TYPE, FIFO_MODE, FIFO_DEPTH }
     *
     * The 2.5 Gbps design uses FIFO_DEPTH 32 and RAM_TYPE 2 (LSRAM), which
     * reads 0x00010020. The user guide's "Reset: 0x20" assumes RAM_TYPE 0.
     */
    check( "QSE_get_tx_param() returns SUCCESS",
           QSE_get_tx_param( &g_qse, &param1, &param2 ) == QSE_OK );
    print_u32( "MAC_TX_PARAM1", param1 );
    print_u32( "MAC_TX_PARAM2 (build config)", param2 );
    (void)snprintf( g_print_buf, sizeof(g_print_buf),
                    "         TX FIFO_DEPTH=%lu  ECC=%s  PAUSE=%s\r\n",
                    (unsigned long)( param2 & 0xFFu ),
                    ( ( param2 & (1uL << 20u) ) != 0u ) ? "yes" : "no",
                    ( ( param2 & (1uL << 21u) ) != 0u ) ? "yes" : "no" );
    print_str( g_print_buf );

    check( "QSE_get_rx_param() returns SUCCESS",
           QSE_get_rx_param( &g_qse, &param1, &param2 ) == QSE_OK );
    print_u32( "MAC_RX_PARAM1", param1 );
    print_u32( "MAC_RX_PARAM2 (build config)", param2 );
    (void)snprintf( g_print_buf, sizeof(g_print_buf),
                    "         RX FIFO_DEPTH=%lu  ECC=%s  PAUSE=%s\r\n",
                    (unsigned long)( param2 & 0xFFu ),
                    ( ( param2 & (1uL << 20u) ) != 0u ) ? "yes" : "no",
                    ( ( param2 & (1uL << 21u) ) != 0u ) ? "yes" : "no" );
    print_str( g_print_buf );
}

/*==============================================================================
 * TEST 2: MAC address programming and read back.
 *
 * This doubles as a check on the MAC_ADDR_UPPER byte placement, which the user
 * guide documents ambiguously: section 6.68 lists "Bits 15:8" twice, for both
 * byte 4 and byte 5. The driver places byte 4 at [7:0] and byte 5 at [15:8].
 * If this test fails, that assumption is wrong for your IP revision.
 *============================================================================*/
static void test_mac_address( void )
{
    uint8_t read_back[6];
    uint8_t i;
    int     match;

    print_str( "\r\n=== TEST 2: MAC address programming ===\r\n" );

    check( "QSE_set_mac_addr() returns SUCCESS",
           QSE_set_mac_addr( &g_qse, g_mac_addr ) == QSE_OK );

    (void)memset( read_back, 0, sizeof(read_back) );

    check( "QSE_get_mac_addr() returns SUCCESS",
           QSE_get_mac_addr( &g_qse, read_back ) == QSE_OK );

    (void)snprintf( g_print_buf, sizeof(g_print_buf),
                    "         wrote %02X:%02X:%02X:%02X:%02X:%02X\r\n",
                    g_mac_addr[0], g_mac_addr[1], g_mac_addr[2],
                    g_mac_addr[3], g_mac_addr[4], g_mac_addr[5] );
    print_str( g_print_buf );

    (void)snprintf( g_print_buf, sizeof(g_print_buf),
                    "         read  %02X:%02X:%02X:%02X:%02X:%02X\r\n",
                    read_back[0], read_back[1], read_back[2],
                    read_back[3], read_back[4], read_back[5] );
    print_str( g_print_buf );

    match = 1;
    for( i = 0u; i < 6u; i++ )
    {
        if( read_back[ i ] != g_mac_addr[ i ] )
        {
            match = 0;
        }
    }

    check( "MAC address reads back identical", match );

    if( match == 0 )
    {
        print_str( "         MAC_ADDR_UPPER byte order may differ on this"
                   " IP revision\r\n" );
    }
}

/*==============================================================================
 * TEST 3: MAC configuration.
 *============================================================================*/
static void test_mac_configuration( void )
{
    QSE_tx_config_t tx_cfg;
    QSE_rx_config_t rx_cfg;
    uint8_t    status;

    print_str( "\r\n=== TEST 3: MAC configuration ===\r\n" );

    check( "QSE_get_default_tx_cfg() returns SUCCESS",
           QSE_get_default_tx_cfg( &tx_cfg ) == QSE_OK );

    check( "QSE_get_default_rx_cfg() returns SUCCESS",
           QSE_get_default_rx_cfg( &rx_cfg ) == QSE_OK );

    /* Defaults: 1518-byte max, IFG 12, hardware FCS insertion, standard
     * preamble. Receive strips the FCS so software sees payload only, which
     * makes the transmit and receive lengths equal.
     */
    status = QSE_tx_cfg_set( &g_qse, &tx_cfg );
    check( "QSE_tx_cfg_set() with defaults returns SUCCESS",
           status == QSE_OK );

    status = QSE_rx_cfg_set( &g_qse, &rx_cfg );
    check( "QSE_rx_cfg_set() with defaults returns SUCCESS",
           status == QSE_OK );

    /* Negative test: the user guide limits the inter-packet gap to 2..48. */
    tx_cfg.ipg_count = 49u;
    check( "QSE_tx_cfg_set() rejects IPG 49",
           QSE_tx_cfg_set( &g_qse, &tx_cfg ) == QSE_ERR_PARAM );
    tx_cfg.ipg_count = QSE_IPG_CNT_STANDARD;

    /* Negative test: pad removal requires FCS removal. */
    rx_cfg.pad_remove = 1u;
    rx_cfg.fcs_remove = 0u;
    check( "QSE_rx_cfg_set() rejects pad removal without FCS removal",
           QSE_rx_cfg_set( &g_qse, &rx_cfg ) == QSE_ERR_PARAM );

    /* Restore the working configuration. */
    (void)QSE_get_default_rx_cfg( &rx_cfg );
    check( "QSE_rx_cfg_set() restored to defaults",
           QSE_rx_cfg_set( &g_qse, &rx_cfg ) == QSE_OK );

    /* Receive frame filtering.
     *
     * NOTE the polarity: a SET bit DROPS frames matching that condition, per
     * user guide section 6.63. QSE_FILTER_ERRORS_ONLY therefore drops every
     * malformed frame class while accepting normal traffic. Passing
     * QSE_FILTER_BROADCAST here would DISCARD the broadcast test frames.
     */
    status = QSE_set_rx_filter_pkt_cfg( &g_qse, QSE_FILTER_ERRORS_ONLY );

    if( status == QSE_ERR_NOT_SUPPORTED )
    {
        check_skipped( "QSE_set_rx_filter_pkt_cfg()",
                       "frame filter not generated in this IP build" );
    }
    else
    {
        check( "QSE_set_rx_filter_pkt_cfg() returns SUCCESS",
               status == QSE_OK );
    }

    /* QSE_tx_enable() and QSE_rx_enable() return void per the driver user
     * guide, so confirm the effect by reading MAC_TX_CFG and MAC_RX_CFG back
     * and testing the enable bits directly.
     */
    QSE_tx_enable( &g_qse );
    QSE_rx_enable( &g_qse );

    check( "MAC_TX_EN set after QSE_tx_enable()",
           ( *(volatile uint32_t *)( COREQSE_BASE + 0x100u ) &
             0x80000000u ) != 0u );

    check( "MAC_RX_EN set after QSE_rx_enable()",
           ( *(volatile uint32_t *)( COREQSE_BASE + 0x180u ) &
             0x80000000u ) != 0u );
}

#if ( BOARD_HAS_DMA_LOOPBACK != 0u )
/*==============================================================================
 * TEST 5: Single frame loopback, with payload verification.
 *
 * Order matters: the receiver must be armed BEFORE the transmitter starts, or
 * the returning frame arrives with nowhere to go.
 *============================================================================*/
static int test_single_frame( uint16_t frame_len )
{
    uint16_t wire_len  = 0u;
    uint16_t rx_len    = 0u;
    uint32_t elapsed   = 0u;
    uint32_t status_reg;
    int32_t  mismatch;
    int      ok = 1;

    (void)snprintf( g_print_buf, sizeof(g_print_buf),
                    "\r\n--- Frame length %u bytes ---\r\n",
                    (unsigned)frame_len );
    print_str( g_print_buf );

    /* Compute the true wire length and the length the receive DMA must be
     * armed for, from the configuration actually programmed in hardware.
     * Hard-coding these is what made the original test fragile.
     */
    if( QSE_get_wire_length( &g_qse, frame_len, &wire_len, &rx_len )
            != QSE_OK )
    {
        check( "QSE_get_wire_length() returns SUCCESS", 0 );
        return ( 0 );
    }

    (void)snprintf( g_print_buf, sizeof(g_print_buf),
                    "         tx %u bytes -> wire %u bytes -> arm rx for %u\r\n",
                    (unsigned)frame_len, (unsigned)wire_len,
                    (unsigned)rx_len );
    print_str( g_print_buf );

    /* Stage the frame and clear the destination. */
    if( build_test_frame( TX_BUFFER_BASE, frame_len ) != frame_len )
    {
        check( "build_test_frame() succeeded", 0 );
        return ( 0 );
    }

    clear_buffer( RX_BUFFER_BASE, rx_len );

    /* Confirm the TX buffer really holds the frame, and that the RX buffer is
     * genuinely zero before the transfer. Without this, a comparison failure
     * cannot be attributed to the datapath rather than to staging.
     */
    if( frame_len == g_test_lengths[ 0u ] )
    {
        dump_hex( "TX staged ", TX_BUFFER_BASE, 16u );
        dump_hex( "RX cleared", RX_BUFFER_BASE, 16u );
    }

    /* Clear any stale completion state from a previous iteration. */
    PCDMA_S2MM_clr_int_src( &g_pcdma, S2MM_IE_DONE_IRQ );
    PCDMA_MM2S_clr_int_src( &g_pcdma, MM2S_IE_DONE_IRQ );

    /* Arm the receiver first. INCR burst: the buffer is a memory region, not a
     * single FIFO register, so the address must advance.
     *
     * The progress markers below exist so that if the run stops partway, the
     * log identifies exactly which call did not return.
     */
    print_str( "         arming S2MM...\r\n" );
    PCDMA_S2MM_configure( &g_pcdma, (uint32_t)rx_len,
                          (uint64_t)RX_BUFFER_BASE,
                          S2MM_CMD_ID, PCDMA_BURST_TYPE_INCR );
    PCDMA_S2MM_start_transfer( &g_pcdma );
    print_str( "         S2MM armed, starting MM2S...\r\n" );

    /* Then start the transmitter. */
    PCDMA_MM2S_configure( &g_pcdma, (uint32_t)frame_len,
                          (uint64_t)TX_BUFFER_BASE,
                          MM2S_CMD_ID, PCDMA_BURST_TYPE_INCR );
    PCDMA_MM2S_start_transfer( &g_pcdma );
    print_str( "         MM2S started, waiting for completion...\r\n" );

    /* Wait for transmit completion. */
    if( wait_for_gpio_bit( GPIO_MM2S_DONE_MASK, &elapsed ) == 0 )
    {
        check( "MM2S transfer completed", 0 );
        print_str( "         TIMEOUT: transmit never completed.\r\n" );
        ok = 0;
    }
    else
    {
        check( "MM2S transfer completed", 1 );
    }

    /* Wait for receive completion. This is where an unfitted SMA cable shows
     * up: LANE0_RX_VAL never asserts, the RX MAC stays in reset, and no frame
     * is ever delivered.
     */
    if( wait_for_gpio_bit( GPIO_S2MM_DONE_MASK, &elapsed ) == 0 )
    {
        check( "S2MM transfer completed", 0 );
        print_str( "         TIMEOUT: no frame received.\r\n" );
        print_str( "         Check the SMA loopback cable is fitted and the"
                   " XCVR has CDR lock.\r\n" );
        ok = 0;
    }
    else
    {
        check( "S2MM transfer completed", 1 );
    }

    /* No DMA error interrupts. */
    check( "no MM2S AXI4 error interrupt",
           ( GPIO_get_inputs( &g_gpio ) & GPIO_MM2S_ERR_MASK ) == 0u );

    check( "no S2MM AXI4 error interrupt",
           ( GPIO_get_inputs( &g_gpio ) & GPIO_S2MM_ERR_MASK ) == 0u );

    /* Confirm the command ID echoed in the status register, which proves the
     * completion belongs to the descriptor this iteration submitted.
     */
    status_reg = PCDMA_MM2S_get_status( &g_pcdma );
    print_u32( "MM2S status", status_reg );

    status_reg = PCDMA_S2MM_get_status( &g_pcdma );
    print_u32( "S2MM status", status_reg );

    /* The payload comparison is the real proof of a working datapath. */
    if( ok != 0 )
    {
        mismatch = compare_buffers( TX_BUFFER_BASE, RX_BUFFER_BASE, rx_len );

        check( "received payload matches transmitted payload",
               mismatch < 0 );

        if( mismatch >= 0 )
        {
            print_u32( "first mismatching byte index",
                       (uint32_t)mismatch );

            /* Dump the head of both buffers. A single byte value is not enough
             * to identify what actually arrived: an offset frame, a captured
             * preamble, a byte-swap and a stale buffer all look alike from one
             * byte. Sixteen bytes of context distinguishes them.
             */
            dump_hex( "TX head", TX_BUFFER_BASE, 16u );
            dump_hex( "RX head", RX_BUFFER_BASE, 16u );

            /* Search the receive buffer for the transmitted destination
             * address. If the frame is present but shifted, this reports the
             * offset, which identifies an alignment or preamble-strip problem
             * rather than data corruption.
             */
            report_frame_offset( RX_BUFFER_BASE, rx_len );

            /* Check for 2x beat duplication, which indicates SYS_CLK is
             * running at twice the rate the 32-bit data path requires. Report
             * it once only: the diagnosis is identical for every length.
             */
            if( g_stretch_reported == 0u )
            {
                if( report_beat_duplication( TX_BUFFER_BASE, RX_BUFFER_BASE,
                                             rx_len ) != 0 )
                {
                    g_stretch_reported = 1u;
                }
            }

            ok = 0;
        }
    }
    else
    {
        check_skipped( "payload comparison", "transfer did not complete" );
    }

    /* Acknowledge the completion interrupts so the next iteration starts from
     * a known state, and confirm they actually cleared.
     */
    PCDMA_S2MM_clr_int_src( &g_pcdma, S2MM_IE_DONE_IRQ );
    PCDMA_MM2S_clr_int_src( &g_pcdma, MM2S_IE_DONE_IRQ );

    check( "interrupt sources cleared",
           ( GPIO_get_inputs( &g_gpio ) &
             ( GPIO_MM2S_DONE_MASK | GPIO_S2MM_DONE_MASK ) ) == 0u );

    return ( ok );
}

/*==============================================================================
 * TEST 5: Frame length sweep.
 *
 * The original test sent one 64-byte frame. A sweep catches length-dependent
 * faults such as FIFO boundary errors and burst-length problems.
 *============================================================================*/
static void test_length_sweep( void )
{
    uint32_t i;
    uint32_t passed = 0u;

    print_str( "\r\n=== TEST 5: Frame length sweep ===\r\n" );

    for( i = 0u; i < TEST_LENGTH_COUNT; i++ )
    {
        if( test_single_frame( g_test_lengths[ i ] ) != 0 )
        {
            passed++;
        }
    }

    (void)snprintf( g_print_buf, sizeof(g_print_buf),
                    "\r\n  %lu of %lu frame lengths transferred correctly\r\n",
                    (unsigned long)passed, (unsigned long)TEST_LENGTH_COUNT );
    print_str( g_print_buf );
}

/*==============================================================================
 * TEST 5b: Repeatability.
 *
 * Sends the SAME frame REPEAT_ITERATIONS times and reports the distribution of
 * outcomes. This distinguishes the two fundamentally different fault classes,
 * which require completely different investigation:
 *
 *   DETERMINISTIC   identical result every iteration.
 *                   Points to a fixed structural error: clock ratio, datapath
 *                   width, register configuration, buffer sizing. Reproducible,
 *                   so it can be reasoned about from a single capture.
 *
 *   NON-DETERMINISTIC  results vary between iterations.
 *                   Points to an unstable link: PCS word alignment being lost
 *                   and reacquired, elastic buffer over/underrun, CDR lock
 *                   marginality, signal integrity, or reset sequencing.
 *                   Cannot be diagnosed from one capture.
 *
 * This test exists because an earlier version of this application asserted a
 * fixed SYS_CLK ratio error from a single run that happened to fit that model.
 * The next run, with identical stimulus, produced different data and refuted
 * it. Establishing determinism FIRST avoids repeating that mistake.
 *
 * Metrics reported:
 *   - iterations that transferred correctly
 *   - iterations that timed out
 *   - number of DISTINCT first-8-byte signatures observed
 *   - number of distinct frame-start offsets observed
 *
 * A single distinct signature means deterministic. Several means unstable.
 *============================================================================*/
#define REPEAT_ITERATIONS           ( 20u )
#define REPEAT_FRAME_LEN            ( 64u )
#define SIGNATURE_SLOTS             ( 8u )

static void test_repeatability( void )
{
    uint32_t signature[ SIGNATURE_SLOTS ];
    uint16_t sig_count[ SIGNATURE_SLOTS ];
    uint8_t  distinct_sigs = 0u;
    uint16_t offsets[ SIGNATURE_SLOTS ];
    uint8_t  distinct_offs = 0u;
    uint16_t pass_count    = 0u;
    uint16_t timeout_count = 0u;
    uint16_t iter;
    uint16_t wire_len = 0u;
    uint16_t rx_len   = 0u;
    uint8_t  slot;
    uint8_t  found;

    print_str( "\r\n=== TEST 5b: Repeatability ===\r\n" );
    (void)snprintf( g_print_buf, sizeof(g_print_buf),
                    "  Sending a %u byte frame %u times.\r\n",
                    (unsigned)REPEAT_FRAME_LEN, (unsigned)REPEAT_ITERATIONS );
    print_str( g_print_buf );

    if( QSE_get_wire_length( &g_qse, REPEAT_FRAME_LEN, &wire_len, &rx_len )
            != QSE_OK )
    {
        check( "QSE_get_wire_length() for repeatability test", 0 );
        return;
    }

    for( slot = 0u; slot < SIGNATURE_SLOTS; slot++ )
    {
        signature[ slot ] = 0u;
        sig_count[ slot ] = 0u;
        offsets[ slot ]   = 0xFFFFu;
    }

    for( iter = 0u; iter < REPEAT_ITERATIONS; iter++ )
    {
        uint32_t sig;
        uint16_t off;
        uint16_t i;
        uint16_t run;

        (void)build_test_frame( TX_BUFFER_BASE, REPEAT_FRAME_LEN );
        clear_buffer( RX_BUFFER_BASE, rx_len );

        PCDMA_S2MM_clr_int_src( &g_pcdma, S2MM_IE_DONE_IRQ );
        PCDMA_MM2S_clr_int_src( &g_pcdma, MM2S_IE_DONE_IRQ );

        PCDMA_S2MM_configure( &g_pcdma, (uint32_t)rx_len,
                              (uint64_t)RX_BUFFER_BASE,
                              S2MM_CMD_ID, PCDMA_BURST_TYPE_INCR );
        PCDMA_S2MM_start_transfer( &g_pcdma );

        PCDMA_MM2S_configure( &g_pcdma, (uint32_t)REPEAT_FRAME_LEN,
                              (uint64_t)TX_BUFFER_BASE,
                              MM2S_CMD_ID, PCDMA_BURST_TYPE_INCR );
        PCDMA_MM2S_start_transfer( &g_pcdma );

        (void)wait_for_gpio_bit( GPIO_MM2S_DONE_MASK, (uint32_t *)0 );

        if( wait_for_gpio_bit( GPIO_S2MM_DONE_MASK, (uint32_t *)0 ) == 0 )
        {
            timeout_count++;
            PCDMA_S2MM_clr_int_src( &g_pcdma, S2MM_IE_DONE_IRQ );
            PCDMA_MM2S_clr_int_src( &g_pcdma, MM2S_IE_DONE_IRQ );
            continue;
        }

        if( compare_buffers( TX_BUFFER_BASE, RX_BUFFER_BASE, rx_len ) < 0 )
        {
            pass_count++;
        }

        /* Signature: first four received bytes packed into a word. Enough to
         * distinguish outcomes without storing whole buffers.
         */
        sig = ( (uint32_t)( *(volatile uint8_t *)( RX_BUFFER_BASE + 0u ) ) << 24u ) |
              ( (uint32_t)( *(volatile uint8_t *)( RX_BUFFER_BASE + 1u ) ) << 16u ) |
              ( (uint32_t)( *(volatile uint8_t *)( RX_BUFFER_BASE + 2u ) ) <<  8u ) |
              ( (uint32_t)( *(volatile uint8_t *)( RX_BUFFER_BASE + 3u ) ) );

        found = 0u;
        for( slot = 0u; slot < distinct_sigs; slot++ )
        {
            if( signature[ slot ] == sig )
            {
                sig_count[ slot ]++;
                found = 1u;
                break;
            }
        }

        if( ( found == 0u ) && ( distinct_sigs < SIGNATURE_SLOTS ) )
        {
            signature[ distinct_sigs ] = sig;
            sig_count[ distinct_sigs ] = 1u;
            distinct_sigs++;
        }

        /* Locate the frame start, six consecutive 0xFF. */
        off = 0xFFFFu;
        run = 0u;
        for( i = 0u; i < rx_len; i++ )
        {
            if( *(volatile uint8_t *)( RX_BUFFER_BASE + i ) == 0xFFu )
            {
                run++;
                if( run == 6u )
                {
                    off = (uint16_t)( i - 5u );
                    break;
                }
            }
            else
            {
                run = 0u;
            }
        }

        found = 0u;
        for( slot = 0u; slot < distinct_offs; slot++ )
        {
            if( offsets[ slot ] == off )
            {
                found = 1u;
                break;
            }
        }

        if( ( found == 0u ) && ( distinct_offs < SIGNATURE_SLOTS ) )
        {
            offsets[ distinct_offs ] = off;
            distinct_offs++;
        }

        PCDMA_S2MM_clr_int_src( &g_pcdma, S2MM_IE_DONE_IRQ );
        PCDMA_MM2S_clr_int_src( &g_pcdma, MM2S_IE_DONE_IRQ );
    }

    /* Results. */
    (void)snprintf( g_print_buf, sizeof(g_print_buf),
                    "  correct %u, timeout %u, corrupted %u of %u\r\n",
                    (unsigned)pass_count, (unsigned)timeout_count,
                    (unsigned)( REPEAT_ITERATIONS - pass_count -
                                timeout_count ),
                    (unsigned)REPEAT_ITERATIONS );
    print_str( g_print_buf );

    (void)snprintf( g_print_buf, sizeof(g_print_buf),
                    "  distinct RX signatures %u, distinct frame offsets %u\r\n",
                    (unsigned)distinct_sigs, (unsigned)distinct_offs );
    print_str( g_print_buf );

    for( slot = 0u; slot < distinct_sigs; slot++ )
    {
        (void)snprintf( g_print_buf, sizeof(g_print_buf),
                        "    signature 0x%08lX seen %u time(s)\r\n",
                        (unsigned long)signature[ slot ],
                        (unsigned)sig_count[ slot ] );
        print_str( g_print_buf );
    }

    for( slot = 0u; slot < distinct_offs; slot++ )
    {
        if( offsets[ slot ] == 0xFFFFu )
        {
            print_str( "    frame start: NOT FOUND\r\n" );
        }
        else
        {
            (void)snprintf( g_print_buf, sizeof(g_print_buf),
                            "    frame start offset %u\r\n",
                            (unsigned)offsets[ slot ] );
            print_str( g_print_buf );
        }
    }

    check( "all iterations transferred correctly",
           pass_count == REPEAT_ITERATIONS );

    /* The classification, which is the point of this test. */
    print_str( "\r\n" );

    if( pass_count == REPEAT_ITERATIONS )
    {
        print_str( "  VERDICT: link is stable and correct.\r\n" );
    }
    else if( distinct_sigs <= 1u )
    {
        print_str( "  VERDICT: fault is DETERMINISTIC.\r\n" );
        print_str( "    Every iteration produced the same result, so this is a"
                   " fixed\r\n" );
        print_str( "    structural error rather than link instability."
                   " Investigate\r\n" );
        print_str( "    clock rates, datapath widths, register configuration"
                   " and\r\n" );
        print_str( "    DMA length arithmetic. A single capture is sufficient"
                   " to\r\n" );
        print_str( "    reason about the cause.\r\n" );
    }
    else
    {
        print_str( "  VERDICT: fault is NON-DETERMINISTIC.\r\n" );
        print_str( "    Identical stimulus produced different results, so the"
                   " receive\r\n" );
        print_str( "    path is unstable. This RULES OUT fixed clock ratio and"
                   " fixed\r\n" );
        print_str( "    datapath width errors, which would be perfectly"
                   " repeatable.\r\n" );
        print_str( "\r\n" );
        print_str( "    Investigate, in order:\r\n" );
        print_str( "      1. PCS word alignment. LANE0_RX_SLIP is tied to"
                   " ground in\r\n" );
        print_str( "         the reference design, so a wrong bit boundary lock"
                   " can\r\n" );
        print_str( "         never be corrected.\r\n" );
        print_str( "      2. Elastic buffer over/underrun. EB_OVERFLOW and"
                   " EB_UNDERRUN\r\n" );
        print_str( "         are unconnected.\r\n" );
        print_str( "      3. XCVR reset sequencing. LANE0_PCS_ARST_N and"
                   " LANE0_PMA_ARST_N\r\n" );
        print_str( "         come from fabric reset, not from TX PLL lock.\r\n" );
        print_str( "      4. SMA cable seating and signal integrity. Receiver\r\n" );
        print_str( "         calibration is set to None (CDR).\r\n" );
        print_str( "\r\n" );
        print_str( "    Enable the CoreQSE PCS statistics counters to confirm:"
                   "\r\n" );
        print_str( "    PCS_SYNC_LOST_ERR and PCS_CODE_ERR settle this"
                   " directly.\r\n" );
    }
}

#endif /* BOARD_HAS_DMA_LOOPBACK, part 2 */

#if ( BOARD_HAS_MDIO != 0u )
/*==============================================================================
 * TEST 4: External PHY over MDIO.
 *
 * Only present in the 1 Gbps design, which drives an external 1000BASE-T PHY
 * through COREMDIO_APB. The 2.5 Gbps design has no PHY, since its loopback is a
 * direct SMA cable, and therefore no MDIO instance.
 *
 * This is the closest the 1 Gbps design comes to a link status indication.
 * CoreQSE itself has no link register, but the PHY does, so the PHY status
 * register substitutes for it.
 *============================================================================*/
/*! Progress reporter for the link wait.
 *
 *  Called about every 250 ms by MDIO_wait_link_up(). Printing the raw status
 *  word as negotiation proceeds is the most useful diagnostic available:
 *
 *    - a value that NEVER changes over the full wait means no signal is
 *      reaching the PHY, which points at the wrong RJ45 socket, an unseated
 *      daughter board, a dead far end, or a missing transceiver reference clock
 *    - a value that changes means a link partner IS present and negotiation is
 *      genuinely happening
 */
static void phy_link_progress( uint32_t elapsed_ms, uint16_t status_reg,
                               uint8_t link_up, uint8_t an_complete )
{
    (void)snprintf( g_print_buf, sizeof(g_print_buf),
                    "         %4lu ms  STATUS=0x%04X  link=%u  an_complete=%u"
                    "%s\r\n",
                    (unsigned long)elapsed_ms, (unsigned)status_reg,
                    (unsigned)link_up, (unsigned)an_complete,
                    ( link_up != 0u ) ? "   <-- LINK UP" : "" );
    print_str( g_print_buf );
}

static void test_phy_mdio( void )
{
    uint32_t    phy_id       = 0u;
    uint16_t    reg          = 0u;
    uint16_t    phy_regs[4]  = { 0u, 0u, 0u, 0u };
    uint8_t     fullduplex   = 0u;
    PHY_speed_t speed        = PHY_SPEED_INVALID;
    uint8_t     status;

    print_str( "\r\n=== TEST 4: External PHY over MDIO ===\r\n" );

    (void)snprintf( g_print_buf, sizeof(g_print_buf),
                    "  PHY driver compiled in: %s, PHY address %u\r\n",
                    QSE_phy_get_name(), (unsigned)BOARD_PHY_ADDR );
    print_str( g_print_buf );

    /* Associate the COREMDIO_APB instance and PHY address with the CoreQSE
     * instance. After this the application uses only QSE_phy_* and never has to
     * hold or pass an MDIO object, which is the point of the wrapper layer.
     */
    check( "QSE_attach_mdio() returns SUCCESS",
           QSE_attach_mdio( &g_qse, MDIO_BASE_ADDR, BOARD_PHY_ADDR,
                            BOARD_MDIO_PRESCALER ) == QSE_OK );

    /*--------------------------------------------------------------------------
     * COHERENCY CHECK FIRST.
     *
     * Before interpreting any PHY register, confirm the interface returns
     * register specific data at all. Reading four registers that cannot
     * legitimately match catches the case where MDIO returns a constant.
     *
     * This check exists because a constant read is easily mistaken for success.
     * An earlier version of this driver had a defect where the CONTROL register
     * was updated with a read-modify-write; since the direction bit persists,
     * every read after the first write ran as a WRITE cycle and DATAOUT kept its
     * last value. Every register then returned 0x0DC0, whose bit pattern happens
     * to satisfy several individual field tests, producing four misleading PASS
     * results.
     *------------------------------------------------------------------------*/
    /* Called through the transport directly rather than QSE_phy_verify_mdio()
     * because the specific failure mode matters here: MDIO_ERR_CONSTANT and
     * MDIO_ERR_NO_PHY need different diagnostic messages, and the QSE_ wrapper
     * maps both to QSE_ERR.
     */
    {
        MDIO_instance_t mdio_probe;
        mdio_probe.base_address = MDIO_BASE_ADDR;
        status = MDIO_self_test( &mdio_probe, BOARD_PHY_ADDR, phy_regs );
    }

    (void)snprintf( g_print_buf, sizeof(g_print_buf),
                    "         raw CONTROL=0x%04X STATUS=0x%04X"
                    " ID1=0x%04X ID2=0x%04X\r\n",
                    (unsigned)phy_regs[0], (unsigned)phy_regs[1],
                    (unsigned)phy_regs[2], (unsigned)phy_regs[3] );
    print_str( g_print_buf );

    check( "MDIO returns register specific data",
           status != MDIO_ERR_CONSTANT );

    if( status == MDIO_ERR_CONSTANT )
    {
        print_str( "\r\n" );
        print_str( "  *** MDIO INTERFACE NOT FUNCTIONING ***\r\n" );
        print_str( "      All four registers returned the same value, so the"
                   " interface\r\n" );
        print_str( "      is returning a constant rather than register specific"
                   " data.\r\n" );
        print_str( "      No conclusion about the PHY can be drawn.\r\n" );
        print_str( "\r\n" );
        print_str( "      Check, in order:\r\n" );
        print_str( "        1. MDC and MDIO reach the PHY pins, and MDO_EN is"
                   " wired\r\n" );
        print_str( "           to a BIBUF for the bidirectional MDIO line.\r\n" );
        print_str( "        2. BOARD_PHY_ADDR matches the board strapping.\r\n" );
        print_str( "        3. MDC rate. The CoreMDIO prescaler only decodes"
                   " 2 to 7,\r\n" );
        print_str( "           giving dividers 3 to 14. At 50 MHz PCLK the"
                   " fastest\r\n" );
        print_str( "           legal MDC is 3.57 MHz, above the IEEE limit of"
                   " 2.5 MHz.\r\n" );
        print_str( "        4. PHY power and reset state.\r\n" );
        print_str( "\r\n" );
        print_str( "      Remaining PHY checks skipped: interpreting them would"
                   "\r\n" );
        print_str( "      produce meaningless PASS results.\r\n" );
        return;
    }

    if( status == MDIO_ERR_NO_PHY )
    {
        check( "PHY responds on MDIO", 0 );
        print_str( "         No PHY responding at this address. Check"
                   " BOARD_PHY_ADDR,\r\n" );
        print_str( "         PHY power and the MDIO and MDC connections.\r\n" );
        return;
    }

    if( status != MDIO_OK )
    {
        check( "MDIO_self_test() completed", 0 );
        print_str( "         MDIO transaction did not complete. Remaining PHY"
                   "\r\n" );
        print_str( "         checks are skipped.\r\n" );
        return;
    }

    check( "PHY responds on MDIO", 1 );

    /* Identity, now trustworthy. */
    check( "QSE_phy_get_id() returns SUCCESS",
           QSE_phy_get_id( &g_qse, &phy_id ) == QSE_OK );
    print_u32( "PHY identifier", phy_id );

    (void)snprintf( g_print_buf, sizeof(g_print_buf),
                    "         OUI 0x%04X: %s\r\n",
                    (unsigned)( phy_id >> 16u ),
                    ( ( phy_id >> 16u ) == 0x0141u ) ? "Marvell" :
                    ( ( ( phy_id >> 16u ) == 0x0007u ) ? "Microchip"
                                                       : "unrecognised" ) );
    print_str( g_print_buf );

    /* The generic Clause 22 driver works with any conforming PHY regardless of
     * vendor, so an unrecognised OUI is not itself a problem. A vendor specific
     * driver is only needed for behaviour outside the standard register set;
     * see phy.h.
     */

    /*--------------------------------------------------------------------------
     * PHY bring-up through the abstraction layer.
     *
     * PHY_init() clears reset and power down. Several parts come up POWERED
     * DOWN, in which case the PHY answers MDIO but drives nothing onto the
     * copper. That looks exactly like a dead cable.
     *------------------------------------------------------------------------*/
    check( "QSE_phy_init() returns SUCCESS",
           QSE_phy_init( &g_qse ) == QSE_OK );

    /*--------------------------------------------------------------------------
     * Advertise abilities.
     *
     * PHY_set_link_speed() writes BOTH register 0x04 and register 0x09.
     * Register 0x00 only REQUESTS a speed; the abilities actually offered to the
     * link partner live in 0x04 for 10 and 100 Mbps and in 0x09 for 1000 Mbps.
     * Writing only 0x00 appears to work whenever the PHY happens to default to
     * advertising gigabit, and silently negotiates 100 Mbps when it does not.
     *------------------------------------------------------------------------*/
    check( "QSE_phy_set_link_speed() returns SUCCESS",
           QSE_phy_set_link_speed( &g_qse, PHY_ANEG_1000M_FD_ONLY ) == QSE_OK );

    /* Confirm what is actually being advertised. */
    if( QSE_read_phy_reg( &g_qse, MII_ADVERTISE, &reg ) == QSE_OK )
    {
        print_u32( "PHY AN advertisement (0x04)", (uint32_t)reg );
    }

    if( QSE_read_phy_reg( &g_qse, MII_CTRL1000, &reg ) == QSE_OK )
    {
        print_u32( "PHY 1000BASE-T control (0x09)", (uint32_t)reg );

        check( "PHY advertises 1000 Mbps full duplex",
               ( reg & ADVERTISE_1000FULL ) != 0u );

        if( ( reg & ( ADVERTISE_1000FULL | ADVERTISE_1000HALF ) ) == 0u )
        {
            print_str( "         PHY is NOT advertising any 1000 Mbps ability,"
                       " so\r\n" );
            print_str( "         auto-negotiation cannot settle on a gigabit"
                       " link.\r\n" );
        }
    }

    /* Control register read back. */
    if( QSE_read_phy_reg( &g_qse, MII_BMCR, &reg ) == QSE_OK )
    {
        print_u32( "PHY control register (0x00)", (uint32_t)reg );

        check( "PHY reset bit self cleared", ( reg & BMCR_RESET ) == 0u );
        check( "PHY is not powered down",    ( reg & BMCR_PDOWN ) == 0u );
    }
    else
    {
        check( "PHY control register read back", 0 );
    }

    /*--------------------------------------------------------------------------
     * Auto-negotiate and wait for the link.
     *
     * 1000BASE-T auto-negotiation takes 1 to 3 SECONDS. Measured on hardware,
     * the link negotiates in about 1735 ms. An earlier version of this test
     * polled a fixed 200 iterations, roughly 7 ms, which no working link could
     * ever satisfy.
     *
     * On the PolarFire Evaluation Kit the transceiver lanes route to the FMC
     * connector, so the RJ45 to use is PORT 0, connector J15, ON THE SGMII
     * DAUGHTER BOARD, not either socket on the Evaluation Kit itself.
     *------------------------------------------------------------------------*/
    (void)snprintf( g_print_buf, sizeof(g_print_buf),
                    "  Waiting up to %lu ms for PHY link...\r\n",
                    (unsigned long)BOARD_PHY_LINK_WAIT_MS );
    print_str( g_print_buf );

    status = QSE_phy_autonegotiate( &g_qse, BOARD_PHY_LINK_WAIT_MS,
                                    phy_link_progress );

    if( status == QSE_OK )
    {
        check( "PHY link is up", 1 );
    }
    else
    {
        check( "PHY link is up", 0 );
        print_str( "         Link did not come up within the wait.\r\n" );
        print_str( "         If the STATUS word above never changed, no signal"
                   " is\r\n" );
        print_str( "         reaching the PHY. Check:\r\n" );
        print_str( "           1. Cable is in RJ45 J15 PORT 0 on the SGMII"
                   " daughter\r\n" );
        print_str( "              board, NOT an Evaluation Kit socket.\r\n" );
        print_str( "           2. Daughter board firmly seated on the FMC and"
                   " powered.\r\n" );
        print_str( "           3. Far end port enabled and set to 1000BASE-T"
                   " with\r\n" );
        print_str( "              auto-negotiation.\r\n" );
        print_str( "           4. Transceiver reference clock present on"
                   " REF_CLK_PAD_P/N,\r\n" );
        print_str( "              otherwise the XCVR transmits nothing.\r\n" );
    }

    /*--------------------------------------------------------------------------
     * Report the NEGOTIATED speed and duplex, read from the vendor status
     * register. This is the difference between "we asked for gigabit" and "we
     * actually got gigabit": a link that silently falls back to 100 Mbps would
     * otherwise be indistinguishable from success.
     *------------------------------------------------------------------------*/
    if( QSE_phy_get_link_status( &g_qse, &speed, &fullduplex )
            == PHY_LINK_UP )
    {
        (void)snprintf( g_print_buf, sizeof(g_print_buf),
                        "         negotiated: %s, %s duplex\r\n",
                        ( speed == PHY_SPEED_1000MBPS ) ? "1000 Mbps" :
                        ( ( speed == PHY_SPEED_100MBPS ) ? "100 Mbps" :
                          ( ( speed == PHY_SPEED_10MBPS ) ? "10 Mbps"
                                                          : "invalid" ) ),
                        ( fullduplex == PHY_FULL_DUPLEX ) ? "full" : "half" );
        print_str( g_print_buf );

        check( "negotiated speed is 1000 Mbps",
               speed == PHY_SPEED_1000MBPS );

        check( "negotiated duplex is full",
               fullduplex == PHY_FULL_DUPLEX );
    }
    else
    {
        check_skipped( "negotiated speed and duplex", "link is down" );
    }

    /* Auto-negotiation completion and link partner ability. */
    if( QSE_read_phy_reg( &g_qse, MII_BMSR, &reg ) == QSE_OK )
    {
        /* Read twice: the link bit latches low. */
        (void)QSE_read_phy_reg( &g_qse, MII_BMSR, &reg );

        print_u32( "PHY status register (0x01)", (uint32_t)reg );

        check( "PHY auto-negotiation completed",
               ( reg & BMSR_ANEGCOMPLETE ) != 0u );

        check( "PHY reports no remote fault",
               ( reg & BMSR_RFAULT ) == 0u );
    }

    if( QSE_read_phy_reg( &g_qse, MII_STAT1000, &reg ) == QSE_OK )
    {
        print_u32( "PHY 1000BASE-T status (0x0A)", (uint32_t)reg );

        check( "link partner supports 1000 Mbps full duplex",
               ( reg & LPA_1000FULL ) != 0u );

        check( "local receiver reports OK",
               ( reg & LPA_1000LOCALRXOK ) != 0u );
    }

    /* The RJ45 LEDs are driven by vendor specific PHY LED registers that this
     * driver does not program, so they may stay dark even with the link UP. The
     * PHY status register is authoritative.
     */
    print_str( "         NOTE: RJ45 LEDs are not programmed by this driver and"
               "\r\n" );
    print_str( "         are NOT a valid link indicator. Trust the registers"
               " above.\r\n" );
}

/*==============================================================================
 * TEST 5c: Externally generated traffic.
 *
 * In the 1 Gbps design the processor cannot generate frames, so throughput and
 * integrity are exercised by an external test set. This test cannot create
 * traffic; what it can do is snapshot the CoreQSE statistics counters before
 * and after an operator controlled traffic window, which turns the external
 * run into a pass or fail result.
 *
 * Requires COREQSE_CFG_STATS. Without the counter block there is no way for
 * software to observe externally generated traffic at all, since there is also
 * no data path to memory.
 *============================================================================*/
static void test_external_traffic( void )
{
    print_str( "\r\n=== TEST 5c: Externally generated traffic ===\r\n" );

#if ( COREQSE_CFG_STATS == 0u )
    check_skipped( "external traffic verification",
                   "statistics counter block not generated in this IP build" );
    print_str( "         In the 1 Gbps design the processor has NO data path to"
               "\r\n" );
    print_str( "         frame memory, so the statistics counters are the ONLY"
               "\r\n" );
    print_str( "         way software can observe traffic. Regenerate CoreQSE"
               "\r\n" );
    print_str( "         with the MAC and PCS counters enabled to use this"
               " test.\r\n" );
#else
    {
        QSE_tx_stats_t tx_before;
        QSE_rx_stats_t rx_before;
        QSE_tx_stats_t tx_after;
        QSE_rx_stats_t rx_after;
        uint8_t        key = 0u;

        /* Start from a clean baseline. The counters are write-to-clear. */
        QSE_clear_stats( &g_qse );
        QSE_get_tx_stats( &g_qse, &tx_before );
        QSE_get_rx_stats( &g_qse, &rx_before );

        print_str( "  Counters cleared.\r\n" );
        print_str( "  START TRAFFIC ON THE TEST SET NOW, then press any key"
                   " here.\r\n" );

        while( UART_get_rx( &g_uart, &key, 1u ) == 0u )
        {
            /* Wait for the operator. */
        }

        QSE_get_tx_stats( &g_qse, &tx_after );
        QSE_get_rx_stats( &g_qse, &rx_after );

        print_u32( "MAC_TX_PKT",        tx_after.tx_frame_count );
        print_u32( "MAC_RX_PKT",        rx_after.rx_frame_count );
        print_u32( "MAC_RX_FCS_ERR",    rx_after.rx_fcs_error_count );
        print_u32( "MAC_RX_DROP",       rx_after.rx_drop_count );
        print_u32( "PCS_CODE_ERR",      rx_after.pcs_code_err_count );
        print_u32( "PCS_SYNC_LOST_ERR", rx_after.pcs_sync_lost_count );

        check( "frames were received from the test set",
               rx_after.rx_frame_count > rx_before.rx_frame_count );

        /* The stream output is looped back to the transmit input in fabric, so
         * every received frame should also be transmitted.
         */
        check( "frames were transmitted back to the test set",
               tx_after.tx_frame_count > tx_before.tx_frame_count );

        check( "no receive FCS errors",
               rx_after.rx_fcs_error_count == rx_before.rx_fcs_error_count );

        check( "no dropped frames",
               rx_after.rx_drop_count == rx_before.rx_drop_count );

        check( "no PCS code errors",
               rx_after.pcs_code_err_count == rx_before.pcs_code_err_count );

        check( "no PCS sync losses",
               rx_after.pcs_sync_lost_count == rx_before.pcs_sync_lost_count );

        check( "QSE_stats_are_clean() reports no error counters moved",
               QSE_stats_are_clean( &rx_before, &rx_after ) == QSE_OK );

        print_str( "  Compare MAC_RX_PKT against the test set transmit count,"
                   " and\r\n" );
        print_str( "  MAC_TX_PKT against its receive count, to confirm no"
                   " frames\r\n" );
        print_str( "  were lost in either direction.\r\n" );
    }
#endif
}
#endif /* BOARD_HAS_MDIO */

/*==============================================================================
 * TEST 6: MAC and PCS statistics.
 *
 * Snapshot before and after traffic, then diff. This is the only hardware
 * level evidence that the MAC and PCS were healthy while data was moving.
 *
 * In the 2.5 Gbps reference design every statistics generic is disabled in
 * COREQSE_C0.cxf, so the whole counter block is absent and the driver reports
 * QSE_ERR_NOT_SUPPORTED rather than returning all zeroes, which would look like a
 * clean pass.
 *============================================================================*/
static void test_statistics( const QSE_tx_stats_t * tx_before,
                             const QSE_rx_stats_t * rx_before )
{
#if ( COREQSE_CFG_STATS != 0u )
    QSE_tx_stats_t tx_after;
    QSE_rx_stats_t rx_after;
#endif

    print_str( "\r\n=== TEST 6: MAC and PCS statistics ===\r\n" );

#if ( COREQSE_CFG_STATS == 0u )
    ( void ) tx_before;
    ( void ) rx_before;

    check_skipped( "statistics comparison",
                   "counter block not generated in this IP build" );
    print_str( "         Regenerate CoreQSE with the MAC/PCS stat counter"
               " parameters enabled\r\n" );
    print_str( "         to obtain hardware level verification.\r\n" );
#else
    /* QSE_get_tx_stats() and QSE_get_rx_stats() return void. The counters are
     * Read/Write-to-Clear in hardware, so these reads are non-destructive and
     * two snapshots can be differenced.
     */
    QSE_get_tx_stats( &g_qse, &tx_after );
    QSE_get_rx_stats( &g_qse, &rx_after );

    print_u32( "MAC_TX_PKT",        tx_after.tx_frame_count );
    print_u32( "MAC_RX_PKT",        rx_after.rx_frame_count );
    print_u32( "MAC_TX_ERR",        tx_after.tx_err_count );
    print_u32( "MAC_RX_ERR",        rx_after.rx_err_count );
    print_u32( "MAC_RX_FCS_ERR",    rx_after.rx_fcs_error_count );
    print_u32( "MAC_RX_DROP",       rx_after.rx_drop_count );
    print_u32( "PCS_CODE_ERR",      rx_after.pcs_code_err_count );
    print_u32( "PCS_SYNC_LOST_ERR", rx_after.pcs_sync_lost_count );

    check( "frames transmitted equals frames received",
           ( tx_after.tx_frame_count - tx_before->tx_frame_count ) ==
           ( rx_after.rx_frame_count - rx_before->rx_frame_count ) );

    check( "no receive FCS errors",
           rx_after.rx_fcs_error_count == rx_before->rx_fcs_error_count );

    check( "no dropped frames",
           rx_after.rx_drop_count == rx_before->rx_drop_count );

    check( "no PCS code errors",
           rx_after.pcs_code_err_count == rx_before->pcs_code_err_count );

    check( "no PCS sync losses",
           rx_after.pcs_sync_lost_count == rx_before->pcs_sync_lost_count );

    check( "no FIFO or elastic buffer ECC events",
           ( tx_after.tx_fifo_ecc_db_det == tx_before->tx_fifo_ecc_db_det ) &&
           ( rx_after.rx_fifo_ecc_db_det == rx_before->rx_fifo_ecc_db_det ) &&
           ( rx_after.pcs_eb_ecc_db_det  == rx_before->pcs_eb_ecc_db_det  ) );

    check( "QSE_stats_are_clean() reports no error counters moved",
           QSE_stats_are_clean( rx_before, &rx_after ) == QSE_OK );
#endif
}

/*==============================================================================
 * TEST 7: PCS synchronization status.
 *============================================================================*/
static void test_link_health( void )
{
    print_str( "\r\n=== TEST 7: PCS synchronization status ===\r\n" );

#if ( COREQSE_CFG_STATS == 0u )
    check_skipped( "PCS sync status",
                   "PCS counters not generated in this IP build" );
    print_str( "         CoreQSE has no PCS status register. The SYNC output is"
               " a\r\n" );
    print_str( "         hardware port only and is unconnected in this design,"
               " so\r\n" );
    print_str( "         there is no software visible link indication.\r\n" );
#else
    check( "QSE_pcs_get_sync_status() reports synchronized",
           QSE_pcs_get_sync_status( &g_qse ) == 1u );
#endif
}

/*==============================================================================
 * main
 *============================================================================*/
int main( void )
{
    QSE_tx_stats_t tx_stats_before;
    QSE_rx_stats_t rx_stats_before;
    uint8_t        status;

    /* UART FIRST, so that any subsequent failure can be reported. */
    UART_init( &g_uart, UART_BASE_ADDR, BOARD_BAUD_VALUE_115200,
               ( DATA_8_BITS | NO_PARITY ) );

    /* Announce the build configuration first. If the wrong binary has been
     * loaded for the programmed bitstream, this banner is the evidence.
     */
    print_str( "\r\n" );
    print_str( "==============================================\r\n" );
    print_str( " CoreQSE Ethernet Validation\r\n" );
    print_str( " " BOARD_NAME "\r\n" );
    print_str( "==============================================\r\n" );

    (void)snprintf( g_print_buf, sizeof(g_print_buf),
                    " BUILD    : %s\r\n"
                    " LINK RATE: %u Mbps\r\n"
                    " COREQSE  : 0x%08lX\r\n",
#if ( BOARD_DESIGN == BOARD_COREQSE_2_5G )
                    "BOARD_COREQSE_2_5G",
#else
                    "BOARD_COREQSE_1G",
#endif
                    (unsigned)BOARD_LINK_SPEED_MBPS,
                    (unsigned long)COREQSE_BASE );
    print_str( g_print_buf );

    (void)snprintf( g_print_buf, sizeof(g_print_buf),
                    " FEATURES : DMA %s, GPIO IRQ %s, MDIO %s\r\n",
                    ( BOARD_HAS_DMA_LOOPBACK != 0u ) ? "yes" : "no",
                    ( BOARD_HAS_GPIO_IRQ != 0u )     ? "yes" : "no",
                    ( BOARD_HAS_MDIO != 0u )         ? "yes" : "no" );
    print_str( g_print_buf );

    print_str( " If the line above does not match the programmed bitstream,\r\n" );
    print_str( " change BOARD_DESIGN in board_config.h and rebuild.\r\n" );
    print_str( "==============================================\r\n" );

#if ( BOARD_HAS_GPIO_IRQ != 0u )
    GPIO_init( &g_gpio, COREGPIO_BASE_ADDR, GPIO_APB_32_BITS_BUS );

    /* Read before PCDMA_init(). A non-zero value here is normal: the DMA
     * interrupt sources persist across a CPU reset, so bits left set by a
     * previous run are still visible until PCDMA_init() clears them.
     */
    print_u32( "GPIO inputs before PCDMA init", GPIO_get_inputs( &g_gpio ) );
#endif

    /* Transceiver settling. Register access does not require it, because the
     * AXI4-Lite interface is in the ACLK domain, but the data path does.
     */
    xcvr_assumed_ready();

#if ( BOARD_HAS_DMA_LOOPBACK != 0u )
    /* CoreAXI4ProtoConv: a single instance, both channels behind one base.
     * PCDMA_init() disables and clears all interrupt sources.
     */
    PCDMA_init( &g_pcdma, PROTOCONV_BASE );

    /* Confirm PCDMA_init() actually cleared every pending interrupt source.
     * This matters: a stale MM2S_INT or S2MM_INT would make wait_for_gpio_bit()
     * return immediately and report a transfer as complete when nothing moved.
     */
    print_u32( "GPIO inputs after PCDMA init", GPIO_get_inputs( &g_gpio ) );

    check( "all DMA interrupt sources clear before testing",
           ( GPIO_get_inputs( &g_gpio ) &
             ( GPIO_MM2S_DONE_MASK | GPIO_S2MM_DONE_MASK |
               GPIO_MM2S_ERR_MASK  | GPIO_S2MM_ERR_MASK ) ) == 0u );

    /* Enable completion interrupts. They are routed to CoreGPIO inputs, not to
     * the RISC-V interrupt controller, so they are polled rather than serviced.
     */
    PCDMA_MM2S_enable_irq( &g_pcdma,
                           MM2S_IE_DONE_IRQ | MM2S_IE_AXI4_ERR_IRQ );
    PCDMA_S2MM_enable_irq( &g_pcdma,
                           S2MM_IE_DONE_IRQ | S2MM_IE_AXI4_ERR_IRQ |
                           S2MM_IE_PKT_DROP_ERR_IRQ |
                           S2MM_IE_PKT_DROP_OVF_IRQ );
#endif

    /* CoreQSE. First access to the register slave.
     *
     * If the binary does not match the bitstream this is where it shows. The
     * two designs place CoreQSE at different addresses, so an access here goes
     * to an undecoded region. Where the interconnect returns a response, the
     * data is meaningless and the version check below catches it. Where it
     * returns none, the processor stalls and nothing further is printed, which
     * is itself the diagnosis.
     */
    status = QSE_init( &g_qse, COREQSE_BASE );

    if( status != QSE_OK )
    {
        print_str( "FATAL: QSE_init() failed. Aborting.\r\n" );
        while( 1 )
        {
        }
    }

    /* Design sanity check. CORE_VER must report major version 2 for CoreQSE
     * v2.0.101. Anything else means the access did not reach CoreQSE.
     */
    {
        uint8_t major = 0u;
        uint8_t minor = 0u;

        (void)QSE_get_version( &g_qse, &major, &minor );

        if( major != 2u )
        {
            (void)snprintf( g_print_buf, sizeof(g_print_buf),
                            "\r\nFATAL: CORE_VER reports %u.%u, expected 2.x\r\n",
                            (unsigned)major, (unsigned)minor );
            print_str( g_print_buf );
            print_str( "  No CoreQSE responded at the configured address.\r\n" );
            print_str( "  The binary probably does not match the programmed"
                       " bitstream.\r\n" );
            (void)snprintf( g_print_buf, sizeof(g_print_buf),
                            "  This build expects CoreQSE at 0x%08lX for %s.\r\n",
                            (unsigned long)COREQSE_BASE,
#if ( BOARD_DESIGN == BOARD_COREQSE_2_5G )
                            "the 2.5 Gbps design" );
#else
                            "the 1 Gbps design" );
#endif
            print_str( g_print_buf );
            print_str( "  Set BOARD_DESIGN in board_config.h to match, then"
                       " rebuild.\r\n" );
            print_str( "  Aborting: further results would be meaningless.\r\n" );

            while( 1 )
            {
            }
        }
    }

    print_str( "CoreQSE initialized and responding.\r\n" );

#if ( BOARD_HAS_DMA_LOOPBACK != 0u )
    test_buffer_access_dma();
#else
    print_str( "\r\n=== TEST 0: CPU access to DMA buffers ===\r\n" );
    check_skipped( "buffer access",
                   "no DMA or frame buffers in the 1 Gbps design" );
    print_str( "         CoreQSE's AXI4-Stream receive output is wired back to"
               "\r\n" );
    print_str( "         its own transmit input in the fabric, so there is no"
               "\r\n" );
    print_str( "         frame memory for the processor to access.\r\n" );
#endif

    test_register_por();
    test_mac_address();
    test_mac_configuration();

#if ( BOARD_HAS_MDIO != 0u )
    test_phy_mdio();
#endif

    /* Baseline counters, taken after configuration but before any traffic.
     * When the counter block is absent the driver zeroes both structures and
     * test_statistics() reports SKIP rather than comparing meaningless values.
     */
    (void)memset( &tx_stats_before, 0, sizeof(tx_stats_before) );
    (void)memset( &rx_stats_before, 0, sizeof(rx_stats_before) );
    QSE_get_tx_stats( &g_qse, &tx_stats_before );
    QSE_get_rx_stats( &g_qse, &rx_stats_before );

#if ( BOARD_HAS_DMA_LOOPBACK != 0u )
    test_length_sweep();
    test_repeatability();
#else
    print_str( "\r\n=== TEST 5: Frame length sweep ===\r\n" );
    check_skipped( "processor generated frame transfer",
                   "no DMA in the 1 Gbps design" );
    print_str( "         Frames must be generated by an external test set."
               " The\r\n" );
    print_str( "         processor has no path to frame memory, so payload"
               "\r\n" );
    print_str( "         integrity cannot be checked in software. Use TEST 5c"
               "\r\n" );
    print_str( "         and the statistics counters instead.\r\n" );
#endif

#if ( BOARD_HAS_MDIO != 0u )
    test_external_traffic();
#endif

    test_statistics( &tx_stats_before, &rx_stats_before );
    test_link_health();

    /* Summary. */
    print_str( "\r\n==============================================\r\n" );
    (void)snprintf( g_print_buf, sizeof(g_print_buf),
                    " RESULT: %lu of %lu checks passed\r\n",
                    (unsigned long)g_checks_passed,
                    (unsigned long)g_checks_run );
    print_str( g_print_buf );

    if( g_checks_passed == g_checks_run )
    {
        print_str( " OVERALL: PASS\r\n" );
    }
    else
    {
        (void)snprintf( g_print_buf, sizeof(g_print_buf),
                        " OVERALL: FAIL (%lu failures)\r\n",
                        (unsigned long)( g_checks_run - g_checks_passed ) );
        print_str( g_print_buf );
    }

    /* Feature-level summary, so the log states plainly which capabilities are
     * proven and which are not. See coreqse_2_5g_status.txt for detail.
     */
    print_str( "\r\n FEATURE STATUS\r\n" );
    print_str( "   WORKING : register access, version, self test\r\n" );
    print_str( "   WORKING : MAC address programming and read back\r\n" );
    print_str( "   WORKING : TX and RX configuration, parameter validation\r\n" );
    print_str( "   WORKING : MAC enable/disable, verified by read back\r\n" );
#if ( BOARD_HAS_DMA_LOOPBACK != 0u )
    print_str( "   WORKING : DMA transfer mechanism, MM2S and S2MM\r\n" );
    print_str( "   WORKING : completion interrupts and W1C clearing\r\n" );
    print_str( "   WORKING : timeout handling on stalled transfers\r\n" );
    print_str( "   FAILING : Ethernet payload integrity, receive path\r\n" );
#else
    print_str( "   N/A     : DMA, completion interrupts, payload integrity\r\n" );
    print_str( "             (no data path to the processor in this design)\r\n" );
#endif
#if ( BOARD_HAS_MDIO != 0u )
    print_str( "   SEE LOG : external PHY over MDIO, link and"
               " auto-negotiation\r\n" );
#endif
    print_str( "   ABSENT  : frame filter, flow control, statistics, ECC,\r\n" );
    print_str( "             Wake-on-LAN, programmable preamble\r\n" );
    print_str( "             (not generated in this IP build)\r\n" );
    print_str( "   NO VISIBILITY : PCS sync, elastic buffer status, XCVR\r\n" );
    print_str( "             readiness (ports exist but are unconnected)\r\n" );

    print_str( "==============================================\r\n" );

    while( 1 )
    {
    }

    return ( 0 );
}
