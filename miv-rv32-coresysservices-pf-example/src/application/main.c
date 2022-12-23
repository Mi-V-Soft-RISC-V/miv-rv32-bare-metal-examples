/*******************************************************************************
 * (c) Copyright 2019-2021 Microchip SoC Products Group. All rights reserved.
 *
 * PF_System_Services driver Device and design info services example.
 *
 * This simple example demonstrates how to use the PF_System_Services driver to
 * execute Device and design info services and data security service supported
 * by PolarFire System Controller.
 *
 * This sample project is targeted at a Mi-V soft processor design running on
 * the PolarFire High Speed Validation board.
 *
 * This program displays the return data from System controller for various
 * Device and Design services.
 *
 * Please refer README.md in the root folder of this project for more details.
 */

#include <stdio.h>
#include "drivers/fpga_ip/CoreSysServices_PF/core_sysservices_pf.h"
#include "drivers/fpga_ip/CoreUARTapb/core_uart_apb.h"
#include "fpga_design_config/hw_platform.h"
#include "miv_rv32_hal/miv_rv32_hal.h"
#include "helper.h"

/******************************************************************************
 * Maximum receiver buffer size.
 *****************************************************************************/
#define MAX_RX_DATA_SIZE                        256
#define ENTER                                   13u

/******************************************************************************
 * CoreUARTapb instance data.
 *****************************************************************************/
UART_instance_t g_uart;

/*
 data structures for function execute_secure_nvm()
*/
const uint8_t read_input_data[] =
"\r Enter the input data (max 253 HEX bytes): \r\n";
const uint8_t read_user_key[] =
"\r Enter the user key (max 12 HEX bytes): \r\n";
const uint8_t read_address[] =
"\r Enter the sNVM page/module address (1 HEX byte. Maximum Page value is 0xDC): \r\n";
const uint8_t write_format[] =
"\r Secure NVM (sNVM) Functions \r\n\
\r Enter the sNVM write format (only 1 byte): \r\n\
\r    1. Non-authenticated plain-text \r\n\
\r    2. Authenticated plain-text \r\n\
\r    3. Authenticated cipher-text \r\n";

/* data structures for function execute_puf_emulation_service() */
const uint8_t optype_comment[] =
"\r PUF Emulation Service \r\n\
\r\n Enter the challenge OPTYPE range(0x0 to 0xFF): \r\n";

const uint8_t challenge_comment[] =
"\r Enter the 16 byte challenge: \r\n";

/* data structures for function execute_digital_signature_service() */
const uint8_t signature_format_comment[] =
"\r Digital Signature Service \r\n\
\r Select the digital signature format: \r\n\
\r    1. Raw format \r\n\
\r    2. DER format \r\n";

const uint8_t read_hash_value_comment[] =
"\r Enter the 48 byte hash value: \r\n";

/*==============================================================================
 * Private functions.
 */
static void execute_puf_emulation_service(void);
static void execute_digital_signature_service(void);
static void execute_secure_nvm(void);

/*==============================================================================
 * Messages displayed over the UART.
 */
const uint8_t g_greeting_msg[] =
"\r\n\r\n **** PolarFire Device and Design system services Example ****\n\n\n\r\
Note: Return data from System controller is displayed byte-wise with LSB first\n\r\
Note: Input data is provided LSB first. Each ASCII charachter is one Nibble of data\n\n\n\r\
Select Service:\r\n\
1. Read Device Serial number \r\n\
2. Read Device User-code \r\n\
3. Read Device Design-info \r\n\
4. Read Device certificate \r\n\
5. Read Digest \r\n\
6. Query security \r\n\
7. Read debug information \r\n\
8. Bit-stream authentication \r\n\
9. IAP image authentication \r\n\
a. Digital signature service \r\n\
b. Secure NVM services\r\n\
c. PUF Emulation_service \r\n\
d. Nonce service \r\n\
e. Digest check service\r\n\
f. IAP service\r\n";

const uint8_t iap_msg[] =
"\n\r\t Select IAP service option \n\r\
1.IAP Program by Index\n\r\
2.IAP Verify by index\n\r\
3.IAP Program by address\n\r\
4.IAP Verify by address\n\r\
5.IAP Auto-Update\n\r";

