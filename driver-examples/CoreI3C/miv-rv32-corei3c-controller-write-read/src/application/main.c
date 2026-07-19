/*******************************************************************************
 * Copyright 2026 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file main.c
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief CoreI3C Controller Write/Read Example
 *
 * This example project demonstrates the use of the CoreI3C IP in controller
 * mode with a Mi-V soft processor. It exercises the following features:
 *
 *   - Controller initialization with SCL timing configuration
 *   - Dynamic Address Assignment (DAA) via ENTDAA
 *   - Write transfers to an I3C target
 *   - Read transfers from an I3C target
 *
 * The operation is controlled via a serial console at 57600 baud.
 *
 * Target hardware: PolarFire Evaluation Kit (MPF300T-EVAL-KIT)
 * Build configs:   miv-rv32-imc-debug / miv-rv32-imc-release
 */

#include <stdio.h>
#include <string.h>
#include "miv_rv32_hal/miv_rv32_hal.h"
#include "fpga_design_config/fpga_design_config.h"
#include "drivers/fpga_ip/CoreUARTapb/core_uart_apb.h"
#include "drivers/fpga_ip/CoreI3C/core_i3c.h"

/*==============================================================================
 * Hardware configuration
 *============================================================================*/

/*
 * Controller's own 7-bit dynamic address on the I3C bus.
 * Must not collide with any target device address.
 */
#define I3C_CTRL_SER_ADDRESS            (0x43u)

/*
 * Maximum number of I3C target devices to discover during DAA.
 */
#define I3C_MAX_TARGETS                 (8u)

/*
 * Maximum TX/RX payload size for UART-driven menu transfers.
 */
#define I3C_MAX_TRANSFER_BYTES          (32u)

/*
 *  SCL timing for 80 MHz ACLK:
 *  Adjust these values to match your Libero design clock frequency.
 */
#define SCL_OD_HIGH_CYCLES              (0x005Du)
#define SCL_OD_LOW_CYCLES               (0x005Du)
#define SCL_PP_HIGH_CYCLES              (0x0003u)
#define SCL_PP_LOW_CYCLES               (0x0004u)

/*
 * Response descriptor bit-field positions (MIPI I3C Basic spec, Table 51).
 */
#define RESP_ERR_STATUS_SHIFT           (28u)
#define RESP_ERR_STATUS_MASK            (0xFu)
#define RESP_TID_SHIFT                  (24u)
#define RESP_TID_MASK                   (0xFu)
#define RESP_DATA_LEN_MASK              (0xFFFFu)

/*===========================================================================*/
/* LPS22DFTR - MEMS Pressure / Temperature Sensor (STMicroelectronics)       */
/*===========================================================================*/

/*
 * Identification register
 */
#define LPS22DF_REG_WHO_AM_I            0x0Fu
#define LPS22DF_EXPECTED_WHO_AM_I       0xB4u

/*
 * Control / interface registers
 */
#define LPS22DF_REG_IF_CTRL             0x0Eu   /* Interface control    */
#define LPS22DF_REG_CTRL_REG1           0x10u   /* ODR, averaging       */
#define LPS22DF_REG_CTRL_REG2           0x11u   /* Boot, one-shot, etc  */
#define LPS22DF_REG_CTRL_REG3           0x12u   /* INT active-level, OD */
#define LPS22DF_REG_CTRL_REG4           0x13u   /* INT enable (DRDY)    */
#define LPS22DF_REG_I3C_IF_CTRL         0x19u

#define LPS22DFTR_DYN_ADDR          (0x0Au)   /* 7-bit dynamic address post-DAA */
#define LPS22DF_REG_IF_CTRL_VAL     (0x80u)   /* I3C interface enable            */
#define LPS22DF_REG_CTRL_REG1_VAL   (0x10u)   /* ODR = 10 Hz continuous          */
#define LPS22DF_REG_CTRL_REG4_VAL   (0x20u)   /* DRDY on INT                     */

/*
 * Status / interrupt registers
 */
#define LPS22DF_REG_INT_SOURCE          0x24u
#define LPS22DF_REG_STATUS              0x27u

/*
 * Data output registers (24-bit pressure, 16-bit temperature)
 */
#define LPS22DF_REG_PRESS_OUT_XL        0x28u
#define LPS22DF_REG_PRESS_OUT_L         0x29u
#define LPS22DF_REG_PRESS_OUT_H         0x2Au
#define LPS22DF_REG_TEMP_OUT_L          0x2Bu
#define LPS22DF_REG_TEMP_OUT_H          0x2Cu

/*
 * FIFO registers
 */
#define LPS22DF_REG_FIFO_STATUS1        0x25u
#define LPS22DF_REG_FIFO_STATUS2        0x26u
#define LPS22DF_REG_FIFO_DATA_PRESS_XL  0x78u

