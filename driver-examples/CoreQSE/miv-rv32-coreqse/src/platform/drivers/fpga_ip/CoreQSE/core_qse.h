/*******************************************************************************
 * Copyright 2026 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file core_qse.h
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief CoreQSE bare metal driver public API.
 *
 * API names, types and constants follow coreqse-driver-user-guide.md.
 * Register details follow CoreQSE User Guide DS50004157A, register
 * specification 2.0.101.
 *
 *//*=========================================================================*/

/*=========================================================================*//**
  @mainpage CoreQSE Bare Metal Driver

  @section intro_sec Introduction

  CoreQSE is an IEEE 802.3-2022 compliant Ethernet MAC with integrated Physical
  Coding Sublayer (PCS), supporting the transceiver 20-bit PMA interface at
  1 Gbps and 2.5 Gbps. This driver provides functions for controlling CoreQSE in
  a bare metal system.

  @section datapath_sec CoreQSE has no CPU accessible data path

  Frame data enters and leaves CoreQSE over AXI4-Stream, which is not addressable
  by the processor. All frame movement must be performed by a companion DMA
  engine, typically CoreAXI4ProtoConv, using its own driver (PCDMA_*).

  This driver covers configuration, filtering, flow control and statistics only.
  It never touches frame data. See QSE_tx_frame() for the consequences.

  @section clock_sec Clocking

  CoreQSE has three clock domains:

    - ACLK        AXI4-Lite register interface. In the reference designs this is
                  the fabric clock, the same clock and reset as the processor,
                  so registers are accessible as soon as software runs.
    - SYS_CLK     System side AXI4-Stream data path.
    - XCVR_TX_CLK / XCVR_RX_CLK   Line side, rate locked to the transceiver.

  SYS_TX_RESETN is driven by the transceiver TX clock stable indication and
  SYS_RX_RESETN by the receiver valid indication. Configuration writes therefore
  succeed before the link is up, but no frame moves until those resets release.
  With external loopback the cable must be fitted for the receive path to leave
  reset.

  @section cfg_sec Optional hardware must be declared

  Frame filtering, hash filtering, flow control, programmable preamble,
  Wake-on-LAN and the entire statistics counter block are each independently
  enabled in the IP configurator. Reading a register belonging to an absent
  block returns 0 and writing it has no effect.

  coreqse_user_config.h declares which blocks are present. Functions guarding
  absent hardware return QSE_ERR_NOT_SUPPORTED rather than silently doing
  nothing. Keep that file in step with your COREQSE_C0.cxf.

  @section usage_sec Typical usage

  @code
  QSE_instance_t  g_qse;
  QSE_tx_config_t tx_cfg;
  QSE_rx_config_t rx_cfg;
  uint8_t         mac[6] = {0x00,0x04,0xA3,0x11,0x22,0x33};

  QSE_init(&g_qse, COREQSE_BASE_ADDR);
  QSE_set_mac_addr(&g_qse, mac);

  QSE_get_default_tx_cfg(&tx_cfg);
  QSE_get_default_rx_cfg(&rx_cfg);
  QSE_tx_cfg_set(&g_qse, &tx_cfg);
  QSE_rx_cfg_set(&g_qse, &rx_cfg);

  QSE_tx_enable(&g_qse);
  QSE_rx_enable(&g_qse);

  // Move frames with the CoreAXI4ProtoConv driver, not this one.
  @endcode
 *//*=========================================================================*/

#ifndef CORE_QSE_H_
#define CORE_QSE_H_

#include <stdint.h>
#include "coreqse_user_config.h"

#include "hal/hal.h"
#include "hal/hal_assert.h"

/* PHY abstraction and MDIO transport.
 *
 * CoreQSE has no MII management hardware of its own, so the QSE_phy_* functions
 * declared later in this file delegate to a separate COREMDIO_APB IP. These
 * includes bring in MDIO_instance_t, MDIO_link_progress_t and PHY_speed_t.
 *
 * phy.h itself includes core_mdio.h, so including it here is sufficient.
 */
#include "phy.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
 *                          Return status constants
 *============================================================================*/

#define QSE_OK                      ( 0u )  /*!< Operation successful.        */
#define QSE_ERR                     ( 1u )  /*!< General error condition.     */
#define QSE_ERR_PARAM               ( 2u )  /*!< Invalid parameter.           */
#define QSE_ERR_FIFO_FULL           ( 3u )  /*!< Transmit FIFO is full.       */

/*! Requested hardware block is absent from this IP build. Not in the driver
 *  user guide; added because most CoreQSE blocks are optional configurator
 *  parameters and silently ignoring a request would hide the condition.
 */
#define QSE_ERR_NOT_SUPPORTED       ( 4u )

/*==============================================================================
 *                        Receive filter mask constants
 *
 * Used with QSE_set_rx_filter_pkt_cfg().
 *
 * IMPORTANT: setting a bit DROPS frames matching that condition. This matches
 * the hardware: user guide DS50004157A section 6.63 states, for example,
 * "Bit 7 - RX_LS_ERR Setting this bit causes the frame to be dropped if a
 * receive frame is less than 64 bytes in length."
 *
 * A mask of zero therefore accepts everything, which is the reset state.
 *============================================================================*/

#define QSE_FILTER_VLAN                 (1uL << 14u) /*!< VLAN tagged.        */
#define QSE_FILTER_UNSUPPORTED_OPCODE   (1uL << 13u) /*!< Unknown control.    */
#define QSE_FILTER_PAUSE_OPCODE         (1uL << 12u) /*!< PAUSE frames.       */
#define QSE_FILTER_CONTROL_FRAME        (1uL << 11u) /*!< Control frames.     */
#define QSE_FILTER_UNICAST_DA_MISMATCH  (1uL << 10u) /*!< Unicast, wrong DA.  */
#define QSE_FILTER_BROADCAST            (1uL <<  9u) /*!< Broadcast frames.   */
#define QSE_FILTER_MULTICAST            (1uL <<  8u) /*!< Multicast frames.   */
#define QSE_FILTER_LENGTH_SHORT         (1uL <<  7u) /*!< Below 64 bytes.     */
#define QSE_FILTER_LENGTH_LONG          (1uL <<  6u) /*!< Above max length.   */
#define QSE_FILTER_LENGTH_CHECK         (1uL <<  5u) /*!< Length field wrong. */
#define QSE_FILTER_FCS_ERROR            (1uL <<  4u) /*!< FCS/CRC errors.     */
#define QSE_FILTER_PCS_ERROR            (1uL <<  3u) /*!< PCS errors.         */

/*! Drop every malformed frame class, accept normal traffic. */
#define QSE_FILTER_ERRORS_ONLY      ( QSE_FILTER_LENGTH_SHORT | \
                                      QSE_FILTER_LENGTH_LONG  | \
                                      QSE_FILTER_LENGTH_CHECK | \
                                      QSE_FILTER_FCS_ERROR    | \
                                      QSE_FILTER_PCS_ERROR )

/*==============================================================================
 *                          Hardware field limits
 *============================================================================*/

