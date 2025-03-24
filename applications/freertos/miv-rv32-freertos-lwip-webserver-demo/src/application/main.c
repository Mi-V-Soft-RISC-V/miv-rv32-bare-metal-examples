
/*******************************************************************************
 * Copyright 2025 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file main.c
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief This project implements a example webserver application using the LwIP
 * TCP/IP stack and FreeRTOS operating system for the PolarFire Evaluation Kit
 * built with Microchip's SoftConsole IDE.
 *
 * Please refer to the file README.md for further details about this example.
 *
 */

/*-------------------------------------------------------------------------*/

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

/* Microchip includes. */
#include "drivers/fpga_ip/CoreGPIO/core_gpio.h"
#include "drivers/fpga_ip/CoreTSE/core_tse.h"
#include "drivers/fpga_ip/CoreTSE/coretse_regs.h"
#include "drivers/fpga_ip/CoreUARTapb/core_uart_apb.h"

#include "fpga_design_config/fpga_design_config.h"
#include "hal/hal.h"
#include "miv_rv32_hal/miv_rv32_hal.h"

/* lwIP includes. */
#include "lwip/dhcp.h"
#include "lwip/ip4_addr.h"
#include "lwip/tcpip.h"

const char *g_hello_msg = "\r\n/* Mi-V RV32 FreeRTOS LWIP Web Server Demo */\r\n";
extern void configure_zl30364(void);
UART_instance_t g_uart;

/* Priorities used by the various different tasks. */
#define HTTPD_TASK_PRIORITY         1
#define LINK_STATUS_TASK_PRIORITY   1

/* Priorities for the demo application tasks. */
#define mainCHECK_TASK_PRIORITY     (configMAX_PRIORITIES - 1)
#define mainCREATOR_TASK_PRIORITY   (tskIDLE_PRIORITY + 3UL)

/* Web server task stack size. */
#define HTTPD_STACK_SIZE            2048
#define LINK_STATUS_TASK_STACK_SIZE 1024

void http_server_netconn_thread(void *arg);
void prvLinkRXTask(void *pvParameters);

void prvLinkStatusTask(void *pvParameters);
/*
 * Ethernet interface configuration function.
 */
static void prvEthernetConfigureInterface(void *param);
/*
 * FreeRTOS hook for when malloc fails, enable in FreeRTOSConfig.
 */
void vApplicationMallocFailedHook(void);

/*
 * FreeRTOS hook for when freertos is idling, enable in FreeRTOSConfig.
 */
void vApplicationIdleHook(void);

/*
 * FreeRTOS hook for when a stackoverflow occurs, enable in FreeRTOSConfig.
 */
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName);

/* The queue used by PTPd task to trnsmit status information to webserver task.
 */
xQueueHandle xPTPdOutQueue = NULL;

/* lwIP MAC configuration. */
static struct netif s_EMAC_if;

/*-----------------------------------------------------------*/

void
get_mac_address(uint8_t *mac_addr)
{
    uint32_t inc;

    for (inc = 0; inc < 6; ++inc)
    {
        mac_addr[inc] = s_EMAC_if.hwaddr[inc];
    }
}

uint32_t
get_ip_address(void)
{
    return (uint32_t)(s_EMAC_if.ip_addr.addr);
}
extern QueueHandle_t xRXQueue;

int
main(void)
{
    UART_init(&g_uart, COREUARTAPB0_BASE_ADDR, BAUD_VALUE_115200, (DATA_8_BITS | NO_PARITY));
    UART_polled_tx_string(&g_uart, (const uint8_t *)g_hello_msg);
    configure_zl30364();
    /*
     * Create the queue used by the RX interrupt and prvLinkRXTask task to
     * synchronize frame reception
     */
    xRXQueue = xQueueCreate(10, sizeof(uint32_t));

    if (xRXQueue == NULL)
    {
        /* Queue was not created and must not be used. */
        UART_polled_tx_string(&g_uart,
                              (const uint8_t *)"QueueCreate function failed with error.\n\r");
    }

    /* Create the web server task. */
    tcpip_init(prvEthernetConfigureInterface, NULL);

    xTaskCreate(prvLinkRXTask, "LinkRX", 1024, NULL, uartPRIMARY_PRIORITY, NULL);

    xTaskCreate(http_server_netconn_thread,
                (signed char *)"http_server",
                HTTPD_STACK_SIZE,
                NULL,
                HTTPD_TASK_PRIORITY,
                NULL);

    /* Create the task the Ethernet link status. */
    xTaskCreate(prvLinkStatusTask,
                (signed char *)"EthLinkStatus",
                LINK_STATUS_TASK_STACK_SIZE,
                NULL,
                LINK_STATUS_TASK_PRIORITY,
                NULL);

    HAL_enable_interrupts();

    /* Start the kernel.  From here on, only tasks and interrupts will run. */
    vTaskStartScheduler();

    /*
     * If all is well, the scheduler will now be running, and the following line
     * will never be reached.  If the following line does execute, then there
     * was insufficient FreeRTOS heap memory available for the idle and/or timer
     * tasks to be created.  See the memory management section on the FreeRTOS
     * web site for more details.
     */
    /* Exit FreeRTOS */
    return 0;
}

