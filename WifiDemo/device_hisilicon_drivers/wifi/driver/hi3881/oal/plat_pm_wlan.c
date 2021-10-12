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
  1 Header File Including
**************************************************************************** */
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#include <linux/module.h> /* kernel module definitions */
#include <linux/interrupt.h>
#include <linux/timer.h>
#ifndef HAVE_PCLINT_CHECK
#include <linux/platform_device.h>
#include <linux/wakelock.h>
#endif

#include "oal_sdio.h"
#include "oal_sdio_comm.h"
#include "oal_sdio_host_if.h"
#include "oal_mm.h"
#include "hi_types.h"
#include "plat_pm_wlan.h"
#include "plat_pm.h"
#include "hcc_host.h"
#include "oam_ext_if.h"
#include "exception_rst.h"
#include "oal_interrupt.h"
#ifdef _PRE_WLAN_FEATURE_WOW
#include "hmac_wow.h"
#endif

#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#include <linux/module.h> /* kernel module definitions */
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/timer.h>
#include <linux/platform_device.h>
#include <linux/kobject.h>
#include <linux/irq.h>
#include <linux/mmc/sdio.h>
#include <linux/mmc/host.h>
#include <linux/mmc/card.h>
#include <linux/mmc/sdio_func.h>
#include <linux/mmc/sdio_ids.h>
#include <linux/mmc/sdio_func.h>
#include <linux/mmc/host.h>
#include <linux/gpio.h>
#include <linux/sched.h>
#include <asm/siginfo.h>
#include <linux/pid_namespace.h>
#include <linux/pid.h>
#include <linux/version.h>
#include "oal_sdio_comm.h"
#include "oal_sdio_host_if.h"
#include "plat_pm_wlan.h"
#include "plat_pm.h"
#include "hcc_host.h"
#include "oam_ext_if.h"
#include "oal_mm.h"
#include "exception_rst.h"
#ifdef _PRE_WLAN_FEATURE_WOW
#include "hmac_wow.h"
#endif
#endif

/* 公用数据与函数接口 */
struct wlan_pm_info *g_gpst_wlan_pm_info = HI_NULL;

wlan_pm_callback_stru     g_wlan_pm_callback = {
    .wlan_pm_wakeup_dev  = wlan_pm_wakeup_dev,
    .wlan_pm_state_get   = wlan_pm_state_get,
    .wlan_pm_wakeup_host = wlan_pm_wakeup_host,
    .wlan_pm_feed_wdg    = wlan_pm_feed_wdg,
    .wlan_pm_wakeup_dev_ack    = wlan_pm_wakeup_dev_ack,
};

int g_wk_fail_process_pid = 0; /* 由应用层注册处理唤醒device失败时的进程ID */
hi_wifi_wkup_fail_cb g_wkup_fail_cb = NULL;
oal_bool_enum_uint8 g_wlan_pm_switch = 0; /* 1131C-debug */
oal_atomic g_wakeup_dev_wait_ack;
oal_atomic g_dev_sleep_wait_ack;
volatile hi_u8 g_wlan_pm_disabling = 0;

void wlan_pm_sleep_work(oal_work_stru *worker);
hi_s32 wlan_pm_wakeup_done_callback(void *data);
hi_s32 wlan_pm_close_done_callback(void *data);

hi_s32 wlan_pm_stop_wdg(hi_void);
hi_s32 wlan_pm_work_submit(struct wlan_pm_info *wlan_pm, oal_work_stru *worker);
void wlan_pm_info_clean(void);

/* ****************************************************************************
 功能描述  : 获取全局wlan结构
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 初始化返回值，成功或失败原因
**************************************************************************** */
struct wlan_pm_info *wlan_pm_get_drv(hi_void)
{
    return g_gpst_wlan_pm_info;
}

/* ****************************************************************************
 功能描述  : wifi是否上电
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 初始化返回值，成功或失败原因
**************************************************************************** */
hi_s32 wlan_pm_is_poweron(hi_void)
{
    struct wlan_pm_info *wlan_pm = wlan_pm_get_drv();

    if (wlan_pm == HI_NULL) {
        return HI_TRUE;
    }
    if (wlan_pm->wlan_power_state == POWER_STATE_OPEN) {
        return HI_TRUE;
    } else {
        return HI_FALSE;
    }
}

hi_s32 wlan_pm_is_shutdown(hi_void)
{
    struct wlan_pm_info *wlan_pm = wlan_pm_get_drv();
    if (wlan_pm == HI_NULL) {
        oam_error_log0(0, 0, "pm_data is HI_NULL!");
        return -FAILURE;
    }

    return ((wlan_pm->wlan_power_state == POWER_STATE_SHUTDOWN) ? true : false);
}

hi_void wlan_pm_set_wkfail_pid(int pid)
{
    g_wk_fail_process_pid = pid;
}

/* ****************************************************************************
 功能描述  : 注册唤醒device失败时的回调处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 初始化返回值，成功或失败原因
**************************************************************************** */
hi_s32 hi_wifi_register_wkup_fail_process_handle(hi_wifi_wkup_fail_cb cb)
{
    if (cb == NULL) {
        return FAILURE;
    }
    g_wkup_fail_cb = cb;
    return SUCCESS;
}

/* ****************************************************************************
 功能描述  : 获取回调handler指针
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 初始化返回值，成功或失败原因
**************************************************************************** */
struct wifi_srv_callback_handler *wlan_pm_get_wifi_srv_handler(hi_void)
{
    struct wlan_pm_info *wlan_pm = wlan_pm_get_drv();
    if (wlan_pm == HI_NULL) {
        return HI_FALSE;
    }
    return &wlan_pm->st_wifi_srv_handler;
}

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
/* ****************************************************************************
 功能描述  : 提交一个kernel work
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 初始化返回值，成功或失败原因
**************************************************************************** */
hi_s32 wlan_pm_work_submit(struct wlan_pm_info *wlan_pm, oal_work_stru *worker)
{
    hi_s32 ret = 0;

    if (work_busy(worker)) {
        /* If comm worker is processing,
          we need't submit again */
        ret = -OAL_EBUSY;
        return ret;
    } else {
        oam_info_log1(0, OAM_SF_PWR, "WiFi %pF Worker Submit\n", (uintptr_t)(hi_void *)worker->func);
        queue_work(wlan_pm->pm_wq, worker);
    }
    return ret;
}
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
/* ****************************************************************************
 功能描述  : 提交一个kernel work
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 初始化返回值，成功或失败原因
**************************************************************************** */
hi_s32 wlan_pm_work_submit(struct wlan_pm_info *wlan_pm, oal_work_stru *worker)
{
    hi_s32 ret = 0;
    if (oal_work_is_busy(worker)) {
        /* If comm worker is processing,
          we need't submit again */
        ret = -OAL_EBUSY;
        return ret;
    } else {
        oam_info_log1(0, OAM_SF_PWR, "WiFi %pF Worker Submit\n", (hi_void *)worker->func);
        if (oal_queue_work(wlan_pm->pm_wq, worker) == false) {
            ret = -OAL_EFAIL;
        }
    }
    return ret;
}

/* ****************************************************************************
 功能描述  : device应答poweroff ack消息处理
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
**************************************************************************** */
hi_s32 wlan_pm_close_done_callback(void *data)
{
    struct wlan_pm_info *wlan_pm = (struct wlan_pm_info *)data;
    if (wlan_pm == HI_NULL) {
        return FAILURE;
    }
    /* 关闭RX通道，防止SDIO RX thread继续访问SDIO */
    oal_disable_bus_state(wlan_pm->bus, OAL_SDIO_RX);
    wlan_pm->close_done_callback++;
    OAL_COMPLETE(&wlan_pm->close_done);
    oam_warning_log0(0, OAM_SF_PWR, "complete H2D_MSG_PM_WLAN_OFF done!");
    return SUCCESS;
}

