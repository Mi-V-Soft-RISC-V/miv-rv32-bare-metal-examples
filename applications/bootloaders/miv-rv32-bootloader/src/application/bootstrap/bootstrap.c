/*******************************************************************************
 * Copyright 2019-2023 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * Mi-V ESS Bootstrap support firmware.
 * This project allows you to write executable into the SPI flash or the I2C
 * EEPROM. The executable is expected to be available in the LSRAM memory using
 * the Libero memory client configurator.
 *
 */
#include <string.h>
#include "miv_rv32_hal/miv_rv32_hal.h"
#include "drivers/fpga_ip/CoreUARTapb/core_uart_apb.h"
#include "drivers/fpga_ip/miv_i2c/miv_i2c.h"
#include "drivers/off_chip/spi_flash/spi_flash.h"

#define FLASH_SECTOR_SIZE               65536   /* flash memory size */
#define FLASH_SECTORS                   128    // There are 126 sectors of 64KB size, using 124
#define FLASH_BLOCK_SIZE                4096   //Sectors compose of 4KB eraseable blocks
#define FLASH_SEGMENT_SIZE              256    // Write segment size is 256

#define FLASH_BLOCK_SEGMENTS            (FLASH_BLOCK_SIZE / FLASH_SEGMENT_SIZE)
#define FLASH_BYTE_SIZE                 (FLASH_SECTOR_SIZE * FLASH_SECTORS)
#define LAST_BLOCK_ADDR                 (FLASH_BYTE_SIZE - FLASH_BLOCK_SIZE)

#ifdef __UNUSED_CODE
static int test_flash(void);
static void mem_test(uint8_t *address);
static int read_program_from_flash(uint8_t *read_buf, uint32_t read_byte_length);
static void Bootloader_JumpToApplication(uint32_t stack_location, uint32_t reset_vector);
#endif

static int write_program_to_i2ceeprom(uint8_t *write_buf, uint32_t file_size);
static int write_program_to_flash(uint8_t *write_buf, uint32_t file_size);
static void copy_hex_to_i2ceeprom(void);
static void copy_hex_to_spiflash(void);

/*
 * Base address of LSRAM memory where the program is loaded.
 */
#define LSRAM_BASE_ADDRESS_LOAD         0x80000000
#define LSRAM_BASE_ADDRESS_WRITE        0x89000000

/*
 * Key value used to determine if valid data is contained in the SPI flash.
 */
#define SPI_FLASH_VALID_CONTENT_KEY     0xB5006BB1

const uint8_t g_bootstrap_choice[] =
"\r\n\r\n\
======================================================================================\r\n\
  MIV_ESS Bootstrap support utility to load binary executable from LSRRAM to \
  Non-Volatile memory  \r\n\
======================================================================================\r\n\
\r\n\
\r\n\
This program supports writing HEX data from Source LSRAM (@ Address 0x800000000) into \
a Non-Volatile memory\r\n\
\r\n\
\r\n\
Choose the destination Non-Volatile memory: \r\n\
 Type 0 to show this menu\r\n\
 Type 1 copy .hex from LSRAM to SPI Flash \r\n\
 Type 2 copy .hex from LSRAM to MikroBus EEPROM \r\n\
 ";

/*
 * Data structure stored at the beginning of SPI flash to indicate the size of
 * data stored inside the SPI flash. This is to avoid having to read the entire
 * flash content at boot time.
 * This data structure is one flash segment long (256 bytes).
 */
typedef struct
{
    uint32_t validity_key;
    uint32_t spi_content_byte_size;
    uint32_t dummy[62];
} flash_content_t;

/******************************************************************************
 * CoreUARTapb instance data.
 *****************************************************************************/
UART_instance_t g_uart;


/******************************************************************************
 * I2C instance data.
 *****************************************************************************/
#define I2C_XFR_DATA_LEN                258u       // 2 byte address + 256 bytes data
uint8_t target_slave_addr = 0x50;
uint8_t i2c_tx_buffer[I2C_XFR_DATA_LEN];
miv_i2c_instance_t g_miv_i2c_inst;


volatile uint32_t g_10ms_count;

const uint8_t g_greeting_msg_spi[] =
" ----> SPI Flash is chosen as destination memory \r\n";

const char * g_greeting_msg_i2c =
" ----> I2C EEPROM is chosen as destination memory \r\n";

/*
 * Maximum UART receiver buffer size.
 */
#define UART_RX_BUF_SIZE    10

/*
 * Used for bootstrap from SPI FLASH
 * The LSRAM max size is 64K.
 * Assuming the executable 32k byte.
 * Copy the whole 32k chunk from LSRAM to flash.
 */