#define QSE_IPG_CNT_MIN             ( 2u )  /*!< Core clamps below this.      */
#define QSE_IPG_CNT_MAX             ( 48u ) /*!< Core clamps above this.      */
#define QSE_IPG_CNT_STANDARD        ( 12u ) /*!< IEEE 802.3 value.            */

/*! Maximum custom preamble size, word/byte count minus one. The field is 3 bits
 *  but the core limits it to 6 because the SFD is appended.
 */
#define QSE_PREAMBLE_SIZE_MAX       ( 6u )

/*! Written to max_tx_pkt_len or max_rx_pkt_len to disable length checking. Any
 *  value with bits [15:14] set disables the check; this is the reset state.
 */
#define QSE_MAX_PKT_LEN_NO_CHECK    ( 0xC000u )

#define QSE_PAUSE_WATERMARK_MAX     ( 8191u ) /*!< 13-bit watermark fields.   */

/*==============================================================================
 *                                 Types
 *============================================================================*/

/***************************************************************************//**
  This structure identifies a CoreQSE hardware instance. QSE_init() initializes
  it. Pass a pointer to an initialized instance as the first parameter of every
  other driver function.

  Do not manipulate the contents directly.
 */
typedef struct QSE_instance
{
    addr_t   base_address;  /*!< AXI4-Lite base address.                      */
    uint8_t  mac_addr[6u];  /*!< Shadow of the programmed MAC address.        */
    uint32_t tx_cfg_shadow; /*!< Shadow of MAC_TX_CFG.                        */
    uint32_t rx_cfg_shadow; /*!< Shadow of MAC_RX_CFG.                        */

    /*! Base address of the associated COREMDIO_APB instance, or 0 when the
     *  design has no PHY.
     *
     *  CoreQSE contains NO MII management hardware, so PHY access is performed
     *  through a separate COREMDIO_APB IP. Recording its address here lets the
     *  QSE_phy_* functions present a single API to the application, the way
     *  CoreTSE does, even though two independent IPs are involved.
     *  See QSE_attach_mdio().
     */
    addr_t   mdio_base_address;

    /*! PHY address on the MDIO bus, 0 to 31. Set by QSE_attach_mdio(). */
    uint8_t  phy_addr;
} QSE_instance_t;

/***************************************************************************//**
  Transmit MAC path configuration, passed to QSE_tx_cfg_set().

  Populate using QSE_get_default_tx_cfg() then adjust, so that fields added in
  later driver revisions receive sane values.
 */
typedef struct QSE_tx_config
{
    uint8_t  fcs_insert_mode;  /*!< 1: hardware appends a 4-byte FCS to every
                                    frame, making the wire frame 4 bytes longer
                                    than the buffer given to the DMA. Use
                                    QSE_get_wire_length() to size buffers.    */

    uint8_t  fcs_stomp_mode;   /*!< 1: corrupt the FCS by XOR with 0xFFFFFFFF.
                                    Negative testing only.                   */

    uint8_t  fcs_error_inject; /*!< 1: corrupt the FCS by XOR with 0x55555555.
                                    Negative testing only.                   */

    uint8_t  pad_enable;       /*!< 1: pad frames with payload shorter than 46
                                    bytes, giving a 60-byte frame before FCS.
                                    Per the user guide this bit is only valid
                                    when fcs_insert_mode is 0.               */

    uint8_t  ipg_count;        /*!< Inter-packet gap in bytes. Valid 2 to 48;
                                    the core clamps outside that range.
                                    QSE_IPG_CNT_STANDARD is the IEEE value.
                                    Below 12 may truncate up to 4 preamble
                                    bytes.                                   */

    uint16_t max_tx_pkt_len;   /*!< Maximum transmit frame length. Feeds the
                                    transmit statistics block; it does not gate
                                    the data path. QSE_MAX_PKT_LEN_NO_CHECK
                                    disables checking.                       */

    uint8_t  preamble_size;    /*!< Custom preamble size in WORDS minus one,
                                    0 to QSE_PREAMBLE_SIZE_MAX. Requires
                                    COREQSE_CFG_TX_PREAMBLE. Set 0 for a
                                    standard preamble.                       */

    uint32_t preamble_content; /*!< Custom preamble pattern, written to
                                    MAC_TX_PRE_LOWER. Requires
                                    COREQSE_CFG_TX_PREAMBLE.                 */
} QSE_tx_config_t;

/***************************************************************************//**
  Receive MAC path configuration, passed to QSE_rx_cfg_set().

  Populate using QSE_get_default_rx_cfg() then adjust.
 */
typedef struct QSE_rx_config
{
    uint8_t  fcs_remove;     /*!< 1: strip the 4-byte FCS before the frame
                                  reaches AXI4-Stream, so software sees the
                                  payload only.                              */

    uint8_t  pad_remove;     /*!< 1: strip pad bytes from short frames.
                                  Requires fcs_remove to also be set; the
                                  hardware ignores it otherwise.             */

    uint16_t max_rx_pkt_len; /*!< Maximum receive frame length. Longer frames
                                  raise an error in AXI4S_RX_INITR_TUSER and
                                  increment MAC_RX_GTMAXB.
                                  QSE_MAX_PKT_LEN_NO_CHECK disables checking. */

    uint8_t  preamble_size;  /*!< Expected custom preamble size in BYTES minus
                                  one, 0 to QSE_PREAMBLE_SIZE_MAX. Note the
                                  receive field counts bytes whereas the
                                  transmit field counts words. Requires
                                  COREQSE_CFG_RX_PREAMBLE.                   */
} QSE_rx_config_t;

/***************************************************************************//**
  PAUSE flow control configuration, passed to QSE_pause_cfg_set().

  Requires COREQSE_CFG_RX_PAUSE and COREQSE_CFG_TX_PAUSE.
 */
typedef struct QSE_pause_config
{
    uint16_t rx_fifo_high_watermark; /*!< Occupancy that triggers PAUSE,
                                          0 to QSE_PAUSE_WATERMARK_MAX.      */

    uint16_t rx_fifo_low_watermark;  /*!< Occupancy that resumes traffic.
                                          Must be below the high watermark.  */

    uint16_t pause_timer;            /*!< Pause quanta placed in transmitted
                                          PAUSE frames, each 512 bit times.
                                          Reset value 0xFFFF.                */

    uint8_t  pause_quanta_retransmit;/*!< Quanta to wait before retransmitting
                                          a PAUSE frame while the FIFO remains
                                          above the low watermark.           */
} QSE_pause_config_t;

/***************************************************************************//**
  Transmit statistics counters, populated by QSE_get_tx_stats().

  Requires COREQSE_CFG_STATS. Individual counters are also independently enabled
  in the configurator, so a field may read 0 because that specific counter was
  not generated.

  @note
  These counters are Read/Write-to-Clear in hardware, NOT read-to-clear.
  Reading is non-destructive. See QSE_clear_stats().
 */
