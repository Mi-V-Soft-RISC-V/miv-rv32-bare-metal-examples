/*******************************************************************************
 * Copyright 2019 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file main.c
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief Application demonstrating the RSA cryptography service. Please see the
 * Readme.md for more details.
 *
 */
#include <stdint.h>
#include "cal/calini.h"
#include "cal/calini.h"
#include "cal/utils.h"
#include "cal/pk.h"
#include "drivers/fpga_ip/CoreUARTapb/core_uart_apb.h"
#include "fpga_design_config/fpga_design_config.h"
#include "miv_rv32_hal/miv_rv32_hal.h"
#include "helper.h"
/******************************************************************************
 * User Crypto base address.
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
******** PolarFire User Crypto RSA Signature Service Example Project *********\r\n\
******************************************************************************\r\n\
 This example project demonstrates the use of the PolarFire SoC MSS User \r\n\
 Crypto RSA signature service. The following system service are demonstrated:\r\n\
  1 - RSA signature generation.\r\n\
  2 - RSA signature verification.\r\n";

const uint8_t g_select_operation_msg[] =
"\r\n\
------------------------------------------------------------------------------\r\n\
 Select the Cryptographic operation to perform:\r\n\
    Press Key '1' to perform RSA signature generation \r\n\
    Press Key '2' to perform RSA signature verification \r\n\
------------------------------------------------------------------------------\r\n";

/******************************************************************************
 * Instruction message.
 *****************************************************************************/
static const uint8_t g_separator[] =
"\r\n\
------------------------------------------------------------------------------";
const uint8_t siggen_select_msg[] =
"\r\n Selected RSA signature generation service. \r\n";
const uint8_t sigver_select_msg[] =
"\r\n Selected RSA signature verification service. \r\n";
const uint8_t rsa_encode_msg[] =
  "\r\n Select RSA encoding/encryption block format: \r\n\
        Press Key '1' for No encoding/padding\r\n\
        Press Key '2' for RSASSA-PKCS1-v1.5\r\n\
        Press Key '3' for ANSI X9.31\r\n\
        Press Key '4' for RSASSA-PSS\r\n";
const uint8_t hash_type_msg[] =
  "\r\n Enter hash type: \r\n\
        Press Key '1' for SHA-1 Algorithm \r\n\
        Press Key '2' for SHA-224 Algorithm \r\n\
        Press Key '3' for SHA-256 Algorithm \r\n\
        Press Key '4' for SHA-384 Algorithm \r\n\
        Press Key '5' for SHA-512 Algorithm \r\n\
        Press Key '6' for SHA-512/224 Algorithm \r\n\
        Press Key '7' for SHA-512/256 Algorithm \r\n\
        Press Key '8' for MD5 Algorithm \r\n\
        Press Key '9' for Kasumi (3GPP) Algorithm \r\n\
        Press Key 'a' for SNOW (3GPP) Algorithm \r\n";
const uint8_t msg[] =
  "\r\n Enter message (max 32 word): \r\n";
const uint8_t read_key_exp[] =
  "\r\n Enter private key exponent value (max 64 word): \r\n";
const uint8_t read_mod_value[] =
  "\r\n Enter modulus N (max 64 word): \r\n";
const uint8_t msg_success[] =
  "\r\n RSA signature generation successful. \
   \r\n Generated Signature value:\r\n";
const uint8_t msg_fail[] =
  "\r\n RSA signature generation fail. \r\n";
const uint8_t read_public_exp[] =
  "\r\n Enter public exponent value (max 64 word): \r\n";
const uint8_t read_sig_val[] =
  "\r\n Enter signature value in words: \r\n";
const uint8_t rsa_verify_success[] =
  "\r\n RSA signature verification successful.\r\n";
const uint8_t rsa_verify_fail[] =
  "\r\n RSA signature verification fail. \r\n";

#define REVERSE_TRUE    1
#define REVERSE_FALSE   0
/*==============================================================================
  Global Variables.
 */