#define FLASH_EXECUTABLE_SIZE    32768u

/* MIV I2C interrupt handler */
void MSYS_EI2_IRQHandler(void)
{
    MIV_I2C_isr (&g_miv_i2c_inst);
}

void SysTick_Handler(void)
{
    g_10ms_count += 10;

      /*
       * For neatness, if we roll over, reset cleanly back to 0 so the count
       * always goes up in proper 10s.
       */
     if(g_10ms_count < 10)
         g_10ms_count = 0;

}
/*-------------------------------------------------------------------------*//**
 * main() function.
 */
int main()
{
    uint8_t rx_data[UART_RX_BUF_SIZE];
    size_t rx_size;
    static uint32_t file_size = 0;
    /**************************************************************************
     * Initialize CoreUARTapb with its base address, baud value, and line
     * configuration.
     *************************************************************************/
    UART_init(&g_uart, COREUARTAPB0_BASE_ADDR,\
              BAUD_VALUE_115200, (DATA_8_BITS | NO_PARITY) );

    /**************************************************************************
     * Display greeting message message.
     *************************************************************************/
    UART_polled_tx_string(&g_uart, g_bootstrap_choice);

    while (1u)
    {
        /**********************************************************************
         * Read data received by the UART.
         *********************************************************************/
        rx_size = UART_get_rx( &g_uart, rx_data, sizeof(rx_data) );

        /**********************************************************************
         * Echo back data received, if any.
         *********************************************************************/
        if ( rx_size > 0 )
        {
            UART_send( &g_uart, rx_data, rx_size );

            switch(rx_data[0])
            {
            case '0':
                UART_polled_tx_string( &g_uart, g_bootstrap_choice);
                break;
            case '1':
                copy_hex_to_spiflash();
                break;
            case '2':
                copy_hex_to_i2ceeprom();
                break;
            case '3':
                UART_polled_tx_string( &g_uart,
                               "Run the Bootloader-* build configurations \
                               if you are looking for YModem functionality \r\n");
                break;
            default:
                UART_polled_tx_string( &g_uart,
                                "Invalid selection. Try again...\r\n");
                break;
            }
        }
    }

    return 0;
}

void copy_hex_to_i2ceeprom(void)
{
    uint8_t rx_size = 0u;
    uint8_t rx_data[UART_RX_BUF_SIZE];

    UART_polled_tx_string(&g_uart, g_greeting_msg_i2c);
    MIV_I2C_init(&g_miv_i2c_inst, MIV_I2C_BASE_ADDR);   //For ~100kHz I2C Clock

    MIV_I2C_config(&g_miv_i2c_inst, 0x0063);

    /* Enable the global external interrupt bit.
       This must be done for all Mi-V cores to enable interrupts globally. */
    HAL_enable_interrupts();

#ifndef MIV_LEGACY_RV32
    MRV_enable_local_irq(MRV32_MSYS_EIE0_IRQn |
                         MRV32_MSYS_EIE1_IRQn |
                         MRV32_MSYS_EIE2_IRQn |
                         MRV32_MSYS_EIE3_IRQn |
                         MRV32_MSYS_EIE4_IRQn |
                         MRV32_MSYS_EIE5_IRQn);;

#endif

    MRV_systick_config(SYS_CLK_FREQ);

    /* The LSRAM used in the Libero Design has a size of 64KB. However,
     * it is important to note that the maximum size of the I2C eeprom
     * when copying from the LSRAM is 32KB
     */
    write_program_to_i2ceeprom((uint8_t *)LSRAM_BASE_ADDRESS_LOAD, FLASH_EXECUTABLE_SIZE);
}

void copy_hex_to_spiflash(void)
{
    spi_flash_init(FLASH_CORE_SPI_BASE);

    /* The LSRAM used in the Libero Design has a size of 64KB. However,
     * it is important to note that the maximum size of the SPI flash
     * when copying from the LSRAM is 32KB
     */
    write_program_to_flash((uint8_t *)LSRAM_BASE_ADDRESS_LOAD, FLASH_EXECUTABLE_SIZE);
}

/*
 *  Write to I2C EEPROM
 */