typedef struct QSE_tx_stats
{
    uint32_t tx_frame_count;      /*!< MAC_TX_PKT, frames transmitted.        */
    uint32_t tx_err_count;        /*!< MAC_TX_ERR, errors of any cause.       */
    uint32_t tx_length_long_err;  /*!< MAC_TX_LL_ERR.                         */
    uint32_t tx_length_check_err; /*!< MAC_TX_LC_ERR.                         */
    uint32_t tx_length_short_err; /*!< MAC_TX_LS_ERR.                         */
    uint32_t tx_fcs_err;          /*!< MAC_TX_FCS_ERR.                        */
    uint32_t tx_frame_err;        /*!< MAC_TX_FRM_ERR.                        */
    uint32_t tx_broadcast_count;  /*!< MAC_TX_BCAST.                          */
    uint32_t tx_multicast_count;  /*!< MAC_TX_MCAST.                          */
    uint32_t tx_pause_frame_count;/*!< MAC_TX_PF, PAUSE frames sent.          */
    uint32_t tx_control_count;    /*!< MAC_TX_CF.                             */
    uint32_t tx_vlan_count;       /*!< MAC_TX_VLAN.                           */
    uint32_t tx_pad_count;        /*!< MAC_TX_PAD, padded frames.             */
    uint32_t tx_64b_count;        /*!< MAC_TX_64B.                            */
    uint32_t tx_127b_count;       /*!< MAC_TX_127B.                           */
    uint32_t tx_255b_count;       /*!< MAC_TX_255B.                           */
    uint32_t tx_511b_count;       /*!< MAC_TX_511B.                           */
    uint32_t tx_1kb_count;        /*!< MAC_TX_1KB.                            */
    uint32_t tx_maxb_count;       /*!< MAC_TX_MAXB.                           */
    uint32_t tx_maxb_vlan_count;  /*!< MAC_TX_MAXB_VLAN.                      */
    uint32_t tx_oversize_count;   /*!< MAC_TX_GTMAXB, above max_tx_pkt_len.   */
    uint32_t tx_fifo_ecc_sb_corr; /*!< MAC_TX_FIFO_SBCOR, ECC corrected.      */
    uint32_t tx_fifo_ecc_db_det;  /*!< MAC_TX_FIFO_DBDET, ECC double bit.     */
} QSE_tx_stats_t;

/***************************************************************************//**
  Receive statistics counters, populated by QSE_get_rx_stats().

  Requires COREQSE_CFG_STATS.

  @note
  These counters are Read/Write-to-Clear in hardware, NOT read-to-clear.
 */
typedef struct QSE_rx_stats
{
    uint32_t rx_frame_count;      /*!< MAC_RX_PKT, frames received.           */
    uint32_t rx_err_count;        /*!< MAC_RX_ERR, errors of any cause.       */
    uint32_t rx_length_long_err;  /*!< MAC_RX_LL_ERR.                         */
    uint32_t rx_length_check_err; /*!< MAC_RX_LC_ERR.                         */
    uint32_t rx_length_short_err; /*!< MAC_RX_LS_ERR, below 64 bytes.         */
    uint32_t rx_fcs_error_count;  /*!< MAC_RX_FCS_ERR. Non-zero means
                                       corruption on the link.               */
    uint32_t rx_frame_err;        /*!< MAC_RX_FRM_ERR.                        */
    uint32_t rx_broadcast_count;  /*!< MAC_RX_BCAST.                          */
    uint32_t rx_multicast_count;  /*!< MAC_RX_MCAST.                          */
    uint32_t rx_pause_frame_count;/*!< MAC_RX_PF, PAUSE frames received.      */
    uint32_t rx_control_count;    /*!< MAC_RX_CF.                             */
    uint32_t rx_vlan_count;       /*!< MAC_RX_VLAN.                           */
    uint32_t rx_pad_count;        /*!< MAC_RX_PAD. Counted only when
                                       fcs_remove is set.                    */
    uint32_t rx_64b_count;        /*!< MAC_RX_64B.                            */
    uint32_t rx_127b_count;       /*!< MAC_RX_127B.                           */
    uint32_t rx_255b_count;       /*!< MAC_RX_255B.                           */
    uint32_t rx_511b_count;       /*!< MAC_RX_511B.                           */
    uint32_t rx_1kb_count;        /*!< MAC_RX_1KB.                            */
    uint32_t rx_maxb_count;       /*!< MAC_RX_MAXB.                           */
    uint32_t rx_maxb_vlan_count;  /*!< MAC_RX_MAXB_VLAN.                      */
    uint32_t rx_oversize_count;   /*!< MAC_RX_GTMAXB.                         */
    uint32_t rx_fifo_ecc_sb_corr; /*!< MAC_RX_FIFO_SBCOR.                     */
    uint32_t rx_fifo_ecc_db_det;  /*!< MAC_RX_FIFO_DBDET.                     */
    uint32_t rx_drop_count;       /*!< MAC_RX_DROP. Non-zero usually means a
                                       filter or length misconfiguration.    */
    uint32_t pcs_eb_ecc_sb_corr;  /*!< PCS_EB_SBCOR, elastic buffer ECC.      */
    uint32_t pcs_eb_ecc_db_det;   /*!< PCS_EB_DBDET.                          */
    uint32_t pcs_code_err_count;  /*!< PCS_CODE_ERR, 16b/20b violations.      */
    uint32_t pcs_sync_lost_count; /*!< PCS_SYNC_LOST_ERR. Closest available
                                       proxy for the link going down.        */
} QSE_rx_stats_t;

/*==============================================================================
 *                              Configuration
 *============================================================================*/

/***************************************************************************//**
  QSE_init() initializes the driver. It records the base address of the CoreQSE
  instance, initializes the QSE_instance_t structure, and disables the transmit
  and receive paths so that configuration is applied to a quiescent MAC.

  Call this before any other CoreQSE driver function.

  @note
  MAC_TX_CFG and MAC_RX_CFG have their enable bits SET at reset (0x8C3C0000 and
  0x803C0000). This function clears them, so the register contents afterwards
  deliberately differ from the documented reset values.

  @param this_qse
    Points to the QSE_instance_t structure holding all data for the CoreQSE
    instance being initialized.

  @param base_addr
    Base address of the CoreQSE registers in the processor memory map.

  @return
    QSE_OK on success, QSE_ERR_PARAM if this_qse is NULL or base_addr is 0.

  @code
  QSE_instance_t g_qse;
  if (QSE_init(&g_qse, COREQSE_BASE_ADDR) != QSE_OK) { return -1; }
  @endcode
 */
uint8_t QSE_init( QSE_instance_t * this_qse, addr_t base_addr );

/***************************************************************************//**
  QSE_get_default_tx_cfg() fills a QSE_tx_config_t with IEEE 802.3 compliant
  defaults: 1518-byte maximum, inter-packet gap 12, hardware FCS insertion,
  padding disabled, standard preamble, no error injection.

  padding defaults off because the user guide states pad_enable is only valid
  when fcs_insert_mode is clear, and the default inserts the FCS.

  Not part of the driver user guide; provided so that applications need not
  hand-populate every field.

  @param cfg
    Structure to populate.

  @return
    QSE_OK, or QSE_ERR_PARAM if cfg is NULL.
 */
uint8_t QSE_get_default_tx_cfg( QSE_tx_config_t * cfg );

