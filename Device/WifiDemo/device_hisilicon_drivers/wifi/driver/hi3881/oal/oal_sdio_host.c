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

/* ****************************************************************************
  1 其他头文件包含
**************************************************************************** */
#include "hdf_wlan_config.h"
#include "oal_util.h"
#include "oal_sdio.h"
#include "oal_sdio_host_if.h"
#include "oal_net.h"
#include "oal_chr.h"
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#include "plat_board_adapt.h"
#include <linux/types.h>
#endif

#ifdef CONFIG_MMC
#include "exception_rst.h"
#include "plat_pm.h"
#include "oal_interrupt.h"
#include "oal_thread.h"

#include "plat_firmware.h"
#include "oam_ext_if.h"
#include "oal_mm.h"

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#ifndef HAVE_PCLINT_CHECK
#include "oal_scatterlist.h"
#endif
#include "plat_board_adapt.h"
#endif
#endif
#include "hdf_wifi_core.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ****************************************************************************
  2 宏定义
**************************************************************************** */
#ifdef CONFIG_MMC
#define DELAY_10_US         10
#define TIMEOUT_MUTIPLE_10  10
#define TIMEOUT_MUTIPLE_5   5
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#define SDIO_RESET_RETRY    5
#define SDIO_RX_RETRY       5
#endif

#ifdef _PRE_HDF_LINUX
#ifndef UINTPTR
typedef uintptr_t UINTPTR;
#endif
#endif

/* ****************************************************************************
  3 Global Variable Definition
**************************************************************************** */
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
enum {
    DUMP_PREFIX_NONE,
    DUMP_PREFIX_ADDRESS,
    DUMP_PREFIX_OFFSET
};
#endif

struct task_struct *g_sdio_int_task = HI_NULL;
#undef CONFIG_SDIO_MSG_ACK_HOST2ARM_DEBUG

static  oal_completion  g_sdio_driver_complete;
oal_semaphore_stru g_chan_wake_sema;
static struct BusDev *g_bus = NULL;
static hi_char *g_sdio_enum_err_str = "probe timeout";

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
hi_u32 g_ul_pm_wakeup_event = HI_FALSE;
struct sdio_func *g_p_gst_sdio_func = HI_NULL;
#endif

/* 0 -sdio 1-gpio */
#ifdef _PRE_FEATURE_NO_GPIO
hi_s32 g_hisdio_intr_mode = 0;
#else
hi_s32 g_hisdio_intr_mode = 1;
#endif

#ifdef CONFIG_SDIO_FUNC_EXTEND
hi_u32 g_sdio_extend_func = 1;
#else
hi_u32 g_sdio_extend_func = 0;
#endif
hi_u32 g_wifi_patch_enable = 1;

module_param(g_hisdio_intr_mode, int, S_IRUGO | S_IWUSR);
module_param(g_sdio_extend_func, uint, S_IRUGO | S_IWUSR);
module_param(g_wifi_patch_enable, uint, S_IRUGO | S_IWUSR);

/* ****************************************************************************
 * 3 Function Definition
 * *************************************************************************** */
hi_s32 oal_sdio_data_sg_irq(struct BusDev *bus);

static hi_s32 oal_sdio_single_transfer(struct BusDev *bus, hi_s32 rw, hi_void *buf, hi_u32 size);
static hi_s32 _oal_sdio_transfer_scatt(struct BusDev *bus, hi_s32 rw, hi_u32 addr, struct scatterlist *sg,
    hi_u32 sg_len, hi_u32 rw_sz);
extern hi_void dw_mci_sdio_card_detect_change(hi_void);

static hi_void oal_sdio_print_state(hi_u32 old_state, hi_u32 new_state)
{
    if (old_state != new_state) {
        oam_info_log4(0, 0, "sdio state changed, tx[%d=>%d],rx[%d=>%d] (1:on, 0:off)\n",
            (old_state & OAL_SDIO_TX) ? 1 : 0, (new_state & OAL_SDIO_TX) ? 1 : 0,
            (old_state & OAL_SDIO_RX) ? 1 : 0, (new_state & OAL_SDIO_RX) ? 1 : 0);
    }
}
struct BusDev *oal_get_sdio_default_handler(hi_void)
{
    return g_bus;
}

hi_void oal_free_sdio_stru(oal_channel_stru *hi_sdio)
{
    hi_unref_param(hi_sdio);
    printk("oal_free_sdio_stru\n");
}

