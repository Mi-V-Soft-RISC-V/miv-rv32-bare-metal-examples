/*******************************************************************************
 * Copyright 2019 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file main.c
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief Non Deterministic Random Bit Generator example for PolarFire user Crypto.
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

#define INVALID_USER_INPUT          -1
#define MAX_NB_OF_RANDOM_BYTES      8
#define EACH_BLK_SIZE               16

/******************************************************************************
 * CoreUARTapb instance data.
 *****************************************************************************/
UART_instance_t g_uart;

uint32_t g_user_crypto_base_addr = 0x62000000UL;
volatile uint8_t instantiate = 0;
uint8_t sc_nonce[NONCE_SERVICE_RESP_LEN] = {0x00};
uint8_t random_bytes[MAX_NB_OF_RANDOM_BYTES * EACH_BLK_SIZE] = {0x00};

/*==============================================================================
  Messages displayed over the Core UART.
 */
const uint8_t g_greeting_msg[] =
"\r\n\r\n\
******************************************************************************\r\n\
***** PolarFire User Crypto Random Bit Generator System Services Example *****\r\n\
******************************************************************************\r\n\
 This example project exercises the random bit generator system\r\n\
 services.\r\n";

const uint8_t g_options[] =
 "\r\n\r\n\
 - Press \"i\" to instantiate random numbers.\r\n\
  - Press \"g\" to generate random numbers.\r\n\
  - Press \"d\" to uninstantiated random numbers.\r\n\
------------------------------------------------------------------------------";
static const uint8_t g_separator[] =
"\r\n\
------------------------------------------------------------------------------";

/* Error Message.*/
const uint8_t g_errror_message_0[] = "\r\n Error - Operation failed because the DRBG is not instantiated.";
const uint8_t g_errror_message_1[] = "\r\n Error - Parameter is SAT_NULL.";
const uint8_t g_errror_message_2[] = "\r\n Error - Parameter exceeds capability of implementation..";
const uint8_t g_errror_message_3[] = "\r\n Error - NRBG fatal error during operation.";
const uint8_t g_errror_message_4[] = "\r\n Error - Function implementation not populated.";

/*==============================================================================
  Display greeting message when application is started.
 */
static void display_greeting(void)
{
    UART_send(&g_uart, g_greeting_msg,sizeof(g_greeting_msg));
}
static void display_options(void)
{
    UART_send(&g_uart, g_options,sizeof(g_options));
}

/*==============================================================================
  Display error message.
 */
static void display_error_info(uint8_t status)
{
    switch(status)
    {
        case SATR_FAIL:
            UART_send(&g_uart, g_errror_message_0, sizeof(g_errror_message_0));
            break;
            
        case SATR_BADPARAM:
            UART_send(&g_uart, g_errror_message_1, sizeof(g_errror_message_1));
            break;
            
        case SATR_BADLEN:
            UART_send(&g_uart, g_errror_message_2, sizeof(g_errror_message_2));
            break;
            
        case SATR_ROFATAL:
            UART_send(&g_uart, g_errror_message_3, sizeof(g_errror_message_3));
            break;
            
        case SATR_FNP:
            UART_send(&g_uart, g_errror_message_4, sizeof(g_errror_message_4));
            break;
            
        default:
            break;
    }
}

/*==============================================================================
  Retrieve a number typed by the user.
 */
static int32_t get_number_from_user(void)
{
    int32_t user_input = 0u;
    uint8_t rx_buff[1];
    uint8_t complete = 0u;
    size_t rx_size;
    
    while(!complete)
    {
        rx_size = UART_get_rx(&g_uart, rx_buff, sizeof(rx_buff));
        if(rx_size > 0)
        {
            UART_send(&g_uart, rx_buff, sizeof(rx_buff));
            if(ENTER == rx_buff[0])
            {
                complete = 1u;
            }
            else if((rx_buff[0] >= '0') && (rx_buff[0] <= '9'))
            {
                user_input = (user_input * 10u) + (rx_buff[0] - '0');
            }
            else
            {
                user_input = INVALID_USER_INPUT;
                complete = 1u;
                UART_send(&g_uart, rx_buff, sizeof(rx_buff));
            }
        }
    }
    return user_input;
}

/*==============================================================================
  Function used to instantiates the DRBG
 */
