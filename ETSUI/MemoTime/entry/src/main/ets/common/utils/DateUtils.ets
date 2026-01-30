/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

/**
 * 日期时间工具函数
 */

/**
 * 格式化日期为 YYYY-MM-DD
 */
export function formatDate(date: Date): string {
  const year = date.getFullYear()
  const month = String(date.getMonth() + 1).padStart(2, '0')
  const day = String(date.getDate()).padStart(2, '0')
  return `${year}-${month}-${day}`
}

/**
 * 格式化时间为 HH:mm
 */
export function formatTime(date: Date): string {
  const hours = String(date.getHours()).padStart(2, '0')
  const minutes = String(date.getMinutes()).padStart(2, '0')
  return `${hours}:${minutes}`
}

/**
 * 格式化日期为中文格式 M月D日
 */
export function formatDateChinese(date: Date): string {
  const month = date.getMonth() + 1
  const day = date.getDate()
  return `${month}月${day}日`
}

/**
 * 格式化日期时间为完整格式
 */
export function formatDateTime(date: Date): string {
  return `${formatDate(date)} ${formatTime(date)}`
}



/**
 * 获取月份名称
 */
export function getMonthName(month: number): string {
  const monthNames = ['一月', '二月', '三月', '四月', '五月', '六月', 
                      '七月', '八月', '九月', '十月', '十一月', '十二月']
  return monthNames[month]
}

/**
 * 判断两个日期是否是同一天
 */
export function isSameDay(date1: Date, date2: Date): boolean {
  return date1.getFullYear() === date2.getFullYear() &&
         date1.getMonth() === date2.getMonth() &&
         date1.getDate() === date2.getDate()
}

/**
 * 判断是否是今天
 */
export function isToday(date: Date): boolean {
  return isSameDay(date, new Date())
}

/**
 * 获取某月的第一天
 */
export function getFirstDayOfMonth(year: number, month: number): Date {
  return new Date(year, month, 1)
}

/**
 * 获取某月的最后一天
 */
export function getLastDayOfMonth(year: number, month: number): Date {
  return new Date(year, month + 1, 0)
}

/**
 * 获取某月的天数
 */
export function getDaysInMonth(year: number, month: number): number {
  return new Date(year, month + 1, 0).getDate()
}

/**
 * 增加/减少月份
 */
export function addMonths(date: Date, months: number): Date {
  const result = new Date(date)
  result.setMonth(result.getMonth() + months)
  return result
}

/**
 * 增加/减少年份
 */
export function addYears(date: Date, years: number): Date {
  const result = new Date(date)
  result.setFullYear(result.getFullYear() + years)
  return result
}

/**
 * 增加/减少天数
 */
export function addDays(date: Date, days: number): Date {
  const result = new Date(date)
  result.setDate(result.getDate() + days)
  return result
}

/**
 * 获取一周的开始日期（周日）
 */
export function getStartOfWeek(date: Date): Date {
  const result = new Date(date)
  const day = result.getDay()
  result.setDate(result.getDate() - day)
  return result
}

/**
 * 获取一周的结束日期（周六）
 */
export function getEndOfWeek(date: Date): Date {
  const result = new Date(date)
  const day = result.getDay()
  result.setDate(result.getDate() + (6 - day))
  return result
}

/**
 * 获取相对时间描述
 */
export function getRelativeTimeString(date: Date): string {
  const now = new Date()
  const diff = date.getTime() - now.getTime()
  const diffDays = Math.floor(diff / (1000 * 60 * 60 * 24))
  
  if (diffDays === 0) {
    return '今天'
  } else if (diffDays === 1) {
    return '明天'
  } else if (diffDays === -1) {
    return '昨天'
  } else if (diffDays > 0 && diffDays <= 7) {
    return `${diffDays}天后`
  } else if (diffDays < 0 && diffDays >= -7) {
    return `${Math.abs(diffDays)}天前`
  } else {
    return formatDateChinese(date)
  }
}

/**
 * 获取星期几的中文名称（通过星期数字 0-6）
 */
export function getWeekDayName(dayOfWeek: number): string {
  const weekDays = ['日', '一', '二', '三', '四', '五', '六']
  return weekDays[dayOfWeek]
}

/**
 * 获取月视图需要显示的日期数组（包括上月和下月的填充日期）
 */
export function getMonthViewDates(year: number, month: number, startFromMonday: boolean): Date[] {
  const dates: Date[] = []
  const firstDay = new Date(year, month, 1)
  const lastDay = new Date(year, month + 1, 0)
  
  // 计算第一天是星期几
  let firstDayOfWeek = firstDay.getDay()
  if (startFromMonday) {
    firstDayOfWeek = firstDayOfWeek === 0 ? 6 : firstDayOfWeek - 1
  }
  
  // 添加上个月的日期
  for (let i = firstDayOfWeek - 1; i >= 0; i--) {
    const date = new Date(year, month, -i)
    dates.push(date)
  }
  
  // 添加当月的日期
  for (let i = 1; i <= lastDay.getDate(); i++) {
    dates.push(new Date(year, month, i))
  }
  
  // 添加下个月的日期，填充到6行
  const remainingDays = 42 - dates.length
  for (let i = 1; i <= remainingDays; i++) {
    dates.push(new Date(year, month + 1, i))
  }
  
  return dates
}

/**
 * 获取一周的日期数组
 */
export function getWeekDates(date: Date, startFromMonday: boolean): Date[] {
  const dates: Date[] = []
  const currentDay = date.getDay()
  
  let diff: number
  if (startFromMonday) {
    diff = currentDay === 0 ? -6 : 1 - currentDay
  } else {
    diff = -currentDay
  }
  
  for (let i = 0; i < 7; i++) {
    const newDate = new Date(date)
    newDate.setDate(date.getDate() + diff + i)
    dates.push(newDate)
  }
  
  return dates
}

/**
 * 获取某月第一天是星期几（返回0-6的数字）
 */
export function getFirstDayOfMonthWeekday(year: number, month: number): number {
  return new Date(year, month, 1).getDay()
}
