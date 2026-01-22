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
  TimeSlot,
  Semester,
  CourseStatistics,
  AppSettings,
  AppTheme,
  ExportData,
  UserInfo
} from '../model/CourseModel';
import { ColorConstants } from '../model/ColorConstants';
import preferences from '@ohos.data.preferences';
import util from '@ohos.util';

// 定义全局状态的 Key
export const COURSE_LIST_KEY = 'GlobalCourseList';

export class CourseDataService {
  private static instance: CourseDataService;
  private preferences: preferences.Preferences | null = null;
  private courses: Course[] = [];
  private semesters: Semester[] = [];
  private settings: AppSettings = this.getDefaultSettings();
  private currentUser: string = '';
  private initPromise: Promise<void> | null = null;

  private constructor() {
  }

  static getInstance(): CourseDataService {
    if (!CourseDataService.instance) {
      CourseDataService.instance = new CourseDataService();
    }
    return CourseDataService.instance;
  }

  async init(context: Context): Promise<void> {
    // 保存 Context 到全局，以便后续重新初始化
    AppStorage.setOrCreate('appContext', context);

    if (this.preferences) {
      console.info('CourseDataService already initialized.');
      return;
    }

    if (this.initPromise) {
      console.info('CourseDataService initialization already in progress, waiting...');
      return this.initPromise;
    }

    this.initPromise = this.performInit(context);
    return this.initPromise;
  }

  private async performInit(context: Context): Promise<void> {
    try {
      console.info('Initializing CourseDataService preferences...');
      this.preferences = await preferences.getPreferences(context, 'course_schedule');
      console.info('Preferences initialized successfully.');
      
      this.currentUser = (await this.preferences.get('current_user', '')) as string;
      console.info(`Current user loaded: ${this.currentUser}`);
      
      await this.loadCourses(); // 加载课程到 AppStorage
      await this.loadSemesters();
      await this.loadSettings();
      await this.loadUserInfoToAppStorage();
      console.info('All data loaded successfully.');

    } catch (error) {
      console.error('Failed to initialize CourseDataService:', error);
      this.preferences = null;
    } finally {
      this.initPromise = null;
    }
  }

  private async loadUserInfoToAppStorage(): Promise<void> {
    const userInfo = await this.getUserInfo();
    AppStorage.setOrCreate('userName', userInfo.name);
    AppStorage.setOrCreate('userAvatar', userInfo.avatar);
    AppStorage.setOrCreate('userSignature', userInfo.signature);
  }

  private getUserKey(key: string): string {
    if (!this.currentUser) {
      return key;
    }
    return `${this.currentUser}_${key}`;
  }

  private async reloadData(): Promise<void> {
    await this.loadCourses();
    await this.loadSemesters();
    await this.loadSettings();
    await this.loadUserInfoToAppStorage();
  }

  private getDefaultSettings(): AppSettings {
    return {
      theme: {
        name: '默认',
        primaryColor: '#007DFF',
        backgroundColor: '#F5F5F5',
        cardBackgroundColor: '#FFFFFF',
        textColor: '#333333',
        secondaryTextColor: '#666666'
      },
      currentSemester: '',
      customTimeSlots: this.getDefaultTimeSlots()
    };
  }

  private getDefaultTimeSlots(): TimeSlot[] {
    return [
      { section: 1, startTime: '08:00', endTime: '08:45' },
      { section: 2, startTime: '08:55', endTime: '09:40' },
      { section: 3, startTime: '10:00', endTime: '10:45' },
      { section: 4, startTime: '10:55', endTime: '11:40' },
      { section: 5, startTime: '14:00', endTime: '14:45' },
      { section: 6, startTime: '14:55', endTime: '15:40' },
      { section: 7, startTime: '16:00', endTime: '16:45' },
      { section: 8, startTime: '16:55', endTime: '17:40' },
      { section: 9, startTime: '19:00', endTime: '19:45' },
      { section: 10, startTime: '19:55', endTime: '20:40' }
    ];
  }

