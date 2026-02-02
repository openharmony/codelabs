/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-20
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
import { SportData, RouteData, LocationInfo, DataPoint } from '../model/SportModel';
import router from '@ohos.router';
import { RouteMapInner } from '../components/RouteMapInner';
import { ChartComponent } from '../components/ChartComponent';

// 移除可能不存在的导入，使用默认值

@Entry
@Component
struct HistoryDetailPage {
  @State sportRecord: SportData | null = null;
  @State routeData: RouteData = new RouteData();

  aboutToAppear() {
    // 获取从HistoryPage传递过来的运动记录数据
    const params = router.getParams();
    console.log('获取到路由参数:', params);

    // 安全地获取record参数
    let recordJson = '';
    if (params) {
      // 使用类型断言，避免any类型
      interface RecordParams {
        record?: string;
      }

      const parsedParams = params as RecordParams;
      if (parsedParams.record) {
        recordJson = parsedParams.record;
        this.sportRecord = JSON.parse(recordJson) as SportData;
        console.log('获取到运动记录 - 类型:', this.sportRecord.type, '距离:', this.sportRecord.distance, '米', '时间:',
          this.sportRecord.duration, '毫秒', '卡路里:', this.sportRecord.calories, 'kcal');
        console.log('速度数据点数量:', this.sportRecord.speedData ? this.sportRecord.speedData.length : 0);
        console.log('卡路里数据点数量:', this.sportRecord.caloriesData ? this.sportRecord.caloriesData.length : 0);
        if (this.sportRecord.speedData && this.sportRecord.speedData.length > 0) {
          console.log('前5个速度数据点:');
          this.sportRecord.speedData.slice(0, 5).forEach((point, index) => {
            console.log(`  数据点${index + 1} - 时间戳:`, point.timestamp, '速度:', point.value, 'm/s');
          });
        }
        if (this.sportRecord.caloriesData && this.sportRecord.caloriesData.length > 0) {
          console.log('前5个卡路里数据点:');
          this.sportRecord.caloriesData.slice(0, 5).forEach((point, index) => {
            console.log(`  数据点${index + 1} - 时间戳:`, point.timestamp, '卡路里:', point.value, 'kcal');
          });
        }

        // 初始化routeData，用于显示轨迹
        if (this.sportRecord && this.sportRecord.route) {
          this.routeData = new RouteData();
          // 将运动记录中的route转换为LocationInfo格式，用于显示轨迹
          this.sportRecord.route.forEach(routePoint => {
            // 显式创建LocationInfo实例，避免未类型化对象字面量
            const locationInfo = new LocationInfo(
              routePoint.latitude,
              routePoint.longitude,
              0, // altitude
              0, // accuracy
              0, // speed
              routePoint.timestamp,
              0, // totalDistance
              0, // netDistance
              0// netDuration
            );
            this.routeData.addRoutePoint(locationInfo);
          });

          // 设置当前位置为最后一个轨迹点
          if (this.routeData.routePoints.length > 0) {
            const lastPoint = this.routeData.routePoints[this.routeData.routePoints.length - 1];
            this.routeData.updateCurrentLocation(lastPoint);
          }
        }
      }
    }
  }

