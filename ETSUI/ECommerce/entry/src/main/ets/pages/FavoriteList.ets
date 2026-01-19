/* 
  * Copyright (c) 2026 Huawei Device Co., Ltd. 
  * Licensed under the Apache License, Version 2.0 (the "License"); 
  * you may not use this file except in compliance with the License. 
  * You may obtain a copy of the License at 
  * 
  *     `http://www.apache.org/licenses/LICENSE-2.0`  
  * 
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
  * See the License for the specific language governing permissions and 
  * limitations under the License. 
*/

import { promptAction, router } from '@kit.ArkUI';
import { Product, User } from '../model/DataModels';
import { RdbUtil } from '../utils/RdbUtil';

@Entry
@Component
// 收藏列表页面
struct FavoriteList {
  @StorageLink('currentUser') currentUser: User | undefined = undefined;
  @State favoriteProducts: Product[] = [];
  @State isManaging: boolean = false;
  @State selectedIds: number[] = [];

  async aboutToAppear() {
    if (!this.currentUser) {
      promptAction.showToast({ message: '请先登录' });
      router.back();
      return;
    }
    await this.refresh();
  }

  // 刷新收藏列表
  async refresh() {
    if (!this.currentUser) {
      this.favoriteProducts = [];
      this.selectedIds = [];
      this.isManaging = false;
      return;
    }
    this.favoriteProducts = (await RdbUtil.getFavoriteProducts(this.currentUser.id)).slice();

    if (this.selectedIds.length > 0) {
      const idSet = new Set<number>(this.favoriteProducts.map(p => p.id));
      this.selectedIds = this.selectedIds.filter(id => idSet.has(id));
    }

    if (this.favoriteProducts.length === 0) {
      this.selectedIds = [];
      this.isManaging = false;
    }
  }

  toggleManage() {
    this.isManaging = !this.isManaging;
    if (!this.isManaging) {
      this.selectedIds = [];
    }
  }

  private isSelected(id: number): boolean {
    return this.selectedIds.indexOf(id) >= 0;
  }

  private setSelected(id: number, selected: boolean) {
    const idx = this.selectedIds.indexOf(id);
    if (selected) {
      if (idx < 0) {
        this.selectedIds = this.selectedIds.concat([id]);
      }
      return;
    }
    if (idx >= 0) {
      const next = this.selectedIds.slice();
      next.splice(idx, 1);
      this.selectedIds = next;
    }
  }

  private toggleSelected(id: number) {
    this.setSelected(id, !this.isSelected(id));
  }

  private isAllSelected(): boolean {
    return this.favoriteProducts.length > 0 && this.selectedIds.length === this.favoriteProducts.length;
  }

  private toggleAll(selected: boolean) {
    if (!selected) {
      this.selectedIds = [];
      return;
    }
    this.selectedIds = this.favoriteProducts.map(p => p.id);
  }

  private async deleteSelected() {
    if (!this.currentUser) return;
    if (this.selectedIds.length === 0) {
      promptAction.showToast({ message: '请选择要删除的收藏' });
      return;
    }

    for (const pid of this.selectedIds) {
      await RdbUtil.removeFavorite(this.currentUser.id, pid);
    }

    this.selectedIds = [];
    await this.refresh();
    promptAction.showToast({ message: '已删除' });
  }

  @Builder
  FavoriteSwipe(p: Product) {
    Row() {
      Button('取消收藏')
        .type(ButtonType.Normal)
        .backgroundColor('#FF4D4F')
        .fontColor(Color.White)
        .width(84)
        .height('100%')
        .onClick(async () => {
          if (!this.currentUser) {
            return;
          }
          await RdbUtil.removeFavorite(this.currentUser.id, p.id);
          await this.refresh();
          promptAction.showToast({ message: '已取消收藏' });
        });
    }
    .height('100%');
  }

