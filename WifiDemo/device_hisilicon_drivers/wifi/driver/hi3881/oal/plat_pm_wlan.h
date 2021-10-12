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

#ifndef __PLAT_PM_WLAN_H__
#define __PLAT_PM_WLAN_H__

/* ****************************************************************************
  1 Include other Head file
**************************************************************************** */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION) || (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#ifndef HAVE_PCLINT_CHECK
#include <linux/kernel.h>
#endif
#endif
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#include <linux/mmc/host.h>
#include <linux/mmc/sdio_func.h>
#include <linux/mmc/sdio.h>
#include <linux/fb.h>
#include <linux/mutex.h>
#endif
#include "oal_ext_if.h"
#include "oal_channel_host_if.h"

/* cmw need to be confirmed */
#define HOST_WAIT_BOTTOM_INIT_TIMEOUT   5000
#define HOST_WAIT_READY_INTERVAL        10
#define WLAN_WAKUP_MSG_WAIT_TIMEOUT     500
#define WLAN_SLEEP_MSG_WAIT_TIMEOUT     50
#define WLAN_FORSLP_MSG_WAIT_TIMEOUT    100       /* suspend wait forceslp work */
#define WLAN_SLP_MSG_WAIT_TIMEOUT       500       /* wdgtimeout wait slp work */
#define WLAN_RESUME_WAIT_TIMEOUT        3000
#define WLAN_POWEROFF_ACK_WAIT_TIMEOUT  1000
#define WLAN_OPEN_BCPU_WAIT_TIMEOUT     1000
#define WLAN_HALT_BCPU_TIMEOUT          1000
#define HOST_WAIT_FOR_DEV_DETACH        0xffffff

#define WLAN_SLEEP_TIMER_PERIOD         80        /* 睡眠定时器80ms定时 */
#define WLAN_SLEEP_DEFAULT_CHECK_CNT    5         /* 默认检查5次，即400ms */
#define WLAN_SLEEP_LONG_CHECK_CNT       10        /* 入网阶段,延长至400ms */
#define WLAN_SDIO_MSG_RETRY_NUM         3
#define WLAN_WAKEUP_FAIL_MAX_TIMES      1         /* 连续多少次wakeup失败，可进入DFR流程 */
#define WLAN_TRY_WAKEUP_FAIL_TIMES      3
#define WLAN_PM_MODULE                  "[wlan]"

#define WLAN_PM_SLPREQ_LEVEL            0
#define WLAN_PM_WKUPDEV_LEVEL           1
#define INVALID_IRQ                     (-1)
#define WK_FAIL_SIG 44
#define TIMEOUT_MUTIPLE_6 6

enum WLAN_PM_CPU_FREQ_ENUM {
    WLCPU_40MHZ     =   1,
    WLCPU_80MHZ     =   2,
    WLCPU_160MHZ    =   3,
    WLCPU_240MHZ    =   4,
    WLCPU_320MHZ    =   5,
    WLCPU_480MHZ    =   6,
};

enum WLAN_PM_SLEEP_STAGE {
    SLEEP_STAGE_INIT    = 0,  /* 初始 */
    SLEEP_REQ_SND       = 1,  /* leep request发送完成 */
    SLEEP_ALLOW_RCV     = 2,  /* 收到allow sleep response */
    SLEEP_DISALLOW_RCV  = 3,  /* 收到allow sleep response */
    SLEEP_CMD_SND       = 4,  /* 允许睡眠reg设置完成 */
};

enum WLAN_PM_SLP_REQ {
    NO_SLPREQ_STATUS     = 0,
    FORCE_SLP_STATUS     = 1,
    REQ_SLP_STATUS       = 2,
};

enum WLAN_PM_SLPREQ_ACK {
    SLPACK_DEV_ALLOW     = 0,
    SLPACK_DEV_DISALLOW  = 1,
};

enum WLAN_PM_REUSME_WIFI_MODE {
    REUSME_WIFI_NO   = 0,
    REUSME_WIFI_AP   = 1,
    REUSME_WIFI_STA  = 2,
};

/* 睡眠模块ID 枚举，每个id占用1个bit，继续扩展时要注意最大为(1 << 31)。 */
typedef enum {
    HI_PM_ID_AP = 0,   /* AP 模块 Id */
    HI_PM_ID_STA = 1,  /* STA 模块 Id  */
    HI_PM_ID_RSV = 5,  /* 内部预留 Id 5 */
    HI_PM_ID_MAX = 31, /* ID不可大于 31 */
} wlan_pm_vote_id;

/* ****************************************************************************
  3 STRUCT DEFINE
**************************************************************************** */
typedef hi_u32 (*wifi_srv_get_pm_pause_func)(hi_void);
typedef void (*hi_wifi_wkup_fail_cb)(void);

struct wifi_srv_callback_handler {
    wifi_srv_get_pm_pause_func p_wifi_srv_get_pm_pause_func;
};

struct wlan_pm_info {
    struct BusDev           *bus;
    unsigned long           wlan_pm_enable;          /* pm使能开关 */
    unsigned long           wlan_power_state;        /* wlan power on state */
    unsigned long           apmode_allow_pm_flag;    /* ap模式下，是否允许下电操作,1:允许,0:不允许 */

    volatile unsigned long  wlan_dev_state;          /* wlan sleep state */
    volatile unsigned long  wlan_host_state;         /* wlan host sleep state */
    volatile hi_u32         vote_status;             /* 0:sleep 1: work，每一bit代表一个投票 */

    oal_workqueue_stru     *pm_wq;                  /* pm work quque */
    oal_work_stru           wakeup_work;             /* 唤醒work */
    oal_work_stru           sleep_work;              /* sleep work */

    oal_timer_list_stru     watchdog_timer;          /* sleep watch dog */
    hi_u32                  packet_cnt;              /* 睡眠周期内统计的packet个数 */
    hi_u32                  wdg_timeout_cnt;         /* timeout check cnt */
    hi_u32                  wdg_timeout_curr_cnt;    /* timeout check current cnt */
    hi_u32                  slpreq_flag;
    hi_u32                  slpack;

    oal_completion          close_done;
    oal_completion          device_ready;
    oal_completion          wakeup_done;
    oal_completion          sleep_request_ack;
    oal_completion          host_sleep_request_ack;

    struct wifi_srv_callback_handler st_wifi_srv_handler;

    /* 维测统计 */
    hi_u32                  open_cnt;
    hi_u32                  close_cnt;
    hi_u32                  close_done_callback;
    hi_u32                  wakeup_succ;
    hi_u32                  wakeup_succ_work_submit;
    hi_u32                  wakeup_dev_ack;
    hi_u32                  wakeup_done_callback;
    hi_u32                  wakeup_fail_wait_sdio;
    hi_u32                  wakeup_fail_timeout;
    hi_u32                  wakeup_fail_set_reg;
    hi_u32                  wakeup_fail_submit_work;
    hi_u32                  wakeup_gpio_up_cnt;

    hi_u32                  sleep_succ;
    hi_u32                  sleep_feed_wdg_cnt;
    hi_u32                  sleep_start_wdg_timer_cnt;
    hi_u32                  sleep_fail_request;
    hi_u32                  sleep_fail_wait_timeout;
    hi_u32                  sleep_fail_set_reg;
    hi_u32                  sleep_fail_forbid;
    hi_u32                  sleep_work_submit;
    hi_u32                  sleep_msg_send_cnt;
    hi_u32                  sleep_dev_ack_cnt;
    hi_u32                  sleep_gpio_low_cnt;
};

typedef struct {
    hi_u32  wakeup_times;                   /**< wakeup times.CNcomment:唤醒次数统计 CNend */
    hi_u32  sleep_threshold_refuse_times;  /**< sleep threshold refuse times.CNcomment:
                                                可睡时间小于设定门限次数统计 CNend */
    hi_u32  sleep_check_refuse_times;       /**< sleep check refuse times.CNcomment:
                                                入睡投票睡眠失败次数统计 CNend */
    hi_u32  sleep_times;                   /**< sleep times.CNcomment:入睡次数统计 CNend */
    hi_u32  sleep_threshold;               /**< sleep threshold, unit is ms, only when
                                                the system' Remaining idle time is bigger than the threshold,
                                                system can enter deep sleep state.CNcomment:深睡眠门限，单位
                                                为ms，当系统剩余空闲时间大于该门限时，方可进入深睡 CNend */
    hi_u32  dsleep_fail_times; /**< the times of power off fail during deepsleep.CNcomment:深睡下电失败次数统计 CNend */
    hi_u8   type;                  /**< hi_lpc_type type, enable low power management.
                                         CNcomment:hi_lp_type类型，低功耗类型 CNend */
    hi_u8   evt_sts;                 /**< sleep event state.CNcomment:唤醒事件状态 CNend */
    hi_u8   int_sts;                 /**< sleep interrupt state.CNcomment:唤醒中断状态 CNend */
    hi_u8   last_time_vote_state;    /**< last time vote state, 0:no sleep, 1: light sleep, 2: deep sleep.
                                        CNcomment:最近一次睡眠状态，0:没有入睡，1:浅睡，2:深睡。 CNend */
    hi_u32  timer_ticks; /**< the time ticks is about to expire, unit is 10 ms, if the value is 0xffffffff, there is
                                no timer that is about to expire.
                            CNcomment:定时器即将到期的时间，单位为10ms；如果是0xffffffff，表示没有即将到期的
                            定时器 CNend */
    hi_u32  timer_handle; /**< the callback function address of the timer that is about to expire. if the value is
                                0xffffffff, it means that there is no timer that is about to expire.
                            CNcomment:即将到期定时器回调函数的地址，如果是0xffffffff,表示没有即将到期的定时器 CNend */
    hi_u32  timer_handle_arg; /**< the parameter of the timer callback function, if the value is 0xffffffff, it means
                                there is no timer to expire.CNcomment:定时器回调函数的参数，如果是0xffffffff,
                                表示没有即将到期的定时器 CNend */
    hi_u32  task_ticks;  /**< the task ticks is about to expire, unit is 10 ms.CNcomment:任务即将到期的时间，
                            单位为10ms。CNend */
    hi_u32  task_id; /**< the task id that is about to expire.CNcomment:即将到期任务的ID。CNend */
    hi_u32  sleep_ticks; /**< last sleep time ticks, unit is 10 ms.CNcomment:最近一次睡眠的tick时间，
                            单位为10ms。CNend */
    hi_u32  veto_info;   /**< veto_info.CNcomment:投票否决睡眠信息，详见hi_lpc_id。CNend */
    hi_u16  dsleep_wk_gpio; /**< wakeup gpio for deep sleep.CNcomment:唤醒的GPIO,该值(1<<x)为1表示GPIOx使能。CNend */
    hi_u16  sdio_busy : 1;
    hi_u16  host_sleeped : 1;
    hi_u16  host_allow_dev_sleep : 1;
    hi_u16  sdio_allow_sleep : 1;
    hi_u16  reserve : 12;  /* reserve 12 bits */
    hi_u32  refuse_vote_handle; /**< Address of voting funcfion refusing to vote for sleep.CNcomment:
                                    拒绝投睡眠票的投票函数地址。CNend */
    hi_u32 host_allow_device_slp_times;
    hi_u32 sdio_wkup_int_times;
    hi_u32 device_sdio_wkup_ack_times;
    hi_u32 sdio_busy_times;
    hi_u32 host_vote_work_times;
    hi_u32 sdio_vote_work_times;
} hi_device_lpc_info;

/* ****************************************************************************
  5 EXTERN FUNCTION
**************************************************************************** */
struct wlan_pm_info *wlan_pm_get_drv(hi_void);
void wlan_pm_dump_host_info(void);
void wlan_pm_dump_device_info(void);
unsigned long wlan_pm_exit(hi_void);
hi_s32 wlan_pm_is_poweron(hi_void);
hi_s32 wlan_pm_is_shutdown(hi_void);
hi_u32 wlan_pm_open(hi_void);
hi_u32 wlan_pm_close(hi_void);
unsigned long wlan_pm_wakeup_dev(hi_void);
unsigned long wlan_pm_wakeup_host(void);
unsigned long wlan_pm_state_get(void);
hi_u32 hi_wifi_plat_pm_enable(hi_void);
hi_u32 hi_wifi_plat_pm_disable(hi_void);
hi_s32 wlan_pm_disable_check_wakeup(hi_s32 flag);
hi_u32 wlan_pm_set_pm_sts_exception(hi_void);
struct wifi_srv_callback_handler *wlan_pm_get_wifi_srv_handler(hi_void);
hi_void wlan_pm_wakeup_dev_ack(hi_void);
hi_void wlan_pm_set_timeout(hi_u32 timeout);
hi_void wlan_pm_feed_wdg(hi_void);
hi_s32 wlan_pm_stop_wdg(hi_void);
void wlan_pm_wdg_timeout(struct wlan_pm_info *wlan_pm);
hi_void wlan_pm_state_set(struct wlan_pm_info *wlan_pm, unsigned long state);

void wlan_pm_info_clean(void);
hi_s32 wlan_device_mem_check(void);
hi_s32 wlan_device_mem_check_result(unsigned long long *time);

unsigned long wlan_pm_dev_sleep_request(void);
unsigned int hi_wifi_host_request_sleep(bool slp);
struct wlan_pm_info *wlan_pm_init(void);

/* 统计hcc中的数据包 */
hi_void wlan_pm_set_packet_cnt(hi_u32 delt);

hi_void wlan_pm_set_wkfail_pid(int pid);
hi_s32 hi_wifi_register_wkup_fail_process_handle(hi_wifi_wkup_fail_cb cb);
hi_u8 wlan_pm_is_disabling(void);
void wlan_pm_dump_device_pm_info(char *buf, int length);
hi_void hi_wlan_dump_pm_info(hi_u8 is_host);
/* 投票禁止睡眠 */
hi_void wlan_pm_add_vote(wlan_pm_vote_id id);
/* 解除对应id的禁止进入睡眠模式状态 */
hi_void wlan_pm_remove_vote(wlan_pm_vote_id id);

void set_device_is_ready(hi_u8 is_ready);

#ifdef _PRE_WLAN_PM_FEATURE_FORCESLP_RESUME
hi_void wlan_resume_state_set(hi_u32 ul_state);
hi_u32 wlan_resume_state_get(hi_void);
#endif

#endif