uint8_t nrbg_instantiate(void)
{
    static int instantiate = 0;
    SATR status;
    
    SYS_init((unsigned int) CORESYS_SERV_BASE_ADDR);
    
    status = SYS_nonce_service(sc_nonce, 0);
    if((SYS_SUCCESS == status) && (instantiate == 0))
    {
        status = CALDRBGInstantiate((uint32_t*)&sc_nonce[0], 8u, 0u, 0u, SATSYMKEYSIZE_AES128,
                                    2u, 10u, SAT_FALSE);
        if(status == SATR_SUCCESS) 
        {
            UART_send(&g_uart, (const uint8_t*)" DRBG instantiates successful.",
                      sizeof(" DRBG instantiates successful."));
        }
        else
        {
            UART_send(&g_uart, (const uint8_t*)" DRBG instantiates fail.",
                      sizeof(" DRBG instantiates fail."));
            display_error_info(status);
        }
    }   
    
    return status;
}

/*==============================================================================
  Generate Random number and display the status and generated number on UART 
  terminal.
 */
static void generate_random_bits(void)
{
    uint32_t nb_of_bytes;
    SATR status;
    const uint8_t read_no_of_byte_to_generate[] = 
    " Enter number of random block to generate( max 8 blocks): ";
    const uint8_t success_msg[] = 
    "\r\n DRBG generate successful. \r\n Generated Number: \r\n";
    const uint8_t fail_msg[] = 
    "\r\n DRBG generate failure.";
      
    for(uint16_t i = 0; i< MAX_NB_OF_RANDOM_BYTES * EACH_BLK_SIZE; i++)
    {
        random_bytes[i] = 0;
    }
    
    UART_send(&g_uart, read_no_of_byte_to_generate, 
              sizeof(read_no_of_byte_to_generate));
    nb_of_bytes = get_number_from_user();
    
    if((nb_of_bytes >= 1) && (nb_of_bytes <= MAX_NB_OF_RANDOM_BYTES))
    {
        /* Generate random bits */
        status =  CALDRBGGenerate(SAT_NULL, 0u, SAT_FALSE,
                                  (uint32_t*)&random_bytes[0] , nb_of_bytes);

        /* Display the generated number and status on UART terminal. */
        if(status == SATR_SUCCESS)
        {
            CALPKTrfRes(SAT_TRUE);
            UART_send(&g_uart, success_msg, sizeof(success_msg));
            /* Display number of number of 128-bit blocks*/
            display_output(random_bytes, nb_of_bytes * EACH_BLK_SIZE);
        }
        else
        {
            UART_send(&g_uart,fail_msg, sizeof(fail_msg));
            display_error_info(status);
        }
    }
    else
    {
        UART_send(&g_uart, (const uint8_t *)"\r\n\r\n Invalid entry.", 
                  sizeof("\r\n\r\n Invalid entry."));
    }
}

/*==============================================================================
  Function used to un-instantiates the DRBG
 */
void nrbg_uninstantiate(void)
{
    SATR status;
    
    /* Un-instantiate the currently instantiated DRBG*/
    status = CALDRBGUninstantiate();
    
    /* Display the status on UART terminal. */
    if(status == SATR_SUCCESS)
    {
        UART_send(&g_uart,(const uint8_t*)" DRBG uninstantiated successful.", 
                  sizeof(" DRBG uninstantiated successful."));
    }
    else
    {
        UART_send(&g_uart,(const uint8_t*)" DRBG uninstantiated failure.", 
                  sizeof(" DRBG uninstantiated failure."));
        display_error_info(status);
    }
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
    
    /* Initialize the core. */
    CALIni();
    
    /* Display greeting message. */
    display_greeting();

    /* display options */
    display_options();

    for(;;)
    {
        /* Start command line interface if any key is pressed. */
        rx_size = UART_get_rx(&g_uart, rx_buff, sizeof(rx_buff));
        if(rx_size > 0)
        {
            switch(rx_buff[0])
            {
                case 'i':
                    UART_send(&g_uart,
                              (const uint8_t*)"\r\n Selected: DRBG instantiates service. \r\n",
                              sizeof("\r\n Selected: DRBG instantiates service. \r\n"));
                    nrbg_instantiate();
                    UART_send(&g_uart, g_separator, sizeof(g_separator));
                    display_options();
                    break;
                    
                case 'g':
                    UART_send(&g_uart,(const uint8_t*)"\r\n Selected: Random data generate service. \r\n",
                              sizeof("\r\n Selected: Random data generate service. \r\n"));
                    generate_random_bits();
                    UART_send(&g_uart, g_separator, sizeof(g_separator));
                    display_options();
                    break;
                    
                case 'd':
                    UART_send(&g_uart,(const uint8_t*)"\r\n Selected: Uninstantiated DRBG service. \r\n",
                              sizeof("\r\n Selected: Uninstantiated DRBG service. \r\n"));
                    nrbg_uninstantiate();
                    UART_send(&g_uart, g_separator, sizeof(g_separator));
                    display_options();
                    break;

              default:
              break;
            }
        }
    }
}

