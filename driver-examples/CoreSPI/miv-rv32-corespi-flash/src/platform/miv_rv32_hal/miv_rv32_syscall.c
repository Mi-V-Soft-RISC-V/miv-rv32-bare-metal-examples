/*******************************************************************************
 * Copyright 2019 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file miv_rv32_syscall.c
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief Stubs for system calls.
 *
 */
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/times.h>
#include "miv_rv32_hal.h"

#ifdef MSCC_STDIO_THRU_CORE_UART_APB
#include <string.h>

#ifndef LEGACY_DIR_STRUCTURE
#include "drivers/fpga_ip/CoreUARTapb/core_uart_apb.h"
#else
#include "core_uart_apb.h"
#endif

#endif  /*MSCC_STDIO_THRU_CORE_UART_APB*/

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MSCC_STDIO_THRU_CORE_UART_APB

/*------------------------------------------------------------------------------
 * CoreUARTapb instance data for the CoreUARTapb instance used for standard
 * output.
 */
static UART_instance_t g_stdio_uart;

/*==============================================================================
 * Flag used to indicate if the UART driver needs to be initialized.
 */
static int g_stdio_uart_init_done = 0;

/*
 * Disable semihosting apis
 */
#pragma import(__use_no_semihosting_swi)

/*==============================================================================
 * sendchar()
 */
int sendchar(int ch)
{
    /*--------------------------------------------------------------------------
    * Initialize the UART driver if it is the first time this function is
    * called.
    */
    if ( !g_stdio_uart_init_done )
    {
        /******************************************************************************
         * Baud value:
         * This value is calculated using the following equation:
         *      BAUD_VALUE = (CLOCK / (16 * BAUD_RATE)) - 1
         *****************************************************************************/
        UART_init( &g_stdio_uart,
                   MSCC_STDIO_UART_BASE_ADDR,
                   ((SYS_CLK_FREQ/(16 * MSCC_STDIO_BAUD_VALUE))-1),
                   (DATA_8_BITS | NO_PARITY));

        g_stdio_uart_init_done = 1;
    }

    /*--------------------------------------------------------------------------
    * Output text to the UART.
    */
    UART_send( &g_stdio_uart, (uint8_t *)&ch, 1 );

    return (ch);
}

/*==============================================================================
 * getachar()
 */
int getachar(void)
{
    uint8_t rx_size;
    uint8_t rx_byte;

    if ( !g_stdio_uart_init_done )
    {
        /******************************************************************************
         * Baud value:
         * This value is calculated using the following equation:
         *      BAUD_VALUE = (CLOCK / (16 * BAUD_RATE)) - 1
         *****************************************************************************/
        UART_init( &g_stdio_uart,
                   MSCC_STDIO_UART_BASE_ADDR,
                   ((SYS_CLK_FREQ/(16 * MSCC_STDIO_BAUD_VALUE))-1),
                   (DATA_8_BITS | NO_PARITY));

        g_stdio_uart_init_done = 1;
    }

    do
    {
        rx_size = UART_get_rx(&g_stdio_uart, &rx_byte, 1);
    } while(0u == rx_size);

    return rx_byte;
}

#endif /*MSCC_STDIO_THRU_CORE_UART_APB*/

#undef errno
static int errno;

static char *__env[1] = { 0 };
char **environ = __env;

void write_hex(int fd, uint32_t hex)
{
    char towrite;

    write( fd , "0x", 2U );

    for (uint32_t ii = 8U ; ii > 0U; ii--)
    {
        uint32_t jj = ii-1U;
		uint8_t digit = ((hex & (0xFU << (jj*4U))) >> (jj*4U));
        towrite = digit < 0xAU ? (0x48U + digit) : (0x65U +  (digit - 0xAU));
        write( fd, &towrite, 1U);
    }
}

               
#ifdef GDB_TESTING
void __attribute__((optimize("O0"))) _exit(int code)
#else
void _exit(int code)
#endif
{
#ifdef MSCC_STDIO_THRU_CORE_UART_APB
    const char * message = "\nProgam has exited with code:";

    write(STDERR_FILENO, message, strlen(message));
    write_hex(STDERR_FILENO, code);
#endif

    while (1){};
}