uint32_t __attribute__ ((section (".crypto_data"))) sig_val[64] = {0x00};
uint32_t __attribute__ ((section (".crypto_data"))) msg_g[32] = { 0x00 };
uint32_t __attribute__ ((section (".crypto_data"))) d_g[64] = {0x00};
uint32_t __attribute__ ((section (".crypto_data"))) n_g[64] = {0x00};
uint32_t __attribute__ ((section (".crypto_data"))) nmu_g[65] = {0x00};
uint32_t __attribute__ ((section (".crypto_data"))) e_g[64] = {0x00};
uint32_t __attribute__ ((section (".crypto_data"))) e_len = 0x00;

/*==============================================================================
  Performs signature generation for RSA public-key cryptography.
 */
static void rsa_sign(void)
{
    SATR result;
    uint8_t encode_type = 0;
    uint8_t hash_type = 0;
    uint16_t msg_len = 0;
    uint16_t mod_len = 0;

    /* Read type of RSA encoding. */
    get_input_data((uint8_t*)&encode_type, sizeof(encode_type), rsa_encode_msg,
                   sizeof(rsa_encode_msg));
    encode_type -= 1;

    /* Read hash type. */
    get_input_data((uint8_t*)&hash_type, sizeof(hash_type), hash_type_msg,
                   sizeof(hash_type_msg));

    /* Read message value. */
    msg_len = get_input_data((uint8_t*)&msg_g, sizeof(msg_g), msg, sizeof(msg));

    /* Read private key exponent D. */
    get_input_data((uint8_t*)&d_g, sizeof(d_g), read_key_exp,
                   sizeof(read_key_exp));

    /* Read module value N. */
    mod_len = get_input_data((uint8_t*)&n_g, sizeof(n_g), read_mod_value,
                             sizeof(read_mod_value));

    /* Change the endianness of data received from UART terminal. */
    CALWordReverse((uint32_t*)&d_g, 64);
    CALWordReverse((uint32_t*)&n_g, 64);
    CALByteReverseWord((uint32_t*)&d_g, 64);
    CALByteReverseWord((uint32_t*)&n_g, 64);

    result = CALPreCompute((uint32_t*)&n_g[0], &nmu_g[0], 64u);
    if(SATR_SUCCESS == result)
    {
        CALPKTrfRes(SAT_TRUE);
    }
    else
    {
        UART_polled_tx_string(&g_uart, (uint8_t*)"\r\n N Mod value is not correct ");
    }

    /* Performs signature generation for RSA public-key cryptography on the hash
       produced from puiMsg using the RSA encoding type. */
    result = CALRSASignHash(encode_type, hash_type, &msg_g[0], &d_g[0],
                            &n_g[0], &nmu_g[0], msg_len, mod_len/4,
                            &sig_val[0], SAT_TRUE, 0);

    UART_send(&g_uart, g_separator, sizeof(g_separator));

    /* Display the generated signature in hex format. */
    if(SATR_SUCCESS == result)
    {
        CALPKTrfRes(SAT_TRUE);
        switch(result)
        {
            case SATR_SUCCESS:

                /* Display the generated signature in hex format. */
                UART_send(&g_uart, msg_success, sizeof(msg_success));
                display_output((uint8_t*)&sig_val[0],sizeof(sig_val),REVERSE_TRUE);
                break;
            case SATR_BADPARAM:
                UART_polled_tx_string(&g_uart,(uint8_t*)"\r\n For PKCS encoding, padding length is less than 3\
                            words.\r\n" );
                break;
        }
    }
    else
    {
        /* RSA signature generation fail */
        UART_send(&g_uart, msg_fail, sizeof(msg_fail));
    }
}

/*==============================================================================
  Performs signature verification for RSA public-key cryptography.
 */
