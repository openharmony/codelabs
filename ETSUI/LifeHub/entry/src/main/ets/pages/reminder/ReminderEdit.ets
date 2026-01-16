/**
 * 编辑提醒页面
 */
import { LifeHubData, Reminder } from '../../model/LifeHubData';
import { router } from '@kit.ArkUI';

interface TypeOption {
  value: 'health' | 'goal' | 'mood' | 'learning' | 'diary' | 'custom';
  label: string;
  icon: string;
}

@Entry
@Component
export struct ReminderEdit {
  private lifeHubData: LifeHubData = new LifeHubData();
  private reminderId: number = 0;
  
  @State title: string = '';
  @State description: string = '';
  @State type: 'health' | 'goal' | 'mood' | 'learning' | 'diary' | 'custom' = 'health';
  @State time: string = '09:00';
  @State selectedDays: boolean[] = [false, true, true, true, true, true, false]; // 周一到周日
  @State isReminderEnabled: boolean = true;
  @State isSaving: boolean = false;

  private types: TypeOption[] = [
    { value: 'health', label: '健康', icon: '💚' },
    { value: 'goal', label: '目标', icon: '🎯' },
    { value: 'mood', label: '心情', icon: '😊' },
    { value: 'learning', label: '学习', icon: '📚' },
    { value: 'diary', label: '日记', icon: '📝' },
    { value: 'custom', label: '自定义', icon: '🔔' }
  ];

  private dayNames = ['周一', '周二', '周三', '周四', '周五', '周六', '周日'];

  aboutToAppear() {
    const context = getContext(this);
    if (context) {
      this.lifeHubData.setContext(context);
    }
    const params = router.getParams() as Record<string, number>;
    this.reminderId = params?.reminderId || 0;
    if (this.reminderId) {
      this.loadReminder();
    }
  }

  async loadReminder() {
    // TODO: 从数据库加载提醒详情
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

        Text('提醒设置')
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .textAlign(TextAlign.Center)
          .layoutWeight(1)

        Button('保存')
          .fontSize(16)
          .backgroundColor(Color.Transparent)
          .fontColor(this.title ? '#007DFF' : '#999')
          .enabled(!!this.title && !this.isSaving)
          .onClick(() => {
            this.saveReminder();
          })
      }
      .width('100%')
      .padding(20)
      .backgroundColor('#f0f0f0')

      Scroll() {
        Column() {
          // 标题
          Text('提醒标题')
            .fontSize(16)
            .fontWeight(FontWeight.Medium)
            .width('100%')
            .textAlign(TextAlign.Start)
            .margin({ top: 20, bottom: 10, left: 15 })

          TextInput({ placeholder: '输入提醒标题', text: this.title })
            .width('100%')
            .height(50)
            .fontSize(16)
            .padding({ left: 15, right: 15 })
            .backgroundColor('#f8f8f8')
            .borderRadius(8)
            .onChange((value: string) => {
              this.title = value;
            })
            .margin({ left: 15, right: 15 })

          // 描述
          Text('提醒描述')
            .fontSize(16)
            .fontWeight(FontWeight.Medium)
            .width('100%')
            .textAlign(TextAlign.Start)
            .margin({ top: 20, bottom: 10, left: 15 })

          TextArea({ placeholder: '输入提醒描述（可选）', text: this.description })
            .width('100%')
            .height(100)
            .fontSize(16)
            .padding(15)
            .backgroundColor('#f8f8f8')
            .borderRadius(8)
            .onChange((value: string) => {
              this.description = value;
            })
            .margin({ left: 15, right: 15 })

          // 类型选择
          Text('提醒类型')
            .fontSize(16)
            .fontWeight(FontWeight.Medium)
            .width('100%')
            .textAlign(TextAlign.Start)
            .margin({ top: 20, bottom: 10, left: 15 })

          Grid() {
            ForEach(this.types, (t: TypeOption) => {
              GridItem() {
                Column() {
                  Text(t.icon)
                    .fontSize(32)
                    .margin({ bottom: 8 })
                  Text(t.label)
                    .fontSize(14)
                    .fontColor(this.type === t.value ? '#007DFF' : '#666')
                }
                .width('100%')
                .height(100)
                .justifyContent(FlexAlign.Center)
                .backgroundColor(this.type === t.value ? '#E3F2FD' : '#f8f8f8')
                .borderRadius(8)
                .border({
                  width: this.type === t.value ? 2 : 0,
                  color: '#007DFF'
                })
                .onClick(() => {
                  this.type = t.value;
                })
              }
            })
          }
          .columnsTemplate('1fr 1fr 1fr')
          .columnsGap(10)
          .rowsGap(10)
          .width('100%')
          .height(220)
          .margin({ left: 15, right: 15 })

          // 时间选择
          Text('提醒时间')
            .fontSize(16)
            .fontWeight(FontWeight.Medium)
            .width('100%')
            .textAlign(TextAlign.Start)
            .margin({ top: 20, bottom: 10, left: 15 })

          Row() {
            Text(this.time)
              .fontSize(24)
              .fontWeight(FontWeight.Bold)
              .fontColor('#007DFF')
          }
          .width('100%')
          .height(60)
          .justifyContent(FlexAlign.Center)
          .backgroundColor('#f8f8f8')
          .borderRadius(8)
          .margin({ left: 15, right: 15 })
          .onClick(() => {
            // TODO: 打开时间选择器
          })

          // 重复设置
          Text('重复设置')
            .fontSize(16)
            .fontWeight(FontWeight.Medium)
            .width('100%')
            .textAlign(TextAlign.Start)
            .margin({ top: 20, bottom: 10, left: 15 })

          Row() {
            ForEach(this.selectedDays, (selected: boolean, index: number) => {
              Column() {
                Text(this.dayNames[index])
                  .fontSize(14)
                  .fontColor(selected ? '#007DFF' : '#666')
                  .fontWeight(selected ? FontWeight.Bold : FontWeight.Normal)
              }
              .width(40)
              .height(50)
              .justifyContent(FlexAlign.Center)
              .backgroundColor(selected ? '#E3F2FD' : '#f8f8f8')
              .borderRadius(8)
              .border({
                width: selected ? 2 : 0,
                color: '#007DFF'
              })
              .onClick(() => {
                this.selectedDays[index] = !this.selectedDays[index];
              })
            })
          }
          .width('100%')
          .justifyContent(FlexAlign.SpaceAround)
          .margin({ left: 15, right: 15, bottom: 20 })
        }
        .width('100%')
      }
      .layoutWeight(1)
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#f5f5f5')
  }

  async saveReminder() {
    if (!this.title) return;

    this.isSaving = true;
    try {
      const days = this.selectedDays.map((selected: boolean, index: number) => 
        selected ? index.toString() : ''
      ).filter(d => d).join(',');

      const reminder: Reminder = {
        id: this.reminderId || undefined,
        title: this.title,
        description: this.description,
        type: this.type,
        time: this.time,
        days: days,
        enabled: this.isReminderEnabled,
        createTime: this.reminderId ? Date.now() : Date.now(),
        updateTime: Date.now()
      };

      if (this.reminderId) {
        await this.lifeHubData.updateReminder(reminder);
      } else {
        await this.lifeHubData.addReminder(reminder);
      }
      router.back();
    } catch (error) {
      console.error('ReminderEdit: 保存失败:', error);
    } finally {
      this.isSaving = false;
    }
  }
}
