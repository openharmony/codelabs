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

/* *
 * @defgroup iot_time System Clock
 * @ingroup osa
 */
#ifndef __HI_TIME_H__
#define __HI_TIME_H__
#include <hi_types_base.h>

/* *
* @ingroup  iot_time
* @brief  Delay, in microseconds.CNcomment:延时，微秒级。CNend
*
* @par 描述:
*           Delay operation implemented by software based on the system clock, blocking the CPU.
CNcomment:延时操作，阻塞CPU。CNend
*
* @attention This API cannot be used for a long time in an interrupt.CNcomment:不允许中断中使用。CNend
*
* @param  us                [IN] type #hi_u32，delay period (unit: microsecond).
CNcomment:延时时间（单位：μs）。CNend
*
* @retval  None
* @par 依赖:
*            @li hi_time.h：Describes system time APIs.CNcomment:文件包含系统时间相关接口。CNend
* @see  None
* @since Hi3861_V100R001C00
*/
hi_void hi_udelay(hi_u32 us);

/* *
 * @ingroup  iot_time
 * @brief  Obtains the tick value of the system (32-bit).CNcomment:获取系统tick值（32bit）。CNend
 *
 * @par 描述:
 * Obtains the tick value of the system (32-bit).CNcomment:获取系统tick值（32bit）。CNend
 *
 * @attention None
 * @param None
 *
 * @retval #hi_u32 Tick value of the system.CNcomment:系统tick值。CNend
 * @par 依赖:
 * @li hi_time.h：Describes system time APIs.CNcomment:文件包含系统时间相关接口。CNend
 * @since Hi3861_V100R001C00
 */
hi_u32 hi_get_tick(hi_void);

/* *
* @ingroup  iot_time
* @brief  Obtains the tick value of the system (64-bit).CNcomment:获取系统tick值（64bit）。CNend
*
* @par 描述:
*           Obtains the tick value of the system (64-bit).CNcomment:获取系统tick值（64bit）。CNend
*
* @attention The hi_mdm_time.h file must be included where the API is called. Otherwise, the API is considered not
*            declared, and the tick value is returned as an int type, resulting in a truncation error.
CNcomment:该接口调用处必须包含头文件hi_time.h，否则按照未声明接口处理，会将tick值当做int类型返回，产生截断错误。CNend
* @param None
*
* @retval  #hi_u64 Tick value of the system.CNcomment:系统tick值。CNend
* @par 依赖:
*            @li hi_time.h：Describes system time APIs.CNcomment:文件包含系统时间相关接口。CNend
* @since Hi3861_V100R001C00
*/
hi_u64 hi_get_tick64(hi_void);


#if LINUX_VERSION_CODE < KERNEL_VERSION(5,0,0)
/* *
 * @ingroup  iot_time
 * @brief  Obtains the system time (unit: s).CNcomment:获取系统时间（单位：s）。CNend
 *
 * @par 描述:
 * Obtains the system time (unit: s).CNcomment:获取系统时间（单位：s）。CNend
 *
 * @attention None
 * @param None
 *
 * @retval #hi_u32 System time.CNcomment:系统时间。CNend
 * @retval #HI_ERR_FAILURE failed to be obtained. CNcomment:获取时间失败。CNend
 * @par 依赖:
 * @li hi_time.h：Describes system time APIs.CNcomment:文件包含系统时间相关接口。CNend
 * @since Hi3861_V100R001C00
 */
hi_u32 hi_get_seconds(hi_void);

/* *
 * @ingroup  iot_time
 * @brief  Obtains the system time (unit: us).CNcomment:获取系统时间（单位：us）。CNend
 *
 * @par 描述:
 * Obtains the system time (unit: us).CNcomment:获取系统时间（单位：us）。CNend
 *
 * @attention None
 * @param None
 *
 * @retval #hi_u64 System time.CNcomment:系统时间。CNend
 * @retval #HI_ERR_FAILURE failed to be obtained. CNcomment:获取时间失败。CNend
 * @par 依赖:
 * @li hi_time.h：Describes system time APIs.CNcomment:文件包含系统时间相关接口。CNend
 * @since Hi3861_V100R001C00
 */
hi_u64 hi_get_us(hi_void);

/* *
 * @ingroup  iot_time
 * @brief  Obtains the real time of the system (unit: s).CNcomment:获取系统实时时间（单位：s）。CNend
 *
 * @par 描述:
 * Obtains the real time of the system (unit: s).CNcomment:获取系统实时时间（单位：s）。CNend
 *
 * @attention None
 * @param None
 *
 * @retval #hi_u32 Real time of the system.CNcomment: 系统实时时间。CNend
 * @retval #HI_ERR_FAILURE failed to be obtained. CNcomment:获取时间失败。CNend
 * @par 依赖:
 * @li hi_time.h：Describes system time APIs.CNcomment:文件包含系统时间相关接口。CNend
 * @since Hi3861_V100R001C00
 */
hi_u32 hi_get_real_time(hi_void);

/* *
* @ingroup  iot_time
* @brief  Sets the real time of the system.CNcomment:设置系统实时时间。CNend
*
* @par 描述:
*           Sets the real time of the system.CNcomment:设置系统实时时间。CNend
*
* @attention None
* @param  seconds            [IN] type #hi_u32，set the real time of the system to this value.
CNcomment:将系统实时时间设置为该值。CNend
*
* @retval #HI_ERR_SUCCESS    Success.
* @retval #HI_ERR_FAILURE    Failure.
* @par 依赖:
*            @li hi_time.h：Describes system time APIs.CNcomment:文件包含系统时间相关接口。CNend
* @since Hi3861_V100R001C00
*/
hi_u32 hi_set_real_time(hi_u32 seconds);
#endif

#endif