static int write_program_to_i2ceeprom(uint8_t *write_buf, uint32_t file_size)
{
    uint32_t mem_addr = 0x80000000; // source address
    uint32_t mem_val;               // read data word from source
    uint8_t page_no;                // I2C device page no. Each page is 256 Bytes. 256 x 64 = 16 KB
    miv_i2c_status_t   status;
    volatile uint8_t miv_i2c_status = 0u;
    UART_polled_tx_string(&g_uart,
                         (const uint8_t *)"\r\nWriting Data into EEPROM using MIV_I2C\n\r");
    for (page_no = 0; page_no <= 127 ; page_no++) //32KB = 128 pages of 256 Bytes
    {
        uint16_t n = 0;
        i2c_tx_buffer[0] = page_no; // 1st word address byte (needs to increment for pages)
        i2c_tx_buffer[1] = 0x00;    // 2nd word address byte - always zero

        // write 256 byte loop
        for (n = 0; n < (I2C_XFR_DATA_LEN-2); n = n + 4, mem_addr = mem_addr + 4)
        {
            mem_val = HW_get_32bit_reg (mem_addr); // get a word from source memory
            i2c_tx_buffer[5+n] =  mem_val >> 24;    //bits 31:24
            i2c_tx_buffer[4+n] =  mem_val >> 16;    //bits 23:16
            i2c_tx_buffer[3+n] =  mem_val >> 8;     //bits 15:8
            i2c_tx_buffer[2+n] =  mem_val;          //bits 7:0
        }

        n = 0u;
        // Write a page of data into EEPROM using the MIV_I2C
        // The start address of each page is included in the first two bytes of the write
        MIV_I2C_write(&g_miv_i2c_inst,
                      target_slave_addr,
                      i2c_tx_buffer,
                      I2C_XFR_DATA_LEN,
                      MIV_I2C_RELEASE_BUS,
                      MIV_I2C_ACK_POLLING_ENABLE);
        do {
            miv_i2c_status = g_miv_i2c_inst.master_status;
        }while (MIV_I2C_IN_PROGRESS == miv_i2c_status);
    }
    UART_polled_tx_string(&g_uart, (const uint8_t *)"\r\nMIV_I2C Write Complete!\n\r");
    return 0u;
}
/*
 *  Write to flash memory
 */