hi_void wlan_pm_wkfail_send_sig(void)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,0,0)
    struct kernel_siginfo info = { 0 };
#else
    struct siginfo info = { 0 };
#endif
    int ret;
    if (g_wk_fail_process_pid <= 0) {
        printk("wk fail process not register \n");
        return;
    }
    printk("wkup fail process pid:%d, sig id:%d \n", g_wk_fail_process_pid, WK_FAIL_SIG);
    info.si_signo = WK_FAIL_SIG;
    info.si_code = 0;
    info.si_int = 0;
    struct task_struct *current_task = HI_NULL;
    rcu_read_lock();
    current_task = pid_task(find_vpid(g_wk_fail_process_pid), PIDTYPE_PID);
    rcu_read_unlock();
    ret = send_sig_info(WK_FAIL_SIG, &info, current_task);
    if (ret < 0) {
        printk("error sending signal\n");
    }
}

#endif

/* ****************************************************************************
 功能描述  : 使能wlan平台低功耗
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 初始化返回值，成功或失败原因
**************************************************************************** */
hi_u32 hi_wifi_plat_pm_enable(hi_void)
{
#ifndef _PRE_FEATURE_NO_GPIO
    if (!g_wlan_pm_switch) {
        return HI_SUCCESS;
    }
#endif
    struct wlan_pm_info *wlan_pm = wlan_pm_get_drv();
    if (wlan_pm == NULL) {
        oam_warning_log0(0, OAM_SF_PWR, "hi_wifi_plat_pm_enable pst_wlan_pm is null!");
        return HI_FAIL;
    }

    if (wlan_pm->wlan_pm_enable == HI_TRUE) {
        oam_warning_log0(0, OAM_SF_PWR, "hi_wifi_plat_pm_enable already enabled!");
        return HI_SUCCESS;
    }
    wlan_pm->wlan_pm_enable = HI_TRUE;
    /* 将timeout值恢复为默认值，并启动定时器 */
    wlan_pm_set_timeout(WLAN_SLEEP_DEFAULT_CHECK_CNT);
    oam_info_log0(0, OAM_SF_PWR, "hi_wifi_plat_pm_enable HI_SUCCESS!");
    return HI_SUCCESS;
}

hi_u8 wlan_pm_is_disabling(void)
{
    return g_wlan_pm_disabling;
}

/* ****************************************************************************
 函 数 名  : hi_wifi_plat_pm_disable
 功能描述  : 去使能wlan平台低功耗
 返 回 值  : 初始化返回值，成功或失败原因
**************************************************************************** */
hi_s32 wlan_pm_disable_check_wakeup(hi_s32 flag)
{
    struct wlan_pm_info *wlan_pm = wlan_pm_get_drv();
    if (wlan_pm == HI_NULL) {
        oam_warning_log0(0, OAM_SF_PWR, "wlan_pm_disable_check_wakeup pst_wlan_pm is null!");
        return HI_SUCCESS;
    }
    hcc_handler_stru *hcc_chl = hcc_host_get_handler();
    if (hcc_chl == HI_NULL) {
        printk("hcc_chl is null \n");
        return HI_SUCCESS;
    }
    g_wlan_pm_disabling = 1;
    hcc_tx_transfer_lock(hcc_host_get_handler());
    if (wlan_pm->wlan_pm_enable == HI_FALSE) {
        oam_warning_log0(0, OAM_SF_PWR, "hi_wifi_plat_pm_disable already disabled!");
        hcc_tx_transfer_unlock(hcc_host_get_handler());
        g_wlan_pm_disabling = 0;
        return HI_SUCCESS;
    }
    if (flag == HI_TRUE) {
        if (wlan_pm_wakeup_dev() != HI_SUCCESS) {
            oam_warning_log0(0, OAM_SF_PWR, "pm wake up dev fail!");
        }
    }
    wlan_pm->wlan_pm_enable = HI_FALSE;
    hcc_tx_transfer_unlock(hcc_host_get_handler());
    g_wlan_pm_disabling = 0;
    oal_cancel_work_sync(&wlan_pm->sleep_work);
    oam_info_log0(0, OAM_SF_PWR, "hi_wifi_plat_pm_disable HI_SUCCESS!");
    return HI_SUCCESS;
}

hi_u32 hi_wifi_plat_pm_disable(hi_void)
{
    return (hi_u32)wlan_pm_disable_check_wakeup(HI_TRUE);
}
/* 投票禁止睡眠 */
hi_void wlan_pm_add_vote(wlan_pm_vote_id id)
{
    struct wlan_pm_info *wlan_pm = wlan_pm_get_drv();
    unsigned long ret;
    oal_spin_lock_stru spin_lock;
    unsigned long flags;
    if ((wlan_pm == NULL) || (wlan_pm->bus == NULL) || (id > HI_PM_ID_MAX)) {
        oam_error_log1(0, OAM_SF_PWR, "wlan_pm_add_vote, para error, id:%u", id);
        return;
    }
    oal_spin_lock_init(&spin_lock);
    oal_spin_lock_irq_save(&spin_lock, &flags);
    wlan_pm->vote_status |= (0x1 << (hi_u32)id);
    oal_spin_unlock_irq_restore(&spin_lock, &flags);

    oal_cancel_work_sync(&wlan_pm->sleep_work);
    g_wlan_pm_disabling = 1;
    hcc_tx_transfer_lock(hcc_host_get_handler());
    ret = wlan_pm_wakeup_dev();
    if (ret != HI_SUCCESS) {
        oam_error_log0(0, 0, "wlan wakeup fail !");
    }
    hcc_tx_transfer_unlock(hcc_host_get_handler());
    g_wlan_pm_disabling = 0;
}

/* 解除对应id的禁止进入睡眠模式状态 */
hi_void wlan_pm_remove_vote(wlan_pm_vote_id id)
{
    struct wlan_pm_info *wlan_pm = wlan_pm_get_drv();
    oal_spin_lock_stru spin_lock;
    unsigned long flags;
    if ((wlan_pm == NULL) || (wlan_pm->bus == NULL) || (id > HI_PM_ID_MAX)) {
        oam_error_log1(0, OAM_SF_PWR, "wlan_pm_remove_vote, para error, id:%u", id);
        return;
    }
    oal_spin_lock_init(&spin_lock);
    oal_spin_lock_irq_save(&spin_lock, &flags);
    wlan_pm->vote_status &= ~(0x1 << (hi_u32)id);
    oal_spin_unlock_irq_restore(&spin_lock, &flags);
    wlan_pm_feed_wdg();
}

hi_s32 plat_set_device_ready(hi_void *data)
{
    oal_completion *device_ready = (oal_completion *)data;
    if (device_ready == HI_NULL) {
        return HI_FAIL;
    }
    OAL_COMPLETE(device_ready);
    return HI_SUCCESS;
}

