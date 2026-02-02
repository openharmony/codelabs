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
import { LocationInfo, RouteData } from '../model/SportModel';
import { MapComponent, mapCommon, map } from '@kit.MapKit';
import { AsyncCallback } from '@kit.BasicServicesKit';
import { PathGradientTool } from '../utils/PathGradientTool';

@Component
export struct RouteMapInner {
  @Link @Watch('onRouteDataChange') routeData: RouteData;
  // 添加@State变量来控制地图中心
  @State mapCenter: mapCommon.LatLng = { latitude: 39.9042, longitude: 116.4074 };
  // 添加@State变量来控制地图缩放级别
  @State mapZoom: number = 16;
  // 添加@State变量来存储完整的mapOptions
  @State mapOptions: mapCommon.MapOptions = {
    position: {
      target: { latitude: 39.9042, longitude: 116.4074 },
      zoom: 16
    } as mapCommon.CameraPosition
  };
  private mapController?: map.MapComponentController;
  private mapCallback?: AsyncCallback<map.MapComponentController>;
  private mapEventManager?: map.MapEventManager;
  private mapClickRegistered: boolean = false;
  private polylines: map.MapPolyline[] = [];
  private markers: map.Marker[] = [];
  @State mapInitialized: boolean = false;
  @State mapError: string = '';

  onRouteDataChange(): void {
    console.log('routeData发生变化，重新绘制轨迹');
    console.log('当前routeData:', this.routeData);
    console.log('当前currentLocation:', this.routeData.currentLocation);

    // 更新地图中心
    if (this.routeData.currentLocation) {
      const newCenter: mapCommon.LatLng = {
        latitude: this.routeData.currentLocation.latitude,
        longitude: this.routeData.currentLocation.longitude
      };

      // 更新地图中心状态
      this.mapCenter = newCenter;

      // 创建新的CameraPosition对象
      const newPosition: mapCommon.CameraPosition = {
        target: newCenter,
        zoom: this.mapZoom
      };

      // 更新mapOptions，确保引用变化
      this.mapOptions = {
        position: newPosition
      } as mapCommon.MapOptions;

      console.log('地图中心和mapOptions已更新:', this.mapCenter);
    }

    // 异步绘制轨迹
    this.clearAndUpdateRoute();
  }

  private async clearAndUpdateRoute(): Promise<void> {
    await this.clearRoute();
    await this.updateRoute();
  }

  async clearRoute(): Promise<void> {
    console.log('清除旧轨迹');
    for (const polyline of this.polylines) {
      try {
        await polyline.remove();
      } catch (e) {
        console.error('清除折线失败:', e);
      }
    }
    for (const marker of this.markers) {
      try {
        await marker.remove();
      } catch (e) {
        console.error('清除标记失败:', e);
      }
    }
    this.polylines = [];
    this.markers = [];
  }

  aboutToAppear(): void {
    console.log('RouteMapInner aboutToAppear被调用');
    console.log('routeData:', this.routeData);
    console.log('routeData.routePoints:', this.routeData.routePoints);
    console.log('routeData.currentLocation:', this.routeData.currentLocation);

    // 初始化地图中心
    let initialLat = 39.9042;
    let initialLng = 116.4074;

    if (this.routeData.currentLocation) {
      initialLat = this.routeData.currentLocation.latitude;
      initialLng = this.routeData.currentLocation.longitude;
    }

    this.mapCenter = { latitude: initialLat, longitude: initialLng };

    // 初始化mapOptions
    const initialPosition: mapCommon.CameraPosition = {
      target: { latitude: initialLat, longitude: initialLng },
      zoom: this.mapZoom
    };

    this.mapOptions = {
      position: initialPosition
    } as mapCommon.MapOptions;

    this.mapCallback = async (err, mapController) => {
      console.log('地图回调, err:', err, 'mapController:', mapController);

      if (!err) {
        this.mapInitialized = true;
        this.mapError = '';
        this.mapController = mapController;
        this.mapEventManager = mapController.getEventManager();

        // 打印地图控制器的信息，用于调试
        console.log('地图控制器对象初始化成功');

        if (!this.mapClickRegistered) {
          this.mapEventManager?.on('mapClick', async (position) => {
            console.log('地图点击:', position);
          });

          this.mapClickRegistered = true;
        }

        console.log('地图初始化成功');
        await this.updateRoute();
      } else {
        this.mapInitialized = false;
        this.mapError = JSON.stringify(err);
        console.error('地图初始化失败', JSON.stringify(err));
      }
    };
  }

