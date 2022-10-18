/*******************************************************************************
 * (c) Copyright 2008-2021 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 * 
 * @file core_gpio.h
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief CoreGPIO bare metal driver public API.
 *
 */

/*=========================================================================*//**
  @mainpage CoreGPIO Bare Metal Driver.

  @section intro_sec Introduction
  The CoreGPIO hardware IP includes up to 32 general purpose input output GPIOs.
  This driver provides a set of functions for controlling the GPIOs as part of a
  bare metal system where no operating system is available. These drivers
  can be adapted for use as part of an operating system but the implementation
  of the adaptation layer between this driver and the operating system's driver
  model is outside the scope of this driver.
  
  @section driver_configuration Driver Configuration
  The CoreGPIO individual IOs can be configured either in the hardware flow or
  as part of the software application through calls to the GPIO_config() function.
  GPIOs configured as as part of the hardware is fixed and cannot be modified
  using a call to the GPI_config() function.
  
  @section theory_op Theory of Operation
  The CoreGPIO driver uses the Actel Hardware Abstraction Layer (HAL) to access
  hardware registers. You must ensure that the Actel HAL is included as part of
  your software project. The Actel HAL is available through the Actel Firmware
  Catalog.
  
  The CoreGPIO driver functions are logically grouped into the following groups:
    - Initialization
    - Configuration
    - Reading and writing GPIO state
    - Interrupt control
  
  The CoreGPIO driver is initialized through a call to the GPIO_init() function.
  The GPIO_init() function must be called before any other GPIO driver functions
  can be called.
  
  Each GPIO port is individually configured through a call to the
  GPIO_config() function. Configuration includes deciding if a GPIO port
  will be used as input, output or both. GPIO ports configured as inputs can be
  further configured to generate interrupts based on the input's state.
  Interrupts can be level or edge sensitive.
  Please note that a CoreGPIO hardware instance can be generated, as part of the
  hardware flow, with a fixed configuration for some or all of its IOs. Attempting
  to modify the configuration of such a hardware configured IO using the
  GPIO_config() function has no effect.
  
  The state of the GPIO ports can be read and written using the following
  functions:
    - GPIO_get_inputs()
    - GPIO_get_outputs()
    - GPIO_set_outputs()
    - GPIO_drive_inout()
    
  Interrupts generated by GPIO ports configured as inputs are controlled using
  the following functions:
    - GPIO_enable_irq()
    - GPIO_disable_irq()
    - GPIO_clear_irq()
    - GPIO_get_irq_sources()
    - GPIO_clear_all_irq_sources()
  
 *//*=========================================================================*/
#ifndef CORE_GPIO_H_
#define CORE_GPIO_H_

#ifndef LEGACY_DIR_STRUCTURE
#include "hal/hal.h"

#else
#include "hal.h"
#endif

/*-------------------------------------------------------------------------*//**
  The gpio_id_t enumeration is used to identify GPIOs as part of the
  parameter to functions:
    - GPIO_config(),
    - GPIO_drive_inout(),
    - GPIO_enable_int(),
    - GPIO_disable_int(),
    - GPIO_clear_int()
 */
typedef enum __gpio_id_t
{
    GPIO_0 = 0,
    GPIO_1 = 1,
    GPIO_2 = 2,
    GPIO_3 = 3,
    GPIO_4 = 4,
    GPIO_5 = 5,
    GPIO_6 = 6,
    GPIO_7 = 7,
    GPIO_8 = 8,
    GPIO_9 = 9,
    GPIO_10 = 10,
    GPIO_11 = 11,
    GPIO_12 = 12,
    GPIO_13 = 13,
    GPIO_14 = 14,
    GPIO_15 = 15,
    GPIO_16 = 16,
    GPIO_17 = 17,
    GPIO_18 = 18,
    GPIO_19 = 19,
    GPIO_20 = 20,
    GPIO_21 = 21,
    GPIO_22 = 22,
    GPIO_23 = 23,
    GPIO_24 = 24,
    GPIO_25 = 25,
    GPIO_26 = 26,
    GPIO_27 = 27,
    GPIO_28 = 28,
    GPIO_29 = 29,
    GPIO_30 = 30,
    GPIO_31 = 31
} gpio_id_t;

typedef enum __gpio_apb_width_t
{
    GPIO_APB_8_BITS_BUS = 0,
    GPIO_APB_16_BITS_BUS = 1,
    GPIO_APB_32_BITS_BUS = 2,
    GPIO_APB_UNKNOWN_BUS_WIDTH = 3
} gpio_apb_width_t;

