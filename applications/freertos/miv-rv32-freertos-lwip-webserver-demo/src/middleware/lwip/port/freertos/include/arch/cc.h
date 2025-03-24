/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#ifndef __CC_H__
#define __CC_H__


#include <stdio.h>
#include <stdlib.h>

#include "cpu.h"
#include "drivers/fpga_ip/CoreUARTapb/core_uart_apb.h"

#define LWIP_SKIP_PACKING_CHECK


#define U16_F "hu"
#define X16_F "hX"
#define U32_F "u"
#define X32_F "X"
#define S16_F "hd"
#define S32_F "d"

typedef unsigned   char    u8_t;
typedef signed     char    s8_t;
typedef unsigned   short   u16_t;
typedef signed     short   s16_t;
typedef unsigned   long    u32_t;
typedef signed     long    s32_t;

UART_instance_t g_uart;

#if 0
typedef u32_t mem_ptr_t;
typedef int sys_prot_t;
#endif

#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_STRUCT
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x

#define LWIP_PLATFORM_DIAG(_x_) do { UART_polled_tx_string(&g_uart, (const uint8_t *)_x_); } while (0)
#define LWIP_PLATFORM_ASSERT(x) while(1);

#if 0
u32_t sys_arch_random(void);
#define LWIP_RAND   sys_arch_random

#else

#define LWIP_RAND() ((u32_t)rand())

#endif

#define LWIP_PLATFORM_BYTESWAP 0

//#define LWIP_PLATFORM_HTONS(x) __REV16(x)

//#define LWIP_PLATFORM_HTONL(x) __REV(x)

#if (__GNUC__ >= 5)
#define LWIP_TIMEVAL_PRIVATE 0
#endif

#ifndef LWIP_TIMEVAL_PRIVATE
#define LWIP_TIMEVAL_PRIVATE 1
#endif

uint16_t lwip_cortem_chksum(void *dataptr, int len);

//#define LWIP_CHKSUM lwip_cortem_chksum

#endif /* __CC_H__ */

