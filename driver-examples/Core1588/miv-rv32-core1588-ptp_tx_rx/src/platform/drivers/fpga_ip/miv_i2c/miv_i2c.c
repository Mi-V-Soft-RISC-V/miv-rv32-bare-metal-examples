/*******************************************************************************
 * Copyright 2022 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * Mi-V I2C Soft IP bare-metal driver. This module is delivered as part of
 * Extended Sub System(ESS) MIV_ESS.
 * Please refer to miv_i2c.h file for more information.
 */

#include "miv_i2c.h"

#define MIV_I2C_ERROR                                   0xFFu

/*------------------------------------------------------------------------------
 * MIV I2C transaction direction.
 */
#define MIV_I2C_WRITE_DIR                               0u
#define MIV_I2C_READ_DIR                                1u

/* -- TRANSACTIONS TYPES -- */
#define MIV_I2C_NO_TRANSACTION                          0u
#define MIV_I2C_MASTER_WRITE_TRANSACTION                1u
#define MIV_I2C_MASTER_READ_TRANSACTION                 2u
#define MIV_I2C_MASTER_SEQUENTIAL_READ_TRANSACTION      3u

/*------------------------------------------------------------------------------
 * MIV I2C HW states
 */
#define MIV_I2C_IDLE                                    0x00u
#define MIV_I2C_TX_STA_CB                               0x01u
#define MIV_I2C_TX_DATA                                 0x02u
#define MIV_I2C_RX_DATA                                 0x03u

/*-------------------------------------------------------------------------*//**
  The MIV_I2C_disable_irq() disables the Mi-V I2C interrupt.
 */
void
MIV_I2C_disable_irq
(
    void
);

/*-------------------------------------------------------------------------*//**
  The MIV_I2C_enable_irq() enables the Mi-V I2C interrupt.
 */
void
MIV_I2C_enable_irq
(
    void
);

/*
 * Please refer to miv_i2c.h for more info
 */
void
MIV_I2C_init
(
    miv_i2c_instance_t *this_i2c,
    addr_t base_addr
)
{
    /* Assign the base address
     * Clock Prescale value set
     * MIV_I2C interrupt enabled
     * I2C core enable
     */
     psr_t processor_state;

     /* Disabling the interrupts */
     processor_state = HAL_disable_interrupts();

     /*
      * Initialize all items of the this_miv_i2c data structure to zero. This
      * initializes all state variables to their init value. It relies on
      * the fact that NO_TRANSACTION, MIV_I2C_SUCCESS and I2C_RELEASE_BUS all
      * have an actual value of zero.
      */
     memset(this_i2c, 0, sizeof(miv_i2c_instance_t));

     this_i2c->base_addr = base_addr;

     HAL_restore_interrupts(processor_state);
}

/*
 * Please refer to miv_i2c.h for more info
 */
void
MIV_I2C_config
(
    miv_i2c_instance_t *this_i2c,
    uint16_t clk_prescale
)
{
    /* Assign the base address
     * Clock Prescale value set
     * MIV_I2C interrupt enabled
     * I2C core enable
     */
     psr_t processor_state;

     /* Disabling the interrupts */
     processor_state = HAL_disable_interrupts();

     /* Before writing to prescale reg, the core enable must be zero */
     HAL_set_8bit_reg_field(this_i2c->base_addr, CTRL_CORE_EN, 0x00u);

     /* Set the prescale value */
     HAL_set_16bit_reg(this_i2c->base_addr, PRESCALE, clk_prescale);

     /* Enable the MIV I2C interrupts */
     HAL_set_8bit_reg_field(this_i2c->base_addr, CTRL_IRQ_EN, 0x01u);

     /* Enable the MIV I2C core */
     HAL_set_8bit_reg_field(this_i2c->base_addr, CTRL_CORE_EN, 0x01u);

     this_i2c->master_state = MIV_I2C_IDLE;

     HAL_restore_interrupts(processor_state);
}

/*
 * Please refer to miv_i2c.h for more info
 */
uint8_t
MIV_I2C_start
(
    miv_i2c_instance_t *this_i2c
)
{
    psr_t processor_state;

    processor_state = HAL_disable_interrupts();

    /* Generate I2C start condition */
    HAL_set_8bit_reg_field(this_i2c->base_addr, CMD_STA, 0x01u);

    HAL_set_8bit_reg_field(this_i2c->base_addr, CMD_WR, 0x01);

    HAL_restore_interrupts(processor_state);

    return 0u;
}