static int write_program_to_flash(uint8_t *write_buf, uint32_t file_size)
{
    uint8_t write_buffer[FLASH_SEGMENT_SIZE];
    uint8_t read_buffer[FLASH_SEGMENT_SIZE];
    uint16_t status;
    int flash_address = 0;
    int count = 0;
    spi_flash_status_t result;
    struct device_Info DevInfo;

    UART_polled_tx_string( &g_uart,
        "\r\n---------------------- Writing SPI flash from DDR memory ----------------------\r\n" );
    UART_polled_tx_string( &g_uart,
        "This may take several minutes to complete if writing a large file.\r\n" );

    spi_flash_control_hw( SPI_FLASH_RESET, 0, &status );

    result = spi_flash_control_hw(SPI_FLASH_READ_DEVICE_ID,
                                  count * FLASH_SECTOR_SIZE,
                                  &DevInfo);

    result = spi_flash_control_hw(SPI_FLASH_READ_DEVICE_ID,
                                  count * FLASH_SECTOR_SIZE,
                                  &DevInfo);

    /*--------------------------------------------------------------------------
     * First fetch status register. First byte in low 8 bits, second byte in
     * upper 8 bits.
     */
    result = spi_flash_control_hw( SPI_FLASH_GET_STATUS, 0, &status );

    result = spi_flash_control_hw(SPI_FLASH_READ_DEVICE_ID,
                                  count * FLASH_SECTOR_SIZE,
                                  &DevInfo );

    /*--------------------------------------------------------------------------
     * Fetch protection register value for each of the 128 sectors.
     * After power up these should all read as 0xFF
     */
    for( count = 0; count != 128; ++count )
    {
        result = spi_flash_control_hw(SPI_FLASH_GET_PROTECT,
                                      count * FLASH_SECTOR_SIZE,
                                      &read_buffer[count] );
    }

    //device D
    result = spi_flash_control_hw(SPI_FLASH_READ_DEVICE_ID,
                                  count * FLASH_SECTOR_SIZE,
                                  &DevInfo );

    /*--------------------------------------------------------------------------
     * Show sector protection in action by:
     *   - unprotecting the first sector
     *   - erasing the sector
     *   - writing some data to the first 256 bytes
     *   - protecting the first sector
     *   - erasing the first sector
     *   - reading back the first 256 bytes of the first sector
     *   - unprotecting the first sector
     *   - erasing the sector
     *   - reading back the first 256 bytes of the first sector
     *
     * The first read should still show the written data in place as the erase
     * will fail. the second read should show all 0xFFs. Step through the code
     * in debug mode and examine the read buffer after the read operations to
     * see this.
     */
    result = spi_flash_control_hw( SPI_FLASH_SECTOR_UNPROTECT, flash_address, NULL );
    //device D   works
    result = spi_flash_control_hw(SPI_FLASH_READ_DEVICE_ID,
                                  count * FLASH_SECTOR_SIZE,
                                  &DevInfo );
    result = spi_flash_control_hw( SPI_FLASH_4KBLOCK_ERASE, flash_address , NULL );

    //device D-- now working
    result = spi_flash_control_hw(SPI_FLASH_READ_DEVICE_ID,
                                  count * FLASH_SECTOR_SIZE,
                                  &DevInfo);

    memset( write_buffer, count, FLASH_SEGMENT_SIZE );
    strcpy( (char *)write_buffer, "Microsemi FLASH test" );
    spi_flash_write( flash_address, write_buffer, FLASH_SEGMENT_SIZE );
    //device D --
    result = spi_flash_control_hw(SPI_FLASH_READ_DEVICE_ID,
                                  count * FLASH_SECTOR_SIZE,
                                  &DevInfo);

    result = spi_flash_control_hw( SPI_FLASH_SECTOR_PROTECT, flash_address, NULL );
    //device D
    result = spi_flash_control_hw(SPI_FLASH_READ_DEVICE_ID,
                                  count * FLASH_SECTOR_SIZE,
                                  &DevInfo );
    result = spi_flash_control_hw( SPI_FLASH_4KBLOCK_ERASE, flash_address , NULL );
    //device D
    result = spi_flash_control_hw(SPI_FLASH_READ_DEVICE_ID,
                                  count * FLASH_SECTOR_SIZE,
                                  &DevInfo );
    result = spi_flash_control_hw( SPI_FLASH_GET_STATUS, 0, &status );
    //device D
    result = spi_flash_control_hw(SPI_FLASH_READ_DEVICE_ID,
                                  count * FLASH_SECTOR_SIZE,
                                  &DevInfo);

    spi_flash_read ( flash_address, read_buffer, FLASH_SEGMENT_SIZE);
    //device D
    result = spi_flash_control_hw(SPI_FLASH_READ_DEVICE_ID,
                                  count * FLASH_SECTOR_SIZE,
                                  &DevInfo);

    result = spi_flash_control_hw( SPI_FLASH_SECTOR_UNPROTECT, flash_address, NULL );
    //device D
    result = spi_flash_control_hw(SPI_FLASH_READ_DEVICE_ID,
                                  count * FLASH_SECTOR_SIZE,
                                  &DevInfo);
    result = spi_flash_control_hw( SPI_FLASH_4KBLOCK_ERASE, flash_address , NULL );
    //device D
    result = spi_flash_control_hw(SPI_FLASH_READ_DEVICE_ID,
                                  count * FLASH_SECTOR_SIZE,
                                  &DevInfo);
    result = spi_flash_control_hw( SPI_FLASH_GET_STATUS, 0, &status );
    //device D
    result = spi_flash_control_hw(SPI_FLASH_READ_DEVICE_ID,
                                  count * FLASH_SECTOR_SIZE,
                                  &DevInfo);

    result = spi_flash_control_hw( SPI_FLASH_SECTOR_UNPROTECT, flash_address, NULL );
    //device D
    result = spi_flash_control_hw(SPI_FLASH_READ_DEVICE_ID,
                                  count * FLASH_SECTOR_SIZE,
                                  &DevInfo);
    result = spi_flash_control_hw(SPI_FLASH_4KBLOCK_ERASE, flash_address , NULL );
    //device D
    result = spi_flash_control_hw(SPI_FLASH_READ_DEVICE_ID,
                                  count * FLASH_SECTOR_SIZE,
                                  &DevInfo);

    spi_flash_read ( flash_address, read_buffer, FLASH_SEGMENT_SIZE );
    //device D
    result = spi_flash_control_hw(SPI_FLASH_READ_DEVICE_ID,
                                  count * FLASH_SECTOR_SIZE,
                                  &DevInfo);
    /*--------------------------------------------------------------------------
     * Read the protection registers again so you can see that the first sector
     * is unprotected now.
     */
    for( count = 0; count != 128; ++count )
    {
        spi_flash_control_hw(SPI_FLASH_GET_PROTECT,
                             count * FLASH_SECTOR_SIZE,
                             &write_buffer[count]);
    }
    //device D
    result = spi_flash_control_hw(SPI_FLASH_READ_DEVICE_ID,
                                  count * FLASH_SECTOR_SIZE,
                                  &DevInfo);
    /*--------------------------------------------------------------------------
     * Write something to all 32768 blocks of 256 bytes in the 8MB FLASH.
     */
    uint32_t nb_blocks_to_write;
    nb_blocks_to_write = (file_size / FLASH_SEGMENT_SIZE);

    if ((file_size % FLASH_SEGMENT_SIZE) > 0)
    {
        ++nb_blocks_to_write;
    }

    for( count = 0; count != nb_blocks_to_write; ++count )
    {
        /*----------------------------------------------------------------------
         * Vary the fill for each chunk of 256 bytes
         */
        memset( write_buffer, count, FLASH_SEGMENT_SIZE );
        strcpy( (char *)write_buffer, "Microsemi FLASH test" );
        /*----------------------------------------------------------------------
         * at the start of each sector we need to make sure it is unprotected
         * so we can erase blocks within it. The spi_flash_write() function
         * unprotects the sector as well but we need to start erasing before the
         * first write takes place.
         */
        if(0 == (flash_address % FLASH_SECTOR_SIZE))
        {
            result = spi_flash_control_hw(SPI_FLASH_SECTOR_UNPROTECT,
                                          flash_address,
                                          NULL);
        }
        /*----------------------------------------------------------------------
         * At the start of each 4K block we issue an erase so that we are then
         * free to write anything we want to the block. If we don't do this the
         * write may fail as we can only effectively turn 1s to 0s when we
         * write. For example if we have an erased location with 0xFF in it and
         * we write 0xAA to it first and then later on write 0x55, the resulting
         * value is 0x00...
         */
        if(0 == (flash_address % FLASH_BLOCK_SIZE))
        {
            result = spi_flash_control_hw(SPI_FLASH_4KBLOCK_ERASE,
                                          flash_address,
                                          NULL);
        }
        /*----------------------------------------------------------------------
         * Write our values to the FLASH, read them back and compare.
         * Placing a breakpoint on the while statement below will allow
         * you break on any failures.
         */
        spi_flash_write( flash_address, write_buf, FLASH_SEGMENT_SIZE );

        spi_flash_read ( flash_address, read_buffer, FLASH_SEGMENT_SIZE );
        if( memcmp( write_buf, read_buffer, FLASH_SEGMENT_SIZE ) )
        {
            while(1) // Breakpoint here will trap write faults
            {

            }

        }
        write_buf += FLASH_SEGMENT_SIZE;
        flash_address += FLASH_SEGMENT_SIZE; /* Step to the next 256 byte chunk */
        //show_progress();
    }

    /*--------------------------------------------------------------------------
     * Record the size written in the first SPI flash segment.
     */
    {
        flash_content_t flash_content;

        flash_content.validity_key = SPI_FLASH_VALID_CONTENT_KEY;
        flash_content.spi_content_byte_size = file_size;

        flash_address = LAST_BLOCK_ADDR;

        /*----------------------------------------------------------------------
         * at the start of each sector we need to make sure it is unprotected
         * so we can erase blocks within it. The spi_flash_write() function
         * unprotects the sector as well but we need to start erasing before the
         * first write takes place.
         */
        if(0 == (flash_address % FLASH_SECTOR_SIZE))
        {
            result = spi_flash_control_hw(SPI_FLASH_SECTOR_UNPROTECT,
                                          flash_address,
                                          NULL);
        }
        /*----------------------------------------------------------------------
         * At the start of each 4K block we issue an erase so that we are then
         * free to write anything we want to the block. If we don't do this the
         * write may fail as we can only effectively turn 1s to 0s when we
         * write. For example if we have an erased location with 0xFF in it and
         * we write 0xAA to it first and then later on write 0x55, the resulting
         * value is 0x00...
         */
        if(0 == (flash_address % FLASH_BLOCK_SIZE))
        {
            result = spi_flash_control_hw(SPI_FLASH_4KBLOCK_ERASE,
                                          flash_address,
                                          NULL);
        }
        /*----------------------------------------------------------------------
         * Write our values to the FLASH, read them back and compare.
         * Placing a breakpoint on the while statement below will allow
         * you break on any failures.
         */
        spi_flash_write( flash_address, (uint8_t *)(&flash_content), FLASH_SEGMENT_SIZE );

        spi_flash_read ( flash_address, read_buffer, FLASH_SEGMENT_SIZE );

        if( memcmp( (uint8_t *)&flash_content, read_buffer, FLASH_SEGMENT_SIZE ) )
        {
            while(1) // Breakpoint here will trap write faults
            {

            }
        }

        write_buf += FLASH_SEGMENT_SIZE;
        flash_address += FLASH_SEGMENT_SIZE; /* Step to the next 256 byte chunk */
    }

    /*--------------------------------------------------------------------------
     * One last look at the protection registers which should all be 0 now
     */
    for( count = 0; count != 128; ++count )
    {
        spi_flash_control_hw(SPI_FLASH_GET_PROTECT,
                             count * FLASH_SECTOR_SIZE,
                             &write_buffer[count]);
    }

    UART_polled_tx_string( &g_uart, "Flash write success\r\n" );

    return(0);
}