  // private async loadCourses(): Promise<void> {
  //   if (!this.preferences) return;
  //   try {
  //     const coursesJson = await this.preferences.get('courses', '[]') as string;
  //     this.courses = JSON.parse(coursesJson);
  //   } catch (error) {
  //     console.error('Failed to load courses:', error);
  //     this.courses = [];
  //   }
  // }
  // --- 核心修改：加载课程并存入 AppStorage ---
  private async loadCourses(): Promise<void> {
    let courses: Course[] = [];
    if (this.preferences) {
      try {
        const key = this.getUserKey('courses');
        const coursesJson = await this.preferences.get(key, '[]') as string;
        courses = JSON.parse(coursesJson);
        
        // 数据清洗：检查并修复重复 ID
        const idSet = new Set<string>();
        let hasDuplicate = false;
        courses.forEach(course => {
          if (idSet.has(course.id)) {
            course.id = util.generateRandomUUID(true);
            hasDuplicate = true;
          }
          idSet.add(course.id);
        });
        
        // 如果发现重复并修复了，需要保存回去
        if (hasDuplicate && this.preferences) {
           await this.preferences.put(key, JSON.stringify(courses));
           await this.preferences.flush();
        }

      } catch (error) {
        console.error('Failed to load courses:', error);
      }
    }

    // 如果是第一次安装，没有任何数据，添加默认数据
    if (courses.length === 0) {
      courses = this.getSampleData();
      // 立即保存默认数据到本地
      if (this.preferences) {
        const key = this.getUserKey('courses');
        await this.preferences.put(key, JSON.stringify(courses));
        await this.preferences.flush();
      }
    }

    // 存入全局 AppStorage，UI 会自动感知这个 Key 的变化
    AppStorage.SetOrCreate(COURSE_LIST_KEY, courses);
    this.courses = courses; // Keep local sync as well
  }

  // --- 核心修改：保存课程 (从 AppStorage 获取最新数据持久化) ---
  private async saveCoursesToPreferences(courses: Course[]): Promise<void> {
    if (!this.preferences) return;
      try {
      const key = this.getUserKey('courses');
      await this.preferences.put(key, JSON.stringify(courses));
      await this.preferences.flush();
    } catch (error) {
      console.error('Failed to save courses:', error);
    }
  }

  // --- 获取当前所有的课程 (UI 组件建议直接用 @StorageLink) ---
  getAllCourses(): Course[] {
    return AppStorage.Get<Course[]>(COURSE_LIST_KEY) || [];
  }


  private async loadSemesters(): Promise<void> {
    if (!this.preferences) {
      return;
    }
    try {
      const key = this.getUserKey('semesters');
      const semestersJson = await this.preferences.get(key, '[]') as string;
      this.semesters = JSON.parse(semestersJson);
    } catch (error) {
      console.error('Failed to load semesters:', error);
      this.semesters = [];
    }
  }

  private async loadSettings(): Promise<void> {
    if (!this.preferences) {
      return;
    }
    try {
      const key = this.getUserKey('settings');
      const settingsJson = await this.preferences.get(key, '{}') as string;
      const loadedSettings = JSON.parse(settingsJson) as Partial<AppSettings>;
      const defaultSettings = this.getDefaultSettings();
      this.settings = {
        currentSemester: loadedSettings.currentSemester || defaultSettings.currentSemester,
        customTimeSlots: loadedSettings.customTimeSlots || defaultSettings.customTimeSlots,
        theme: defaultSettings.theme
      };
    } catch (error) {
      console.error('Failed to load settings:', error);
      this.settings = this.getDefaultSettings();
    }
  }

  private async saveCourses(): Promise<void> {
    // 兼容旧方法，调用新方法
    const courses = this.getAllCourses();
    await this.saveCoursesToPreferences(courses);
  }