/***************************************************************************//**
  QSE_get_default_rx_cfg() fills a QSE_rx_config_t with IEEE 802.3 compliant
  defaults: 1518-byte maximum, FCS removal enabled so software sees payload
  only, pad removal disabled, standard preamble.

  Not part of the driver user guide.

  @param cfg
    Structure to populate.

  @return
    QSE_OK, or QSE_ERR_PARAM if cfg is NULL.
 */
uint8_t QSE_get_default_rx_cfg( QSE_rx_config_t * cfg );

/***************************************************************************//**
  QSE_tx_cfg_set() configures the transmit MAC path: FCS insertion, stomp and
  error injection, padding of short frames, inter-packet gap, maximum transmit
  packet length, and custom preamble content and size.

  The transmit enable bit is preserved, so this may be called on a running MAC.
  The hardware samples these fields on a packet boundary.

  @param this_qse
    Points to an initialized QSE_instance_t.

  @param cfg
    Points to the transmit configuration to apply.

  @return
    QSE_OK,
    QSE_ERR_PARAM if a pointer is NULL, max_tx_pkt_len is 0, ipg_count is
    outside 2 to 48, or preamble_size exceeds QSE_PREAMBLE_SIZE_MAX,
    QSE_ERR_NOT_SUPPORTED if preamble_size is non-zero but
    COREQSE_CFG_TX_PREAMBLE is 0.
 */
uint8_t QSE_tx_cfg_set( QSE_instance_t * this_qse,
                        const QSE_tx_config_t * cfg );

/***************************************************************************//**
  QSE_rx_cfg_set() configures the receive MAC path: FCS removal, pad removal,
  maximum receive packet length and custom preamble size.

  The receive enable bit is preserved.

  @param this_qse
    Points to an initialized QSE_instance_t.

  @param cfg
    Points to the receive configuration to apply.

  @return
    QSE_OK,
    QSE_ERR_PARAM if a pointer is NULL, max_rx_pkt_len is 0, preamble_size
    exceeds QSE_PREAMBLE_SIZE_MAX, or pad_remove is set without fcs_remove,
    QSE_ERR_NOT_SUPPORTED if preamble_size is non-zero but
    COREQSE_CFG_RX_PREAMBLE is 0.
 */
uint8_t QSE_rx_cfg_set( QSE_instance_t * this_qse,
                        const QSE_rx_config_t * cfg );

/***************************************************************************//**
  QSE_set_mac_addr() programs the 48-bit MAC address. The hardware uses it as
  the source address of generated frames, as the destination reference for
  unicast receive filtering and PAUSE frame detection, and for Wake-on-LAN
  magic packet matching.

  mac_addr[0] is the first byte on the wire. Bytes 0 to 3 occupy
  MAC_ADDR_LOWER; bytes 4 and 5 occupy MAC_ADDR_UPPER bits [7:0] and [15:8].

  @param this_qse
    Points to an initialized QSE_instance_t.

  @param mac_addr
    Points to a six-byte MAC address, mac_addr[0] transmitted first.

  @return
    QSE_OK, or QSE_ERR_PARAM if a pointer is NULL.

  @code
  uint8_t mac[6] = {0x00,0x04,0xA3,0x11,0x22,0x33};
  QSE_set_mac_addr(&g_qse, mac);
  @endcode
 */
uint8_t QSE_set_mac_addr( QSE_instance_t * this_qse,
                          const uint8_t * mac_addr );

/***************************************************************************//**
  QSE_get_mac_addr() reads the MAC address back from hardware.

  Not part of the driver user guide. Useful as a write/read-back confirmation
  that the MAC_ADDR_UPPER byte placement matches expectations on your IP
  revision, which the CoreQSE user guide documents ambiguously.

  @param this_qse
    Points to an initialized QSE_instance_t.

  @param mac_addr
    Six-byte buffer receiving the address, mac_addr[0] transmitted first.

  @return
    QSE_OK, or QSE_ERR_PARAM if a pointer is NULL.
 */
uint8_t QSE_get_mac_addr( const QSE_instance_t * this_qse,
                          uint8_t * mac_addr );

/***************************************************************************//**
  QSE_get_wire_length() returns the number of bytes a frame occupies on the wire
  and the number of bytes a receive DMA should be armed for, based on the
  transmit and receive configuration currently programmed in hardware.

  Not part of the driver user guide. Provided because hardware padding, FCS
  insertion and receive FCS removal together are the most common cause of
  silent truncation and packet drop when sizing DMA transfers by hand.

  @param this_qse
    Points to an initialized QSE_instance_t.

  @param payload_len
    Bytes handed to the transmit DMA, including the 14-byte Ethernet header.

  @param wire_len
    Receives the length on the wire. May be NULL.

  @param rx_len
    Receives the length the receive DMA should be armed for. May be NULL.

  @return
    QSE_OK, or QSE_ERR_PARAM if this_qse is NULL or payload_len is 0.

  @code
  uint16_t wire, rx;
  QSE_get_wire_length(&g_qse, 60u, &wire, &rx);   // wire 64, rx 60
  @endcode
 */
uint8_t QSE_get_wire_length( const QSE_instance_t * this_qse,
                             uint16_t payload_len,
                             uint16_t * wire_len,
                             uint16_t * rx_len );

/*==============================================================================
 *                            Data path control
 *============================================================================*/

/***************************************************************************//**
  QSE_tx_enable() enables the MAC transmit data path, allowing frames to flow
  from the AXI4-Stream transmit interface to the line.

  Configure the MAC before enabling.

  @param this_qse
    Points to an initialized QSE_instance_t.
 */
void QSE_tx_enable( QSE_instance_t * this_qse );

/***************************************************************************//**
  QSE_tx_disable() disables the MAC transmit data path. The transition takes
  effect on a packet boundary, so no partial frame is produced.

  @param this_qse
    Points to an initialized QSE_instance_t.
 */
void QSE_tx_disable( QSE_instance_t * this_qse );

/***************************************************************************//**
  QSE_rx_enable() enables the MAC receive data path, allowing incoming frames
  to be delivered to the AXI4-Stream receive interface.

  @note
  The receive path additionally requires SYS_RX_RESETN, driven by the
  transceiver receiver valid indication, to be released. Setting this bit does
  not make the receiver operational without CDR lock; with external loopback the
  cable must be fitted.

  @param this_qse
    Points to an initialized QSE_instance_t.
 */
void QSE_rx_enable( QSE_instance_t * this_qse );

/***************************************************************************//**
  QSE_rx_disable() disables the MAC receive data path.

  @param this_qse
    Points to an initialized QSE_instance_t.
 */
void QSE_rx_disable( QSE_instance_t * this_qse );