#ifdef __UNUSED_CODE
/*-------------------------------------------------------------------------*//**
 * Test flash on RTG4
 */
static int test_flash(void)
{
    uint8_t write_buffer[FLASH_SEGMENT_SIZE];
    uint8_t read_buffer[FLASH_SEGMENT_SIZE];
    uint16_t status;
    int flash_address = 0;
    int count = 0;
    spi_flash_status_t result;
    struct device_Info DevInfo;

    spi_flash_control_hw( SPI_FLASH_RESET, 0, &status );

    result = spi_flash_control_hw( SPI_FLASH_READ_DEVICE_ID,
            count * FLASH_SECTOR_SIZE,
            &DevInfo );

    result = spi_flash_control_hw( SPI_FLASH_READ_DEVICE_ID,
            count * FLASH_SECTOR_SIZE,
            &DevInfo );

    /*--------------------------------------------------------------------------
     * First fetch status register. First byte in low 8 bits, second byte in
     * upper 8 bits.
     */
    result = spi_flash_control_hw( SPI_FLASH_GET_STATUS, 0, &status );

    result = spi_flash_control_hw( SPI_FLASH_READ_DEVICE_ID,
            count * FLASH_SECTOR_SIZE,
            &DevInfo );

    /*--------------------------------------------------------------------------
     * Fetch protection register value for each of the 128 sectors.
     * After power up these should all read as 0xFF
     */
    for( count = 0; count != 128; ++count )
    {
        result = spi_flash_control_hw( SPI_FLASH_GET_PROTECT,
                count * FLASH_SECTOR_SIZE,
                &read_buffer[count] );
    }

    //device D
    result = spi_flash_control_hw( SPI_FLASH_READ_DEVICE_ID,
            count * FLASH_SECTOR_SIZE,
            &DevInfo );

    /*--------------------------------------------------------------------------
     * Show sector protection in action by:
     *   - unprotecting the first sector
     *   - erasing the sector
     *   - writing some data to the first 256 bytes
     *   - protecting the first sector
     *   - erasing the first sector
     *   - reading back the first 256 bytes of the first sector
     *   - unprotecting the first sector
     *   - erasing the sector
     *   - reading back the first 256 bytes of the first sector
     *
     * The first read should still show the written data in place as the erase
     * will fail. the second read should show all 0xFFs. Step through the code
     * in debug mode and examine the read buffer after the read operations to
     * see this.
     */
    result = spi_flash_control_hw( SPI_FLASH_SECTOR_UNPROTECT, flash_address, NULL );
    //device D   works
    result = spi_flash_control_hw( SPI_FLASH_READ_DEVICE_ID,
            count * FLASH_SECTOR_SIZE,
            &DevInfo );
    result = spi_flash_control_hw( SPI_FLASH_4KBLOCK_ERASE, flash_address , NULL );
    //device D-- now working
    result = spi_flash_control_hw( SPI_FLASH_READ_DEVICE_ID,
            count * FLASH_SECTOR_SIZE,
            &DevInfo );
    memset( write_buffer, count, FLASH_SEGMENT_SIZE );
    strcpy( (char *)write_buffer, "Microsemi FLASH test" );

    spi_flash_write( flash_address, write_buffer, FLASH_SEGMENT_SIZE );
    //device D --
    result = spi_flash_control_hw( SPI_FLASH_READ_DEVICE_ID,
            count * FLASH_SECTOR_SIZE,
            &DevInfo );

    result = spi_flash_control_hw( SPI_FLASH_SECTOR_PROTECT, flash_address, NULL );
    //device D
    result = spi_flash_control_hw( SPI_FLASH_READ_DEVICE_ID,
            count * FLASH_SECTOR_SIZE,
            &DevInfo );
    result = spi_flash_control_hw( SPI_FLASH_4KBLOCK_ERASE, flash_address , NULL );
    //device D
    result = spi_flash_control_hw( SPI_FLASH_READ_DEVICE_ID,
            count * FLASH_SECTOR_SIZE,
            &DevInfo );
    result = spi_flash_control_hw( SPI_FLASH_GET_STATUS, 0, &status );
    //device D
    result = spi_flash_control_hw( SPI_FLASH_READ_DEVICE_ID,
            count * FLASH_SECTOR_SIZE,
            &DevInfo );

    spi_flash_read ( flash_address, read_buffer, FLASH_SEGMENT_SIZE);
    //device D
    result = spi_flash_control_hw( SPI_FLASH_READ_DEVICE_ID,
            count * FLASH_SECTOR_SIZE,
            &DevInfo );

    result = spi_flash_control_hw( SPI_FLASH_SECTOR_UNPROTECT, flash_address, NULL );
    //device D
    result = spi_flash_control_hw( SPI_FLASH_READ_DEVICE_ID,
            count * FLASH_SECTOR_SIZE,
            &DevInfo );
    result = spi_flash_control_hw( SPI_FLASH_4KBLOCK_ERASE, flash_address , NULL );
    //device D
    result = spi_flash_control_hw( SPI_FLASH_READ_DEVICE_ID,
            count * FLASH_SECTOR_SIZE,
            &DevInfo );
    result = spi_flash_control_hw( SPI_FLASH_GET_STATUS, 0, &status );
    //device D
    result = spi_flash_control_hw( SPI_FLASH_READ_DEVICE_ID,
            count * FLASH_SECTOR_SIZE,
            &DevInfo );

    result = spi_flash_control_hw( SPI_FLASH_SECTOR_UNPROTECT, flash_address, NULL );
    //device D
    result = spi_flash_control_hw( SPI_FLASH_READ_DEVICE_ID,
            count * FLASH_SECTOR_SIZE,
            &DevInfo );
    result = spi_flash_control_hw( SPI_FLASH_4KBLOCK_ERASE, flash_address , NULL );
    //device D
    result = spi_flash_control_hw( SPI_FLASH_READ_DEVICE_ID,
            count * FLASH_SECTOR_SIZE,
            &DevInfo );

    spi_flash_read ( flash_address, read_buffer, FLASH_SEGMENT_SIZE );
    //device D
    result = spi_flash_control_hw( SPI_FLASH_READ_DEVICE_ID,
            count * FLASH_SECTOR_SIZE,
            &DevInfo );
    /*--------------------------------------------------------------------------
     * Read the protection registers again so you can see that the first sector
     * is unprotected now.
     */
    for( count = 0; count != 128; ++count )
    {
        spi_flash_control_hw( SPI_FLASH_GET_PROTECT, count * FLASH_SECTOR_SIZE,
                &write_buffer[count] );
    }
    //device D
    result = spi_flash_control_hw( SPI_FLASH_READ_DEVICE_ID,
            count * FLASH_SECTOR_SIZE,
            &DevInfo );
    /*--------------------------------------------------------------------------
     * Write something to all 32768 blocks of 256 bytes in the 8MB FLASH.
     */
    for( count = 0; count != 1000 /*32768*/; ++count )
    {
        /*----------------------------------------------------------------------
         * Vary the fill for each chunk of 256 bytes
         */
        memset( write_buffer, count, FLASH_SEGMENT_SIZE );
        strcpy( (char *)write_buffer, "Microsemi FLASH test" );
        /*----------------------------------------------------------------------
         * at the start of each sector we need to make sure it is unprotected
         * so we can erase blocks within it. The spi_flash_write() function
         * unprotects the sector as well but we need to start erasing before the
         * first write takes place.
         */
        if(0 == (flash_address % FLASH_SECTOR_SIZE))
        {
            result = spi_flash_control_hw( SPI_FLASH_SECTOR_UNPROTECT, flash_address, NULL );
        }
        /*----------------------------------------------------------------------
         * At the start of each 4K block we issue an erase so that we are then
         * free to write anything we want to the block. If we don't do this the
         * write may fail as we can only effectively turn 1s to 0s when we
         * write. For example if we have an erased location with 0xFF in it and
         * we write 0xAA to it first and then later on write 0x55, the resulting
         * value is 0x00...
         */
        if(0 == (flash_address % FLASH_BLOCK_SIZE))
        {
            result = spi_flash_control_hw( SPI_FLASH_4KBLOCK_ERASE, flash_address , NULL );
        }
        /*----------------------------------------------------------------------
         * Write our values to the FLASH, read them back and compare.
         * Placing a breakpoint on the while statement below will allow
         * you break on any failures.
         */
        spi_flash_write( flash_address, write_buffer, FLASH_SEGMENT_SIZE );
        spi_flash_read ( flash_address, read_buffer, FLASH_SEGMENT_SIZE );
        if( memcmp( write_buffer, read_buffer, FLASH_SEGMENT_SIZE ) )
        {
            while(1) // Breakpoint here will trap write faults
            {
                result = spi_flash_control_hw( SPI_FLASH_READ_DEVICE_ID,
                        count * FLASH_SECTOR_SIZE,
                        &DevInfo );
                spi_flash_control_hw( SPI_FLASH_RESET, 0, &status );

            }

        }

        flash_address += FLASH_SEGMENT_SIZE; /* Step to the next 256 byte chunk */
    }

    /*--------------------------------------------------------------------------
     * One last look at the protection registers which should all be 0 now
     */
    for( count = 0; count != 128; ++count )
    {
        spi_flash_control_hw(SPI_FLASH_GET_PROTECT,
                             count * FLASH_SECTOR_SIZE,
                             &write_buffer[count]);
    }

    UART_polled_tx_string( &g_uart, "  Flash test success\r\n" );

    return(0);
}

