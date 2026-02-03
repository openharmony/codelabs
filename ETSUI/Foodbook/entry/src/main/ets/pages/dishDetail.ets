/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
// dishDetail.ets
import router from '@ohos.router'; // 修正后的导入路径
import { Constants } from '../common/Constants';
import { Dish } from './Index';

interface RouterParams {
  id: string;
}

@Entry
@Component
struct DishDetail {
  @StorageProp('favoriteList') favorites: Dish[] = [];
  @StorageProp('likeList') likes: Dish[] = [];
  @StorageProp('historyList') histories: Dish[] = [];
  @State dishData: Dish = {
    id: '',
    name: '',
    description: '',
    image: $r('app.media.run'),
    ingredients: '',
    seasonings: '',
    routeUrl: 'newpages/huangmenji'
  };

  aboutToAppear() {
    const params = router.getParams() as RouterParams;
    if (params?.id) {
      const target = this.favorites.find(item => item.id === params.id);
      if (target) {
        this.dishData = target;
      } else {
        const historyTarget = this.histories.find(item => item.id === params.id);
        if (historyTarget) {
          this.dishData = historyTarget;
        }
      }
    }

    if (this.dishData.id) {
      this.addToHistory();
    }
  }

  addToHistory(): void {
    const history: Dish[] = AppStorage.get(Constants.HISTORY_LIST) || [];
    const filtered = history.filter((item: Dish) => item.id !== this.dishData.id);
    AppStorage.set(Constants.HISTORY_LIST, [this.dishData, ...filtered]);
  }

  build() {
    Column() {
      Row() {
        Image($r('app.media.return1'))
          .width(30)
          .height(30)
          .margin(10)
          .onClick(() => router.back())
      }
      .width('100%')
      .justifyContent(FlexAlign.Start)

      Image(this.dishData.image)
        .width('100%')
        .height(300)
        .objectFit(ImageFit.Cover)

      Scroll() {
        Column({ space: 15 }) {
          Text(this.dishData.name)
            .fontSize(24)
            .fontWeight(FontWeight.Bold)
            .margin({ top: 10 })

          Text(this.dishData.description)
            .fontSize(16)
            .fontColor('#666')
            .padding(10)

          Divider().strokeWidth(1).color('#eee')

          Column() {
            Text('食材清单').fontSize(18).fontWeight(500)
            Text(this.dishData.ingredients)
              .fontSize(14)
              .margin({ top: 8 })
          }
          .padding(10)

          Divider().strokeWidth(1).color('#eee')

          Column() {
            Text('所需调料').fontSize(18).fontWeight(500)
            Text(this.dishData.seasonings)
              .fontSize(14)
              .margin({ top: 8 })
          }
          .padding(10)
        }
      }
    }
    .height('100%')
  }
}
