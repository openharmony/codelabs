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
#ifndef __LINUX_PM_H__
#define __LINUX_PM_H__

#include "linux/list.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef struct pm_message {
    int event;
} dev_pm_message_t;

struct dev_pm_info {
    dev_pm_message_t power_state;
    LOS_DL_LIST      entry;
};

#define PM_EVENT_INVALID        (-1)
#define PM_EVENT_ON             0x0000
#define PM_EVENT_FREEZE         0x0001
#define PM_EVENT_SUSPEND        0x0002
#define PM_EVENT_HIBERNATE      0x0004
#define PM_EVENT_QUIESCE        0x0008
#define PM_EVENT_RESUME         0x0010

#define PMSG_INVALID    ((struct pm_message){ .event = PM_EVENT_INVALID, })
#define PMSG_ON         ((struct pm_message){ .event = PM_EVENT_ON, })
#define PMSG_FREEZE     ((struct pm_message){ .event = PM_EVENT_FREEZE, })
#define PMSG_QUIESCE    ((struct pm_message){ .event = PM_EVENT_QUIESCE, })
#define PMSG_SUSPEND    ((struct pm_message){ .event = PM_EVENT_SUSPEND, })
#define PMSG_HIBERNATE  ((struct pm_message){ .event = PM_EVENT_HIBERNATE, })
#define PMSG_RESUME     ((struct pm_message){ .event = PM_EVENT_RESUME, })

int dpm_suspend_start(dev_pm_message_t state);
void dpm_resume_end(dev_pm_message_t state);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif

