/**
 * 心情模块主页面
 */
import { LifeHubData, MoodRecord } from '../../model/LifeHubData';
import { router } from '@kit.ArkUI';

@Component
export struct MoodMain {
  private lifeHubData: LifeHubData = new LifeHubData();
  
  @State moodRecords: MoodRecord[] = [];
  @State isLoading: boolean = true;

  aboutToAppear() {
    const context = getContext(this);
    if (context) {
      this.lifeHubData.setContext(context);
    }
    this.loadMoodRecords();
  }

  async loadMoodRecords() {
    this.isLoading = true;
    try {
      this.moodRecords = await this.lifeHubData.getMoodRecords(30);
    } catch (error) {
      console.error('MoodMain: 加载数据失败:', error);
    } finally {
      this.isLoading = false;
    }
  }

  build() {
    Column() {
      Row() {
        Text('心情记录')
          .fontSize(24)
          .fontWeight(FontWeight.Bold)
        
        Blank()
        
        Button('+')
          .fontSize(20)
          .width(40)
          .height(40)
          .borderRadius(20)
          .backgroundColor('#E91E63')
          .fontColor(Color.White)
          .onClick(() => {
            router.pushUrl({ url: 'pages/mood/MoodRecord' });
          })
      }
      .width('100%')
      .padding(20)
      .backgroundColor('#f0f0f0')

      if (this.isLoading) {
        LoadingProgress()
          .width(50)
          .height(50)
          .margin({ top: 100 })
      } else if (this.moodRecords.length === 0) {
        Column() {
          Text('😊')
            .fontSize(60)
            .margin({ bottom: 20 })
          Text('还没有心情记录')
            .fontSize(18)
            .fontColor('#666')
        }
        .width('100%')
        .layoutWeight(1)
        .justifyContent(FlexAlign.Center)
      } else {
        List() {
          ForEach(this.moodRecords, (record: MoodRecord) => {
            ListItem() {
              Row() {
                Text(record.mood)
                  .fontSize(40)
                  .width(60)

                Column() {
                  Text(record.date)
                    .fontSize(16)
                    .fontWeight(FontWeight.Medium)
                    .textAlign(TextAlign.Start)
                    .width('100%')

                  if (record.note) {
                    Text(record.note)
                      .fontSize(14)
                      .fontColor('#666')
                      .textAlign(TextAlign.Start)
                      .width('100%')
                      .margin({ top: 5 })
                      .maxLines(2)
                      .textOverflow({ overflow: TextOverflow.Ellipsis })
                  }
                }
                .layoutWeight(1)
                .alignItems(HorizontalAlign.Start)
                .padding({ left: 15 })

                Text(`评分: ${record.score}`)
                  .fontSize(14)
                  .fontColor('#E91E63')
              }
              .width('100%')
              .padding(15)
              .backgroundColor(Color.White)
              .borderRadius(10)
              .margin({ left: 15, right: 15, bottom: 10 })
            }
          }, (record: MoodRecord) => record.id?.toString() || '')
        }
        .width('100%')
        .layoutWeight(1)
      }
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#f5f5f5')
  }
}
