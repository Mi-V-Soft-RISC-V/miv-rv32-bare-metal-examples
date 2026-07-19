/*******************************************************************************
 * (c) Copyright 2007-2023 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * I3C slave sensor register definitions for the FTDI_PROG_ADAPTER_I3C
 * daughter board.
 *
 * Daughter board sensors (active jumper config: J13+J15, J12+J14, J5+J6):
 *   - BMP581         : Pressure Sensor (Bosch)              [J13+J15]
 *   - LPS22DFTR      : Pressure/Temp Sensor (ST)            [J12+J14]
 *   - 24FC1025T-ISN  : 1Mb I2C Serial EEPROM (Microchip)    [J5+J6]
 *
 * NOT connected (jumpers open):
 *   - MMC5633NJL     : 3-axis Magnetic Sensor (MEMSIC)      [J7+J8]
 *   - LSM6DSO32TR    : 6-axis Accel/Gyro (ST)               [J9+J10]
 *   - ICM-42605      : 6-axis Accel/Gyro (TDK/InvenSense)   [J11 area]
 *
 */
#ifndef __I3C_SLAVE_SENSORS_H
#define __I3C_SLAVE_SENSORS_H   1

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================*/
/* DAT-index-based sensor assignment.                                        */
/* The DAT init table in hw_platform.h assigns:                              */
/*   DAT[0] = BMP581,      DAT[1] = LPS22DFTR                                */
/* After ENTDAA, DCT indices are REVERSED vs DAT order (confirmed by         */
/* baseline main.c lines 1014-1107 where BMP581=0x00010008, LPS=0x00000008): */
/*   DCT[0] = LPS22DFTR,   DCT[1] = BMP581                                   */
/*                                                                           */
/* DAT-index assignment is the same method used by the working reference     */
/* firmware.  PID-based matching proved unreliable across power cycles.      */
/*===========================================================================*/
#define BMP581_DCT_INDEX                1u
#define LPS22DFTR_DCT_INDEX             0u

/*===========================================================================*/
/* Static I2C/I3C addresses for SETDASA.                                     */
/* From baseline main.c SETDASA block (lines 260-265, #if 0).                */
/* BMP581:    I2C addr 0x46 (SDO=LOW per datasheet)                          */
/* LPS22DFTR: I2C addr 0x5C (SA0=LOW per datasheet)                          */
/*===========================================================================*/
#define BMP581_STATIC_ADDR              0x46u
#define LPS22DFTR_STATIC_ADDR           0x5Cu

/*===========================================================================*/
/* DAT Word 0 values for SETDASA.                                            */
/* Format (MIPI I3C spec, CoreI3C PG):                                       */
/*   bits [6:0]   = 7-bit static (I2C) address                               */
/*   bits [22:16] = 7-bit dynamic address to assign                          */
/*   bit  [23]    = even parity of bits [22:16]                              */
/*                                                                           */
/* BMP581:    dyn=0x09 parity=1 -> bits[23:16]=0x89, static=0x46             */
/*            DAT_W0 = 0x00890046                                            */
/* LPS22DFTR: dyn=0x0A parity=1 -> bits[23:16]=0x8A, static=0x5C             */
/*            DAT_W0 = 0x008A005C                                            */
/*===========================================================================*/
#define BMP581_SETDASA_DAT_W0           0x00891046u
#define LPS22DFTR_SETDASA_DAT_W0        0x008A105Cu

/*===========================================================================*/
/* SETDASA CMND_FIFO encoding (from baseline main.c line 274).               */
/* CCC 0x87 (SETDASA), CMD_ATTR = Address Assignment (0x2)                   */
/*===========================================================================*/
#define SETDASA_CMND_FIFO_W0            0x0200439Au

/*===========================================================================*/
/* MMC5633NJL - 3-axis Magnetic Sensor                                       */
/*===========================================================================*/

/*
 * Identification registers
 */
#define MMC5633_REG_PRODUCT_ID          0x39u
#define MMC5633_EXPECTED_PRODUCT_ID     0x10u

/*
 * Control registers used for IBI configuration
 */
#define MMC5633_REG_CTRL0               0x1Bu
#define MMC5633_REG_CTRL1               0x1Cu
#define MMC5633_REG_CTRL2               0x1Du