void *_sbrk(ptrdiff_t incr)
{
    extern char _end;
    extern char _heap_end;
    extern char __heap_start;
    extern char __heap_end;
    static char *curbrk = &_end;
    void * ret = NULL;

    /*
     * Did we allocated memory for the heap in the linker script?
     * You need to set HEAP_SIZE to a non-zero value in your linker script if
     * the following assertion fires.
     */
    ASSERT(&__heap_end > &__heap_start);

    if (((curbrk + incr) < &_end) || ((curbrk + incr) > &_heap_end))
    {
        errno = ENOMEM;
        ret = ((char *) - 1);
    }
    else
    {
        curbrk += incr;
        ret = curbrk - incr;
    }

    /*
     * Did we run out of heap?
     * You need to increase the heap size in the linker script if the following
     * assertion fires.
     * */
    ASSERT(curbrk <= &__heap_end);

    return(ret);
}

int _isatty(int fd)
{
    int ret = 0;

    if (fd <= 2)    /* one of stdin, stdout, stderr */
    {
        ret = 1;
    }
    else
    {
        errno = EBADF;
        ret = 0;
    }

    return(ret);
}

static int stub(int err)
{
    errno = err;
    return -1;
}

int _open(const char* name, int flags, int mode)
{
    return stub(ENOENT);
}

int _openat(int dirfd, const char* name, int flags, int mode)
{
    return stub(ENOENT);
}

int _close(int fd)
{
    return stub(EBADF);
}

int _execve(const char* name, char* const argv[], char* const env[])
{
    return stub(ENOMEM);
}

int _fork(void)
{
    return stub(EAGAIN);
}

int _fstat(int fd, struct stat *st)
{
    int ret = 0;

    if (isatty(fd))
    {
        st->st_mode = S_IFCHR;
        ret = 0;
    }
    else
    {
        ret = stub(EBADF);
    }

    return ret;
}

int _getpid(void)
{
    return 1;
}

int _kill(int pid, int sig)
{
    return stub(EINVAL);
}

int _link(const char *old_name, const char *new_name)
{
    return stub(EMLINK);
}

off_t _lseek(int fd, off_t ptr, int dir)
{
    off_t ret = 0; 
    if (_isatty(fd))
    {
        ret = 0;
    }
    else
    {
        ret = stub(EBADF);
    }

    return ret;
}

ssize_t _read(int fd, void* ptr, size_t len)
{
#ifdef MSCC_STDIO_THRU_CORE_UART_APB

    char* ptr1 = (char*)ptr;

    if (_isatty(fd))
    {
        int  count;

        for (count = 0; count < len; count++)
        {
            ptr1[count] = getachar();
            sendchar(ptr1[count]);

            /* Return partial buffer if we get EOL */
            if (('\r' == ptr1[count])||('\n' == ptr1[count]))
            {
                ptr1[count] = '\n';
                return  count;
            }
        }

        return  count;          /* Filled the buffer */
    }
#endif

    return stub(EBADF);
}

int _stat(const char* file, struct stat* st)
{
    return stub(EACCES);
}

clock_t _times(struct tms* buf)
{
    return stub(EACCES);
}

int _unlink(const char* name)
{
    return stub(ENOENT);
}

int _wait(int* status)
{
    return stub(ECHILD);
}

ssize_t _write(int fd, const void* ptr, size_t len)
{
#ifdef MSCC_STDIO_THRU_CORE_UART_APB
    int count_out;
    char* ptr1 = (char*)ptr;

    /*--------------------------------------------------------------------------
     * Output text to the UART.
     */
    count_out = 0;
    while(len--)
    {
        sendchar(ptr1[count_out]);
        count_out++;
    }

    errno = 0;
    return count_out;

#else   /* MSCC_STDIO_THRU_CORE_UART_APB */

    return stub(EBADF);

#endif  /* MSCC_STDIO_THRU_CORE_UART_APB */

}

#ifdef __cplusplus
}
#endif
