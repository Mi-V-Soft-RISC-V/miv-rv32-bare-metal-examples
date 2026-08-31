/*******************************************************************************
 * Copyright 2026 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file board_config.h
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief Selects which CoreQSE reference design the application is built for.
 *
 * The 1 Gbps and 2.5 Gbps reference designs are NOT variants of one another.
 * They differ in topology, not just link rate, so the test application must be
 * built for one or the other.
 *
 *   Design                    2.5 Gbps                1 Gbps
 *   ------------------------  ----------------------  ----------------------
 *   Processor                 MIV_RV32_C1             MIV_RV32_C0
 *   Frame data movement       COREAXI4PROTOCONV       none
 *   Frame buffers             3x PF_SRAM_AHBL_AXI     none
 *   Completion interrupts     CoreGPIO, 4 bits        none
 *   PHY management            none                    COREMDIO_APB
 *   Interconnect              3 masters, 4 slaves     1 master, 1 slave
 *   XCVR serial rate          3125 Mbps, div 2        1250 Mbps, div 4
 *   Loopback                  external SMA cable      external PHY and RJ45
 *   Traffic source            the processor           Spirent Test Centre
 *
 * The consequence for software is fundamental: in the 1 Gbps design CoreQSE's
 * AXI4-Stream receive output is wired directly back to its own transmit input
 * in the fabric,
 *
 *     wire [31:0] COREQSE_C0_0_AXI4S_INITR_TDATA;
 *     .AXI4S_TX_TARG_TDATA  ( COREQSE_C0_0_AXI4S_INITR_TDATA ),
 *     .AXI4S_RX_INITR_TDATA ( COREQSE_C0_0_AXI4S_INITR_TDATA ),
 *
 * so there is no path by which the processor can stage or inspect a frame.
 * Traffic is generated and verified externally by a test set. The processor
 * configures the MAC and the PHY, and reads statistics.
 *
 * Select the target design below.
 */

#ifndef FPGA_DESIGN_CONFIG_H_
#define FPGA_DESIGN_CONFIG_H_

/*------------------------------------------------------------------------------
 * Supported designs.
 */
#define BOARD_COREQSE_2_5G          ( 1u )
#define BOARD_COREQSE_1G            ( 2u )

/*==============================================================================
 *
 *                  >>>  SELECT THE TARGET DESIGN HERE  <<<
 *
 * Set BOARD_DESIGN to BOARD_COREQSE_2_5G or BOARD_COREQSE_1G, then rebuild.
 *
 * Alternatively leave the line below commented out and pass the selection on
 * the compiler command line, which avoids editing the file when switching:
 *
 *     -DBOARD_DESIGN=BOARD_COREQSE_2_5G
 *     -DBOARD_DESIGN=BOARD_COREQSE_1G
 *
 * In SoftConsole this goes in
 *   Project Properties -> C/C++ Build -> Settings -> GNU RISC-V Cross C
 *   Compiler -> Preprocessor -> Defined symbols.
 *
 * CRITICAL: the binary MUST match the programmed bitstream. The two designs
 * place CoreQSE at DIFFERENT addresses:
 *
 *     2.5 Gbps design   CoreQSE at 0x60020000
 *     1 Gbps design     CoreQSE at 0x70000000
 *
 * Running the wrong binary accesses an address that is not decoded in that
 * design. Depending on the interconnect that either returns meaningless data
 * or produces no AXI response at all, in which case the processor stalls with
 * no possibility of recovery.
 *
 * The application prints its build configuration at startup and performs a
 * design sanity check on CORE_VER before running any test. That check catches
 * the case where the access RETURNS wrong data. It cannot catch the case where
 * the access never completes, because no software can. Confirm the bitstream
 * before loading the binary.
 *
 *============================================================================*/
#ifndef BOARD_DESIGN
#define BOARD_DESIGN                BOARD_COREQSE_2_5G // BOARD_COREQSE_2_5G BOARD_COREQSE_1G
#endif

/*==============================================================================
 *                        2.5 Gbps design
 *
 * Addresses from COREAXI4INTERCONNECT_C0 of Design/CoreQSE_2_5Gbps:
 *   SLAVE0 0x60000000 - 0x60007FFF   PF_SRAM_AHBL_AXI_C0, transmit buffer
 *   SLAVE1 0x60010000 - 0x6001FFFF   COREAXI4PROTOCONV
 *   SLAVE2 0x60020000 - 0x6002FFFF   COREQSE
 *   SLAVE3 0x60030000 - 0x60037FFF   PF_SRAM_AHBL_AXI_C1, receive buffer
 *============================================================================*/
#if ( BOARD_DESIGN == BOARD_COREQSE_2_5G )

#define BOARD_NAME                  "CoreQSE 2.5 Gbps, far end loopback via SMA"
#define BOARD_LINK_SPEED_MBPS       ( 2500u )

/*! The processor can generate and verify frames itself. */
#define BOARD_HAS_DMA_LOOPBACK      ( 1u )

/*! Completion interrupts are routed to CoreGPIO inputs. */
#define BOARD_HAS_GPIO_IRQ          ( 1u )

/*! No MDIO in this design: the SMA loopback has no PHY. */
#define BOARD_HAS_MDIO              ( 0u )

/*! No PHY exists, so neither VSC8575_PHY nor M88E1340_PHY is defined and
 *  null_phy.c provides stubs. This keeps the build linking without pulling in a
 *  PHY driver that has no hardware behind it.
 */
#undef  VSC8575_PHY
#undef  M88E1340_PHY