/*
 * Please refer to miv_i2c.h for more info
 */
uint8_t
MIV_I2C_stop
(
    miv_i2c_instance_t *this_i2c
)
{
    psr_t processor_state;

    processor_state = HAL_disable_interrupts();

    /* Generate I2C stop condition */
    HAL_set_8bit_reg_field(this_i2c->base_addr, CMD_STO, 0x01u);

    HAL_set_8bit_reg_field(this_i2c->base_addr, CMD_WR, 0x01);

    HAL_restore_interrupts(processor_state);

    return 0u;
}

/*
 * Please refer to miv_i2c.h for more info
 */
void
MIV_I2C_write
(
    miv_i2c_instance_t *this_i2c,
    uint8_t i2c_target_addr,
    const uint8_t *write_buffer,
    uint16_t write_size,
    uint8_t bus_options,
    uint8_t ack_polling_options
)
{
    psr_t processor_state;

    processor_state = HAL_disable_interrupts();

    /* I2C write flow
     *
     * Check I2C status for ongoing transaction
     * Populate the structure with input data
     * Generate start condition
     * Set the write_direction and target address.
     */

    /* Update the transaction only when there is no ongoing I2C transaction */
    if (this_i2c->transaction == MIV_I2C_NO_TRANSACTION)
    {
        this_i2c->transaction = MIV_I2C_MASTER_WRITE_TRANSACTION;
    }

    /* Update the Pending transaction information so that transaction can restarted */
    this_i2c->pending_transaction = MIV_I2C_MASTER_WRITE_TRANSACTION ;

    /* Populate the i2c instance structure */

    /* Set the target addr */
    this_i2c->target_addr = i2c_target_addr;
    this_i2c->dir         = MIV_I2C_WRITE_DIR;

    /* Set up the tx buffer */
    this_i2c->master_tx_buffer = write_buffer;
    this_i2c->master_tx_size   = write_size;
    this_i2c->master_tx_idx    = 0u;

    /* Set the I2C status in progress and setup the options */
    this_i2c->bus_options         = bus_options;
    this_i2c->ack_polling_options = ack_polling_options;
    this_i2c->master_status       = MIV_I2C_IN_PROGRESS;


    /* Generate I2C start condition */
    HAL_set_8bit_reg_field(this_i2c->base_addr, CMD_STA, 0x01u);

    /* write target address and write bit */
    HAL_set_8bit_reg_field(this_i2c->base_addr, TX_DIR, MIV_I2C_WRITE_DIR);
    HAL_set_8bit_reg_field(this_i2c->base_addr, TX_TARGET_ADDR, i2c_target_addr);

    /* Set WR bit to transmit start condition and control byte */
    HAL_set_8bit_reg_field(this_i2c->base_addr, CMD_WR, 0x01);

    /* Set current master hw state -> transmitted start condition and
     * control byte
     */
    this_i2c->master_state = MIV_I2C_TX_STA_CB;

    /*
     * Clear interrupts if required (depends on repeated starts).
     * Since the Bus is on hold, only then prior status needs to
     * be cleared.
     */
    if ( MIV_I2C_HOLD_BUS == this_i2c->bus_status )
    {
        /* Must toggle IACK bit to clear the MIV_I2C IRQ*/
        HAL_set_8bit_reg_field(this_i2c->base_addr, CMD_IACK, 0x01);
        HAL_set_8bit_reg_field(this_i2c->base_addr, CMD_IACK, 0x00);
    }

    MIV_I2C_enable_irq();

    HAL_restore_interrupts(processor_state);

}

/*
 * Please refer to miv_i2c.h for more info
 */