/***************************************************************************//**
  QSE_tx_frame() is NOT IMPLEMENTABLE on CoreQSE and always returns
  QSE_ERR_NOT_SUPPORTED.

  The driver user guide describes this function as writing frame data to the
  transmit AXI4-Stream interface. CoreQSE exposes no processor addressable data
  path: AXI4S_TX_TARG_* are streaming ports driven by a companion DMA engine,
  and they are not reachable through the AXI4-Lite register interface. There is
  no transmit data FIFO register in the register map, which spans 0x000 to
  0x50C and contains only configuration, address, filter, flow control and
  statistics registers.

  To transmit a frame, place it in memory and use the DMA driver, for example
  CoreAXI4ProtoConv:

  @code
  uint16_t wire, rx;
  QSE_get_wire_length(&g_qse, frame_len, &wire, &rx);

  PCDMA_S2MM_configure(&g_pcdma, rx, RX_BUF, S2MM_ID, PCDMA_BURST_TYPE_INCR);
  PCDMA_S2MM_start_transfer(&g_pcdma);          // arm receiver FIRST

  PCDMA_MM2S_configure(&g_pcdma, frame_len, TX_BUF, MM2S_ID,
                       PCDMA_BURST_TYPE_INCR);
  PCDMA_MM2S_start_transfer(&g_pcdma);
  @endcode

  The function is retained so that code written against the driver user guide
  still compiles, and so the limitation is reported rather than silently
  producing no traffic.

  @param this_qse
    Points to an initialized QSE_instance_t.

  @param frame_data
    Unused.

  @param frame_len
    Unused.

  @return
    Always QSE_ERR_NOT_SUPPORTED.
 */
uint8_t QSE_tx_frame( QSE_instance_t * this_qse,
                      const uint8_t * frame_data,
                      uint32_t frame_len );

/*==============================================================================
 *                                 Status
 *============================================================================*/

/***************************************************************************//**
  QSE_get_tx_status() reports transmit error status.

  @warning
  CoreQSE has NO transmit status register. The transmit FIFO underrun condition
  is exposed only as the hardware output port SYS_MAC_TX_ERR_FIFO_UNDERRUN,
  which asserts for one clock cycle in the XCVR_TX_CLK domain. It is not
  readable through AXI4-Lite, and in the reference designs the port is left
  unconnected.

  This function therefore returns 0 unconditionally unless the statistics
  counter block is present, in which case it derives a status from MAC_TX_ERR.
  For real underrun detection, route SYS_MAC_TX_ERR_FIFO_UNDERRUN to a CoreGPIO
  input or a sticky capture register in your design.

  @param this_qse
    Points to an initialized QSE_instance_t.

  @return
    Non-zero if a transmit error has been counted, 0 otherwise. Always 0 when
    the statistics block is absent.
 */
uint8_t QSE_get_tx_status( QSE_instance_t * this_qse );

/***************************************************************************//**
  QSE_get_rx_status() reports receive error status.

  @warning
  As with QSE_get_tx_status(), CoreQSE has no receive status register. The
  receive FIFO overflow condition is exposed only as the output port
  SYS_MAC_RX_FIFO_OVERFLOW, which is unconnected in the reference designs.

  This function returns a status derived from MAC_RX_ERR and MAC_RX_DROP when
  the statistics block is present, and 0 otherwise.

  @param this_qse
    Points to an initialized QSE_instance_t.

  @return
    Non-zero if a receive error or drop has been counted, 0 otherwise. Always 0
    when the statistics block is absent.
 */
uint8_t QSE_get_rx_status( QSE_instance_t * this_qse );

/***************************************************************************//**
  QSE_pcs_get_sync_status() reports PCS synchronization state.

  @warning
  CoreQSE has NO PCS status register. Synchronization is exposed only as the
  hardware output ports SYNC and PCS_RX_SYNC_FSM_CUR_STATE, both in the
  XCVR_TX_CLK domain and both left unconnected in the reference designs. The
  register map contains no link or sync status register.

  When the statistics block is present this function samples PCS_CODE_ERR and
  PCS_SYNC_LOST_ERR twice and reports synchronized if neither moved, which is
  an indirect indication rather than a true sync flag. When the statistics block
  is absent there is no software visible link indication at all and the function
  returns 0.

  For a genuine link indication, route the SYNC output, or the transceiver
  LANE0_RX_VAL and LANE0_TX_CLK_STABLE signals, to a CoreGPIO input.

  @param this_qse
    Points to an initialized QSE_instance_t.

  @return
    1 if the PCS receive path appears stable, 0 if not synchronized or if no
    indication is available.
 */
uint8_t QSE_pcs_get_sync_status( QSE_instance_t * this_qse );

/***************************************************************************//**
  QSE_self_test() checks registers with known reset values against those values.

  Verifies CORE_VER bits [15:0] read as 0, MAC_TX_CFG equals 0x8C3C0000 and
  MAC_RX_CFG equals 0x803C0000, both ignoring bit 31, which QSE_init() clears
  by design.

  Must be called before QSE_tx_cfg_set() or QSE_rx_cfg_set(), which legitimately
  change these registers.

  MAC_TX_PARAM1/2 and MAC_RX_PARAM1/2 are deliberately not checked: they are
  read-only registers reporting the build-time configuration, so their value
  depends on the IP configurator settings. Use QSE_get_tx_param() and
  QSE_get_rx_param() to read them.

  Not part of the driver user guide.

  @param this_qse
    Points to an initialized QSE_instance_t.

  @param failed_offset
    Receives the offset of the first mismatching register, or 0xFFFFFFFF if all
    checks passed. Always written when non-NULL. May be NULL.

  @return
    QSE_OK if all checked registers hold reset values, QSE_ERR otherwise,
    QSE_ERR_PARAM if this_qse is NULL.
 */
uint8_t QSE_self_test( const QSE_instance_t * this_qse,
                       uint32_t * failed_offset );

/***************************************************************************//**
  QSE_get_version() reads CORE_VER and returns the hardware revision.

  Not part of the driver user guide.

  @param this_qse
    Points to an initialized QSE_instance_t.

  @param major
    Receives the major version. May be NULL.

  @param minor
    Receives the minor version. May be NULL.

  @return
    QSE_OK, or QSE_ERR_PARAM if this_qse is NULL.
 */
uint8_t QSE_get_version( const QSE_instance_t * this_qse,
                         uint8_t * major, uint8_t * minor );

/***************************************************************************//**
  QSE_get_tx_param() reads MAC_TX_PARAM1 and MAC_TX_PARAM2, which are read-only
  registers reporting the build-time transmit configuration:

  @code
  MAC_TX_PARAM2 = { 11'b0, PAUSE_EN, ECC_ENABLE, CHECK_LT, PREAMBLE,
                    RAM_TYPE, FIFO_MODE, FIFO_DEPTH }
  @endcode

  Useful for confirming at run time that the programmed bitstream matches the
  configurator settings you expect. Do not compare the result against a
  constant: a build with FIFO_DEPTH 32 and LSRAM reads 0x00010020, whereas the
  same build with RAM_TYPE 0 reads 0x00000020.

  Not part of the driver user guide.

  @param this_qse
    Points to an initialized QSE_instance_t.

  @param param1
    Receives MAC_TX_PARAM1. May be NULL.

  @param param2
    Receives MAC_TX_PARAM2. May be NULL.

  @return
    QSE_OK, or QSE_ERR_PARAM if this_qse is NULL.
 */
uint8_t QSE_get_tx_param( const QSE_instance_t * this_qse,
                          uint32_t * param1, uint32_t * param2 );