static void rsa_verify(void)
{
    SATR result;
    uint8_t encode_type = 0;
    uint8_t hash_type = 0;
    uint16_t msg_len = 0;
    uint16_t mod_len = 0;
    uint16_t e_len = 0;
    uint8_t opt = 0;
    uint8_t rsa_flag = 0 ;

    /* Read type of RSA encoding. */
    get_input_data((uint8_t*)&encode_type, sizeof(encode_type), rsa_encode_msg,
                   sizeof(rsa_encode_msg));
    encode_type -= 1;

   /* Read hash type. */
    get_input_data((uint8_t*)&hash_type, sizeof(hash_type), hash_type_msg,
                   sizeof(hash_type_msg));

    /* Read message value. */
    msg_len = get_input_data((uint8_t*)&msg_g, sizeof(msg_g), msg, sizeof(msg));

    /* Read public exponent value E. */
    e_len = get_input_data((uint8_t*)&e_g, sizeof(e_g), read_public_exp,
                           sizeof(read_public_exp));

    /* Read module value N */
    mod_len = get_input_data((uint8_t*)&n_g, sizeof(n_g), read_mod_value,
                             sizeof(read_mod_value));

    /* check if all the value of sig_val is zero if not zero then user generated
     * signature value */
    for ( int w_count = 0 ; w_count < 64 ; w_count ++)
    {
        if(sig_val[w_count] | 0x00000000 != 0)
        {
            rsa_flag = 1;
            break ;
        }
    }
    if( rsa_flag == 0){

    /* Read signature value. i.e. puiS */
        get_input_data((uint8_t*)&sig_val, sizeof(sig_val), read_sig_val,
                       sizeof(read_sig_val));
        CALWordReverse((uint32_t*)&sig_val, 64);
    }

    /* Change the endianness of data received from UART terminal. */
    CALWordReverse((uint32_t*)&e_g, 64);
    CALWordReverse((uint32_t*)&n_g, 64);
    CALByteReverseWord((uint32_t*)&e_g, 64);
    CALByteReverseWord((uint32_t*)&n_g, 64);

    result = CALPreCompute((uint32_t*)&n_g[0], &nmu_g[0], 64u);
    if(SATR_SUCCESS == result)
    {
        CALPKTrfRes(SAT_TRUE);
    }
    else
    {
        UART_polled_tx_string(&g_uart, (uint8_t*)"\r\n N Mod value is not correct ");
    }

    /* Performs signature verification for RSA public-key cryptography on the
       hash produced from puiMsg using the RSA encoding type. */
    result = CALRSAVerifyHash(encode_type, hash_type, &msg_g[0], &e_g[0],
                              e_len/4, &n_g[0], &nmu_g[0], msg_len,
                              mod_len/4, &sig_val[0], SAT_TRUE, 0);

    UART_send(&g_uart, g_separator, sizeof(g_separator));

    /* Display the return status of RSA signature verification. */
    if(SATR_SUCCESS == result)
    {
        result = CALPKTrfRes(SAT_TRUE);
        if(SATR_SUCCESS == result)
        {
            UART_send(&g_uart, rsa_verify_success, sizeof(rsa_verify_success));
        }
    }
    else
    {
        UART_send(&g_uart, rsa_verify_fail, sizeof(rsa_verify_fail));
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
 * Perform RSA signature
 */
/* Main function */
void main(void)
{
    uint8_t rx_buff[1];
    uint8_t rx_size = 0;

    /* UART initialization */
    UART_init(&g_uart, COREUARTAPB0_BASE_ADDR, BAUD_VALUE_115200,
              (DATA_8_BITS | NO_PARITY));

    /* Initializes the Athena Processor */
    CALIni();

    /* Display greeting message. */
    display_greeting();

    /* Select cryptographic operation to perform */
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
                    /* Performs signature generation for RSA */
                    UART_send(&g_uart, siggen_select_msg, sizeof(siggen_select_msg));
                    rsa_sign();
                    display_option();
                    display_operation_choices();
                break;

                case '2':
                    /* Performs signature verification for RSA*/
                    UART_send(&g_uart, sigver_select_msg, sizeof(sigver_select_msg));
                    rsa_verify();
                    display_option();
                    display_operation_choices();
                break;

              default:
              break;
            }
        }
    }

    return;
}