void
MIV_I2C_read
(
    miv_i2c_instance_t *this_i2c,
    uint8_t i2c_target_addr,
    uint8_t *read_buffer,
    uint16_t read_size,
    uint8_t bus_options,
    uint8_t ack_polling_options
)
{
    psr_t processor_state;
    uint8_t status = MIV_I2C_SUCCESS;

    processor_state = HAL_disable_interrupts();

    /* MIV I2C Read operation flow
     *
     * Check for ongoing transaction
     * Populate the i2c instance structure
     * Generate the start condition
     * Set the READ_direction bit and target addr
     */

    uint8_t read_stat = HAL_get_8bit_reg_field(this_i2c->base_addr, STAT_TIP);

    /* Update the transaction only when there is no ongoing I2C transaction */
    if (this_i2c->transaction == MIV_I2C_NO_TRANSACTION)
    {
        this_i2c->transaction = MIV_I2C_MASTER_READ_TRANSACTION;
    }

    this_i2c->pending_transaction = MIV_I2C_MASTER_READ_TRANSACTION;

    /* Populate the MIV I2C instance structure */

    this_i2c->target_addr = i2c_target_addr;
    this_i2c->dir         = MIV_I2C_READ_DIR;

    /* Populate read buffer */
    this_i2c->master_rx_buffer = read_buffer;
    this_i2c->master_rx_size   = read_size;
    this_i2c->master_rx_idx    = 0u;

    /* Set the BUS and ACK polling options */
    this_i2c->bus_options         = bus_options;
    this_i2c->ack_polling_options = ack_polling_options;

    /* Generate the start condition */
    HAL_set_8bit_reg_field(this_i2c->base_addr, CMD_STA, 0x01u);

    /* Set the DIR bit and target addr */
    HAL_set_8bit_reg_field(this_i2c->base_addr, TX_DIR, MIV_I2C_READ_DIR);
    HAL_set_8bit_reg_field(this_i2c->base_addr, TX_TARGET_ADDR, i2c_target_addr);

    /* Set the WR bit to transmit the start condition and command byte */
    HAL_set_8bit_reg_field(this_i2c->base_addr, CMD_WR, 0x01u);

    /* Set the i2c master state and status transmitting STA and Command Byte */
    this_i2c->master_state  = MIV_I2C_TX_STA_CB;
    this_i2c->master_status = MIV_I2C_IN_PROGRESS;

    /* Toggle the IACK bit if required */
    /*
     * Clear interrupts if required (depends on repeated starts).
     * Since the Bus is on hold, only then prior status needs to
     * be cleared.
     */
    if ( MIV_I2C_HOLD_BUS == this_i2c->bus_status )
    {
        /* Must toggle IACK bit to clear the MIV_I2C IRQ*/
        HAL_set_8bit_reg_field(this_i2c->base_addr, CMD_IACK, 0x01);
        HAL_set_8bit_reg_field(this_i2c->base_addr, CMD_IACK, 0x00);
    }
    /* Enable the I2C interrupt */
    MIV_I2C_enable_irq();

    HAL_restore_interrupts(processor_state);
}

/*
 * Please refer to miv_i2c.h for more info
 */
void
MIV_I2C_write_read
(
    miv_i2c_instance_t *this_i2c,
    uint8_t target_addr,
    const uint8_t *write_buffer,
    uint16_t write_size,
    uint8_t *read_buffer,
    uint16_t read_size,
    uint8_t bus_options,
    uint8_t ack_polling_options
)
{
    uint8_t status = MIV_I2C_SUCCESS;
    psr_t processor_state;

    processor_state = HAL_disable_interrupts();

    uint8_t read_stat = HAL_get_8bit_reg_field(this_i2c->base_addr, STAT_TIP);

    /* I2C write read operation flow
     *
     * Used to read the data from set address offset
     *
     * Configure the i2c instance structure
     * generate the start and configure the dir and target addr
     * set wr bit to transmit the start and command byte
     *
     */

    /* Update the transaction only when there is no ongoing I2C transaction */
    if (this_i2c->transaction == MIV_I2C_NO_TRANSACTION)
    {
        this_i2c->transaction = MIV_I2C_MASTER_READ_TRANSACTION;
    }

    this_i2c->pending_transaction = MIV_I2C_MASTER_READ_TRANSACTION;

    /* Populate the I2C instance */

    this_i2c->target_addr = target_addr;

    /* setup the i2c direction */
    this_i2c->dir = MIV_I2C_WRITE_DIR;

    /* set up transmit buffer */
    this_i2c->master_tx_buffer = write_buffer;
    this_i2c->master_tx_size   = write_size;
    this_i2c->master_tx_idx    = 0u;

    /* set up receive buffer */
    this_i2c->master_rx_buffer = read_buffer;
    this_i2c->master_rx_size   = read_size;
    this_i2c->master_rx_idx    = 0u;

    /* Set the bus and ack polling options */
    this_i2c->bus_options         = bus_options;
    this_i2c->ack_polling_options = ack_polling_options;

    /* Generate the start command */
    HAL_set_8bit_reg_field(this_i2c->base_addr, CMD_STA, 0x01u);

    /* Set the DIR and target addr */
    HAL_set_8bit_reg_field(this_i2c->base_addr, TX_TARGET_ADDR, target_addr);
    HAL_set_8bit_reg_field(this_i2c->base_addr, TX_DIR, this_i2c->dir);

    /* Set the WR bit to transmit the start command and command byte */
    HAL_set_8bit_reg_field(this_i2c->base_addr, CMD_WR, 0x01);

    /* Set the i2c master state and status transmitting STA and Command Byte */
    this_i2c->master_state  = MIV_I2C_TX_STA_CB;
    this_i2c->master_status = MIV_I2C_IN_PROGRESS;

    /*
     * Clear interrupt if required
     */
    if ( MIV_I2C_HOLD_BUS == this_i2c->bus_status )
    {
        /* Must toggle IACK bit to clear the MIV_I2C IRQ*/
        HAL_set_8bit_reg_field(this_i2c->base_addr, CMD_IACK, 0x01u);
        HAL_set_8bit_reg_field(this_i2c->base_addr, CMD_IACK, 0x00u);
    }

    /* Enable the I2C interrupt */
    MIV_I2C_enable_irq();

    HAL_restore_interrupts(processor_state);
}