hi_s32 oal_sdio_send_msg(struct BusDev *bus, unsigned long val)
{
    hi_s32 ret;
    oal_channel_stru *hi_sdio = (oal_channel_stru *)bus->priData.data;
    if (hi_sdio == HI_NULL || hi_sdio->func == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{oal_sdio_send_msg::sdio is not initialized,can't send sdio msg!}");
        return -OAL_EINVAL;
    }
    if (hi_sdio->pst_pm_callback) {
        if (hi_sdio->pst_pm_callback->wlan_pm_wakeup_dev() != HI_SUCCESS) {
            oam_error_log0(0, OAM_SF_ANY, "{oal_sdio_send_msg::host wakeup device failed}");
        }
    }

    if (val >= H2D_MSG_COUNT) {
        oam_error_log1(0, OAM_SF_ANY, "[Error]invalid param[%lu]!\n", val);
        return -OAL_EINVAL;
    }
    oal_sdio_wake_lock(bus);
    bus->ops.claimHost(bus);
    /* sdio message can sent when wifi power on */
    if (wlan_pm_is_poweron() == 0) {
        oam_error_log0(0, OAM_SF_ANY, "{oal_sdio_send_msg::wifi power off,can't send sdio msg!}");
        bus->ops.releaseHost(bus);
        oal_sdio_wake_unlock(bus);
        return -OAL_EBUSY;
    }

    unsigned long data = 1 << val;
    ret = bus->ops.writeData(bus, HISDIO_REG_FUNC1_WRITE_MSG, FOUR_BYTE, (hi_u8 *)&data);
    if (ret) {
        oam_error_log2(0, OAM_SF_ANY, "{oal_sdio_send_msg::failed to send sdio msg[%lu]!ret=%d}", val, ret);
    }
    bus->ops.releaseHost(bus);
    oal_sdio_wake_unlock(bus);
    return ret;
}

/*
 * Description  : read or write buf
 * Input        : struct sdio_func *func, hi_s32 rw, hi_u32 addr, hi_u8 *buf, hi_u32 rw_sz
 * Output       : None
 * Return Value : static hi_s32
 */
hi_s32 oal_sdio_rw_buf(struct BusDev *bus, hi_s32 rw, hi_u32 addr, hi_u8 *buf, hi_u32 rw_sz)
{
    hi_s32 ret = HI_SUCCESS;

    /* padding len of buf has been assure when alloc */
    rw_sz = HISDIO_ALIGN_4_OR_BLK(rw_sz);
    if (OAL_WARN_ON(rw_sz != HISDIO_ALIGN_4_OR_BLK(rw_sz))) {
        /* just for debug, remove later */
        printk("invalid len %u\n", rw_sz);
        return -OAL_EINVAL;
    }

    bus->ops.claimHost(bus);
    if (rw == SDIO_READ) {
        ret = bus->ops.bulkRead(bus, addr, rw_sz, buf, 0);
    } else if (rw == SDIO_WRITE) {
        ret = bus->ops.bulkWrite(bus, addr, rw_sz, buf, 0);
    }

    bus->ops.releaseHost(bus);

    return ret;
}

hi_s32 oal_sdio_check_rx_len(oal_channel_stru *hi_sdio, hi_u32 count)
{
    hi_unref_param(hi_sdio);
    hi_unref_param(count);
    return HI_SUCCESS;
}

static hi_s32 oal_sdio_xfercount_get(struct BusDev *bus, hi_u32 *xfercount)
{
    hi_s32 ret = 0;
    oal_channel_stru *hi_sdio = (oal_channel_stru *)bus->priData.data;
#ifdef CONFIG_SDIO_MSG_ACK_HOST2ARM_DEBUG
    /* read from 0x0c */
    ret = bus->ops.readData(bus, HISDIO_REG_FUNC1_XFER_COUNT, FOUR_BYTE, (hi_u8*)xfercount);
    if (oal_unlikely(ret)) {
        printk("[ERROR]sdio read single package len failed ret=%d\n", ret);
        return ret;
    }
    hi_sdio->sdio_extend->xfer_count = *xfercount;
#else
    if (g_sdio_extend_func) {
        *xfercount = hi_sdio->sdio_extend->xfer_count;
        return HI_SUCCESS;
    }

    /* read from 0x0c */
    ret = bus->ops.readData(bus, HISDIO_REG_FUNC1_XFER_COUNT, FOUR_BYTE, (hi_u8*)xfercount);
    if (oal_unlikely(ret)) {
        printk("[E]sdio read xercount failed ret=%d\n", ret);
        return ret;
    }
    hi_sdio->sdio_extend->xfer_count = *xfercount;
#endif
    return HI_SUCCESS;
}

/*
 * Description  : rx data from sdio, Just support single transfer
 * Input        : None
 * Output       : None
 * Return Value : hi_s32
 */
hi_s32 oal_sdio_data_sg_irq(struct BusDev *bus)
{
    oal_channel_stru *hi_sdio = (oal_channel_stru *)bus->priData.data;
    hi_s32 ret;
    hi_u32              xfer_count;

    if (hi_sdio == HI_NULL || hi_sdio->func == HI_NULL || hi_sdio->bus_data == HI_NULL) {
        return -OAL_EINVAL;
    }

    ret = oal_sdio_xfercount_get(bus, &xfer_count);
    if (oal_unlikely(ret)) {
        return -OAL_EFAIL;
    }

    if (oal_unlikely(oal_sdio_check_rx_len(hi_sdio, xfer_count) != HI_SUCCESS)) {
        printk("[SDIO][Err]Sdio Rx Single Transfer len[%u] invalid\n", xfer_count);
    }

    /* beacause get buf may cost lot of time, so release bus first */
    if (hi_sdio->bus_ops.rx == HI_NULL) {
        return -OAL_EINVAL;
    }

    bus->ops.releaseHost(bus);
    hi_sdio->bus_ops.rx(hi_sdio->bus_data);
    bus->ops.claimHost(bus);

    return HI_SUCCESS;
}

hi_s32 oal_sdio_transfer_rx_register(struct BusDev *bus, hisdio_rx rx)
{
    oal_channel_stru *hi_sdio = (oal_channel_stru *)bus->priData.data;
    if (hi_sdio == HI_NULL) {
        return -OAL_EINVAL;
    }
    hi_sdio->bus_ops.rx = rx;
    return HI_SUCCESS;
}

/*
 * Description  : msg irq
 * Input        : hisdio_rx rx
 * Output       : HI_NULL
 * Return Value : hi_s32
 */
hi_void oal_sdio_transfer_rx_unregister(struct BusDev *bus)
{
    oal_channel_stru *hi_sdio = (oal_channel_stru *)bus->priData.data;
    hi_sdio->bus_ops.rx = HI_NULL;
}

/*
 * Description  : sdio msg register
 * Input        :
 * Output       : None
 * Return Value : hi_s32
 */
hi_s32 oal_sdio_message_register(struct BusDev *bus, hi_u8 msg, sdio_msg_rx cb, hi_void *data)
{
    oal_channel_stru *hi_sdio = (oal_channel_stru *)bus->priData.data;
    if (hi_sdio == HI_NULL || msg >= D2H_MSG_COUNT) {
        return -OAL_EFAIL;
    }
    hi_sdio->msg[msg].msg_rx = cb;
    hi_sdio->msg[msg].data = data;
    return HI_SUCCESS;
}

/*
 * Description  : sdio msg unregister
 * Input        :
 * Output       : None
 * Return Value : hi_s32
 */
hi_void oal_sdio_message_unregister(struct BusDev *bus, hi_u8 msg)
{
    oal_channel_stru *hi_sdio = NULL;
    if (bus == HI_NULL || msg >= D2H_MSG_COUNT) {
        return;
    }
    hi_sdio = (oal_channel_stru *)bus->priData.data;
    hi_sdio->msg[msg].msg_rx = HI_NULL;
    hi_sdio->msg[msg].data = HI_NULL;
}

static hi_s32 oal_sdio_msg_stat(struct BusDev *bus, hi_u32 *msg)
{
    hi_s32 ret = 0;
    oal_channel_stru *hi_sdio = (oal_channel_stru *)bus->priData.data;
#ifdef CONFIG_SDIO_MSG_ACK_HOST2ARM_DEBUG
    /* read from old register */
#ifdef CONFIG_SDIO_D2H_MSG_ACK
    ret = bus->ops.readData(bus, HISDIO_REG_FUNC1_MSG_FROM_DEV, FOUR_BYTE, (hi_u8 *)msg);
#else
    ret = bus->ops.readData(bus, HISDIO_REG_FUNC1_MSG_FROM_DEV, ONE_BYTE, (hi_u8 *)msg);
#endif

    if (ret) {
        printk("sdio readb error![ret=%d]\n", ret);
        return ret;
    }
    hi_sdio->sdio_extend->msg_stat = *msg;
#else
    if (g_sdio_extend_func) {
        *msg = hi_sdio->sdio_extend->msg_stat;
    }

    if (*msg == 0) {
        /* no sdio message! */
        return HI_SUCCESS;
    }
#ifdef CONFIG_SDIO_D2H_MSG_ACK
    /* read from old register */
    /* 当使用0x30寄存器时需要下发CMD52读0x2B 才会产生HOST2ARM ACK中断 */
    hi_u8 buf;
    ret = bus->ops.readData(bus, HISDIO_REG_FUNC1_MSG_HIGH_FROM_DEV, ONE_BYTE, &buf);
    if (ret) {
        printk("[E]sdio readb error![ret=%d]\n", ret);
    }
#endif
#endif
    return HI_SUCCESS;
}

/*
 * Description  : msg irq
 * Input        : oal_channel_stru *hi_sdio
 * Output       : None
 * Return Value : hi_s32
 */
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#define cpu_clock(m) (m)
#endif

hi_s32 oal_sdio_msg_irq(struct BusDev *bus)
{
    oal_bitops           bit = 0;
    hi_u32               msg = 0;
    hi_s32               ret;
    unsigned long        msg_tmp;
    oal_channel_stru *hi_sdio = (oal_channel_stru *)bus->priData.data;
    /* reading interrupt form ARM Gerneral Purpose Register(0x28)  */
    ret = oal_sdio_msg_stat(bus, &msg);
    if (ret) {
        printk("[SDIO][Err]oal_sdio_msg_stat error![ret=%d]\n", ret);
        return ret;
    }
    msg_tmp = (unsigned long)msg;

    if (!msg) {
        return HI_SUCCESS;
    }
    if (oal_bit_atomic_test(D2H_MSG_DEVICE_PANIC, &msg_tmp)) {
        oal_disable_sdio_state(bus, OAL_SDIO_ALL);
    }
    oal_sdio_release_host(bus);
    oal_sdio_rx_transfer_unlock(bus);
    if (oal_bit_atomic_test_and_clear(D2H_MSG_DEVICE_PANIC, &msg_tmp)) {
        bit = D2H_MSG_DEVICE_PANIC;
        hi_sdio->msg[bit].count++;
        hi_sdio->last_msg = bit;
        hi_sdio->msg[bit].cpu_time = cpu_clock(UINT_MAX);
        if (hi_sdio->msg[bit].msg_rx) {
            printk("device panic msg come, 0x%8x\n", msg);
            hi_sdio->msg[bit].msg_rx(hi_sdio->msg[bit].data);
        }
    }
    bit = 0;
    oal_bit_atomic_for_each_set(bit, (const unsigned long *)&msg_tmp, D2H_MSG_COUNT) {
        if (bit >= D2H_MSG_COUNT) {
            printk("oal_sdio_msg_irq, bit >= D2H_MSG_COUNT\n");
            return -OAL_EFAIL;
        }
        hi_sdio->msg[bit].count++;
        hi_sdio->last_msg = bit;
        hi_sdio->msg[bit].cpu_time = cpu_clock(UINT_MAX);
        if (hi_sdio->msg[bit].msg_rx) {
            hi_sdio->msg[bit].msg_rx(hi_sdio->msg[bit].data);
        }
    }
    oal_sdio_rx_transfer_lock(bus);
    oal_sdio_claim_host(bus);

    return HI_SUCCESS;
}

hi_u32 oal_sdio_credit_info_update(oal_channel_stru *hi_sdio)
{
    hi_u8 short_free_cnt, large_free_cnt;
    hi_u32 ret = 0;
    oal_spin_lock(&hi_sdio->sdio_credit_info.credit_lock);

    short_free_cnt = hisdio_short_pkt_get(hi_sdio->sdio_extend->credit_info);
    large_free_cnt = hisdio_large_pkt_get(hi_sdio->sdio_extend->credit_info);

    if (hi_sdio->sdio_credit_info.short_free_cnt != short_free_cnt) {
#ifdef CONFIG_SDIO_DEBUG
        printk("short free cnt:%d ==> %d\r\n", hi_sdio->sdio_credit_info.short_free_cnt, short_free_cnt);
#endif
        hi_sdio->sdio_credit_info.short_free_cnt = short_free_cnt;
        ret = 1;
    }

    if (hi_sdio->sdio_credit_info.large_free_cnt != large_free_cnt) {
#ifdef CONFIG_SDIO_DEBUG
        printk("large free cnt:%d ==> %d\r\n", hi_sdio->sdio_credit_info.large_free_cnt, large_free_cnt);
#endif
        hi_sdio->sdio_credit_info.large_free_cnt = large_free_cnt;
        ret = 1;
    }

    oal_spin_unlock(&hi_sdio->sdio_credit_info.credit_lock);

    return ret;
}

hi_void oal_sdio_credit_update_cb_register(oal_channel_stru *hi_sdio, hisdio_rx cb)
{
    if (hi_sdio == HI_NULL) {
        return;
    }
    if (OAL_WARN_ON(hi_sdio->credit_update_cb != HI_NULL)) {
        return;
    }
    hi_sdio->credit_update_cb = cb;
    return;
}

hi_s32 oal_sdio_get_credit(struct BusDev *bus, hi_u32 *uc_hipriority_cnt)
{
    hi_s32 ret;
    bus->ops.claimHost(bus);
    ret = oal_sdio_memcpy_fromio(bus, (hi_u8 *)uc_hipriority_cnt,
                                 HISDIO_EXTEND_CREDIT_ADDR, sizeof(*uc_hipriority_cnt));
    bus->ops.releaseHost(bus);
    /* 此处要让出CPU */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    schedule();
#endif
    return ret;
}

#ifndef CONFIG_SDIO_MSG_ACK_HOST2ARM_DEBUG
static hi_s32 oal_sdio_extend_buf_get(struct BusDev *bus)
{
    hi_s32 ret = HI_SUCCESS;
    oal_channel_stru *hi_sdio = (oal_channel_stru *)bus->priData.data;
    if (g_sdio_extend_func) {
        ret = oal_sdio_memcpy_fromio(bus, (hi_void *)hi_sdio->sdio_extend,
                                     HISDIO_EXTEND_BASE_ADDR, sizeof(hisdio_extend_func));
        if (oal_likely(ret == HI_SUCCESS)) {
#ifdef CONFIG_SDIO_DEBUG
            printk(KERN_DEBUG"=========extend buff:%d=====\n",
                   HISDIO_COMM_REG_SEQ_GET(hi_sdio->sdio_extend->credit_info));
            oal_print_hex_dump((hi_u8 *)hi_sdio->sdio_extend,
                               sizeof(hisdio_extend_func), 32, "extend :"); /* 32 进制 */

            /* 此credit更新只在调试时使用 */
            if (oal_sdio_credit_info_update(hi_sdio)) {
                if (oal_likely(hi_sdio->credit_update_cb)) {
                    hi_sdio->credit_update_cb(hi_sdio->bus_data);
                }
            }
#endif
        } else {
            oam_info_log0(0, OAM_SF_ANY, "{[SDIO][Err]sdio read extend_buf fail!}");
        }
    }
    return ret;
}
#else
static hi_s32 oal_sdio_extend_buf_get(struct BusDev *bus)
{
    hi_s32 ret;
    oal_channel_stru *hi_sdio = (oal_channel_stru *)bus->priData.data;
    {
        memset_s(hi_sdio->sdio_extend, sizeof(struct hisdio_extend_func), 0, sizeof(struct hisdio_extend_func));
        ret = oal_sdio_memcpy_fromio(bus, (hi_void *)&hi_sdio->sdio_extend->credit_info,
            HISDIO_EXTEND_BASE_ADDR + 12, HISDIO_EXTEND_REG_COUNT + 4); /* addr+ 12, count+ 4 */
#ifdef CONFIG_SDIO_DEBUG
        if (ret == HI_SUCCESS) {
            printk(KERN_DEBUG "=========extend buff:%d=====\n",
                HISDIO_COMM_REG_SEQ_GET(hi_sdio->sdio_extend->credit_info));
            oal_print_hex_dump((hi_u8 *)hi_sdio->sdio_extend,
                               sizeof(struct hisdio_extend_func), 32, "extend :"); /* 32 进制 */
        }
#endif
    }
    return ret;
}
#endif

hi_s32 oal_sdio_transfer_rx_reserved_buff(struct BusDev *bus)
{
    hi_u32 i = 0;
    hi_s32 ret;
    hi_u32 left_size;
    hi_u32 seg_nums, seg_size;
    struct scatterlist *sg = HI_NULL;
    struct scatterlist *sg_t = HI_NULL;
    oal_channel_stru *hi_sdio = (oal_channel_stru *)bus->priData.data;
    if (hi_sdio->sdio_extend == HI_NULL) {
        oam_error_log0(0, OAM_SF_ANY, "{hi_sdio->sdio_extend NULL!}");
        return -OAL_EINVAL;
    }

    hi_u32 ul_extend_len = hi_sdio->sdio_extend->xfer_count;

    if (ul_extend_len == 0) {
        oam_error_log0(0, OAM_SF_ANY, "{extend_len is zero!}");
        return -OAL_EINVAL;
    }

    seg_size = hi_sdio->rx_reserved_buff_len;
    if (seg_size == 0) {
        oam_error_log0(0, OAM_SF_ANY, "{seg_size is zero!}");
        return -OAL_EINVAL;
    }
    seg_nums = ((ul_extend_len - 1) / seg_size) + 1;
    if (hi_sdio->scatt_info[SDIO_READ].max_scatt_num < seg_nums) {
        oam_error_log2(0, OAM_SF_ANY, "{sdio seg nums :%u large than rx scatt num %u!}", seg_nums,
            hi_sdio->scatt_info[SDIO_READ].max_scatt_num);
        return -OAL_EINVAL;
    }

    oam_info_log1(0, OAM_SF_ANY, "{drop the rx buff length:%u}", ul_extend_len);

    sg = hi_sdio->scatt_info[SDIO_READ].sglist;
    if (sg == HI_NULL) {
        printk("oal_sdio_transfer_rx_reserved_buff::sg is null!\n");
        return -OAL_EINVAL;
    }
    sg_init_table(sg, seg_nums);
    left_size = ul_extend_len;
    for_each_sg(sg, sg_t, seg_nums, i) {
        if (sg_t == HI_NULL) {
            printk("oal_sdio_transfer_rx_reserved_buff::sg_t is null!\n");
            return -OAL_EINVAL;
        }
        sg_set_buf(sg_t, (const void *)(UINTPTR)hi_sdio->rx_reserved_buff, oal_min(seg_size, left_size));
        left_size = left_size - seg_size;
    }
    ret = _oal_sdio_transfer_scatt(bus, SDIO_READ, HISDIO_REG_FUNC1_FIFO, sg, seg_nums, ul_extend_len);
    if (oal_unlikely(ret)) {
        printk("sdio trans revered mem failed! ret=%d\n", ret);
    }
    return ret;
}

#undef CONFIG_SDIO_RX_NETBUF_ALLOC_FAILED_DEBUG
#ifdef CONFIG_SDIO_RX_NETBUF_ALLOC_FAILED_DEBUG
hi_u32 g_rx_alloc_netbuf_debug = 0;
module_param(g_rx_alloc_netbuf_debug, uint, S_IRUGO | S_IWUSR);
#endif

oal_netbuf_stru *oal_sdio_alloc_rx_netbuf(hi_u32 ul_len)
{
#ifdef CONFIG_SDIO_RX_NETBUF_ALLOC_FAILED_DEBUG
    if (g_rx_alloc_netbuf_debug) {
        if (prandom_u32() % 256) { /* 256 */
            return HI_NULL;
        }
    }
#endif

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    return __netdev_alloc_skb(HI_NULL, ul_len, GFP_KERNEL);
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)

#ifdef _PRE_LWIP_ZERO_COPY
    return oal_pbuf_netbuf_alloc(ul_len);
#else
    return oal_dev_alloc_skb(ul_len);
#endif
#endif
}