const uint8_t g_separator[] =
"\r\n----------------------------------------------------------------------\r\n";

static void display_greeting(void);

uint8_t data_buffer [1024];
uint8_t input_text[506] = {0x00};

static void clear_data_buffer(void)
{
    uint32_t idx = 0u;

    while (idx < 1024u)
    {
        data_buffer[idx++] = 0x00u;
    }
}
/*==============================================================================
 * Display greeting message when application is started.
 */
static void display_greeting(void)
{
    UART_polled_tx_string(&g_uart, g_greeting_msg);
}

void revers_sequence(uint8_t* src, uint32_t size)
{
	uint32_t temp_size = (size - 1);
	uint32_t cnt = 0u;
	uint32_t temp_val = 0u;

	while (cnt < temp_size)
	{
		temp_val = *((src+temp_size));
		*((src+temp_size)) = *((src+cnt));
		*((src+cnt)) = temp_val;
		cnt++;
		temp_size--;
	}
}

void execute_iap(void)
{
    uint8_t cmd = 0u;
    uint32_t spiaddr = 0u;
    uint8_t uart_input[8] = {0};
    uint32_t* ptr = 0u;

     /* Read FF timeout value*/
    get_input_data(uart_input, (sizeof(cmd)),
                  (const uint8_t*)iap_msg,
				  sizeof(iap_msg));

    ptr = ((uint32_t*)&uart_input);

    switch(*ptr)
    {
        case 1:
            cmd = IAP_PROGRAM_BY_SPIIDX_CMD;
            UART_polled_tx_string(&g_uart,(const uint8_t*)
                    "\n\rEnter Index number in decimal\n\r");
            spiaddr = get_value_from_uart(10);

            break;
        case 2:
            cmd = IAP_VERIFY_BY_SPIIDX_CMD;
            UART_polled_tx_string(&g_uart,(const uint8_t*)
                    "\n\rEnter Index number in decimal\n\r");
            spiaddr = get_value_from_uart(10);

            break;
        case 3:
            cmd = IAP_PROGRAM_BY_SPIADDR_CMD;
            UART_polled_tx_string(&g_uart,(const uint8_t*)
                    "\n\rEnter 32 bit Address in hex without 0x prefix \n\r");
            spiaddr = get_value_from_uart(16);

            break;
        case 4:
            cmd = IAP_VERIFY_BY_SPIADDR_CMD;
            UART_polled_tx_string(&g_uart,(const uint8_t*)
                    "\n\rEnter 32 bit Address in hex without 0x prefix \n\r");
            spiaddr = get_value_from_uart(16);

            break;
        case 5:
            cmd = IAP_AUTOUPDATE_CMD;
            /* SPI Directory index 1 is used for auto update image */

        break;
        default: UART_polled_tx_string(&g_uart,(const uint8_t*)
                "Invalid input.\r\n");
    }

    SYS_iap_service(cmd, spiaddr, 0);
}

void execute_bitstream_authenticate(void)
{
    uint32_t spi_flash_address = 0u;
    uint32_t mb_offset = 0u;
    uint8_t uart_input[8] = {0};
    uint32_t* ptr = 0u;
    uint32_t count = 0u;

    get_input_data(uart_input,(sizeof(spi_flash_address)),
                  (const uint8_t*)"Enter the address within SPI FLASH",
				  sizeof("Enter the address within SPI FLASH"));

    revers_sequence(uart_input, count);

    ptr = ((uint32_t*)&uart_input);
    spi_flash_address = *ptr;

    /* Read mb_offset where timeout value will be stored in MB*/
    count = get_input_data(uart_input,(sizeof(mb_offset)),
                  (const uint8_t*)"Enter MailBox offset for this service.(0 to 0x200)",
				  sizeof("Enter MailBox offset for this service.(0 to 0x200)"));

    revers_sequence(uart_input, count);
    ptr = ((uint32_t*)&uart_input);
    mb_offset = *ptr;

    SYS_bitstream_authenticate_service(spi_flash_address, mb_offset);
}