/*==============================================================================
 * Global driver instances
 *============================================================================*/
static UART_instance_t   g_uart;
static i3c_instance_t    g_i3c;

/*
 * DAA result: holds the discovered target list after I3C_do_daa().
 */
static i3c_address_t     g_targets[I3C_MAX_TARGETS];
static uint32_t          g_num_targets = 0u;

/*
 * Interrupt-driven flag set in the MSYS EI0 ISR when INT_O fires.
 * The main loop polls this flag and calls I3C_get_irq_status().
 */
static volatile uint8_t  g_i3c_irq_pending = 0u;

/*==============================================================================
 * UART helper – transmit a null-terminated string
 *============================================================================*/
static void uart_print(const char *s)
{
    UART_polled_tx_string(&g_uart, (const uint8_t *)s);
}

/*==============================================================================
 * UART helper – receive a single character (blocking)
 *============================================================================*/
static uint8_t uart_getc(void)
{
    uint8_t ch;
    while (UART_get_rx(&g_uart, &ch, 1u) == 0u)
    {
        ; /* spin */
    }
    return ch;
}

/*==============================================================================
 * Print a byte array as hex pairs over UART
 *============================================================================*/
static void uart_print_hex(const uint8_t *data, uint32_t len)
{
    static const char hex_chars[] = "0123456789ABCDEF";
    char pair[4];
    pair[2] = ' ';
    pair[3] = '\0';
    for (uint32_t i = 0u; i < len; i++)
    {
        pair[0] = hex_chars[(data[i] >> 4u) & 0xFu];
        pair[1] = hex_chars[data[i] & 0xFu];
        uart_print(pair);
    }
}

/*==============================================================================
 * Print an unsigned 32-bit value as decimal over UART
 *============================================================================*/
static void uart_print_u32(uint32_t val)
{
    char buf[12];
    int32_t i = (int32_t)(sizeof(buf)) - 1;
    buf[i] = '\0';
    if (val == 0u)
    {
        buf[--i] = '0';
    }
    else
    {
        while (val > 0u && i > 0)
        {
            buf[--i] = (char)('0' + (val % 10u));
            val /= 10u;
        }
    }
    uart_print(&buf[i]);
}

/*==============================================================================
 * Print the welcome banner and menu
 *============================================================================*/
static void print_banner(void)
{
    uart_print(
        "\r\n"
        " =========================================================\r\n"
        "        CoreI3C Controller Write/Read Example\r\n"
        "        MIV_RV32 / PolarFire Eval Kit\r\n"
        " =========================================================\r\n"
    );
}

static void print_menu(void)
{
    uart_print(
        "\r\n"
        " ---- I3C Bus Menu ----------------------------------------\r\n"
        "  1 - Run DAA (discover targets)\r\n"
        "  2 - Configure Slave\r\n"
        "  3 - Read from slave\r\n"
        " ----------------------------------------------------------\r\n"
        " Enter choice: "
    );
}

/*==============================================================================
 * Menu action 1: Run DAA
 *============================================================================*/
static void action_run_daa(void)
{
    uart_print("\r\n[DAA] Running ENTDAA...\r\n");

    i3c_status_t rc = I3C_do_daa(&g_i3c, g_targets, I3C_MAX_TARGETS);

    if (rc == I3C_SUCCESS)
    {
        /* Count populated entries */
        g_num_targets = 0u;
        for (uint32_t i = 0u; i < I3C_MAX_TARGETS; i++)
        {
            if (g_targets[i].dynamic_addr != 0u)
            {
                g_num_targets++;
            }
        }
        uart_print("[DAA] Complete.\r\n");

        for (uint32_t i = 0u; i < g_num_targets; i++)
        {
            uart_print("  [");
            uart_print_u32(i);
            uart_print("] DynAddr=0x");
            uart_print_hex(&g_targets[i].dynamic_addr, 1u);
            uart_print(" BCR=0x");
            uart_print_hex(&g_targets[i].bcr, 1u);
            uart_print(" DCR=0x");
            uart_print_hex(&g_targets[i].dcr, 1u);
            uart_print("\r\n");
        }
    }
    else if (rc == I3C_ERR_DAA_FAILED)
    {
        uart_print("[DAA] No devices responded to ENTDAA.\r\n");
        g_num_targets = 0u;
    }
    else if (rc == I3C_ERR_NOT_CTRL)
    {
        uart_print("[DAA] Error: not the active controller.\r\n");
    }
    else
    {
        uart_print("[DAA] Error code: ");
        uart_print_u32((uint32_t)rc);
        uart_print("\r\n");
    }
}

/*==============================================================================
 * Menu action 2: Configure slave
 *============================================================================*/