hi_s32 oal_sdio_build_rx_netbuf_list(struct BusDev *bus, oal_netbuf_head_stru    *head)
{
#ifdef CONFIG_SDIO_FUNC_EXTEND
    hi_s32 i;
    hi_u8 buff_len;
    hi_u16 buff_len_t;
#endif
    hi_s32 ret = HI_SUCCESS;
    hi_u32 sum_len = 0;
    oal_netbuf_stru *netbuf = HI_NULL;
    oal_channel_stru *hi_sdio = (oal_channel_stru *)bus->priData.data;

    if (!oal_netbuf_list_empty(head)) {
        oam_error_log0(0, OAM_SF_ANY, "oal_sdio_build_rx_netbuf_list: oal netbuf list empty");
        return -OAL_EINVAL;
    }
#ifdef CONFIG_SDIO_FUNC_EXTEND
    for (i = 0; i < HISDIO_EXTEND_REG_COUNT; i++) {
        buff_len = hi_sdio->sdio_extend->comm_reg[i];
        if (buff_len == 0) {
            break;
        }

        buff_len_t = buff_len << HISDIO_D2H_SCATT_BUFFLEN_ALIGN_BITS;

        netbuf = oal_sdio_alloc_rx_netbuf(buff_len_t);
        if (netbuf == HI_NULL) {
            oam_error_log2(0, OAM_SF_ANY, "{[WIFI][E]rx no mem:%u, index:%d}", buff_len, i);
            goto failed_netbuf_alloc;
        }

        oal_netbuf_put(netbuf, buff_len_t);
        sum_len += buff_len_t;

        if ((!oal_netbuf_head_prev(head)) || (!oal_netbuf_head_next(head))) {
            oam_error_log0(0, OAM_SF_ANY, "oal_sdio_build_rx_netbuf_list: head null");
            goto failed_netbuf_alloc;
        }
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
        __skb_queue_tail(head, netbuf);
#else
        oal_netbuf_list_tail(head, netbuf);
#endif
    }

    if (OAL_WARN_ON(HISDIO_ALIGN_4_OR_BLK(sum_len) != hi_sdio->sdio_extend->xfer_count)) {
        oam_warning_log3(0, OAM_SF_ANY, "{[WIFI][E]scatt total len[%u] should = xfercount[%u],after pad len:%u}",
            sum_len, hi_sdio->sdio_extend->xfer_count, HISDIO_ALIGN_4_OR_BLK(sum_len));
        hi_sdio->error_stat.rx_scatt_info_not_match++;
        goto failed_netbuf_alloc;
    }
#else
    netbuf = oal_sdio_alloc_rx_netbuf(hi_sdio->sdio_extend->xfer_count);
    if (netbuf == HI_NULL) {
        oam_error_log1(0, OAM_SF_ANY, "{rx no mem:%u}", hi_sdio->sdio_extend->xfer_count);
        goto failed_netbuf_alloc;
    }

    oal_netbuf_put(netbuf, hi_sdio->sdio_extend->xfer_count);
    sum_len += hi_sdio->sdio_extend->xfer_count;
    __skb_queue_tail(head, netbuf);
#endif

    if (oal_unlikely(oal_netbuf_list_empty(head))) {
#ifdef CONFIG_PRINTK
        printk("unvaild scatt info:xfercount:%u\n", hi_sdio->sdio_extend->xfer_count);
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
        print_hex_dump_bytes("scatt extend:", DUMP_PREFIX_ADDRESS, hi_sdio->sdio_extend->comm_reg,
            HISDIO_EXTEND_REG_COUNT);
#endif
#endif
        return -OAL_EINVAL;
    }

    return ret;
failed_netbuf_alloc:
    oal_netbuf_list_purge(head);
    ret = oal_sdio_transfer_rx_reserved_buff(bus);
    if (ret != HI_SUCCESS) {
        printk("oal_sdio_transfer_rx_reserved_buff fail\n");
    }
    return -OAL_ENOMEM;
}

static hi_s32 oal_sdio_get_func1_int_status(struct BusDev *bus, hi_u8 *int_stat)
{
    hi_s32 ret = 0;
    oal_channel_stru *hi_sdio = (oal_channel_stru *)bus->priData.data;
    if (g_sdio_extend_func) {
        hi_sdio->sdio_extend->int_stat &= hi_sdio->func1_int_mask;
        *int_stat = (hi_sdio->sdio_extend->int_stat & 0xF);
        return HI_SUCCESS;
    } else {
        /* read interrupt indicator register */
        ret = bus->ops.readData(bus, HISDIO_REG_FUNC1_INT_STATUS, ONE_BYTE, int_stat);
        if (oal_unlikely(ret)) {
            printk("[SDIO][Err]failed to read sdio func1 interrupt status!ret=%d\n", ret);
            return ret;
        }
        *int_stat = (*int_stat) & hi_sdio->func1_int_mask;
    }
    return HI_SUCCESS;
}

static hi_s32 oal_sdio_clear_int_status(struct BusDev *bus, hi_u8 int_stat)
{
    hi_s32 ret = 0;

    if (g_sdio_extend_func) {
        return HI_SUCCESS;
    }
    ret = bus->ops.writeData(bus, HISDIO_REG_FUNC1_INT_STATUS, ONE_BYTE, &int_stat);
    if (oal_unlikely(ret)) {
        printk("[SDIO][Err]failed to clear sdio func1 interrupt!ret=%d\n", ret);
        return ret;
    }
    return HI_SUCCESS;
}

/*
 * Description  : sdio rx data
 * Input        :
 * Output       : None
 * Return Value : hi_s32
 */
hi_s32 oal_sdio_do_isr(struct BusDev *bus)
{
    hi_u8 int_mask;
    hi_s32 ret;
    oal_channel_stru *hi_sdio = (oal_channel_stru *)bus->priData.data;

    hi_sdio->sdio_int_count++;
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    hi_s32 rx_retry_count = SDIO_RX_RETRY;
#endif
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    /* sdio bus state access lock by sdio bus claim locked. */
    if (oal_unlikely(HI_TRUE != oal_sdio_get_state(hi_sdio, OAL_SDIO_RX))) {
        return HI_SUCCESS;
    }
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    /* sdio bus state access lock by sdio bus claim locked. */
    while (1) {
        if (oal_unlikely(HI_TRUE == oal_sdio_get_state(hi_sdio, OAL_SDIO_RX))) {
            break;
        }
        rx_retry_count--;
        if (rx_retry_count == 0) {
            printk("[SDIO][W][%s]sdio closed,state:%u\n", __FUNCTION__, oal_sdio_get_state(hi_sdio, OAL_SDIO_RX));
            return HI_SUCCESS;
        }
        msleep(10); /* sleep 10ms */
    }
#endif

#ifndef CONFIG_SDIO_MSG_ACK_HOST2ARM_DEBUG
    ret = oal_sdio_extend_buf_get(bus);
    if (oal_unlikely(ret)) {
        printk("[SDIO][Err]failed to read sdio extend area ret=%d\n", ret);
        return -OAL_EFAIL;
    }
#endif

    ret = oal_sdio_get_func1_int_status(bus, &int_mask);
    if (oal_unlikely(ret)) {
        return ret;
    }

    if (oal_unlikely(0 == (int_mask & HISDIO_FUNC1_INT_MASK))) {
        hi_sdio->func1_stat.func1_no_int_count++;
        return HI_SUCCESS;
    }

    /* clear interrupt mask */
    ret = oal_sdio_clear_int_status(bus, int_mask);
    if (oal_unlikely(ret)) {
        return ret;
    }

    if (int_mask & HISDIO_FUNC1_INT_RERROR) {
        /* try to read the data again */
        hi_sdio->func1_stat.func1_err_int_count++;
    }

    /* message interrupt, flow control */
    if (int_mask & HISDIO_FUNC1_INT_MFARM) {
        hi_sdio->func1_stat.func1_msg_int_count++;
        if (oal_sdio_msg_irq(bus) != HI_SUCCESS) {
            return -OAL_EFAIL;
        }
    }

    if (int_mask & HISDIO_FUNC1_INT_DREADY) {
        hi_sdio->func1_stat.func1_data_int_count++;
        return oal_sdio_data_sg_irq(bus);
    }
    hi_sdio->func1_stat.func1_unknow_int_count++;
    return HI_SUCCESS;
}

/*
 * Description  : sdio interrupt routine
 * Input        : func  sdio_func handler
 * Output       : None
 * Return Value : err or succ
 */
hi_void oal_sdio_isr(void *func)
{
    struct BusDev *bus = oal_get_bus_default_handler();
    hi_s32 ret;

    if (func == HI_NULL) {
        oam_error_log0(0, 0, "oal_sdio_isr func null\n");
        return;
    }
    bus->ops.claimHost(bus);
    ret = oal_sdio_do_isr(bus);
    if (oal_unlikely(ret)) {
        oam_error_log0(0, 0, "oal_sdio_do_isr fail\n");
        oal_exception_submit(TRANS_FAIL);
    }
    bus->ops.releaseHost(bus);
}

#undef COFNIG_TEST_SDIO_INT_LOSS

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
oal_atomic g_pm_spinlock_get;
#endif

/*
 * Description  : initialize sdio pm interface
 * Input        :
 * Output       : None
 * Return Value : hi_void
 */
hi_void oal_unregister_gpio_intr(struct BusDev *bus)
{
    oal_channel_stru *hi_sdio = (oal_channel_stru *)bus->priData.data;
    /* disable wlan irq */
    oal_wlan_gpio_intr_enable(bus, HI_FALSE);

    /* free irq when sdio driver deinit */
    oal_free_irq(hi_sdio->ul_wlan_irq, hi_sdio);
    oal_kthread_stop(hi_sdio->gpio_rx_tsk);
    hi_sdio->gpio_rx_tsk = HI_NULL;
}

