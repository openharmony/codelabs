// Copyright 2023 YourCompany. All rights reserved.
import { SportType, SportData, RoutePoint, LocationInfo, DataPoint } from '../model/SportModel';
import { ReminderService } from './ReminderService';

export class SportService {
  private currentSportData: SportData | null = null;
  private startTime: number = 0;
  private lastLocation: RoutePoint | null = null;
  private lastTimestamp: number = 0;
  private totalDistance: number = 0;
  private totalDuration: number = 0;
  private totalSpeed: number = 0;
  private speedCount: number = 0;
  private updateTimer: number = -1;
  private pauseStartTime: number = 0;
  private totalPauseDuration: number = 0;
  private isPaused: boolean = false;
  private reminderService: ReminderService;
  
  constructor(reminderService?: ReminderService) {
    this.reminderService = reminderService || new ReminderService();
    console.log('SportService 构造函数 - ReminderService 实例:', this.reminderService);
  }

  startSport(type: SportType): SportData {
    this.startTime = Date.now();
    this.lastTimestamp = this.startTime;
    this.totalDistance = 0;
    this.totalDuration = 0;
    this.totalSpeed = 0;
    this.speedCount = 0;
    this.lastLocation = null;

    this.currentSportData = new SportData(
      type,
      0,
      0,
      0,
      0,
      0,
      this.startTime,
      0,
      []
    );

    // 启动定时器，每秒更新一次运动数据
    this.startUpdateTimer();
    
    // 启动提醒服务并触发运动开始提醒
    this.reminderService.startReminderService();
    this.reminderService.onSportStart();
    
    console.log('运动服务已启动:', this.currentSportData);
    return this.currentSportData;
  }
  
  // 启动更新定时器
  private startUpdateTimer(): void {
    // 清除之前的定时器
    this.stopUpdateTimer();
    
    // 创建新的定时器，每秒更新一次
    this.updateTimer = setInterval(() => {
      if (this.currentSportData && !this.isPaused) {
        // 计算总持续时间（总时间 - 总暂停时间）
        const totalTime = Date.now() - this.startTime;
        const currentDuration = totalTime - this.totalPauseDuration;
        this.currentSportData.duration = currentDuration;
        
        // 计算卡路里
        const calories = this.calculateCalories(this.totalDistance, currentDuration, this.currentSportData.type);
        this.currentSportData.calories = calories;
        
        // 记录速度和卡路里数据
        const speedDataPoint = new DataPoint(Date.now(), this.currentSportData.currentSpeed);
        const caloriesDataPoint = new DataPoint(Date.now(), calories);
        this.currentSportData.speedData.push(speedDataPoint);
        this.currentSportData.caloriesData.push(caloriesDataPoint);
        console.log('记录速度数据点 - 时间戳:', speedDataPoint.timestamp, '速度:', speedDataPoint.value, 'm/s');
        console.log('记录卡路里数据点 - 时间戳:', caloriesDataPoint.timestamp, '卡路里:', caloriesDataPoint.value, 'kcal');
        
        // 触发提醒服务的运动数据更新处理
        this.reminderService.onSportDataUpdate(this.currentSportData);
        
        console.log('定时器更新运动数据，总时间:', totalTime, '总暂停时间:', this.totalPauseDuration, '持续时间:', currentDuration, '速度:', this.currentSportData.currentSpeed, '卡路里:', calories);
      }
    }, 1000);
  }
  
  // 停止更新定时器
  private stopUpdateTimer(): void {
    if (this.updateTimer !== -1) {
      clearInterval(this.updateTimer);
      this.updateTimer = -1;
    }
  }

  updateLocation(location: LocationInfo): void {
    if (!this.currentSportData) {
      return;
    }

    console.log('收到位置更新，速度信息:', location.speed, '位置信息:', location);

    const routePoint = new RoutePoint(
      location.latitude,
      location.longitude,
      location.timestamp
    );

    let distance = 0;
    let currentSpeed = location.speed || 0;
    
    if (this.lastLocation) {
      distance = this.calculateDistance(
        this.lastLocation.latitude,
        this.lastLocation.longitude,
        location.latitude,
        location.longitude
      );
      this.totalDistance += distance;
      
      // 计算时间差（毫秒）
      const timeDiff = location.timestamp - this.lastTimestamp;
      console.log('距离:', distance, '时间差:', timeDiff);
      
      // 如果时间差大于0，计算速度（米/秒）
      if (timeDiff > 0) {
        const timeDiffSeconds = timeDiff / 1000;
        const calculatedSpeed = distance / timeDiffSeconds;
        console.log('计算的速度:', calculatedSpeed);
        
        // 只有当计算出的速度大于当前速度或当前速度为0时，才使用计算的速度
        if (currentSpeed === 0 || calculatedSpeed > currentSpeed) {
          currentSpeed = calculatedSpeed;
        }
        
        // 限制速度在合理范围内
        if (currentSpeed < 0.5) {
          currentSpeed = 0.5; // 最低速度0.5 m/s
        } else if (currentSpeed > 20) {
          currentSpeed = 20; // 最高速度20 m/s
        }
      }
    }

    // 更新上一个位置和时间戳
    this.lastLocation = routePoint;
    this.lastTimestamp = location.timestamp;
    
    this.currentSportData.route.push(routePoint);
    this.currentSportData.distance = this.totalDistance;
    this.currentSportData.duration = Date.now() - this.startTime;
    
    // 设置当前速度
    this.currentSportData.currentSpeed = currentSpeed;
    console.log('最终使用的速度:', currentSpeed);

    // 累计速度和计数
    if (currentSpeed > 0) {
      this.totalSpeed += currentSpeed;
      this.speedCount++;
    }
    
    // 计算平均速度
    this.currentSportData.averageSpeed = this.speedCount > 0 ? this.totalSpeed / this.speedCount : 0;

    this.currentSportData.calories = this.calculateCalories(
      this.currentSportData.distance,
      this.currentSportData.duration,
      this.currentSportData.type
    );

    // 触发提醒服务的运动数据更新处理
    this.reminderService.onSportDataUpdate(this.currentSportData);

    console.log('运动数据已更新:', this.currentSportData);
  }