  private async saveSemesters(): Promise<void> {
    if (!this.preferences) {
      return;
    }
    try {
      const key = this.getUserKey('semesters');
      await this.preferences.put(key, JSON.stringify(this.semesters));
      await this.preferences.flush();
    } catch (error) {
      console.error('Failed to save semesters:', error);
    }
  }

  private async saveSettings(): Promise<void> {
    if (!this.preferences) {
      return;
    }
    try {
      const key = this.getUserKey('settings');
      await this.preferences.put(key, JSON.stringify(this.settings));
      await this.preferences.flush();
    } catch (error) {
      console.error('Failed to save settings:', error);
    }
  }

  // getCourses(): Course[] {
  //   const result: Course[] = [];
  //   for (const course of this.courses) {
  //     result.push(course);
  //   }
  //   return result;
  // }
  // 修改 getCourses 方法，让它从 AppStorage 获取数据，而不是空的 this.courses
  getCourses(): Course[] {
    return this.getAllCourses(); // 直接调用你已经写好的 getAllCourses()
  }

  getCoursesByDay(day: WeekDay): Course[] {
    return this.getAllCourses().filter(course =>
      course.day === day
    );
  }

  // searchCourses(keyword: string): Course[] {
  //   if (!keyword.trim()) {
  //     return this.courses;
  //   }
  //   const lowerKeyword = keyword.toLowerCase();
  //   return this.courses.filter(course =>
  //     course.name.toLowerCase().includes(lowerKeyword) ||
  //     course.location.toLowerCase().includes(lowerKeyword) ||
  //     course.teacher.toLowerCase().includes(lowerKeyword) ||
  //     course.remark.toLowerCase().includes(lowerKeyword)
  //   );
  // }
  searchCourses(keyword: string): Course[] {
    const courses = this.getAllCourses(); // 获取最新数据
    if (!keyword.trim()) {
      return courses;
    }
    const lowerKeyword = keyword.toLowerCase();
    return courses.filter(course =>
    course.name.toLowerCase().includes(lowerKeyword) ||
    course.location.toLowerCase().includes(lowerKeyword) ||
    course.teacher.toLowerCase().includes(lowerKeyword) ||
    course.remark.toLowerCase().includes(lowerKeyword)
    );
  }

  // --- 增删改逻辑：先更新 UI (AppStorage)，再保存到磁盘 ---

  addCourse(course: Course): void {
    const currentList = this.getAllCourses();
    // 必须创建一个新数组引用，ArkUI 才能检测到变化
    const newList = [...currentList, course];

    AppStorage.Set(COURSE_LIST_KEY, newList); // 1. 触发 UI 刷新
    this.saveCoursesToPreferences(newList);   // 2. 异步保存
  }

  updateCourse(course: Course): void {
    const currentList = this.getAllCourses();
    const index = currentList.findIndex(c => c.id === course.id);
    if (index !== -1) {
      currentList[index] = course;
      const newList = [...currentList]; // 创建新引用
      AppStorage.Set(COURSE_LIST_KEY, newList);
      this.saveCoursesToPreferences(newList);
    }
  }

  deleteCourse(courseId: string): void {
    const currentList = this.getAllCourses();
    const newList = currentList.filter(c => c.id !== courseId);

    AppStorage.Set(COURSE_LIST_KEY, newList);
    this.saveCoursesToPreferences(newList);
  }

  async clearAllCourses(): Promise<void> {
    const newList: Course[] = [];
    AppStorage.SetOrCreate(COURSE_LIST_KEY, newList);
    await this.saveCoursesToPreferences(newList);
    
    // 标记为已初始化，防止被误判为首次安装而恢复默认数据
    if (this.preferences) {
      const isInitializedKey = this.getUserKey('is_initialized');
      await this.preferences.put(isInitializedKey, true);
      await this.preferences.flush();
    }

    this.courses = newList;
  }

  getCourseById(courseId: string): Course | undefined {
    return this.getAllCourses().find(c => c.id === courseId);
  }

  getTimeSlots(): TimeSlot[] {
    return [...this.settings.customTimeSlots];
  }

