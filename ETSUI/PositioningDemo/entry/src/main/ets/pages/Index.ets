import { LocationService } from '../service/LocationService';
import { SportService } from '../service/SportService';
import { HistoryService } from '../service/HistoryService';
import { SportType, SportData, RouteData, LocationInfo } from '../model/SportModel';
import { SportTypeSelector } from '../components/SportTypeSelector';
import { SportDataDisplay } from '../components/SportDataDisplay';
import { SportControlButton } from '../components/SportControlButton';
import { RouteMapInner } from '../components/RouteMapInner';
import { common } from '@kit.AbilityKit';
import router from '@ohos.router';
import { map } from '@kit.MapKit';

@Entry
@Component
struct Index {
  @State selectedType: SportType = SportType.RUNNING;
  @State sportData: SportData | null = null;
  @State isRunning: boolean = false;
  @State isPaused: boolean = false;
  @State errorMessage: string = '';
  @State currentLocation: LocationInfo | null = null;
  @State routePoints: LocationInfo[] = [];
  @State isSimulator: boolean = false;
  @State routeData: RouteData = new RouteData();
  
  private locationService: LocationService = new LocationService();
  private sportService: SportService = new SportService();
  private historyService: HistoryService = new HistoryService();
  private simulationTimer: number = -1;
  private routeMapController?: map.MapComponentController;

  async aboutToAppear() {
    const context = getContext(this) as common.UIAbilityContext;
    await this.historyService.init(context);
    this.isSimulator = true;
    console.log('使用模拟器模式');
  }

  onSelectSportType(type: SportType): void {
    this.selectedType = type;
  }

  onStartSport(): void {
    this.startSport();
  }

  onPauseSport(): void {
    this.pauseSport();
  }

  onResumeSport(): void {
    this.resumeSport();
  }

  onStopSport(): void {
    this.stopSport();
  }

