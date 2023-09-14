
/*******************************************************************************
 * Copyright 2019 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file main.c
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief Application demonstrating the DSA cryptography service. Please see the
 * Readme.md for more details.
 *
 */
#include <stdint.h>
#include <string.h>
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
#include "drivers/fpga_ip/CoreUARTapb/core_uart_apb.h"
#include "fpga_design_config/fpga_design_config.h"
#include "miv_rv32_hal/miv_rv32_hal.h"
#include "helper.h"


#define KEY_SIZE 32
#define DATA_LENGTH_32_BYTES  32
#define REVERSE_FALSE          0
#define REVERSE_TRUE           1

/*flag to verify that  DSA Signature has been generated before verification */
#define SIG_GEN_TRUE           1
#define SIG_GEN_FALSE          0
#define PARAM_WORD_SIZE        12
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
************* PolarFire User Crypto DSA Service Example Project **************\r\n\
******************************************************************************\r\n\
 This example project demonstrates the use of the DSA service to calculate\r\n\
 a DSA signature on a provided message and also verifies the generated message\r\n\
signature. \r\n";
const uint8_t g_select_operation_msg[] =
"\r\n\
------------------------------------------------------------------------------\r\n\
 Select the Cryptographic operation to perform:\r\n\
    Press Key '1' to perform DSA signature generation \r\n\
    Press Key '2' to perform DSA signature verification \r\n\
------------------------------------------------------------------------------\r\n";
const uint8_t dsa_generation_msg[] =
"\r\n\
******************************************************************************\r\n\
                              DSA Signature Generation\r\n\
******************************************************************************\r\n" ;
const uint8_t dsa_verification_msg[] =
"\r\n\
******************************************************************************\r\n\
                              DSA Signature verification\r\n\
******************************************************************************\r\n";
const uint8_t select_private_key_msg[] =
"\r\n\
------------------------------------------------------------------------------\r\n\
 Select one of the following options:\r\n\
    Press Key '1' to generate private key through CALECKeyPairGen() function \r\n\
    Press Key '2' to get private key from the terateram macro. \r\n\
------------------------------------------------------------------------------\r\n";

const uint8_t select_public_key_msg[] =
"\r\n\
------------------------------------------------------------------------------\r\n\
 Select one of the following options:\r\n\
    Press Key '1' to generate public key through entered private_key \r\n\
    Press Key '2' to get public key from the terateram macro. \r\n\
------------------------------------------------------------------------------\r\n";

static const uint8_t g_separator[] =
"\r\n\
------------------------------------------------------------------------------";
static const uint8_t msg[] =
      "\r\n Enter message (max size : 12 words): \r\n";
static const uint8_t read_private_key[] =
      "\r\n Enter private key x (max size : 12 words):\r\n";
static const uint8_t read_public_key[] =
      "\r\n Enter public key y (max size : 12 words):\r\n";
static const uint8_t msg_gen_success[] = "\r\n\
------------------------------------------------------------------------------\r\n\
     \r DSA signature generation successful \r\n";
static const uint8_t msg_gen_fail[] = "\r\n\
------------------------------------------------------------------------------\r\n\
     \r\n DSA signature generation fail \r\n";
static const uint8_t msg_ver_success[] = "\r\n\
------------------------------------------------------------------------------\r\n\
     \r DSA signature verification successful \r\n";
static const uint8_t msg_ver_fail[] = "\r\n\
------------------------------------------------------------------------------\r\n\
     \r\n DSA signature verification fail \r\n";
static const uint8_t read_g[] =
      "\r\n Enter the g parameter \r\n";

static const uint8_t read_k[] =
      "\r\n Enter the random value k (exactly 8 words ):\r\n";
static const uint8_t read_p[] =
      "\r\n Enter the prime number p (exactly 96 words):\r\n";
static const uint8_t read_q[] =
      "\r\n Enter the prime number q (exactly 96 words):\r\n";