  async setTimeSlots(timeSlots: TimeSlot[]): Promise<void> {
    this.settings.customTimeSlots = timeSlots;
    await this.saveSettings();
  }

  getSemesters(): Semester[] {
    return [...this.semesters];
  }

  addSemester(semester: Semester): void {
    this.semesters.push(semester);
    this.saveSemesters();
  }

  updateSemester(semester: Semester): void {
    const index = this.semesters.findIndex(s => s.id === semester.id);
    if (index !== -1) {
      this.semesters[index] = semester;
      this.saveSemesters();
    }
  }

  deleteSemester(semesterId: string): void {
    this.semesters = this.semesters.filter(s => s.id !== semesterId);
    this.saveSemesters();
  }

  getCurrentSemester(): Semester | undefined {
    return this.semesters.find(s => s.id === this.settings.currentSemester);
  }

  setCurrentSemester(semesterId: string): void {
    this.settings.currentSemester = semesterId;
    this.saveSettings();
  }

  getSettings(): AppSettings {
    const result: AppSettings = {
      currentSemester: this.settings.currentSemester,
      customTimeSlots: this.settings.customTimeSlots,
      theme: this.settings.theme
    };
    return result;
  }

  updateSettings(settings: Partial<AppSettings>): void {
    if (settings.currentSemester !== undefined) {
      this.settings.currentSemester = settings.currentSemester;
    }
    if (settings.customTimeSlots !== undefined) {
      this.settings.customTimeSlots = settings.customTimeSlots;
    }
    if (settings.theme !== undefined) {
      this.settings.theme = settings.theme;
    }
    this.saveSettings();
  }

  // 统计方法改为直接接收数组参数，或者你可以让统计页面也使用 @StorageLink
  getStatistics(): CourseStatistics {
    const courses = this.getAllCourses();
    const stats: CourseStatistics = {
      totalCourses: courses.length,
      totalHours: 0,
      coursesByDay: new Map<WeekDay, number>(),
    };

    for (let i = 1; i <= 7; i++) {
      stats.coursesByDay.set(i as WeekDay, 0);
    }

    for (const course of courses) {
      const hours = (course.endSection - course.startSection + 1) * 0.75;
      stats.totalHours += hours;
      const dayCount = stats.coursesByDay.get(course.day) || 0;
      stats.coursesByDay.set(course.day, dayCount + 1);
    }
    return stats;
  }


  exportData(): string {
    const data: ExportData = {
      courses: this.courses,
      semesters: this.semesters,
      settings: this.settings,
      exportDate: new Date().toISOString()
    };
    return JSON.stringify(data, null, 2);
  }

  importData(jsonData: string): boolean {
    try {
      const data = JSON.parse(jsonData) as Partial<ExportData>;
      if (data.courses && Array.isArray(data.courses)) {
        // this.courses = data.courses;
        // this.saveCourses();
        // 导入时更新 AppStorage
        AppStorage.Set(COURSE_LIST_KEY, data.courses);
        this.saveCoursesToPreferences(data.courses);
      }
      if (data.semesters && Array.isArray(data.semesters)) {
        this.semesters = data.semesters;
        this.saveSemesters();
      }
      if (data.settings) {
        const defaultSettings = this.getDefaultSettings();
        this.settings = {
          currentSemester: data.settings.currentSemester || defaultSettings.currentSemester,
          customTimeSlots: data.settings.customTimeSlots || defaultSettings.customTimeSlots,
          theme: data.settings.theme || defaultSettings.theme
        };
        this.saveSettings();
      }
      return true;
    } catch (error) {
      console.error('Failed to import data:', error);
      return false;
    }
  }

  getWeekDayName(day: WeekDay): string {
    const names = ['', '周一', '周二', '周三', '周四', '周五', '周六', '周日'];
    return names[day];
  }