void execute_iap_image_authenticate(void)
{
    uint32_t spi_idx = 0u;
    uint8_t uart_input[8] = {0};
    uint32_t* ptr = 0u;
    uint32_t count = 0u;

    count = get_input_data(uart_input,(sizeof(spi_idx)),
                  (const uint8_t*)"Enter the index within SPI FLASH. (Index = 1 is not a valid value)",
				  sizeof("Enter the index within SPI FLASH. (Index = 1 is not a valid value)"));

    revers_sequence(uart_input, count);
    ptr = ((uint32_t*)&uart_input);
    spi_idx = *ptr;

    SYS_IAP_image_authenticate_service(spi_idx);
}

void execute_digest_check(void)
{
    uint32_t options = 0u;
    uint32_t mb_offset = 0u;
    uint8_t uart_input[8] = {0};
    uint16_t ret_val = 0u;
    uint32_t* ptr = 0u;
    uint32_t count = 0u;

    count = get_input_data(uart_input, 2,
                  (const uint8_t*)"Enter 16 bit hex value. Check API description for more info.\n\r",
				  sizeof("Enter 16 bit hex value. Check API description for more info.\n\r"));

    revers_sequence(uart_input, count);

    ptr = ((uint32_t*)&uart_input);
    options = *ptr;

    /* Read mb_offset where timeout value will be stored in MB*/
    count = get_input_data(uart_input,(sizeof(mb_offset)),
                  (const uint8_t*)"Enter MailBox offset for this service.(0 to 0x200)",
				  sizeof("Enter MailBox offset for this service.(0 to 0x200)"));

    revers_sequence(uart_input, count);
    ptr = ((uint32_t*)&uart_input);
    mb_offset = *ptr;

    ret_val = SYS_digest_check_service(options, mb_offset);

    if (0u == ret_val)
    {
    	UART_polled_tx_string(&g_uart,(const uint8_t*)"\r\nDigest check Success ");
    }
    else
    {
    	UART_polled_tx_string(&g_uart,(const uint8_t*)"\r\nDigest check return value: ");
    	display_output((uint8_t*)&ret_val, 2u);
    }
}

void execute_serial_number_service(void)
{
    uint8_t status;

    UART_polled_tx_string(&g_uart,(const uint8_t*)"Device serial number: ");
    status = SYS_get_serial_number(data_buffer, 0u);

    if (SYS_SUCCESS == status)
    {
       display_output(data_buffer, SERIAL_NUMBER_RESP_LEN);
    }
    else
    {
        UART_polled_tx_string(&g_uart,
                             (const uint8_t*)"Service read device serial number failed.\r\n");
    }

    UART_polled_tx_string(&g_uart, g_separator);
}

void execute_usercode_service(void)
{
    uint8_t status;

    UART_polled_tx_string(&g_uart, (const uint8_t*)"32bit USERCODE: ");
    status = SYS_get_user_code(data_buffer, 0u);

    if (SYS_SUCCESS == status)
    {
       display_output(data_buffer, USERCODE_RESP_LEN);
    }
    else
    {
        UART_polled_tx_string(&g_uart,(const uint8_t*)"USERCODE Service failed.\r\n");
    }

    UART_polled_tx_string(&g_uart, g_separator);
}

void execute_designinfo_service(void)
{
    uint8_t status;

    UART_polled_tx_string(&g_uart, (const uint8_t*)"Design ID: ");
    status = SYS_get_design_info(data_buffer, 0u);

    if (SYS_SUCCESS == status)
    {
       display_output(data_buffer, 32u);

        UART_polled_tx_string(&g_uart, (const uint8_t*)"\r\nDesign Version: ");
       display_output((data_buffer + 32), 2);

        UART_polled_tx_string(&g_uart, (const uint8_t*)"\r\nDesign Back-Level: ");
       display_output((data_buffer + 34), 2);
    }
    else
    {
        UART_polled_tx_string(&g_uart,(const uint8_t*)"DesignInfo Service failed.\r\n");
    }

    UART_polled_tx_string(&g_uart, g_separator);
}