/*
 * Data output registers (X, Y, Z - 18-bit each)
 */
#define MMC5633_REG_XOUT0               0x00u
#define MMC5633_REG_XOUT1               0x01u
#define MMC5633_REG_YOUT0               0x02u
#define MMC5633_REG_YOUT1               0x03u
#define MMC5633_REG_ZOUT0               0x04u
#define MMC5633_REG_ZOUT1               0x05u

/*
 * Status register
 */
#define MMC5633_REG_STATUS              0x18u

/*
 * Interrupt/IBI related
 */
#define MMC5633_REG_INT_CTRL0           0x1Au

/*===========================================================================*/
/* LSM6DSO32TR - 6-axis Accelerometer / Gyroscope                            */
/*===========================================================================*/

/*
 * Identification register
 */
#define LSM6DSO32_REG_WHO_AM_I          0x0Fu
#define LSM6DSO32_EXPECTED_WHO_AM_I     0x6Cu

/*
 * Control registers
 */
#define LSM6DSO32_REG_CTRL1_XL         0x10u   /* Accel ODR and FS */
#define LSM6DSO32_REG_CTRL2_G          0x11u   /* Gyro ODR and FS */
#define LSM6DSO32_REG_CTRL3_C          0x12u
#define LSM6DSO32_REG_CTRL4_C          0x13u
#define LSM6DSO32_REG_CTRL5_C          0x14u
#define LSM6DSO32_REG_CTRL6_C          0x15u

/*
 * IBI / interrupt related
 */
#define LSM6DSO32_REG_INT1_CTRL        0x0Du
#define LSM6DSO32_REG_INT2_CTRL        0x0Eu
#define LSM6DSO32_REG_I3C_BUS_AVB      0x62u

/*
 * Status register
 */
#define LSM6DSO32_REG_STATUS           0x1Eu

/*
 * Data output registers
 */
#define LSM6DSO32_REG_OUTX_L_G        0x22u   /* Gyro X low byte */
#define LSM6DSO32_REG_OUTX_L_A        0x28u   /* Accel X low byte */
#define LSM6DSO32_REG_OUT_TEMP_L       0x20u   /* Temperature low byte */

/*
 * IBI configuration for I3C
 */
#define LSM6DSO32_REG_CTRL9_XL        0x18u
#define LSM6DSO32_REG_MD1_CFG         0x5Eu
#define LSM6DSO32_REG_MD2_CFG         0x5Fu

/*
 * IBI enable patterns from reference firmware
 */
#define LSM6DSO32_REG_DRDY_MASK        0x30u   /* DRDY pulsed mask reg */
#define LSM6DSO32_REG_MASTER_CFG       0x31u
#define LSM6DSO32_REG_FUNC_CFG_ACCESS  0x36u   /* embedded functions access */
#define LSM6DSO32_REG_TAP_CFG0         0x37u

/*===========================================================================*/
/* BMP581 - Pressure Sensor                                                  */
/*===========================================================================*/

/*
 * Identification register
 */
#define BMP581_REG_CHIP_ID              0x01u
#define BMP581_EXPECTED_CHIP_ID         0x50u

/*
 * Revision register
 */
#define BMP581_REG_REV_ID               0x02u

/*
 * Status / control
 */
#define BMP581_REG_INT_STATUS           0x27u
#define BMP581_REG_INT_SOURCE           0x15u
#define BMP581_REG_INT_CONFIG           0x14u
#define BMP581_REG_ODR_CONFIG           0x37u
#define BMP581_REG_OSR_CONFIG           0x36u
#define BMP581_REG_DSP_CONFIG           0x30u
#define BMP581_REG_NVM_ADDR             0x2Bu
#define BMP581_REG_NVM_DATA_LSB         0x2Cu

/*
 * Data output registers (24-bit pressure, 16-bit temperature)
 */
#define BMP581_REG_PRESS_DATA_0         0x20u
#define BMP581_REG_PRESS_DATA_1         0x21u
#define BMP581_REG_PRESS_DATA_2         0x22u
#define BMP581_REG_TEMP_DATA_0          0x1Du
#define BMP581_REG_TEMP_DATA_1          0x1Eu

