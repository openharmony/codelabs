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
import { LocationInfo } from '../model/SportModel';
import { geoLocationManager } from '@kit.LocationKit';
import { abilityAccessCtrl, Permissions, Context } from '@kit.AbilityKit';

// 定义定位结果类型，包含是否是位置开关关闭的错误
export interface LocationResult {
  location: LocationInfo | null;
  isLocationSwitchOff: boolean;
}

export class LocationService {
  private locationChange: (location: LocationInfo) => void = () => {};
  private isTracking: boolean = false;

  async requestPermissions(context: Context): Promise<boolean> {
    const atManager = abilityAccessCtrl.createAtManager();
    const permissions: Array<Permissions> = ['ohos.permission.LOCATION', 'ohos.permission.APPROXIMATELY_LOCATION'];
    
    try {
      const result = await atManager.requestPermissionsFromUser(context, permissions);
      console.log('权限请求结果:', result);
      return result.authResults.every(authResult => authResult === 0);
    } catch (err) {
      console.error('请求权限失败:', err);
      return false;
    }
  }

  async startLocationUpdate(callback: (location: LocationInfo) => void): Promise<boolean> {
    this.locationChange = callback;
    
    try {
      // 根据不同场景调整位置请求参数
      // 对于模拟器环境，使用更宽松的参数以提高成功率
      const requestInfo: geoLocationManager.LocationRequest = {
        priority: geoLocationManager.LocationRequestPriority.FIRST_FIX,
        scenario: geoLocationManager.LocationRequestScenario.NAVIGATION,
        timeInterval: 2, // 缩短时间间隔，获取更频繁的位置更新
        distanceInterval: 5, // 缩短距离间隔
        maxAccuracy: 100, // 放宽精度要求，提高模拟器上的成功率
      };

      console.log('请求位置更新, 参数:', requestInfo);
      
      // 使用on方法监听位置变化
      await geoLocationManager.on('locationChange', requestInfo, (location) => {
        console.log('收到位置更新:', location);
        console.log('持续定位速度信息:', location.speed, '速度类型:', typeof location.speed);
        
        // 检查位置数据是否有效
        if (location && location.latitude && location.longitude) {
          const speed = location.speed !== undefined ? location.speed : 0;
          console.log('持续定位使用的速度值:', speed);
          
          const locationInfo = new LocationInfo(
            location.latitude,
            location.longitude,
            location.altitude || 0,
            location.accuracy || 0,
            speed,
            location.timeStamp || Date.now(),
            0,
            0,
            0
          );
          this.locationChange(locationInfo);
        } else {
          console.error('收到无效位置数据:', location);
        }
      });

      this.isTracking = true;
      console.log('位置追踪已启动');
      return true;
    } catch (err) {
      console.error('启动位置更新失败:', JSON.stringify(err));
      return false;
    }
  }

  stopLocationUpdate(): void {
    if (this.isTracking) {
      try {
        geoLocationManager.off('locationChange');
        this.isTracking = false;
        console.log('位置追踪已停止');
      } catch (err) {
        console.error('停止位置追踪失败:', err);
      }
    }
  }
  
  // 获取当前定位方法
  async getCurrentLocation(): Promise<LocationResult> {
    try {
      console.log('开始获取单次定位');
      
      // 为单次定位请求添加适合模拟器的参数配置
      // 放宽精度要求，提高在模拟器上的成功率
      const requestInfo: geoLocationManager.LocationRequest = {
        priority: geoLocationManager.LocationRequestPriority.FIRST_FIX,
        scenario: geoLocationManager.LocationRequestScenario.DAILY_LIFE_SERVICE,
        maxAccuracy: 200, // 放宽精度要求，提高模拟器上的成功率
        timeInterval: 3000, // 适当延长时间间隔
        distanceInterval: 0 // 不限制距离间隔
      };
      
      console.log('使用参数获取单次定位:', requestInfo);
      
      // 尝试获取真实定位
      const location = await geoLocationManager.getCurrentLocation(requestInfo);
      console.log('获取到单次定位:', location);
      console.log('定位速度信息:', location.speed, '速度类型:', typeof location.speed);
      
      if (location && location.latitude && location.longitude) {
        const speed = location.speed !== undefined ? location.speed : 0;
        console.log('使用的速度值:', speed);
        
        const locationInfo = new LocationInfo(
          location.latitude,
          location.longitude,
          location.altitude || 0,
          location.accuracy || 0,
          speed,
          location.timeStamp || Date.now(),
          0,
          0,
          0
        );
        return { location: locationInfo, isLocationSwitchOff: false };
      }
      
      // 真实定位失败，返回空位置
      console.log('真实定位失败，返回空位置');
      return { location: null, isLocationSwitchOff: false };
    } catch (err) {
      console.error('获取单次定位失败:', JSON.stringify(err));
      
      // 检查是否是位置开关关闭的错误
      const errorStr = JSON.stringify(err);
      const isLocationSwitchOff = errorStr.includes('code":3301100') || errorStr.includes('The location switch is off');
      
      if (isLocationSwitchOff) {
        console.error('位置开关处于关闭状态');
        return { location: null, isLocationSwitchOff: true };
      } else {
        console.error('位置服务可能未开启或定位失败');
        return { location: null, isLocationSwitchOff: false };
      }
    }
  }
}