void execute_devicecertificate_service(void)
{
    uint8_t status;

    UART_polled_tx_string(&g_uart, (const uint8_t*)"Device Certificate: ");
    status = SYS_get_device_certificate(data_buffer, 0);

    if(SYS_SUCCESS == status)
    {
       display_output(data_buffer, DEVICE_CERTIFICATE_RESP_LEN);
    }
    else
    {
        UART_polled_tx_string(&g_uart,(const uint8_t*)
                              "Device Certificate Service failed.\r\n");

        if(SYS_DCF_INVALID_SIGNATURE == status)
        {
            UART_polled_tx_string(&g_uart,(const uint8_t*)
                                  "Error - Invalid certificate signature");
        }

        if(SYS_DCF_DEVICE_MISMATCH == status)
        {
            UART_polled_tx_string(&g_uart,(const uint8_t*)"Error - Device Mismatch");
        }

        if(SYS_DCF_SYSTEM_ERROR == status)
        {
            UART_polled_tx_string(&g_uart,(const uint8_t*)"Error - System Error");
        }
    }

    UART_polled_tx_string(&g_uart, g_separator);
}

void execute_readdigest_service(void)
{
    uint8_t status;

    UART_polled_tx_string(&g_uart, (const uint8_t*)"Read Digest: ");
    status = SYS_read_digest(data_buffer, 0u);

    if(SYS_SUCCESS == status)
    {
       display_output(data_buffer, READ_DIGEST_RESP_LEN);
    }
    else
    {
        UART_polled_tx_string(&g_uart,(const uint8_t*)"Read Digest Service failed.\r\n");
    }

    UART_polled_tx_string(&g_uart, g_separator);
}

void execute_querysecurity_service(void)
{
    uint8_t status;

    UART_polled_tx_string(&g_uart, (const uint8_t*)"Security locks: ");
    status = SYS_query_security(data_buffer, 0u);

    if (SYS_SUCCESS == status)
    {
       display_output(data_buffer, QUERY_SECURITY_RESP_LEN);
    }
    else
    {
        UART_polled_tx_string(&g_uart,
                              (const uint8_t*)"Query Security Service failed.\r\n");
    }

    UART_polled_tx_string(&g_uart, g_separator);
}

void execute_readdebuginfo_service(void)
{
    uint8_t status;

    UART_polled_tx_string(&g_uart, (const uint8_t*)"Debug info: ");
    status = SYS_read_debug_info(data_buffer, 0u);

    if (SYS_SUCCESS == status)
    {
       display_output(data_buffer, READ_DEBUG_INFO_RESP_LEN);
    }
    else
    {
        UART_polled_tx_string(&g_uart,
                              (const uint8_t*)"Read Debug Info Service failed.\r\n");
    }

    UART_polled_tx_string(&g_uart, g_separator);
}

void execute_nonce_service(void)
{
    uint8_t status;

    UART_polled_tx_string(&g_uart, (const uint8_t*)"Generated Nonce: ");
    status = SYS_nonce_service(data_buffer, 0u);

    if (SYS_SUCCESS == status)
    {
       display_output(data_buffer, NONCE_SERVICE_RESP_LEN);
    }
    else
    {
        UART_polled_tx_string(&g_uart, (const uint8_t*)"Nonce Service  failed.\r\n");

        if (SYS_NONCE_PUK_FETCH_ERROR == status)
        {
            UART_polled_tx_string(&g_uart, (const uint8_t*)"Error fetching PUK");
        }
        else if (SYS_NONCE_SEED_GEN_ERROR == status)
        {
            UART_polled_tx_string(&g_uart, (const uint8_t*)"Error generating seed");
        }
    }

    UART_polled_tx_string(&g_uart, g_separator);
}

/******************************************************************************
 * main function.
 *****************************************************************************/
