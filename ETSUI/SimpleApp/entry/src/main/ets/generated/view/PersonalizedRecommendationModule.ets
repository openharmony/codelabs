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
import LazyDataSource from '../util/LazyDataSource';
import { CommonConstants } from '../common/CommonConstants'
import {
  VerticalGraphicTextShareItemModel,
  DataSourceStyleAStyleA
} from '../viewmodel/PersonalizedRecommendationModuleData';

@Component
export struct PersonalizedRecommendationModule {
  @State itemModel: DataSourceStyleAStyleA = DataSourceStyleAStyleA.getInstance();
  @State lazyDataSource: LazyDataSource<VerticalGraphicTextShareItemModel> = this.itemModel.lazyDataSource;
  @State loadingStatus: LoadingStatus = LoadingStatus.OFF;
  @State images: Resource[] = [$r('app.media.recommend1'), $r('app.media.recommend2'), $r('app.media.recommend3')]
  @StorageLink('currentWidthBreakpoint') currentWidthBreakpoint: string = BreakpointConstants.BREAKPOINT_SM;
  @Consume('appPathStack') appPathStack: NavPathStack;

  aboutToAppear() {
    this.loadResources();
  }

  jump(pageName: string = 'EmptyPagePathStack'): void {
    this.appPathStack.pushPathByName(pageName, null);
  }

  loadResources(): void {
    this.loadingStatus = LoadingStatus.LOADING;
    this.itemModel.getResources()
      .then(() => {
        this.loadingStatus = LoadingStatus.SUCCESS;
      })
      .catch(() => {
        this.loadingStatus = LoadingStatus.FAILED;
      });
  }

  build() {
    Column() {
      if (this.loadingStatus === LoadingStatus.FAILED) {
        LoadingFailedView(() => this.loadResources())
      } else if (this.loadingStatus === LoadingStatus.SUCCESS) {
        WaterFlow() {
          LazyForEach(this.lazyDataSource, (item: VerticalGraphicTextShareItemModel, index: number) => {
            FlowItem() {
              ProductCard({
                image: item.getImage(),
                title: item.getTitle(),
                subtitle: item.getSubtitle(),
                price: item.getPrice(),
                priceDetail: item.getPriceCaption()
              })
            }
            .width(CommonConstants.FULL_PERCENT)
            .onClick(() => this.jump("PersonalizedRecommendationModuleDetail" + index.toString()))
          }, (item: VerticalGraphicTextShareItemModel, index: number) => index + JSON.stringify(item))
        }
        .columnsTemplate('1fr 1fr')
        .columnsGap(8)
        .rowsGap(8)
        .width(CommonConstants.FULL_PERCENT)
      }
    }
    .width('92%')
  }
}

@Component
export struct ProductCard {
  @State image: Resource = $r('app.media.recommend4')
  @State title: string = ''
  @State subtitle: string = ''
  @State price: string = ''
  @State priceDetail: string = ''
  @State tags: Array<string> = []
  @State monthSales: string = ''
  @State buttonType: string = 'default'

  build() {
    Column() {
      Image(this.image)
        .width(CommonConstants.FULL_PERCENT)
        .aspectRatio(1)
        .borderRadius($r('sys.float.corner_radius_level8'))
      Column() {
        Text(this.title)
          .fontSize($r('app.integer.subtitle_text_S'))
          .fontWeight(FontWeight.Medium)
          .fontColor($r('app.color.font_primary'))
          .width(CommonConstants.FULL_PERCENT)
          .textAlign(TextAlign.Start)
          .maxLines(2)
          .textOverflow({ overflow: TextOverflow.Ellipsis })
        if (this.subtitle) {
          Text(this.subtitle)
            .fontSize($r('app.integer.body_text_S'))
            .fontColor($r('app.color.font_secondary'))
            .fontWeight(FontWeight.Regular)
            .width(CommonConstants.FULL_PERCENT)
            .margin({ top: 4 })
        }
        Row() {
          Text('¥' + this.price)
            .fontSize($r('app.integer.subtitle_text_M'))
            .fontColor('#ffec4b32')
            .fontWeight(FontWeight.Bold)
            .margin({ right: 2 })
          Text(this.priceDetail)
            .fontSize($r('app.integer.body_text_S'))
            .fontColor('#ff3b3b3b')
        }
        .alignItems(VerticalAlign.Bottom)
        .margin({ top: 4 })
      }
      .width(CommonConstants.FULL_PERCENT)
      .padding(8)
      .alignItems(HorizontalAlign.Start)
    }
    .width(CommonConstants.FULL_PERCENT)
    .backgroundColor($r('app.color.comp_background_primary_trans'))
    .borderRadius($r('sys.float.corner_radius_level8'))
  }
}