/*******************************************************************************
 * Copyright 2019 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file main.c
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief This example project demonstrates the usage of PolarFire User Crypto 
 * key tree services.
 * 
 */
#include <math.h>
#include<stdio.h>
#include<string.h>
#include "stdint.h"
#include "helper.h"
#include "hal/hal.h"
#include "fpga_design_config/fpga_design_config.h"
#include "drivers/fpga_ip/CoreUARTapb/core_uart_apb.h"
#include "drivers/fpga_ip/CoreSysServices_PF/core_sysservices_pf.h"
#include "drivers/fpga_ip/CoreSysServices_PF/coresysservicespf_regs.h"

#include "cal/calpolicy.h"
#include "cal/pk.h"
#include "cal/pkx.h"
#include "cal/pkxlib.h"
#include "cal/calini.h"
#include "cal/utils.h"
#include "cal/hash.h"
#include "cal/drbgf5200.h"
#include "cal/drbg.h"
#include "cal/nrbg.h"
#include "cal/sym.h"
#include "cal/shaf5200.h"
#include "cal/calenum.h"

#define DATA_LENGTH_32_BYTES  32

/******************************************************************************
 * CoreUARTapb instance data.
 *****************************************************************************/
UART_instance_t g_uart;

uint32_t g_user_crypto_base_addr = 0x62000000UL;

/*==============================================================================
  Messages displayed over the UART.
 */
const uint8_t g_greeting_msg[] =
"\r\n\r\n\
******************************************************************************\r\n\
************* PolarFire User Crypto Key Tree Example Project *****************\r\n\
******************************************************************************\r\n\
 This example project demonstrates the use of the User Crypto Key Tree service.\r\n";

static const uint8_t g_separator[] =
"\r\n\
------------------------------------------------------------------------------";
const uint8_t read_key_input[] = 
"\r\n Enter the 256 bit key to be used: \r\n";
const uint8_t read_op_type[] = 
"\r\n Enter user operation type:\r\n";
const uint8_t read_path_nonce_input[] = 
"\r\n Enter the 256 bit path nonce: \r\n";
const uint8_t path_nonce_option[] =
"\r\n\r\n\
--------------------------------------------------------------------------------\r\n\
1. Press 1 to enter 256 bit path nonce value \r\n\
2. press 2 to generate 256 bit path nonce \r\n ";



/*==============================================================================
  Key Tree service
 */
void keytree(void)
{
    uint8_t status = 0x00u;
    uint32_t key[8] = {0x00};
    uint8_t op_type = 0;
    uint8_t opt = 0 ;
    uint8_t path_nonce[NONCE_SERVICE_RESP_LEN] = {0x00};
    uint32_t g_key_out[100] = {0x00};
    
    /* Read the 32 bytes of key input data from UART terminal. */
    get_input_data((uint8_t*)&key[0], sizeof(key), read_key_input, 
                   sizeof(read_key_input));
    
    /* Read the 1 bytes of operation type data from UART terminal. */
    get_input_data(&op_type, sizeof(op_type), read_op_type, sizeof(read_op_type));
    
    /* As bPathSizeSel is set to SAT_TRUE, read the 16 bytes of path nonce 
     * input data from UART terminal. 
     */
    get_input_data(&opt, sizeof(opt), path_nonce_option, sizeof(path_nonce_option));

    if( opt != 1  )
    {

        SYS_init((unsigned int) CORESYS_SERV_BASE_ADDR);

        status = SYS_nonce_service(path_nonce, 0);
        if (status == SYS_SUCCESS)
        {
            UART_send(&g_uart, g_separator, sizeof(g_separator));
            UART_polled_tx_string(&g_uart,(uint8_t*)"\r\nGenerated Path_Nonce: \r\n");
            display_output((uint8_t*)&path_nonce[0], DATA_LENGTH_32_BYTES);
        }
        else
        {

            UART_send(&g_uart, g_separator, sizeof(g_separator));
            UART_polled_tx_string(&g_uart,(uint8_t*)"\r\n Error In path Nonce generation \r\n");
        }
    }
    else
    {

        get_input_data((uint8_t*)&path_nonce[0], sizeof(path_nonce), read_path_nonce_input,
                   sizeof(read_path_nonce_input));

        /* Change the endianness of data received from UART terminal. */
        CALWordReverse((uint32_t*)&path_nonce, (sizeof(path_nonce)/4));
        CALByteReverseWord((uint32_t*)&path_nonce, (sizeof(path_nonce)/4));
    }

    
    /* Change the endianness of data received from UART terminal. */
    CALWordReverse((uint32_t*)&key, (sizeof(key)/4));
    CALByteReverseWord((uint32_t*)&key, (sizeof(key)/4));
    
    /* Derive a secret key */
    status = CALKeyTree(1, key, op_type, (uint32_t*)&path_nonce[0], g_key_out);
    
    /* Display the secret key in hex format. */
    if(SATR_SUCCESS == status)
    {
        status = CALPKTrfRes(SAT_TRUE);
        UART_send(&g_uart, g_separator, sizeof(g_separator));
        UART_send(&g_uart, (const uint8_t *)"\r\n Key tree successful \
                   \r\n Secret key:\r\n", sizeof("\r\n Key tree successful \
                   \r\n Secret key:\r\n"));
        display_output((uint8_t*)&g_key_out[0], DATA_LENGTH_32_BYTES);
    }
    else
    {
        UART_send(&g_uart, g_separator, sizeof(g_separator));
        UART_send(&g_uart, (const uint8_t *)"\r\n Key tree service fail\r\n", 
                  sizeof("\r\n Key tree service fail\r\n"));
    }
}

/*==============================================================================
  Display greeting message when application is started.
 */
static void display_greeting(void)
{
    UART_send(&g_uart, g_greeting_msg,sizeof(g_greeting_msg));
}


/******************************************************************************
 * main function.
 *****************************************************************************/
int main( void )
{  

    uint8_t rx_buff[1];
    size_t rx_size = 0;
    /* Initialize CoreUARTapb with its base address, baud value, and line
       configuration. */
    UART_init(&g_uart, COREUARTAPB0_BASE_ADDR, BAUD_VALUE_115200, 
              (DATA_8_BITS | NO_PARITY));
 
    /* Initializes the Athena Processor */
    CALIni();
    
    /* Display greeting message. */
    display_greeting();
      
    /* Perform Key tree */

    for(;;)
    {

        keytree();
    }
}
