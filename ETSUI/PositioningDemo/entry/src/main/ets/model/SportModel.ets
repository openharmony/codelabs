export enum SportType {
  RUNNING = 'running',
  WALKING = 'walking',
  CYCLING = 'cycling'
}

export class RoutePoint {
  latitude: number = 0;
  longitude: number = 0;
  timestamp: number = 0;

  constructor(latitude: number, longitude: number, timestamp: number) {
    this.latitude = latitude;
    this.longitude = longitude;
    this.timestamp = timestamp;
  }
}

export class LocationInfo {
  latitude: number = 0;
  longitude: number = 0;
  altitude: number = 0;
  accuracy: number = 0;
  speed: number = 0;
  timestamp: number = 0;
  totalDistance: number = 0;
  netDistance: number = 0;
  netDuration: number = 0;

  constructor(latitude: number, longitude: number, altitude: number, accuracy: number, speed: number, timestamp: number, totalDistance: number = 0, netDistance: number = 0, netDuration: number = 0) {
    this.latitude = latitude;
    this.longitude = longitude;
    this.altitude = altitude;
    this.accuracy = accuracy;
    this.speed = speed;
    this.timestamp = timestamp;
    this.totalDistance = totalDistance;
    this.netDistance = netDistance;
    this.netDuration = netDuration;
  }
}

// 添加时间序列数据结构
export class DataPoint {
  timestamp: number;
  value: number;

  constructor(timestamp: number, value: number) {
    this.timestamp = timestamp;
    this.value = value;
  }
}

export class SportData {
  type: SportType = SportType.RUNNING;
  distance: number = 0;
  duration: number = 0;
  calories: number = 0;
  averageSpeed: number = 0;
  currentSpeed: number = 0;
  startTime: number = 0;
  speedData: DataPoint[] = [];      // 速度时间序列数据
  caloriesData: DataPoint[] = [];    // 卡路里时间序列数据
  endTime: number = 0;
  route: RoutePoint[] = [];

  constructor(type: SportType, distance: number, duration: number, calories: number, averageSpeed: number, currentSpeed: number, startTime: number, endTime: number, route: RoutePoint[]) {
    this.type = type;
    this.distance = distance;
    this.duration = duration;
    this.calories = calories;
    this.averageSpeed = averageSpeed;
    this.currentSpeed = currentSpeed;
    this.startTime = startTime;
    this.endTime = endTime;
    this.route = route;
  }
}

@Observed
export class RouteData {
  @Track routePoints: LocationInfo[] = [];
  @Track currentLocation: LocationInfo | null = null;

  updateRoutePoints(points: LocationInfo[]): void {
    this.routePoints = points;
  }

  updateCurrentLocation(location: LocationInfo | null): void {
    this.currentLocation = location;
  }

  addRoutePoint(location: LocationInfo): void {
    this.routePoints = [...this.routePoints, location];
  }
}