/*==============================================================================
  Global Variables.
 */
 uint32_t __attribute__ ((section (".crypto_data"))) msg_g[32] = { 0x00 };
 uint32_t __attribute__ ((section (".crypto_data"))) k_g[8] = { 0x00 };
 uint32_t __attribute__ ((section (".crypto_data"))) p_g[96] = { 0x00 };
 uint32_t __attribute__ ((section (".crypto_data"))) pmu_g[97] = { 0x00 };
 uint32_t __attribute__ ((section (".crypto_data"))) r_g[8] = { 0x00 };
 uint32_t __attribute__ ((section (".crypto_data"))) s_g[8] = { 0x00 };
 uint32_t __attribute__ ((section (".crypto_data"))) g_g[96] = { 0x00 };
 uint32_t __attribute__ ((section (".crypto_data"))) x_g[8] = { 0x00 };
 uint32_t __attribute__ ((section (".crypto_data"))) y_g[96] = { 0x00 };
 uint32_t __attribute__ ((section (".crypto_data"))) q_g[8] = { 0x00 };
 uint32_t __attribute__ ((section (".crypto_data"))) qmu_g[9] = { 0x00 };
 uint32_t __attribute__ ((section (".crypto_data"))) qlen_g = 0;
 uint32_t __attribute__ ((section (".crypto_data"))) plen_g = 0;
 uint16_t __attribute__ ((section (".crypto_data"))) msg_len = 0;
 uint32_t __attribute__ ((section (".crypto_data"))) buffer[] = {0x00};

/*==============================================================================
  Local functions.
 */
static void dsa_sign(void);
static void display_greeting(void);
static void dsa_verify(void);

/*==============================================================================
  Performs signature generation for ECDSA public-key cryptography.
 */
 void dsa_verify(void)
 {
    SATR result;
    uint8_t opt ;

    get_input_data((uint8_t*)&opt, sizeof(opt),select_public_key_msg,
                   sizeof(select_public_key_msg));
    if (opt == 1)
    {
        /* generate private and public key pair */
        result = CALExpo(g_g, x_g, p_g, pmu_g, 8, plen_g/4, y_g);

        if(SATR_SUCCESS == result)
        {
            CALPKTrfRes(SAT_TRUE);

            /* display the generated public key */
            UART_send(&g_uart, g_separator, sizeof(g_separator));
            UART_polled_tx_string(&g_uart,(uint8_t*)"\r\n Public key generation successful \r\n" );
            UART_send(&g_uart, (const uint8_t *)"\r\n public_key value : \r\n\r\n",
                      sizeof("\r\n public_key value : \r\n"));
            display_output((uint8_t*)&y_g[0], sizeof(y_g),REVERSE_TRUE);
        }
    }
    else
    {
        /* get public key y point */
        get_input_data((uint8_t*)&y_g[0], sizeof(y_g), read_public_key,
                       sizeof(read_public_key));
        /* adjust endianness */
        CALWordReverse(y_g, sizeof(y_g));
        CALByteReverseWord(y_g, sizeof(y_g));
    }
    /* generate public key*/
    result = CALExpo(g_g, x_g, p_g, pmu_g, 8, plen_g/4, y_g);

    result = CALPKTrfRes(SAT_TRUE);

    UART_send(&g_uart, g_separator, sizeof(g_separator));
    UART_polled_tx_string(&g_uart, (uint8_t*)"\r\n Public key Generated y = g^x mod p \r\n ");


    result = CALDSAVerifyHash(&msg_g[0], SATHASHTYPE_SHA384, msg_len, &g_g[0],
                                 &y_g[0], &r_g[0], &s_g[0], &p_g[0], &pmu_g[0],q_g,
                                 qmu_g, qlen_g/4, plen_g/4, SAT_TRUE,
                                 X52CCR_DEFAULT);
    if(SATR_SUCCESS == result)
    {
        result = CALPKTrfRes(SAT_TRUE);
        switch(result)
        {
            case SATR_SUCCESS:

                UART_polled_tx_string(&g_uart, (uint8_t*)msg_ver_success);
                break;
            case SATR_VERPARMR:

                UART_send(&g_uart, g_separator, sizeof(g_separator));
                UART_polled_tx_string(&g_uart, (uint8_t*)"\r\n Signature R parameter must be in range [1,Q-1] ");
                break ;
            case SATR_VERPARMS:

                UART_send(&g_uart, g_separator, sizeof(g_separator));
                UART_polled_tx_string(&g_uart, (uint8_t*)"\r\n Signature S parameter must be in range [1,Q-1] ");
                break ;
            case SATR_VERIFYFAIL:

                UART_polled_tx_string(&g_uart, (uint8_t*)msg_ver_fail);
                break ;
        }
    }
 }


