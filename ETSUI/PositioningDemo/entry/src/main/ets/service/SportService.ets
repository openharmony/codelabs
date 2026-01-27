import { SportType, SportData, RoutePoint, LocationInfo } from '../model/SportModel';

export class SportService {
  private currentSportData: SportData | null = null;
  private startTime: number = 0;
  private lastLocation: RoutePoint | null = null;
  private totalDistance: number = 0;
  private totalDuration: number = 0;
  private totalSpeed: number = 0;
  private speedCount: number = 0;

  startSport(type: SportType): SportData {
    this.startTime = Date.now();
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

    console.log('运动服务已启动:', this.currentSportData);
    return this.currentSportData;
  }

  updateLocation(location: LocationInfo): void {
    if (!this.currentSportData) {
      return;
    }

    const routePoint = new RoutePoint(
      location.latitude,
      location.longitude,
      location.timestamp
    );

    if (this.lastLocation) {
      const distance = this.calculateDistance(
        this.lastLocation.latitude,
        this.lastLocation.longitude,
        location.latitude,
        location.longitude
      );
      this.totalDistance += distance;
    }

    this.lastLocation = routePoint;
    this.currentSportData.route.push(routePoint);
    this.currentSportData.distance = this.totalDistance;
    this.currentSportData.duration = Date.now() - this.startTime;
    this.currentSportData.currentSpeed = location.speed;

    this.totalSpeed += location.speed;
    this.speedCount++;
    this.currentSportData.averageSpeed = this.totalSpeed / this.speedCount;

    this.currentSportData.calories = this.calculateCalories(
      this.currentSportData.distance,
      this.currentSportData.duration,
      this.currentSportData.type
    );

    console.log('运动数据已更新:', this.currentSportData);
  }

  pauseSport(): void {
    if (this.currentSportData) {
      console.log('运动已暂停');
    }
  }

  resumeSport(): void {
    if (this.currentSportData) {
      console.log('运动已恢复');
    }
  }

  stopSport(): SportData | null {
    if (!this.currentSportData) {
      return null;
    }

    this.currentSportData.endTime = Date.now();
    this.currentSportData.duration = this.currentSportData.endTime - this.startTime;

    console.log('运动已停止:', this.currentSportData);
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
}
