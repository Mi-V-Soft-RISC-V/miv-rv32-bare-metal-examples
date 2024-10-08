/*******************************************************************************
 * Copyright 2024 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file main.c
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief This example project demonstrates the use of CoreAXI4ProtoConverter IP
 * for stream data transfers.
 *
 */

#include <stdio.h>
#include "miv_rv32_hal/miv_rv32_hal.h"
#include "fpga_design_config/fpga_design_config.h"
#include "drivers/fpga_ip/CoreUARTapb/core_uart_apb.h"
#include "drivers/fpga_ip/CoreAXI4ProtoConv/core_axi4protoconv.h"

UART_instance_t   g_uart;
PCDMA_instance_t  g_pcdma;

#define DDR_BASE_ADDR                               0x80010000u
#define DDR_MID_ADDR                                0x80090000u
#define MM2S_CMD_ID                                 0xa6
#define S2MM_CMD_ID                                 0xb6
#define TRANSFER_SIZE_BYTES                         1000u
#define NO_OF_DESCRIPTOR                            5u
#define BUFF_SIZE                                   100u
#define CMD_ID_SHIFT                                16u
#define PASS                                        0u
#define FAIL                                        1u

uint8_t *ddr_memory_sm = (uint8_t *)DDR_BASE_ADDR;
uint8_t *ddr_memory_ms = (uint8_t *)DDR_MID_ADDR;
char g_buffer[BUFF_SIZE] = {0};
uint16_t g_cmd_id_m2s[BUFF_SIZE] = {0};
uint16_t g_cmd_id_s2m[BUFF_SIZE] = {0};
uint8_t g_index_m2s = 0u;
uint8_t g_index_s2m = 0u;

/******************************************************************************
 * Instruction message. This message will be transmitted to the UART terminal
 * when the program starts.
 *****************************************************************************/
const uint8_t g_message[] =
"\r\n *************************************************************************\
\r\n\r\n      CoreAXI4ProtoConverter Stream Example Project         \r\n\r\n\
 **************************************************************************\r\n\
\r\n\n This example project demonstrates the use of CoreAXI4ProtoConv driver\r\n\
 with Mi-V soft processor.\r\n";

const uint8_t selection_message[] =
"\r\n\n Press '1' to perform AXI4ProtoConverter Single Descriptor Stream Transfer!\r\n\
\r\n Press '2' to perform AXI4ProtoConverter Multiple Descriptor Stream Transfer!\r\n";

/* Functions*/
static void pcdma_single_desc_stream_transfer(void);
static void pcdma_multi_desc_stream_transfer(void);
static void memory_init(uint8_t *, uint32_t);
static uint8_t verify_transfer(uint8_t *, uint8_t *, uint32_t);

/***************************************************************************//**
 * main function.
 */
int main(void)
{
    uint8_t rx_size = 0;
    uint8_t rx_buff[1];

    /**************************************************************************
     * Initialize CoreUARTapb with its base address, baud value, and line
     * configuration.
     *************************************************************************/
    UART_init( &g_uart, COREUARTAPB0_BASE_ADDR,
            BAUD_VALUE_115200, (DATA_8_BITS | NO_PARITY) );

    /**************************************************************************
     * Send the Greeting message.
     *************************************************************************/
    UART_polled_tx_string( &g_uart, g_message );

    UART_polled_tx_string( &g_uart, selection_message );

    PCDMA_init(&g_pcdma, COREAXI4PROTOCONV_BASE_ADDR);

    memory_init(ddr_memory_sm, TRANSFER_SIZE_BYTES);

    /**************************************************************************
     * Enable MSYS external interrupt 0
     *************************************************************************/
    MRV_enable_local_irq(MRV32_MSYS_EIE0_IRQn);

    PCDMA_MM2S_enable_irq(&g_pcdma, MM2S_IE_DONE_IRQ);

    /**************************************************************************
     * Enable MSYS external interrupt 1
     *************************************************************************/
    MRV_enable_local_irq(MRV32_MSYS_EIE1_IRQn);

    PCDMA_S2MM_enable_irq(&g_pcdma, S2MM_IE_DONE_IRQ);

    /**************************************************************************
     * Enables all interrupts
     *************************************************************************/
    HAL_enable_interrupts();

    while(1)
    {
        rx_size = UART_get_rx( &g_uart, rx_buff, sizeof(rx_buff));
        if (rx_size > 0u)
        {
            switch (rx_buff[0])
            {
                case '1':
                    pcdma_single_desc_stream_transfer();
                    UART_polled_tx_string( &g_uart, selection_message );
                    break;

                case '2':
                    pcdma_multi_desc_stream_transfer();
                    UART_polled_tx_string( &g_uart, selection_message );
                    break;

                default:
                    UART_polled_tx_string( &g_uart, "\r\n\r\n Invalid Option "
                                                            "Selected!!\r\n" );
                    UART_polled_tx_string( &g_uart, selection_message );
                    break;
            }
            rx_size = 0u;
        }
    }
}

