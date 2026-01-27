import { LocationInfo, RouteData } from '../model/SportModel';
import { MapComponent, mapCommon, map } from '@kit.MapKit';
import { AsyncCallback } from '@kit.BasicServicesKit';
import { PathGradientTool } from '../utils/PathGradientTool';

@Component
export struct RouteMapInner {
  @Link @Watch('onRouteDataChange') routeData: RouteData;
  
  private mapController?: map.MapComponentController;
  private mapOptions?: mapCommon.MapOptions;
  private mapCallback?: AsyncCallback<map.MapComponentController>;
  private mapEventManager?: map.MapEventManager;
  private mapClickRegistered: boolean = false;
  private polylines: map.MapPolyline[] = [];
  private markers: map.Marker[] = [];

  async onRouteDataChange(): Promise<void> {
    console.log('routeData发生变化，重新绘制轨迹');
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
    
    const initialLat = this.routeData.currentLocation?.latitude || 39.9;
    const initialLon = this.routeData.currentLocation?.longitude || 116.4;
    
    this.mapOptions = {
      position: {
        target: {
          latitude: initialLat,
          longitude: initialLon
        },
        zoom: 14
      }
    };

    this.mapCallback = async (err, mapController) => {
      console.log('地图回调, err:', err, 'mapController:', mapController);
      
      if (!err) {
        this.mapController = mapController;
        this.mapEventManager = mapController.getEventManager();

        if (!this.mapClickRegistered) {
          this.mapEventManager?.on('mapClick', async (position) => {
            console.log('地图点击:', position);
          });

          this.mapClickRegistered = true;
        }

        console.log('地图初始化成功');
        await this.updateRoute();
      } else {
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

    console.log('routePoints长度:', this.routeData.routePoints.length, 'currentLocation:', this.routeData.currentLocation);

    if (this.routeData.routePoints.length > 1) {
      console.log('开始绘制速度轨迹');
      
      const colors = PathGradientTool.getPathColors(this.routeData.routePoints, 100);
      console.log('获取到颜色数组:', colors, '长度:', colors?.length);
      
      if (colors && colors.length > 0) {
        for (let i = 0; i < this.routeData.routePoints.length - 1; i++) {
          const point1 = this.routeData.routePoints[i];
          const point2 = this.routeData.routePoints[i + 1];
          
          const coordinates: mapCommon.LatLng[] = [
            {
              latitude: point1.latitude,
              longitude: point1.longitude
            },
            {
              latitude: point2.latitude,
              longitude: point2.longitude
            }
          ];

          const color = colors[i] || '#4ECDC4';
          const colorValue = parseInt('0x' + color.substring(1), 16);
          const polylineOptions: mapCommon.MapPolylineOptions = {
            points: coordinates,
            width: 12,
            color: colorValue,
            visible: true
          };

          console.log(`绘制第${i}段轨迹, 颜色: ${color}, 坐标:`, coordinates, '颜色值:', colorValue);

          try {
            const polyline = await this.mapController.addPolyline(polylineOptions);
            this.polylines.push(polyline);
            console.log(`第${i}段轨迹绘制成功`);
          } catch (e) {
            console.error('添加折线失败:', e);
          }
        }
        console.log('绘制速度轨迹完成, 共绘制', this.polylines.length, '段');
      } else {
        console.log('颜色数组为空，无法绘制轨迹');
      }
    } else {
      console.log('routePoints少于2个点，不绘制轨迹');
    }

    if (this.routeData.currentLocation) {
      console.log('开始绘制当前位置标记');
      const position: mapCommon.LatLng = {
        latitude: this.routeData.currentLocation.latitude,
        longitude: this.routeData.currentLocation.longitude
      };

      const markerOptions: mapCommon.MarkerOptions = {
        position: position,
        anchorU: 0.5,
        anchorV: 1.0,
        visible: true
      };

      try {
        const marker = await this.mapController.addMarker(markerOptions);
        this.markers.push(marker);
        console.log('绘制当前位置标记完成');
      } catch (e) {
        console.error('添加标记失败:', e);
      }
    } else {
      console.log('currentLocation为空，不绘制标记');
    }
  }

  build() {
    Column() {
      if (this.routeData) {
        Text(`调试: routePoints长度=${this.routeData.routePoints.length} currentLocation=${this.routeData.currentLocation ? '有' : '无'}`)
          .fontSize(10)
          .fontColor('#999999')
          .margin({ bottom: 10 })
      }

      MapComponent({
        mapOptions: this.mapOptions,
        mapCallback: this.mapCallback
      })
        .width('100%')
        .height(300)
        .borderRadius(12)
    }
    .width('100%')
    .padding({ left: 16, right: 16 })
  }
}

@Component
export struct RouteMap {
  @Prop routeData: RouteData;
  @State innerRouteData: RouteData = new RouteData();

  aboutToAppear(): void {
    console.log('RouteMap aboutToAppear被调用');
    this.innerRouteData = this.routeData;
  }

  build() {
    RouteMapInner({
      routeData: $innerRouteData
    })
  }
}
