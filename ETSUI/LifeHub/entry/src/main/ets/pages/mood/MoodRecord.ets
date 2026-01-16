/**
 * 记录心情页面
 */
import { LifeHubData, MoodRecord as MoodRecordModel } from '../../model/LifeHubData';
import { router } from '@kit.ArkUI';

// 心情选项接口
interface MoodOption {
  emoji: string;
  score: number;
  label: string;
}

@Entry
@Component
export struct MoodRecord {
  private lifeHubData: LifeHubData = new LifeHubData();
  
  @State selectedMood: string = '😊';
  @State score: number = 5;
  @State note: string = '';
  @State isSaving: boolean = false;

  private moods: MoodOption[] = [
    { emoji: '😄', score: 9, label: '非常开心' },
    { emoji: '😊', score: 7, label: '开心' },
    { emoji: '😐', score: 5, label: '一般' },
    { emoji: '😔', score: 3, label: '低落' },
    { emoji: '😢', score: 1, label: '难过' }
  ];

  aboutToAppear() {
    const context = getContext(this);
    if (context) {
      this.lifeHubData.setContext(context);
    }
  }

  build() {
    Column() {
      Row() {
        Button('取消')
          .fontSize(16)
          .backgroundColor(Color.Transparent)
          .fontColor('#007DFF')
          .onClick(() => {
            router.back();
          })

        Text('记录心情')
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .textAlign(TextAlign.Center)
          .layoutWeight(1)

        Button('保存')
          .fontSize(16)
          .backgroundColor(Color.Transparent)
          .fontColor('#007DFF')
          .enabled(!this.isSaving)
          .onClick(() => {
            this.saveMood();
          })
      }
      .width('100%')
      .padding(20)
      .backgroundColor('#f0f0f0')

      Scroll() {
        Column() {
          Text('选择心情')
            .fontSize(18)
            .fontWeight(FontWeight.Medium)
            .width('100%')
            .textAlign(TextAlign.Center)
            .margin({ top: 30, bottom: 20 })

          Flex({ wrap: FlexWrap.Wrap, justifyContent: FlexAlign.Center }) {
            ForEach(this.moods, (mood: MoodOption) => {
              Column() {
                Text(mood.emoji)
                  .fontSize(50)
                Text(mood.label)
                  .fontSize(14)
                  .fontColor('#333')
                  .margin({ top: 10 })
              }
              .width(100)
              .height(120)
              .justifyContent(FlexAlign.Center)
              .backgroundColor(this.selectedMood === mood.emoji ? '#E91E6320' : '#f0f0f0')
              .borderRadius(12)
              .onClick(() => {
                this.selectedMood = mood.emoji;
                this.score = mood.score;
              })
              .margin({ right: 10, bottom: 10 })
            })
          }
          .width('100%')
          .padding({ left: 20, right: 20 })

          Column() {
            Text('备注（可选）')
              .fontSize(16)
              .fontWeight(FontWeight.Medium)
              .textAlign(TextAlign.Start)
              .width('100%')
              .margin({ bottom: 15, top: 20 })

            TextArea({ placeholder: '记录此刻的想法...', text: this.note })
              .width('100%')
              .height(120)
              .onChange((value: string) => {
                this.note = value;
              })
          }
          .width('100%')
          .padding(20)
          .backgroundColor(Color.White)
          .borderRadius(12)
          .margin({ top: 20, left: 15, right: 15, bottom: 20 })
        }
      }
      .layoutWeight(1)
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#f5f5f5')
  }

  async saveMood() {
    this.isSaving = true;
    try {
      const record: MoodRecordModel = {
        mood: this.selectedMood,
        score: this.score,
        note: this.note,
        date: this.getTodayString(),
        time: Date.now()
      };

      await this.lifeHubData.addMoodRecord(record);
      router.back();
    } catch (error) {
      console.error('MoodRecord: 保存失败:', error);
    } finally {
      this.isSaving = false;
    }
  }

  getTodayString(): string {
    const date = new Date();
    const year = date.getFullYear();
    const month = String(date.getMonth() + 1).padStart(2, '0');
    const day = String(date.getDate()).padStart(2, '0');
    return `${year}-${month}-${day}`;
  }
}