/*-----------------------------------------------------------*/
void
vApplicationMallocFailedHook(void)
{
    /* vApplicationMallocFailedHook() will only be called if
    configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
    function that will get called if a call to pvPortMalloc() fails.
    pvPortMalloc() is called internally by the kernel whenever a task, queue,
    timer or semaphore is created.  It is also called by various parts of the
    demo application.  If heap_1.c or heap_2.c are used, then the size of the
    heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
    FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
    to query the size of free heap space that remains (although it does not
    provide information on how the remaining heap might be fragmented). */
    taskDISABLE_INTERRUPTS();
    __asm volatile("ebreak");
    for (;;)
        ;
}
/*-----------------------------------------------------------*/

void
vApplicationIdleHook(void)
{
    /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
    to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
    task.  It is essential that code added to this hook function never attempts
    to block in any way (for example, call xQueueReceive() with a block time
    specified, or call vTaskDelay()).  If the application makes use of the
    vTaskDelete() API function (as this demo application does) then it is also
    important that vApplicationIdleHook() is permitted to return to its calling
    function, because it is the responsibility of the idle task to clean up
    memory allocated by the kernel to any task that has since been deleted. */
}
/*-----------------------------------------------------------*/

void
vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
    (void)pcTaskName;
    (void)pxTask;

    /* Run time stack overflow checking is performed if
    configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
    function is called if a stack overflow is detected. */
    taskDISABLE_INTERRUPTS();
    __asm volatile("ebreak");
    for (;;)
        ;
}
/*-----------------------------------------------------------*/

void
vApplicationTickHook(void)
{
    /* The tests in the full demo expect some interaction with interrupts. */
#if (mainCREATE_SIMPLE_BLINKY_DEMO_ONLY != 1)
    {
        // vFullDemoTickHook();
    }
#endif
}

/*-----------------------------------------------------------*/

static void
prvEthernetConfigureInterface(void *param)
{
    struct ip4_addr xIpAddr, xNetMast, xGateway;

    extern err_t ethernetif_init(struct netif * netif);

    /* Parameters are not used - suppress compiler error. */
    (void)param;

    /* Create and configure the EMAC interface. */
#ifdef NET_USE_DHCP
    IP4_ADDR(&xIpAddr, 0, 0, 0, 0);
    IP4_ADDR(&xGateway, 192, 168, 1, 254);
#else
    IP4_ADDR(&xIpAddr, 169, 254, 1, 23);
    IP4_ADDR(&xGateway, 169, 254, 1, 23);
#endif

    IP4_ADDR(&xNetMast, 255, 255, 255, 0);

    netif_add(&s_EMAC_if, &xIpAddr, &xNetMast, &xGateway, NULL, ethernetif_init, tcpip_input);

    /* bring it up */

#ifdef NET_USE_DHCP
    dhcp_start(&s_EMAC_if);
#else
    netif_set_up(&s_EMAC_if);
#endif

    /* make it the default interface */
    netif_set_default(&s_EMAC_if);
}

uint8_t buf[200] = {0};
uint32_t ip_addr;
uint16_t address[4];

void
prvLinkStatusTask(void *pvParameters)
{
    static uint8_t acquired = 0;
    uint32_t temp = 0;

    UART_polled_tx_string(&g_uart, (const uint8_t *)"Acquiring IP address");

    for (;;)
    {
        volatile uint8_t linkup;

        /* Run through loop every 1200 milliseconds. */
        vTaskDelay(1200 / portTICK_RATE_MS);

        if (0 == acquired)
        {
            ip_addr = get_ip_address();
            if (0 != ip_addr)
            {
                acquired = 1;
            }
            UART_polled_tx_string(&g_uart, (const uint8_t *)".");
        }

        if (acquired == 1)
        {
            address[0] = ((uint16_t)((ip_addr >> 24u) & 0x000000FFu));
            address[1] = ((int)((ip_addr >> 16u) & 0x000000FFu));
            address[2] = ((int)((ip_addr >> 8u) & 0x000000FFu));
            address[3] = ((int)((ip_addr)&0x000000FFu));

            sprintf(buf, "\n\r %d.%d.%d.%d\n\r", address[3], address[2], address[1], address[0]);
            UART_polled_tx_string(&g_uart, (const uint8_t *)buf);

            sprintf(buf,
                    "\n\r Tx Byte Count:%d Rx Byte Count:%d Rx Frame Drop:%d\n\r",
                    *(unsigned int *)(TSE_BASEADDR + TBYT_REG_OFFSET),
                    *(unsigned int *)(TSE_BASEADDR + RBYT_REG_OFFSET),
                    *(unsigned int *)(TSE_BASEADDR + RDRP_REG_OFFSET));
            UART_polled_tx_string(&g_uart, (const uint8_t *)buf);

            vTaskDelay(2000 / portTICK_RATE_MS);
        }
    }
    vTaskDelete(NULL);
}
