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
  1 头文件包含
**************************************************************************** */
#include <linux/delay.h>
#include <linux/rtc.h>

#include "plat_pm.h"
#include "plat_pm_wlan.h"
#include "exception_rst.h"
#include "plat_firmware.h"
#include "oal_file.h"
#include "oal_sdio_host_if.h"
#include "hcc_host.h"
#include "oam_ext_if.h"
#include "oal_chr.h"

/* ****************************************************************************
  3 全局变量定义
**************************************************************************** */
struct st_exception_info *g_pst_exception_info = HI_NULL;

static hi_u8 g_dev_pannic = HI_FALSE;

void oal_wakeup_exception(void)
{
    unsigned long flags;

    if (g_pst_exception_info == HI_NULL) {
        printk("[E]%s, g_pst_exception_info is null\n", __FUNCTION__);
        return;
    }

    oal_spin_lock_irq_save(&(g_pst_exception_info->excp_lock), &flags);
    if (work_busy(&(g_pst_exception_info->excp_worker))) {
        oal_spin_unlock_irq_restore(&g_pst_exception_info->excp_lock, &flags);
        return;
    }

    g_pst_exception_info->excetion_type = TRANS_FAIL;
    schedule_work(&(g_pst_exception_info->excp_worker));
    oal_spin_unlock_irq_restore(&(g_pst_exception_info->excp_lock), &flags);
}

void oal_exception_submit(hi_s32 excep_type)
{
    unsigned long flags;

    if (g_pst_exception_info == NULL) {
        printk("[E]%s, g_pst_exception_info is null\n", __FUNCTION__);
        return;
    }

    if (wlan_pm_is_shutdown()) {
        return;
    }

    if (HOST_ALLOW_TO_SLEEP == wlan_pm_state_get()) {
        return;
    }
    wlan_pm_set_pm_sts_exception();
    oal_spin_lock_irq_save(&(g_pst_exception_info->excp_lock), &flags);
    if (work_busy(&(g_pst_exception_info->excp_worker))) {
        printk("excep %d block, exception %d is working\n", excep_type, g_pst_exception_info->excetion_type);
        oal_spin_unlock_irq_restore(&g_pst_exception_info->excp_lock, &flags);
        return;
    }

    g_pst_exception_info->excetion_type = excep_type;
    schedule_work(&(g_pst_exception_info->excp_worker));
    oal_spin_unlock_irq_restore(&(g_pst_exception_info->excp_lock), &flags);
}

hi_s32 oal_exception_is_busy(void)
{
    if (oal_unlikely(g_pst_exception_info == NULL)) {
        return HI_FALSE;
    }

    if (work_busy(&(g_pst_exception_info->excp_worker))) {
        /* sdio mem dump is processing, can't power off or submit repeat */
        return HI_TRUE;
    }

    return HI_FALSE;
}

hi_s32 oal_trigger_exception(hi_s32 is_sync)
{
    unsigned long timeout_jiffies;
    struct BusDev *bus = oal_get_bus_default_handler();
    if (oal_exception_is_busy() == HI_TRUE) {
        return HI_TRUE;
    }
    printk("oal_trigger_exception start\n");
    /* trigger device panic */
    if (oal_bus_send_msg(bus, H2D_MSG_TEST) != HI_SUCCESS) {
        printk("send sdio panic message failed!\n");
        return HI_FALSE;
    }

    if (is_sync != HI_TRUE) {
        printk("sdio exception is doing...\n");
        return HI_TRUE;
    }

    /* wait device panic */
    timeout_jiffies = OAL_TIME_JIFFY + OAL_MSECS_TO_JIFFIES(2000); /* jiffies 2000 */
    for (;;) {
        if (oal_exception_is_busy() == HI_TRUE) {
            break;
        }

        if (oal_time_after(OAL_TIME_JIFFY, timeout_jiffies) > 0) {
            printk("wait panic message timeout!\n");
            return HI_FALSE;
        }

        oal_msleep(OAL_JIFFIES_TO_MSECS(1));
    }

    printk("trigger sdio exception manually sucuess\n");
    return HI_TRUE;
}

/* Try to dump device mem, controlled by flag sdio_dump_mem_flag */
void oal_try_to_dump_device_mem(hi_s32 is_sync)
{
    hi_s32 ret;
    if ((g_pst_exception_info->dump_mem_flag) == NOT_DUMP_MEM) {
        printk("sdio_dump_mem_flag is NOT_DUMP_MEM\r\n");
        return;
    }

    printk("Try to dump device mem!\n");
    ret = oal_trigger_exception(is_sync);
    if (ret != HI_TRUE) {
        printk("call oal_trigger_exception fail!\n");
    }
}

hi_void oal_set_dev_panic(hi_void)
{
    g_dev_pannic = HI_TRUE;
}

hi_void oal_clear_dev_panic(hi_void)
{
    g_dev_pannic = HI_FALSE;
}

hi_u8 oal_dev_is_panic(hi_void)
{
    return g_dev_pannic;
}

hi_s32 oal_device_panic_callback(void *data)
{
    hi_unref_param(data);
    oam_error_log0(0, 0, "=======device_panic=========\n");
    oal_set_dev_panic();
    oal_exception_submit(DEVICE_PANIC);
    if (hisi_sched_event(DEV_PANIC) != HI_SUCCESS) {
        printk("DEV_PANIC event notification failed\n");
    }
    return HI_SUCCESS;
}

hi_void oal_frw_exception_report(hi_void)
{
    if (hisi_sched_event(DRIVER_HUNG) != HI_SUCCESS) {
        printk("FRW_ENQUEUE_FAIL event notification failed\n");
    }
}