/*-------------------------------------------------------------------------*//**
 */
typedef struct __gpio_instance_t
{
    addr_t              base_addr;
    gpio_apb_width_t    apb_bus_width;
} gpio_instance_t;

/*-------------------------------------------------------------------------*//**
  GPIO ports definitions used to identify GPIOs as part of the parameter to
  function GPIO_set_outputs().
  These definitions can also be used to identity GPIO through logical
  operations on the return value of function GPIO_get_inputs().
 */
#define GPIO_0_MASK         0x00000001UL
#define GPIO_1_MASK         0x00000002UL
#define GPIO_2_MASK         0x00000004UL
#define GPIO_3_MASK         0x00000008UL
#define GPIO_4_MASK         0x00000010UL
#define GPIO_5_MASK         0x00000020UL
#define GPIO_6_MASK         0x00000040UL
#define GPIO_7_MASK         0x00000080UL
#define GPIO_8_MASK         0x00000100UL
#define GPIO_9_MASK         0x00000200UL
#define GPIO_10_MASK        0x00000400UL
#define GPIO_11_MASK        0x00000800UL
#define GPIO_12_MASK        0x00001000UL
#define GPIO_13_MASK        0x00002000UL
#define GPIO_14_MASK        0x00004000UL
#define GPIO_15_MASK        0x00008000UL
#define GPIO_16_MASK        0x00010000UL
#define GPIO_17_MASK        0x00020000UL
#define GPIO_18_MASK        0x00040000UL
#define GPIO_19_MASK        0x00080000UL
#define GPIO_20_MASK        0x00100000UL
#define GPIO_21_MASK        0x00200000UL
#define GPIO_22_MASK        0x00400000UL
#define GPIO_23_MASK        0x00800000UL
#define GPIO_24_MASK        0x01000000UL
#define GPIO_25_MASK        0x02000000UL
#define GPIO_26_MASK        0x04000000UL
#define GPIO_27_MASK        0x08000000UL
#define GPIO_28_MASK        0x10000000UL
#define GPIO_29_MASK        0x20000000UL
#define GPIO_30_MASK        0x40000000UL
#define GPIO_31_MASK        0x80000000UL

/*-------------------------------------------------------------------------*//**
 * GPIO modes
 */
#define GPIO_INPUT_MODE              0x0000000002UL
#define GPIO_OUTPUT_MODE             0x0000000005UL
#define GPIO_INOUT_MODE              0x0000000003UL

/*-------------------------------------------------------------------------*//**
 * Possible GPIO inputs interrupt configurations.
 */
#define GPIO_IRQ_LEVEL_HIGH           0x0000000000UL
#define GPIO_IRQ_LEVEL_LOW            0x0000000020UL
#define GPIO_IRQ_EDGE_POSITIVE        0x0000000040UL
#define GPIO_IRQ_EDGE_NEGATIVE        0x0000000060UL
#define GPIO_IRQ_EDGE_BOTH            0x0000000080UL

/*-------------------------------------------------------------------------*//**
 * Possible states for GPIO configured as INOUT.
 */
typedef enum gpio_inout_state
{
    GPIO_DRIVE_LOW = 0,
    GPIO_DRIVE_HIGH,
    GPIO_HIGH_Z
} gpio_inout_state_t;

/*-------------------------------------------------------------------------*//**
  The GPIO_init() function initialises a CoreGPIO hardware instance and the data
  structure associated with the CoreGPIO hardware instance.
  Please note that a CoreGPIO hardware instance can be generated with a fixed
  configuration for some or all of its IOs as part of the hardware flow. Attempting
  to modify the configuration of such a hardware configured IO using the
  GPIO_config() function has no effect.

  @param this_gpio
    Pointer to the gpio_instance_t data structure instance holding all data
    regarding the CoreGPIO hardware instance being initialized. A pointer to the
    same data structure will be used in subsequent calls to the CoreGPIO driver
    functions in order to identify the CoreGPIO instance that should perform the
    operation implemented by the called driver function.

  @param base_addr
    The base_addr parameter is the base address in the processor's memory map for
    the registers of the GPIO instance being initialized.
    
  @param bus_width
    The bus_width parameter informs the driver of the APB bus width selected during
    the hardware flow configuration of the CoreGPIO hardware instance. It indicates
    to the driver whether the CoreGPIO hardware registers will be visible as 8, 16
    or 32 bits registers. Allowed value are:
        - GPIO_APB_8_BITS_BUS
        - GPIO_APB_16_BITS_BUS
        - GPIO_APB_32_BITS_BUS
  
  @return
    none.
    
  Example:
  @code
    #define COREGPIO_BASE_ADDR  0xC2000000
    
    gpio_instance_t g_gpio;
    
    void system_init( void )
    {
        GPIO_init( &g_gpio,    COREGPIO_BASE_ADDR, GPIO_APB_32_BITS_BUS );
    }
  @endcode
  */
