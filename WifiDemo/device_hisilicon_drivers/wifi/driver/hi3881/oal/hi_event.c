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
#include <hi_types_base.h>
#include <los_memory.h>
#include <los_event.h>
#include <los_hwi.h>
#include <hi_event.h>
#include <hi_stdlib.h>

#define HI_EVENT_SUPPORT_BITS 0X00FFFFFF /* LITEOS本身限制EVENT不支持25bit,VXWORKS仅支持低24bit。 */
#define RSV_MAX_SIZE 3

typedef struct {
    hi_bool use;
    hi_u8 rsv[RSV_MAX_SIZE];
    EVENT_CB_S os_event;
} hi_event_item;

typedef struct {
    hi_u8 init;          /* 模块是否初始化 */
    hi_u8 max_event_cnt; /* 模块最多支持多少个event */
    hi_u8 used_count;
    hi_u8 pad;
    hi_event_item *item; /* 指向item数组 */
} hi_event_ctrl;

hi_event_ctrl g_event_ctrl = {
    0,
};

extern hi_u32 ms2systick(HI_IN hi_u32 ms, HI_IN hi_bool include0);

hi_u8 osa_event_get_usage(hi_void)
{
    return g_event_ctrl.used_count;
}

hi_u32 hi_event_init(hi_u8 max_event_cnt, hi_pvoid event_space)
{
    if (!g_event_ctrl.init) {
        if (max_event_cnt == 0) {
            return HI_ERR_EVENT_INVALID_PARAM;
        }

        if (event_space) {
            g_event_ctrl.item = (hi_event_item *)event_space;
        } else {
            g_event_ctrl.item = (hi_event_item *)LOS_MemAlloc(m_aucSysMem0, sizeof(hi_event_item) * max_event_cnt);
        }

        if (g_event_ctrl.item == HI_NULL) {
            return HI_ERR_EVENT_NOT_ENOUGH_MEMORY;
        }

        memset_s(g_event_ctrl.item, sizeof(hi_event_item) * max_event_cnt, 0, sizeof(hi_event_item) * max_event_cnt);
        g_event_ctrl.max_event_cnt = max_event_cnt;
        g_event_ctrl.init = HI_TRUE;

        return HI_ERR_SUCCESS;
    }

    return HI_ERR_EVENT_RE_INIT;
}

hi_u32 hi_event_create(HI_OUT hi_u32 *id)
{
    hi_u32 ret;
    hi_u32 i = 0;
    hi_u32 int_save = 0;
    hi_event_item *item = HI_NULL;
    hi_u32 temp_event_id = HI_INVALID_EVENT_ID;

    if (!g_event_ctrl.init) {
        return HI_ERR_EVENT_NOT_INIT;
    }

    if ((HI_NULL == id)) {
        return HI_ERR_EVENT_INVALID_PARAM;
    }

    int_save = LOS_IntLock();

    for (i = 0; i < g_event_ctrl.max_event_cnt; i++) {
        item = &g_event_ctrl.item[i];

        if (HI_FALSE == item->use) {
            item->use = HI_TRUE;
            temp_event_id = i;
            break;
        }
    }

    LOS_IntRestore(int_save);

    if (temp_event_id == HI_INVALID_EVENT_ID) {
        return HI_ERR_EVENT_CREATE_NO_HADNLE;
    }

    if (item != HI_NULL) {
        ret = LOS_EventInit(&(item->os_event));

        if (ret != LOS_OK) {
            return HI_ERR_EVENT_CREATE_SYS_FAIL;
        }
    }

    *id = temp_event_id;
    g_event_ctrl.used_count++;
    return HI_ERR_SUCCESS;
}

hi_u32 hi_event_send(hi_u32 id, hi_u32 event_bits)
{
    hi_u32 ret;
    hi_event_item *item = HI_NULL;

    if (hi_unlikely(!g_event_ctrl.init)) {
        return HI_ERR_EVENT_NOT_INIT;
    }

    if (hi_unlikely(id >= g_event_ctrl.max_event_cnt)) {
        return HI_ERR_EVENT_INVALID_PARAM;
    }

    if (hi_unlikely(event_bits & (~HI_EVENT_SUPPORT_BITS))) {
        return HI_ERR_EVENT_INVALID_PARAM;
    }

    item = &g_event_ctrl.item[id];

    if (hi_unlikely(item->use == HI_FALSE)) {
        return HI_ERR_EVENT_INVALID_PARAM;
    }

    event_bits = (event_bits & HI_EVENT_SUPPORT_BITS);
    ret = LOS_EventWrite(&(item->os_event), event_bits);
    if (hi_unlikely(ret != LOS_OK)) {
        return HI_ERR_EVENT_SEND_FAIL;
    }

    return HI_ERR_SUCCESS;
}

