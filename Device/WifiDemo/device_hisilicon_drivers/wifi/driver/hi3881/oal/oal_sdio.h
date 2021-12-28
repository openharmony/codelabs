/*
 * Copyright (C) 2021 HiSilicon (Shanghai) Technologies CO., LIMITED.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __OAL_SDIO_H__
#define __OAL_SDIO_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef  CONFIG_HI110X_SDIO_STD_CARD_SUPPORT
#define HISDIO_VENDOR_ID_HISI                      0x0296      /* VENDOR ID */
#define HISDIO_PRODUCT_ID_HISI                     0x5347      /* Product 1102 */
#else
#define HISDIO_VENDOR_ID_HISI                      0x22D1      /* VENDOR ID */
#define HISDIO_PRODUCT_ID_HISI                     0x1101      /* Product 1102 */
#endif

#define SDIO_MAX_CONTINUS_RX_COUNT                 64          /* sdio max continus rx cnt */

#define HISDIO_REG_FUNC1_FIFO                      0x00        /* Read Write FIFO */
#define HISDIO_REG_FUNC1_INT_STATUS                0x08        /* interrupt mask and clear reg */
#define HISDIO_REG_FUNC1_INT_ENABLE                0x09        /* interrupt */
#define HISDIO_REG_FUNC1_XFER_COUNT                0x0c        /* notify number of bytes to be read */
#define HISDIO_REG_FUNC1_WRITE_MSG                 0x24        /* write msg to device */
#define HISDIO_REG_FUNC1_MSG_FROM_DEV              0x28        /* notify Host that device has got the msg */
#define HISDIO_REG_FUNC1_MSG_HIGH_FROM_DEV         0x2b        /* Host receive the msg ack */

/* sdio extend function, add 64B register for hcc */
#define HISDIO_FUNC1_EXTEND_REG_BASE        0x3c
#define HISDIO_FUNC1_EXTEND_REG_LEN         64

#define HISDIO_FUNC1_INT_DREADY             (1 << 0)           /* data ready interrupt */
#define HISDIO_FUNC1_INT_RERROR             (1 << 1)           /* data read error interrupt */
#define HISDIO_FUNC1_INT_MFARM              (1 << 2)           /* ARM Msg interrupt */
#define HISDIO_FUNC1_INT_ACK                (1 << 3)           /* ACK interrupt */

#define HISDIO_FUNC1_INT_MASK (HISDIO_FUNC1_INT_DREADY | HISDIO_FUNC1_INT_RERROR | HISDIO_FUNC1_INT_MFARM)

#define HISDIO_RX_THREAD_NICE               (-20)
#define HISDIO_SDIO_RX_THREAD_POLICY        OAL_SCHED_FIFO
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#define HISDIO_GPIO_RX_THREAD_PRIORITY      20
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#define HISDIO_GPIO_RX_THREAD_PRIORITY      2
#endif

#define HISDIO_RX_THREAD_PRIORITY           99

#define HISDIO_SDIO_DISPOSE_THREAD_PRIORITY 10
#define HISDIO_RX_THREAD_STACKSIZE          0x6000

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
extern void hisi_sdio_rescan(int slot);
#else
extern int hisi_sdio_rescan(int slot);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of oal_sdio.h */
