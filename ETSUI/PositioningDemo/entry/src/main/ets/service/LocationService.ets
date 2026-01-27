import { LocationInfo } from '../model/SportModel';
import { geoLocationManager } from '@kit.LocationKit';
import { abilityAccessCtrl, Permissions } from '@kit.AbilityKit';

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
      const requestInfo: geoLocationManager.LocationRequest = {
        'priority': geoLocationManager.LocationRequestPriority.FIRST_FIX,
        'scenario': geoLocationManager.LocationRequestScenario.UNSET,
        'timeInterval': 1,
        'distanceInterval': 0,
        'maxAccuracy': 0
      };

      await geoLocationManager.on('locationChange', requestInfo, (location) => {
        console.log('收到位置更新:', location);
        const locationInfo = new LocationInfo(
          location.latitude,
          location.longitude,
          location.altitude,
          location.accuracy,
          location.speed,
          location.timeStamp,
          0,
          0,
          0
        );
        this.locationChange(locationInfo);
      });

      this.isTracking = true;
      console.log('位置追踪已启动');
      return true;
    } catch (err) {
      console.error('启动位置追踪失败:', err);
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
}