void dsa_sign(void)
{
    SATR result;

    /* Read message value. */
    msg_len = get_input_data((uint8_t*)&msg_g, sizeof(msg_g), msg, sizeof(msg));

    /* Read DSA value G parameter. */
    get_input_data((uint8_t*)&g_g, sizeof(g_g), read_g,
                   sizeof(read_g));

    /* Read random per-message value K parameter. */
    get_input_data((uint8_t*)&k_g, sizeof(k_g), read_k,
                   sizeof(read_k));

    /* Read Private key X parameter. */
    get_input_data((uint8_t*)&x_g, sizeof(x_g), read_private_key,
                   sizeof(read_private_key));

    /* Read module value P. */
    plen_g = get_input_data((uint8_t*)&p_g, sizeof(p_g), read_p,
                            sizeof(read_p));

    /* Read module value Q. */
    qlen_g = get_input_data((uint8_t*)&q_g, sizeof(q_g), read_q,
                            sizeof(read_q));

    /* Change the endianness of received parameter from UART terminal.*/
    CALWordReverse((uint32_t*)&g_g, (sizeof(g_g)/4));
    CALByteReverseWord((uint32_t*)&g_g, (sizeof(g_g)/4));
    CALWordReverse((uint32_t*)&k_g, (sizeof(k_g)/4));
    CALByteReverseWord((uint32_t*)&k_g,(sizeof(k_g)/4));
    CALWordReverse((uint32_t*)&x_g, (sizeof(x_g)/4));
    CALByteReverseWord((uint32_t*)&x_g, (sizeof(x_g)/4));
    CALWordReverse((uint32_t*)&p_g, (sizeof(p_g)/4));
    CALByteReverseWord((uint32_t*)&p_g, (sizeof(p_g)/4));
    CALWordReverse((uint32_t*)&q_g, (sizeof(q_g)/4));
    CALByteReverseWord((uint32_t*)&q_g, (sizeof(q_g)/4));

    /* Initiate generation of pre-compute value for P. */
    result = CALPreCompute((uint32_t*)&p_g[0], (uint32_t*)&pmu_g[0], 96u);
    if(SATR_SUCCESS == result)
    {
        CALPKTrfRes(SAT_TRUE);
    }

    /* Initiate generation of pre-compute value for Q. */
    result = CALPreCompute((uint32_t*)&q_g[0], (uint32_t*)&qmu_g[0], 8u);
    if(SATR_SUCCESS == result)
    {
        CALPKTrfRes(SAT_TRUE);
    }

    /* Calculates a DSA signature with SCA countermeasures. */
    result = CALDSASignHash(&msg_g[0], SATHASHTYPE_SHA384, msg_len, &g_g[0],
                              &k_g[0], &x_g[0], &p_g[0], &pmu_g[0], &q_g[0],
                              &qmu_g[0], qlen_g/4, plen_g/4, r_g, s_g, SAT_TRUE,
                              X52CCR_DEFAULT);

    /* Display the generated signature. */
    if(SATR_SUCCESS == result)
    {
        CALPKTrfRes(SAT_TRUE);
        switch(result)
        {
            case SATR_SUCCESS:

                /* Display the generated signature in hex format. */
                UART_send(&g_uart, msg_gen_success, sizeof(msg_gen_success));
                UART_polled_tx_string(&g_uart,(uint8_t*)"\r\n R value:\r\n\r\n");
                display_output((uint8_t*)&r_g[0],sizeof(r_g),REVERSE_TRUE);
                UART_polled_tx_string(&g_uart,(uint8_t*)"\r\n S value:\r\n\r\n");
                display_output((uint8_t*)&s_g[0],sizeof(s_g),REVERSE_TRUE);
                break;
            case SATR_SIGNPARMK:

                UART_polled_tx_string(&g_uart, (uint8_t*)"\r\n\n k parameter must be in [1,q-1] \r\n");
                UART_send(&g_uart, msg_gen_fail, sizeof(msg_gen_fail));
                break ;

            case SATR_SIGNFAIL :
                UART_send(&g_uart, msg_gen_fail, sizeof(msg_gen_fail));
                break ;
        }

    }
    else
    {
        UART_send(&g_uart, msg_gen_fail, sizeof(msg_gen_fail));
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
 * Perform DSA signature and verification sequentially
 */
/* Main function */
void main(void)
{
    uint8_t rx_buff[1];
    uint8_t rx_size = 0;
    uint8_t sig_gen_success = SIG_GEN_FALSE ;

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
                    /* Performs signature generation for DSA */
                    UART_send(&g_uart, dsa_generation_msg, sizeof(dsa_generation_msg));
                    dsa_sign();
                    sig_gen_success = SIG_GEN_TRUE ;
                    display_option();
                    display_operation_choices();
                break;

                case '2':
                    /* Performs signature verification for DSA */
                    UART_send(&g_uart, dsa_verification_msg, sizeof(dsa_verification_msg));
                    if(sig_gen_success == SIG_GEN_TRUE)
                    {
                        dsa_verify();
                    }
                    else
                    {
                        UART_polled_tx_string(&g_uart,(uint8_t*)"\r\n Please perform DSA signature Generation at least once before DSA verification \r\n ");
                    }
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
