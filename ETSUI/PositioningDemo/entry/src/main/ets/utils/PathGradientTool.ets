// SPDX-License-Identifier: MIT
import { LocationInfo } from '../model/SportModel';

export class PathGradientTool {
  static getPathColors(points: LocationInfo[], colorInterval: number): string[] {
    if (!points || points.length < 2) {
      return [];
    }

    console.log('PathGradientTool: 开始计算颜色, 点数:', points.length, '间隔:', colorInterval);
    
    const colors: string[] = [];
    const speeds: number[] = [];
    
    for (let i = 1; i < points.length; i++) {
      const prevPoint = points[i - 1];
      const currPoint = points[i];
      
      const distance = currPoint.totalDistance - prevPoint.totalDistance;
      const duration = currPoint.netDuration - prevPoint.netDuration;
      
      let speed = 0;
      if (duration > 0 && distance > 0) {
        speed = distance / duration;
      }
      
      speeds.push(speed);
    }
    
    console.log('PathGradientTool: 计算得到速度数组:', speeds);
    
    const maxSpeed = Math.max(...speeds);
    const minSpeed = Math.min(...speeds);
    const range = maxSpeed - minSpeed;
    
    console.log('PathGradientTool: 最大速度:', maxSpeed, '最小速度:', minSpeed, '范围:', range);
    
    for (let i = 0; i < speeds.length; i++) {
      const speed = speeds[i];
      const color = PathGradientTool.getSpeedColor(speed, minSpeed, maxSpeed, range);
      colors.push(color);
    }
    
    console.log('PathGradientTool: 生成颜色数组:', colors, '长度:', colors.length);
    
    return colors;
  }

  private static getSpeedColor(speed: number, minSpeed: number, maxSpeed: number, range: number): string {
    if (range === 0 || speed === 0) {
      return '#4ECDC4';
    }
    
    const ratio = (speed - minSpeed) / range;
    
    if (ratio <= 0.2) {
      return '#FF3032';
    } else if (ratio <= 0.4) {
      return '#FA7B22';
    } else if (ratio <= 0.6) {
      return '#F5BE14';
    } else if (ratio <= 0.8) {
      return '#7AC36C';
    } else {
      return '#00C8C3';
    }
  }
}