  build() {
    Column() {
      this.Header()

      if (this.errorMessage) {
        Text(this.errorMessage)
          .fontSize(14)
          .fontColor(Color.Red)
          .margin({ top: 10, bottom: 10 })
      }

      if (!this.isRunning) {
        SportTypeSelector({
          selectedType: this.selectedType,
          onSelect: (type: SportType) => {
            this.onSelectSportType(type);
          }
        })
        .margin({ top: 20 })
      }

      if (this.isRunning) {
        RouteMapInner({
          routeData: $routeData
        })
        .margin({ top: 20 })
      }

      if (this.sportData) {
        Text(`调试: 距离=${this.sportData.distance.toFixed(2)} 时间=${this.sportData.duration.toFixed(0)}`)
          .fontSize(12)
          .fontColor('#999999')
          .margin({ top: 10, bottom: 10 })
        
        Column() {
          Row() {
            Column() {
              Text(this.sportData.distance.toFixed(0))
                .fontSize(24)
                .fontWeight(FontWeight.Bold)
                .fontColor('#FF6B6B')
                .margin({ bottom: 4 })
              Text('距离')
                .fontSize(12)
                .fontColor('#999999')
            }
            .alignItems(HorizontalAlign.Center)
            
            Column() {
              Text(this.formatDuration(this.sportData.duration))
                .fontSize(24)
                .fontWeight(FontWeight.Bold)
                .fontColor('#4ECDC4')
                .margin({ bottom: 4 })
              Text('时间')
                .fontSize(12)
                .fontColor('#999999')
            }
            .alignItems(HorizontalAlign.Center)
          }
          .width('100%')
          .justifyContent(FlexAlign.SpaceEvenly)
          .margin({ bottom: 20 })

          Row() {
            Column() {
              Text(this.sportData.currentSpeed.toFixed(1))
                .fontSize(24)
                .fontWeight(FontWeight.Bold)
                .fontColor('#45B7D1')
                .margin({ bottom: 4 })
              Text('速度')
                .fontSize(12)
                .fontColor('#999999')
            }
            .alignItems(HorizontalAlign.Center)
            
            Column() {
              Text(this.sportData.calories.toFixed(0))
                .fontSize(24)
                .fontWeight(FontWeight.Bold)
                .fontColor('#FFA502')
                .margin({ bottom: 4 })
              Text('卡路里')
                .fontSize(12)
                .fontColor('#999999')
            }
            .alignItems(HorizontalAlign.Center)
          }
          .width('100%')
          .justifyContent(FlexAlign.SpaceEvenly)
        }
        .width('100%')
        .padding(20)
        .backgroundColor('#FFFFFF')
        .borderRadius(16)
        .shadow({ radius: 8, color: '#1A000000', offsetX: 0, offsetY: 2 })
      }

      Blank()

      SportControlButton({
        isRunning: this.isRunning,
        isPaused: this.isPaused,
        onStart: () => {
          this.onStartSport();
        },
        onPause: () => {
          this.onPauseSport();
        },
        onResume: () => {
          this.onResumeSport();
        },
        onStop: () => {
          this.onStopSport();
        }
      })
      .margin({ bottom: 30 })
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F8F9FA')
    .padding({ top: 20, left: 16, right: 16 })
  }

  @Builder
  Header() {
    Row() {
      Column() {
        Text('运动助手')
          .fontSize(28)
          .fontWeight(FontWeight.Bold)
          .fontColor('#333333')
        Text(this.getSportTypeName())
          .fontSize(14)
          .fontColor('#999999')
          .margin({ top: 4 })
      }
      .alignItems(HorizontalAlign.Start)

      Blank()

      Row() {
        Text('📊')
          .fontSize(24)
          .margin({ right: 16 })
          .onClick(() => {
            router.pushUrl({ url: 'pages/HistoryPage' });
          })

        if (this.isRunning && !this.isPaused) {
          Row() {
            Circle()
              .width(12)
              .height(12)
              .fill('#FF6B6B')
              .margin({ right: 6 })
            Text('运动中')
              .fontSize(14)
              .fontColor('#FF6B6B')
          }
          .padding({ left: 12, right: 12, top: 6, bottom: 6 })
          .backgroundColor('#FF6B6B20')
          .borderRadius(20)
        }
      }
    }
    .width('100%')
    .justifyContent(FlexAlign.SpaceBetween)
  }

  async startSport() {
    this.errorMessage = '';
    this.routePoints = [];

    try {
      if (this.isSimulator) {
        console.log('模拟器环境，直接启动模拟');
        this.sportData = this.sportService.startSport(this.selectedType);
        this.isRunning = true;
        this.isPaused = false;
        console.log('运动服务已启动, sportData:', this.sportData);
        this.startSimulation();
      } else {
        const context = getContext(this) as common.UIAbilityContext;
        console.log('开始请求权限...');
        const hasPermission = await this.locationService.requestPermissions(context);
        console.log('权限结果:', hasPermission);
        
        if (!hasPermission) {
          this.errorMessage = '位置权限被拒绝';
          return;
        }

        console.log('启动运动服务...');
        this.sportData = this.sportService.startSport(this.selectedType);
        this.isRunning = true;
        this.isPaused = false;
        
        console.log('真机环境，启动真实位置更新');
        const success = await this.locationService.startLocationUpdate((location: LocationInfo) => {
          console.log('收到位置更新:', location);
          if (this.isRunning && !this.isPaused) {
            this.currentLocation = location;
            this.routePoints.push(location);
            this.sportService.updateLocation(location);
            this.sportData = this.sportService.getCurrentSportData();
          }
        });

        if (!success) {
          console.error('启动位置更新失败');
          this.errorMessage = '启动位置追踪失败';
          this.isRunning = false;
        }
      }
    } catch (err) {
      console.error('启动运动失败:', err);
      this.errorMessage = '发生错误: ' + JSON.stringify(err);
      this.isRunning = false;
    }
  }

  pauseSport() {
    this.sportService.pauseSport();
    this.isPaused = true;
    this.sportData = this.sportService.getCurrentSportData();
  }

  resumeSport() {
    this.sportService.resumeSport();
    this.isPaused = false;
  }

  stopSport() {
    const finalData = this.sportService.stopSport();
    this.locationService.stopLocationUpdate();
    this.stopSimulation();
    this.isRunning = false;
    this.isPaused = false;
    this.currentLocation = null;
    
    if (finalData) {
      this.historyService.saveSportRecord(finalData);
      this.showSportSummary(finalData);
    }
  }

  startSimulation() {
    console.log('开始模拟位置更新');
    let baseLat = 39.9042;
    let baseLon = 116.4074;
    let index = 0;
    let lastLat = baseLat;
    let lastLon = baseLon;
    let lastTime = Date.now();
    let totalDistance = 0;
    
    this.simulationTimer = setInterval(() => {
      console.log('模拟定时器触发, isRunning:', this.isRunning, 'isPaused:', this.isPaused);
      
      if (!this.isRunning || this.isPaused) {
        console.log('模拟暂停，跳过更新');
        return;
      }
      
      index++;
      
      const angle = index * 0.05;
      const radius = 0.002;
      
      const lat = baseLat + Math.sin(angle) * radius;
      const lon = baseLon + Math.cos(angle) * radius;
      
      const speed = 2 + Math.random() * 3;
      const currentTime = Date.now();
      
      const distance = this.calculateDistance(lastLat, lastLon, lat, lon);
      totalDistance += distance;
      const netDuration = currentTime - lastTime;
      
      console.log('生成模拟位置:', index, 'lat:', lat, 'lon:', lon, 'speed:', speed, 'distance:', distance, 'totalDistance:', totalDistance);
      
      const location = new LocationInfo(
        lat,
        lon,
        100,
        10,
        speed,
        currentTime,
        totalDistance,
        distance,
        netDuration
      );
      
      lastLat = lat;
      lastLon = lon;
      lastTime = currentTime;
      
      console.log('更新routeData之前, routePoints长度:', this.routePoints.length, 'routeData.routePoints长度:', this.routeData.routePoints.length);
      this.routeData.addRoutePoint(location);
      this.routeData.updateCurrentLocation(location);
      this.routePoints.push(location);
      this.sportService.updateLocation(location);
      this.sportData = this.sportService.getCurrentSportData();
      console.log('更新routeData之后, routePoints长度:', this.routePoints.length, 'routeData.routePoints长度:', this.routeData.routePoints.length);
      
      console.log('模拟位置更新完成, routePoints长度:', this.routePoints.length, 'sportData:', this.sportData);
    }, 2000);
    
    console.log('模拟定时器已启动, ID:', this.simulationTimer);
  }

  stopSimulation() {
    if (this.simulationTimer !== -1) {
      clearInterval(this.simulationTimer);
      this.simulationTimer = -1;
      console.log('停止模拟位置更新');
    }
  }

  calculateDistance(lat1: number, lon1: number, lat2: number, lon2: number): number {
    const R = 6371000;
    const dLat = this.toRadians(lat2 - lat1);
    const dLon = this.toRadians(lon2 - lon1);
    const a = Math.sin(dLat / 2) * Math.sin(dLat / 2) +
              Math.cos(this.toRadians(lat1)) * Math.cos(this.toRadians(lat2)) *
              Math.sin(dLon / 2) * Math.sin(dLon / 2);
    const c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1 - a));
    return R * c;
  }

  toRadians(degrees: number): number {
    return degrees * (Math.PI / 180);
  }

  showSportSummary(data: SportData) {
    AlertDialog.show({
      title: '运动完成',
      message: `运动类型: ${this.getSportTypeName()}\n距离: ${this.sportService.formatDistance(data.distance)}\n时间: ${this.sportService.formatDuration(data.duration)}\n平均速度: ${this.sportService.formatSpeed(data.averageSpeed)}\n消耗卡路里: ${this.sportService.formatCalories(data.calories)}`,
      confirm: {
        value: '确定',
        action: () => {
          this.sportData = null;
        }
      }
    });
  }

  getSportTypeName(): string {
    switch (this.selectedType) {
      case SportType.RUNNING:
        return '跑步';
      case SportType.WALKING:
        return '行走';
      case SportType.CYCLING:
        return '骑行';
      default:
        return '运动';
    }
  }

  private formatDuration(duration: number): string {
    const hours = Math.floor(duration / 3600000);
    const minutes = Math.floor((duration % 3600000) / 60000);
    const seconds = Math.floor((duration % 60000) / 1000);

    if (hours > 0) {
      return `${hours}:${String(minutes).padStart(2, '0')}:${String(seconds).padStart(2, '0')}`;
    }
    return `${String(minutes).padStart(2, '0')}:${String(seconds).padStart(2, '0')}`;
  }
}
