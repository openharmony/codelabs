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

#include <hi_isr.h>
#include <los_hwi.h>
#include <linux/workqueue.h>
#include <los_exc.h>
#ifndef HAVE_PCLINT_CHECK
#include "encoding.h"
#endif


#define IRQ_WORD_LOW    0
#define IRQ_WORD_HIGH   1
#define IRQ_WORD_NUM    2
#define low_32bit(u64_data) ((hi_u32)((u64_data) & 0xFFFFFFFF))
#define high_32bit(u64_data) ((hi_u32)((((u64_data) >> 32) & 0xFFFFFFFF) << 32))
#define INT_DISABLE_BITS_ALL 0x0

volatile hi_u32 g_flash_irq_mask[IRQ_WORD_NUM] = {0};   /* flash处理时屏蔽中断, MACHINE_TIMER_IRQ对应的
                                                             系统timer中断默认不屏蔽 */
volatile hi_u32 g_flash_irq_backup[IRQ_WORD_NUM] = {0}; /* flash屏蔽前中断备份 */
volatile hi_u64 g_irq_force_mask_in_flash = 0; /* 中断服务程序不在flash中，可在flash操作过程中中断执行 */
volatile hi_u64 g_irq_force_unmask_in_flash = 0;

hi_u8 g_irq_prio_backup[OS_HWI_MAX_NUM] = {0}; /* irq priority backup for irq_enable */

hi_void disable_int_in_flash(hi_void)
{
    hi_u32 current_val;
    hi_u32 int_value;
    hi_u32 need_mask;

    int_value = LOS_IntLock();
    need_mask = (g_flash_irq_mask[IRQ_WORD_LOW] | low_32bit(g_irq_force_mask_in_flash));
    need_mask &= ~(low_32bit(g_irq_force_unmask_in_flash));
    if (need_mask) {
        g_flash_irq_backup[IRQ_WORD_LOW] = READ_CSR(mie);
        current_val = g_flash_irq_backup[IRQ_WORD_LOW];
        WRITE_CSR(mie, current_val & (~need_mask));
    }
    need_mask = (g_flash_irq_mask[IRQ_WORD_HIGH] | high_32bit(g_irq_force_mask_in_flash));
    need_mask &= ~(high_32bit(g_irq_force_unmask_in_flash));
    if (need_mask) {
        g_flash_irq_backup[IRQ_WORD_HIGH] = READ_CUSTOM_CSR(LOCIEN0);
        current_val = g_flash_irq_backup[IRQ_WORD_HIGH];
        WRITE_CUSTOM_CSR_VAL(LOCIEN0, current_val & (~need_mask));
    }
    LOS_IntRestore(int_value);
}

hi_void enable_int_in_flash(hi_void)
{
    hi_u32 current_val;
    hi_u32 int_value;
    hi_u32 need_mask;

    int_value = LOS_IntLock();
    need_mask = (g_flash_irq_mask[IRQ_WORD_LOW] | low_32bit(g_irq_force_mask_in_flash));
    need_mask &= ~(low_32bit(g_irq_force_unmask_in_flash));
    if (need_mask) {
        current_val = READ_CSR(mie);
        WRITE_CSR(mie, current_val | g_flash_irq_backup[IRQ_WORD_LOW]);
    }
    need_mask = (g_flash_irq_mask[IRQ_WORD_HIGH] | high_32bit(g_irq_force_mask_in_flash));
    need_mask &= ~(high_32bit(g_irq_force_unmask_in_flash));
    if (need_mask) {
        current_val = READ_CUSTOM_CSR(LOCIEN0);
        WRITE_CUSTOM_CSR_VAL(LOCIEN0, current_val | g_flash_irq_backup[IRQ_WORD_HIGH]);
    }
    LOS_IntRestore(int_value);
}

hi_void disable_all_ints(hi_void)
{
    WRITE_CSR(mie, INT_DISABLE_BITS_ALL);
    WRITE_CUSTOM_CSR_VAL(LOCIEN0, INT_DISABLE_BITS_ALL);
}

hi_u64 set_force_int_mask_in_flash(hi_u64 mask)
{
    hi_u32 int_value;
    int_value = LOS_IntLock();
    g_irq_force_mask_in_flash |= mask;
    g_irq_force_unmask_in_flash &= ~mask;
    LOS_IntRestore(int_value);
    return g_irq_force_mask_in_flash;
}

hi_u64 set_force_int_unmask_in_flash(hi_u64 unmask)
{
    hi_u32 int_value;
    int_value = LOS_IntLock();
    g_irq_force_unmask_in_flash |= unmask;
    g_irq_force_mask_in_flash &= ~unmask;
    LOS_IntRestore(int_value);
    return g_irq_force_unmask_in_flash;
}

