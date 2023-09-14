/*******************************************************************************
 * Copyright 2019 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file main.c
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief Application demonstrating the CCM cryptography service. Please see the
 * Readme.md for more details.
 *
 */
#include "cal/calini.h"
#include "drivers/fpga_ip/CoreUARTapb/core_uart_apb.h"
#include "fpga_design_config/fpga_design_config.h"
#include "miv_rv32_hal/miv_rv32_hal.h"
#include "helper.h"
#include "ccm.h"

/******************************************************************************
 * User Crypto base address. This will be used in config_user.h in CAL.
 *****************************************************************************/
uint32_t g_user_crypto_base_addr = 0x62000000UL;

/******************************************************************************
 * CoreUARTapb instance data.
 *****************************************************************************/
UART_instance_t g_uart;

/*==============================================================================
  Messages displayed over the UART.
 */
const uint8_t g_greeting_msg[] =
"\r\n\r\n\
******************************************************************************\r\n\
************* PolarFire User Crypto CCM Example Project **********************\r\n\
******************************************************************************\r\n\
 This example project demonstrates the use of the PolarFire User Crypto \r\n\
 CCM service using AES cipher Services. The following CCM services \r\n\
 are demonstrated:\r\n\
  1 - Encryption and authenticated.\r\n\
  2 - Decryption and authenticated.\r\n";

const uint8_t g_select_operation_msg[] =
"\r\n\
------------------------------------------------------------------------------\r\n\
 Select the CCM operation to perform:\r\n\
    Press key '1' to perform Encryption and Authenticated \r\n\
    Press key '2' to perform Decryption and Authenticated \r\n\
------------------------------------------------------------------------------\r\n";

static const uint8_t aes_encrypt_select_msg[] =
"\r\n Selected Encryption and Authenticated using CCM. \r\n";

static const uint8_t aes_decrypt_select_msg[] =
"\r\n Selected Decryption and Authenticated using CCM. \r\n";

static const uint8_t g_separator[] =
"\r\n\
------------------------------------------------------------------------------";

static const uint8_t read_key_msg[] =
"\r\n Enter the 128-bit/16-byte key: \r\n";

static const uint8_t read_nonce[] =
"\r\n Enter the Nonce (max : 16 Bytes): \r\n";

static const uint8_t read_aad[] =
"\r\n Enter the Additional authenticated data (max : 32 Bytes): \r\n";

static const uint8_t read_msg[] =
"\r\n Enter the input data to encrypt and authenticate (max : 64 bytes):\r\n";

static const uint8_t read_MAC_len[] =
"\r\n Enter the Number of octets in authentication field: \r\n\
  Press key '1' for 4 bytes \r\n\
  Press key '2' for 8 bytes \r\n\
  Press key '3' for 12 bytes \r\n\
  Press key '4' for 16 bytes \r\n";

static const uint8_t read_dec_msg[] =
"\r\n Enter the encrypted and authenticated message (max : 64 bytes):\r\n";

volatile __attribute__ ((section (".crypto_data"))) struct test_vector data = { 0x00 };