int main(void)
{
    size_t rx_size;
    uint8_t rx_buff[1] = {0x00};

    clear_data_buffer();

    /*Initialize the PF_System_Services driver*/
    SYS_init(CSS_PF_BASE_ADDRESS);
    /*
      Initialize CoreUARTapb with its base address, baud value, and line
      configuration.
     */
    UART_init(&g_uart, UART_BASE_ADDRESS, BAUD_VALUE_115200, (DATA_8_BITS | NO_PARITY));

    display_greeting();

    for (;;)
    {
        /* Start command line interface if any key is pressed. */
        rx_size = UART_get_rx(&g_uart, rx_buff, sizeof(rx_buff));

        if (rx_size > 0u)
        {
        	UART_polled_tx_string(&g_uart, g_separator);
            switch(rx_buff[0])
            {
                case '1':
                    execute_serial_number_service();
                break;

                case '2':
                    execute_usercode_service();
                break;

                case '3':
                    execute_designinfo_service();
                break;

                case '4':
                    execute_devicecertificate_service();
                break;

                case '5':
                    execute_readdigest_service();
                break;

                case '6':
                    execute_querysecurity_service();
                break;

                case '7':
                    execute_readdebuginfo_service();
                break;

                case '8':
                    execute_bitstream_authenticate();
                break;

                case '9':
                    execute_iap_image_authenticate();
                break;

                case 'a':
                    execute_digital_signature_service();
                break;

                case 'b':
                    execute_secure_nvm();
                break;

                case 'c':
                    execute_puf_emulation_service();
                break;

                case 'd':
                    execute_nonce_service();
                break;

                case 'e':
                    execute_digest_check();
                break;

                case 'f':
                    execute_iap();
                break;

                default:
                    display_greeting();
                break;
            }
            display_greeting();
        }
    }
}

/*==============================================================================
  Demonstrates Secure NVM service.
 */
static void execute_secure_nvm(void)
{
    uint16_t addr = 0x00u;
    uint8_t user_key[24] = {0x00};
    uint8_t admin[4] = {0x00};
    uint8_t text[280] = {0x00};
    uint8_t format = 0u;
    uint8_t status = 0u;
    uint32_t count = 0u;
    uint8_t cmd = 0u;

    /* Read the write data format from user. */
    get_input_data(&format,1,write_format, sizeof(write_format));

    if((0u == format) || (format > 3u))
    {
    	status = SYS_PARAM_ERR;
    }

    if(SYS_PARAM_ERR != status)
    {
		/* Read address of NVM region where the user data will be stored.*/
		get_input_data((uint8_t*)&addr,2,read_address, sizeof(read_address));

		if(addr > 0xDC)
		{
			status = SYS_PARAM_ERR;
		}
    }

    if(SYS_PARAM_ERR != status)
    {

		/* Based on the format send command and descriptor to the system controller. */
		if (format == 2u)
		{
			/* Read user key from UART terminal.*/
			count = get_input_data(&user_key[0],12,read_user_key,sizeof(read_user_key));
			revers_sequence(user_key, count);
			cmd = SNVM_AUTHEN_TEXT_REQUEST_CMD;
		}
		else if (format == 3u)
		{
			/* Read user key from UART terminal.*/
			count = get_input_data(&user_key[0],12,read_user_key,sizeof(read_user_key));
			revers_sequence(user_key, count);
			cmd = SNVM_AUTHEN_CIPHERTEXT_REQUEST_CMD;
		}
		else
		{
			cmd = SNVM_NON_AUTHEN_TEXT_REQUEST_CMD;
		}

        /* Read data to be written into sNVm region from UART terminal.*/
        count  = get_input_data(&input_text[0],252,read_input_data,sizeof(read_input_data));

		status = SYS_secure_nvm_write(cmd, addr, &input_text[0], user_key, 0);
    }

    /* Display status information on UART terminal. */
    if (status == SYS_SUCCESS)
    {
        UART_polled_tx_string(&g_uart,
                              (const uint8_t*)"\r\n Secure NVM write successful.");

        /* Read the data from the sNVM region and display the same on UART terminal.*/
        UART_polled_tx_string(&g_uart, g_separator);

        if (format == 1)
        {
           SYS_secure_nvm_read(addr, 0, &admin[0], text, 252u, 0);
           UART_polled_tx_string(&g_uart,(const uint8_t*)"\r\n Page admin data");
           display_output(&admin[0], 4);
           UART_polled_tx_string(&g_uart, (const uint8_t*)"\r\n Data read from sNVM region:");
           display_output(&text[0], 252);
        }
        else
        {
           SYS_secure_nvm_read(addr, user_key, &admin[0], text, 236u, 0);
           UART_polled_tx_string(&g_uart,(const uint8_t*)"\r\n Page admin data");
           display_output(&admin[0], 4);
           UART_polled_tx_string(&g_uart, (const uint8_t*)"\r\n Data read from sNVM region:");
           display_output(&text[0], 236);
        }
    }
    else
    {
        UART_polled_tx_string(&g_uart,
                              (const uint8_t*)"\r\n Secure NVM write fail.");
        UART_polled_tx_string(&g_uart,
                                      (const uint8_t*)"\r\n Status code:");
        display_output(&status, 1);
    }
}

