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
