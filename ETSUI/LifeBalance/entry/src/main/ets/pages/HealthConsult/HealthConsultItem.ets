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
import { HealthConsultData } from "../../model/HealthConsultModel";
import { HEALTH_TAB_ID_TO_NAME } from "../../model/HealthConsultTabModel";
import promptAction from '@ohos.promptAction'
import HealthConsultFavoriteModel from "../../model/HealthConsultFavoriteModel";

/**
 * The health consult list item component.
 */
@Component
export default struct HealthConsultItem {
  healthConsultData: HealthConsultData | null = null;
  @State isFavorite: boolean = false;

  // 1. 【新增】接收外部传入的缩放比例
  @Prop fontScale: number = 1.0;

  build() {
    Column({ space: 8 }) {
      Flex({
        justifyContent: FlexAlign.SpaceBetween,
        alignItems: ItemAlign.Center
      }) {
        Text(HEALTH_TAB_ID_TO_NAME[this.healthConsultData!.tabId])
          .padding({
            left: 14,
            right: 14,
            top: 6,
            bottom: 6
          })
          .backgroundColor('#F0F7FF')
          .fontColor($r('sys.color.brand'))
          .borderRadius(15)
          .fontSize(13 * this.fontScale)
          .fontWeight(FontWeight.Medium)

        SymbolGlyph($r(this.isFavorite ? 'sys.symbol.star_fill' : 'sys.symbol.star'))
          .fontColor(["#EAC452"])
          .fontSize(20 * this.fontScale)
          .padding(6)
          .onClick(e => {
            this.isFavorite = !this.isFavorite
            HealthConsultFavoriteModel.setFavorite(this.healthConsultData!.id, this.isFavorite)
            promptAction.showToast({
              message: this.isFavorite ? '已收藏' : '已取消收藏',
              duration: 1000,
              bottom: 100
            })
          })
      }
      .width('100%')

      Text(this.healthConsultData!.title)
        .fontSize(18 * this.fontScale)
        .fontColor('#000')
        .maxLines(1)
        .textOverflow({ overflow: TextOverflow.Ellipsis })
        .fontWeight(500)
        .width('100%')

      Text(this.healthConsultData!.abstract)
        .fontSize(14 * this.fontScale)
        .fontColor('#333')
        .maxLines(3)
        .textOverflow({ overflow: TextOverflow.Ellipsis })
        .width('100%')

    }
    .padding({
      left: 12,
      right: 12,
      top: 20,
      bottom: 20
    })
    .alignItems(HorizontalAlign.Start)
  }
}