/*===========================================================================*/
/* LPS22DFTR - MEMS Pressure / Temperature Sensor (STMicroelectronics)       */
/*                                                                           */
/* Init sequence from baseline reference firmware (miv-rv32i-systick-blinky  */
/* main.c, "LPS22DF" section):                                               */
/*   IF_CTRL        (0x0E) = 0x80   (I3C interface enable)                   */
/*   CTRL_REG1      (0x10) = 0x10   (ODR = 10 Hz continuous)                 */
/*   CTRL_REG4      (0x13) = 0x20   (DRDY on INT)                            */
/*   I3C_IF_CTRL    (0x19) = 0x80   (I3C bus available config)               */
/*===========================================================================*/

/*
 * Identification register
 */
#define LPS22DF_REG_WHO_AM_I            0x0Fu
#define LPS22DF_EXPECTED_WHO_AM_I       0xB4u

/*
 * Control / interface registers
 */
#define LPS22DF_REG_IF_CTRL             0x0Eu   /* Interface control    */
#define LPS22DF_REG_CTRL_REG1           0x10u   /* ODR, averaging       */
#define LPS22DF_REG_CTRL_REG2           0x11u   /* Boot, one-shot, etc  */
#define LPS22DF_REG_CTRL_REG3           0x12u   /* INT active-level, OD */
#define LPS22DF_REG_CTRL_REG4           0x13u   /* INT enable (DRDY)    */
#define LPS22DF_REG_I3C_IF_CTRL         0x19u

/*
 * Status / interrupt registers
 */
#define LPS22DF_REG_INT_SOURCE          0x24u
#define LPS22DF_REG_STATUS              0x27u

/*
 * Data output registers (24-bit pressure, 16-bit temperature)
 */
#define LPS22DF_REG_PRESS_OUT_XL        0x28u
#define LPS22DF_REG_PRESS_OUT_L         0x29u
#define LPS22DF_REG_PRESS_OUT_H         0x2Au
#define LPS22DF_REG_TEMP_OUT_L          0x2Bu
#define LPS22DF_REG_TEMP_OUT_H          0x2Cu

/*
 * FIFO registers
 */
#define LPS22DF_REG_FIFO_STATUS1        0x25u
#define LPS22DF_REG_FIFO_STATUS2        0x26u
#define LPS22DF_REG_FIFO_DATA_PRESS_XL  0x78u

/*===========================================================================*/
/* ICM-42605 - 6-axis Accelerometer / Gyroscope  (NOT connected)             */
/*===========================================================================*/

/*
 * Identification register
 */
#define ICM42605_REG_WHO_AM_I           0x75u
#define ICM42605_EXPECTED_WHO_AM_I      0x42u

/*
 * Control registers
 */
#define ICM42605_REG_PWR_MGMT0         0x4Eu
#define ICM42605_REG_GYRO_CONFIG0      0x4Fu
#define ICM42605_REG_ACCEL_CONFIG0     0x50u
#define ICM42605_REG_INT_CONFIG        0x14u
#define ICM42605_REG_INT_SOURCE0       0x65u

/*
 * Data output registers
 */
#define ICM42605_REG_TEMP_DATA1        0x1Du
#define ICM42605_REG_ACCEL_DATA_X1     0x1Fu
#define ICM42605_REG_GYRO_DATA_X1      0x25u

/*===========================================================================*/
/* 24FC1025T - 1Mb I2C Serial EEPROM                                         */
/*===========================================================================*/

/*
 * I2C static address (7-bit). A0=1 on daughter board.
 */
#define EEPROM_I2C_ADDR                 0x51u

/*
 * EEPROM parameters in bytes
 */
#define EEPROM_PAGE_SIZE                128u
#define EEPROM_TOTAL_SIZE               131072u
#define EEPROM_ADDR_BYTES               2u

/*
 * Test data pattern for write/read verification
 */
#define EEPROM_TEST_ADDR_H              0x00u
#define EEPROM_TEST_ADDR_L              0x00u
#define EEPROM_TEST_DATA_0              0x0Du
#define EEPROM_TEST_DATA_1              0x0Bu

#ifdef __cplusplus
}
#endif

#endif /* __I3C_SLAVE_SENSORS_H */
