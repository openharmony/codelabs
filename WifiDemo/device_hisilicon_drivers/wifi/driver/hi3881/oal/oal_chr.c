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
#include "oal_chr.h"
#include "oam_ext_if.h"
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#include "linux/miscdevice.h"
#include "oal_wait.h"
#include "oal_mutex.h"
#endif


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
hi_wifi_driver_event g_driver_event = UNKNOWN;
static atomic_t g_hisi_chardev_available = ATOMIC_INIT(1);
#define DEVNAME "hisi_wifi"
static hi_u8 g_driver_exit = HI_FALSE;
oal_wait_queue_head_stru g_read_wait_queue;
#define DEV_EXIT 0

hi_u32 hisi_sched_event(hi_wifi_driver_event event)
{
    if (atomic_read(&g_hisi_chardev_available) != 0) {
        oam_warning_log0(0, OAM_SF_ANY, "device is not open!\n");
        return HI_FAIL;
    }
    g_driver_event = event;
    hi_wait_queue_wake_up_interrupt(&g_read_wait_queue);
    return HI_SUCCESS;
}

static ssize_t hisi_read(struct file *file, char __user *buffer, size_t length, loff_t *offset)
{
    ssize_t copied = 0;
    if (length < sizeof(hi_wifi_driver_event)) {
        oam_warning_log0(0, OAM_SF_ANY, "hisi_read parameter error\n");
        return -EINVAL;
    }

    hi_wait_event_interruptible(g_read_wait_queue, (g_driver_event != UNKNOWN || g_driver_exit == HI_TRUE));

    if (g_driver_exit) {
        return -EINVAL;
    }

    copied = sizeof(hi_wifi_driver_event);
    copied -= copy_to_user(buffer, (hi_u8 *)&g_driver_event, copied);
    g_driver_event = UNKNOWN;
    return copied;
}

static int hisi_release(struct inode *inode, struct file *file)
{
    printk("hisi_release\n");
    atomic_inc(&g_hisi_chardev_available);
    return 0;
}

static long hisi_ioctl(struct file *filp, unsigned int command, unsigned long arg)
{
    int ret = 0;
    switch (command) {
        case DEV_EXIT: {
            g_driver_exit = HI_TRUE;
            hi_wait_queue_wake_up_interrupt(&g_read_wait_queue);
            break;
        }
        default:
            ret = -EINVAL;
            break;
    }
    return ret;
}

static int hisi_open(struct inode *inode, struct file *file)
{
    int err = 0;
    if (!atomic_dec_and_test(&g_hisi_chardev_available)) {
        err = -EBUSY;
    }

    if (err) {
        atomic_inc(&g_hisi_chardev_available);
    }
    hi_wait_queue_init_head(&g_read_wait_queue);
    g_driver_exit = HI_FALSE;
    return err;
}

static struct file_operations hisi_fops = {
    .owner = THIS_MODULE,
    .read  = hisi_read,
    .open  = hisi_open,
    .unlocked_ioctl = hisi_ioctl,
    .release = hisi_release,
};

static struct miscdevice hisi_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVNAME,
    .fops = &hisi_fops,
    .nodename = DEVNAME
};

hi_u32 oal_register_ioctl(hi_void)
{
    if (misc_register(&hisi_device)) {
        return HI_FAIL;
    }
    return HI_SUCCESS;
}

#else
static hi_wifi_driver_event_cb g_fuc_callback = HI_NULL;

hi_u32 hisi_sched_event(hi_wifi_driver_event event)
{
    if (g_fuc_callback == HI_NULL) {
        return HI_FAIL;
    }
    return g_fuc_callback(event);
}

hi_u32 oal_register_ioctl(hi_wifi_driver_event_cb event_cb)
{
    g_fuc_callback = event_cb;
    return HI_SUCCESS;
}
#endif

hi_void oal_unregister_ioctl(hi_void)
{
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
    misc_deregister(&hisi_device);
#else
    g_fuc_callback = HI_NULL;
#endif
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