#define COREQSE_BASE_ADDR           ( 0x60020000u )
#define PROTOCONV_BASE_ADDR         ( 0x60010000u )
#define TX_BUFFER_BASE_ADDR         ( 0x60000000u )
#define RX_BUFFER_BASE_ADDR         ( 0x60030000u )
#define UART_BASE_ADDR              ( 0x71000000u )
#define COREGPIO_BASE_ADDR          ( 0x72000000u )

/*! MIV_RV32_C1 fabric clock, from PF_CCC_C0. */
#define BOARD_SYS_CLK_FREQ          ( 50000000UL )

/*==============================================================================
 *                          1 Gbps design
 *
 * Addresses confirmed against Application/CoreQSE_1G/application/main.c and
 * hw_platform.h of the reference application:
 *   COREQSE       0x70000000   (interconnect SLAVE0, 0x70000000 - 0x7FFFFFFF)
 *   COREMDIO_APB  0x60000000
 *   CoreUARTapb   0x61000000
 *============================================================================*/
#elif ( BOARD_DESIGN == BOARD_COREQSE_1G )

#define BOARD_NAME                  "CoreQSE 1 Gbps, external PHY and RJ45"
#define BOARD_LINK_SPEED_MBPS       ( 1000u )

/*! No DMA and no frame buffers. CoreQSE's stream output is looped back to its
 *  own input in the fabric, so the processor cannot stage or read a frame.
 *  Traffic is generated and verified by an external test set.
 */
#define BOARD_HAS_DMA_LOOPBACK      ( 0u )

/*! No CoreGPIO in this design. */
#define BOARD_HAS_GPIO_IRQ          ( 0u )

/*! COREMDIO_APB is present for external PHY management. */
#define BOARD_HAS_MDIO              ( 1u )

#define COREQSE_BASE_ADDR           ( 0x70000000u )
#define MDIO_BASE_ADDR              ( 0x60000000u )
#define UART_BASE_ADDR              ( 0x61000000u )

/*! MIV_RV32_C0 fabric clock, from hw_platform.h of the 1 Gbps application. */
#define BOARD_SYS_CLK_FREQ          ( 50000000UL )

/*------------------------------------------------------------------------------
 * External PHY address on the MDIO bus.
 *
 * The reference application uses Clause 22 access at PHY address 0.
 */
#define BOARD_PHY_ADDR              ( 0u )

/*! MDIO clock prescaler.
 *
 *  From the CoreMDIO_APB RTL, clk_div_reg is 3 bits and only values 2 to 7
 *  select a defined divider: 2->3, 3->4, 4->5, 5->7, 6->10, 7->14. Anything
 *  else falls through to the default of 2, which is the FASTEST setting.
 *
 *  MDC = PCLK / divider. IEEE 802.3 limits MDC to 2.5 MHz, so at a 50 MHz PCLK
 *  the divider would need to be at least 20. No legal value achieves that: 7
 *  gives the largest divider of 14, so MDC = 3.57 MHz, which is above the
 *  limit. Value 7 is therefore the closest available and is what the reference
 *  application uses. MDIO has been confirmed working on hardware at this
 *  setting.
 */
#define BOARD_MDIO_PRESCALER        ( 0x7u )

/*! Time allowed for the PHY link to come up, in milliseconds.
 *
 *  1000BASE-T auto-negotiation takes 1 to 3 seconds: IEEE 802.3 defines a
 *  break_link_timer of 1.2 to 1.5 s before the page exchange and master/slave
 *  resolution even begin. Measured on hardware, the link negotiates in about
 *  1735 ms, so 5000 ms gives comfortable margin.
 */
#define BOARD_PHY_LINK_WAIT_MS      ( 5000u )

/*==============================================================================
 *                        EXTERNAL PHY SELECTION
 *
 * This example ships ONE PHY implementation: null_phy.c, which uses only the
 * standardised IEEE 802.3 Clause 22 registers 0x00 to 0x0F. That is sufficient
 * to bring up a 1000BASE-T link on any conforming PHY, and is what was used to
 * validate the 1 Gbps design on hardware. Both macros below are therefore
 * undefined.
 *
 * The PHY fitted to the SGMII daughter board reports identifier 0x01410DC0, a
 * Marvell OUI. The generic Clause 22 sequence brings it up correctly: measured
 * on hardware, auto-negotiation completes in about 1735 ms at 1000 Mbps full
 * duplex.
 *
 * IF YOU NEED A VENDOR SPECIFIC DRIVER
 * ------------------------------------
 * Vendor drivers are needed only for behaviour outside the standard register
 * set: reading the resolved link speed and duplex directly, LED control, cable
 * diagnostics, or SERDES and MAC interface configuration.
 *
 * The CoreTSE driver ships tested implementations that can be adapted:
 *
 *   vsc8575.c         Microchip VSC8575
 *   m88e1340_phy.c    Marvell M88E1340
 *   m88e1111_phy.c    Marvell M88E1111
 *
 * Copy the file into the CoreQSE driver directory, adapt it as described in
 * phy.h, then define the matching macro here. null_phy.c guards itself out when
 * either macro is defined, so exactly one implementation is compiled.
 *============================================================================*/
#undef  M88E1340_PHY
#undef  VSC8575_PHY

#else
#error "BOARD_DESIGN must be BOARD_COREQSE_2_5G or BOARD_COREQSE_1G"
#endif

/*------------------------------------------------------------------------------
 * Derived UART baud divisor.
 */
#define BOARD_BAUD_VALUE_115200     \
    ( ( BOARD_SYS_CLK_FREQ / ( 16u * 115200u ) ) - 1u )

#endif /* FPGA_DESIGN_CONFIG_H_ */
