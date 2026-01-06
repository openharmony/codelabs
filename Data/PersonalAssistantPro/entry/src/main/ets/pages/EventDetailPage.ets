// entry/src/main/ets/pages/EventDetailPage.ets
import { Event, EventModel } from '../model/EventModel';
import { DateUtils } from '../common/utils/DateUtils';
import { StyleConstants } from '../common/constants/StyleConstants';
import { promptAction } from '@kit.ArkUI';

/**
 * 日程详情页面
 * 业务逻辑：展示详情 -> 可跳转编辑 -> 编辑返回后刷新数据
 */
@Component
export struct EventDetailPage {
  // ✅ 1. 接收从 MainPage 路由传递的参数
  @State eventItem: Event | undefined = undefined;

  // ✅ 2. 接收路由栈，用于跳转和返回
  @Consume('pageStack') pageStack: NavPathStack;

  // ✅ 3. 监听全局刷新信号 (用于编辑后回来刷新页面)
  @StorageProp('refreshEventList') @Watch('onRefreshChange') refreshSignal: number = 0;

  async onRefreshChange() {
    // 当编辑页保存并触发 refreshEventList 时，这里会执行
    // 我们需要重新从数据库查一遍最新的 eventItem
    if (this.eventItem && this.eventItem.id) {
      // 这里模拟一个 queryById (实际可以用 queryAll 过滤，或在 Model 加 queryById)
      // 为了代码简洁，直接复用 queryAll + 过滤，性能允许
      const allEvents = await EventModel.queryUpcoming(); // 或者 queryAll
      const latest = allEvents.find(e => e.id === this.eventItem?.id);

      // 如果找到了最新数据（可能改了时间、标题），更新 UI
      if (latest) {
        this.eventItem = latest;
      } else {
        // 如果没找到，说明可能被删了，或者时间过期了
        // 这里可以不做处理，或者 pop
      }
    }
  }

  /**
   * 处理删除操作
   */
  private handleDelete() {
    promptAction.showDialog({
      title: '删除日程',
      message: '确定要删除这个日程吗？',
      buttons: [
        { text: '取消', color: StyleConstants.COLOR_TEXT_SECONDARY },
        { text: '删除', color: StyleConstants.COLOR_DANGER }
      ]
    }).then(async (resp) => {
      if (resp.index === 1 && this.eventItem) {
        await EventModel.deleteById(this.eventItem.id);
        promptAction.showToast({ message: '日程已删除' });

        // 触发全局刷新，通知列表页
        AppStorage.setOrCreate('refreshEventList', Date.now());

        // 返回列表页
        this.pageStack.pop();
      }
    });
  }

  /**
   * 处理编辑操作
   */
  private handleEdit() {
    if (this.eventItem) {
      // 跳转到编辑页，传入当前对象
      this.pageStack.pushPath({ name: 'EventEditPage', param: this.eventItem });
    }
  }

  @Builder
  DetailItem(label: string, value: string, icon?: Resource) {
    Row() {
      if (icon) {
        Image(icon).width(20).height(20).margin({ right: 12 }).fillColor(StyleConstants.COLOR_PRIMARY)
      } else {
        // 默认圆点
        Circle({ width: 8, height: 8 }).fill(StyleConstants.COLOR_PRIMARY).margin({ right: 12 })
      }

      Column() {
        Text(label)
          .fontSize(12)
          .fontColor(StyleConstants.COLOR_TEXT_SECONDARY)
          .margin({ bottom: 4 })

        Text(value)
          .fontSize(16)
          .fontColor(StyleConstants.COLOR_TEXT_PRIMARY)
          .fontWeight(FontWeight.Medium)
      }
      .alignItems(HorizontalAlign.Start)
    }
    .width('100%')
    .padding({ top: 16, bottom: 16 })
    .border({ width: { bottom: 1 }, color: '#F0F0F0' })
  }

  build() {
    NavDestination() {
      Column() {
        // --- 1. 自定义标题栏 ---
        Row() {
          Image($r('app.media.ic_back'))
            .width(24).height(24)
            .onClick(() => this.pageStack.pop())

          Text('日程详情')
            .fontSize(20).fontWeight(FontWeight.Bold)
            .margin({ left: 16 })
            .layoutWeight(1)

          // 右侧菜单
          Row() {
            Text('编辑')
              .fontSize(16).fontColor(StyleConstants.COLOR_PRIMARY)
              .margin({ right: 16 })
              .onClick(() => this.handleEdit())

            Text('删除')
              .fontSize(16).fontColor(StyleConstants.COLOR_DANGER)
              .onClick(() => this.handleDelete())
          }
        }
        .width('100%').height(56)
        .padding({ left: 16, right: 16 })
        .backgroundColor(Color.White)

        if (this.eventItem) {
          Scroll() {
            Column() {
              // --- 2. 顶部卡片 (标题 & 状态) ---
              Column() {
                Text(this.eventItem.title)
                  .fontSize(24)
                  .fontWeight(FontWeight.Bold)
                  .fontColor(StyleConstants.COLOR_TEXT_PRIMARY)
                  .margin({ bottom: 12 })

                Row() {
                  if (this.eventItem.isAllDay) {
                    Text('全天')
                      .fontSize(12).fontColor(Color.White)
                      .backgroundColor(StyleConstants.COLOR_PRIMARY)
                      .padding({ left: 8, right: 8, top: 4, bottom: 4 })
                      .borderRadius(4)
                      .margin({ right: 8 })
                  }

                  if (this.eventItem.reminderId > -1) {
                    Text('提醒开启')
                      .fontSize(12).fontColor(StyleConstants.COLOR_SUCCESS)
                      .backgroundColor('#E8F5E9')
                      .padding({ left: 8, right: 8, top: 4, bottom: 4 })
                      .borderRadius(4)
                  }
                }
              }
              .width('100%')
              .alignItems(HorizontalAlign.Start)
              .padding(24)
              .backgroundColor(Color.White)
              .margin({ bottom: 12 })

              // --- 3. 详细信息列表 ---
              Column() {
                // 开始时间
                this.DetailItem(
                  '开始时间',
                  DateUtils.formatFull(this.eventItem.startTime)
                )

                // 结束时间
                this.DetailItem(
                  '结束时间',
                  DateUtils.formatFull(this.eventItem.endTime)
                )

                // 备注
                this.DetailItem(
                  '备注说明',
                  this.eventItem.description || '暂无备注'
                )
              }
              .padding({ left: 24, right: 24 })
              .backgroundColor(Color.White)
              .width('100%')
            }
            .width('100%')
          }
          .layoutWeight(1)
          .backgroundColor('#F1F3F5')
        } else {
          // 异常空状态
          Column() {
            Text('未找到日程信息').fontSize(16).fontColor(Color.Gray)
          }
          .height('100%').justifyContent(FlexAlign.Center)
        }
      }
      .width('100%').height('100%').backgroundColor('#F1F3F5')
    }
    .hideTitleBar(true)
  }
}