/* ****************************************************************************
 功能描述  : 使能/关闭 WLAN GPIO 中断
 输入参数  : 1:enable; 0:disenable
 输出参数  : 无
 返 回 值  : 成功或失败原因
**************************************************************************** */
hi_void oal_wlan_gpio_intr_enable(struct BusDev *bus, hi_u32  ul_en)
{
    oal_channel_stru *hi_sdio = (oal_channel_stru *)bus->priData.data;
#ifndef _PRE_FEATURE_NO_GPIO
    unsigned long flags;

    oal_spin_lock_irq_save(&hi_sdio->st_irq_lock, &flags);
    if (ul_en) {
        oal_enable_irq(hi_sdio->ul_wlan_irq);
    } else {
        oal_disable_irq_nosync(hi_sdio->ul_wlan_irq);
    }
    oal_spin_unlock_irq_restore(&hi_sdio->st_irq_lock, &flags);
#else
    hi_unref_param(hi_sdio);
    hi_unref_param(ul_en);
#endif
}

hi_s32 oal_register_sdio_intr(struct BusDev *bus)
{
    hi_s32 ret;

    bus->ops.claimHost(bus);
    /* use sdio bus line data1 for sdio data interrupt */
    ret = bus->ops.claimIrq(bus, (IrqHandler *)oal_sdio_isr, NULL);
    if (ret < 0) {
        oam_error_log0(0, 0, "oal_register_sdio_intr:: failed to register sdio interrupt");
        bus->ops.releaseHost(bus);
        return -OAL_EFAIL;
    }
    bus->ops.releaseHost(bus);
    oam_info_log0(0, 0, "oal_register_sdio_intr:: sdio interrupt register");
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    oal_channel_stru *hi_sdio = (oal_channel_stru *)bus->priData.data;
    struct sdio_func *func = hi_sdio->func;
    pm_runtime_get_sync(mmc_dev(hi_sdio->func->card->host));
#endif
    return ret;
}

hi_void oal_unregister_sdio_intr(struct BusDev *bus)
{
    bus->ops.claimHost(bus);
    /* use sdio bus line data1 for sdio data interrupt */
    bus->ops.releaseIrq(bus);
    bus->ops.releaseHost(bus);
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    oal_channel_stru *hi_sdio = (oal_channel_stru *)bus->priData.data;
    struct sdio_func *func = hi_sdio->func;
    pm_runtime_put_sync(mmc_dev(hi_sdio->func->card->host));
#endif
}

/*
 * Description  : unregister interrupt
 * Input        : None
 * Output       : None
 */
hi_void oal_sdio_interrupt_unregister(struct BusDev *bus)
{
    if (g_hisdio_intr_mode) {
        /* use GPIO interrupt for sdio data interrupt */
        oal_unregister_gpio_intr(bus);
    } else {
        /* use sdio interrupt for sdio data interrupt */
        oal_unregister_sdio_intr(bus);
    }
}

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
unsigned long oal_sdio_get_sleep_state(struct BusDev *bus)
{
    int ret;
    unsigned long ul_value;

    bus->ops.claimHost(bus);
    (void)bus->ops.readFunc0(bus, HISDIO_WAKEUP_DEV_REG, 1, (hi_u8 *)&ul_value);
    bus->ops.releaseHost(bus);

    return ul_value;
}
#endif

/*
 * Description  : get device low power state by sdio 0xf1~0xf4 registers for debug
 * Input        : None
 * Output       : None
 */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
hi_void oal_sdio_get_dev_pm_state(struct BusDev *bus, unsigned long *pst_ul_f1,
    unsigned long *pst_ul_f2, unsigned long *pst_ul_f3, unsigned long *pst_ul_f4)
{
    int ret;
    bus->ops.claimHost(bus);
    (void)bus->ops.readFunc0(bus, 0xf1, 1, (hi_u8 *)pst_ul_f1);
    (void)bus->ops.readFunc0(bus, 0xf2, 1, (hi_u8 *)pst_ul_f2);
    (void)bus->ops.readFunc0(bus, 0xf3, 1, (hi_u8 *)pst_ul_f3);
    (void)bus->ops.readFunc0(bus, 0xf4, 1, (hi_u8 *)pst_ul_f4);
    bus->ops.releaseHost(bus);

    return;
}
#endif

/*
 * Description  : wakeup device
 * Input        : None
 * Output       : None
 * Return Value :
 */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
hi_s32 oal_sdio_wakeup_dev(struct BusDev *bus)
{
    int ret;
    oal_channel_stru *hi_sdio = (oal_channel_stru *)bus->priData.data;
    if (hi_sdio == HI_NULL) {
        return -OAL_EFAIL;
    }
    oal_sdio_claim_host(bus);
    hi_u32 data = DISALLOW_TO_SLEEP_VALUE;
    ret = bus->ops.writeFunc0(bus, HISDIO_WAKEUP_DEV_REG, 1, (hi_u8 *)&data);
    oal_sdio_release_host(bus);

    return ret;
}
#endif

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
hi_s32 oal_sdio_wakeup_dev(struct BusDev *bus)
{
    int ret;
    oal_channel_stru *hi_sdio = (oal_channel_stru *)bus->priData.data;
    if (hi_sdio == HI_NULL) {
        return -OAL_EFAIL;
    }
    oal_sdio_claim_host(bus);
    hi_u32 data = DISALLOW_TO_SLEEP_VALUE;
    ret = bus->ops.writeFunc0(bus, HISDIO_WAKEUP_DEV_REG, 1, (hi_u8 *)&data);
    oal_sdio_release_host(bus);
    return ret;
}
#endif

/*
 * Description  : allow device to sleep
 * Input        : None
 * Output       : None
 * Return Value :
 */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
hi_s32 oal_sdio_sleep_dev(struct BusDev *bus)
{
    oal_channel_stru *hi_sdio = (oal_channel_stru *)bus->priData.data;
    if (hi_sdio == HI_NULL) {
        return -OAL_EFAIL;
    }
    int ret;
    oal_sdio_claim_host(bus);
    hi_u32 data = ALLOW_TO_SLEEP_VALUE;
    ret = bus->ops.writeFunc0(bus, HISDIO_WAKEUP_DEV_REG, 1, (hi_u8 *)&data);
    oal_sdio_release_host(bus);
    return ret;
}
#endif

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
hi_s32 oal_sdio_sleep_dev(struct BusDev *bus)
{
    oal_channel_stru *hi_sdio = (oal_channel_stru *)bus->priData.data;

    if (hi_sdio == HI_NULL) {
        return -OAL_EFAIL;
    }
    oal_sdio_claim_host(bus);
    uint8_t data = ALLOW_TO_SLEEP_VALUE;
    (void)bus->ops.writeFunc0(bus, HISDIO_WAKEUP_DEV_REG, 1, &data);
    oal_sdio_release_host(bus);

    return HI_SUCCESS;
}
#endif

/*
 * Description  : init sdio function
 * Input        : adapter   oal_sdio handler
 * Output       :
 * Return Value : succ or fail
 */
hi_s32 oal_sdio_dev_init(struct BusDev *bus)
{
    hi_s32 ret;
    hi_u32 data;
    oal_channel_stru *hi_sdio = (oal_channel_stru *)bus->priData.data;
    if (hi_sdio == HI_NULL) {
        return -OAL_EFAIL;
    }

    oal_sdio_claim_host(bus);
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    oal_disable_sdio_state(bus, OAL_SDIO_ALL);
#endif

    /* before enable sdio function 1, clear its interrupt flag, no matter it exist or not */
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#ifdef _PRE_WLAN_PM_FEATURE_FORCESLP_RESUME
    if (wlan_resume_state_get() == 0) { /* host镜像恢复起来不清中断，防止数据丢失 */
#endif
#endif
        data = HISDIO_FUNC1_INT_MASK;
        ret = bus->ops.writeData(bus, HISDIO_REG_FUNC1_INT_STATUS, ONE_BYTE, (hi_u8 *)&data);
        if (ret) {
            printk("failed to clear sdio interrupt! ret=%d\n", ret);
            goto failed_clear_func1_int;
        }
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#ifdef _PRE_WLAN_PM_FEATURE_FORCESLP_RESUME
    }
#endif
#endif
    /*
     * enable four interrupt sources in function 1:
     * data ready for host to read
     * read data error
     * message from arm is available
     * device has receive message from host
     *  */
    data = HISDIO_FUNC1_INT_MASK;
    ret = bus->ops.writeData(bus, HISDIO_REG_FUNC1_INT_ENABLE, ONE_BYTE, (hi_u8 *)&data);
    if (ret < 0) {
        printk("failed to enable sdio interrupt! ret=%d\n", ret);
        goto failed_enable_func1;
    }

    oal_enable_sdio_state(bus, OAL_SDIO_ALL);
    oal_sdio_release_host(bus);

    return HI_SUCCESS;

failed_enable_func1:
failed_clear_func1_int:
    (void)bus->ops.disableBus(bus);
    oal_sdio_release_host(bus);
    return ret;
}

/*
 * Description  : deinit
 * Input        :
 * Output       : None
 * Return Value : hi_s32
 */
static hi_void oal_sdio_dev_deinit(struct BusDev *bus)
{
    bus->ops.claimHost(bus);
    hi_u32 data = 0;
    (void)bus->ops.writeData(bus, HISDIO_REG_FUNC1_INT_ENABLE, ONE_BYTE, (uint8_t *)&data);
    oal_sdio_interrupt_unregister(bus);
    (void)bus->ops.disableBus(bus);
    oal_disable_sdio_state(bus, OAL_SDIO_ALL);
    bus->ops.releaseHost(bus);

    printk("oal_sdio_dev_deinit! \n");
}

/*
 * Description  : get the sdio bus state
 * Input        :
 * Output       : None
 * Return Value : TRUE/FALSE
 */
hi_s32 oal_sdio_get_state(const oal_channel_stru *hi_sdio, hi_u32 mask)
{
    if (hi_sdio == HI_NULL) {
        return HI_FALSE;
    }

    if ((hi_sdio->state & mask) == mask) {
        return HI_TRUE;
    } else {
        return HI_FALSE;
    }
}

/*
 * Description  : set the sdio bus state
 * Input        : struct iodevice *io_dev, hi_u8 state: TRUE/FALSE
 * Output       : None
 * Return Value : hi_void
 */
hi_void oal_enable_sdio_state(struct BusDev *bus, hi_u32 mask)
{
    oal_channel_stru *hi_sdio = (oal_channel_stru *)bus->priData.data;
    hi_u32 old_state;
    if (hi_sdio == HI_NULL) {
        printk("oal_enable_sdio_state: hi_sdio null!\n");
        return;
    }

    old_state = hi_sdio->state;
    hi_sdio->state |= mask;
    oal_sdio_print_state(old_state, hi_sdio->state);
}

hi_void oal_disable_sdio_state(struct BusDev *bus, hi_u32 mask)
{
    hi_u32 old_state;
    oal_channel_stru *hi_sdio = (oal_channel_stru *)bus->priData.data;
    if (hi_sdio == HI_NULL) {
        printk("oal_enable_sdio_state: hi_sdio null!\n");
        return;
    }

    old_state = hi_sdio->state;
    hi_sdio->state &= ~mask;
    oal_sdio_print_state(old_state, hi_sdio->state);
}

/*
 * Description  : free sdio dev
 * Input        :
 * Output       : None
 * Return Value : hi_void
 */
static hi_void oal_sdio_free(oal_channel_stru *hi_sdio)
{
    if (hi_sdio == HI_NULL) {
        return;
    }
    OAL_MUTEX_DESTROY(&hi_sdio->rx_transfer_lock);
    oal_free_sdio_stru(hi_sdio);
}

hi_s32 oal_sdio_interrupt_register(struct BusDev *bus)
{
    oal_channel_stru *hi_sdio = (oal_channel_stru *)bus->priData.data;
#ifndef _PRE_FEATURE_NO_GPIO
    hi_s32 ret;

    if (g_hisdio_intr_mode) {
        /* use gpio interrupt for sdio data interrupt */
        ret = oal_register_gpio_intr(bus);
        if (ret < 0) {
            printk("failed to register gpio interrupt\n");
            return ret;
        }
    } else {
        /* use sdio interrupt for sdio data interrupt */
        ret = oal_register_sdio_intr(bus);
        if (ret < 0) {
            printk("failed to register sdio interrupt\n");
            return ret;
        }
    }
#else
    hi_unref_param(hi_sdio);
#endif
    return HI_SUCCESS;
}

