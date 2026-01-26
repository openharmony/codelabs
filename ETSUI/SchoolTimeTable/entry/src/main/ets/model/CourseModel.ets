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
 */
export enum WeekDay {
  MONDAY = 1,
  TUESDAY = 2,
  WEDNESDAY = 3,
  THURSDAY = 4,
  FRIDAY = 5,
  SATURDAY = 6,
  SUNDAY = 7
}

export interface Course {
  id: string;
  name: string;
  location: string;
  teacher: string;
  day: WeekDay;
  startSection: number;
  endSection: number;
  color: string;
  remark: string;
  semester: string;
  startWeek: number;
  endWeek: number;
  credit: number;
}

export interface TimeSlot {
  section: number;
  startTime: string;
  endTime: string;
}

export interface Semester {
  id: string;
  name: string;
  startDate: string;
  endDate: string;
  currentWeek: number;
  totalWeeks: number;
}

export interface CourseStatistics {
  totalCourses: number;
  totalHours: number;
  coursesByDay: Map<WeekDay, number>;
}

export interface AppTheme {
  name: string;
  primaryColor: string;
  backgroundColor: string;
  cardBackgroundColor: string;
  textColor: string;
  secondaryTextColor: string;
}

export interface AppSettings {
  theme: AppTheme;
  currentSemester: string;
  customTimeSlots: TimeSlot[];
}

export interface ExportData {
  courses: Course[];
  semesters: Semester[];
  settings: AppSettings;
  exportDate: string;
}

export interface UserInfo {
  name: string;
  avatar: string;
  signature: string;
}