/**
 *  Read from flash
 */
static int read_program_from_flash(uint8_t *read_buf, uint32_t read_byte_length)
{
    uint16_t status;
    int flash_address = 0;
    int count = 0;
    uint32_t nb_segments_to_read;
    spi_flash_status_t result;
    struct device_Info DevInfo;
    flash_content_t flash_content;

    UART_polled_tx_string( &g_uart,
        "\r\n------------------- Reading from SPI flash into TCM memory --------------------\r\n" );
    UART_polled_tx_string( &g_uart,
        "This will take several minutes to complete in order to read the full SPI \
        flash \r\ncontent.\r\n" );

    spi_flash_control_hw( SPI_FLASH_RESET, 0, &status );

    result = spi_flash_control_hw(SPI_FLASH_READ_DEVICE_ID,
                                  count * FLASH_SECTOR_SIZE,
                                  &DevInfo);

    result = spi_flash_control_hw(SPI_FLASH_READ_DEVICE_ID,
                                  count * FLASH_SECTOR_SIZE,
                                  &DevInfo);

    /*--------------------------------------------------------------------------
     * First fetch status register. First byte in low 8 bits, second byte in
     * upper 8 bits.
     */
    result = spi_flash_control_hw(SPI_FLASH_GET_STATUS, 0, &status);

    result = spi_flash_control_hw(SPI_FLASH_READ_DEVICE_ID,
                                  count * FLASH_SECTOR_SIZE,
                                  &DevInfo);

    /*--------------------------------------------------------------------------
     * Retrieve the size of the data previously written to SPI flash.
     */
    spi_flash_read ( LAST_BLOCK_ADDR, (uint8_t *)&flash_content, FLASH_SEGMENT_SIZE );

    if(SPI_FLASH_VALID_CONTENT_KEY == flash_content.validity_key)
    {
        read_byte_length = flash_content.spi_content_byte_size;
    }
    else
    {
        read_byte_length = 0;
    }

    /*--------------------------------------------------------------------------
     * Read from flash 256 bytes increments (FLASH_SEGMENT_SIZE).
     */
    nb_segments_to_read = read_byte_length / FLASH_SEGMENT_SIZE;
    if((read_byte_length % FLASH_SEGMENT_SIZE) > 0)
    {
        ++nb_segments_to_read;
    }

    for( count = 0; count != nb_segments_to_read; ++count )
    {
        /*----------------------------------------------------------------------
         * Write our values to the FLASH, read them back and compare.
         * Placing a breakpoint on the while statement below will allow
         * you break on any failures.
         */

        spi_flash_read ( flash_address, read_buf, FLASH_SEGMENT_SIZE );
        read_buf += FLASH_SEGMENT_SIZE;

        flash_address += FLASH_SEGMENT_SIZE; /* Step to the next 256 byte chunk */
    }

    UART_polled_tx_string( &g_uart, "  Flash read success\r\n" );

    return(0);
}

/*
 * Simple sanity check
 */
static void mem_test(uint8_t *address)
{
    volatile uint8_t value=2;
    volatile uint32_t value32=3;
    *address = 1;
    value = *address;
    value32 = (uint32_t)*address;

    if((value32 == value) &&(value == 1))
        UART_polled_tx_string( &g_uart, "  Read/Write success\r\n" );
    else
        UART_polled_tx_string( &g_uart, "  Read/Write fail\r\n" );
}

/*------------------------------------------------------------------------------
 * Call this function if you want to switch to another program
 * de-init any loaded drivers before calling this function
 */
//volatile uint32_t cj_debug;
static void Bootloader_JumpToApplication(uint32_t stack_location, uint32_t reset_vector)
{
     /*
      * The bootstrap is going to copy the program from NV memory to the TCM.
      * The TCM start address is 0x40000000. Jump to that address.
      */
    __asm__ volatile ("fence.i");
    __asm__ volatile("lui ra,0x40000");
    __asm__ volatile("ret");
    /*User application execution should now start and never return here.... */
}

#endif /*__UNUSED_CODE*/