/*
 * Description  : initialize sdio interface
 * Input        :
 * Output       : None
 * Return Value : succ or fail
 */
static hi_s32 oal_sdio_init(struct BusDev *bus)
{
    oal_channel_stru *hi_sdio = (oal_channel_stru *)bus->priData.data;
    void *func = hi_sdio->func;
    hi_s32 ret;
    if (func == HI_NULL) {
        printk("oal_sdio_probe func/func->card->host ids null\n");
        return -OAL_EFAIL;
    }

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    g_p_gst_sdio_func = func;
#endif
    /* alloce sdio control struct */
    if (hi_sdio == HI_NULL) {
        g_sdio_enum_err_str = "failed to alloc hi_sdio!";
        printk(KERN_ERR "%s\n", g_sdio_enum_err_str);
        goto failed_sdio_alloc;
    }
    OAL_MUTEX_INIT(&hi_sdio->rx_transfer_lock);
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    /* func keep a pointer to oal_sdio */
    sdio_set_drvdata((struct sdio_func *)func, hi_sdio);
#endif

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    /* register interrupt process function */
    ret = oal_sdio_interrupt_register(bus);
    if (ret < 0) {
        g_sdio_enum_err_str = "failed to register sdio interrupt";
        printk("%s\n", g_sdio_enum_err_str);
        printf("failed to register sdio interrupt\n");
        goto failed_sdio_int_reg;
    }
#endif

    oal_disable_sdio_state(bus, OAL_SDIO_ALL);

    if (oal_sdio_dev_init(bus) != HI_SUCCESS) {
        g_sdio_enum_err_str = "sdio dev init failed";
        goto failed_sdio_dev_init;
    }

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    /* Print the sdio's cap */
    oam_print_info("max_segs:%u, max_blk_size:%u,max_blk_count:%u,,max_seg_size:%u,max_req_size:%u\n",
        sdio_get_max_segs((struct sdio_func *)func), sdio_get_max_blk_size((struct sdio_func*)func),
        sdio_get_max_block_count((struct sdio_func *)func), sdio_get_max_seg_size((struct sdio_func *)func),
        sdio_get_max_req_size((struct sdio_func *)func));
#endif

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    /* register interrupt process function */
    ret = oal_sdio_interrupt_register(bus);
    if (ret < 0) {
        g_sdio_enum_err_str = "failed to register sdio interrupt";
        oam_print_info("%s\n", g_sdio_enum_err_str);
        goto failed_sdio_int_reg;
    }
#endif

    oal_wake_lock_init(&hi_sdio->st_sdio_wakelock, "wlan_sdio_lock");
    oal_sema_init(&g_chan_wake_sema, 1);

    return HI_SUCCESS;
failed_sdio_int_reg:
failed_sdio_dev_init:
    oal_sdio_free(hi_sdio);
failed_sdio_alloc:

    return -OAL_EFAIL;
}

/* ****************************************************************************
 功能描述  : 强行释放wakelock锁
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功或失败原因
**************************************************************************** */
hi_void oal_sdio_wakelocks_release_detect(struct BusDev *bus)
{
    oal_channel_stru *hi_sdio = (oal_channel_stru *)bus->priData.data;
    /* before call this function , please make sure the rx/tx queue is empty and no data transfer!! */
    if (hi_sdio == HI_NULL) {
        printk("oal_sdio_wakelocks_release_detect hi_sdio null\n");
        return;
    }
    if (oal_sdio_wakelock_active(hi_sdio) != 0) {
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#ifdef CONFIG_HAS_WAKELOCK
        printk("[E]We still hold %s   %lu wake locks, Now release all", hi_sdio->st_sdio_wakelock.st_wakelock.ws.name,
            hi_sdio->st_sdio_wakelock.lock_count);
#endif
#endif
        hi_sdio->st_sdio_wakelock.lock_count = 1;
        oal_sdio_wake_unlock(bus);
    }
}

static hi_s32 oal_sdio_single_transfer(struct BusDev *bus, hi_s32 rw, hi_void *buf, hi_u32 size)
{
    if ((bus == HI_NULL) || (bus->priData.data == HI_NULL) || (buf == HI_NULL) || ((uintptr_t)buf & 0x3)) {
        printk("oal_sdio_single_transfer:hi_sdio/hi_sdio->func/buf/(uintptr_t)buf & 0x3 err\n");
        return -OAL_EINVAL;
    }

    return oal_sdio_rw_buf(bus, rw, HISDIO_REG_FUNC1_FIFO, buf, size);
}

hi_s32 oal_sdio_transfer_tx(struct BusDev *bus, oal_netbuf_stru *netbuf)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u32 tailroom, tailroom_add;
    if (netbuf == HI_NULL) {
        printk("oal_sdio_transfer_tx netbuf null\n");
        return -OAL_EFAIL;
    }

    tailroom = HISDIO_ALIGN_4_OR_BLK(oal_netbuf_len(netbuf)) - oal_netbuf_len(netbuf);
    if (tailroom > oal_netbuf_tailroom(netbuf)) {
        tailroom_add = tailroom - oal_netbuf_tailroom(netbuf);
        /* relloc the netbuf */
        ret = oal_netbuf_expand_head(netbuf, 0, tailroom_add, GFP_ATOMIC);
        if (oal_unlikely(ret != HI_SUCCESS)) {
            printk("alloc tail room failed\n");
            return -OAL_EFAIL;
        }
    }

    oal_netbuf_put(netbuf, tailroom);

    return oal_sdio_single_transfer(bus, SDIO_WRITE, oal_netbuf_data(netbuf), oal_netbuf_len(netbuf));
}

hi_void check_sg_format(struct scatterlist *sg, hi_u32 sg_len)
{
    hi_u32 i = 0;
    struct scatterlist *sg_t = HI_NULL;
    for_each_sg(sg, sg_t, sg_len, i) {
        if (oal_unlikely(HI_NULL == sg_t)) {
            return;
        }
        if (OAL_WARN_ON(((uintptr_t)sg_virt(sg_t) & 0x03) || (sg_t->length & 0x03))) {
            printk("check_sg_format:[i:%d][addr:%p][len:%u]\n", i, sg_virt(sg_t), sg_t->length);
        }
    }
}

hi_void dump_sg_format(struct scatterlist *sg, hi_u32 sg_len)
{
    hi_u32 i = 0;
    struct scatterlist *sg_t = HI_NULL;
    printk("sg dump nums:%d\n", sg_len);
    if (sg == HI_NULL) {
        printk("dump_sg_format::sg is null!\n");
        return;
    }
    for_each_sg(sg, sg_t, sg_len, i) {
        if (sg_t == HI_NULL) {
            printk("dump_sg_format::sg_t is null!\n");
            return;
        }
        printk("sg descr:%3d,addr:%p,len:%6d\n", i, sg_virt(sg_t), sg_t->length);
    }
}

#ifdef CONFIG_HISDIO_H2D_SCATT_LIST_ASSEMBLE
hi_s32 oal_sdio_tx_scatt_list_merge(oal_channel_stru *hi_sdio, struct scatterlist *sg, hi_u32 sg_len, hi_u32 rw_sz)
{
    hi_s32 i;
    hi_s32 offset = 0;
    hi_s32 left_size, nents;
    struct scatterlist *sg_t = HI_NULL;
    hi_u8 *pst_scatt_buff = (hi_u8 *)hi_sdio->scatt_buff.buff;
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    hi_u32 seg_size = sdio_get_max_seg_size(hi_sdio->func);
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    hi_u32 seg_size = sdio_get_max_blk_size(hi_sdio->func);
#endif
    if (seg_size == 0) {
        return -OAL_EINVAL;
    }

    if (oal_unlikely(rw_sz > hi_sdio->scatt_buff.len)) {
        printk("[E]sdio tx request %u bytes,scatt buf had %u,failed!\n", rw_sz, hi_sdio->scatt_buff.len);
        OAL_BUG_ON(1);
        return -OAL_ENOMEM;
    }

    if (sg == HI_NULL) {
        printk("oal_sdio_tx_scatt_list_merge::sg is null!\n");
        return -OAL_EINVAL;
    }
    for_each_sg(sg, sg_t, sg_len, i) {
        if (sg_t == HI_NULL) {
            printk("oal_sdio_tx_scatt_list_merge::sg_t is null!\n");
            return -OAL_EINVAL;
        }
        memcpy_s(pst_scatt_buff + offset, sg_t->length, sg_virt(sg_t), sg_t->length);
        offset += sg_t->length;
    }

    if (oal_unlikely(offset > rw_sz)) {
        printk("[E]%s offset:%u > rw_sz:%u!\n", __FUNCTION__, offset, rw_sz);
        OAL_BUG_ON(1);
        return -OAL_EINVAL;
    }

    left_size = offset;
    /* reset the sg list! */
    nents = ((left_size - 1) / seg_size) + 1;
    if (oal_unlikely(nents > (hi_s32)sg_len)) {
        printk("[E]%s merged scatt list num %d > sg_len:%u,max seg size:%u\n", __FUNCTION__, nents, sg_len, seg_size);
        OAL_BUG_ON(1);
        return -OAL_ENOMEM;
    }

    sg_init_table(sg, nents);
    for_each_sg(sg, sg_t, nents, i) {
        if (HI_NULL == sg_t) {
            printk("oal_sdio_tx_scatt_list_merge::sg_t is null!\n");
            return -OAL_EINVAL;
        }
        sg_set_buf(sg_t, pst_scatt_buff + i * seg_size, oal_min(seg_size, left_size));
        left_size = left_size - seg_size;
    }

    return nents;
}
#endif

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
hi_s32 oal_mmc_io_rw_scat_extended(const oal_channel_stru *hi_sdio, hi_s32 write, hi_u32 fn, hi_u32 addr,
    hi_s32 incr_addr, struct scatterlist *sg, hi_u32 sg_len, hi_u32 blocks, hi_u32 blksz)
{
    struct mmc_request mrq = { 0 };
    struct mmc_command cmd = { 0 };
    struct mmc_data data = { 0 };
    struct mmc_card *card = HI_NULL;
    struct sdio_func *func = HI_NULL;

    OAL_BUG_ON(!hi_sdio);
    OAL_BUG_ON(!sg);
    OAL_BUG_ON(sg_len == 0);
    OAL_BUG_ON(fn > 7); /* fn must bigger than 7 */

    if (OAL_WARN_ON(blksz == 0)) {
        return -EINVAL;
    }

    /* sanity check */
    if (oal_unlikely(addr & ~0x1FFFF)) {
        return -EINVAL;
    }
    func = hi_sdio->func;
    card = func->card;

    /* sg format */
    check_sg_format(sg, sg_len);

#ifdef CONFIG_HISDIO_H2D_SCATT_LIST_ASSEMBLE
#ifndef LITEOS_IPC_CODE
    if (write) {
        /* copy the buffs ,align to SDIO_BLOCK
        Fix the sdio host ip fifo depth issue temporarily */
        hi_s32 ret = oal_sdio_tx_scatt_list_merge(hi_sdio, sg, sg_len, blocks * blksz);
        if (oal_likely(ret > 0)) {
            sg_len = ret;
        } else {
            return ret;
        }
    }
#endif
#endif

    mrq.cmd = &cmd;
    mrq.data = &data;

    cmd.opcode = SD_IO_RW_EXTENDED;
    cmd.arg = write ? 0x80000000 : 0x00000000;
    cmd.arg |= fn << 28; /* left shift 28 */
    cmd.arg |= incr_addr ? 0x04000000 : 0x00000000;
    cmd.arg |= addr << 9;                      /* left shift 9 */
    if (blocks == 1 && blksz <= 512) {         /* blksz range [1 512] */
        cmd.arg |= (blksz == 512) ? 0 : blksz; /* blksz equal 512, use byte mode */
    } else {
        cmd.arg |= 0x08000000 | blocks; /* block mode */
    }
    cmd.flags = MMC_RSP_SPI_R5 | MMC_RSP_R5 | MMC_CMD_ADTC;

    data.blksz = blksz;
    data.blocks = blocks;
    data.flags = write ? MMC_DATA_WRITE : MMC_DATA_READ;
    data.sg = sg;
    data.sg_len = sg_len;
#ifdef CONFIG_SDIO_DEBUG
    printk("[blksz:%u][blocks:%u][sg_len:%u][mode:%s]\n", blksz, blocks, sg_len, write ? "write" : "read");
    printk("%s : [cmd opcode:%d][cmd arg:0x%8x][cmd flags: 0x%8x]\n", mmc_hostname(card->host), cmd.opcode, cmd.arg,
        cmd.flags);
    printk("Sdio %s data transfer start\n", write ? "write" : "read");
#endif

    mmc_set_data_timeout(&data, card);
    mmc_wait_for_req(card->host, &mrq);

#ifdef CONFIG_SDIO_DEBUG
    printk("wait for %s transfer over.\n", write ? "write" : "read");
#endif
    if (cmd.error) {
        return cmd.error;
    }
    if (data.error) {
        return data.error;
    }

    if (OAL_WARN_ON(mmc_host_is_spi(card->host))) {
        printk("HiSi WiFi  driver do not support spi sg transfer!\n");
        return -EIO;
    }
    if (cmd.resp[0] & R5_ERROR) {
        return -EIO;
    }
    if (cmd.resp[0] & R5_FUNCTION_NUMBER) {
        return -EINVAL;
    }
    if (cmd.resp[0] & R5_OUT_OF_RANGE) {
        return -ERANGE;
    }
#ifdef CONFIG_SDIO_DEBUG
    do {
        int i;
        struct scatterlist *sg_t;
        for_each_sg(data.sg, sg_t, data.sg_len, i) {
            printk(KERN_DEBUG "======netbuf pkts %d, len:%d=========\n", i, sg_t->length);
            oal_print_hex_dump(sg_virt(sg_t), sg_t->length, 32, "sg buf  :"); /* 32 进制 */
        }
    } while (0);
    printk("Transfer done. %s sucuess!\n", write ? "write" : "read");
#endif
    return 0;
}
#endif

