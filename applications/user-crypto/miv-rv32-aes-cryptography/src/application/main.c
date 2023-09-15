/*******************************************************************************
 * Copyright 2019 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file main.c
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief This Example Project demonstrates the usage of PolarFire User Crypto
 * services for message encryption and decryption using AES algorithm.
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

#define KEY_SIZE 32

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
***************************************************************************\r\n\
******* PolarFire User Crypto AES Cryptography Service Example Project ****\r\n\
***************************************************************************\r\n\
 This example project demonstrates the use of the PolarFire User Crypto \r\n\
 Cryptography Services. The following User Athena services are demonstrated:\r\n\
  1 - AES-256 encryption.\r\n\
  2 - AES-256 decryption.\r\n";

const uint8_t g_select_operation_msg[] =
"\r\n\
------------------------------------------------------------------------------\r\n\
 Select the Cryptographic operation to perform:\r\n\
    Press key '1' to perform AES-256 encryption \r\n\
    Press key '2' to perform AES-256 decryption \r\n\
------------------------------------------------------------------------------\r\n";
static const uint8_t aes_encrypt_select_msg[] = 
"\r\n Selected AES-256 encryption service. \r\n";
static const uint8_t aes_decrypt_select_msg[] = 
"\r\n Selected AES-256 decryption service. \r\n";
static const uint8_t g_separator[] =
"\r\n\
------------------------------------------------------------------------------";
static const uint8_t read_key_msg[] =
"\r\n Enter the 256-bit/32-byte key: \r\n";
static const uint8_t read_data_msg[] =
"\r\n Enter the 16 bytes of input data to encrypt:\r\n";
static const uint8_t read_iv_msg[] =
"\r\n Enter the 16 bytes initialization vector(IV):\r\n";
static const uint8_t read_dma_enable_ip[] =
"\r\n Enter 1 to perform Symmetric encryption with DMA or \r\n\
   0 to perform Symmetric encryption without DMA: \r\n";
static const uint8_t read_encrypted_data_msg[] =
"\r\n Enter the 16 bytes of cipher text to decrypt: \r\n";

/*==============================================================================
  Global Variables.
 */
static uint8_t  __attribute__ ((section (".crypto_data"))) g_cipher_text[KEY_SIZE/2] = {0x00};
static uint8_t  __attribute__ ((section (".crypto_data"))) g_key_256bit[KEY_SIZE] = {0x00};
static uint8_t  __attribute__ ((section (".crypto_data"))) g_iv[KEY_SIZE/2] = {0x00};
static uint8_t  __attribute__ ((section (".crypto_data"))) g_plain_text[KEY_SIZE] = {0x00};

/*==============================================================================
  Clear AES global variable.
 */
static void clear_aes_var(void)
{
    uint16_t var = 0;
    
    for(var = 0; var < sizeof(g_key_256bit); var++)
    {
        g_key_256bit[var] = 0;
    }
    for(var = 0; var < sizeof(g_iv); var++)
    {
        g_iv[var] = 0;
    }
    for(var = 0; var < sizeof(g_plain_text); var++)
    {
        g_plain_text[var] = 0;
    }
    for(var = 0; var < sizeof(g_cipher_text); var++)
    {
        g_cipher_text[var] = 0;
    }
}

/*==============================================================================
  AES-256 encryption.
 */
static void aes256_encryption(void)
{
    uint8_t status = 0u;
    uint8_t use_dma = 0;
    uint16_t msg_len = 0;
       
    /* Clear input and output variables */
    clear_aes_var();
    
    /* Get key. */
    get_input_data(&g_key_256bit[0], sizeof(g_key_256bit), read_key_msg, 
                   sizeof(read_key_msg));
    
    /* Get Initialization Vector value */  
    get_input_data(&g_iv[0], sizeof(g_iv), read_iv_msg, sizeof(read_iv_msg));
    
    /* Read input data to be encrypt. */
    msg_len = get_input_data(&g_plain_text[0], sizeof(g_plain_text), 
                             read_data_msg, sizeof(read_data_msg));
    
    /* Use dma or not */  
    use_dma = enable_dma(read_dma_enable_ip, sizeof(read_dma_enable_ip));
    
    if(use_dma != 1)
    {
        /* Without DMA */
        status = CALSymEncrypt(SATSYMTYPE_AES256, (uint32_t *)&g_key_256bit[0],
                               SATSYMMODE_ECB, g_iv, SAT_TRUE, g_plain_text, 
                               g_cipher_text , msg_len);
    }
    else
    {
        /* With DMA */
        status = CALSymEncryptDMA(SATSYMTYPE_AES256, (uint32_t *)&g_key_256bit[0],
                                  SATSYMMODE_ECB, g_iv, SAT_TRUE, 
                                  g_plain_text, g_cipher_text , msg_len, 
                                  X52CCR_DEFAULT);
    }
       
    /* Display the encrypted data. i.e. Cipher text*/
    if(SATR_SUCCESS == status)
    {
        CALPKTrfRes(SAT_TRUE);
        UART_send(&g_uart, g_separator, sizeof(g_separator));
        UART_send(&g_uart, (const uint8_t *)"\r\n Encrypted data:\r\n", 
                  sizeof("\r\n Encrypted data:\r\n"));
        display_output(g_cipher_text, sizeof(g_cipher_text));
    }
    else
    {
        UART_send(&g_uart, g_separator, sizeof(g_separator));
        UART_send(&g_uart, (const uint8_t *)"\r\n Error\r\n", 
                  sizeof("\r\n Error\r\n"));
    }
}