/***************************************************************************//**
  QSE_get_rx_param() reads MAC_RX_PARAM1 and MAC_RX_PARAM2. Same field layout
  and same caveats as QSE_get_tx_param().

  Not part of the driver user guide.

  @param this_qse
    Points to an initialized QSE_instance_t.

  @param param1
    Receives MAC_RX_PARAM1. May be NULL.

  @param param2
    Receives MAC_RX_PARAM2. May be NULL.

  @return
    QSE_OK, or QSE_ERR_PARAM if this_qse is NULL.
 */
uint8_t QSE_get_rx_param( const QSE_instance_t * this_qse,
                          uint32_t * param1, uint32_t * param2 );

/*==============================================================================
 *                              Flow control
 *============================================================================*/

/***************************************************************************//**
  QSE_pause_cfg_set() configures IEEE 802.3x PAUSE flow control: receive FIFO
  high and low watermarks, the pause timer value and the retransmission
  interval.

  When receive FIFO occupancy rises above the high watermark the MAC emits a
  PAUSE frame; it resumes when occupancy falls below the low watermark.

  Requires COREQSE_CFG_RX_PAUSE for the watermarks and COREQSE_CFG_TX_PAUSE for
  the pause timer.

  @param this_qse
    Points to an initialized QSE_instance_t.

  @param cfg
    Points to the PAUSE configuration to apply.

  @return
    QSE_OK,
    QSE_ERR_PARAM if a pointer is NULL, a watermark exceeds
    QSE_PAUSE_WATERMARK_MAX, or the high watermark is not above the low one,
    QSE_ERR_NOT_SUPPORTED if flow control is absent from this IP build.
 */
uint8_t QSE_pause_cfg_set( QSE_instance_t * this_qse,
                           const QSE_pause_config_t * cfg );

/*==============================================================================
 *                            Receive filtering
 *============================================================================*/

/***************************************************************************//**
  QSE_set_rx_filter_pkt_cfg() configures packet based receive frame filtering.

  Each bit set in filter_mask causes frames matching that condition to be
  DROPPED. A mask of 0 accepts everything, which is the reset state. Use
  QSE_FILTER_ERRORS_ONLY to drop malformed frames while accepting normal
  traffic.

  Requires COREQSE_CFG_RX_FRMFIL_PKT. Takes effect in store-and-forward FIFO
  mode only.

  @param this_qse
    Points to an initialized QSE_instance_t.

  @param filter_mask
    Bitwise OR of QSE_FILTER_* constants.

  @return
    QSE_OK,
    QSE_ERR_PARAM if this_qse is NULL,
    QSE_ERR_NOT_SUPPORTED if the filter block is absent.

  @code
  QSE_set_rx_filter_pkt_cfg(&g_qse,
                            QSE_FILTER_FCS_ERROR | QSE_FILTER_LENGTH_LONG);
  @endcode
 */
uint8_t QSE_set_rx_filter_pkt_cfg( QSE_instance_t * this_qse,
                                   uint32_t filter_mask );

/***************************************************************************//**
  QSE_get_rx_filter_pkt_cfg() reads the current receive filter mask.

  Not part of the driver user guide.

  @param this_qse
    Points to an initialized QSE_instance_t.

  @param filter_mask
    Receives the current mask.

  @return
    QSE_OK,
    QSE_ERR_PARAM if a pointer is NULL,
    QSE_ERR_NOT_SUPPORTED if the filter block is absent.
 */
uint8_t QSE_get_rx_filter_pkt_cfg( const QSE_instance_t * this_qse,
                                   uint32_t * filter_mask );

/***************************************************************************//**
  QSE_set_rx_filter_hash() programs the 64-bit hash table used for destination
  MAC address filtering. Frames whose destination address maps to a cleared
  hash entry are filtered out.

  Requires COREQSE_CFG_RX_FRMFIL_HASH. Takes effect in store-and-forward FIFO
  mode only.

  @param this_qse
    Points to an initialized QSE_instance_t.

  @param hash_table_low
    Lower 32 bits of the hash table, written to MAC_RX_FRMFIL_HTR0.

  @param hash_table_high
    Upper 32 bits of the hash table, written to MAC_RX_FRMFIL_HTR1.

  @return
    QSE_OK,
    QSE_ERR_PARAM if this_qse is NULL,
    QSE_ERR_NOT_SUPPORTED if the hash block is absent.
 */
uint8_t QSE_set_rx_filter_hash( QSE_instance_t * this_qse,
                                uint32_t hash_table_low,
                                uint32_t hash_table_high );

/*==============================================================================
 *                              Wake-on-LAN
 *============================================================================*/

/***************************************************************************//**
  QSE_wol_enable() enables Wake-on-LAN magic packet detection. The core scans
  incoming frames for the magic packet pattern containing the configured MAC
  address repeated 16 times, and asserts WOL_DETECT for one clock cycle on a
  match.

  Set the MAC address with QSE_set_mac_addr() before enabling.

  Requires COREQSE_CFG_RX_WOL.

  @param this_qse
    Points to an initialized QSE_instance_t.
 */
void QSE_wol_enable( QSE_instance_t * this_qse );

/***************************************************************************//**
  QSE_wol_disable() disables Wake-on-LAN magic packet detection.

  @param this_qse
    Points to an initialized QSE_instance_t.
 */
void QSE_wol_disable( QSE_instance_t * this_qse );

/*==============================================================================
 *                               Statistics
 *============================================================================*/

/***************************************************************************//**
  QSE_get_tx_stats() reads all enabled transmit statistics counters into a
  QSE_tx_stats_t structure. Counters not enabled in the configurator read 0.

  @warning
  The driver user guide states these counters are read-to-clear. They are NOT.
  User guide DS50004157A gives every statistics register the property
  "Read/Write-to Clear" with access type RWC, meaning a WRITE clears the
  counter and a read is non-destructive.

  Consequently this function does not reset the counters. To measure activity
  over an interval, take two snapshots and subtract, or call QSE_clear_stats()
  first.

  Requires COREQSE_CFG_STATS. When the counter block is absent the structure is
  zeroed, which is indistinguishable from a healthy idle link; check
  COREQSE_CFG_STATS before relying on the result.

  @param this_qse
    Points to an initialized QSE_instance_t.

  @param stats_out
    Structure receiving the counter values.

  @code
  QSE_tx_stats_t before, after;
  QSE_get_tx_stats(&g_qse, &before);
  // ... run traffic ...
  QSE_get_tx_stats(&g_qse, &after);
  uint32_t sent = after.tx_frame_count - before.tx_frame_count;
  @endcode
 */
void QSE_get_tx_stats( QSE_instance_t * this_qse,
                       QSE_tx_stats_t * stats_out );

/***************************************************************************//**
  QSE_get_rx_stats() reads all enabled receive and PCS statistics counters into
  a QSE_rx_stats_t structure.

  The same read-to-clear correction applies as for QSE_get_tx_stats(): reads are
  non-destructive.

  Requires COREQSE_CFG_STATS.

  @param this_qse
    Points to an initialized QSE_instance_t.

  @param stats_out
    Structure receiving the counter values.
 */
void QSE_get_rx_stats( QSE_instance_t * this_qse,
                       QSE_rx_stats_t * stats_out );