  build() {
    Column() {
      Row() {
        Button({ type: ButtonType.Normal, stateEffect: true }) {
          Text('<')
            .fontSize(24)
            .fontColor('#111827');
        }
        .backgroundColor(Color.Transparent)
        .width(44)
        .height(44)
        .onClick(() => {
          router.back();
        });

        Text(`我的收藏(${this.favoriteProducts.length})`)
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .fontColor('#111827');

        Blank();

        Button(this.isManaging ? '完成' : '管理')
          .type(ButtonType.Normal)
          .height(32)
          .fontSize(13)
          .fontColor('#6B7280')
          .backgroundColor(Color.Transparent)
          .enabled(this.favoriteProducts.length > 0)
          .onClick(() => {
            this.toggleManage();
          });

        Button('刷新')
          .type(ButtonType.Normal)
          .height(32)
          .fontSize(13)
          .fontColor('#6B7280')
          .backgroundColor(Color.Transparent)
          .onClick(async () => {
            await this.refresh();
          });
      }
      .width('100%')
      .height(56)
      .padding({ left: 6, right: 12 })
      .backgroundColor(Color.White);

      if (this.favoriteProducts.length === 0) {
        Column({ space: 12 }) {
          Text('暂无收藏')
            .fontSize(16)
            .fontColor('#6B7280');

          Button('去逛逛')
            .height(44)
            .width('60%')
            .backgroundColor('#2563EB')
            .fontColor(Color.White)
            .borderRadius(22)
            .onClick(() => {
              router.back();
            });
        }
        .layoutWeight(1)
        .width('100%')
        .justifyContent(FlexAlign.Center)
        .alignItems(HorizontalAlign.Center)
        .backgroundColor('#F5F5F5');
      } else {
        List({ space: 10 }) {
          ForEach(this.favoriteProducts, (p: Product) => {
            ListItem() {
              Row({ space: 12 }) {
                if (this.isManaging) {
                  Checkbox()
                    .select(this.isSelected(p.id))
                    .onChange((v: boolean) => {
                      this.setSelected(p.id, v);
                    });
                }

                Image(RdbUtil.resolveImageSource(p.image))
                  .width(72)
                  .height(72)
                  .borderRadius(12)
                  .backgroundColor('#F3F4F6');

                Column({ space: 6 }) {
                  Text(p.name)
                    .fontSize(14)
                    .fontWeight(FontWeight.Bold)
                    .fontColor('#111827')
                    .maxLines(2)
                    .textOverflow({ overflow: TextOverflow.Ellipsis });

                  Row({ space: 8 }) {
                    Text(`¥${p.price}`)
                      .fontSize(16)
                      .fontWeight(FontWeight.Bold)
                      .fontColor('#FF5000');

                    Text(p.category)
                      .fontSize(11)
                      .fontColor('#6B7280')
                      .backgroundColor('#F3F4F6')
                      .padding({ left: 6, right: 6, top: 2, bottom: 2 })
                      .borderRadius(10);

                    Blank();
                  }
                }
                .layoutWeight(1)
                .alignItems(HorizontalAlign.Start);
              }
              .padding(12)
              .backgroundColor(Color.White)
              .borderRadius(14);
            }
            .swipeAction(this.isManaging ? undefined : { end: this.FavoriteSwipe(p) })
            .onClick(() => {
              if (this.isManaging) {
                this.toggleSelected(p.id);
              } else {
                router.pushUrl({ url: 'pages/ProductDetail', params: { product: p } });
              }
            });
          });
        }
        .padding({ left: 12, right: 12, top: 10, bottom: 10 })
        .layoutWeight(1)
        .backgroundColor('#F5F5F5');
      }

      if (this.isManaging && this.favoriteProducts.length > 0) {
        Row({ space: 10 }) {
          Row({ space: 8 }) {
            Checkbox()
              .select(this.isAllSelected())
              .onChange((v: boolean) => {
                this.toggleAll(v);
              });

            Text('全选')
              .fontSize(14)
              .fontColor('#111827');
          }

          Blank();

          Button(`删除(${this.selectedIds.length})`)
            .height(40)
            .fontSize(14)
            .fontColor(Color.White)
            .backgroundColor(this.selectedIds.length > 0 ? '#FF4D4F' : '#D1D5DB')
            .borderRadius(20)
            .enabled(this.selectedIds.length > 0)
            .onClick(async () => {
              await this.deleteSelected();
            });
        }
        .padding({ left: 16, right: 16, top: 8, bottom: 10 })
        .backgroundColor('#F5F5F5');
      }
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F5F5F5');
  }
}