/*==============================================================================
  Demonstrates  PUF emulation service.
 */
static void execute_puf_emulation_service(void)
{
    uint8_t challenge[16] = {0x00};
    uint8_t op_type[2] = {0x00};
    uint8_t response[32] = {0x00};
    uint8_t status = 0u;
    uint32_t count = 0u;

    /* Read challenge optype from UART terminal.*/
    get_input_data(&op_type[0], 1, optype_comment, sizeof(optype_comment));

    /* Read challenge from UART terminal.*/
    count = get_input_data(&challenge[0], sizeof(challenge), challenge_comment, sizeof(challenge_comment));
    revers_sequence(challenge, count);

    status = SYS_puf_emulation_service(challenge,op_type[0],response, 0);

    /* Display status information on UART terminal. */
    if(status == SYS_SUCCESS)
    {
    	UART_polled_tx_string(&g_uart,
                            (const uint8_t*)"\r\n\n puf emulation \
                                successful.\r\n Generated Response:");

    	display_output(&response[0], 32);
    }
    else
    {
        UART_polled_tx_string(&g_uart,
                              (const uint8_t*)"\r\n Service puf emulation failed.");
    }
}

/*==============================================================================
  Demonstrates  Digital signature service.
 */
static void execute_digital_signature_service(void)
{
    uint8_t hash[DIGITAL_SIGNATURE_HASH_LEN] = {0x00};
    uint8_t response[152] = {0x00};
    uint8_t format = 0u;
    uint8_t status = 0u;
    uint32_t count = 0u;

    /* Get the Digital signature format from user (1 byte).*/
    get_input_data(&format, sizeof(format), signature_format_comment, sizeof(signature_format_comment));

    /* Read hash value from UART terminal.*/
    count = get_input_data(&hash[0], sizeof(hash), read_hash_value_comment, sizeof(read_hash_value_comment));
    revers_sequence(hash, count);

    if (format == 1u)
    {
        status = SYS_digital_signature_service(hash,
                                               DIGITAL_SIGNATURE_RAW_FORMAT_REQUEST_CMD,
                                               response,
                                               0u);
    }
    else if (format == 2u)
    {
        status = SYS_digital_signature_service(hash,
                                               DIGITAL_SIGNATURE_DER_FORMAT_REQUEST_CMD,
                                               response,
                                               0u);
    }
    else
    {
    	status = SYS_PARAM_ERR;
    }

    /* Display status information on UART terminal. */
    if (status == SYS_SUCCESS)
    {
        UART_polled_tx_string(&g_uart,
                              (const uint8_t*)"\r\n Digital Signature service successful.");

        if (format == 1u)
        {
        	UART_polled_tx_string(&g_uart,
                                (const uint8_t*)"\r\n Output Digital Signature - Raw format:");

        	display_output(&response[0], 96);
        }
        else
        {
        	UART_polled_tx_string(&g_uart,
                                (const uint8_t*)"\r\n Output Digital Signature - DER format:");
        	display_output(&response[0], 104);
        }
    }
    else
    {
        UART_polled_tx_string(&g_uart,
                              (const uint8_t*)"\r\n Digital Signature service failed.");
    }
}
