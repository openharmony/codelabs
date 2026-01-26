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
import { Address, User } from '../model/DataModels';
import { RdbUtil } from '../utils/RdbUtil';

@Entry
@Component
// 地址管理页面
struct AddressManage {
  @StorageLink('currentUser') currentUser: User | undefined = undefined;

  @State addressList: Address[] = [];
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

  // 刷新地址列表
  async refresh() {
    if (!this.currentUser) {
      return;
    }
    this.addressList = (await RdbUtil.getAddresses(this.currentUser.id)).slice();
    if (this.selectedIds.length > 0) {
      const idSet = new Set<number>(this.addressList.map(a => a.id));
      this.selectedIds = this.selectedIds.filter(id => idSet.has(id));
    }
  }

  private openCreate() {
    router.pushUrl({ url: 'pages/AddressEdit' });
  }

  private openEdit(a: Address) {
    router.pushUrl({ url: 'pages/AddressEdit', params: { address: a } });
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
    return this.addressList.length > 0 && this.selectedIds.length === this.addressList.length;
  }

  private toggleAll(selected: boolean) {
    if (!selected) {
      this.selectedIds = [];
      return;
    }
    this.selectedIds = this.addressList.map(a => a.id);
  }

  private toggleManage() {
    this.isManaging = !this.isManaging;
    if (!this.isManaging) {
      this.selectedIds = [];
    }
  }

  private async deleteSelected() {
    if (!this.currentUser) return;
    if (this.selectedIds.length === 0) {
      promptAction.showToast({ message: '请选择要删除的地址' });
      return;
    }

    for (const id of this.selectedIds) {
      await RdbUtil.deleteAddress(id);
    }

    this.selectedIds = [];
    await this.refresh();
    promptAction.showToast({ message: '已删除' });
  }

  private maskPhone(phone: string): string {
    const p = (phone || '').trim();
    if (p.length < 7) return p;
    return `${p.slice(0, 3)}****${p.slice(-4)}`;
  }

  private tagBg(label: string): string {
    const l = (label || '').trim();
    if (l === '家') return '#FFF7E6';
    if (l === '公司') return '#E6FFFB';
    return '#F0F5FF';
  }

  private tagFg(label: string): string {
    const l = (label || '').trim();
    if (l === '家') return '#FA8C16';
    if (l === '公司') return '#13C2C2';
    return '#2F54EB';
  }

  @Builder
  AddressSwipe(a: Address) {
    Row({ space: 8 }) {
      if (!a.isDefault) {
        Button('设默认')
          .width(76)
          .height('100%')
          .fontColor(Color.White)
          .backgroundColor('#1698CE')
          .onClick(async () => {
            if (!this.currentUser) return;
            await RdbUtil.setDefaultAddress(this.currentUser.id, a.id);
            await this.refresh();
          });
      }

      Button('删除')
        .width(66)
        .height('100%')
        .fontColor(Color.White)
        .backgroundColor('#FF4D4F')
        .onClick(async () => {
          await RdbUtil.deleteAddress(a.id);
          await this.refresh();
        });
    }
    .height('100%')
    .padding({ left: 10, right: 10 });
  }

  @Builder
  AddressCard(a: Address) {
    Row() {
      Column() {
      }
      .width(4)
      .height(54)
      .backgroundColor(a.isDefault ? '#FF5000' : '#E5E7EB')
      .borderRadius(2)
      .margin({ right: 10, top: 4, bottom: 4 });

      Column({ space: 10 }) {
        Row() {
          Text(a.name)
            .fontSize(16)
            .fontWeight(FontWeight.Bold)
            .fontColor('#111827');

          Text(this.maskPhone(a.phone))
            .fontSize(14)
            .fontColor('#111827')
            .margin({ left: 12 });

          Blank();

          Button('编辑')
            .type(ButtonType.Normal)
            .height(28)
            .fontSize(13)
            .fontColor('#1698CE')
            .backgroundColor(Color.Transparent)
            .onClick(() => {
              this.openEdit(a);
            });
        }

        Text(`${a.province}${a.city}${a.detail}`)
          .fontSize(13)
          .fontColor('#4B5563');

        Row({ space: 8 }) {
          if (a.isDefault) {
            Text('默认')
              .fontSize(12)
              .fontColor('#FF5000')
              .backgroundColor('#FFF2E8')
              .padding({ left: 8, right: 8, top: 3, bottom: 3 })
              .borderRadius(10);
          }

          Text((a.label || '其他').trim())
            .fontSize(12)
            .fontColor(this.tagFg(a.label))
            .backgroundColor(this.tagBg(a.label))
            .padding({ left: 8, right: 8, top: 3, bottom: 3 })
            .borderRadius(10);
        }
      }
      .layoutWeight(1)
      .alignItems(HorizontalAlign.Start);
    }
    .padding(14)
    .backgroundColor(Color.White)
    .borderRadius(12)
    .shadow({ radius: 8, color: '#12000000', offsetY: 2 });
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

        Text('收货地址')
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
          .onClick(() => {
            this.toggleManage();
          });
      }
      .width('100%')
      .height(56)
      .padding({ left: 6, right: 12 })
      .backgroundColor(Color.White);

      Column() {
        if (this.addressList.length === 0) {
          Column({ space: 10 }) {
            Text('暂无收货地址')
              .fontSize(14)
              .fontColor('#6B7280');
          }
          .width('100%')
          .margin({ top: 120 })
          .alignItems(HorizontalAlign.Center);
        } else {
          List({ space: 8 }) {
            ForEach(this.addressList, (a: Address) => {
              ListItem() {
                Row({ space: 10 }) {
                  if (this.isManaging) {
                    Checkbox()
                      .select(this.isSelected(a.id))
                      .onChange((v: boolean) => {
                        this.setSelected(a.id, v);
                      });
                  }

                  Column() {
                    this.AddressCard(a);
                  }
                  .layoutWeight(1)
                  .onClick(() => {
                    if (this.isManaging) {
                      this.toggleSelected(a.id);
                    }
                  });
                }
              }
              .swipeAction(this.isManaging ? undefined : { end: this.AddressSwipe(a) })
            });
          }
          .width('100%')
          .padding({ left: 12, right: 12, top: 8, bottom: 8 });
        }
      }
      .layoutWeight(1)
      .backgroundColor('#F5F5F5')
      .onVisibleAreaChange([0.0, 1.0], async (isVisible: boolean) => {
        if (isVisible) {
          await this.refresh();
        }
      });

      if (this.isManaging) {
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
      } else {
        Row() {
          Button('新增收货地址')
            .layoutWeight(1)
            .height(46)
            .fontColor(Color.White)
            .backgroundColor($r('app.color.address_manage_bg'))
            .borderRadius(23)
            .onClick(() => {
              this.openCreate();
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