import { HistoryService } from '../service/HistoryService';
import { SportData } from '../model/SportModel';
import { common } from '@kit.AbilityKit';
import router from '@ohos.router';

@Entry
@Component
struct HistoryPage {
  @State sportRecords: SportData[] = [];
  private historyService: HistoryService = new HistoryService();

  async aboutToAppear() {
    const context = getContext(this) as common.UIAbilityContext;
    await this.historyService.init(context);
    this.loadRecords();
  }

  async loadRecords() {
    this.sportRecords = await this.historyService.getSportRecords();
    console.log('加载运动记录:', this.sportRecords);
  }

  build() {
    Column() {
      Row() {
        Text('📊 运动历史')
          .fontSize(24)
          .fontWeight(FontWeight.Bold)
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
      .padding({ left: 16, right: 16, top: 20, bottom: 20 })

      if (this.sportRecords.length === 0) {
        Column() {
          Text('暂无运动记录')
            .fontSize(16)
            .fontColor('#999999')
            .margin({ top: 40 })
        }
        .width('100%')
        .height('100%')
        .justifyContent(FlexAlign.Center)
      } else {
        List({ space: 12 }) {
          ForEach(this.sportRecords, (record: SportData, index: number) => {
            ListItem() {
              this.RecordItem(record, index)
            }
          }, (record: SportData, index: number) => `${index}_${record.startTime}`)
        }
        .width('100%')
        .layoutWeight(1)
        .padding({ left: 16, right: 16 })
      }
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F8F9FA')
  }

  @Builder
  RecordItem(record: SportData, index: number) {
    Column() {
      Row() {
        Column() {
          Text(this.getSportTypeName(record.type))
            .fontSize(18)
            .fontWeight(FontWeight.Bold)
            .fontColor('#333333')
            .margin({ bottom: 4 })
          Text(this.formatDate(record.startTime))
            .fontSize(12)
            .fontColor('#999999')
        }
        .alignItems(HorizontalAlign.Start)

        Blank()

        Column() {
          Text(`${this.formatDistance(record.distance)}`)
            .fontSize(20)
            .fontWeight(FontWeight.Bold)
            .fontColor('#4ECDC4')
            .margin({ bottom: 4 })
          Text(`${this.formatDuration(record.duration)}`)
            .fontSize(12)
            .fontColor('#999999')
        }
        .alignItems(HorizontalAlign.End)
      }
      .width('100%')
      .justifyContent(FlexAlign.SpaceBetween)

      Row() {
        this.StatItem('平均速度', this.formatSpeed(record.averageSpeed), '#45B7D1')
        this.StatItem('消耗卡路里', this.formatCalories(record.calories), '#FFA502')
      }
      .width('100%')
      .justifyContent(FlexAlign.SpaceEvenly)
      .margin({ top: 12 })
    }
    .width('100%')
    .padding(16)
    .backgroundColor('#FFFFFF')
    .borderRadius(12)
    .shadow({ radius: 8, color: '#1A000000', offsetX: 0, offsetY: 2 })
  }

  @Builder
  StatItem(label: string, value: string, color: string) {
    Column() {
      Text(value)
        .fontSize(16)
        .fontWeight(FontWeight.Bold)
        .fontColor(color)
        .margin({ bottom: 4 })
      Text(label)
        .fontSize(12)
        .fontColor('#999999')
    }
    .alignItems(HorizontalAlign.Center)
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
