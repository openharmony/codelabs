# MemoTime - 日程管理应用（ArkTS）

## 介绍

本项目是一个基于 OpenHarmony 开发的日程管理应用，使用 ArkTS 语言进行开发。主要功能包括：

- **多视图日历展示**：支持日视图、周视图、月视图、年视图四种切换方式
- **日程管理**：创建、编辑、删除日程，支持设置时间、地点、颜色标签等
- **智能提醒**：支持多种提醒时机（准时、提前5分钟至提前1周），通过系统通知提醒
- **语音录制**：为日程添加语音备注，支持录音、播放、管理功能
- **标签分类**：使用彩色标签对日程进行分类管理
- **外部应用链接**：支持关联外部应用（如腾讯会议）

![MemoTime 应用界面](entry/src/main/resources/base/media/app_icon.png)

## 相关概念

- **Navigation**：Navigation 组件作为 Page 页面的根容器，通过属性设置展示页面的标题、工具栏、菜单
- **Grid**：网格容器组件，用于日历的月视图和年视图布局展示
- **List**：列表组件，用于展示日程列表，适合连续、多行呈现同类数据
- **Swiper**：滑块视图容器，支持左右滑动切换日期和视图
- **@State 状态管理**：管理组件内部状态，实现响应式 UI 更新
- **relationalStore**：关系型数据库，用于本地持久化存储日程和提醒数据
- **notificationManager**：通知管理服务，实现日程提醒通知功能
- **AVRecorder/AVPlayer**：音频录制与播放 API，实现语音备注功能

## 环境搭建

### 软件要求

- **DevEco Studio 版本**：DevEco Studio 5.0 Release 及以上
- **OpenHarmony SDK 版本**：API version 12 及以上

### 硬件要求

- **开发板类型**：润和 RK3568 开发板（或兼容的 OpenHarmony 设备）
- **OpenHarmony 系统**：5.0 Release 及以上

### 环境搭建步骤

完成本项目开发需要先完成开发环境的搭建：

1. **搭建开发环境**
   - 完成 DevEco Studio 的安装和开发环境配置

2. **获取项目代码**
   ```bash
   git clone <项目仓库地址>
   cd memo_time
   ```

3. **安装依赖**
   - 使用 DevEco Studio 打开项目
   - 等待 IDE 自动同步依赖

4. **运行调测**
   - 连接 OpenHarmony 设备或启动模拟器
   - 点击 Run 按钮运行应用

## 代码结构解读

```
├── entry/src/main/ets                 // 代码区
│   ├── common
│   │   ├── constants
│   │   │   └── Constants.ets          // 常量定义（视图类型、提醒时机等）
│   │   └── utils
│   │       ├── DateUtils.ets          // 日期工具函数
│   │       └── Logger.ets             // 日志工具类
│   ├── components
│   │   ├── calendar
│   │   │   ├── DayView.ets            // 日视图组件
│   │   │   ├── WeekView.ets           // 周视图组件
│   │   │   ├── MonthView.ets          // 月视图组件
│   │   │   └── YearView.ets           // 年视图组件
│   │   ├── common
│   │   │   └── FloatingButton.ets     // 悬浮按钮组件
│   │   ├── recording
│   │   │   ├── RecordingList.ets      // 录音列表组件
│   │   │   └── RecordingPanel.ets     // 录音面板组件
│   │   └── schedule
│   │       ├── ScheduleCard.ets       // 日程卡片组件
│   │       └── ScheduleList.ets       // 日程列表组件
│   ├── entryability
│   │   └── EntryAbility.ets           // 程序入口类
│   ├── model
│   │   ├── Recording.ets              // 录音数据模型
│   │   ├── Reminder.ets               // 提醒数据模型
│   │   ├── Schedule.ets               // 日程数据模型
│   │   └── Tag.ets                    // 标签数据模型
│   ├── pages
│   │   ├── Index.ets                  // 主页面（日历视图）
│   │   ├── ScheduleDetailPage.ets     // 日程详情页面
│   │   ├── ScheduleEditPage.ets       // 日程编辑页面
│   │   └── SettingsPage.ets           // 设置页面
│   └── service
│       ├── AudioService.ets           // 音频录制与播放服务
│       ├── DataService.ets            // 数据持久化服务
│       └── NotificationService.ets    // 通知提醒服务
├── entry/src/main/resources           // 资源文件
└── entry/src/test                     // 单元测试
```

## 构建应用页面

### 主页面（日历视图）

主页面使用 Stack 和 Column 组件实现整体布局，从上到下依次是顶部导航栏、视图切换标签页、日历内容区。支持日/周/月/年四种视图切换，底部有悬浮添加按钮。

