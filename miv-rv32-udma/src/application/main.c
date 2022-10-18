/*******************************************************************************
 * Copyright 2022 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * MIV uDMA Example project
 *
 * This project will demonstrate the use of MIV uDMA Bare-metal software driver.
 * A 32-bit data is written into the source address and the uDMA controller
 * will copy the data from src_addr to dest_addr.
 * A verify function will check if the correct data is read or not, and based
 * on the response, UART messages will be printed.
 *
 * Please refer README.md in the root folder of this project for more details.
 */

#include <stdio.h>
#include "miv_rv32_hal/miv_rv32_hal.h"
#include "fpga_design_config/fpga_design_config.h"
#include "drivers/fpga_ip/CoreUARTapb/core_uart_apb.h"
#include "drivers/fpga_ip/miv_udma/miv_udma.h"

/******************************************************************************
 * Peripheral instance data.
 *****************************************************************************/
UART_instance_t g_uart;

miv_udma_instance_t g_miv_ess_udma;

/* Local function to verify the uDMA transaction */
static uint8_t
verify_write
(
    uint32_t* write_buff,
    uint32_t* read_buff,
    uint16_t size
);

/*
 *  Local Constants to hold the source and destination address
 *  Application developer should update these values as per requirements.
 */
#define MIV_UDMA_TRANSFER_SRC_ADDR              0x89000000u
#define MIV_UDMA_TRANSFER_DEST_ADDR             0x8A000000u

/*==============================================================================
 * Messages displayed over the UART.
 */
const uint8_t g_greeting_msg[] =
"\r\n\r\n\t\t **** PolarFire MiV uDMA Example ****\n\n\n\r\
This application will demonstrate the use of MIV uDMA bare-metal driver.\r\n\n\
It will copy the data from MIV_UDMA_TRANSFER_SRC_ADDR to MIV_UDMA_TRANSFER_DEST_ADDR.\r\n\n\
";

static void display_greeting(void);

uint32_t g_udma_status = 0u;

/*==============================================================================
 * Display greeting message when application is started.
 */
static void display_greeting(void)
{
    UART_polled_tx_string(&g_uart, g_greeting_msg);
}

/* MIV RV32 External interrupt handlers for MIV_ESS uDMA */
void MSYS_EI1_IRQHandler()
{
    g_udma_status = MIV_uDMA_read_status(&g_miv_ess_udma);

    MIV_uDMA_reset(&g_miv_ess_udma);
}

/******************************************************************************
 * main function.
 *****************************************************************************/
void main(void)
{
    size_t rx_size;
    uint8_t rx_buff[1] = {0x00};
    uint32_t counter = 0u;

    /* Initialize CoreUARTapb with its base address, baud value, and line
     * configuration.
     */
    UART_init(&g_uart, COREUARTAPB0_BASE_ADDR, BAUD_VALUE_115200, (DATA_8_BITS | NO_PARITY));

    display_greeting();

    MRV_enable_local_irq(MRV32_MSYS_EIE1_IRQn);

    HAL_enable_interrupts();

    uint32_t *src_addr = (uint32_t*)MIV_UDMA_TRANSFER_SRC_ADDR;
    uint32_t *dest_addr = (uint32_t*)MIV_UDMA_TRANSFER_DEST_ADDR;

    /* Write some data in the source memory */
    *src_addr = 0xAFAFAFAF;

    MIV_uDMA_init(&g_miv_ess_udma, MIV_ESS_uDMA_BASE_ADDR);

    MIV_uDMA_config(&g_miv_ess_udma, MIV_UDMA_TRANSFER_SRC_ADDR ,
                    MIV_UDMA_TRANSFER_DEST_ADDR, 0x8, MIV_uDMA_CTRL_IRQ_CONFIG);

    MIV_uDMA_start(&g_miv_ess_udma);

    while(1)
    {
        if (g_udma_status == MIV_uDMA_STATUS_BUSY)
        {
            UART_polled_tx_string(&g_uart,
            (const uint8_t *)"\r\n uDMA busy");

            g_udma_status = 0xff;
        }
        else if (g_udma_status == MIV_uDMA_STATUS_ERROR)
        {
            UART_polled_tx_string(&g_uart,
            (const uint8_t *)"\r\n uDMA error");
            g_udma_status = 0xff;
        }
        else if (g_udma_status == 0u)
        {
            counter = verify_write(src_addr, dest_addr, 8u);
            if (counter == 0u)
            {
                UART_polled_tx_string(&g_uart,
                        (const uint8_t *)"\r\n uDMA Success");
                g_udma_status = 0xff;
            }
            else
            {
                UART_polled_tx_string(&g_uart,
                        (const uint8_t *)"\r\n uDMA wrong data");
                g_udma_status = 0xff;
            }
        }
    }
}

/**************************************************************************//**
 * Read the data from destination memory and compare the same with source
 * address contents.
 */
static uint8_t verify_write(uint32_t* write_buff, uint32_t* read_buff, uint16_t size)
{
    uint8_t error = 0U;
    uint16_t index = 0U;

    while(size != 0U)
    {
        if(write_buff[index] != read_buff[index])
        {
            error = 1U;
            break;
        }
        index++;
        size--;
    }

    return error;
}