/*==============================================================================
  AES-256 decryption.
 */
static void aes256_decryption(void)
{
    uint8_t status = 0u;
    uint8_t use_dma = 0;
    uint16_t cipher_len = 0;

         
    /* Clear input and output variables. */
    clear_aes_var();
    
    /* Get Key from the user to perform Decryption. Note this key should be 
     * same as that used for message encryption. */
    get_input_data(&g_key_256bit[0], sizeof(g_key_256bit), read_key_msg, 
                   sizeof(read_key_msg));
        
    /* Get Initialization Vector value */  
    get_input_data(&g_iv[0], sizeof(g_iv), read_iv_msg, sizeof(read_iv_msg));
    
    /* Read encrypted data i.e 16 bytes of cipher text to be decrypt. */
    cipher_len = get_input_data(&g_cipher_text[0], sizeof(g_cipher_text), 
                   read_encrypted_data_msg, sizeof(read_encrypted_data_msg));
    
    /* Use dma or not */  
    use_dma = enable_dma(read_dma_enable_ip, sizeof(read_dma_enable_ip));
    
    if(use_dma != 1)
    {
        status = CALSymDecrypt(SATSYMTYPE_AES256, (uint32_t*)&g_key_256bit[0], 
                               SATSYMMODE_ECB, g_iv, SAT_TRUE, g_cipher_text, 
                               g_plain_text, cipher_len);
    }
    else
    {
        status = CALSymDecryptDMA(SATSYMTYPE_AES256, (uint32_t*)&g_key_256bit[0], 
                                  SATSYMMODE_ECB, g_iv, SAT_TRUE, g_cipher_text, 
                                  g_plain_text, cipher_len, 
                                  X52CCR_DEFAULT);
    }
      
    /* Display the Decrypted data i.e. Plain text. */
    if(SATR_SUCCESS == status)
    {
        CALPKTrfRes(SAT_TRUE);
        UART_send(&g_uart, g_separator, sizeof(g_separator));
        UART_send(&g_uart, (const uint8_t *)"\r\n Decrypted data:\r\n",
                  sizeof("\r\n Decrypted data:\r\n"));  
        display_output(g_plain_text, sizeof(g_plain_text));
    }
    else
    {
        UART_send(&g_uart, g_separator, sizeof(g_separator));
        UART_send(&g_uart, (const uint8_t *)"\r\n Error\r\n", 
                  sizeof("\r\n Error\r\n"));
    }
}

/*==============================================================================
  Display greeting message when application is started.
 */
static void display_greeting(void)
{
    UART_send(&g_uart, g_greeting_msg,sizeof(g_greeting_msg));
}

/*==============================================================================
  Display the choice of cryptographic operation to perform.
 */
static void display_operation_choices(void)
{
    UART_send(&g_uart, g_select_operation_msg, sizeof(g_select_operation_msg));
}

/*==============================================================================
  Display the Option to continue.
 */
static void display_option(void)
{
    uint8_t rx_size;
    uint8_t rx_buff[1];
    
    UART_send(&g_uart, g_separator, sizeof(g_separator));
    UART_send(&g_uart, (const uint8_t*)"\r\n Press any key to continue.\r\n",
              sizeof("\r\n Press any key to continue.\r\n"));
    do
    {
        rx_size = UART_get_rx(&g_uart, rx_buff, sizeof(rx_buff));
    } while(0u == rx_size);
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
    
    /* Initializes the Athena Processor. */
    CALIni();
    
    /* Display greeting message. */
    display_greeting();

    /* Select cryptographic operation */
    display_operation_choices();

    for(;;)
    {
        /* Read inputs from UART terminal. */
        rx_size = UART_get_rx(&g_uart, rx_buff, sizeof(rx_buff));
        if(rx_size > 0)
        {
            switch(rx_buff[0])
            {
                case '1':
                    /* Perform encryption using AES-256 */
                    UART_send(&g_uart, aes_encrypt_select_msg, 
                              sizeof(aes_encrypt_select_msg));
                    aes256_encryption();
                    display_option();
                    display_operation_choices();
                break;

                case '2':
                    /* Perform decryption using AES-256 */
                    UART_send(&g_uart, aes_decrypt_select_msg, 
                              sizeof(aes_decrypt_select_msg));
                    aes256_decryption();
                    display_option();
                    display_operation_choices();
                break;

                default:
                break;
            }
        }
    }
}
