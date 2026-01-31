// SPDX-License-Identifier: MIT
import { LocationService } from '../service/LocationService';
import { SportService } from '../service/SportService';
import { ReminderService } from '../service/ReminderService';
import { HistoryService } from '../service/HistoryService';
import { StatisticsService } from '../service/StatisticsService';
import { AchievementService } from '../service/AchievementService';
import { SportType, SportData, RouteData, LocationInfo } from '../model/SportModel';
import { SportControlButton } from '../components/SportControlButton';
import { RouteMap } from '../components/RouteMapInner';
import { ReminderSettingComponent } from '../components/ReminderSettingComponent';
import { common } from '@kit.AbilityKit';
import router from '@ohos.router';
import { map } from '@kit.MapKit';
import promptAction from '@ohos.promptAction';

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
  @State selectedMode: string = '自由跑';
  
  private locationService: LocationService = new LocationService();
  private reminderService: ReminderService = new ReminderService();
  private sportService: SportService = new SportService(this.reminderService);
  private historyService: HistoryService = new HistoryService();
  private statisticsService: StatisticsService = new StatisticsService(this.historyService);
  private achievementService: AchievementService = new AchievementService(this.historyService, this.statisticsService);
  private simulationTimer: number = -1;
  private uiUpdateTimer: number = -1;
  private routeMapController?: map.MapComponentController;

  async aboutToAppear() {
    const context = getContext(this) as common.UIAbilityContext;
    this.historyService.init(context).then(() => {
      // 不再硬编码为true，而是让应用自动检测或由用户选择
      // 默认使用真实模式，允许模拟器尝试使用真实GPS
      this.isSimulator = false;
      console.log('默认使用真实模式，允许模拟器尝试使用真实GPS');
    }).catch((error: Error) => {
      console.error('初始化历史服务失败:', error);
    });
    
    // 初始化成就服务
    await this.achievementService.init(context);
    console.log('成就服务已初始化');
    
    // 初始化提醒服务
    this.reminderService.init(context);
    console.log('提醒服务已初始化');
    console.log('ReminderService 实例:', this.reminderService);
    console.log('SportService 使用的 ReminderService 实例:', this.sportService.getReminderService());
    
    // 页面初始化时自动获取当前定位
    try {
      await this.autoGetLocation();
    } catch (error) {
      console.error('自动获取定位失败:', error);
      // 如果获取定位失败，设置一个默认位置
      this.setDefaultLocation();
    }
  }
  
  // 自动获取定位
  private async autoGetLocation(): Promise<void> {
    const context = getContext(this) as common.UIAbilityContext;
    const hasPermission = await this.locationService.requestPermissions(context);
    
    if (hasPermission) {
      const result = await this.locationService.getCurrentLocation();
      if (result.location) {
        this.currentLocation = result.location;
        this.routeData.updateCurrentLocation(result.location);
        this.routeData.addRoutePoint(result.location);
        console.log('自动获取到真实定位:', result.location);
      } else {
        console.error('自动获取定位失败，使用默认位置');
        this.setDefaultLocation();
      }
    } else {
      console.error('没有定位权限，使用默认位置');
      this.setDefaultLocation();
    }
  }
  
  // 设置默认位置
  private setDefaultLocation(): void {
    const defaultLocation = new LocationInfo(
      39.9042, // 北京天安门附近
      116.4074,
      0,
      0,
      0,
      Date.now(),
      0,
      0,
      0
    );
    this.currentLocation = defaultLocation;
    this.routeData.updateCurrentLocation(defaultLocation);
    this.routeData.addRoutePoint(defaultLocation);
    console.log('使用默认位置:', defaultLocation);
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
      Scroll() {
        Column() {
          this.TopNavBar()
          
          if (this.errorMessage) {
            Text(this.errorMessage)
              .fontSize(14)
              .fontColor(Color.Red)
              .margin({ bottom: 12 })
              .alignSelf(ItemAlign.Center)
          }
          
          if (!this.isRunning) {
            // 添加当前定位显示和获取定位按钮
            Column() {
              Text(`当前定位: ${this.currentLocation ? `${this.currentLocation.latitude.toFixed(6)}, ${this.currentLocation.longitude.toFixed(6)}` : '未获取'}`)
                .fontSize(14)
                .fontColor('#666666')
                .margin({ bottom: 8 })
                .width('100%')
                .textAlign(TextAlign.Center)
              
              Row() {
                Button('获取当前定位')
                  .fontSize(14)
                  .fontColor('#FFFFFF')
                  .backgroundColor('#4ECDC4')
                  .borderRadius(8)
                  .padding({ left: 16, right: 16, top: 6, bottom: 6 })
                  .onClick(() => {
                    this.getCurrentLocation();
                  })
                
                Button('打开定位设置')
                  .fontSize(14)
                  .fontColor('#4ECDC4')
                  .backgroundColor('#FFFFFF')
                  .borderRadius(8)
                  .padding({ left: 16, right: 16, top: 6, bottom: 6 })
                  .border({ width: 1, color: '#4ECDC4' })
                  .onClick(() => {
                    this.openLocationSettings();
                  })
                  .margin({ left: 8 })
              }
              .justifyContent(FlexAlign.Center)
            }
            .margin({ bottom: 12 })
          }
          
          this.SportTypeSelector()
          
          // 提醒设置组件
          ReminderSettingComponent({
            reminderService: this.reminderService
          })
          .margin({ bottom: 16 })
          
          this.MapArea()
          
          if (this.sportData) {
            Column() {
              Row() {
                Column() {
                  Text(this.sportData.distance.toFixed(0))
                    .fontSize(20)
                    .fontWeight(FontWeight.Bold)
                    .fontColor('#FF6B6B')
                    .margin({ bottom: 2 })
                  Text('距离')
                    .fontSize(12)
                    .fontColor('#999999')
                }
                .alignItems(HorizontalAlign.Center)
                
                Column() {
                  Text(this.formatDuration(this.sportData.duration))
                    .fontSize(20)
                    .fontWeight(FontWeight.Bold)
                    .fontColor('#4ECDC4')
                    .margin({ bottom: 2 })
                  Text('时间')
                    .fontSize(12)
                    .fontColor('#999999')
                }
                .alignItems(HorizontalAlign.Center)
                
                Column() {
                  Text(this.sportData.currentSpeed.toFixed(1))
                    .fontSize(20)
                    .fontWeight(FontWeight.Bold)
                    .fontColor('#45B7D1')
                    .margin({ bottom: 2 })
                  Text('速度')
                    .fontSize(12)
                    .fontColor('#999999')
                }
                .alignItems(HorizontalAlign.Center)
                
                Column() {
                  Text(this.sportData.calories.toFixed(0))
                    .fontSize(20)
                    .fontWeight(FontWeight.Bold)
                    .fontColor('#FFA502')
                    .margin({ bottom: 2 })
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
            .padding(16)
            .backgroundColor('#FFFFFF')
            .borderRadius(12)
            .shadow({ radius: 6, color: '#1A000000', offsetX: 0, offsetY: 2 })
            .margin({ bottom: 16 })
          }
        }
        .width('100%')
        .backgroundColor('#F8F9FA')
        .padding({ left: 16, right: 16 })
      }
      .flexGrow(1)
      
      // 中央控制区域固定在底部
      Row() {
        this.CentralControl()
      }
      .margin({ bottom: 20 })
    }
    .width('100%')
    .height('100%')
    .padding({ top: 10, bottom: 10 })
  }

  @Builder
  TopNavBar() {
    Row() {
      Column() {
        Text('运动助手')
          .fontSize(22)
          .fontWeight(FontWeight.Bold)
          .fontColor('#333333')
        Text(this.getSportTypeName())
          .fontSize(12)
          .fontColor('#999999')
      }
      .alignItems(HorizontalAlign.Start)

      Blank()

      Row() {
        Text('👤')
          .fontSize(20)
          .margin({ right: 12 })
          .onClick(() => {
            router.pushUrl({ url: 'pages/ProfilePage' });
          })

        Text('📊')
          .fontSize(20)
          .margin({ right: 12 })
          .onClick(() => {
            router.pushUrl({ url: 'pages/HistoryPage' });
          })

        Text('🏆')
          .fontSize(20)
          .margin({ right: 12 })
          .onClick(() => {
            router.pushUrl({ url: 'pages/AchievementPage' });
          })

        if (this.isRunning && !this.isPaused) {
          Row() {
            Circle()
              .width(10)
              .height(10)
              .fill('#FF6B6B')
              .margin({ right: 4 })
            Text('运动中')
              .fontSize(12)
              .fontColor('#FF6B6B')
          }
          .padding({ left: 8, right: 8, top: 4, bottom: 4 })
          .backgroundColor('#FF6B6B20')
          .borderRadius(16)
        }
      }
    }
    .width('100%')
    .justifyContent(FlexAlign.SpaceBetween)
    .margin({ bottom: 12 })
  }

  @Builder
  SportTypeSelector() {
    Column() {
      // 运动类型选择
      Row() {
        Text('运动类型:')
          .fontSize(12)
          .fontColor('#666666')
          .margin({ right: 8 })
        
        Text('跑步')
          .fontSize(12)
          .fontWeight(this.selectedType === SportType.RUNNING ? FontWeight.Bold : FontWeight.Normal)
          .fontColor(this.selectedType === SportType.RUNNING ? '#FFFFFF' : '#333333')
          .backgroundColor(this.selectedType === SportType.RUNNING ? '#4ECDC4' : '#F0F0F0')
          .borderRadius(16)
          .padding({ left: 16, right: 16, top: 6, bottom: 6 })
          .margin({ right: 6 })
          .onClick(() => {
            this.selectedType = SportType.RUNNING;
          })
        
        Text('行走')
          .fontSize(12)
          .fontWeight(this.selectedType === SportType.WALKING ? FontWeight.Bold : FontWeight.Normal)
          .fontColor(this.selectedType === SportType.WALKING ? '#FFFFFF' : '#333333')
          .backgroundColor(this.selectedType === SportType.WALKING ? '#4ECDC4' : '#F0F0F0')
          .borderRadius(16)
          .padding({ left: 16, right: 16, top: 6, bottom: 6 })
          .margin({ right: 6 })
          .onClick(() => {
            this.selectedType = SportType.WALKING;
          })
        
        Text('骑行')
          .fontSize(12)
          .fontWeight(this.selectedType === SportType.CYCLING ? FontWeight.Bold : FontWeight.Normal)
          .fontColor(this.selectedType === SportType.CYCLING ? '#FFFFFF' : '#333333')
          .backgroundColor(this.selectedType === SportType.CYCLING ? '#4ECDC4' : '#F0F0F0')
          .borderRadius(16)
          .padding({ left: 16, right: 16, top: 6, bottom: 6 })
          .onClick(() => {
            this.selectedType = SportType.CYCLING;
          })
      }
      .margin({ bottom: 8 })
      .justifyContent(FlexAlign.Start)
      
      // 运行模式切换
      Row() {
        Text('运行模式:')
          .fontSize(12)
          .fontColor('#666666')
          .margin({ right: 8 })
        
        if (this.isSimulator) {
          // 当前是模拟模式，显示真实模式按钮
          Text('切换到真实模式')
            .fontSize(12)
            .fontColor('#4ECDC4')
            .onClick(() => {
              this.isSimulator = false;
              console.log('切换到真实模式');
            })
        } else {
          // 当前是真实模式，显示模拟模式按钮
          Text('切换到模拟模式')
            .fontSize(12)
            .fontColor('#4ECDC4')
            .onClick(() => {
              this.isSimulator = true;
              console.log('切换到模拟模式');
            })
        }
      }
      .margin({ bottom: 12 })
      .justifyContent(FlexAlign.Start)
    }
  }

  @Builder
  MapArea() {
    // 地图组件
    RouteMap({
      routeData: $routeData
    })
    .margin({ bottom: 16 })
    .height(250)
  }

  @Builder
  CentralControl() {
    SportControlButton({
      isRunning: this.isRunning,
      isPaused: this.isPaused,
      onStart: () => {
        this.startSport();
      },
      onPause: () => {
        this.pauseSport();
      },
      onResume: () => {
        this.resumeSport();
      },
      onStop: () => {
        this.stopSport();
      }
    })
    .margin({ bottom: 20 })
  }

  async startSport() {
    this.errorMessage = '';
    this.routePoints = [];
    // 重置routeData，确保每次运动开始时都是新的实例
    this.routeData = new RouteData();

    try {
      if (this.isSimulator) {
        console.log('模拟器环境，直接启动模拟');
        this.sportData = this.sportService.startSport(this.selectedType);
        this.isRunning = true;
        this.isPaused = false;
        console.log('运动服务已启动, sportData:', this.sportData);
        
        // 模拟器模式下，设置初始位置
        const initialLocation = new LocationInfo(
          39.9042, // 北京天安门附近
          116.4074,
          0,
          0,
          0,
          Date.now(),
          0,
          0,
          0
        );
        this.currentLocation = initialLocation;
        this.routeData.updateCurrentLocation(initialLocation);
        this.routeData.addRoutePoint(initialLocation);
        
        this.startSimulation();
        this.startUIUpdateTimer();
      } else {
        const context = getContext(this) as common.UIAbilityContext;
        console.log('开始请求权限...');
        const hasPermission = await this.locationService.requestPermissions(context);
        console.log('权限结果:', hasPermission);
        
        if (!hasPermission) {
          this.errorMessage = '位置权限被拒绝，自动切换到模拟模式';
          // 权限被拒绝，自动切换到模拟模式
          this.isSimulator = true;
          this.sportData = this.sportService.startSport(this.selectedType);
          this.isRunning = true;
          this.isPaused = false;
          
          // 模拟器模式下，设置初始位置
          const initialLocation = new LocationInfo(
            39.9042, // 北京天安门附近
            116.4074,
            0,
            0,
            0,
            Date.now(),
            0,
            0,
            0
          );
          this.currentLocation = initialLocation;
          this.routeData.updateCurrentLocation(initialLocation);
          this.routeData.addRoutePoint(initialLocation);
          
          this.startSimulation();
          return;
        }

        console.log('启动运动服务...');
        this.sportData = this.sportService.startSport(this.selectedType);
        this.isRunning = true;
        this.isPaused = false;
        
        // 启动UI更新定时器
        this.startUIUpdateTimer();
        
        console.log('真机环境，启动真实位置更新');
        try {
          const success = await this.locationService.startLocationUpdate((location: LocationInfo) => {
            console.log('收到位置更新:', location);
            if (this.isRunning && !this.isPaused) {
              this.currentLocation = location;
              this.routePoints.push(location);
              this.routeData.updateCurrentLocation(location);
              this.routeData.addRoutePoint(location);
              this.sportService.updateLocation(location);
              this.sportData = this.sportService.getCurrentSportData();
            }
          });

          if (!success) {
            console.error('启动位置更新失败，自动切换到模拟模式');
            this.errorMessage = '启动位置追踪失败，自动切换到模拟模式';
            // 启动真实位置服务失败，自动切换到模拟模式
            this.locationService.stopLocationUpdate();
            this.isSimulator = true;
            
            // 模拟器模式下，设置初始位置
            const initialLocation = new LocationInfo(
              39.9042, // 北京天安门附近
              116.4074,
              0,
              0,
              0,
              Date.now(),
              0,
              0,
              0
            );
            this.currentLocation = initialLocation;
            this.routeData.updateCurrentLocation(initialLocation);
            this.routeData.addRoutePoint(initialLocation);
            
            this.startSimulation();
          }
        } catch (err) {
          console.error('启动位置更新发生异常，自动切换到模拟模式:', err);
          this.errorMessage = '位置服务异常，自动切换到模拟模式';
          // 发生异常，自动切换到模拟模式
          this.locationService.stopLocationUpdate();
          this.isSimulator = true;
          
          // 模拟器模式下，设置初始位置
          const initialLocation = new LocationInfo(
            39.9042, // 北京天安门附近
            116.4074,
            0,
            0,
            0,
            Date.now(),
            0,
            0,
            0
          );
          this.currentLocation = initialLocation;
          this.routeData.updateCurrentLocation(initialLocation);
          this.routeData.addRoutePoint(initialLocation);
          
          this.startSimulation();
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
    // 暂停时停止UI更新定时器
    this.stopUIUpdateTimer();
    this.sportData = this.sportService.getCurrentSportData();
  }

  resumeSport() {
    this.sportService.resumeSport();
    this.isPaused = false;
    // 恢复时重新启动UI更新定时器
    this.startUIUpdateTimer();
  }

  stopSport() {
    const finalData = this.sportService.stopSport();
    this.locationService.stopLocationUpdate();
    this.stopSimulation();
    this.stopUIUpdateTimer();
    this.isRunning = false;
    this.isPaused = false;
    this.currentLocation = null;
    
    if (finalData) {
      this.historyService.saveSportRecord(finalData).then(async () => {
        // 检查并更新成就
        const newlyUnlocked = await this.achievementService.checkAndUpdateAchievements(finalData);
        if (newlyUnlocked.length > 0) {
          console.log(`解锁了 ${newlyUnlocked.length} 个成就`);
          // 显示成就解锁提示
          promptAction.showToast({
            message: `🎉 解锁了 ${newlyUnlocked.length} 个成就！`,
            duration: 3000
          });
        }
      }).catch((error: Error) => {
        console.error('保存记录或检查成就失败:', error);
      });
      this.showSportSummary(finalData);
    }
  }
  
  // 启动UI更新定时器
  private startUIUpdateTimer(): void {
    // 清除之前的定时器
    this.stopUIUpdateTimer();
    
    // 创建新的定时器，每秒更新一次UI
    this.uiUpdateTimer = setInterval(() => {
      if (this.isRunning) {
        // 从运动服务获取最新的运动数据
        const latestData = this.sportService.getCurrentSportData();
        if (latestData) {
          // 更新@State变量，触发UI刷新
          this.sportData = latestData;
          console.log('UI更新定时器触发，更新运动数据');
        }
      }
    }, 1000);
  }
  
  // 停止UI更新定时器
  private stopUIUpdateTimer(): void {
    if (this.uiUpdateTimer !== -1) {
      clearInterval(this.uiUpdateTimer);
      this.uiUpdateTimer = -1;
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
      
      let lat = baseLat;
      let lon = baseLon;
      
      // 生成更加平滑的轨迹，避免出现明显的横向线段
      if (index > 0) {
        // 生成直线运动轨迹，从中心点向北偏东方向移动
        const step = 0.0005; // 每步移动的距离（约50米）
        const direction = Math.PI / 4; // 45度方向，北偏东
        
        // 根据索引生成平滑的位置，避免跳跃
        lat = baseLat + Math.sin(direction) * step * index;
        lon = baseLon + Math.cos(direction) * step * index;
      }
      
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
      
      index++;
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
    promptAction.showDialog({
      title: '运动完成',
      message: `运动类型: ${this.getSportTypeName()}\n距离: ${data.distance.toFixed(2)}米\n时间: ${this.formatDuration(data.duration)}\n平均速度: ${data.averageSpeed.toFixed(1)}米/秒\n消耗卡路里: ${data.calories.toFixed(0)}`,
      buttons: [
        {
          text: '确定',
          color: '#007AFF'
        }
      ]
    }).then(() => {
      this.sportData = null;
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
  
  // 获取当前定位方法
  private getCurrentLocation(): void {
    console.log('开始获取当前定位');
    const context = getContext(this) as common.UIAbilityContext;
    
    // 检查并请求定位权限
    this.locationService.requestPermissions(context).then((hasPermission) => {
      console.log('权限检查结果:', hasPermission);
      if (!hasPermission) {
        this.errorMessage = '位置权限被拒绝，无法获取定位';
        return;
      }
      
      // 获取当前定位
      console.log('权限已获取，开始请求真实定位');
      this.locationService.getCurrentLocation().then((result) => {
        console.log('定位结果返回:', result);
        if (result.location) {
          this.currentLocation = result.location;
          console.log('获取到真实定位:', result.location);
          this.errorMessage = '';
          
          // 更新routeData的当前位置
          this.routeData.updateCurrentLocation(result.location);
          this.routeData.addRoutePoint(result.location);
          
          // 显示成功提示
          promptAction.showToast({
            message: '获取真实定位成功',
            duration: 2000
          });
        } else {
          if (result.isLocationSwitchOff) {
            console.error('位置开关处于关闭状态');
            this.errorMessage = '位置开关处于关闭状态，请开启后重试';
            
            // 显示位置开关关闭提示
            promptAction.showToast({
              message: '位置开关关闭，请开启定位服务',
              duration: 2000
            });
          } else {
            console.error('真实定位失败，检查模拟器GPS是否开启');
            this.errorMessage = '获取真实定位失败，请检查设备GPS是否开启';
            
            // 显示失败提示
            promptAction.showToast({
              message: '真实定位失败',
              duration: 2000
            });
          }
        }
      });
    });
  }
  
  // 跳转到定位设置
  private openLocationSettings(): void {
    const context = getContext(this) as common.UIAbilityContext;
    
    try {
      context.startAbility({
        uri: 'location_manager_settings'
      });
      console.log('已跳转到定位设置');
    } catch (error) {
      console.error('跳转定位设置失败:', error);
      promptAction.showToast({
        message: '跳转定位设置失败',
        duration: 2000
      });
    }
  }
}