/* MIV_I2C_isr()
 * Please refer to miv_i2c.h for more info
 */
void
MIV_I2C_isr
(
    miv_i2c_instance_t *this_i2c
)
{
    uint8_t i2c_state;
    uint8_t i2c_ack_status;
    uint8_t i2c_al_status;
    uint8_t hold_bus;

    /* Read the I2C master state */
    i2c_state = this_i2c->master_state;

    /* Read the ack and al status */
    i2c_ack_status = HAL_get_8bit_reg_field(this_i2c->base_addr, STAT_RXACK);
    i2c_al_status  = HAL_get_8bit_reg_field(this_i2c->base_addr, STAT_AL);

    switch (i2c_state)
    {
    /* I2C ISR State Machine
     *
     * Cases:
     * - Transmit start condition and control byte
     *      - Received ACK and bus arbitration was not lost (Read or Write)
     *      - Received NACK
     *      - Bus arbitration lost
     *
     * - Transmit data
     *      - Received ACK and bus arbitration was not lost (Read or Write)
     *      - Received NACK
     *      - Bus arbitration lost
     *
     * - Receive data
     *      - Received ACK and bus arbitration was not lost (Read or Write)
     *      - Bus arbitration lost
     */

    case  MIV_I2C_TX_STA_CB:

        /* Received ACK from target and I2C bus arbitration is not lost */
        if (i2c_ack_status == 0u && i2c_al_status == 0u)
        {
            /* If I2C master write operation */
            if (this_i2c->dir == MIV_I2C_WRITE_DIR)
            {
                /* write first byte of data and set the WR bit to transfer the data */
                HAL_set_8bit_reg(this_i2c->base_addr, TRANSMIT,
                            this_i2c->master_tx_buffer[this_i2c->master_tx_idx]);

                HAL_set_8bit_reg_field(this_i2c->base_addr, CMD_WR, 0x01u);

                /* Increment the index */
                this_i2c->master_tx_idx++;

                /* Set the master state to TX data */
                this_i2c->master_state = MIV_I2C_TX_DATA;
            }
            /* Master read operation */
            else
            {
                if (this_i2c->master_rx_size == 1u)
                {
                    /* Send the ACK if the rx size is 1, transmit NACK to slave
                     * after receiving 1 byte to indicate slave to stop sending
                     * the data
                     */
                    HAL_set_8bit_reg_field(this_i2c->base_addr, CMD_ACK, 0x01u);
                }

                /* Send the RD command to slave */
                HAL_set_8bit_reg_field(this_i2c->base_addr, CMD_RD, 0x01u);

                /* Increment the index */
                this_i2c->master_rx_idx++;

                /* Change state to receive data */
                this_i2c->master_state = MIV_I2C_RX_DATA;
            }
        }
        else if (i2c_ack_status == 1u)
        {
            if (this_i2c->ack_polling_options == MIV_I2C_ACK_POLLING_ENABLE)
            {
                /* Target responded with NACK and ACK polling option is enabled
                 *
                 * Re-send the start condition and control byte
                 *
                 * TO-DO: This might become infinite loop check for timeout
                 *        options.
                 */
                HAL_set_8bit_reg_field(this_i2c->base_addr, CMD_STA, 0x01u);
                HAL_set_8bit_reg_field(this_i2c->base_addr, TX_TARGET_ADDR, this_i2c->target_addr);
                HAL_set_8bit_reg_field(this_i2c->base_addr, TX_DIR, this_i2c->dir);

                HAL_set_8bit_reg_field(this_i2c->base_addr, CMD_WR, 0x01u);

                this_i2c->master_tx_idx = 0u;
                this_i2c->master_state = MIV_I2C_TX_STA_CB;
            }

            else
            {
                /* Target responded with NACK and ACK polling is disabled
                 * Abort the transaction and move to IDLE state
                 */
                HAL_set_8bit_reg_field(this_i2c->base_addr, CMD_STO, 0x01u);

                this_i2c->master_status = MIV_I2C_FAILED;
                this_i2c->transaction   = MIV_I2C_NO_TRANSACTION;

                this_i2c->master_state  = MIV_I2C_IDLE;
            }
        }

        else if (i2c_al_status == 1u)
        {
            /* Arbitration was lost on the BUS during the transmission of
             * previous start condition and control byte.
             * Re-send the STA and CB
             */

            HAL_set_8bit_reg_field(this_i2c->base_addr, CMD_STA, 0x01u);
            HAL_set_8bit_reg_field(this_i2c->base_addr, TX_TARGET_ADDR, this_i2c->target_addr);
            HAL_set_8bit_reg_field(this_i2c->base_addr, TX_DIR, this_i2c->dir);

            HAL_set_8bit_reg_field(this_i2c->base_addr, CMD_WR, 0x01u);

            this_i2c->master_state = MIV_I2C_TX_STA_CB;
        }

        break;

        /* Transmit master data */
    case MIV_I2C_TX_DATA:

        /* ACK received and arbitration was not lost */
        if (i2c_ack_status == 0u && i2c_al_status == 0u)
        {
            uint8_t tx_buff[this_i2c->master_tx_size];
            if (this_i2c->master_tx_idx < this_i2c->master_tx_size)
            {
                HAL_set_8bit_reg(this_i2c->base_addr, TRANSMIT,
                        this_i2c->master_tx_buffer[this_i2c->master_tx_idx]);

                tx_buff[this_i2c->master_tx_idx] = this_i2c->master_tx_buffer[this_i2c->master_tx_idx];

                HAL_set_8bit_reg_field(this_i2c->base_addr, CMD_WR, 0x01u);

                /* Increment the index */
                this_i2c->master_tx_idx++;

                /* Set the master state to TX data */
                this_i2c->master_state = MIV_I2C_TX_DATA;
            }

            /* All the bytes are transmitted */
            else if (this_i2c->master_tx_idx == this_i2c->master_tx_size)
            {
                /* If this is a MASTER_READ_TRANSACTION, hold bus and start a 
				   new transfer in read mode now that the read address has been 
				   written to the slave */
                if(this_i2c->transaction == MIV_I2C_MASTER_READ_TRANSACTION)

                {
                    //Switch direction to READ
                    this_i2c->dir         = MIV_I2C_READ_DIR;

                    // Set the STA bit
                    HAL_set_8bit_reg_field(this_i2c->base_addr, CMD_STA, 0x01u);

                    /* Set the DIR bit and target addr */
                    HAL_set_8bit_reg_field(this_i2c->base_addr, TX_DIR, MIV_I2C_READ_DIR);
                    HAL_set_8bit_reg_field(this_i2c->base_addr, TX_TARGET_ADDR, this_i2c->target_addr);

                    /* Set the WR bit to transmit the start condition and command byte */
                    HAL_set_8bit_reg_field(this_i2c->base_addr, CMD_WR, 0x01u);

                    // Reset the buffer index
                    this_i2c->master_tx_idx = 0u;
                    this_i2c->master_rx_idx = 0u;

                    /* Set the master state to RX data */
                    this_i2c->master_state = MIV_I2C_RX_DATA;
                }

                else
                {
                    /* If releasing the bus, transmit the stop condition at the end
                     * of the transfer.
                     */
                    hold_bus = this_i2c->bus_status & MIV_I2C_HOLD_BUS;

                    if (hold_bus == 0)
                    {
                        HAL_set_8bit_reg_field(this_i2c->base_addr, CMD_STO, 0x01u);
                    }
                    else
                    {
                        MIV_I2C_disable_irq();
                    }
                    this_i2c->master_status = MIV_I2C_SUCCESS;
                    this_i2c->transaction   = MIV_I2C_NO_TRANSACTION;
                    this_i2c->master_state  = MIV_I2C_IDLE;
                }
            }
        }

        else if (i2c_ack_status == 1u)
        {
            /* Received NACK from target device
             *
             * Release the bus and end the transfer
             */
            HAL_set_8bit_reg_field(this_i2c->base_addr, CMD_STO, 0x01u);

            this_i2c->master_status = MIV_I2C_FAILED;
            this_i2c->transaction   = MIV_I2C_NO_TRANSACTION;

            this_i2c->master_state  = MIV_I2C_IDLE;
        }

        else if (i2c_al_status == 1u)
        {
            /* Arbitration was lost on the BUS during the transmission of
             * previous start condition and control byte.
             * Re-send the STA and CB
             */
            HAL_set_8bit_reg_field(this_i2c->base_addr, CMD_STA, 0x01u);

            HAL_set_8bit_reg_field(this_i2c->base_addr, TX_TARGET_ADDR,
                                                        this_i2c->target_addr);
            HAL_set_8bit_reg_field(this_i2c->base_addr, TX_DIR, this_i2c->dir);

            HAL_set_8bit_reg_field(this_i2c->base_addr, CMD_WR, 0x01);

            this_i2c->master_state = MIV_I2C_TX_STA_CB;
        }

        break;

        /* Receive target device data */
    case MIV_I2C_RX_DATA:

        if (i2c_al_status == 0u)
        {
            if (this_i2c->master_rx_idx < this_i2c->master_rx_size)
            {
                this_i2c->master_rx_buffer[this_i2c->master_rx_idx - 1u] =
                        HAL_get_8bit_reg(this_i2c->base_addr, RECEIVE);

                /* If next byte is last one
                 * Send NACK to target device to stop sending data
                 */
                if (this_i2c->master_rx_idx == (this_i2c->master_rx_size - 1u))
                {
                    HAL_set_8bit_reg_field(this_i2c->base_addr, CMD_ACK, 0x01u);
                }

                else
                {
                    /* Send ACK to receive next bytes */
                    HAL_set_8bit_reg_field(this_i2c->base_addr, CMD_ACK, 0x00u);
                }

                /* Set RD bit to receive next byte */
                HAL_set_8bit_reg_field(this_i2c->base_addr, CMD_RD, 0x01u);

                this_i2c->master_rx_idx++;
            }

            /* Received all bytes */
            else //if (this_i2c->master_rx_idx == this_i2c->master_rx_size)
            {
                this_i2c->master_rx_buffer[this_i2c->master_rx_idx - 1] =
                        HAL_get_8bit_reg(this_i2c->base_addr, RECEIVE);

                hold_bus = this_i2c->bus_status & MIV_I2C_HOLD_BUS;

                if (hold_bus == 0)
                {
                    HAL_set_8bit_reg_field(this_i2c->base_addr, CMD_STO, 0x01u);
                }
                else
                {
                    MIV_I2C_disable_irq();
                }
                this_i2c->master_status = MIV_I2C_SUCCESS;
                this_i2c->transaction   = MIV_I2C_NO_TRANSACTION;

                this_i2c->master_state  = MIV_I2C_IDLE;
            }
        }

        else if (i2c_al_status == 1u)
        {
            /* Arbitration was lost on the BUS during the transmission of
             * previous start condition and control byte.
             * Re-send the STA and CB
             */
            HAL_set_8bit_reg_field(this_i2c->base_addr, TX_TARGET_ADDR,
                                                        this_i2c->target_addr);
            HAL_set_8bit_reg_field(this_i2c->base_addr, TX_DIR, this_i2c->dir);

            HAL_set_8bit_reg_field(this_i2c->base_addr, CMD_STA, 0x01u);

            HAL_set_8bit_reg_field(this_i2c->base_addr, CMD_WR, 0x01);

            this_i2c->master_state = MIV_I2C_TX_STA_CB;
        }

        break;
    }

    /* Toggle the IACK bit to clear interrupt */
    HAL_set_8bit_reg_field(this_i2c->base_addr, CMD_IACK, 0x01u);
    HAL_set_8bit_reg_field(this_i2c->base_addr, CMD_IACK, 0x00u);
}

/*
 * Please refer to miv_i2c.h for more info
 */
uint8_t
MIV_I2C_get_status
(
    miv_i2c_instance_t *this_i2c
)
{
    uint8_t i2c_status;

    i2c_status = HAL_get_8bit_reg(this_i2c->base_addr, STATUS);

    return i2c_status;
}