hi_u32 hi_int_lock(hi_void)
{
    return LOS_IntLock();
}

hi_void hi_int_restore(hi_u32 int_value)
{
    LOS_IntRestore(int_value);
}

hi_u32 hi_irq_enable(hi_u32 vector)
{
    if (g_hook_osa.hi_irq_enable_hook != HI_NULL) {
        return g_hook_osa.hi_irq_enable_hook(vector);
    }

    if (vector >= OS_HWI_MAX_NUM) {
        return HI_ERR_ISR_INVALID_PARAM;
    }
    hi_u32 ret;

    ret = LOS_AdapIrqEnable(vector, g_irq_prio_backup[vector]);
    if (ret != LOS_OK) {
        if (ret == OS_ERRNO_HWI_NOT_CREATED) {
            return HI_ERR_ISR_NOT_CREATED;
        } else {
            return HI_ERR_ISR_ENABLE_IRQ_FAIL;
        }
    }
    return HI_ERR_SUCCESS;
}

hi_void hi_irq_disable(hi_u32 vector)
{
    if (g_hook_osa.hi_irq_disable_hook != HI_NULL) {
        g_hook_osa.hi_irq_disable_hook(vector);
        return;
    }

    if (vector >= OS_HWI_MAX_NUM) {
        return;
    }

    LOS_AdapIrqDisable(vector);
}

hi_void set_mask(hi_u32 vector)
{
    if (vector < OS_HIMIDEER_MIE_TOTAL_CNT) {
        g_flash_irq_mask[IRQ_WORD_LOW] |= 1 << vector;
    } else {
        g_flash_irq_mask[IRQ_WORD_HIGH] |= 1 << (vector - OS_HIMIDEER_MIE_TOTAL_CNT);
    }
}
hi_void reset_mask(hi_u32 vector)
{
    if (vector < OS_HIMIDEER_MIE_TOTAL_CNT) {
        g_flash_irq_mask[IRQ_WORD_LOW] &= ~(hi_u32)(1 << vector);
    } else {
        g_flash_irq_mask[IRQ_WORD_HIGH] &= ~(hi_u32)(1 << (vector - OS_HIMIDEER_MIE_TOTAL_CNT));
    }
}

hi_u32 hi_irq_request(hi_u32 vector, hi_u32 flags, irq_routine routine, uintptr_t param)
{
    hi_u32 ret;
    hi_u16 pri = flags & HI_IRQ_FLAG_PRI_MASK;

    if (g_hook_osa.hi_irq_request_hook != HI_NULL) {
        return g_hook_osa.hi_irq_request_hook(vector, flags, routine, param);
    }

    if ((routine == HI_NULL) || (vector >= OS_HWI_MAX_NUM) || (pri == 0)) {
        return HI_ERR_ISR_INVALID_PARAM;
    }
    if ((flags & HI_IRQ_FLAG_NOT_IN_FLASH) &&
        (((uintptr_t)routine >= HI_FLASH_BASE) && ((uintptr_t)routine < HI_FLASH_BASE + HI_FLASH_SIZE))) {
        return HI_ERR_ISR_IRQ_ADDR_NOK;
    }
    if ((flags & HI_IRQ_FLAG_NOT_IN_FLASH) == 0) {
        set_mask(vector);
    }

    g_irq_prio_backup[vector] = (hi_u8)pri;
    ret = LOS_HwiCreate(vector, pri, 0, (HWI_PROC_FUNC)routine, param);
    if (ret != LOS_OK) {
        if (ret == OS_ERRNO_HWI_ALREADY_CREATED) {
            return HI_ERR_ISR_ALREADY_CREATED;
        } else {
            return HI_ERR_ISR_REQ_IRQ_FAIL;
        }
    }

    return HI_ERR_SUCCESS;
}

hi_u32 hi_irq_free(hi_u32 vector)
{
    hi_u32 ret;

    if (g_hook_osa.hi_irq_free_hook != HI_NULL) {
        return g_hook_osa.hi_irq_free_hook(vector);
    }
    /* los接口内判断了vector合法性 */
    ret = LOS_HwiDelete(vector);
    reset_mask(vector);
    if (ret != LOS_OK) {
        return HI_ERR_ISR_DEL_IRQ_FAIL;
    }

    return HI_ERR_SUCCESS;
}

hi_bool hi_is_int_context(hi_void)
{
    if (g_hook_osa.hi_is_int_context_hook != HI_NULL) {
        return g_hook_osa.hi_is_int_context_hook();
    }

    return (hi_bool)OS_INT_ACTIVE;
}