void GPIO_init
(
    gpio_instance_t *   this_gpio,
    addr_t              base_addr,
    gpio_apb_width_t    bus_width
);

/*-------------------------------------------------------------------------*//**
  The GPIO_config() function is used to configure an individual GPIO port.
 
  @param this_gpio
    The this_gpio parameter is a pointer to the gpio_instance_t structure holding
    all data regarding the CoreGPIO instance controlled through this function call.

  @param port_id
    The port_id parameter identifies the GPIO port to be configured.
    An enumeration item of the form GPIO_n where n is the number of the GPIO
    port is used to identify the GPIO port. For example GPIO_0 identifies the
    first GPIO port and GPIO_31 the last one.
    
  @param config
    The config parameter specifies the configuration to be applied to the GPIO
    port identified by the first parameter. It is a logical OR of GPIO mode and
    the interrupt mode. The interrupt mode is only relevant if the GPIO is
    configured as input.
       Possible modes are:
           - GPIO_INPUT_MODE,
           - GPIO_OUTPUT_MODE,
           - GPIO_INOUT_MODE.
       Possible interrupt modes are:
           - GPIO_IRQ_LEVEL_HIGH,
           - GPIO_IRQ_LEVEL_LOW,
           - GPIO_IRQ_EDGE_POSITIVE,
           - GPIO_IRQ_EDGE_NEGATIVE,
           - GPIO_IRQ_EDGE_BOTH
 
  @return
    none.
    
  For example the following call will configure GPIO 4 as an input generating
  interrupts on a low to high transition of the input:
  @code
  GPIO_config( &g_gpio, GPIO_4, GPIO_INPUT_MODE | GPIO_IRQ_EDGE_POSITIVE );
  @endcode
 */
void GPIO_config
(
    gpio_instance_t *   this_gpio,
    gpio_id_t           port_id,
    uint32_t            config
);

/*-------------------------------------------------------------------------*//**
  The GPIO_set_outputs() function is used to set the state of the GPIO ports
  configured as outputs.
 
  @param this_gpio
    The this_gpio parameter is a pointer to the gpio_instance_t structure holding
    all data regarding the CoreGPIO instance controlled through this function call.

  @param value
    The value parameter specifies the state of the GPIO ports configured as
    outputs. It is a bit mask of the form (GPIO_n_MASK | GPIO_m_MASK) where n
    and m are numbers identifying GPIOs.
    For example (GPIO_0_MASK | GPIO_1_MASK | GPIO_2_MASK ) specifies that the
    first, second and third GPIOs' must be set high and all other outputs set
    low.

  @return
    none.
    
  Example 1:
    Set GPIOs outputs 0 and 8 high and all other GPIO outputs low.
    @code
        GPIO_set_outputs( &g_gpio, GPIO_0_MASK | GPIO_8_MASK );
    @endcode

  Example 2:
    Set GPIOs outputs 2 and 4 low without affecting other GPIO outputs.
    @code
        uint32_t gpio_outputs;
        gpio_outputs = GPIO_get_outputs( &g_gpio );
        gpio_outputs &= ~( GPIO_2_MASK | GPIO_4_MASK );
        GPIO_set_outputs( &g_gpio, gpio_outputs );
    @endcode

  @see GPIO_get_outputs()
 */
void GPIO_set_outputs
(
    gpio_instance_t *   this_gpio,
    uint32_t            value
);

/*-------------------------------------------------------------------------*//**
  The GPIO_set_output() function is used to set the state of a single GPIO
  port configured as output.
 
  @param this_gpio
    The this_gpio parameter is a pointer to the gpio_instance_t structure holding
    all data regarding the CoreGPIO instance controlled through this function call.

  @param port_id
    The port_id parameter specifies the GPIO port that will have its output set
    by a call to this function.
  
  @param value
    The value parameter specifies the desired state for the GPIO output. A value
    of 0 will set the output low and a value of 1 will set the port high.
  
  @return
    none.
 */
void GPIO_set_output
(
    gpio_instance_t *   this_gpio,
    gpio_id_t           port_id,
    uint8_t             value
);

