/**
 * 健康模块主页面
 */
import { LifeHubData, HealthRecord } from '../../model/LifeHubData';
import { router } from '@kit.ArkUI';

@Component
export struct HealthMain {
  private lifeHubData: LifeHubData = new LifeHubData();
  
  @State healthRecords: HealthRecord[] = [];
  @State isLoading: boolean = true;

  aboutToAppear() {
    const context = getContext(this);
    if (context) {
      this.lifeHubData.setContext(context);
    }
    this.loadHealthRecords();
  }

  async loadHealthRecords() {
    this.isLoading = true;
    try {
      this.healthRecords = await this.lifeHubData.getHealthRecords(undefined, 30);
    } catch (error) {
      console.error('HealthMain: 加载数据失败:', error);
    } finally {
      this.isLoading = false;
    }
  }

  build() {
    Column() {
      // 标题栏
      Row() {
        Text('健康记录')
          .fontSize(24)
          .fontWeight(FontWeight.Bold)
        
        Blank()
        
        Button('+')
          .fontSize(20)
          .width(40)
          .height(40)
          .borderRadius(20)
          .backgroundColor('#4CAF50')
          .fontColor(Color.White)
          .onClick(() => {
            router.pushUrl({ url: 'pages/health/HealthRecord' });
          })
      }
      .width('100%')
      .padding(20)
      .backgroundColor('#f0f0f0')

      // 内容区域
      if (this.isLoading) {
        Column() {
          LoadingProgress()
            .width(50)
            .height(50)
            .margin({ top: 100 })
        }
        .width('100%')
        .layoutWeight(1)
        .justifyContent(FlexAlign.Center)
      } else if (this.healthRecords.length === 0) {
        Column() {
          Text('💚')
            .fontSize(60)
            .margin({ bottom: 20 })
          Text('还没有健康记录')
            .fontSize(18)
            .fontColor('#666')
          Text('点击右上角"+"开始记录')
            .fontSize(14)
            .fontColor('#999')
            .margin({ top: 10 })
        }
        .width('100%')
        .layoutWeight(1)
        .justifyContent(FlexAlign.Center)
      } else {
        List() {
          ForEach(this.healthRecords, (record: HealthRecord) => {
            ListItem() {
              Row() {
                Column() {
                  Text(this.getTypeIcon(record.type))
                    .fontSize(32)
                }
                .width(60)
                .justifyContent(FlexAlign.Center)

                Column() {
                  Text(this.getTypeName(record.type))
                    .fontSize(18)
                    .fontWeight(FontWeight.Medium)
                    .textAlign(TextAlign.Start)
                    .width('100%')

                  Text(`${record.value} ${record.unit}`)
                    .fontSize(16)
                    .fontColor('#666')
                    .textAlign(TextAlign.Start)
                    .width('100%')
                    .margin({ top: 5 })

                  Text(record.date)
                    .fontSize(12)
                    .fontColor('#999')
                    .textAlign(TextAlign.Start)
                    .width('100%')
                    .margin({ top: 5 })
                }
                .layoutWeight(1)
                .alignItems(HorizontalAlign.Start)
                .padding({ left: 15 })
              }
              .width('100%')
              .padding(15)
              .backgroundColor(Color.White)
              .borderRadius(10)
              .margin({ left: 15, right: 15, bottom: 10 })
            }
          }, (record: HealthRecord) => record.id?.toString() || '')
        }
        .width('100%')
        .layoutWeight(1)
        .padding({ bottom: 10 })
      }
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#f5f5f5')
  }

  getTypeIcon(type: string): string {
    const icons: Record<string, string> = {
      'weight': '⚖️',
      'blood_pressure': '🩺',
      'blood_sugar': '🩸',
      'exercise': '🏃',
      'sleep': '😴'
    };
    return icons[type] || '💚';
  }

  getTypeName(type: string): string {
    const names: Record<string, string> = {
      'weight': '体重',
      'blood_pressure': '血压',
      'blood_sugar': '血糖',
      'exercise': '运动',
      'sleep': '睡眠'
    };
    return names[type] || '健康记录';
  }
}