hi_u32 wlan_pm_open(hi_void)
{
    hi_u32 ret;
    hi_u32 i;
    oal_completion  device_ready;
    struct BusDev *bus = hcc_host_get_handler()->bus;
    oal_channel_stru *hi_sdio = (oal_channel_stru *)bus->priData.data;

    set_device_is_ready(HI_FALSE);
    OAL_INIT_COMPLETION(&device_ready);
    oal_bus_message_register(bus, D2H_MSG_WLAN_READY, plat_set_device_ready,
        &device_ready);

    if (wlan_power_on() != HI_SUCCESS) {
        oam_error_log0(0, OAM_SF_PWR, "wlan_pm_open:: wlan_power_on fail!");
        return HI_FAIL;
    }

    /* 主动检查device状态是否ready */
    for (i = 0; i < (HOST_WAIT_BOTTOM_INIT_TIMEOUT / HOST_WAIT_READY_INTERVAL); i++) {
        ret = oal_wait_for_completion_timeout(&device_ready, (hi_u32)OAL_MSECS_TO_JIFFIES(HOST_WAIT_READY_INTERVAL));
        if (ret > 0) {
            break;
        }

        oal_sdio_isr((void *)hi_sdio->func);
    }

    if (ret == 0) {
        oam_error_log0(0, 0, "oal_wait_for_completion_timeout timeout!");
        return HI_FAIL;
    }

    printk("Device is Ready!\r\n");
    set_device_is_ready(HI_TRUE);
    struct wlan_pm_info *wlan_pm_info = wlan_pm_init();
    if (wlan_pm_info == NULL) {
        oam_error_log0(0, OAM_SF_ANY, "wlan_pm_init fail error\n");
    }
    printk("wlan_pm_open SUCCESSFULLY!!\r\n");
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : close wifi,如果bfgx没有开,下电，否则下命令关WCPU
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 初始化返回值，成功或失败原因
**************************************************************************** */
#ifdef _PRE_FEATURE_NO_GPIO
hi_u32 wlan_pm_close(hi_void)
{
    printk("[plat_pm]wifi need always on,do not close!!\n");
    return HI_SUCCESS;
}
#endif

/* ****************************************************************************
 功能描述  : debug, 发消息到device，串口输出维测信息
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
**************************************************************************** */
void wlan_pm_dump_host_info(void)
{
    oal_channel_stru *hi_sdio = NULL;
    struct wlan_pm_info *wlan_pm = wlan_pm_get_drv();

    if (wlan_pm == NULL || wlan_pm->bus == NULL) {
        return;
    }
    hi_sdio = (oal_channel_stru *)wlan_pm->bus->priData.data;
#if (_PRE_FEATURE_SDIO == _PRE_FEATURE_CHANNEL_TYPE)
    printk("----------wlan_pm_dump_host_info begin-----------\n");
    printk("wlan power on:%ld, enable:%ld\n", wlan_pm->wlan_power_state, wlan_pm->wlan_pm_enable);
    printk("dev state:%ld, sleep stage:%d\n", wlan_pm->wlan_dev_state, wlan_pm->slpack);
    printk("host sleep state:%ld\n", wlan_pm->wlan_host_state);
    printk("vote state:%x,g_wlan_pm_disabling:%d \n", wlan_pm->vote_status, g_wlan_pm_disabling);
    printk("open:%d,close:%d\n", wlan_pm->open_cnt, wlan_pm->close_cnt);
    printk("gpio_intr[no.%lu]:%llu\n", hi_sdio->ul_wlan_irq, hi_sdio->gpio_int_count);
    printk("data_intr:%llu\n", hi_sdio->data_int_count);
    printk("sdio_intr:%llu\n", hi_sdio->sdio_int_count);
    printk("sdio_intr_finish:%llu\n", hi_sdio->data_int_finish_count);

    printk("sdio_no_int_count:%u\n", hi_sdio->func1_stat.func1_no_int_count);
    printk("sdio_err_int_count:%u\n", hi_sdio->func1_stat.func1_err_int_count);
    printk("sdio_msg_int_count:%u\n", hi_sdio->func1_stat.func1_msg_int_count);
    printk("sdio_data_int_count:%u\n", hi_sdio->func1_stat.func1_data_int_count);
    printk("sdio_unknow_int_count:%u\n", hi_sdio->func1_stat.func1_unknow_int_count);

    printk("wakeup_intr:%llu\n", hi_sdio->wakeup_int_count);
    printk("D2H_MSG_WAKEUP_SUCC:%d\n", hi_sdio->msg[D2H_MSG_WAKEUP_SUCC].count);
    printk("D2H_MSG_ALLOW_SLEEP:%d\n", hi_sdio->msg[D2H_MSG_ALLOW_SLEEP].count);
    printk("D2H_MSG_DISALLOW_SLEEP:%d\n", hi_sdio->msg[D2H_MSG_DISALLOW_SLEEP].count);

    printk("wakeup_dev_wait_ack:%d\n", oal_atomic_read(&g_wakeup_dev_wait_ack));
    printk("wakeup_succ:%d\n", wlan_pm->wakeup_succ);
    printk("wakeup_dev_ack:%d\n", wlan_pm->wakeup_dev_ack);
    printk("wakeup_done_callback:%d\n", wlan_pm->wakeup_done_callback);
    printk("wakeup_succ_work_submit:%d\n", wlan_pm->wakeup_succ_work_submit);
    printk("wakeup_fail_wait_sdio:%d\n", wlan_pm->wakeup_fail_wait_sdio);
    printk("wakeup_fail_timeout:%d\n", wlan_pm->wakeup_fail_timeout);
    printk("wakeup_fail_set_reg:%d\n", wlan_pm->wakeup_fail_set_reg);
    printk("wakeup_fail_submit_work:%d\n", wlan_pm->wakeup_fail_submit_work);
    printk("sleep_succ:%d\n", wlan_pm->sleep_succ);
    printk("dev_sleep_wait_ack:%d\n", oal_atomic_read(&g_dev_sleep_wait_ack));
    printk("sleep_msg_send_cnt:%d\n", wlan_pm->sleep_msg_send_cnt);
    printk("sleep_dev_ack_cnt:%d\n", wlan_pm->sleep_dev_ack_cnt);
    printk("sleep feed wdg:%d\n", wlan_pm->sleep_feed_wdg_cnt);
    printk("sleep start wdg timer:%d\n", wlan_pm->sleep_start_wdg_timer_cnt);
    printk("sleep_fail_request:%d\n", wlan_pm->sleep_fail_request);
    printk("sleep_fail_set_reg:%d\n", wlan_pm->sleep_fail_set_reg);
    printk("sleep_fail_wait_timeout:%d\n", wlan_pm->sleep_fail_wait_timeout);
    printk("sleep_fail_forbid:%d\n", wlan_pm->sleep_fail_forbid);
    printk("sleep_work_submit:%d\n", wlan_pm->sleep_work_submit);
    printk("----------wlan_pm_dump_host_info end-----------\n");
#endif
}

/* ****************************************************************************
 功能描述  : device侧低功耗维测信息，串口输出维测信息
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
**************************************************************************** */
void wlan_pm_dump_device_pm_info(char *buf, int length)
{
    hi_device_lpc_info *lpc_info = (hi_device_lpc_info *)buf;
    if (lpc_info == HI_NULL || length < (int)sizeof(hi_device_lpc_info)) {
        oam_error_log2(0, 0, "length = %d, %d", length, sizeof(hi_device_lpc_info));
        return;
    }
    printk("----------wlan_pm_dump_device_pm_info begin-----------\n");
    printk("wakeup_times:%u\n", lpc_info->wakeup_times);
    printk("sleep_threshold_refuse_times:%u\n", lpc_info->sleep_threshold_refuse_times);
    printk("sleep_check_refuse_times:%u\n", lpc_info->sleep_check_refuse_times);
    printk("sleep_times:%u\n", lpc_info->sleep_times);
    printk("sleep_threshold:%u\n", lpc_info->sleep_threshold);
    printk("dsleep_fail_times:%u\n", lpc_info->dsleep_fail_times);
    printk("type:%d\n", lpc_info->type);
    printk("evt_sts:%x\n", lpc_info->evt_sts);
    printk("int_sts:%x\n", lpc_info->int_sts);
    printk("last_time_vote_state:%d\n", lpc_info->last_time_vote_state);
    printk("timer_ticks:%u\n", lpc_info->timer_ticks);
    printk("timer_handle:%x\n", lpc_info->timer_handle);
    printk("timer_handle_arg:%u\n", lpc_info->timer_handle_arg);
    printk("task_id:%u\n", lpc_info->task_id);
    printk("task_ticks:%d\n", lpc_info->task_ticks);
    printk("sleep_ticks:%d\n", lpc_info->sleep_ticks);
    printk("veto_info:%x\n", lpc_info->veto_info);
    printk("sdio_busy:%u\n", lpc_info->sdio_busy);
    printk("host_sleeped:%d\n", lpc_info->host_sleeped);
    printk("host_allow_dev_sleep:%d\n", lpc_info->host_allow_dev_sleep);
    printk("sdio_allow_sleep:%d\n", lpc_info->sdio_allow_sleep);
    printk("refuse_vote_handle:%x\n", lpc_info->refuse_vote_handle);
    printk("host_allow_device_slp_times:%u\n", lpc_info->host_allow_device_slp_times);
    printk("sdio_wkup_int_times:%u\n", lpc_info->sdio_wkup_int_times);
    printk("device_sdio_wkup_ack_times:%u\n", lpc_info->device_sdio_wkup_ack_times);
    printk("sdio_busy_times:%u\n", lpc_info->sdio_busy_times);
    printk("host_vote_work_times:%u\n", lpc_info->host_vote_work_times);
    printk("sdio_vote_work_times:%u\n", lpc_info->sdio_vote_work_times);

    printk("----------wlan_pm_dump_device_pm_info end-----------\n");
}

hi_void wlan_pm_dump_device_info(hi_void)
{
    struct wlan_pm_info *wlan_pm = wlan_pm_get_drv();
    if (wlan_pm == HI_NULL) {
        return;
    }
    oal_bus_send_msg(wlan_pm->bus, H2D_MSG_PM_DEBUG);
}

/* ****************************************************************************
 功能描述  : 打印低功耗维测信息
 输入参数  : 1:host，0:device
 输出参数  : 无
 返 回 值  :
**************************************************************************** */
hi_void hi_wlan_dump_pm_info(hi_u8 is_host)
{
    if (is_host > 1) { /* 大于1为异常参数 */
        printk("para error, 1 : host, 0 : device;input:%u \n", is_host);
        return;
    }
    if (is_host) {
        wlan_pm_dump_host_info();
    } else {
        wlan_pm_dump_device_info();
    }
}

/* ****************************************************************************
 功能描述  : 唤醒流程reinit chan
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 初始化返回值，成功或失败原因
**************************************************************************** */
hi_s32 wlan_pm_chan_reinit(void)
{
#if (_PRE_FEATURE_SDIO == _PRE_FEATURE_CHANNEL_TYPE)
    hi_s32 ret = HI_SUCCESS;
#ifdef _PRE_WLAN_PM_FEATURE_FORCESLP_RESUME
    if (wlan_resume_state_get() != 0) { // forceslp resume flow
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
        ret = oal_sdio_func_probe_resume(oal_get_bus_default_handler());
#endif
    }
#endif
    return ret;
#else
    return HI_SUCCESS;
#endif
}

/* ****************************************************************************
 功能描述  : 唤醒device
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 初始化返回值，成功或失败原因
**************************************************************************** */
hi_s32 wlan_pm_wakeup_dev_again(hi_void)
{
    struct wlan_pm_info *wlan_pm = wlan_pm_get_drv();
    hi_s32 ret;
    if (wlan_pm == HI_NULL || wlan_pm->bus == HI_NULL) {
        oam_warning_log0(0, OAM_SF_PWR, "[pm]HI_NULL!!");
        return HI_FAIL;
    }
    hi_u32 wakeup_times = 0;
    for (; wakeup_times < WLAN_TRY_WAKEUP_FAIL_TIMES; wakeup_times++) {
        OAL_INIT_COMPLETION(&wlan_pm->wakeup_done);
        oal_atomic_set(&g_wakeup_dev_wait_ack, 1);
        /* 写device sdio寄存器，允许睡眠 */
        if (wakeup_times) {
            oal_sdio_sleep_dev(wlan_pm->bus);
        }
        /* waitting for dev channel stable */
        mdelay(10); /* delay 10ms */
        ret = oal_sdio_wakeup_dev(wlan_pm->bus);
        if (ret < 0) {
            wlan_pm->wakeup_fail_wait_sdio++;
            oam_warning_log1(0, OAM_SF_PWR, "[pm]sdio wake device failed!,  ret:%d!\n", ret);
            continue;
        }
        ret = (hi_s32)oal_wait_for_completion_timeout(&wlan_pm->wakeup_done,
            (hi_u32)OAL_MSECS_TO_JIFFIES(WLAN_WAKUP_MSG_WAIT_TIMEOUT));
        if (ret == 0) {
            wlan_pm->wakeup_fail_timeout++;
            printk("wlan_pm_wakeup_dev_again wait cor completion timeout[%u]\n", wakeup_times);
            continue;
        }
        return HI_SUCCESS;
    }
    printk("[ERROR]wk again fail:[%u] \n", wakeup_times);
    return HI_FAIL;
}

hi_void wlan_pm_wakeup_fail_process(hi_void)
{
    oal_msleep(200); /* sleep 200ms */
    printk("wakeup device fail, dump host pm msg and notification app layer \r\n");
    /* 打印host侧pm统计信息 */
    wlan_pm_dump_host_info();
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    /* 通知应用层 */
    wlan_pm_wkfail_send_sig();
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    if (g_wkup_fail_cb != NULL) {
        g_wkup_fail_cb();
    } else {
        printk("wk fail process not register \n");
    }
#endif
#if (_PRE_FEATURE_SDIO == _PRE_FEATURE_CHANNEL_TYPE)
    oal_wakeup_exception();
#endif
}

unsigned long wlan_pm_wakeup_dev(hi_void)
{
    struct wlan_pm_info *wlan_pm = wlan_pm_get_drv();
    hi_s32 ret;
    if (wlan_pm == HI_NULL || wlan_pm->bus == HI_NULL) {
        oam_error_log0(0, OAM_SF_PWR, "[pm]HI_NULL!!");
        return HI_SUCCESS;
    }
    if (wlan_pm->wlan_power_state == POWER_STATE_SHUTDOWN) {
        return OAL_EFAIL;
    }

    if ((wlan_pm->wlan_dev_state == HOST_DISALLOW_TO_SLEEP) || (wlan_pm->wlan_pm_enable == HI_FALSE)) {
        return HI_SUCCESS;
    } else if (wlan_pm->wlan_dev_state == HOST_EXCEPTION) {
        return OAL_EFAIL;
    }
    /* wait for sdio wakeup */
    ret = oal_down_timeout(&g_chan_wake_sema, TIMEOUT_MUTIPLE_6 * OAL_TIME_HZ);
    if (ret == -ETIME) {
        wlan_pm->wakeup_fail_wait_sdio++;
        oam_error_log0(0, OAM_SF_PWR, "[pm]sdio controller is not ready!");
        goto wakeup_fail;
    }
    oal_up(&g_chan_wake_sema);

#if (_PRE_FEATURE_SDIO == _PRE_FEATURE_CHANNEL_TYPE)
    oal_wlan_gpio_intr_enable(wlan_pm->bus, HI_FALSE);
#endif
    wlan_pm->wakeup_gpio_up_cnt++;
#ifndef _PRE_FEATURE_NO_GPIO
    board_set_wlan_h2d_pm_state(WLAN_PM_WKUPDEV_LEVEL);
#endif
#if (_PRE_FEATURE_SDIO == _PRE_FEATURE_CHANNEL_TYPE)
    oal_wlan_gpio_intr_enable(wlan_pm->bus, HI_TRUE);
#endif
    wlan_pm_state_set(wlan_pm, HOST_DISALLOW_TO_SLEEP);
    ret = wlan_pm_wakeup_dev_again();
    if (ret != HI_SUCCESS) {
        goto wakeup_fail;
    }
    wlan_pm->wakeup_succ++;
    wlan_pm->wdg_timeout_curr_cnt = 0;
    wlan_pm->packet_cnt = 0;
    wlan_pm_feed_wdg();
    return HI_SUCCESS;
wakeup_fail:
#ifndef _PRE_FEATURE_NO_GPIO
    board_set_wlan_h2d_pm_state(WLAN_PM_SLPREQ_LEVEL);
#endif
    wlan_pm_state_set(wlan_pm, HOST_ALLOW_TO_SLEEP);
    wlan_pm_wakeup_fail_process();
    return HI_FAIL;
}

hi_void wlan_pm_wakeup_dev_ack(hi_void)
{
    struct wlan_pm_info *wlan_pm = wlan_pm_get_drv();
    if (oal_atomic_read(&g_wakeup_dev_wait_ack) != 0) {
        if (wlan_pm == HI_NULL) {
            oam_warning_log0(0, OAM_SF_PWR, "wlan_pm_wakeup_dev_ack, dev_info is null \n");
            return;
        }
        OAL_COMPLETE(&wlan_pm->wakeup_done);
        oal_atomic_set(&g_wakeup_dev_wait_ack, 0);
    }
    return;
}

/* ****************************************************************************
 功能描述  : device唤醒host
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 初始化返回值，成功或失败原因
**************************************************************************** */
unsigned long wlan_pm_wakeup_host(void)
{
    oal_channel_stru *hi_sdio = NULL;
    struct wlan_pm_info *wlan_pm = wlan_pm_get_drv();

    if (wlan_pm == NULL || wlan_pm->bus == NULL) {
        printk("wlan_pm_wakeup_host pst wlan pm is null \r\n");
        return HI_SUCCESS;
    }
    oal_bus_wake_lock(wlan_pm->bus);
    if (wlan_pm_work_submit(wlan_pm, &wlan_pm->wakeup_work) != 0) {
        wlan_pm->wakeup_fail_submit_work++;

        oal_bus_wake_unlock(wlan_pm->bus);
        hi_sdio = (oal_channel_stru *)wlan_pm->bus->priData.data;
        oam_warning_log1(0, OAM_SF_PWR, "wlan_pm_wakeup_host submit work fail, release wakelock %lu!\n",
                         hi_sdio->st_sdio_wakelock.lock_count);
    } else {
        wlan_pm->wakeup_succ_work_submit++;
    }
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : device ack处理
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
**************************************************************************** */
hi_void wlan_pm_ack_handle(hi_s32 ack, hi_void *data)
{
    struct wlan_pm_info *wlan_pm = (struct wlan_pm_info *)data;
    if (wlan_pm == NULL) {
        oam_info_log0(0, OAM_SF_PWR, "[pm]wlan_pm_ack_handle data is null !\n");
        return;
    }
    wlan_pm->sleep_dev_ack_cnt++;
    if (oal_atomic_read(&g_dev_sleep_wait_ack) != 0) {
        wlan_pm->slpack = ack;
        OAL_COMPLETE(&wlan_pm->sleep_request_ack);
        oal_atomic_set(&g_dev_sleep_wait_ack, 0);
    }
}

/* ****************************************************************************
 功能描述  : device应答allow_sleep消息处理
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
**************************************************************************** */
hi_s32 wlan_pm_allow_sleep_callback(void *data)
{
    wlan_pm_ack_handle(SLPACK_DEV_ALLOW, data);
    return SUCCESS;
}

/* ****************************************************************************
 功能描述  : device应答allow_sleep消息处理
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
**************************************************************************** */
hi_s32 wlan_pm_disallow_sleep_callback(void *data)
{
    wlan_pm_ack_handle(SLPACK_DEV_DISALLOW, data);
    return SUCCESS;
}

/* ****************************************************************************
 功能描述  : device应答host sleep消息处理
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
**************************************************************************** */
hi_s32 wlan_pm_host_sleep_ack_callback(void *data)
{
    struct wlan_pm_info *wlan_pm = (struct wlan_pm_info *)data;
    if (wlan_pm == NULL) {
        oam_info_log0(0, OAM_SF_PWR, "[pm]wlan_pm_ack_handle data is null !\n");
        return SUCCESS;
    }
    OAL_COMPLETE(&wlan_pm->host_sleep_request_ack);
    return SUCCESS;
}

/* ****************************************************************************
 功能描述  : device唤醒host work
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 初始化返回值，成功或失败原因
**************************************************************************** */
void wlan_pm_wakeup_work(oal_work_stru *worker)
{
    struct wlan_pm_info *wlan_pm = wlan_pm_get_drv();
    unsigned long ret;
    hi_unref_param(worker);
    if (wlan_pm == NULL || wlan_pm->bus == NULL) {
        return;
    }
    hcc_tx_transfer_lock(hcc_host_get_handler());

    ret = wlan_pm_wakeup_dev();
    if (ret != HI_SUCCESS) {
        oam_error_log0(0, 0, "wlan wakeup fail !");
    }
    hcc_tx_transfer_unlock(hcc_host_get_handler());
    oal_bus_wake_unlock(wlan_pm->bus);
    oam_info_log0(0, OAM_SF_PWR, "wlan_d2h_wakeup_succ !\n");
    return;
}

/* ****************************************************************************
 功能描述  : device应答wakeup succ消息处理
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
**************************************************************************** */
hi_s32 wlan_pm_wakeup_done_callback(void *data)
{
    struct wlan_pm_info *wlan_pm = (struct wlan_pm_info *)data;
    if (wlan_pm == NULL) {
        oam_info_log0(0, OAM_SF_PWR, "[pm]wlan_pm_wakeup_done_callback data is null !\n");
        return SUCCESS;
    }
    wlan_pm->wakeup_done_callback++;
    wlan_pm_wakeup_dev_ack();
    return SUCCESS;
}

/* ****************************************************************************
 功能描述  : dfx时更新dev状态为异常
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 初始化返回值，成功或失败原因
**************************************************************************** */
hi_u32 wlan_pm_set_pm_sts_exception(hi_void)
{
    struct wlan_pm_info *wlan_pm = wlan_pm_get_drv();
    if (wlan_pm == HI_NULL) {
        return HI_FAIL;
    }
    wlan_pm_state_set(wlan_pm, HOST_EXCEPTION);
    return HI_SUCCESS;
}

/* ****************************************************************************
 功能描述  : 允许device睡眠
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功或失败原因
**************************************************************************** */
void wlan_pm_sleep_work(oal_work_stru *worker)
{
    hi_unref_param(worker);
    unsigned long ret;
    ret = wlan_pm_dev_sleep_request();
    if (ret != HI_SUCCESS) {
        oam_info_log0(0, 0, "wlan_pm_sleep_work fail !");
    }
}

/* ****************************************************************************
 功能描述  : host向device发送睡眠请求，表明host想睡
 输入参数  : true:通知device host要睡眠，false:通知device host不睡
 输出参数  : 无
 返 回 值  : HI_SUCCESS/FAIL
**************************************************************************** */
unsigned int hi_wifi_host_request_sleep(bool slp)
{
    struct wlan_pm_info *wlan_pm = wlan_pm_get_drv();
    hi_s32 l_ret;
    hi_u32 ul_ret;

    if (wlan_pm == HI_NULL || wlan_pm->bus == HI_NULL) {
        oam_warning_log0(0, OAM_SF_PWR, "[pm]HI_NULL!!");
        return HI_FAIL;
    }
    hcc_tx_transfer_lock(hcc_host_get_handler());
    if ((wlan_pm->wlan_host_state == HOST_SLEEPED) && slp) {
        hcc_tx_transfer_unlock(hcc_host_get_handler());
        return HI_SUCCESS;
    }
    if ((wlan_pm->wlan_host_state == HOST_NOT_SLEEP) && (!slp)) {
        hcc_tx_transfer_unlock(hcc_host_get_handler());
        return HI_SUCCESS;
    }
    if (wlan_pm_wakeup_dev() != HI_SUCCESS) {
        oam_info_log0(0, OAM_SF_PWR, "[pm]host slp request, wkeup dev fail!");
        hcc_tx_transfer_unlock(hcc_host_get_handler());
        return HI_FAIL;
    }
    OAL_INIT_COMPLETION(&wlan_pm->host_sleep_request_ack);
    if (slp) {
        l_ret = oal_bus_send_msg(wlan_pm->bus, H2D_MSG_HOST_SLEEP);
    } else {
        l_ret = oal_bus_send_msg(wlan_pm->bus, H2D_MSG_HOST_DISSLEEP);
    }
    if (l_ret != HI_SUCCESS) {
        oam_error_log0(0, OAM_SF_PWR, "[pm]wlan_pm_sleep_request fail !\n");
        goto fail_sleep;
    }
    ul_ret = oal_wait_for_completion_timeout(&wlan_pm->host_sleep_request_ack,
        (hi_u32)OAL_MSECS_TO_JIFFIES(WLAN_SLEEP_MSG_WAIT_TIMEOUT));
    if (ul_ret == 0) {
        oam_error_log0(0, 0, "wait dev ack timeout !");
        goto fail_sleep;
    }
    wlan_pm->wlan_host_state = slp ? HOST_SLEEPED : HOST_NOT_SLEEP;
    hcc_tx_transfer_unlock(hcc_host_get_handler());
    return HI_SUCCESS;
fail_sleep:
    hcc_tx_transfer_unlock(hcc_host_get_handler());
    oam_error_log0(0, 0, "hi_wifi_host_request_sleep fail !");
    return HI_FAIL;
}


/* ****************************************************************************
 功能描述  : 发送允许device睡眠 请求给device
 输入参数  : 无
 输出参数  : 无
 返 回 值  : HI_SUCCESS/FAIL
**************************************************************************** */
hi_s32 wlan_pm_allow_dev_sleep(void)
{
    struct wlan_pm_info *wlan_pm = wlan_pm_get_drv();
    hi_s32 ret;
    if (wlan_pm == HI_NULL || wlan_pm->bus == HI_NULL) {
        oam_warning_log0(0, OAM_SF_PWR, "[pm]HI_NULL!!");
        return HI_FAIL;
    }
    wlan_pm->slpack = SLPACK_DEV_ALLOW;
    OAL_INIT_COMPLETION(&wlan_pm->sleep_request_ack);
    wlan_pm->sleep_msg_send_cnt++;

    oal_atomic_set(&g_dev_sleep_wait_ack, 1);
    ret = oal_bus_send_msg(wlan_pm->bus, H2D_MSG_SLEEP_REQ);
    if (ret != HI_SUCCESS) {
        wlan_pm->sleep_fail_request++;
        oal_atomic_set(&g_dev_sleep_wait_ack, 0);
        oam_warning_log0(0, OAM_SF_PWR, "[pm]wlan_pm_sleep_request send msg fail !\n");
        return OAL_EFAIL;
    }
    ret = (hi_s32)oal_wait_for_completion_timeout(&wlan_pm->sleep_request_ack,
        (hi_u32)OAL_MSECS_TO_JIFFIES(WLAN_SLEEP_MSG_WAIT_TIMEOUT));
    if (ret == 0) {
        /* 超时处理，拉低gpio通知dev流程完成，再拉高gpio告知dev睡眠失败 */
#ifndef _PRE_FEATURE_NO_GPIO
        board_set_wlan_h2d_pm_state(WLAN_PM_SLPREQ_LEVEL);
        board_set_wlan_h2d_pm_state(WLAN_PM_WKUPDEV_LEVEL);
#endif
        wlan_pm->slpreq_flag = NO_SLPREQ_STATUS;
        wlan_pm->sleep_fail_wait_timeout++;
        oam_warning_log3(0, OAM_SF_PWR, "[pm]sleep_work timeouts:%u,msg send:%d,ack cnt:%d !\n",
            wlan_pm->sleep_fail_wait_timeout, wlan_pm->sleep_msg_send_cnt, wlan_pm->sleep_dev_ack_cnt);
        return OAL_EFAIL;
    }
    if (wlan_pm->slpack == SLPACK_DEV_DISALLOW) {
        wlan_pm->sleep_fail_forbid++;
        return OAL_EFAIL;
    }
    return HI_SUCCESS;
}
unsigned long wlan_pm_dev_sleep_request(void)
{
    struct wlan_pm_info *wlan_pm = wlan_pm_get_drv();
    hi_s32 ret;
    if (wlan_pm == HI_NULL || wlan_pm->bus == HI_NULL) {
        oam_error_log0(0, OAM_SF_PWR, "[pm]HI_NULL!!");
        return HI_FAIL;
    }
    wlan_pm->wdg_timeout_curr_cnt = 0;
    wlan_pm->packet_cnt = 0;
    hcc_tx_transfer_lock(hcc_host_get_handler());
    if ((wlan_pm->wlan_pm_enable == HI_FALSE) || (wlan_pm->vote_status)) {
        wlan_pm_feed_wdg();
        hcc_tx_transfer_unlock(hcc_host_get_handler());
        return HI_SUCCESS;
    }
    if (wlan_pm->wlan_dev_state == HOST_ALLOW_TO_SLEEP) {
        hcc_tx_transfer_unlock(hcc_host_get_handler());
        return HI_SUCCESS;
    } else if (wlan_pm->wlan_dev_state == HOST_EXCEPTION) {
        oam_info_log0(0, OAM_SF_PWR, "[pm]host exception, don't sleep!");
        hcc_tx_transfer_unlock(hcc_host_get_handler());
        return OAL_EFAIL;
    }
    ret = wlan_pm_allow_dev_sleep();
    if (ret != HI_SUCCESS) {
        goto fail_sleep;
    }
#if (_PRE_FEATURE_SDIO == _PRE_FEATURE_CHANNEL_TYPE)
    oal_wlan_gpio_intr_enable(wlan_pm->bus, HI_FALSE);
#endif
    wlan_pm->sleep_gpio_low_cnt++;
    wlan_pm_state_set(wlan_pm, HOST_ALLOW_TO_SLEEP);
    /* 拉低gpio告知dev流程结束并等待dev去读状态 */
#ifndef _PRE_FEATURE_NO_GPIO
    board_set_wlan_h2d_pm_state(WLAN_PM_SLPREQ_LEVEL);
#endif

#if (_PRE_FEATURE_SDIO == _PRE_FEATURE_CHANNEL_TYPE)
    /* 等待dev读取gpio状态完毕 */
    udelay(100); /* delay 100us */
    /* 写device sdio寄存器，允许睡眠 */
    oal_sdio_sleep_dev(wlan_pm->bus);
    oal_wlan_gpio_intr_enable(wlan_pm->bus, HI_TRUE);
#endif
    hcc_tx_transfer_unlock(hcc_host_get_handler());
    wlan_pm->sleep_succ++;
    return HI_SUCCESS;
fail_sleep:
    wlan_pm_feed_wdg();
    hcc_tx_transfer_unlock(hcc_host_get_handler());
    return HI_FAIL;
}

#ifdef _PRE_WLAN_PM_FEATURE_FORCESLP_RESUME

hi_void wlan_resume_state_set(hi_u32 ul_state)
{
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    if (ul_state == 0) {
        g_ul_wlan_resume_state = 0;
    } else {
        g_ul_wlan_resume_state = 1;

        g_ul_wlan_resume_wifi_init_flag = g_ul_wlan_resume_state;
    }
#endif
}

hi_u32 wlan_resume_state_get(hi_void)
{
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    return g_ul_wlan_resume_state;
#else
    return 0;
#endif
}
#endif

/* ****************************************************************************
 功能描述  : 获取pm的sleep状态
 输入参数  :
 输出参数  :
 返 回 值  : 1:allow to sleep; 0:disallow to sleep
**************************************************************************** */
unsigned long wlan_pm_state_get(void)
{
    struct wlan_pm_info *wlan_pm = wlan_pm_get_drv();
    if (wlan_pm == HI_NULL) {
        return DEV_SHUTDOWN;
    }
    return wlan_pm->wlan_dev_state;
}

/* ****************************************************************************
 功能描述  : 获取pm的sleep状态
 输入参数  :
 输出参数  :
 返 回 值  : 1:allow to sleep; 0:disallow to sleep
**************************************************************************** */
hi_void wlan_pm_state_set(struct wlan_pm_info *wlan_pm, unsigned long state)
{
    oal_channel_stru *hi_sdio = (oal_channel_stru *)wlan_pm->bus->priData.data;
    unsigned long flag;
    if (wlan_pm == HI_NULL) {
        oam_error_log0(0, 0, "wlan_pm_state_set pm info is null !");
        return;
    }
    oal_spin_lock_irq_save(&hi_sdio->st_pm_state_lock, &flag);
    wlan_pm->wlan_dev_state = state;
    oal_spin_unlock_irq_restore(&hi_sdio->st_pm_state_lock, &flag);
}

/* ****************************************************************************
 功能描述  : 睡眠定时器超时时间设置
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 初始化返回值，成功或失败原因
**************************************************************************** */
hi_void wlan_pm_set_timeout(hi_u32 timeout)
{
    struct wlan_pm_info *wlan_pm = wlan_pm_get_drv();
    if (wlan_pm == HI_NULL) {
        oam_error_log0(0, 0, "wlan_pm_set_timeout dev info not init !");
        return;
    }
    oam_info_log1(0, OAM_SF_PWR, "wlan_pm_set_timeout[%d]", timeout);

    wlan_pm->wdg_timeout_cnt = timeout;
    wlan_pm->wdg_timeout_curr_cnt = 0;
    wlan_pm->packet_cnt = 0;
    wlan_pm_feed_wdg();
}

/* ****************************************************************************
 功能描述  : 启动50ms睡眠定时器
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 初始化返回值，成功或失败原因
**************************************************************************** */
hi_void wlan_pm_feed_wdg(hi_void)
{
    struct wlan_pm_info *wlan_pm = wlan_pm_get_drv();
    if (wlan_pm == NULL) {
        oam_error_log0(0, 0, "wlan_pm_feed_wdg dev info not init !");
        return;
    }
    wlan_pm->sleep_feed_wdg_cnt++;
    if ((wlan_pm->wlan_pm_enable) && (wlan_pm->vote_status == 0)) {
        oal_timer_start(&wlan_pm->watchdog_timer, WLAN_SLEEP_TIMER_PERIOD);
        wlan_pm->sleep_start_wdg_timer_cnt++;
    }
}

/* ****************************************************************************
 功能描述  : 停止50ms睡眠定时器
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 初始化返回值，成功或失败原因
**************************************************************************** */
hi_s32 wlan_pm_stop_wdg(hi_void)
{
    oam_info_log0(0, OAM_SF_PWR, "wlan_pm_stop_wdg \r\n");
    struct wlan_pm_info *wlan_pm = wlan_pm_get_drv();
    if (wlan_pm == NULL) {
        oam_error_log0(0, 0, "wlan_pm_stop_wdg dev info not init !");
        return HI_FAIL;
    }
    wlan_pm->wdg_timeout_curr_cnt = 0;
    wlan_pm->packet_cnt = 0;
    if (in_interrupt()) {
        return oal_timer_delete(&wlan_pm->watchdog_timer);
    } else {
        return oal_timer_delete_sync(&wlan_pm->watchdog_timer);
    }
}

hi_void wlan_pm_set_packet_cnt(hi_u32 delt)
{
    struct wlan_pm_info *wlan_pm = wlan_pm_get_drv();
    if (wlan_pm == NULL) {
        oam_warning_log0(0, OAM_SF_PWR, "[pm] dev info is null \n");
        return;
    }
    wlan_pm->packet_cnt += delt;
}

/* ****************************************************************************
 功能描述  : 50ms睡眠定时器超时处理，提交一个sleep work
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 初始化返回值，成功或失败原因
**************************************************************************** */
void wlan_pm_wdg_timeout(struct wlan_pm_info *wlan_pm)
{
    if (wlan_pm == NULL) {
        oam_error_log0(0, 0, "wlan_pm_wdg_timeout dev info is null !");
        return;
    }
    if ((wlan_pm->wlan_pm_enable) && (wlan_pm->vote_status == 0)) {
        if (wlan_pm->packet_cnt == 0) {
            wlan_pm->wdg_timeout_curr_cnt++;
            if ((wlan_pm->wdg_timeout_curr_cnt >= wlan_pm->wdg_timeout_cnt) &&
                (wlan_pm_work_submit(wlan_pm, &wlan_pm->sleep_work) != 0)) {
                oam_warning_log0(0, OAM_SF_PWR, "wlan_pm_sleep_work submit fail,work is running !\n");
            } else if (wlan_pm->wdg_timeout_curr_cnt >= wlan_pm->wdg_timeout_cnt) {
                /* 提交了sleep work后，定时器不重启，避免重复提交sleep work */
                wlan_pm->sleep_work_submit++;
                return;
            }
        } else {
            wlan_pm->wdg_timeout_curr_cnt = 0;
            wlan_pm->packet_cnt = 0;
        }
    } else {
        wlan_pm->packet_cnt = 0;
        return;
    }
    wlan_pm_feed_wdg();
    return;
}

hi_s32 wlan_pm_before_device_slp_callback(void *data)
{
    (void)data;
    printk("!");
    return SUCCESS;
}
hi_s32 wlan_pm_device_wkup_callback(void *data)
{
    (void)data;
    printk("!@\n");
    return SUCCESS;
}


/* ****************************************************************************
 功能描述  : WLAN PM device状态初始化接口
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 初始化返回值，成功或失败原因
**************************************************************************** */
struct wlan_pm_info *wlan_pm_init(void)
{
    struct wlan_pm_info *wlan_pm;
    oal_channel_stru *hi_sdio = NULL;
    struct BusDev *bus = oal_get_bus_default_handler();

    wlan_pm = oal_kzalloc(sizeof(struct wlan_pm_info), OAL_GFP_KERNEL);
    if (wlan_pm == NULL) {
        oam_error_log0(0, OAM_SF_PWR, "[pm]no mem to allocate wlan_wk_slp_dev !\n");
        return NULL;
    }
    memset_s(wlan_pm, sizeof(struct wlan_pm_info), 0, sizeof(struct wlan_pm_info));
    /* work queue初始化 */
    wlan_pm->pm_wq = oal_create_singlethread_workqueue("wlan_pm_wq");
    if (wlan_pm->pm_wq == HI_NULL) {
        oam_error_log0(0, OAM_SF_PWR, "[plat_pm]Failed to create wlan_pm_wq!\n");
        oal_free(wlan_pm);
        return HI_NULL;
    }
    /* 默认关低功耗 */
    wlan_pm->wlan_pm_enable  = HI_FALSE;
    wlan_pm->vote_status  = 0; /* 默认所有投票都允许device睡眠 */
    OAL_INIT_WORK(&wlan_pm->sleep_work,  wlan_pm_sleep_work);

    wlan_pm->bus = bus;
    if (wlan_pm->bus == HI_NULL) {
        oal_free(wlan_pm);
        return HI_NULL;
    }
    hi_sdio = (oal_channel_stru *)wlan_pm->bus->priData.data;
    hi_sdio->pst_pm_callback = &g_wlan_pm_callback;

    oal_spin_lock_init(&hi_sdio->st_pm_state_lock);

    wlan_pm->wlan_power_state           = POWER_STATE_OPEN;
    wlan_pm->wlan_dev_state             = HOST_DISALLOW_TO_SLEEP;
    wlan_pm->slpreq_flag                = NO_SLPREQ_STATUS;
    wlan_pm->slpack                     = SLPACK_DEV_DISALLOW;
    wlan_pm->wlan_host_state            = HOST_NOT_SLEEP;
    /* sleep timer初始化 */
    oal_timer_init(&wlan_pm->watchdog_timer, 0, (void *)wlan_pm_wdg_timeout, (unsigned long)(uintptr_t)wlan_pm);
    wlan_pm->wdg_timeout_cnt            = WLAN_SLEEP_DEFAULT_CHECK_CNT;
    wlan_pm->wdg_timeout_curr_cnt       = 0;
    wlan_pm->packet_cnt                 = 0;

    g_gpst_wlan_pm_info = wlan_pm;

    OAL_INIT_COMPLETION(&wlan_pm->wakeup_done);
    OAL_INIT_COMPLETION(&wlan_pm->sleep_request_ack);
    OAL_INIT_COMPLETION(&wlan_pm->host_sleep_request_ack);

    oal_bus_message_register(wlan_pm->bus, D2H_MSG_WAKEUP_SUCC, wlan_pm_wakeup_done_callback, wlan_pm);
    oal_bus_message_register(wlan_pm->bus, D2H_MSG_ALLOW_SLEEP, wlan_pm_allow_sleep_callback, wlan_pm);
    oal_bus_message_register(wlan_pm->bus, D2H_MSG_DISALLOW_SLEEP, wlan_pm_disallow_sleep_callback,
        wlan_pm);
    oal_bus_message_register(wlan_pm->bus, D2H_MSG_HOST_SLEEP_ACK, wlan_pm_host_sleep_ack_callback,
        wlan_pm);
    oal_bus_message_register(wlan_pm->bus, D2H_MSG_BEFORE_DEV_SLEEP, wlan_pm_before_device_slp_callback,
        NULL);
    oal_bus_message_register(wlan_pm->bus, D2H_MSG_DEV_WKUP, wlan_pm_device_wkup_callback, NULL);

    return wlan_pm;
}

/* ****************************************************************************
 功能描述  : WLAN pm退出接口
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 初始化返回值，成功或失败原因
**************************************************************************** */
unsigned long wlan_pm_exit(hi_void)
{
    oal_channel_stru *hi_sdio = NULL;
    struct wlan_pm_info *wlan_pm = wlan_pm_get_drv();

    if (wlan_pm == HI_NULL || wlan_pm->bus == HI_NULL) {
        return HI_SUCCESS;
    }
    hi_wifi_plat_pm_disable();
    hi_s32 ret = wlan_pm_stop_wdg();
    if (ret != HI_SUCCESS) {
        oam_error_log0(0, OAM_SF_PWR, "wlan_pm_stop_wdg fail\r\n");
    }
    oal_sdio_sleep_dev(wlan_pm->bus);
    if (wlan_pm->bus != HI_NULL && wlan_pm->bus->priData.data != NULL) {
        hi_sdio = (oal_channel_stru *)wlan_pm->bus->priData.data;
        hi_sdio->pst_pm_callback = NULL;
    }
    oal_bus_message_unregister(wlan_pm->bus, D2H_MSG_WAKEUP_SUCC);
    oal_bus_message_unregister(wlan_pm->bus, D2H_MSG_ALLOW_SLEEP);
    oal_bus_message_unregister(wlan_pm->bus, D2H_MSG_DISALLOW_SLEEP);
    oal_bus_message_unregister(wlan_pm->bus, D2H_MSG_HOST_SLEEP_ACK);
    oal_bus_message_unregister(wlan_pm->bus, D2H_MSG_WLAN_READY);
    oal_bus_message_unregister(wlan_pm->bus, D2H_MSG_BEFORE_DEV_SLEEP);
    oal_bus_message_unregister(wlan_pm->bus, D2H_MSG_DEV_WKUP);

    oal_destroy_workqueue(wlan_pm->pm_wq);
    kfree(wlan_pm);
    g_gpst_wlan_pm_info = HI_NULL;
    printk("[plat_pm]wlan_pm_exit SUCCESSFULLY\r\n");
    return HI_SUCCESS;
}

void wlan_pm_info_clean(void)
{
    oal_channel_stru *hi_sdio = NULL;
    struct wlan_pm_info *wlan_pm = wlan_pm_get_drv();
    if (wlan_pm == NULL || wlan_pm->bus == NULL) {
        return;
    }
#if (_PRE_FEATURE_SDIO == _PRE_FEATURE_CHANNEL_TYPE)
    hi_sdio = (oal_channel_stru *)wlan_pm->bus->priData.data;
    hi_sdio->data_int_count                   = 0;
    hi_sdio->wakeup_int_count                 = 0;
    hi_sdio->gpio_int_count                   = 0;
    hi_sdio->sdio_int_count                   = 0;
    hi_sdio->data_int_finish_count            = 0;
    hi_sdio->func1_stat.func1_no_int_count    = 0;
    hi_sdio->func1_stat.func1_err_int_count   = 0;
    hi_sdio->func1_stat.func1_msg_int_count   = 0;
    hi_sdio->func1_stat.func1_data_int_count  = 0;
    hi_sdio->func1_stat.func1_unknow_int_count = 0;

    hi_sdio->msg[D2H_MSG_WAKEUP_SUCC].count = 0;
    hi_sdio->msg[D2H_MSG_ALLOW_SLEEP].count = 0;
    hi_sdio->msg[D2H_MSG_DISALLOW_SLEEP].count = 0;
    hi_sdio->msg[D2H_MSG_DEVICE_PANIC].count = 0;

    hi_sdio->ul_sdio_suspend               = 0;
    hi_sdio->ul_sdio_resume                = 0;
#endif
    wlan_pm->wakeup_succ = 0;
    wlan_pm->wakeup_dev_ack = 0;
    wlan_pm->wakeup_done_callback = 0;
    wlan_pm->wakeup_succ_work_submit = 0;
    wlan_pm->wakeup_gpio_up_cnt = 0;
    wlan_pm->wakeup_fail_wait_sdio = 0;
    wlan_pm->wakeup_fail_timeout = 0;
    wlan_pm->wakeup_fail_set_reg = 0;
    wlan_pm->wakeup_fail_submit_work = 0;

    wlan_pm->sleep_succ = 0;
    wlan_pm->sleep_feed_wdg_cnt = 0;
    wlan_pm->wakeup_done_callback = 0;
    wlan_pm->sleep_fail_set_reg = 0;
    wlan_pm->sleep_fail_wait_timeout = 0;
    wlan_pm->sleep_fail_forbid = 0;
    wlan_pm->sleep_work_submit = 0;
    wlan_pm->sleep_msg_send_cnt = 0;
    wlan_pm->sleep_gpio_low_cnt = 0;
    return;
}