/***************************************************************************//**
  QSE_clear_stats() clears all enabled transmit, receive and PCS statistics
  counters.

  @note
  The driver user guide describes this as clearing counters by reading and
  discarding their values, relying on read-to-clear behaviour. The hardware is
  write-to-clear (access type RWC), so this implementation WRITES each counter
  register instead. Reading would leave every counter unchanged.

  Requires COREQSE_CFG_STATS.

  @param this_qse
    Points to an initialized QSE_instance_t.
 */
void QSE_clear_stats( QSE_instance_t * this_qse );

/***************************************************************************//**
  QSE_stats_are_clean() compares two receive snapshots and reports whether any
  error counter incremented.

  Checks every receive error counter, the drop counter, the FIFO and elastic
  buffer ECC counters, and the PCS code error and sync lost counters. Because
  CoreQSE has no link status register, a change in pcs_sync_lost_count is the
  most direct evidence available that the link degraded.

  Not part of the driver user guide.

  @param before
    Snapshot taken before traffic.

  @param after
    Snapshot taken after traffic.

  @return
    QSE_OK if no error counter changed, QSE_ERR if any did, QSE_ERR_PARAM if a
    pointer is NULL.
 */
uint8_t QSE_stats_are_clean( const QSE_rx_stats_t * before,
                             const QSE_rx_stats_t * after );

/*==============================================================================
 *                              Frame helper
 *============================================================================*/

/***************************************************************************//**
  QSE_build_frame_header() writes a 14-byte Ethernet II header into a buffer:
  six byte destination address, six byte source address, two byte EtherType.

  Not part of the driver user guide. Provided because receive filtering compares
  the destination address, so a buffer filled with an arbitrary pattern produces
  a frame with an effectively random destination that is only accepted when
  filtering happens to be permissive.

  @param buffer
    Destination buffer, at least 14 bytes.

  @param dest_mac
    Six byte destination address. NULL requests broadcast.

  @param src_mac
    Six byte source address. NULL uses the address set by QSE_set_mac_addr().

  @param ether_type
    EtherType in host order, for example 0x88B5 for local experimental use.

  @param this_qse
    Points to an initialized QSE_instance_t, used when src_mac is NULL.

  @return
    Number of bytes written, always 14, or 0 on invalid parameters.
 */
uint16_t QSE_build_frame_header( uint8_t * buffer,
                                 const uint8_t * dest_mac,
                                 const uint8_t * src_mac,
                                 uint16_t ether_type,
                                 const QSE_instance_t * this_qse );

/*==============================================================================
 *              EXTERNAL PHY MANAGEMENT, MDIO / MII STATION
 *
 * IMPORTANT ARCHITECTURAL NOTE
 * ----------------------------
 * CoreQSE contains NO MII management hardware. Its register map, spanning 0x000
 * to 0x50C, holds only MAC configuration, MAC address, receive filtering, flow
 * control and statistics registers. There is no MIIMADDR, no MIIMCTRL, no
 * MIIMIND, and the IP has no MDC or MDIO pins.
 *
 * This differs from CoreTSE, which embeds the MII station in the MAC itself at
 * offsets 0x20 to 0x34.
 *
 * PHY access on CoreQSE designs is therefore performed by a SEPARATE
 * COREMDIO_APB IP, on a different bus and at a different base address. In the
 * 1 Gbps reference design:
 *
 *     COREQSE        0x70000000   AXI4-Lite, via COREAXI4INTERCONNECT
 *     COREMDIO_APB   0x60000000   APB, via CoreAPB3 slot 0
 *
 * The functions below exist so that the APPLICATION sees one coherent API, in
 * the style of TSE_phy_init() and TSE_read_phy_reg(), rather than having to
 * drive two drivers side by side. They are a presentation layer: each one
 * delegates to core_mdio.c, which owns the actual hardware. Nothing here makes
 * CoreQSE capable of MDIO on its own.
 *
 * MDIO cannot be added to CoreQSE in software. A management interface needs an
 * MDC output pin, a bidirectional MDIO pin with tristate control, a serial shift
 * register and a clock divider, all of which are RTL. Adding it would require
 * the IP itself to be modified and re-released.
 *
 * DESIGNS WITHOUT A PHY
 * ---------------------
 * The 2.5 Gbps design connects the transceiver to an SMA cable directly, so it
 * has no PHY and no COREMDIO_APB instance. In that case do not call
 * QSE_attach_mdio(); the QSE_phy_* functions then return QSE_ERR_NOT_SUPPORTED,
 * and link state must be inferred from the PCS counters using
 * QSE_pcs_get_sync_status() instead.
 *
 * PHY IMPLEMENTATION
 * ------------------
 * This example ships one PHY implementation:
 *
 *     phy.h             the interface
 *     null_phy.c        generic IEEE 802.3 Clause 22, no vendor registers
 *
 * That is sufficient to bring up a 1000BASE-T link on any conforming PHY. If a
 * vendor specific driver is needed, the CoreTSE driver ships tested
 * implementations that can be adapted; see phy.h for the procedure.
 *============================================================================*/

/***************************************************************************//**
  QSE_attach_mdio() associates a COREMDIO_APB instance and a PHY address with
  this CoreQSE instance, enabling the QSE_phy_* functions.

  Call after QSE_init() and before any other QSE_phy_* function. Designs without
  a PHY simply omit this call.

  @param this_qse
    Points to an initialized QSE_instance_t.

  @param mdio_base_addr
    Base address of the COREMDIO_APB instance, 0x60000000 in the 1 Gbps design.

  @param phy_addr
    PHY address on the MDIO bus, 0 to 31.

  @param mdio_prescaler
    MDIO clock prescaler. Only values 2 to 7 are decoded by the IP, giving
    dividers 3, 4, 5, 7, 10 and 14 respectively; other values select the fastest
    setting. MDC = PCLK / divider, and IEEE 802.3 limits MDC to 2.5 MHz.

  @return
    QSE_OK, or QSE_ERR_PARAM if this_qse is NULL, mdio_base_addr is 0 or
    phy_addr exceeds 31.

  @code
  QSE_init(&g_qse, COREQSE_BASE_ADDR);
  QSE_attach_mdio(&g_qse, MDIO_BASE_ADDR, BOARD_PHY_ADDR, BOARD_MDIO_PRESCALER);
  QSE_phy_init(&g_qse);
  @endcode
 */
uint8_t QSE_attach_mdio( QSE_instance_t * this_qse,
                         addr_t           mdio_base_addr,
                         uint8_t          phy_addr,
                         uint32_t         mdio_prescaler );

/***************************************************************************//**
  QSE_read_phy_reg() reads a PHY register over MDIO, Clause 22.

  Equivalent to TSE_read_phy_reg(). Unlike that function, which returns the
  value directly and cannot report failure, this one returns a status and
  delivers the data through a pointer, so an unresponsive PHY is distinguishable
  from a register that genuinely reads zero.

  @param this_qse
    Points to an instance that has had QSE_attach_mdio() called on it.

  @param reg_addr
    PHY register address, 0 to 31.

  @param data
    Receives the 16-bit value. Unmodified on failure.

  @return
    QSE_OK, QSE_ERR_PARAM, QSE_ERR_NOT_SUPPORTED if no MDIO is attached, or
    QSE_ERR on a transport timeout.
 */