  build() {
    Column() {
      // 页面标题和返回按钮
      Row() {
        Text('🏃 运动详情')
          .fontSize(24)
          .fontWeight(700)
          .fontColor('#333333')

        Blank()

        Text('返回')
          .fontSize(16)
          .fontColor('#4ECDC4')
          .onClick(() => {
            router.back();
          })
      }
      .width('100%')
      .padding({
        left: 16,
        right: 16,
        top: 16,
        bottom: 16
      })

      // 添加滚动组件
      Scroll() {
        Column() {
          if (this.sportRecord) {
            // 运动概览卡片
            Column() {
              Text(this.getSportTypeName(this.sportRecord.type))
                .fontSize(20)
                .fontWeight(700)
                .fontColor('#333333')
                .margin({ bottom: 6 })

              Text(this.formatDate(this.sportRecord.startTime))
                .fontSize(14)
                .fontColor('#999999')
                .margin({ bottom: 12 })

              // 运动数据概览
              Row() {
                this.StatItem('距离', this.formatDistance(this.sportRecord.distance), '#FF6B6B')
                this.StatItem('时间', this.formatDuration(this.sportRecord.duration), '#4ECDC4')
                this.StatItem('平均速度', this.formatSpeed(this.sportRecord.averageSpeed), '#45B7D1')
                this.StatItem('卡路里', this.formatCalories(this.sportRecord.calories), '#FFA502')
              }
              .width('100%')
              .justifyContent(FlexAlign.SpaceAround)
            }
            .width('100%')
            .padding(16)
            .backgroundColor('#FFFFFF')
            .borderRadius(16)
            .shadow({
              radius: 6,
              color: '#1A000000',
              offsetX: 0,
              offsetY: 2
            })
            .margin({ bottom: 12 })

            // 运动轨迹地图
            Column() {
              Text('📍 运动轨迹')
                .fontSize(18)
                .fontWeight(700)
                .fontColor('#333333')
                .margin({ bottom: 10 })

              RouteMapInner({
                routeData: $routeData
              })
            }
            .width('100%')
            .padding(16)
            .backgroundColor('#FFFFFF')
            .borderRadius(16)
            .shadow({
              radius: 6,
              color: '#1A000000',
              offsetX: 0,
              offsetY: 2
            })
            .margin({ bottom: 12 })

            // 速度变化图表
            if (this.sportRecord.speedData && this.sportRecord.speedData.length > 0) {
              ChartComponent({
                data: this.sportRecord.speedData,
                title: '速度变化趋势',
                xLabel: '时间',
                yLabel: '速度 (m/s)',
                color: '#4ECDC4'
              })
                .width('100%')
                .margin({ bottom: 12 })
            }

            // 卡路里增长图表
            if (this.sportRecord.caloriesData && this.sportRecord.caloriesData.length > 0) {
              ChartComponent({
                data: this.sportRecord.caloriesData,
                title: '卡路里增长趋势',
                xLabel: '时间',
                yLabel: '卡路里',
                color: '#FF6B6B'
              })
                .width('100%')
                .margin({ bottom: 12 })
            }

            // 运动数据详情
            Column() {
              Text('📈 运动数据')
                .fontSize(18)
                .fontWeight(700)
                .fontColor('#333333')
                .margin({ bottom: 12 })

              // 速度信息
              Row() {
                Text('最高速度:')
                  .fontSize(16)
                  .fontColor('#666666')
                Blank()
                Text(`${(this.sportRecord.currentSpeed || 0).toFixed(1)} m/s`)
                  .fontSize(16)
                  .fontWeight(700)
                  .fontColor('#45B7D1')
              }
              .width('100%')
              .padding({ top: 10, bottom: 10 })

              // 路线信息
              Row() {
                Text('路线点数量:')
                  .fontSize(16)
                  .fontColor('#666666')
                Blank()
                Text(`${this.sportRecord.route ? this.sportRecord.route.length : 0} 个`)
                  .fontSize(16)
                  .fontWeight(700)
                  .fontColor('#333333')
              }
              .width('100%')
              .padding({ top: 10, bottom: 10 })
            }
            .width('100%')
            .padding(16)
            .backgroundColor('#FFFFFF')
            .borderRadius(16)
            .shadow({
              radius: 6,
              color: '#1A000000',
              offsetX: 0,
              offsetY: 2
            })
            .margin({ bottom: 24 })
          } else {
            // 加载失败提示
            Column() {
              Text('加载运动记录失败')
                .fontSize(16)
                .fontColor('#999999')
            }
            .width('100%')
            .height(300)
            .justifyContent(FlexAlign.Center)
          }
        }
        .width('100%')
        .padding({ left: 16, right: 16 })
      }
      .width('100%')
      .height('100%')
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F8F9FA')
  }

  @Builder
  StatItem(label: string, value: string, color: string) {
    Column() {
      Text(value)
        .fontSize(20)
        .fontWeight(700) // 使用数字值代替FontWeight枚举
        .fontColor(color)
        .margin({ bottom: 4 })
      Text(label)
        .fontSize(12)
        .fontColor('#999999')
    }

    // 移除alignItems，使用默认对齐方式
  }

  getSportTypeName(type: string): string {
    switch (type) {
      case 'running':
        return '跑步';
      case 'walking':
        return '行走';
      case 'cycling':
        return '骑行';
      default:
        return '运动';
    }
  }

  formatDate(timestamp: number): string {
    const date = new Date(timestamp);
    const year = date.getFullYear();
    const month = String(date.getMonth() + 1).padStart(2, '0');
    const day = String(date.getDate()).padStart(2, '0');
    const hours = String(date.getHours()).padStart(2, '0');
    const minutes = String(date.getMinutes()).padStart(2, '0');
    return `${year}-${month}-${day} ${hours}:${minutes}`;
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