  checkConflict(day: WeekDay, startSection: number, endSection: number, excludeCourseId?: string): boolean {
    const allCourses = this.getAllCourses();

    // 查找是否存在时间重叠的课程
    const conflictCourse = allCourses.find(course => {
      // 1. 如果是同一个课程（编辑模式），跳过不比较
      if (excludeCourseId && course.id === excludeCourseId) {
        return false;
      }

      // 2. 必须是同一天
      if (course.day !== day) {
        return false;
      }

      // 3. 核心冲突算法：判断两个时间段是否有交集
      // 现有课程区间: [course.startSection, course.endSection]
      // 新增课程区间: [startSection, endSection]
      // 交集判断公式: max(StartA, StartB) <= min(EndA, EndB)
      // 或者更简单的：A开始 <= B结束 且 A结束 >= B开始
      const isOverlap = (course.startSection <= endSection) && (course.endSection >= startSection);

      return isOverlap;
    });

    return conflictCourse !== undefined;
  }


  private getSampleData(): Course[] {
    return [
      {
        id: '1',
        name: '高等数学',
        location: '教学楼A101',
        teacher: '张教授',
        day: WeekDay.MONDAY,
        startSection: 1,
        endSection: 2,
        color: ColorConstants.MACARON_COLORS[9], // Deep Blue Grey

        remark: '必修课，需要带教材',
        semester: '2024-2025学年第一学期',
        startWeek: 1,
        endWeek: 18,
        credit: 4
      },
      {
        id: '2',
        name: '大学英语',
        location: '外语楼B203',
        teacher: '李老师',
        day: WeekDay.MONDAY,
        startSection: 3,
        endSection: 4,
        color: ColorConstants.MACARON_COLORS[0], // Pink
        remark: '需要带耳机',
        semester: '2024-2025学年第一学期',
        startWeek: 1,
        endWeek: 18,
        credit: 3
      },
      {
        id: '3',
        name: '计算机基础',
        location: '实验楼C305',
        teacher: '王老师',
        day: WeekDay.TUESDAY,
        startSection: 5,
        endSection: 6,
        color: ColorConstants.MACARON_COLORS[3], // Mint
        remark: '上机课',
        semester: '2024-2025学年第一学期',
        startWeek: 1,
        endWeek: 18,
        credit: 2
      },
      {
        id: '4',
        name: '线性代数',
        location: '教学楼A102',
        teacher: '赵教授',
        day: WeekDay.WEDNESDAY,
        startSection: 1,
        endSection: 2,
        color: ColorConstants.MACARON_COLORS[2], // Green
        remark: '',
        semester: '2024-2025学年第一学期',
        startWeek: 1,
        endWeek: 18,
        credit: 3
      },
      {
        id: '5',
        name: '物理实验',
        location: '实验楼D401',
        teacher: '刘老师',
        day: WeekDay.THURSDAY,
        startSection: 7,
        endSection: 8,
        color: ColorConstants.MACARON_COLORS[4], // Purple

        remark: '分组实验',
        semester: '2024-2025学年第一学期',
        startWeek: 1,
        endWeek: 18,
        credit: 1
      }
    ];
    // this.courses = sampleCourses;
    // this.saveCourses();
  }

  async isLoggedIn(): Promise<boolean> {
    if (!this.preferences) {
      return false;
    }
    return await this.preferences.get('is_logged_in', false) as boolean;
  }

  async setLoggedIn(loggedIn: boolean): Promise<void> {
    if (this.preferences) {
      await this.preferences.put('is_logged_in', loggedIn);
      await this.preferences.flush();
    }
  }

  private async ensureInitIfPossible(): Promise<void> {
    if (this.preferences) {
      return;
    }

    if (this.initPromise) {
      await this.initPromise;
      return;
    }

    const context = AppStorage.Get<Context>('appContext');
    if (context) {
      await this.init(context);
    }
  }

  private readUsersFromJson(usersJson: string): Record<string, string> {
    try {
      const users = JSON.parse(usersJson) as Record<string, string>;
      return users ?? {};
    } catch (error) {
      console.error('Failed to parse users_data:', error);
      return {};
    }
  }

