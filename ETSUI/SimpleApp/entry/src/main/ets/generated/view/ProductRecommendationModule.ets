/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License,Version 2.0 (the "License");
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

import { LoadingStatus } from '../common/CommonEnums';
import { LoadingFailedView } from '../view/LoadingFailedView';
import { BreakpointConstants } from '../common/BreakpointConstants';
import { CommonConstants } from '../common/CommonConstants';
import { BreakpointType } from '../util/BreakpointType';
import LazyDataSource from '../util/LazyDataSource';
import {
  DataSource, Data
} from '../viewmodel/ProductRecommendationModuleData';

@Component
export struct ProductRecommendationModule {
  @State itemModel: Data =
    Data.getInstance();
  @State lazyDataSource: LazyDataSource<DataSource> =
    this.itemModel.lazyDataSource;
  @State loadingStatus: LoadingStatus = LoadingStatus.OFF;
  @Consume('appPathStack') appPathStack: NavPathStack;
  @StorageLink('currentWidthBreakpoint') currentWidthBreakpoint: string = BreakpointConstants.BREAKPOINT_SM;
  private columnNumArray: number[] = [2, 4, 4];

  aboutToAppear() {
    this.loadResources();
  }

  jump(pageName: string = 'EmptyPagePathStack'): void {
    this.appPathStack.pushPathByName(pageName, null);
  }

  loadResources(): void {
    this.loadingStatus = LoadingStatus.LOADING;
    this.itemModel.getResources().then(() => {
      this.loadingStatus = LoadingStatus.SUCCESS;
    }).catch(() => {
      this.loadingStatus = LoadingStatus.FAILED;
    });
  }

  build() {
    Column() {
      if (this.loadingStatus === LoadingStatus.FAILED) {
        LoadingFailedView(() => this.loadResources())
      } else if (this.loadingStatus === LoadingStatus.SUCCESS) {
        WaterFlow() {
          LazyForEach(this.lazyDataSource, (item: DataSource, index: number) => {
            FlowItem() {
              VerticalGraphicComplexTextItem({
                image: item.getImage(),
                title_label: item.getTitleLabel(),
                title: item.getTitle(),
                price_ten: item.getPriceTen(),
                price_dot: item.getPriceDot(),
                price_label: item.getPriceLabel(),
                des_labels: item.getDesLabels()
              })
            }
            .onClick(() => {
              this.jump("ProductRecommendationModuleDetail" + index.toString())
            })
            .width(CommonConstants.FULL_PERCENT)
          }, (item: DataSource, index: number) => index + JSON.stringify(item))
        }
        .nestedScroll({ scrollForward: NestedScrollMode.PARENT_FIRST, scrollBackward: NestedScrollMode.SELF_FIRST })
        .cachedCount(4)
        .columnsTemplate(("1fr ").repeat(new BreakpointType(this.columnNumArray[0], this.columnNumArray[1],
          this.columnNumArray[2]).getValue(this.currentWidthBreakpoint)).trim())
        .columnsGap("8vp")
        .rowsGap("8vp")
        .width(CommonConstants.FULL_PERCENT)
      }
    }
    .width('92%')
    .borderRadius($r('sys.float.corner_radius_level8'))
    .margin({ bottom: "10vp" })
  }
}

@Component
export struct VerticalGraphicComplexTextItem {
  @State image: Resource = $r('app.media.noImage')
  @State title_label: string = '自营'
  @State title: string = '苹果/AirPods Max 全新升级大功率aaa';
  @State price_ten: string = '￥3999';
  @State price_dot: string = '.00'
  @State price_label: string = '近期销量5万+'
  @State des_labels: string[] = ['百亿补贴', '国家补贴', '包邮']
  @State label_colors: string[] = ['#85CDAD', '#D95825', '#CB5B70', '#B89972']

