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
// entry/src/main/ets/pages/EventListPage.ets
import { Event, EventModel } from '../model/EventModel';
import { Logger } from '../common/utils/Logger';

const logger = new Logger('EventListPage');

@Component
export struct EventListPage {
  @State events: Array<Event> = [];
  @State keyword: string = '';

  @Consume('pageStack') pageStack: NavPathStack;

  @StorageProp('refreshEventList') @Watch('onRefreshChange') refreshSignal: number = 0;

  aboutToAppear() {
    this.loadEvents();
  }

  onRefreshChange() {
    this.loadEvents();
  }

  async loadEvents() {
    try {
      if (this.keyword && this.keyword.trim().length > 0) {
        this.events = await EventModel.search(this.keyword);
      } else {
        this.events = await EventModel.queryUpcoming();
      }
    } catch (err) {
      logger.error('Load events failed', JSON.stringify(err));
      this.events = [];
    }
  }

  async deleteEvent(index: number, id: number) {
    const success = await EventModel.deleteById(id);
    if (success) {
      this.events.splice(index, 1);
      logger.info(`Deleted event id: ${id}`);
    }
  }

  @Builder
  private CustomHeader(): void {
    Row() {
      Text($r('app.string.event_title'))
        .fontSize(24)
        .fontWeight(FontWeight.Bold)

      Blank()

      Button({ type: ButtonType.Circle, stateEffect: true }) {
        Text('+')
          .fontSize(24)
          .fontColor(Color.Black)
      }
      .width(40)
      .height(40)
      .backgroundColor(Color.Transparent)
      .onClick(() => {
        this.pageStack.pushPath({ name: 'EventEditPage' });
      })
    }
    .width('100%')
    .height(56)
    .padding({ left: 16, right: 16 })
    .alignItems(VerticalAlign.Center)
  }

  @Builder
  private SearchBar(): void {
    Row() {
      Search({ value: this.keyword, placeholder: '搜索日程标题、备注...' })
        .width('100%')
        .height(40)
        .backgroundColor(Color.White)
        .placeholderColor(Color.Gray)
        .placeholderFont({ size: 14 })
        .textFont({ size: 14 })
        .onChange((value: string) => {
          this.keyword = value;
          this.loadEvents();
        })
        .onSubmit((value: string) => {
          this.keyword = value;
          this.loadEvents();
        })
    }
    .width('100%')
    .padding({ left: 16, right: 16, bottom: 8 })
  }

  @Builder
  DeleteAction(index: number, id: number) {
    Button() {
      Text($r('app.string.btn_delete'))
        .fontColor(Color.White)
        .fontSize(14)
    }
    .width(80)
    .height('100%')
    .backgroundColor(Color.Red)
    .onClick(() => {
      this.deleteEvent(index, id);
    })
  }

  private formatTime(timestamp: number): string {
    const date = new Date(timestamp);
    const month = date.getMonth() + 1;
    const day = date.getDate();
    const hours = date.getHours().toString().padStart(2, '0');
    const minutes = date.getMinutes().toString().padStart(2, '0');
    return `${month}/${day} ${hours}:${minutes}`;
  }

  @Builder
  EventItem(item: Event, index: number) {
    ListItem() {
      Row() {
        Column()
          .width(4)
          .height(40)
          .backgroundColor('#007DFF')
          .borderRadius(2)
          .margin({ right: 12 })

        Column() {
          Text(item.title)
            .fontSize(16)
            .fontWeight(FontWeight.Medium)
            .fontColor(Color.Black)
            .maxLines(1)
            .textOverflow({ overflow: TextOverflow.Ellipsis })

          Text(item.isAllDay ? $r('app.string.label_all_day') : `${this.formatTime(item.startTime)} - ${this.formatTime(item.endTime)}`)
            .fontSize(14)
            .fontColor(Color.Gray)
            .margin({ top: 4 })
        }
        .alignItems(HorizontalAlign.Start)
        .layoutWeight(1)

        Text(item.isAllDay ? 'All Day' : $r('app.string.event_status_upcoming'))
          .fontSize(12)
          .fontColor('#007DFF')
          .backgroundColor('#E5F1FF')
          .padding({ left: 8, right: 8, top: 2, bottom: 2 })
          .borderRadius(10)
      }
      .width('100%')
      .padding(12)
      .backgroundColor(Color.White)
      .borderRadius(12)
    }
    .margin({ bottom: 10 })
    .onClick(() => {
      // ✅ [核心修改] 跳转到详情页，而不是直接编辑页
      this.pageStack.pushPath({ name: 'EventDetailPage', param: item });
    })
    .swipeAction({
      end: this.DeleteAction(index, item.id)
    })
  }

  build() {
    Column() {
      this.CustomHeader()
      this.SearchBar()

      GridRow({
        columns: { sm: 4, md: 8, lg: 12 },
        gutter: { x: 12, y: 12 }
      }) {
        GridCol({
          span: { sm: 4, md: 6, lg: 8 },
          offset: { sm: 0, md: 1, lg: 2 }
        }) {
          Column() {
            if (this.events.length === 0) {
              Column() {
                Image($r('app.media.app_icon'))
                  .width(64)
                  .height(64)
                  .opacity(0.5)
                  .grayscale(1)

                if (this.keyword.length > 0) {
                  Text('无搜索结果').fontSize(16).fontColor(Color.Gray).margin({ top: 16 })
                } else {
                  Text($r('app.string.event_empty_tip')).fontSize(16).fontColor(Color.Gray).margin({ top: 16 })
                }
              }
              .justifyContent(FlexAlign.Center)
              .height('80%')
              .width('100%')
            } else {
              List() {
                ForEach(this.events, (item: Event, index: number) => {
                  this.EventItem(item, index)
                }, (item: Event) => item.id.toString())
              }
              .width('100%')
              .layoutWeight(1)
              .padding({ left: 16, right: 16, top: 10 })
            }
          }
          .width('100%')
          .height('100%')
        }
      }
      .layoutWeight(1)
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F1F3F5')
  }
}