void pcdma_single_desc_stream_transfer(void)
{
    uint8_t status = FAIL;
    UART_polled_tx_string( &g_uart, "\r\n\n ---------------------------------");
    UART_polled_tx_string( &g_uart, "\r\n\n Selected Single Descriptor Stream "
                                                             "Transfer.... \n");

    /* Set the MM2S transfer size, 64-bit address, command id, and burst type.*/
    PCDMA_MM2S_configure(&g_pcdma, TRANSFER_SIZE_BYTES, DDR_BASE_ADDR,
                                           MM2S_CMD_ID, PCDMA_BURST_TYPE_INCR);

    /* Initiate the data transfer */
    PCDMA_MM2S_start_transfer(&g_pcdma);

    /* Set the S2MM transfer size, 64-bit address, command id, and burst type.*/
    PCDMA_S2MM_configure(&g_pcdma, TRANSFER_SIZE_BYTES, DDR_MID_ADDR,
                                           S2MM_CMD_ID, PCDMA_BURST_TYPE_INCR);

    /* Initiate the data transfer */
    PCDMA_S2MM_start_transfer(&g_pcdma);

    status = verify_transfer(ddr_memory_sm, ddr_memory_ms, TRANSFER_SIZE_BYTES);

    if(status)
    {
        UART_polled_tx_string( &g_uart, "\r\n\n Single Descriptor Stream Data "
                                             "Transfer Failed: Data mismatch");
    }
    else
    {
        UART_polled_tx_string( &g_uart, "\r\n\n Single Descriptor Stream Data "
                                          "Transfer Successful: Data matches");
    }
    UART_polled_tx_string( &g_uart, "\r\n\n ---------------------------------");
}

void pcdma_multi_desc_stream_transfer(void)
{
    uint8_t status = FAIL;
    UART_polled_tx_string( &g_uart, "\r\n\n ---------------------------------");
    UART_polled_tx_string( &g_uart, "\r\n\n Selected Multiple Descriptor "
                                                    "Stream Transfer..... \n");

    for(uint8_t d = 1; d < NO_OF_DESCRIPTOR; d++)
    {
    /* Set the MM2S transfer size, 64-bit address, command id, and burst type.*/
        PCDMA_MM2S_configure(&g_pcdma, (d * d), DDR_BASE_ADDR,
                                     (MM2S_CMD_ID + d), PCDMA_BURST_TYPE_INCR);

        /* Initiate the data transfer */
        PCDMA_MM2S_start_transfer(&g_pcdma);

    /* Set the S2MM transfer size, 64-bit address, command id, and burst type.*/
        PCDMA_S2MM_configure(&g_pcdma, (d * d), DDR_MID_ADDR,
                                     (S2MM_CMD_ID + d), PCDMA_BURST_TYPE_INCR);

        /* Initiate the data transfer */
        PCDMA_S2MM_start_transfer(&g_pcdma);

        if (((g_cmd_id_m2s[0] >= (MM2S_CMD_ID + 1)) &&
            (g_cmd_id_m2s[0] <= (MM2S_CMD_ID + d))) &&
            ((g_cmd_id_s2m[0] >= (S2MM_CMD_ID + 1)) &&
           (g_cmd_id_s2m[0] <= (S2MM_CMD_ID + d))))
        {
            UART_polled_tx_string(&g_uart, "\r\n MM2S & S2MM External "
                                                     "Interrupts Occurred\n");
            sprintf(g_buffer,"\r\n MM2S CMD ID: %x \n\r S2MM CMD ID: %x\n",
                                             g_cmd_id_m2s[0], g_cmd_id_s2m[0]);
            UART_polled_tx_string(&g_uart, g_buffer);
        }
        else
        {
            UART_polled_tx_string(&g_uart, "\r\n\r\n Unknown CMD ID of MM2S "
                                                               "and S2MM\r\n");
            sprintf(g_buffer, " MM2S CMD ID: %x \n\r S2MM CMD ID: %x\n",
                                             g_cmd_id_m2s[0], g_cmd_id_s2m[0]);
            UART_polled_tx_string(&g_uart, g_buffer);
        }
        /* Verify data transfer */
        status = verify_transfer(ddr_memory_sm, ddr_memory_ms, (d * d));
    }

    if(status)
    {
        UART_polled_tx_string( &g_uart, "\r\n\n Multiple Descriptor Stream Data "
                                             "Transfer Failed: Data mismatch");
    }
    else
    {
        UART_polled_tx_string( &g_uart, "\r\n\n Multiple Descriptor Stream Data "
                                           "Transfer Successful: Data matches");
    }
    UART_polled_tx_string( &g_uart, "\r\n\n ---------------------------------");
}

void memory_init(uint8_t *source_addr, uint32_t size)
{
    for(uint32_t id = 0; id < size; id++)
    {
        *source_addr = id;
        source_addr++;
    }
}
/* Compare the data in source and destination memory addresses */
static
uint8_t verify_transfer(uint8_t *source_addr, uint8_t *dest_addr, uint32_t size)
{
    uint8_t error = PASS;
    uint32_t index = 0;
    while(size != 0)
    {
        if(source_addr[index] != dest_addr[index])
        {
            error = FAIL;
            break;
        }
        index++;
        size--;
    }
    return error;
}

/******************************************************************************
 * Interrupt Service Routine (ISR) for the MSYS external interrupt 0,
 * which is connected to MM2S_INT
 *****************************************************************************/
void MSYS_EI0_IRQHandler(void)
{
    uint32_t intrsrc_value_m2s;

    intrsrc_value_m2s = PCDMA_MM2S_get_int_src(&g_pcdma);
    g_cmd_id_m2s[g_index_m2s] = intrsrc_value_m2s >> CMD_ID_SHIFT;

    PCDMA_MM2S_clr_int_src(&g_pcdma, MM2S_IE_DONE_IRQ);
}

/******************************************************************************
 * Interrupt Service Routine (ISR) for the MSYS external interrupt 1,
 * which is connected to S2MM_INT
 *****************************************************************************/
void MSYS_EI1_IRQHandler(void)
{
    uint32_t intrsrc_value_s2m;

    intrsrc_value_s2m = PCDMA_S2MM_get_int_src(&g_pcdma);
    g_cmd_id_s2m[g_index_s2m] = intrsrc_value_s2m >> CMD_ID_SHIFT;

    PCDMA_S2MM_clr_int_src(&g_pcdma, S2MM_IE_DONE_IRQ);
}