  private getUsersCacheJson(): string {
    return AppStorage.Get<string>('users_data_cache') || '{}';
  }

  private setUsersCacheJson(usersJson: string): void {
    AppStorage.SetOrCreate('users_data_cache', usersJson);
  }

  private async loadUsers(): Promise<Record<string, string>> {
    await this.ensureInitIfPossible();

    if (this.preferences) {
      const usersJson = await this.preferences.get('users_data', '{}') as string;
      return this.readUsersFromJson(usersJson);
    }

    return this.readUsersFromJson(this.getUsersCacheJson());
  }

  private async saveUsers(users: Record<string, string>): Promise<void> {
    const usersJson = JSON.stringify(users);

    await this.ensureInitIfPossible();
    if (this.preferences) {
      await this.preferences.put('users_data', usersJson);
      await this.preferences.flush();
      return;
    }

    this.setUsersCacheJson(usersJson);
  }

  async register(username: string, password: string): Promise<string> {
    try {
      const users = await this.loadUsers();

      if (users[username]) {
        return '账号已被注册';
      }

      users[username] = password;
      await this.saveUsers(users);
      return 'success';
    } catch (error) {
      console.error('Registration failed:', error);
      return '注册失败，请重试';
    }
  }

  async login(username: string, password: string): Promise<string> {
    try {
      const users = await this.loadUsers();

      if (!users[username]) {
        return '账号不存在';
      }

      if (users[username] !== password) {
        return '密码错误';
      }

      await this.ensureInitIfPossible();
      if (this.preferences) {
        this.currentUser = username;
        await this.preferences.put('current_user', username);
        await this.preferences.flush();
        await this.setLoggedIn(true);
        await this.reloadData();
      }

      return 'success';
    } catch (error) {
      console.error('Login failed:', error);
      return '登录失败，请重试';
    }
  }

  async logout(): Promise<void> {
    this.currentUser = '';
    if (this.preferences) {
      await this.preferences.put('current_user', '');
      await this.preferences.flush();
    }
    await this.setLoggedIn(false);
  }

  async getUserInfo(): Promise<UserInfo> {
    try {
      if (this.preferences) {
        const nameKey = this.getUserKey('user_name');
        const avatarKey = this.getUserKey('user_avatar');
        const signatureKey = this.getUserKey('user_signature');

        const name = await this.preferences.get(nameKey, '学生');
        let avatar = await this.preferences.get(avatarKey, 'bit_logo') as string;
        const signature = await this.preferences.get(signatureKey, '认真上课，天天向上！');

        // Migration: If the avatar is the old default blue color, replace it with the new logo
        if (avatar === '#007DFF') {
          avatar = 'bit_logo';
          await this.preferences.put(avatarKey, 'bit_logo');
          await this.preferences.flush();
        }

        return {
          name: name as string,
          avatar: avatar,
          signature: signature as string
        };
      }
    } catch (error) {
      console.error('Failed to get user info:', error);
    }

    return {
      name: '学生',
      avatar: 'bit_logo',
      signature: '认真上课，天天向上！'
    };
  }

  async saveUserInfo(userInfo: UserInfo): Promise<void> {
    try {
      // Sync to AppStorage
      AppStorage.setOrCreate('userName', userInfo.name);
      AppStorage.setOrCreate('userAvatar', userInfo.avatar);
      AppStorage.setOrCreate('userSignature', userInfo.signature);

      if (this.preferences) {
        const nameKey = this.getUserKey('user_name');
        const avatarKey = this.getUserKey('user_avatar');
        const signatureKey = this.getUserKey('user_signature');

        await this.preferences.put(nameKey, userInfo.name);
        await this.preferences.put(avatarKey, userInfo.avatar);
        await this.preferences.put(signatureKey, userInfo.signature);
        await this.preferences.flush();
      }
    } catch (error) {
      console.error('Failed to save user info:', error);
      throw new Error(`Failed to save user info: ${error}`);
    }
  }
}