  @Builder
  labelStyle(content: string, color: string) {
    Text(content)
      .fontSize($r('app.integer.caption_text_S'))
      .fontWeight(FontWeight.Regular)
      .fontColor(color)
      .borderWidth(0.5)
      .borderColor(color)
      .padding(1)
      .borderRadius(1)
  }

  build() {
    Column({ space: 4 }) {
      Image(this.image)
        .borderRadius($r('sys.float.corner_radius_level4'))
        .width(CommonConstants.FULL_PERCENT)
      Row() {
        if (this.title_label !== '') {
          Text(this.title_label)
            .fontSize($r('app.integer.caption_text_S'))
            .fontWeight(FontWeight.Regular)
            .fontColor($r('app.color.font_on_primary'))
            .backgroundColor('#DF4D4C')
            .padding(1)
            .borderRadius($r('sys.float.corner_radius_level1'))
        }
        Text(this.title)
          .width('90%')
          .fontSize($r('app.integer.caption_text_M'))
          .fontWeight(FontWeight.Medium)
          .fontColor($r('app.color.font_primary'))
          .maxLines(1)
          .textOverflow({ overflow: TextOverflow.None })
          .padding({ left: 2 })
      }
      .width(CommonConstants.FULL_PERCENT)
      .padding({
        left: 6,
        right: 6
      })
      .alignItems(VerticalAlign.Center)

      Row() {
        Text(this.price_ten)
          .fontSize($r('app.integer.body_text_S'))
          .fontWeight(FontWeight.Bold)
          .fontColor('#DF4D4C')
        Text(this.price_dot)
          .fontSize($r('app.integer.caption_text_M'))
          .fontWeight(FontWeight.Bold)
          .fontColor('#DF4D4C')
          .alignSelf(ItemAlign.Center)
        if (this.price_label.includes('补贴')) {
          Text(this.price_label)
            .fontSize(6)
            .fontWeight(FontWeight.Regular)
            .fontColor($r('app.color.font_on_primary'))
            .backgroundColor('#EB4C00')
            .padding(1)
            .borderRadius({
              topLeft: $r('sys.float.corner_radius_level1'),
              bottomLeft: $r('sys.float.corner_radius_level1'),
              topRight: $r('sys.float.corner_radius_level4'),
              bottomRight: $r('sys.float.corner_radius_level4')
            })
            .margin({ left: 4 })
        } else {
          Text(this.price_label)
            .fontSize($r('app.integer.caption_text_M'))
            .fontWeight(FontWeight.Regular)
            .fontColor($r('app.color.font_tertiary'))
            .margin({ left: 4 })
        }
      }
      .width(CommonConstants.FULL_PERCENT)
      .alignItems(VerticalAlign.Top)
      .padding({
        left: 4,
        right: 4
      })

      Row() {
        Row({ space: 4 }) {
          ForEach(this.des_labels, (content: string) => {
            if (content === '国家补贴') {
              this.labelStyle(content, this.label_colors[0])
            } else if (content.includes('首购')) {
              this.labelStyle(content, this.label_colors[2])
            } else if (content === '百亿补贴') {
              Text(content)
                .fontSize($r('app.integer.caption_text_S'))
                .fontWeight(FontWeight.Bold)
                .fontColor($r('app.color.font_on_primary'))
                .backgroundColor(this.label_colors[1])
                .padding(2)
                .borderRadius(2)
            } else {
              this.labelStyle(content, this.label_colors[3])
            }
          })
        }
        .width('90%')

        Text('+')
          .width(16)
          .height(16)
          .fontSize($r('app.integer.subtitle_text_S'))
          .fontWeight(FontWeight.Regular)
          .fontColor('#D15C7D')
          .backgroundColor('#FCECEF')
          .textAlign(TextAlign.Center)
          .borderRadius('50%')
      }
      .width(CommonConstants.FULL_PERCENT)
      .justifyContent(FlexAlign.SpaceBetween)
      .padding({
        left: 6,
        right: 6
      })
    }
    .width(CommonConstants.FULL_PERCENT)
  }
}