static hi_s32 _oal_sdio_transfer_scatt(struct BusDev *bus, hi_s32 rw, hi_u32 addr, struct scatterlist *sg,
    hi_u32 sg_len, hi_u32 rw_sz)
{
    oal_channel_stru *hi_sdio = (oal_channel_stru *)bus->priData.data;
    struct BusConfig busCfg = {0};
#ifdef CONFIG_HISI_SDIO_TIME_DEBUG
    oal_time_t_stru time_start;
    time_start = oal_ktime_get();
#endif
    hi_s32 ret;
    hi_s32 write = (rw == SDIO_READ) ? 0 : 1;
    bus->ops.claimHost(bus);

    if (oal_unlikely(oal_sdio_get_state(hi_sdio, OAL_SDIO_ALL) != HI_TRUE)) {
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
        if (printk_ratelimit())
#endif
            printk("[W][%s]sdio closed,state:%u\n", __FUNCTION__, oal_sdio_get_state(hi_sdio, OAL_SDIO_ALL));
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
        schedule();
#endif
        bus->ops.releaseHost(bus);
        return -OAL_EFAIL;
    }
    busCfg.busType = BUS_SDIO;
    ret = bus->ops.getBusInfo(bus, &busCfg);
    if (ret != 0) {
        printk("get Bus info failed!\n");
        return -OAL_EINVAL;
    }
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    ret = oal_mmc_io_rw_scat_extended(hi_sdio, write, busCfg.busInfo.sdioInfo.funcNumSize, addr, 0, sg, sg_len,
        (rw_sz / HISDIO_BLOCK_SIZE) ? : 1, min(rw_sz, (hi_u32)HISDIO_BLOCK_SIZE));
#else
    if (rw == SDIO_READ) {
        ret = bus->ops.bulkRead(bus, addr, rw_sz, (uint8_t *)sg, sg_len);
    } else {
        ret = bus->ops.bulkWrite(bus, addr, rw_sz, (uint8_t *)sg, sg_len);
    }
#endif
    if (oal_unlikely(ret)) {
#ifdef CONFIG_HISI_SDIO_TIME_DEBUG
        hi_u64 trans_us;
        oal_time_t_stru time_stop = oal_ktime_get();
        trans_us = (hi_u64)oal_ktime_to_us(oal_ktime_sub(time_stop, time_start));
        printk("[W][%s]trans_us:%llu\n", __FUNCTION__, trans_us);
#endif
        if (write) {
            oam_error_log1(0, OAM_SF_ANY, "{oal_sdio_transfer_scatt::write failed=%d}", ret);
        } else {
            oam_error_log1(0, OAM_SF_ANY, "{oal_sdio_transfer_scatt::read failed=%d}", ret);
        }
        oal_exception_submit(TRANS_FAIL);
    }
    bus->ops.releaseHost(bus);
    if (rw == SDIO_READ) {
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
        schedule();
#endif
    }
    return ret;
}

/*
 * Description  : sdio scatter data transfer
 * Input        :
 * Output       : None
 * Return Value : err or succ
 */
hi_s32 oal_sdio_transfer_scatt(struct BusDev *bus, hi_s32 rw, hi_u32 addr, struct scatterlist *sg,
    hi_u32 sg_len, hi_u32 sg_max_len, hi_u32 rw_sz)
{
    hi_s32 ret;
    hi_u32 align_len;
    hi_u32 align_t;
    oal_channel_stru *hi_sdio = (oal_channel_stru *)bus->priData.data;

    if ((hi_sdio == HI_NULL) || (rw_sz == 0) || (sg_max_len < sg_len)) {
        return -OAL_EINVAL;
    }

#ifdef CONFIG_SDIO_DEBUG
    hi_s32 write = (rw == SDIO_READ) ? 0 : 1;
#endif

    if ((!hi_sdio) || (!rw_sz) || (sg_max_len < sg_len) || (sg == HI_NULL)) {
        oam_error_log3(0, OAM_SF_ANY, "oal_sdio_transfer_scatt: hi_sdio:%p,/rw_sz:%d,/sg_max_len<sg_len?:%d,/sg null}",
            (uintptr_t)hi_sdio, rw_sz, sg_max_len < sg_len);
        return -OAL_EINVAL;
    }

    if (OAL_WARN_ON(!sg_len)) {
        oam_error_log2(0, OAM_SF_ANY,
            "Sdio %d(1:read,2:write) Scatter list num should never be zero, total request len: %u}",
            rw == SDIO_READ ? 1 : 2, rw_sz); /* read:1 write:2  */
        return -OAL_EINVAL;
    }

    align_t = HISDIO_ALIGN_4_OR_BLK(rw_sz);
    align_len = align_t - rw_sz;

    if (oal_likely(align_len)) {
        if (oal_unlikely(sg_len + 1 > sg_max_len)) {
            oam_error_log2(0, OAM_SF_ANY, "{sg list over,sg_len:%u, sg_max_len:%u\n}", sg_len, sg_max_len);
            return -OAL_ENOMEM;
        }
        sg_set_buf(&sg[sg_len], (const void *)(UINTPTR)hi_sdio->sdio_align_buff, align_len);
        sg_len++;
    }
    sg_mark_end(&sg[sg_len - 1]);

#ifdef CONFIG_SDIO_DEBUG
    oam_warning_log4(0, OAM_SF_ANY, "{sdio %s request %u bytes transfer, scatter list num %u, used %u bytes to align}",
        write ? "write" : "read", rw_sz, sg_len, align_len);
#endif

    rw_sz = align_t;

    /* sdio scatter list driver ,when letter than 512 bytes bytes mode, other blockmode */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    OAL_WARN_ON((rw_sz >= HISDIO_BLOCK_SIZE) && (rw_sz & (HISDIO_BLOCK_SIZE - 1)));
    OAL_WARN_ON((rw_sz < HISDIO_BLOCK_SIZE) && (rw_sz & (4 - 1))); /* 4 */
#endif

    if (OAL_WARN_ON(align_len & 0x3)) {
        oam_warning_log1(0, OAM_SF_ANY, "{not 4 bytes align:%u\n}", align_len);
    }

    ret = _oal_sdio_transfer_scatt(bus, rw, addr, sg, sg_len, rw_sz);

    return ret;
}

hi_s32 oal_sdio_transfer_netbuf_list(struct BusDev *bus, const oal_netbuf_head_stru *head, hi_s32 rw)
{
    hi_u8 sg_realloc = 0;
    hi_s32 ret;
    hi_u32 idx = 0;
    hi_u32 queue_len;
    hi_u32 sum_len = 0;
    hi_u32 request_sg_len;
    oal_netbuf_stru *netbuf = HI_NULL;
    oal_netbuf_stru *tmp = HI_NULL;
    struct scatterlist *sg = HI_NULL;
    struct sg_table sgtable = { 0 };
    oal_channel_stru *hi_sdio = (oal_channel_stru *)bus->priData.data;
    if ((!hi_sdio) || (!head)) {
        printk("hi_sdio / head null\n");
        return -OAL_EINVAL;
    }

    if (OAL_WARN_ON(rw >= SDIO_OPT_BUTT)) {
        printk("invalid rw:%d\n", rw);
        return -OAL_EINVAL;
    }

    if (OAL_WARN_ON(oal_netbuf_list_empty(head))) {
        return -OAL_EINVAL;
    }
    if (rw == SDIO_WRITE) {
        if (hi_sdio->pst_pm_callback) {
            if (hi_sdio->pst_pm_callback->wlan_pm_wakeup_dev() != HI_SUCCESS) {
                oam_error_log0(0, OAM_SF_ANY, "{oal_sdio_transfer_netbuf_list::host wakeup device failed}");
                return -OAL_EBUSY;
            }
        }
    }
    queue_len = oal_netbuf_list_len(head);
    /* must realloc the sg list mem, alloc more sg for the align buff */
    request_sg_len = queue_len + 1;
    if (oal_unlikely(request_sg_len > hi_sdio->scatt_info[rw].max_scatt_num)) {
        oam_warning_log2(0, OAM_SF_ANY, "transfer_netbuf_list realloc sg!, request:%d,max scatt num:%d",
                         request_sg_len, hi_sdio->scatt_info[rw].max_scatt_num);
        /* must realloc the sg list mem, alloc more sgs for the align buff */
        if (sg_alloc_table(&sgtable, request_sg_len, GFP_KERNEL) != 0) {
            oam_error_log0(0, OAM_SF_ANY, "{transfer_netbuf_list alloc sg failed!}");
            return -OAL_ENOMEM;
        }
        sg_realloc = 1;
        sg = sgtable.sgl;
    } else {
        sg = hi_sdio->scatt_info[rw].sglist;
    }

    memset_s(sg, sizeof(struct scatterlist) * request_sg_len, 0, sizeof(struct scatterlist) * request_sg_len);

    oal_skb_queue_walk_safe(head, netbuf, tmp) {
        /* assert, should drop the scatt transfer */
        if (!oal_is_aligned((uintptr_t)oal_netbuf_data(netbuf), 4)) { /* 4 字节对齐 */
            oam_error_log0(0, OAM_SF_ANY, "{oal_sdio_transfer_netbuf_list netbuf 4 aligned fail!}");
            return -OAL_EINVAL;
        }
        if (OAL_WARN_ON(!oal_is_aligned(oal_netbuf_len(netbuf), HISDIO_H2D_SCATT_BUFFLEN_ALIGN))) {
            /* This should never happned, debug */
            oal_netbuf_hex_dump(netbuf);
        }
        if (!oal_is_aligned(oal_netbuf_len(netbuf), HISDIO_H2D_SCATT_BUFFLEN_ALIGN)) {
            oam_error_log0(0, OAM_SF_ANY, "{oal_sdio_transfer_netbuf_list netbuf 8 aligned fail!}");
            return -OAL_EINVAL;
        }
        sg_set_buf(&sg[idx], oal_netbuf_data(netbuf), oal_netbuf_len(netbuf));
        sum_len += oal_netbuf_len(netbuf);
        idx++;
    }

    if (oal_unlikely(idx > queue_len)) {
        printk("idx:%d, queue_len:%d\n", idx, queue_len);
        return -OAL_EINVAL;
    }
    wlan_pm_set_packet_cnt(1);
    ret = oal_sdio_transfer_scatt(bus, rw, HISDIO_REG_FUNC1_FIFO, sg, idx, request_sg_len, sum_len);
    if (sg_realloc) {
        sg_free_table(&sgtable);
    }
    return ret;
}