```text
// Index.ets
@Entry
@Component
struct Index {
  @State currentViewType: CalendarViewType = CalendarViewType.MONTH
  @State selectedDate: Date = new Date()
  @State schedules: Schedule[] = []

  build() {
    Stack({ alignContent: Alignment.BottomEnd }) {
      Column() {
        this.TopBar()        // 顶部导航栏
        this.ViewTypeTabs()  // 视图切换标签
        Column() {
          this.CalendarContent()  // 日历内容区
        }
        .layoutWeight(1)
      }

      // 悬浮添加按钮
      FloatingButton({
        iconRes: $r('sys.media.ohos_ic_public_add'),
        onBtnClick: () => this.navigateToAddSchedule()
      })
    }
  }
}
```

### 月视图组件

月视图使用 Grid 组件展示当月日期，每个日期单元格显示日期和当天的日程指示器。

```text
// MonthView.ets
@Component
export struct MonthView {
  @Prop year: number
  @Prop month: number
  @Link selectedDate: Date
  @Prop schedules: Schedule[]

  build() {
    Column() {
      // 星期标题行
      Row() {
        ForEach(['日', '一', '二', '三', '四', '五', '六'], (day: string) => {
          Text(day).fontSize(12).width('14.28%')
        })
      }

      // 日期网格
      Grid() {
        ForEach(this.generateDays(), (day: DayInfo) => {
          GridItem() {
            this.DayCell(day)
          }
        })
      }
      .columnsTemplate('1fr 1fr 1fr 1fr 1fr 1fr 1fr')
    }
  }
}
```

### 日程编辑页面

日程编辑页面支持设置日程标题、时间、地点、颜色、提醒等信息，使用表单布局展示各项设置。

```text
// ScheduleEditPage.ets
@Entry
@Component
struct ScheduleEditPage {
  @State title: string = ''
  @State startTime: number = 0
  @State endTime: number = 0
  @State location: string = ''
  @State color: string = '#4ECDC4'
  @State reminderTiming: ReminderTiming = ReminderTiming.MINUTES_15

  build() {
    Navigation() {
      Scroll() {
        Column() {
          // 标题输入
          this.TitleInput()
          // 时间选择
          this.TimeSelector()
          // 地点输入
          this.LocationInput()
          // 颜色选择
          this.ColorPicker()
          // 提醒设置
          this.ReminderSetting()
          // 录音附件
          this.RecordingSection()
        }
      }
    }
  }
}
```

### 提醒通知服务

通知服务负责检查待触发的提醒，并通过系统通知发送日程提醒。使用轮询机制每分钟检查一次。

```text
// NotificationService.ets
export class NotificationService {
  // 发送日程提醒通知
  async sendScheduleReminder(schedule: Schedule, reminder: Reminder): Promise<void> {
    const notificationRequest: notificationManager.NotificationRequest = {
      id: this.generateNotificationId(reminder.id),
      content: {
        notificationContentType: notificationManager.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
        normal: {
          title: '日程提醒',
          text: schedule.title,
          additionalText: reminder.getTimingText()
        }
      }
    }
    await notificationManager.publish(notificationRequest)
  }
}
```

### 语音录制功能

音频服务封装了 OpenHarmony 的 AVRecorder 和 AVPlayer API，支持录音的录制、暂停、恢复、停止，以及播放、暂停、进度控制等功能。

```text
// AudioService.ets
export class AudioService {
  // 开始录音
  async startRecording(callback?: RecordingCallback): Promise<boolean> {
    this.avRecorder = await media.createAVRecorder()
    const avConfig: media.AVRecorderConfig = {
      audioSourceType: media.AudioSourceType.AUDIO_SOURCE_TYPE_MIC,
      profile: {
        audioBitrate: 128000,
        audioChannels: 1,
        audioCodec: media.CodecMimeType.AUDIO_AAC,
        audioSampleRate: 44100,
        fileFormat: media.ContainerFormatType.CFT_MPEG_4A
      },
      url: `fd://${file.fd}`
    }
    await this.avRecorder.prepare(avConfig)
    await this.avRecorder.start()
    return true
  }
}
```

## 权限配置

本应用需要以下权限：

| 权限名称 | 权限说明 |
|---------|---------|
| ohos.permission.NOTIFICATION | 发送通知权限，用于日程提醒 |
| ohos.permission.MICROPHONE | 麦克风权限，用于语音录制 |

在 `module.json5` 中配置：

```json
{
   "requestPermissions": [
      {
         "name": "ohos.permission.NOTIFICATION"
      },
      {
         "name": "ohos.permission.MICROPHONE",
         "reason": "$string:microphone_reason",
         "usedScene": {
            "abilities": ["EntryAbility"],
            "when": "inuse"
         }
      }
   ]
}
```

## 总结

通过本项目，您可以了解到以下知识点：

1. 如何使用 Grid 和 List 组件实现日历多视图布局
2. 如何使用 relationalStore 实现本地数据持久化
3. 如何使用 notificationManager 实现日程提醒通知
4. 如何使用 AVRecorder/AVPlayer 实现语音录制与播放
5. 如何使用 @State 状态管理实现响应式 UI
6. 如何使用 router 实现页面导航和参数传递

## 许可证

本项目采用 Apache License 2.0 许可证。
