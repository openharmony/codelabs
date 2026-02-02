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

// 定义LatLng接口
interface LatLng {
  latitude: number;
  longitude: number;
}

@Component
export struct RouteMapInner {
  @Link @Watch('onRouteDataChange') routeData: RouteData;
  // 添加@State变量来存储当前位置
  @State currentLocation: LatLng = { latitude: 39.9042, longitude: 116.4074 };
  @State routePointsCount: number = 0;
  @State isLocationAvailable: boolean = false;

  onRouteDataChange(): void {
    console.log('routeData发生变化，更新轨迹信息');
    console.log('当前routeData:', this.routeData);
    console.log('当前currentLocation:', this.routeData.currentLocation);

    // 更新当前位置
    if (this.routeData.currentLocation) {
      const newLocation: LatLng = {
        latitude: this.routeData.currentLocation.latitude,
        longitude: this.routeData.currentLocation.longitude
      };

      // 更新当前位置状态
      this.currentLocation = newLocation;
      this.isLocationAvailable = true;
      this.routePointsCount = this.routeData.routePoints.length;

      console.log('位置信息已更新:', this.currentLocation);
    }
  }

  aboutToAppear(): void {
    console.log('RouteMapInner aboutToAppear被调用');
    console.log('routeData:', this.routeData);
    console.log('routeData.routePoints:', this.routeData.routePoints);
    console.log('routeData.currentLocation:', this.routeData.currentLocation);

    // 初始化当前位置
    if (this.routeData.currentLocation) {
      this.currentLocation = {
        latitude: this.routeData.currentLocation.latitude,
        longitude: this.routeData.currentLocation.longitude
      };
      this.isLocationAvailable = true;
    }

    this.routePointsCount = this.routeData.routePoints.length;
  }

  build() {
    Column() {
      // 显示当前位置
      if (this.isLocationAvailable) {
        Column() {
          Text('当前位置:')
            .fontSize(12)
            .fontColor('#666666')
            .margin({ bottom: 4 })

          Text(`${this.currentLocation.latitude.toFixed(6)}, ${this.currentLocation.longitude.toFixed(6)}`)
            .fontSize(14)
            .fontColor('#333333')
            .margin({ bottom: 10 })
        }
      } else {
        Text('正在获取位置信息...')
          .fontSize(12)
          .fontColor('#999999')
          .margin({ bottom: 10 })
      }
    }
    .width('100%')
    .padding({ left: 16, right: 16 })
    .backgroundColor('#FFFFFF')
    .borderRadius(12)
    .padding(16)
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