static void action_configure_slave(void)
{

    uart_print("\r\nConfiguring LPS22DFTR sensor...\r\n");

    /* Step 1a: Write IF_CTRL = 0x80 (enable I3C interface) */
    uint8_t wr_val = LPS22DF_REG_IF_CTRL_VAL;
    I3C_private_write(&g_i3c, LPS22DFTR_DCT_INDEX, LPS22DF_REG_IF_CTRL,
                          &wr_val, 1u);

    /* Step 1b: Write CTRL_REG1 = 0x10 (ODR = 10 Hz) */
    wr_val = LPS22DF_REG_CTRL_REG1_VAL;
    I3C_private_write(&g_i3c, LPS22DFTR_DCT_INDEX, LPS22DF_REG_CTRL_REG1,
                          &wr_val, 1u);

    /* Step 1c: Write CTRL_REG4 = 0x20 (DRDY on INT) */
    wr_val = LPS22DF_REG_CTRL_REG4_VAL;
    I3C_private_write(&g_i3c, LPS22DFTR_DCT_INDEX, LPS22DF_REG_CTRL_REG4,
                              &wr_val, 1u);

    I3C_drain_all_fifos(&g_i3c, 16u, 8u);

    uart_print("\r\nConfiguring LPS22DFTR sensor is done...\r\n");
}

/*==============================================================================
* Menu action 3: Read ChipID from slave
*============================================================================*/
static void action_read_from_slave(void)
{
    uint8_t data[1] ;
    data[0]= 0u;

    /* Step 2: Read WHO_AM_I register (0x0F) — write reg addr, then read 1 byte */
    I3C_private_read(&g_i3c, LPS22DFTR_DCT_INDEX, LPS22DF_REG_WHO_AM_I,
                                 data, 1u);

    uint8_t sub2_pass = ((data[0] & 0xFFu) == LPS22DF_EXPECTED_WHO_AM_I) ? 1u : 0u;
    if (sub2_pass == 1)
    {
        uart_print ("Chip ID reading is success \r\n");
        uart_print ("ChipID value is =0x");
        uart_print_hex(&data[0], 1u);
        uart_print("\r\n");
    }
    else
        uart_print ("Chip ID read is failing \r\n");

    I3C_drain_all_fifos(&g_i3c, 8u, 4u);

}

/*==============================================================================
 * main
 *============================================================================*/
int main(void)
{
    /*--------------------------------------------------------------------------
     * Initialize CoreUARTapb at 57600 baud, 8N1.
     *------------------------------------------------------------------------*/
    UART_init(&g_uart,
              COREUARTAPB0_BASE_ADDR,
              BAUD_VALUE_57600,
              (DATA_8_BITS | NO_PARITY));

    print_banner();

    /*--------------------------------------------------------------------------
     * Initialize the CoreI3C controller.
     *
     * SCL timing: configured for 50 MHz ACLK. Adjust SCL_*_CYCLES constants
     * in this file to match your Libero design clock frequency.
     *------------------------------------------------------------------------*/
    i3c_cfg_t cfg = {
        .scl_od_high_cycles = SCL_OD_HIGH_CYCLES,
        .scl_od_low_cycles  = SCL_OD_LOW_CYCLES,
        .scl_pp_high_cycles = SCL_PP_HIGH_CYCLES,
        .scl_pp_low_cycles  = SCL_PP_LOW_CYCLES,
        .i2c_dev_present    = 0u,  /* No legacy I2C devices */
        .iba_include        = 1u,  /* Include broadcast address */
        .halt_on_timeout    = 1u,  /* Halt on command timeout */
        .ibi_capable        = 1u   /* IBI_CAP_EN set at synthesis */
    };

    i3c_status_t rc = I3C_init_controller(&g_i3c,
                                          COREI3C_BASE_ADDR,
                                          I3C_CTRL_SER_ADDRESS,
                                          &cfg);

    if (rc != I3C_SUCCESS)
    {
        uart_print("[INIT] CoreI3C initialization failed! Error: ");
        uart_print_u32((uint32_t)rc);
        uart_print("\r\n");
        while (1) { ; }
    }

    uart_print("[INIT] CoreI3C initialized OK.\r\n");

    /*--------------------------------------------------------------------------
     * Main menu loop.
     *------------------------------------------------------------------------*/
    while (1)
    {
        /*----------------------------------------------------------------------
         * Print menu and wait for user input.
         *--------------------------------------------------------------------*/
        print_menu();
        uint8_t choice = uart_getc();

        /* Echo choice */
        UART_send(&g_uart, &choice, 1u);
        uart_print("\r\n");

        switch (choice)
        {
            case '1':
                action_run_daa();
                break;

            case '2':
                action_configure_slave();
                break;

            case '3':
                action_read_from_slave();
                break;

            default:
                uart_print("\r\n[ERROR] Invalid selection. Enter 1-3 \r\n");
                break;
        }
    }

    /* Never reached */
    return 0;
}
