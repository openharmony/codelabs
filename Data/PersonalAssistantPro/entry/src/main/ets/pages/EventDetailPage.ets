/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
// entry/src/main/ets/pages/EventDetailPage.ets
import { Event, EventModel } from '../model/EventModel';
import { DateUtils } from '../common/utils/DateUtils';
import { StyleConstants } from '../common/constants/StyleConstants';
import { promptAction } from '@kit.ArkUI';
import { Logger } from '../common/utils/Logger';

const logger = new Logger('EventDetailPage');

@Component
export struct EventDetailPage {
  // 1. 接收从 MainPage 路由传递的参数
  @State eventItem: Event | undefined = undefined;
  @State forceUpdateTrigger: number = 0;
  // 2. 接收路由栈
  @Consume('pageStack') pageStack: NavPathStack;

  // 3. 监听列表刷新信号（只用于删除时通知列表页，不用来刷新自己了）
  @StorageProp('refreshEventList') refreshSignal: number = 0;

  /**
   * 核心刷新逻辑：根据 ID 查库
   */
  async reloadFromDb() {
    // 1. 先查库，拿到最新的数据
    if (this.eventItem && this.eventItem.id) {
      const freshData = await EventModel.queryById(this.eventItem.id);

      if (freshData) {
        logger.error(`[DEBUG_FORCE] 查到新数据: ${freshData.startTime}, 准备执行【毁灭重生】刷新`);

        // 🔥🔥🔥【第一步】销毁 UI 🔥🔥🔥
        // 设为 undefined，build() 里的 if (this.eventItem) 会变成 false
        // 整个详情页的内容会被从屏幕上移除
        this.eventItem = undefined;

        // 🔥🔥🔥【第二步】重建 UI 🔥🔥🔥
        // 延迟 10ms (极短时间，人眼几乎看不出闪烁，但足够让引擎重绘)
        setTimeout(() => {
          this.eventItem = freshData; // 重新赋值，UI 重新创建，读取新时间
          logger.error('[DEBUG_FORCE] UI 已重建，时间应为最新');
        }, 10);
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

        // 通知列表页刷新
        AppStorage.setOrCreate('refreshEventList', Date.now());

        this.pageStack.pop();
      }
    });
  }

  private handleEdit() {
    if (this.eventItem) {
      this.pageStack.pushPath({ name: 'EventEditPage', param: this.eventItem });
    }
  }

  @Builder
  DetailItem(label: string, value: string, icon?: Resource) {
    Row() {
      if (icon) {
        Image(icon).width(20).height(20).margin({ right: 12 }).fillColor(StyleConstants.COLOR_PRIMARY)
      } else {
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
    // NavDestination 是处理 Navigation 页面生命周期的关键
    NavDestination() {
      Column() {
        // --- 1. 标题栏 ---
        Row() {
          Image($r('app.media.ic_back'))
            .width(24).height(24)
            .onClick(() => this.pageStack.pop())

          Text('日程详情')
            .fontSize(20).fontWeight(FontWeight.Bold)
            .margin({ left: 16 })
            .layoutWeight(1)

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
              // --- 2. 顶部卡片 ---
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

              // --- 3. 详细信息 ---
              Column() {
                this.DetailItem('开始时间', DateUtils.formatFull(this.eventItem.startTime) + (this.forceUpdateTrigger >= 0 ? "" : ""))
                this.DetailItem('结束时间', DateUtils.formatFull(this.eventItem.endTime) + (this.forceUpdateTrigger >= 0 ? "" : ""))
                this.DetailItem('备注说明', this.eventItem.description || '暂无备注')
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
          Column() {
            Text('未找到日程信息').fontSize(16).fontColor(Color.Gray)
          }
          .height('100%').justifyContent(FlexAlign.Center)
        }
      }
      .width('100%').height('100%').backgroundColor('#F1F3F5')
    }
    .hideTitleBar(true)
    // 使用 .onShown() 生命周期
    // 当编辑页 pop，详情页重新显示时，这个方法会被系统自动调用
    .onShown(() => {
      logger.info('[DEBUG] onShown 触发，延时执行刷新...');
      setTimeout(() => {
        this.reloadFromDb();
      }, 50); // 50毫秒足够让页面稳定下来，从而正确响应 UI 刷新
    })
  }
}