uint8_t QSE_read_phy_reg( const QSE_instance_t * this_qse,
                          uint8_t                reg_addr,
                          uint16_t             * data );

/***************************************************************************//**
  QSE_write_phy_reg() writes a PHY register over MDIO, Clause 22.

  Equivalent to TSE_write_phy_reg().

  @param this_qse
    Points to an instance that has had QSE_attach_mdio() called on it.

  @param reg_addr
    PHY register address, 0 to 31.

  @param data
    Value to write.

  @return
    QSE_OK, QSE_ERR_PARAM, QSE_ERR_NOT_SUPPORTED if no MDIO is attached, or
    QSE_ERR on a transport timeout.
 */
uint8_t QSE_write_phy_reg( const QSE_instance_t * this_qse,
                           uint8_t                reg_addr,
                           uint16_t               data );

/***************************************************************************//**
  QSE_phy_verify_mdio() confirms that the MDIO interface returns register
  specific data, and should be called before trusting any PHY register.

  It reads four registers that cannot legitimately hold the same value. If they
  all match, the interface is returning a constant rather than real data.

  Not present in the CoreTSE driver, which only rejects an all-ones read. This
  check was added after a fault in which every register returned 0x0DC0: a
  plausible looking value whose bit pattern satisfied several individual field
  tests, so the PHY appeared healthy while carrying no information at all.

  @param this_qse
    Points to an instance that has had QSE_attach_mdio() called on it.

  @param readings
    Optional four element array receiving the raw CONTROL, STATUS, ID1 and ID2
    values so the caller can print them. May be NULL.

  @return
    QSE_OK if the registers differ and the identifier is plausible,
    QSE_ERR if the interface returns a constant or no PHY responded,
    QSE_ERR_NOT_SUPPORTED if no MDIO is attached.
 */
uint8_t QSE_phy_verify_mdio( const QSE_instance_t * this_qse,
                             uint16_t               readings[4] );

/***************************************************************************//**
  QSE_phy_get_id() reads the combined 32-bit PHY identifier, register 0x02 in
  the upper half and 0x03 in the lower.

  The upper 16 bits are the vendor OUI and indicate which PHY implementation to
  select in fpga_design_config.h:

      0x0141xxxx   Marvell     -> M88E1340_PHY
      0x0007xxxx   Microchip   -> VSC8575_PHY

  @param this_qse
    Points to an instance that has had QSE_attach_mdio() called on it.

  @param phy_id
    Receives the identifier.

  @return
    QSE_OK, QSE_ERR_PARAM, QSE_ERR_NOT_SUPPORTED or QSE_ERR.
 */
uint8_t QSE_phy_get_id( const QSE_instance_t * this_qse, uint32_t * phy_id );

/***************************************************************************//**
  QSE_phy_init() initializes the external PHY: releases reset and clears power
  down.

  Equivalent to TSE_phy_init(). Several PHY parts come out of reset with the
  power down bit SET, in which case the PHY answers MDIO but drives nothing onto
  the copper, which presents exactly like a disconnected cable.

  @param this_qse
    Points to an instance that has had QSE_attach_mdio() called on it.

  @return
    QSE_OK, QSE_ERR_NOT_SUPPORTED if no MDIO is attached, or QSE_ERR.
 */
uint8_t QSE_phy_init( const QSE_instance_t * this_qse );

/***************************************************************************//**
  QSE_phy_set_link_speed() configures which speeds and duplex modes the PHY
  advertises during auto-negotiation.

  Equivalent to TSE_phy_set_link_speed().

  @warning
  This writes BOTH register 0x04 and register 0x09. Register 0x00 only REQUESTS
  a speed; the abilities actually offered to the link partner live in 0x04 for
  10 and 100 Mbps and in 0x09 for 1000 Mbps. Configuring only 0x00 appears to
  work whenever the PHY happens to default to advertising gigabit, and silently
  negotiates 100 Mbps when it does not.

  @param this_qse
    Points to an instance that has had QSE_attach_mdio() called on it.

  @param speed_duplex_select
    Bitwise OR of PHY_ANEG_* values from phy.h. Use PHY_ANEG_1000M_FD_ONLY for
    the 1 Gbps design.

  @return
    QSE_OK, QSE_ERR_NOT_SUPPORTED if no MDIO is attached, or QSE_ERR.
 */
uint8_t QSE_phy_set_link_speed( const QSE_instance_t * this_qse,
                                uint32_t               speed_duplex_select );

/***************************************************************************//**
  QSE_phy_autonegotiate() enables and restarts auto-negotiation, then waits for
  the link.

  Equivalent to TSE_phy_autonegotiate(), with two differences: the wait is
  expressed in milliseconds rather than a raw iteration count, and a progress
  callback is supported.

  @warning
  1000BASE-T auto-negotiation takes 1 to 3 SECONDS. IEEE 802.3 defines a
  break_link_timer of 1.2 to 1.5 s before the page exchange even begins.
  Measured on the reference hardware the link negotiates in about 1735 ms.
  Allow at least 3000 ms.

  @param this_qse
    Points to an instance that has had QSE_attach_mdio() called on it.

  @param wait_ms
    Approximate maximum wait in milliseconds.

  @param progress_cb
    Called about every 250 ms with the current status. Printing it makes a wrong
    cable or socket obvious: the status word stays completely static when no link
    partner is present. May be NULL.

  @return
    QSE_OK if the link came up, QSE_ERR on timeout, QSE_ERR_NOT_SUPPORTED if no
    MDIO is attached.
 */
uint8_t QSE_phy_autonegotiate( const QSE_instance_t * this_qse,
                               uint32_t               wait_ms,
                               MDIO_link_progress_t   progress_cb );

/***************************************************************************//**
  QSE_phy_get_link_status() reports link state and, when up, the NEGOTIATED
  speed and duplex read from the PHY's vendor status register.

  Equivalent to TSE_phy_get_link_status().

  Reporting the negotiated values matters: it distinguishes "we asked for
  gigabit" from "we actually got gigabit". A link that silently falls back to
  100 Mbps would otherwise look like success.

  @param this_qse
    Points to an instance that has had QSE_attach_mdio() called on it.

  @param speed
    Receives the negotiated speed. May be NULL.

  @param fullduplex
    Receives PHY_FULL_DUPLEX or PHY_HALF_DUPLEX. May be NULL.

  @return
    PHY_LINK_UP or PHY_LINK_DOWN. Also PHY_LINK_DOWN when no MDIO is attached.
 */
uint8_t QSE_phy_get_link_status( const QSE_instance_t * this_qse,
                                 PHY_speed_t          * speed,
                                 uint8_t              * fullduplex );

/***************************************************************************//**
  QSE_phy_get_name() returns the name of the compiled PHY implementation, for
  example "M88E1340", "VSC8575" or "none". Useful in test logs so the build
  configuration is visible.

  @return
    Pointer to a static string. Never NULL.
 */
const char * QSE_phy_get_name( void );

#ifdef __cplusplus
}
#endif

#endif /* CORE_QSE_H_ */