/*-------------------------------------------------------------------------*//**
  The GPIO_get_inputs() function is used to read the state of all GPIOs
  configured as inputs.
 
  @param this_gpio
    The this_gpio parameter is a pointer to the gpio_instance_t structure holding
    all data regarding the CoreGPIO instance controlled through this function call.

  @return
    This function returns a 32 bit unsigned integer where each bit represents
    the state of an input. The least significant bit representing the state of
    GPIO 0 and the most significant bit the state of GPIO 31.
 */
uint32_t GPIO_get_inputs
(
    gpio_instance_t *   this_gpio
);

/*-------------------------------------------------------------------------*//**
  The GPIO_get_outputs() function is used to read the current state of all
  GPIO outputs.
 
  @param this_gpio
    The this_gpio parameter is a pointer to the gpio_instance_t structure holding
    all data regarding the CoreGPIO instance controlled through this function call.

  @return
     This function returns a 32 bit unsigned integer where each bit represents
     the state of an output. The least significant bit representing the state
     of GPIO 0 and the most significant bit the state of GPIO 31.
 */
uint32_t GPIO_get_outputs
(
    gpio_instance_t *   this_gpio
);

/*-------------------------------------------------------------------------*//**
  The GPIO_drive_inout() function is used to set the output state of a
  GPIO configured as INOUT. An INOUT GPIO can be in one of three states:
    - high
    - low
    - high impedance
  An INOUT output would typically be used where several devices can drive the
  state of a signal. The high and low states are equivalent to the high and low
  states of a GPIO configured as output. The high impedance state is used to
  prevent the GPIO from driving the state of the output and therefore allow
  reading the state of the GPIO as an input.
  Please note that the GPIO port you wish to use as INOUT through this function
  must be configurable through software. Therefore the GPIO ports used as INOUT
  must not have a fixed configuration selected as part of the hardware flow.
 
  @param this_gpio
    The this_gpio parameter is a pointer to the gpio_instance_t structure holding
    all data regarding the CoreGPIO instance controlled through this function call.

  @param port_id
    The port_id parameter identifies the GPIO for which this function will
    change the output state.
    An enumeration item of the form GPIO_n where n is the number of the GPIO
    port is used to identify the GPIO port. For example GPIO_0 identifies the
    first GPIO port and GPIO_31 the last one.
    
  @param inout_state
    The inout_state parameter specifies the state of the I/O identified by the
    first parameter. Possible states are:
                           - GPIO_DRIVE_HIGH,
                           - GPIO_DRIVE_LOW,
                           - GPIO_HIGH_Z (high impedance)

  @return
    none.
    
  Example:
    The call to GPIO_drive_inout() below will set the GPIO 7 output to
    high impedance state.
    @code
    GPIO_drive_inout( &g_gpio, GPIO_7, GPIO_HIGH_Z );
    @endcode
 */
void GPIO_drive_inout
(
    gpio_instance_t *   this_gpio,
    gpio_id_t           port_id,
    gpio_inout_state_t  inout_state
);

/*-------------------------------------------------------------------------*//**
  The GPIO_enable_irq() function is used to enable an interrupt to be
  generated based on the state of the input identified as parameter.
 
  @param this_gpio
    The this_gpio parameter is a pointer to the gpio_instance_t structure holding
    all data regarding the CoreGPIO instance controlled through this function call.

  @param port_id
    The port_id parameter identifies the GPIO input the call to
    GPIO_enable_irq() will enable to generate interrupts.
    An enumeration item of the form GPIO_n where n is the number of the GPIO
    port is used to identify the GPIO port. For example GPIO_0 identifies the
    first GPIO port and GPIO_31 the last one.
    
  @return
    none.
    
  Example:
    The call to GPIO_enable_irq() below will allow GPIO 8 to generate
    interrupts.
    @code
    GPIO_enable_irq( &g_gpio, GPIO_8 );
    @endcode
 */
void GPIO_enable_irq
(
    gpio_instance_t *   this_gpio,
    gpio_id_t           port_id
);

/*-------------------------------------------------------------------------*//**
  The GPIO_disable_irq() function is used to disable interrupt from being
  generated based on the state of the input specified as parameter.
 
  @param this_gpio
    The this_gpio parameter is a pointer to the gpio_instance_t structure holding
    all data regarding the CoreGPIO instance controlled through this function call.

  @param port_id
    The port_id parameter identifies the GPIO input the call to
    GPIO_disable_irq() will disable from generating interrupts.
    An enumeration item of the form GPIO_n where n is the number of the GPIO
    port is used to identify the GPIO port. For example GPIO_0 identifies the
    first GPIO port and GPIO_31 the last one.
 
  @return
    none.
    
  Example:
    The call to GPIO_disable_irq() below will prevent GPIO 8 from generating
    interrupts.
    @code
    GPIO_disable_irq( &g_gpio, GPIO_8 );
    @endcode
 */