static void clear_ccm_var(void)
{
    uint16_t var = 0;

    data.M = 0;
    data.L = 0;
    data.lm = 0;
    data.la = 0;

    for(var = 0; var < CCM_BLOCKSIZE; var++)
    {
        data.key[var] = 0;
        data.nonce[var] = 0;
    }

    for(var = 0; var < 100; var++)
    {
        data.msg[var] = 0;
        data.output[var] = 0;
        data.dec_mac[var] = 0;
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

/*==============================================================================
  Perform CCM encryption.
 */
void perform_ccm_encrypt( void )
{
    uint16_t len = 0;
    uint8_t addr = 0x4;
    uint8_t i = 0x4;
    uint8_t status = 0;
    uint8_t text[256] = {0x00};
    uint8_t admin[4] = {0x00};
    uint8_t input_text[256] = {0x00};
    uint8_t user_key[12] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                            0x08, 0x09, 0x10, 0x11};


    /* Clear input and output variables */
    clear_ccm_var();

    /* Get key. */
    get_input_data((uint8_t*)&data.key, 16u, read_key_msg, sizeof(read_key_msg));

    /* Nonce N */
    len = get_input_data((uint8_t*)&data.nonce, 16u, read_nonce, sizeof(read_nonce));
    data.L = 15 - len;

    /* Additional authenticated data a */
    data.la = get_input_data((uint8_t*)&data.aad, 32u, read_aad, sizeof(read_aad));

    /* Message m */
    data.lm = get_input_data((uint8_t*)data.msg, 64u, read_msg, sizeof(read_msg));

    /* Read MAC length */
    get_input_data((uint8_t*)&data.M, 1u, read_MAC_len, sizeof(read_MAC_len));
    data.M *= 4;

    /* CCM Encryption */
    MACCcmEnrypt(data.M,
                 data.L,
                 (uint8_t*)data.nonce,
                 (uint8_t*)data.msg,
                 data.lm,
                 (uint8_t*)data.aad,
                 data.la,
                 (uint16_t*)data.key,
                 (uint8_t *)data.output);

    UART_send(&g_uart, g_separator, sizeof(g_separator));
    UART_send(&g_uart, (const uint8_t *)"\r\n Encrypted and Authentication data:\r\n",
              sizeof("\r\n Encrypted and Authentication data:\r\n"));
    display_output((uint8_t* )data.output, data.lm + data.M);
}

/*==============================================================================
  Perform CCM decryption.
 */
void perform_ccm_decrypt( void )
{
    uint16_t len = 0;
    uint8_t addr = 0x4;
    uint8_t i = 0x4;
    uint8_t status = 0;
    uint8_t text[256] = {0x00};
    uint8_t admin[4] = {0x00};
    uint8_t input_text[256] = {0x00};
    uint8_t user_key[12] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
                            0x09, 0x0a, 0xb, 0x0c};

    /* Clear input and output variables */
    clear_ccm_var();

    /* Get key. */
    get_input_data((uint8_t*)&data.key, 16u, read_key_msg, sizeof(read_key_msg));

    /* Nonce N */
    len = get_input_data((uint8_t*)&data.nonce, 16u, read_nonce,
                          sizeof(read_nonce));

    data.L = 15 - len;

    /* Additional authenticated data a */
    data.la = get_input_data((uint8_t*)&data.aad, 32u, read_aad,
                              sizeof(read_aad));

    /* Encrypted and authenticated message c */
    data.lm = get_input_data((uint8_t*)data.msg, 64u, read_dec_msg,
                             sizeof(read_dec_msg));

    /* Read MAC length */
    get_input_data((uint8_t*)&data.M, 1u, read_MAC_len, sizeof(read_MAC_len));
    data.M *= 4;

    /* Decrypt and Authenticate the input data.*/
    MACCcmDecrypt( data.M,
                   data.L,
                   (uint8_t*)data.nonce,
                   (uint8_t*)data.msg,
                   data.lm,
                   (uint8_t*)data.aad,
                   data.la,
                   (uint16_t*)&data.key,
                   (uint8_t *)&data.output,
				   (uint8_t *)&data.dec_mac);

    /* Display decrypted data and MAC value. */
    UART_send(&g_uart, g_separator, sizeof(g_separator));
    UART_send(&g_uart, (const uint8_t *)"\r\n Decrypted  data:\r\n",
              sizeof("\r\n Decrypted  data:\r\n"));
    display_output((uint8_t*)(data.output), data.lm - data.M);

    UART_send(&g_uart, (const uint8_t *)"\r\n Authenticated data (MAC):\r\n",
                                  sizeof("\r\n Authenticated data (MAC):\r\n"));
    display_output((uint8_t*)(data.dec_mac), data.M);
}

/******************************************************************************
 * main function.
 *****************************************************************************/
int main( void )
{
    uint8_t rx_buff[1];
    size_t rx_size = 0;

    /* Initialize CoreUARTapb with its base address, baud value. */
    UART_init(&g_uart, COREUARTAPB0_BASE_ADDR, BAUD_VALUE_115200,
              (DATA_8_BITS | NO_PARITY));
	/* Initializes the Athena Processor */
    CALIni();

    /* Display greeting message. */
    display_greeting();

    /* Select CCM operation */
    display_operation_choices();

    for(;;)
    {
        /* Read input from UART terminal. */
        rx_size = UART_get_rx(&g_uart, rx_buff, sizeof(rx_buff));
        if(rx_size > 0)
        {
            switch(rx_buff[0])
            {
                case '1':
                    /* Perform Encryption using AES-128.*/
                    UART_send(&g_uart, aes_encrypt_select_msg,
                              sizeof(aes_encrypt_select_msg));

                    /* Authenticate and encrypt the input data.*/
                    perform_ccm_encrypt();

                    display_option();
                    display_operation_choices();
                break;

                case '2':
                    /* Perform decryption using AES-128 */
                    UART_send(&g_uart, aes_decrypt_select_msg,
                              sizeof(aes_decrypt_select_msg));

                    /* Authenticate and decrypt the input data.*/
                    perform_ccm_decrypt();

                    display_option();
                    display_operation_choices();
                break;

                default:
                break;
            }
        }
    }
}