/*
 * Description  :
 * Input        : struct device *dev
 * Output       : None
 * Return Value : static hi_s32
 */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
static hi_s32 oal_sdio_suspend(struct device *dev)
{
    struct sdio_func *func = HI_NULL;
    oal_channel_stru *hi_sdio = HI_NULL;

    printk(KERN_ERR "+++++++sdio suspend+++++++++++++\n");
    if (dev == HI_NULL) {
        printk("[WARN]dev is null\n");
        return HI_SUCCESS;
    }
    func = dev_to_sdio_func(dev);
    hi_sdio = sdio_get_drvdata(func);
    if (hi_sdio == HI_NULL) {
        printk("hi_sdio is null\n");
        return HI_SUCCESS;
    }

    if (oal_down_interruptible(&g_chan_wake_sema)) {
        printk(KERN_ERR "g_chan_wake_sema down failed.");
        return -OAL_EFAIL;
    }

    if (oal_sdio_wakelock_active(hi_sdio) != 0) {
        /* has wake lock so stop controller's suspend,
         * otherwise controller maybe error while sdio reinit */
        printk(KERN_ERR "Already wake up");
        oal_up(&g_chan_wake_sema);
        return -OAL_EFAIL;
    }
    hi_sdio->ul_sdio_suspend++;
    return HI_SUCCESS;
}
#endif

/*
 * Description  : sdio resume
 * Input        : struct device *dev
 * Output       : None
 * Return Value : static hi_s32
 */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
static hi_s32 oal_sdio_resume(struct device *dev)
{
    struct sdio_func *func = HI_NULL;
    oal_channel_stru *hi_sdio = HI_NULL;

    printk("+++++++sdio resume+++++++++++++\n");
    if (dev == HI_NULL) {
        printk("[WARN]dev is null\n");
        return HI_SUCCESS;
    }
    func = dev_to_sdio_func(dev);
    hi_sdio = sdio_get_drvdata(func);
    if (hi_sdio == HI_NULL) {
        printk("hi_sdio is null\n");
        return HI_SUCCESS;
    }
    oal_up(&g_chan_wake_sema);

    hi_sdio->ul_sdio_resume++;

    return HI_SUCCESS;
}
#endif

MODULE_DEVICE_TABLE(sdio, g_oal_sdio_ids);

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
static const struct dev_pm_ops oal_sdio_pm_ops = {
    .suspend = oal_sdio_suspend,
    .resume = oal_sdio_resume,
};
#endif

hi_void oal_sdio_dev_shutdown(struct device *dev)
{
    struct BusDev *bus = oal_get_bus_default_handler();
    oal_channel_stru *hi_sdio = (oal_channel_stru *)bus->priData.data;

    hi_unref_param(dev);
    /* poweroff */
    if ((hi_sdio == HI_NULL) || (hi_sdio->func == NULL)) {
        goto exit;
    }

    hi_wifi_plat_pm_disable();
    oal_sdio_sleep_dev(oal_get_sdio_default_handler());

    if (oal_sdio_send_msg(oal_get_sdio_default_handler(), H2D_MSG_PM_WLAN_OFF) != HI_SUCCESS) {
        goto exit;
    }

    if (HI_TRUE != oal_sdio_get_state(hi_sdio, OAL_SDIO_ALL)) {
        goto exit;
    }
    if (g_hisdio_intr_mode) {
        oal_wlan_gpio_intr_enable(bus, 0);
    } else {
        hi_s32   ret;
        oal_sdio_claim_host(bus);
        ret = bus->ops.disableBus(bus);
        oal_sdio_release_host(bus);
        if (ret) {
            goto exit;
        }
    }
exit:
    return;
}

hi_void sdio_card_detect_change(hi_s32 val)
{
    hi_unref_param(val);
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    hisi_sdio_rescan(HdfWifiGetBusIdx());
#else
    hi_s32 ret = hisi_sdio_rescan(HdfWifiGetBusIdx());
    if (ret != HI_ERR_SUCCESS) {
        oam_error_log0(0, 0, "sdio rescan failed.");
    }
#endif
}

hi_void hi_wlan_power_set(hi_s32 on)
{
    /*
     * this should be done in mpw1
     * it depends on the gpio used to power up and down 1101 chip
     *
     *  */
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    if (on) {
        printk("sdio probe:pull up power on gpio\n");
        board_power_on();
    } else {
        printk("sdio probe:pull down power on gpio\n");
        board_power_off();
    }
#else
    hi_unref_param(on);
#endif
}

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
hi_s32 oal_sdio_func_probe_resume(struct BusDev *bus)
{
    hi_s32 ret;

    oam_info_log0(0, OAM_SF_ANY, "{start to register sdio module!}");
    ret = oal_sdio_init(bus);
    if (ret) {
        printk("fail to init sdio\n");
        goto failed_sdio_enum;
    }
    return HI_SUCCESS;
failed_sdio_enum:
    /* sdio can not remove!
      hi_sdio_detectcard_to_core(0); */
    return -OAL_EFAIL;
}
#endif

hi_s32 oal_sdio_func_init(struct BusDev *bus)
{
    hi_s32 ret;
    hi_s32 i;
    void *func = NULL;
    oal_channel_stru *hi_sdio = (oal_channel_stru *)bus->priData.data;
    struct BusConfig busCfg = {0};

    if (hi_sdio == HI_NULL) {
        return -OAL_EFAIL;
    }

    struct HdfConfigWlanBus *busConfig = NULL;
    struct HdfConfigWlanDeviceList *devList = NULL;
    struct HdfConfigWlanRoot *rootConfig = HdfWlanGetModuleConfigRoot();
    if (rootConfig == NULL) {
        oam_print_err("%s:NULL ptr!", __func__);
        return -OAL_EFAIL;
    }
    devList = &rootConfig->wlanConfig.deviceList;
    if (devList == NULL) {
        oam_print_err("%s:No device defined.", __func__);
        return -OAL_EFAIL;
    }
    for (i = 0; i < devList->deviceListSize; i++) {
        busConfig = &rootConfig->wlanConfig.deviceList.deviceInst[i].bus;
        ret = bus->ops.init(bus, busConfig);
        if (ret == 0) {
            break;
        }
    }
    if (i == devList->deviceListSize) {
        oam_print_err("%s:sdio init failed", __func__);
        return -OAL_EFAIL;
    }
    busCfg.busType = BUS_SDIO;
    if (bus->ops.getBusInfo(bus, &busCfg) != 0) {
        oam_print_err("get bus info failed!\n");
        return -OAL_EFAIL;
    }
    func = busCfg.busInfo.sdioInfo.data;
    if (func == NULL) {
        oam_print_err("func is NULL!\n");
        return -OAL_EFAIL;
    }
    hi_sdio->func = func;

    /* notify mmc core to detect sdio device */
    ret = oal_sdio_init(bus);
    if (ret) {
        oam_print_err("fail to detect sdio card, ret=%d\n", ret);
        goto failed_sdio_reg;
    }
    oal_sdio_claim_host(bus);
    oal_disable_sdio_state(bus, OAL_SDIO_ALL);

    oal_sdio_release_host(bus);

    return HI_SUCCESS;
failed_sdio_reg:
    return -OAL_EFAIL;
}

hi_s32 oal_sdio_func_reset(void)
{
    hi_s32 ret;
    hi_s32 l_need_retry = 1;
    struct BusDev *bus = oal_get_bus_default_handler();
    OAL_INIT_COMPLETION(&g_sdio_driver_complete);

    oal_sdio_func_remove(bus);
#ifndef _PRE_FEATURE_NO_GPIO
    wlan_rst();
#endif
detect_retry:
    /* notify mmc core to detect sdio device */
    ret = oal_sdio_init(bus);
    if (ret) {
        oam_print_err("fail to detect sdio card, ret=%d\n", ret);
        goto failed_sdio_enum;
    }
    if (oal_wait_for_completion_timeout(&g_sdio_driver_complete, TIMEOUT_MUTIPLE_5 * HZ) != 0) {
        printk("hisi sdio load sucuess, sdio enum done.\n");
    } else {
        printk("sdio enum timeout, reason[%s]\n", g_sdio_enum_err_str);

        if (l_need_retry) {
            printk("sdio enum retry.\n");
            l_need_retry = 0;
            hi_wlan_power_set(1);
            goto detect_retry;
        }
        goto failed_sdio_enum;
    }
    return HI_SUCCESS;

failed_sdio_enum:
    hi_wlan_power_set(0);
    return -OAL_EFAIL;
}

hi_void oal_sdio_func_remove(struct BusDev *bus)
{
    oal_channel_stru *hi_sdio = (oal_channel_stru *)bus->priData.data;
    hi_unref_param(hi_sdio);
    oal_wake_lock_exit(&hi_sdio->st_sdio_wakelock);
    oal_sdio_dev_deinit(bus);
    oal_sdio_free(hi_sdio);
    oal_sema_destroy(&g_chan_wake_sema);

    printk("hisilicon connectivity sdio driver has been removed.");
}

static hi_void oal_sdio_credit_info_init(oal_channel_stru *hi_sdio)
{
    hi_sdio->sdio_credit_info.large_free_cnt = 0;
    hi_sdio->sdio_credit_info.short_free_cnt = 0;
    oal_spin_lock_init(&hi_sdio->sdio_credit_info.credit_lock);
}

