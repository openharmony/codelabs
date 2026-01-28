import { ReminderType, ReminderConfig, SportGoal } from '../model/ReminderModel';
import { ReminderService } from '../service/ReminderService';
import promptAction from '@ohos.promptAction';

// ReminderItem参数接口
interface ReminderItemParams {
  type: ReminderType;
  title: string;
  description: string;
  showInterval?: boolean;
}

// 对话框按钮接口
interface DialogButton {
  text: string;
  color: string;
}

// 提醒设置组件
@Component
export struct ReminderSettingComponent {
  @Prop reminderService: ReminderService;
  @State isExpanded: boolean = false;
  @State tempGoal: SportGoal = {};
  @State showGoalDialog: boolean = false;

  build() {
    Column() {
      // 标题栏
      Row() {
        Text('提醒设置')
          .fontSize(16)
          .fontWeight(700)
          .fontColor('#333333')
        Text('')
          .flexGrow(1)
        Text(this.isExpanded ? '收起' : '展开')
          .fontSize(12)
          .fontColor('#4ECDC4')
          .onClick(() => {
            this.isExpanded = !this.isExpanded;
          })
      }
      .width('100%')
      .padding(16)
      .backgroundColor('#FFFFFF')
      .borderRadius(12)
      .shadow({ radius: 4, color: '#1A000000', offsetX: 0, offsetY: 2 })

      // 展开内容
      if (this.isExpanded) {
        Column() {
          // 运动开始提醒
          this.ReminderItem({
            type: ReminderType.SPORT_START,
            title: '运动开始提醒',
            description: '运动开始时提醒您做好准备'
          })

          // 运动时长提醒
          this.ReminderItem({
            type: ReminderType.SPORT_DURATION,
            title: '运动时长提醒',
            description: '定期提醒您运动时长',
            showInterval: true
          })

          // 目标达成提醒
          this.ReminderItem({
            type: ReminderType.GOAL_ACHIEVEMENT,
            title: '目标达成提醒',
            description: '达成运动目标时提醒您'
          })

          // 休息提醒
          this.ReminderItem({
            type: ReminderType.REST_REMINDER,
            title: '休息提醒',
            description: '长时间运动时提醒您休息'
          })

          // 定期运动提醒
          this.ReminderItem({
            type: ReminderType.REGULAR_SPORT,
            title: '定期运动提醒',
            description: '提醒您保持规律运动'
          })

          // 运动目标设置
          this.GoalSettingItem()

          // 保存按钮
          Button('保存设置')
            .fontSize(14)
            .fontColor('#FFFFFF')
            .backgroundColor('#4ECDC4')
            .borderRadius(8)
            .padding({ left: 32, right: 32, top: 10, bottom: 10 })
            .margin({ top: 20, bottom: 10 })
            .onClick(() => {
              this.saveSettings();
            })
        }
        .width('100%')
        .padding(16)
        .backgroundColor('#FFFFFF')
        .borderRadius(12)
        .margin({ top: 8 })
        .shadow({ radius: 4, color: '#1A000000', offsetX: 0, offsetY: 2 })
      }

      // 目标设置对话框
      this.GoalDialog()
    }
    .width('100%')
  }

  // 提醒项构建器
  @Builder
  ReminderItem(params: ReminderItemParams) {
    Column() {
      // 直接渲染提醒项内容
      this.RenderReminderContent(params)
    }
  }

  // 渲染提醒内容的辅助方法
  @Builder
  RenderReminderContent(params: ReminderItemParams) {
    // 直接在条件中调用方法获取配置
    if (this.reminderService.getSettings().getReminderConfig(params.type)) {
      Column() {
        Row() {
          Column() {
            Text(params.title)
              .fontSize(14)
              .fontWeight(500)
              .fontColor('#333333')
              .margin({ bottom: 4 })
            Text(params.description)
              .fontSize(12)
              .fontColor('#999999')
          }
          .flexGrow(1)

          Toggle({
            type: ToggleType.Switch,
            isOn: this.reminderService.getSettings().getReminderConfig(params.type)?.enabled || false
          })
            .onChange((isEnabled: boolean) => {
              const config = this.reminderService.getSettings().getReminderConfig(params.type);
              if (config) {
                const updatedConfig: ReminderConfig = {
                  type: config.type,
                  enabled: isEnabled,
                  interval: config.interval,
                  message: config.message
                };
                this.updateReminderConfig(params.type, updatedConfig);
              }
            })
        }
        .width('100%')
        .margin({ bottom: 8 })

        // 间隔设置
        if (params.showInterval && (this.reminderService.getSettings().getReminderConfig(params.type)?.enabled || false)) {
          Row() {
            Text('提醒间隔:')
              .fontSize(12)
              .fontColor('#666666')
              .margin({ right: 8 })
            Text(`${Math.floor((this.reminderService.getSettings().getReminderConfig(params.type)?.interval || 1800) / 60)} 分钟`)
              .fontSize(12)
              .fontColor('#333333')
              .margin({ right: 12 })
            Button('修改')
              .fontSize(12)
              .fontColor('#4ECDC4')
              .backgroundColor('#FFFFFF')
              .borderRadius(4)
              .padding({ left: 8, right: 8, top: 2, bottom: 2 })
              .border({ width: 1, color: '#4ECDC4' })
              .onClick(() => {
                const config = this.reminderService.getSettings().getReminderConfig(params.type);
                if (config) {
                  this.showIntervalDialog(params.type, config);
                }
              })
          }
          .width('100%')
          .padding({ left: 0 })
        }
      }
      .width('100%')
      .padding({ bottom: 16 })
      .borderWidth({ bottom: 1 })
      .borderColor('#F0F0F0')
    }
  }