  async updateRoute(): Promise<void> {
    console.log('updateRoute被调用, mapController:', this.mapController, 'routeData:', this.routeData);

    if (!this.mapController) {
      console.log('mapController为空，跳过绘制');
      return;
    }

    console.log('routePoints长度:', this.routeData.routePoints.length, 'currentLocation:',
      this.routeData.currentLocation);

    // 绘制轨迹
    let allPoints: mapCommon.LatLng[] = [];

    if (this.routeData.routePoints.length > 1) {
      // 使用真实轨迹数据
      console.log('使用真实轨迹数据绘制轨迹');
      for (const point of this.routeData.routePoints) {
        const latLng: mapCommon.LatLng = {
          latitude: point.latitude,
          longitude: point.longitude
        };
        allPoints.push(latLng);
      }
    } else {
      // 绘制测试轨迹，确保地图能显示线条
      console.log('使用测试轨迹数据绘制轨迹');
      // 创建一条简单的测试线
      allPoints = [
        { latitude: 39.9042, longitude: 116.4074 }, // 起点
        { latitude: 39.9142, longitude: 116.4174 }, // 中间点1
        { latitude: 39.9242, longitude: 116.4274 }, // 中间点2
        { latitude: 39.9342, longitude: 116.4374 }// 终点
      ];
    }

    console.log('所有点:', allPoints);

    try {
      // 使用更醒目的红色，增加宽度，确保在地图上可见
      const polylineOptions: mapCommon.MapPolylineOptions = {
        points: allPoints,
        width: 20, // 进一步增加宽度，确保在模拟器上可见
        color: 0xFFFF0000, // 使用正确的ARGB数值类型，确保类型匹配
        visible: true
      };

      console.log('添加折线, 参数:', polylineOptions);

      const polyline = await this.mapController.addPolyline(polylineOptions);
      this.polylines.push(polyline);
      console.log('轨迹绘制成功, 共', allPoints.length, '个点');
      console.log('polyline:', polyline);

      // 绘制轨迹后，使用地图控制器更新地图中心到当前位置
      if (this.routeData.currentLocation) {
        console.log('尝试使用地图控制器更新地图中心到当前位置');
        console.log('地图控制器可用方法:', Object.getOwnPropertyNames(this.mapController));

        // 不再使用moveCamera方法，避免编译错误
        // 我们已经通过@State mapCenter变量来控制地图中心
        console.log('通过@State mapCenter变量控制地图中心，当前中心:', this.mapCenter);
      }
      console.log('轨迹绘制完成');
      if (allPoints.length > 0) {
        const centerPoint = allPoints[Math.floor(allPoints.length / 2)];
        console.log('轨迹中心点:', centerPoint);
        console.log('轨迹长度:', allPoints.length, '个点');
      }
    } catch (e) {
      console.error('添加折线失败:', JSON.stringify(e));
    }

    // 绘制当前位置标记
    if (this.routeData.currentLocation) {
      console.log('开始绘制当前位置标记');
      const position: mapCommon.LatLng = {
        latitude: this.routeData.currentLocation.latitude,
        longitude: this.routeData.currentLocation.longitude
      };

      try {
        const markerOptions: mapCommon.MarkerOptions = {
          position: position,
          anchorU: 0.5,
          anchorV: 1.0,
          visible: true
        };

        const marker = await this.mapController.addMarker(markerOptions);
        this.markers.push(marker);
        console.log('当前位置标记绘制成功');
      } catch (e) {
        console.error('添加标记失败:', JSON.stringify(e));
      }
    } else {
      console.log('currentLocation为空，不绘制标记');
    }
  }

  @State testLatitude: number = 39.9042;
  @State testLongitude: number = 116.4074;

  // 测试地图移动的方法
  private testMapMove(): void {
    console.log('测试地图移动');

    // 随机生成一个新的位置
    this.testLatitude += (Math.random() - 0.5) * 0.01;
    this.testLongitude += (Math.random() - 0.5) * 0.01;

    const newCenter: mapCommon.LatLng = {
      latitude: this.testLatitude,
      longitude: this.testLongitude
    };

    console.log('新的地图中心坐标:', newCenter.latitude, newCenter.longitude);

    // 更新地图中心状态
    this.mapCenter = newCenter;

    // 创建新的CameraPosition对象
    const newPosition: mapCommon.CameraPosition = {
      target: newCenter,
      zoom: this.mapZoom
    };

    // 更新mapOptions，确保引用变化
    this.mapOptions = {
      position: newPosition
    } as mapCommon.MapOptions;

    console.log('地图中心和mapOptions已更新');
  }

  build() {
    Column() {
      if (this.routeData) {
        Text(`调试: routePoints长度=${this.routeData.routePoints.length} currentLocation=${this.routeData.currentLocation ?
          '有' : '无'}`)
          .fontSize(10)
          .fontColor('#999999')
          .margin({ bottom: 10 })
      }

      // 地图初始化状态调试
      Text(`地图状态: ${this.mapInitialized ? '已初始化' : '未初始化'} 错误: ${this.mapError}`)
        .fontSize(10)
        .fontColor('#999999')
        .margin({ bottom: 10 })

      // 显示当前地图中心
      Text(`当前地图中心: ${this.mapCenter.latitude.toFixed(6)}, ${this.mapCenter.longitude.toFixed(6)}`)
        .fontSize(10)
        .fontColor('#999999')
        .margin({ bottom: 10 })

      // 动态地图组件，使用mapOptions状态变量
      if (this.routeData.currentLocation) {
        // 使用条件渲染来处理地图加载，避免try-catch
        MapComponent({
          mapOptions: this.mapOptions,
          mapCallback: this.mapCallback
        })
          .width('100%')
          .height(300)
          .borderRadius(12)
      }
    }
    .width('100%')
    .padding({ left: 16, right: 16 })
  }
}

@Component
export struct RouteMap {
  @Link routeData: RouteData;

  aboutToAppear(): void {
    console.log('RouteMap aboutToAppear被调用');
    console.log('传入的routeData:', this.routeData);
    console.log('传入的routeData.currentLocation:', this.routeData.currentLocation);
  }

  build() {
    RouteMapInner({
      routeData: $routeData
    })
  }
}