void GPIO_disable_irq
(
    gpio_instance_t *   this_gpio,
    gpio_id_t           port_id
);

/*-------------------------------------------------------------------------*//**
  The GPIO_clear_irq() function is used to clear the interrupt generated by
  the GPIO specified as parameter. The GPIO_clear_irq() function  must be
  called as part of a GPIO interrupt service routine (ISR) in order to prevent
  the same interrupt event re-triggering a call to the GPIO ISR.
  Please note that interrupts may also need to be cleared in the processor's
  interrupt controller.
 
  @param this_gpio
    The this_gpio parameter is a pointer to the gpio_instance_t structure holding
    all data regarding the CoreGPIO instance controlled through this function call.

  @param port_id
    The port_id parameter identifies the GPIO input for which to clear the
    interrupt.
    An enumeration item of the form GPIO_n where n is the number of the GPIO
    port is used to identify the GPIO port. For example GPIO_0 identifies the
    first GPIO port and GPIO_31 the last one.
    
  @return
    none.
    
  Example:
    The example below demonstrates the use of the GPIO_clear_irq() function as
    part of the GPIO 9 interrupt service routine.  
    @code
    void GPIO9_IRQHandler( void )
    {
        do_interrupt_processing();
        
        GPIO_clear_irq( &g_gpio, GPIO_9 );
        
        NVIC_ClearPendingIRQ( GPIO9_IRQn );
    }
    @endcode
 */
void GPIO_clear_irq
(
    gpio_instance_t *   this_gpio,
    gpio_id_t           port_id
);

/*-------------------------------------------------------------------------*//**
  The GPIO_get_irq_sources() function is used to identify the source of
  interrupt. i.e. the GPIO input line whose state change triggered the interrupt.
  The GPIO_get_irq_sources() function  must be called as part of a GPIO
  interrupt service routine (ISR) in order to determine the interrupt source.

  @param this_gpio
    The this_gpio parameter is a pointer to the gpio_instance_t structure holding
    all data regarding the CoreGPIO instance controlled through this function call.

  @return
     This function returns a 32 bit unsigned integer where each bit represents
     the pin number of GPIO.

  Example:
    The example below demonstrates the use of the GPIO_get_irq_sources() function
    as part of the GPIO 9 interrupt service routine.
    @code
    void GPIO9_IRQHandler( void )
    {
        do_interrupt_processing();

        GPIO_clear_all_irq_sources(g_p_mygpio, GPIO_get_irq_sources(g_p_mygpio));

        NVIC_ClearPendingIRQ( GPIO9_IRQn );
    }
    @endcode
 */
uint32_t GPIO_get_irq_sources
(
    gpio_instance_t *   this_gpio
);

/*-------------------------------------------------------------------------*//**
  The GPIO_clear_all_irq_sources() function is used to clear the all the active
  interrupt generated by the GPIO specified as parameter. The
  GPIO_clear_all_irq_sources() function  must be called as part of a GPIO interrupt
  service routine (ISR) in order to prevent the same interrupt event
  re-triggering a call to the GPIO ISR.
  Please note that interrupts may also need to be cleared in the processor's
  interrupt controller.

  @param this_gpio
    The this_gpio parameter is a pointer to the gpio_instance_t structure holding
    all data regarding the CoreGPIO instance controlled through this function call.

  @param bitmask
     This bitmask parameter is a 32 bit unsigned integer where each bit represents
     the GPIO pin used to clears the interrupt bit register of the corresponding
     GPIO bit. The least significant bit representing the status of GPIO 0 and
     the most significant bit the status of GPIO 31.

  @return
    none.

  Example:
    The example below demonstrates the use of the GPIO_clear_all_irq_sources() function as
    part of the GPIO 9 interrupt service routine.
    @code
    void GPIO9_IRQHandler( void )
    {
        do_interrupt_processing();

        do_interrupt_processing();

        GPIO_clear_all_irq_sources(g_p_mygpio, GPIO_get_irq_sources(g_p_mygpio));

        NVIC_ClearPendingIRQ( GPIO9_IRQn );
    }
    @endcode
 */
void GPIO_clear_all_irq_sources
(
    gpio_instance_t *   this_gpio,
    uint32_t            bitmask
);
#endif /* CORE_GPIO_H_ */