  // 运动目标设置项
  @Builder
  GoalSettingItem() {
    Column() {
      Row() {
        Column() {
          Text('运动目标设置')
            .fontSize(14)
            .fontWeight(500)
            .fontColor('#333333')
            .margin({ bottom: 4 })
          Text('设置您的运动目标，达成时会收到提醒')
            .fontSize(12)
            .fontColor('#999999')
        }
        .flexGrow(1)

        Button('设置目标')
          .fontSize(12)
          .fontColor('#4ECDC4')
          .backgroundColor('#FFFFFF')
          .borderRadius(4)
          .padding({ left: 12, right: 12, top: 4, bottom: 4 })
          .border({ width: 1, color: '#4ECDC4' })
          .onClick(() => {
            const currentGoal = this.reminderService.getSettings().getGoal();
            this.tempGoal = {
              distance: currentGoal.distance,
              duration: currentGoal.duration,
              calories: currentGoal.calories
            };
            this.showGoalDialog = true;
          })
      }
      .width('100%')

      // 当前目标显示
      if (this.reminderService.getSettings().getGoal().distance || this.reminderService.getSettings().getGoal().duration || this.reminderService.getSettings().getGoal().calories) {
        Row() {
          Text('当前目标:')
            .fontSize(12)
            .fontColor('#666666')
            .margin({ right: 8 })
          Text(this.getGoalText(this.reminderService.getSettings().getGoal()))
            .fontSize(12)
            .fontColor('#333333')
        }
        .width('100%')
        .margin({ top: 8 })
      }
    }
    .width('100%')
    .padding({ bottom: 16 })
  }

  // 目标文本
  private getGoalText(goal: SportGoal): string {
    const parts: string[] = [];
    if (goal.distance) {
      parts.push(`${goal.distance} 米`);
    }
    if (goal.duration) {
      parts.push(`${goal.duration} 分钟`);
    }
    if (goal.calories) {
      parts.push(`${goal.calories} 卡路里`);
    }
    return parts.join('、') || '未设置';
  }

  // 显示间隔设置对话框
  private showIntervalDialog(type: ReminderType, config: ReminderConfig): void {
    // 预设间隔选项
    const intervals = [5, 10, 15, 20, 30, 45, 60];
    const currentMinutes = Math.floor((config.interval || 1800) / 60);
    
    // 构建按钮数组
    const buttons: DialogButton[] = [];
    
    // 添加间隔选项按钮
    for (const minutes of intervals) {
      const button: DialogButton = {
        text: `${minutes}分钟`,
        color: minutes === currentMinutes ? '#007AFF' : '#333333'
      };
      buttons.push(button);
    }
    
    // 添加取消按钮
    const cancelButton: DialogButton = {
      text: '取消',
      color: '#999999'
    };
    buttons.unshift(cancelButton);
    
    promptAction.showDialog({
      title: '设置提醒间隔',
      message: '请选择提醒间隔',
      buttons: buttons
    }).then((result) => {
      if (result.index > 0) { // 不是取消按钮
        const selectedMinutes = intervals[result.index - 1];
        const updatedConfig: ReminderConfig = {
          type: config.type,
          enabled: config.enabled,
          interval: selectedMinutes * 60,
          message: config.message
        };
        this.updateReminderConfig(type, updatedConfig);
        promptAction.showToast({ message: '间隔设置成功' });
      }
    });
  }

  // 更新提醒配置
  private updateReminderConfig(type: ReminderType, config: ReminderConfig): void {
    this.reminderService.updateReminderConfig(config);
    console.log('提醒配置已更新:', type);
  }

  // 保存设置
  private saveSettings(): void {
    promptAction.showToast({ message: '设置已保存' });
  }