oal_channel_stru *oal_sdio_init_module(struct BusDev *bus, hi_void *data)
{
#ifdef CONFIG_HISDIO_H2D_SCATT_LIST_ASSEMBLE
    hi_u32 tx_scatt_buff_len = 0;
#endif
    hi_u32 ul_rx_seg_size;
    hi_void *p_sdio = NULL;
    oal_channel_stru *hi_sdio;

    oam_info_log0(0, OAM_SF_ANY, "{hii110x sdio driver installing...!}");
    hi_sdio = (oal_channel_stru *)oal_memalloc(sizeof(oal_channel_stru));
    if (hi_sdio == HI_NULL) {
        printk("[E]alloc oal_sdio failed [%d]\n", (hi_s32)sizeof(oal_channel_stru));
        return HI_NULL;
    }
    p_sdio = hi_sdio;
    memset_s(p_sdio, sizeof(oal_channel_stru), 0, sizeof(oal_channel_stru));

#ifdef CONFIG_SDIO_FUNC_EXTEND
    g_sdio_extend_func = 1;
#else
    g_sdio_extend_func = 0;
#endif

    ul_rx_seg_size = ALIGN((HSDIO_HOST2DEV_PKTS_MAX_LEN), HISDIO_BLOCK_SIZE);
    /* alloc rx reserved mem */
    hi_sdio->rx_reserved_buff = (hi_void *)oal_memalloc(ul_rx_seg_size);
    if (hi_sdio->rx_reserved_buff == HI_NULL) {
        printk("[E]alloc rx_reserved_buff failed [%u]\n", ul_rx_seg_size);
        goto failed_rx_reserved_buff_alloc;
    }
    hi_sdio->rx_reserved_buff_len = ul_rx_seg_size;
    oam_info_log1(0, OAM_SF_ANY, "{alloc %u bytes rx_reserved_buff!}", ul_rx_seg_size);

    hi_sdio->func1_int_mask = HISDIO_FUNC1_INT_MASK;

    oal_sdio_credit_info_init(hi_sdio);

    hi_sdio->sdio_extend = (hisdio_extend_func *)oal_memalloc(sizeof(hisdio_extend_func));
    if (hi_sdio->sdio_extend == HI_NULL) {
        printk("[E]alloc sdio_extend failed [%d]\n", (hi_s32)sizeof(hisdio_extend_func));
        goto failed_sdio_extend_alloc;
    }
    memset_s(hi_sdio->sdio_extend, sizeof(hisdio_extend_func), 0, sizeof(hisdio_extend_func));
    hi_sdio->bus_data = data;
    hi_sdio->scatt_info[SDIO_READ].max_scatt_num = HISDIO_DEV2HOST_SCATT_MAX + 1;

    hi_sdio->scatt_info[SDIO_READ].sglist =
        oal_kzalloc(sizeof(struct scatterlist) * (HISDIO_DEV2HOST_SCATT_MAX + 1), OAL_GFP_KERNEL);
    if (hi_sdio->scatt_info[SDIO_READ].sglist == HI_NULL) {
        goto failed_sdio_read_sg_alloc;
    }

    /* 1 for algin buff, 1 for scatt info buff */
    hi_sdio->scatt_info[SDIO_WRITE].max_scatt_num = HISDIO_HOST2DEV_SCATT_MAX + 2; /* add 2 */
    hi_sdio->scatt_info[SDIO_WRITE].sglist =
        oal_kzalloc(sizeof(struct scatterlist) * (hi_sdio->scatt_info[SDIO_WRITE].max_scatt_num), OAL_GFP_KERNEL);
    if (hi_sdio->scatt_info[SDIO_WRITE].sglist == HI_NULL) {
        goto failed_sdio_write_sg_alloc;
    }

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    hi_sdio->sdio_align_buff = memalign(CACHE_ALIGNED_SIZE, SKB_DATA_ALIGN(HISDIO_BLOCK_SIZE));
#else
    hi_sdio->sdio_align_buff = oal_kzalloc(HISDIO_BLOCK_SIZE, OAL_GFP_KERNEL);
#endif
    if (hi_sdio->sdio_align_buff == HI_NULL) {
        goto failed_sdio_align_buff_alloc;
    }
#ifdef CONFIG_HISDIO_H2D_SCATT_LIST_ASSEMBLE
    tx_scatt_buff_len = HISDIO_HOST2DEV_SCATT_SIZE + HISDIO_HOST2DEV_SCATT_MAX *
        (HCC_HDR_TOTAL_LEN + oal_round_up(HSDIO_HOST2DEV_PKTS_MAX_LEN, HISDIO_H2D_SCATT_BUFFLEN_ALIGN));
    tx_scatt_buff_len = HISDIO_ALIGN_4_OR_BLK(tx_scatt_buff_len);

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    tx_scatt_buff_len = SKB_DATA_ALIGN(tx_scatt_buff_len);
    hi_sdio->scatt_buff.buff = memalign(CACHE_ALIGNED_SIZE, tx_scatt_buff_len);
#else
    hi_sdio->scatt_buff.buff = oal_memalloc(tx_scatt_buff_len);
#endif

    if (hi_sdio->scatt_buff.buff == HI_NULL) {
        printk("alloc scatt_buff failed,request %u bytes\n", tx_scatt_buff_len);
        goto failed_sdio_scatt_buff_alloc;
    }
    hi_sdio->scatt_buff.len = tx_scatt_buff_len;

    printk("alloc scatt_buff ok,request %u bytes\n", tx_scatt_buff_len);
#endif
    bus->priData.data = (void *)hi_sdio;
    hi_s32 ret = oal_sdio_message_register(bus, D2H_MSG_DEVICE_PANIC, oal_device_panic_callback, HI_NULL);
    if (ret != HI_SUCCESS) {
        printk("oal_sdio_message_register fail\n");
    }
    g_bus = bus;
    return hi_sdio;

#ifdef CONFIG_HISDIO_H2D_SCATT_LIST_ASSEMBLE
failed_sdio_scatt_buff_alloc:
    oal_free(hi_sdio->sdio_align_buff);
#endif
failed_sdio_align_buff_alloc:
    oal_free(hi_sdio->scatt_info[SDIO_WRITE].sglist);
failed_sdio_write_sg_alloc:
    oal_free(hi_sdio->scatt_info[SDIO_READ].sglist);
failed_sdio_read_sg_alloc:
    oal_free(hi_sdio->sdio_extend);
failed_sdio_extend_alloc:
    oal_free(hi_sdio->rx_reserved_buff);
failed_rx_reserved_buff_alloc:
    oal_free(hi_sdio);
    return HI_NULL;
}
EXPORT_SYMBOL(oal_sdio_init_module);

hi_s32 oal_sdio_exit_module(void *data)
{
    printk("sdio module unregistered\n");
    oal_channel_stru *hi_sdio = (oal_channel_stru *)data;
#ifdef CONFIG_HISDIO_H2D_SCATT_LIST_ASSEMBLE
    oal_free(hi_sdio->scatt_buff.buff);
#endif
    oal_free(hi_sdio->sdio_align_buff);
    oal_free(hi_sdio->scatt_info[SDIO_WRITE].sglist);
    oal_free(hi_sdio->scatt_info[SDIO_READ].sglist);
    oal_free(hi_sdio->sdio_extend);
    oal_free(hi_sdio->rx_reserved_buff);
    oal_free(hi_sdio);
    hi_sdio = NULL;
    g_bus = NULL;
    return HI_SUCCESS;
}
EXPORT_SYMBOL(oal_sdio_exit_module);

hi_u32 oal_sdio_func_max_req_size(struct BusDev *bus)
{
    hi_u32 max_blocks;
    hi_u32 size, size_device;
    hi_u32 size_host;
    hi_s32 ret;
    oal_channel_stru *hi_sdio = (oal_channel_stru *)bus->priData.data;
    struct BusConfig busCfg = {0};

    /* host transer limit */
    /* Blocks per command is limited by host count, host transfer
     * size and the maximum for IO_RW_EXTENDED of 511 blocks. */
    if (hi_sdio == HI_NULL) {
        printk("oal_sdio_func_max_req_size:hi_sdio is null!\n");
        return -OAL_EFAIL;
    }
    busCfg.busType = BUS_SDIO;
    ret = bus->ops.getBusInfo(bus, &busCfg);
    if (ret != 0) {
        printk("get bus info failed!\n");
        return -OAL_EFAIL;
    }
    max_blocks = oal_min(busCfg.busInfo.sdioInfo.maxBlockNum, 511u);
    size = max_blocks * HISDIO_BLOCK_SIZE;
    size = oal_min(size, busCfg.busInfo.sdioInfo.maxRequestSize);
    /* device transer limit,per adma descr limit 32K in bootloader,
    and total we have 20 descs */
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    size_device = (32 * 1024); /* size device 32*1024 */
#else
    size_device = (32 * 1024) * 20; /* size device 32*1024*20 */
#endif

#ifdef HOST_SDIO_MAX_TRANSFER_SIZE
    size_host = HOST_SDIO_MAX_TRANSFER_SIZE;
#else
    size_host = 0xffffffff;
#endif
    size = oal_min(size, size_device);
    size = oal_min(size, size_host);
    return size;
}

hi_s32 oal_sdio_transfer_prepare(struct BusDev *bus)
{
    oal_enable_sdio_state(bus, OAL_SDIO_ALL);
#ifdef _PRE_FEATURE_NO_GPIO
    if (oal_register_sdio_intr(bus) < 0) {
        printk("failed to register sdio interrupt\n");
        return -OAL_EFAIL;
    }
#else
    oal_wlan_gpio_intr_enable(bus, HI_TRUE);
#endif
    return HI_SUCCESS;
}
#endif /* #ifdef CONFIG_MMC */

hi_void oal_netbuf_list_hex_dump(const oal_netbuf_head_stru *head)
{
#ifdef CONFIG_PRINTK
    hi_s32 index = 0;
    oal_netbuf_stru *netbuf = HI_NULL;
    oal_netbuf_stru *tmp = HI_NULL;
    if (!oal_netbuf_queue_num(head)) {
        return;
    }
    printk(KERN_DEBUG "prepare to dump %d pkts=========\n", oal_netbuf_queue_num(head));
    oal_skb_queue_walk_safe(head, netbuf, tmp) {
        index++;
        printk(KERN_DEBUG "======netbuf pkts %d, len:%d=========\n", index, oal_netbuf_len(netbuf));
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
        print_hex_dump_bytes("netbuf  :", DUMP_PREFIX_ADDRESS, oal_netbuf_data(netbuf), oal_netbuf_len(netbuf));
#endif
    }
#else
    OAL_REFERENCE(head);
#endif
}

hi_void oal_netbuf_hex_dump(const oal_netbuf_stru *netbuf)
{
#ifdef CONFIG_PRINTK
    printk(KERN_DEBUG "==prepare to netbuf,%p,len:%d=========\n", oal_netbuf_data(netbuf), oal_netbuf_len(netbuf));
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    print_hex_dump_bytes("netbuf  :", DUMP_PREFIX_ADDRESS, oal_netbuf_data(netbuf), oal_netbuf_len(netbuf));
#endif
#else
    OAL_REFERENCE(netbuf);
#endif
}

hi_u32 oal_sdio_get_large_pkt_free_cnt(oal_channel_stru *hi_sdio)
{
    hi_u32 free_cnt;

    if (hi_sdio == HI_NULL) {
        return -OAL_EFAIL;
    }
    oal_spin_lock(&hi_sdio->sdio_credit_info.credit_lock);
    free_cnt = (hi_u32)hi_sdio->sdio_credit_info.large_free_cnt;
    oal_spin_unlock(&hi_sdio->sdio_credit_info.credit_lock);
    return free_cnt;
}

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
hi_s32 sdio_dev_init(struct BusDev *bus)
{
    hi_s32 ret;
    hi_u32 data;
    oal_channel_stru *pstSdio = (oal_channel_stru *)bus->priData.data;
    if (pstSdio == HI_NULL || pstSdio->func == HI_NULL) {
        printk("sdio handler is NULL or func is NULL!\n");
        return -OAL_EFAIL;
    }
    bus->ops.claimHost(bus);

    /* before enable sdio function 1, clear its interrupt flag, no matter it exist or not */
    /* if clear the interrupt, host can't receive dev wake up ok ack. debug by 1131c */
    /*
     * enable four interrupt sources in function 1:
     * data ready for host to read
     * read data error
     * message from arm is available
     * device has receive message from host
     *  */
    data = HISDIO_FUNC1_INT_MASK;
    ret = bus->ops.writeData(bus, HISDIO_REG_FUNC1_INT_ENABLE, ONE_BYTE, (hi_u8 *)&data);
    if (ret < 0) {
        oam_info_log1(0, OAM_SF_ANY, "{failed to enable sdio interrupt! ret=%d}", ret);
    }
    oal_enable_sdio_state(bus, OAL_SDIO_ALL);
    bus->ops.releaseHost(bus);

    oam_info_log1(0, OAM_SF_ANY, "{sdio function %d enabled.}", pstSdio->func->num);
    return ret;
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