  pauseSport(): void {
    if (this.currentSportData && !this.isPaused) {
      // 暂停时停止定时器
      this.stopUpdateTimer();
      // 记录暂停开始时间
      this.pauseStartTime = Date.now();
      // 设置暂停状态
      this.isPaused = true;
      console.log('运动已暂停，暂停开始时间:', this.pauseStartTime);
    }
  }

  resumeSport(): void {
    if (this.currentSportData && this.isPaused) {
      // 计算这次暂停的时间
      const pauseDuration = Date.now() - this.pauseStartTime;
      // 累加到总暂停时间
      this.totalPauseDuration += pauseDuration;
      // 设置非暂停状态
      this.isPaused = false;
      // 恢复时重新启动定时器
      this.startUpdateTimer();
      console.log('运动已恢复，本次暂停时间:', pauseDuration, '总暂停时间:', this.totalPauseDuration);
    }
  }

  stopSport(): SportData | null {
    if (!this.currentSportData) {
      return null;
    }

    // 停止定时器
    this.stopUpdateTimer();
    
    // 如果当前处于暂停状态，计算最后一次暂停的时间
    if (this.isPaused) {
      const pauseDuration = Date.now() - this.pauseStartTime;
      this.totalPauseDuration += pauseDuration;
      this.isPaused = false;
      console.log('运动停止时处于暂停状态，本次暂停时间:', pauseDuration, '总暂停时间:', this.totalPauseDuration);
    }
    
    // 计算实际运动时间（总时间 - 总暂停时间）
    this.currentSportData.endTime = Date.now();
    const totalTime = this.currentSportData.endTime - this.startTime;
    const actualDuration = totalTime - this.totalPauseDuration;
    this.currentSportData.duration = actualDuration;

    // 触发提醒服务的运动结束处理
    this.reminderService.onSportEnd();

    console.log('运动已停止，总时间:', totalTime, '总暂停时间:', this.totalPauseDuration, '实际运动时间:', actualDuration);
    const finalData = this.currentSportData;
    this.currentSportData = null;
    return finalData;
  }

  getCurrentSportData(): SportData | null {
    return this.currentSportData;
  }

  private calculateDistance(lat1: number, lon1: number, lat2: number, lon2: number): number {
    const R = 6371000;
    const dLat = this.toRadians(lat2 - lat1);
    const dLon = this.toRadians(lon2 - lon1);
    const a = Math.sin(dLat / 2) * Math.sin(dLat / 2) +
              Math.cos(this.toRadians(lat1)) * Math.cos(this.toRadians(lat2)) *
              Math.sin(dLon / 2) * Math.sin(dLon / 2);
    const c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1 - a));
    return R * c;
  }

  private toRadians(degrees: number): number {
    return degrees * (Math.PI / 180);
  }

  private calculateCalories(distance: number, duration: number, type: SportType): number {
    const durationInMinutes = duration / 60000;
    const distanceInKm = distance / 1000;

    let caloriesPerMinute = 0;
    switch (type) {
      case SportType.RUNNING:
        caloriesPerMinute = 10;
        break;
      case SportType.WALKING:
        caloriesPerMinute = 4;
        break;
      case SportType.CYCLING:
        caloriesPerMinute = 8;
        break;
    }

    return caloriesPerMinute * durationInMinutes;
  }

  formatDistance(distance: number): string {
    if (distance >= 1000) {
      return `${(distance / 1000).toFixed(2)} km`;
    }
    return `${distance.toFixed(0)} m`;
  }

  formatDuration(duration: number): string {
    const hours = Math.floor(duration / 3600000);
    const minutes = Math.floor((duration % 3600000) / 60000);
    const seconds = Math.floor((duration % 60000) / 1000);

    if (hours > 0) {
      return `${hours}:${String(minutes).padStart(2, '0')}:${String(seconds).padStart(2, '0')}`;
    }
    return `${String(minutes).padStart(2, '0')}:${String(seconds).padStart(2, '0')}`;
  }

  formatSpeed(speed: number): string {
    return `${speed.toFixed(1)} m/s`;
  }

  formatCalories(calories: number): string {
    return `${calories.toFixed(0)} kcal`;
  }

  // 获取提醒服务实例
  getReminderService(): ReminderService {
    console.log('SportService.getReminderService - 返回的实例:', this.reminderService);
    return this.reminderService;
  }
} 