  // 目标设置对话框
  @Builder
  GoalDialog() {
    if (this.showGoalDialog) {
      Column() {
        Text('设置运动目标')
          .fontSize(16)
          .fontWeight(FontWeight.Bold)
          .fontColor('#333333')
          .margin({ bottom: 16 })

        // 距离目标
        Row() {
          Text('距离目标:')
            .fontSize(14)
            .fontColor('#666666')
            .width(80)
          TextInput({
            text: this.tempGoal.distance?.toString() || '',
            placeholder: '请输入距离'
          })
          .width(120)
          .height(40)
          .fontSize(14)
          .padding({ left: 8, right: 8 })
          .backgroundColor('#F5F5F5')
          .borderRadius(4)
          .onChange((value: string) => {
            this.tempGoal.distance = value ? parseFloat(value) : undefined;
          })
          Text('米')
            .fontSize(14)
            .fontColor('#666666')
            .margin({ left: 8 })
        }
        .margin({ bottom: 12 })

        // 时长目标
        Row() {
          Text('时长目标:')
            .fontSize(14)
            .fontColor('#666666')
            .width(80)
          TextInput({
            text: this.tempGoal.duration?.toString() || '',
            placeholder: '请输入时长'
          })
          .width(120)
          .height(40)
          .fontSize(14)
          .padding({ left: 8, right: 8 })
          .backgroundColor('#F5F5F5')
          .borderRadius(4)
          .onChange((value: string) => {
            this.tempGoal.duration = value ? parseFloat(value) : undefined;
          })
          Text('分钟')
            .fontSize(14)
            .fontColor('#666666')
            .margin({ left: 8 })
        }
        .margin({ bottom: 12 })

        // 卡路里目标
        Row() {
          Text('卡路里目标:')
            .fontSize(14)
            .fontColor('#666666')
            .width(80)
          TextInput({
            text: this.tempGoal.calories?.toString() || '',
            placeholder: '请输入卡路里'
          })
          .width(120)
          .height(40)
          .fontSize(14)
          .padding({ left: 8, right: 8 })
          .backgroundColor('#F5F5F5')
          .borderRadius(4)
          .onChange((value: string) => {
            this.tempGoal.calories = value ? parseFloat(value) : undefined;
          })
          Text('卡路里')
            .fontSize(14)
            .fontColor('#666666')
            .margin({ left: 8 })
        }
        .margin({ bottom: 20 })

        // 按钮
        Row() {
          Button('取消')
            .fontSize(14)
            .fontColor('#999999')
            .backgroundColor('#FFFFFF')
            .borderRadius(8)
            .padding({ left: 24, right: 24, top: 8, bottom: 8 })
            .border({ width: 1, color: '#E0E0E0' })
            .onClick(() => {
              this.showGoalDialog = false;
            })
          Button('保存')
            .fontSize(14)
            .fontColor('#FFFFFF')
            .backgroundColor('#4ECDC4')
            .borderRadius(8)
            .padding({ left: 24, right: 24, top: 8, bottom: 8 })
            .margin({ left: 12 })
            .onClick(() => {
              this.saveGoal();
              this.showGoalDialog = false;
            })
        }
        .justifyContent(FlexAlign.End)
      }
      .width(300)
      .padding(20)
      .backgroundColor('#FFFFFF')
      .borderRadius(12)
      .position({ x: '50%', y: '50%' })
      .translate({ x: -150, y: -150 })
    }
  }

  // 保存目标
  private saveGoal(): void {
    console.log('开始保存目标...');
    console.log('tempGoal - 距离:', this.tempGoal.distance, '米');
    console.log('tempGoal - 时长:', this.tempGoal.duration, '分钟');
    console.log('tempGoal - 卡路里:', this.tempGoal.calories);
    
    const goal: SportGoal = {};
    if (this.tempGoal.distance !== undefined) {
      goal.distance = parseFloat(this.tempGoal.distance.toString());
      console.log('保存距离目标:', goal.distance, '米');
    }
    if (this.tempGoal.duration !== undefined) {
      goal.duration = parseFloat(this.tempGoal.duration.toString());
      console.log('保存时长目标:', goal.duration, '分钟');
    }
    if (this.tempGoal.calories !== undefined) {
      goal.calories = parseFloat(this.tempGoal.calories.toString());
      console.log('保存卡路里目标:', goal.calories);
    }
    
    console.log('准备保存的目标 - 距离:', goal.distance, '米');
    console.log('准备保存的目标 - 时长:', goal.duration, '分钟');
    console.log('准备保存的目标 - 卡路里:', goal.calories);
    this.reminderService.setSportGoal(goal);
    
    // 保存后立即获取目标，验证是否保存成功
    const savedGoal = this.reminderService.getSettings().getGoal();
    console.log('保存后的目标 - 距离:', savedGoal.distance, '米');
    console.log('保存后的目标 - 时长:', savedGoal.duration, '分钟');
    console.log('保存后的目标 - 卡路里:', savedGoal.calories);
    
    promptAction.showToast({ message: '目标已保存' });
  }
}