hi_u32 hi_event_wait(hi_u32 id, hi_u32 mask, HI_OUT hi_u32 *event_bits, hi_u32 timeout, hi_u32 flag)
{
    hi_event_item *item = HI_NULL;
    hi_u32 tick = HI_SYS_WAIT_FOREVER;
    hi_u32 tmp_bits = 0;

    if (hi_unlikely(!g_event_ctrl.init)) {
        return HI_ERR_EVENT_NOT_INIT;
    }

    if (hi_unlikely((id >= g_event_ctrl.max_event_cnt) || (event_bits == HI_NULL))) {
        return HI_ERR_EVENT_INVALID_PARAM;
    }

    item = &g_event_ctrl.item[id];

    if (hi_unlikely(item->use == HI_FALSE)) {
        return HI_ERR_EVENT_INVALID_PARAM;
    }

    if (hi_unlikely(mask & (~HI_EVENT_SUPPORT_BITS))) {
        return HI_ERR_EVENT_INVALID_PARAM;
    }

    if (hi_unlikely(HI_SYS_WAIT_FOREVER != timeout)) {
        tick = ms2systick(timeout, HI_TRUE);
    }

    tmp_bits = LOS_EventRead(&(item->os_event), mask, flag, tick);

    if (hi_unlikely(tmp_bits == LOS_ERRNO_EVENT_READ_TIMEOUT)) {
        return HI_ERR_EVENT_WAIT_TIME_OUT;
    }

    if (hi_unlikely(!tmp_bits || (tmp_bits & LOS_ERRTYPE_ERROR))) {
        return HI_ERR_EVENT_WAIT_FAIL;
    } else {
        *event_bits = tmp_bits;
        return HI_ERR_SUCCESS;
    }
}

hi_u32 hi_event_clear(hi_u32 id, hi_u32 event_bits)
{
    hi_u32 ret;
    hi_event_item *item = HI_NULL;

    if (hi_unlikely(!g_event_ctrl.init)) {
        return HI_ERR_EVENT_NOT_INIT;
    }

    if (hi_unlikely(id >= g_event_ctrl.max_event_cnt)) {
        return HI_ERR_EVENT_INVALID_PARAM;
    }

    item = &g_event_ctrl.item[id];

    if (hi_unlikely(item->use == HI_FALSE)) {
        return HI_ERR_EVENT_INVALID_PARAM;
    }

    if (hi_unlikely(event_bits & (~HI_EVENT_SUPPORT_BITS))) {
        return HI_ERR_EVENT_INVALID_PARAM;
    }

    event_bits = (event_bits & HI_EVENT_SUPPORT_BITS);

    ret = LOS_EventClear(&(item->os_event), ~event_bits);
    if (hi_unlikely(ret != LOS_OK)) {
        return HI_ERR_EVENT_CLEAR_FAIL;
    }

    return HI_ERR_SUCCESS;
}

hi_u32 hi_event_delete(hi_u32 id)
{
    hi_u32 ret;
    hi_event_item *item = HI_NULL;

    if (!g_event_ctrl.init) {
        return HI_ERR_EVENT_NOT_INIT;
    }

    if (id >= g_event_ctrl.max_event_cnt) {
        return HI_ERR_EVENT_INVALID_PARAM;
    }

    item = &g_event_ctrl.item[id];

    if (item->use == HI_FALSE) {
        return HI_ERR_EVENT_INVALID_PARAM;
    }

    ret = LOS_EventDestroy(&(item->os_event));
    if (ret != LOS_OK) {
        return HI_ERR_EVENT_DELETE_FAIL;
    }

    memset_s(item, sizeof(hi_event_item), 0x0, sizeof(hi_event_item));
    g_event_ctrl.used_count--;
    return HI_ERR_SUCCESS;
}
