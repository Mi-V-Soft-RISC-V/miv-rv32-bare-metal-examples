
/*
 *  Copyright 2018 Microchip Corporation.
 *  All rights reserved.
 *
 *  Configure ZL30722 device.
 *
 * SVN $Revision: $
 * SVN $Date: $
 */


#include <fpga_design_config/fpga_design_config.h>
#include "drivers/fpga_ip/CoreSPI/core_spi.h"

extern void print(uint8_t *string);

spi_instance_t g_spi;

void
ZL30722_SPI_init(void)
{
    SPI_init(&g_spi, CORESPI_BASE_ADDR, 32);
    SPI_configure_master_mode(&g_spi);
}

void
ZL30722_read(uint8_t *buffer, const int length)
{
    uint8_t rx_buff[1], tx_buff[4];
    int i;

    SPI_set_slave_select(&g_spi, SPI_SLAVE_0);

    /* select eeprom */
    tx_buff[0] = 0x02; // write command
    tx_buff[1] = 0x00;
    tx_buff[2] = 0x00;
    tx_buff[3] = 0x01; // eeprom select byte
    SPI_transfer_block(&g_spi, tx_buff, 4, rx_buff, 0);

    /* read eeprom */
    tx_buff[0] = 0x03; // read command
    for (i = 0; i < length; i++)
    {
        tx_buff[1] = (i >> 8); // high byte of address
        tx_buff[2] = (i & 0xff); // low byte of address
        SPI_transfer_block(&g_spi, tx_buff, 3, rx_buff, 1);
        buffer[i] = rx_buff[0];
    }

    SPI_clear_slave_select(&g_spi, SPI_SLAVE_0);
}

void
ZL30722_write(const uint8_t *buffer, const int length)
{
    uint8_t rx_buff[16], tx_buff[35];
    int i, j;

    SPI_set_slave_select(&g_spi, SPI_SLAVE_0);

    /* select eeprom */
    tx_buff[0] = 0x02; // write command
    tx_buff[1] = 0x00;
    tx_buff[2] = 0x00;
    tx_buff[3] = 0x01; // eeprom select byte
    SPI_transfer_block(&g_spi, tx_buff, 4, rx_buff, 0);

    /* write eeprom */
    for (j = 0; j < length; j += 32)
    {
        tx_buff[0] = 0x06; // write enable command
        SPI_transfer_block(&g_spi, tx_buff, 1, rx_buff, 0);

        tx_buff[0] = 0x02; // write command
        tx_buff[1] = (j >> 8); // high byte of address
        tx_buff[2] = (j & 0xff); // low byte of address

        for (i = 0; i < 32; i++)
            tx_buff[i + 3] = buffer[i + j];

        SPI_transfer_block(&g_spi, tx_buff, 35, rx_buff, 0);

        tx_buff[0] = 0x05; // read status command
        rx_buff[0] = 0x01;
        while (rx_buff[0] & 0x01)
            SPI_transfer_block(&g_spi, tx_buff, 1, rx_buff, 1);

        /* print status bar */
        print(".");
    }
    print("done\n\r");
    SPI_clear_slave_select(&g_spi, SPI_SLAVE_0);
}

void
ZL30722_compare(const uint8_t *image, const int length)
{
    uint8_t rx_buff[1], tx_buff[4];
    int i;
    int equal = 1;

    SPI_set_slave_select(&g_spi, SPI_SLAVE_0);

    /* select eeprom */
    tx_buff[0] = 0x02; // write command
    tx_buff[1] = 0x00;
    tx_buff[2] = 0x00;
    tx_buff[3] = 0x01; // eeprom select byte
    SPI_transfer_block(&g_spi, tx_buff, 4, rx_buff, 0);

    /* read eeprom */
    tx_buff[0] = 0x03; // read command
    for (i = 0; (i < length) && equal; i++)
    {
        tx_buff[1] = (i >> 8); // high byte of address
        tx_buff[2] = (i & 0xff); // low byte of address
        SPI_transfer_block(&g_spi, tx_buff, 3, rx_buff, 1);
        if (image[i] != rx_buff[0])
            equal = 0;
    }

    if (!equal)
    {
        print("ZL30722 eeprom not equal ZL30722 image file... writing eeprom\n\r");
        ZL30722_write(image, length);
    }
    else
    {
        print("ZL30722 eeprom equals ZL30722 image file... nothing to be done\n\r");
    }

    SPI_clear_slave_select(&g_spi, SPI_SLAVE_0);
}
