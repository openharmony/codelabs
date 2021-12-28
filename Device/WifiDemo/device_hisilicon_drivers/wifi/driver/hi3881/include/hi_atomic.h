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

#ifndef __HI_ATOMIC_H__
#define __HI_ATOMIC_H__
#include <hi_types.h>
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#include <los_hwi.h>
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    volatile hi_s32 counter;
} hi_atomic;

#define hi_atomic_init(i)            { (i) }
#define hi_atomic_read(v)            ((v)->counter)
#define hi_atomic_set(v, i)          (((v)->counter) = (i))

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#define hi_atomic_inc(v)             hi_atomic_add_return(1, v)
#define hi_atomic_dec(v)             hi_atomic_sub_return(1, v)

#define hi_atomic_inc_return(v)      (hi_atomic_add_return(1, v))
#define hi_atomic_dec_return(v)      (hi_atomic_sub_return(1, v))
#define hi_atomic_inc_return_optimize(v)      (hi_atomic_add_return_optimize(1, v))
#elif (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#define hi_atomic_inc(v)             atomic_add_return(1, (atomic_t *)v)
#define hi_atomic_dec(v)             atomic_sub_return(1, (atomic_t *)v)
#define hi_atomic_inc_return(v)      (atomic_add_return(1, (atomic_t *)v))
#define hi_atomic_dec_return(v)      (atomic_sub_return(1, (atomic_t *)v))
#endif /* _PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION */

/* *
 * @ingroup  iot_atomic
 * If the atomic operation is performed, the operation result is returned.
CNcomment:原子加操作，返回操作结果CNend
 */
#define hi_atomic_add_return_op(i, v) (hi_atomic_add_return(i, v))

/* *
 * @ingroup  iot_atomic
 * The operation result is returned when the atomic subtraction operation is performed.
CNcomment:原子减操作，返回操作结果CNend
 */
#define hi_atomic_sub_return_op(i, v) (hi_atomic_sub_return(i, v))

/* *
 * @ingroup  iot_atomic
 * The specified bit in the atomic setting variable is 1.
CNcomment:原子设置变量中指定bit位为1CNend
 */
#define hi_atomic_bit_set_op(bit, v) (hi_atomic_bit_set(bit, v))
/* *
 * @ingroup  iot_atomic
 * The specified bit in the atomic setting variable is 0.
CNcomment:原子设置变量中指定bit位为0CNend
 */
#define hi_atomic_bit_clear_op(bit, v) (hi_atomic_bit_clear(bit, v))

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
/* *
* @ingroup  iot_atomic
* @brief   If the atomic operation is performed, the operation result is returned.
CNcomment:原子加操作，返回操作结果CNend
*
* @par 描述:
*          If the atomic operation is performed, the operation result is returned.
CNcomment:原子加操作，返回操作结果CNend
*
* @attention None
* @param  i     [IN] type #hi_s32， The number of operands added to an atom.CNcomment:与原子相加的操作数CNend
* @param  v     [IN] type #hi_atomic*，Pointer to the atomic structure address.CNcomment:原子结构地址指针CNend
*
* @retval #hi_s32  Add Operation Result. CNcomment:加操作结果CNend
* @par 依赖:
*           @li hi_atomic.h：Header file where the interface declaration is located.
CNcomment:该接口声明所在的头文件。CNend
* @see  None
* @since Hi3861_V100R001C00
*/
static inline hi_s32 hi_atomic_add_return(hi_s32 i, hi_atomic *v)
{
    hi_u32 irq_status;

    irq_status = LOS_IntLock();
    v->counter += i;
    (hi_void)LOS_IntRestore(irq_status);
    return v->counter;
}

/* *
* @ingroup  iot_atomic
* @brief   The operation result is returned when the atomic subtraction operation is performed.
CNcomment:原子减操作，返回操作结果CNend
*
* @par 描述:
*          The operation result is returned when the atomic subtraction operation is performed.
CNcomment:原子减操作，返回操作结果CNend
*
* @attention None
* @param  i     [IN] type #hi_s32， The number of operands subtracted from the atom.
CNcomment:被原子相减的操作数CNend
* @param  v     [IN] type #hi_atomic*，Pointer to the atomic structure address.CNcomment:原子结构地址指针CNend
*
* @retval #hi_s32 Reduce the operation result. CNcomment:减操作结果CNend
* @par 依赖:
*           @li hi_atomic.h：Header file where the interface declaration is located.
CNcomment:该接口声明所在的头文件。CNend
* @see  None
* @since Hi3861_V100R001C00
*/
static inline hi_s32 hi_atomic_sub_return(hi_s32 i, hi_atomic *v)
{
    hi_u32 irq_status;

    irq_status = LOS_IntLock();
    v->counter = v->counter - i;
    (hi_void)LOS_IntRestore(irq_status);

    return v->counter;
}

/* *
* @ingroup  iot_atomic
* @brief   The specified bit in the atomic setting variable is 1.CNcomment:原子设置变量中指定bit位为1CNend
*
* @par 描述:
*          The specified bit in the atomic setting variable is 1.CNcomment:原子设置变量中指定bit位为1CNend
*
* @attention None
* @param  bit     [IN] type #hi_s32， Position of the bit that is set to 1. The value range is 0-31.
CNcomment:被置1的bit位置，范围0-31.CNend
* @param  value   [IN] type #hi_u32*，Address pointer of the set variable.CNcomment:置位变量的地址指针CNend
*
* @retval #None
* @par 依赖:
*           @li hi_atomic.h：Header file where the interface declaration is located.
CNcomment:该接口声明所在的头文件。CNend
* @see  None
* @since Hi3861_V100R001C00
*/
static inline hi_void hi_atomic_bit_set(hi_s32 bit, volatile hi_u32 *value)
{
    hi_u32 irq_status;
    irq_status = LOS_IntLock();

    *value |= (1 << bit);

    (hi_void)LOS_IntRestore(irq_status);
}

/* *
* @ingroup  iot_atomic
* @brief   The specified bit in the atomic setting variable is 0.CNcomment:原子设置变量中指定bit位为0CNend
*
* @par 描述:
*          The specified bit in the atomic setting variable is 0.CNcomment:原子设置变量中指定bit位为0CNend
*
* @attention None
* @param  bit     [IN] type #hi_s32， Position of the bit that is set to 0. The value range is 0-31.
CNcomment:被置0的bit位置，范围0-31.CNend
* @param  value   [IN] type #hi_u32*，Address pointer of the set variable.CNcomment:置位变量的地址指针CNend
*
* @retval #None
* @par 依赖:
*           @li hi_atomic.h：Header file where the interface declaration is located.
CNcomment:该接口声明所在的头文件。CNend
* @see  None
* @since Hi3861_V100R001C00
*/
static inline hi_void hi_atomic_bit_clear(hi_s32 bit, volatile hi_u32 *value)
{
    hi_u32 irq_status;
    hi_u32 mask;

    irq_status = LOS_IntLock();
    mask = 1 << bit;
    *value = (*value) & (~mask);

    (hi_void)LOS_IntRestore(irq_status);
}

__attribute__((always_inline)) static inline hi_s32 hi_atomic_add_return_optimize(hi_s32 i, hi_atomic *v)
{
    hi_u32 irq_status;

    irq_status = LOS_IntLock();
    v->counter += i;
    (hi_void)LOS_IntRestore(irq_status);
    return v->counter;
}
#endif /* #if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION) */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of hi_atomic.h */
