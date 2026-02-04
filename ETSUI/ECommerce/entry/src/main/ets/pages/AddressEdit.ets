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
// 地址编辑页面
struct AddressEdit {
  @StorageLink('currentUser') currentUser: User | undefined = undefined;

  @State isEdit: boolean = false;
  @State editId: number = 0;

  @State name: string = '';
  @State phone: string = '';
  @State province: string = '';
  @State city: string = '';
  @State detail: string = '';
  @State label: string = '家';
  @State isDefault: boolean = false;

  private readonly labelOptions: string[] = ['家', '公司', '学校', '其他'];

  aboutToAppear() {
    if (!this.currentUser) {
      promptAction.showToast({ message: '请先登录' });
      router.back();
      return;
    }

    const params = router.getParams() as Record<string, Object>;
    if (params && params['address']) {
      const a = params['address'] as Address;
      this.isEdit = true;
      this.editId = a.id || 0;
      this.name = (a.name || '').toString();
      this.phone = (a.phone || '').toString();
      this.province = (a.province || '').toString();
      this.city = (a.city || '').toString();
      this.detail = (a.detail || '').toString();
      this.label = (a.label || '其他').toString();
      this.isDefault = !!a.isDefault;
    } else {
      this.isEdit = false;
      this.editId = 0;
    }
  }

  private validate(): boolean {
    if (!this.name.trim() || !this.phone.trim() || !this.province.trim() || !this.city.trim() || !this.detail.trim()) {
      promptAction.showToast({ message: '请把信息填写完整' });
      return false;
    }
    return true;
  }

  // 保存地址
  private async save() {
    if (!this.currentUser) return;
    if (!this.validate()) return;

    const a = new Address(
      this.isEdit ? this.editId : 0,
      this.currentUser.id,
      this.name.trim(),
      this.phone.trim(),
      this.province.trim(),
      this.city.trim(),
      this.detail.trim(),
      this.isDefault,
      (this.label || '其他').trim()
    );

    if (this.isEdit) {
      await RdbUtil.updateAddress(a);
    } else {
      await RdbUtil.addAddress(a);
    }

    promptAction.showToast({ message: '已保存' });
    router.back();
  }

  private async deleteCurrent() {
    if (!this.isEdit) return;
    await RdbUtil.deleteAddress(this.editId);
    promptAction.showToast({ message: '已删除' });
    router.back();
  }

  @Builder
  SectionTitle(title: string) {
    Text(title)
      .fontSize(14)
      .fontWeight(FontWeight.Bold)
      .fontColor('#111827')
      .margin({ bottom: 10 });
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

        Text(this.isEdit ? '编辑收货地址' : '新增收货地址')
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .fontColor('#111827');

        Blank();
      }
      .width('100%')
      .height(56)
      .padding({ left: 6, right: 12 })
      .backgroundColor(Color.White);

      Scroll() {
        Column({ space: 12 }) {
          Column() {
            this.SectionTitle('收货信息');

            TextInput({ placeholder: '收货人姓名', text: this.name })
              .height(44)
              .fontSize(14)
              .backgroundColor('#F7FAFC')
              .borderRadius(12)
              .padding({ left: 12, right: 12 })
              .onChange((v: string) => {
                this.name = v;
              });

            TextInput({ placeholder: '手机号', text: this.phone })
              .type(InputType.PhoneNumber)
              .height(44)
              .fontSize(14)
              .backgroundColor('#F7FAFC')
              .borderRadius(12)
              .padding({ left: 12, right: 12 })
              .onChange((v: string) => {
                this.phone = v;
              });
          }
          .padding(14)
          .backgroundColor(Color.White)
          .borderRadius(12);

          Column() {
            this.SectionTitle('地址信息');

            Row({ space: 10 }) {
              TextInput({ placeholder: '省份', text: this.province })
                .layoutWeight(1)
                .height(44)
                .fontSize(14)
                .backgroundColor('#F7FAFC')
                .borderRadius(12)
                .padding({ left: 12, right: 12 })
                .onChange((v: string) => {
                  this.province = v;
                });

              TextInput({ placeholder: '城市', text: this.city })
                .layoutWeight(1)
                .height(44)
                .fontSize(14)
                .backgroundColor('#F7FAFC')
                .borderRadius(12)
                .padding({ left: 12, right: 12 })
                .onChange((v: string) => {
                  this.city = v;
                });
            }

            TextInput({ placeholder: '详细地址（街道/门牌号）', text: this.detail })
              .height(44)
              .fontSize(14)
              .backgroundColor('#F7FAFC')
              .borderRadius(12)
              .padding({ left: 12, right: 12 })
              .onChange((v: string) => {
                this.detail = v;
              });
          }
          .padding(14)
          .backgroundColor(Color.White)
          .borderRadius(12);

          Column({ space: 12 }) {
            this.SectionTitle('标签与默认');

            Row({ space: 10 }) {
              Text('地址标签')
                .fontSize(14)
                .fontColor('#111827');
              Blank();
            }

            Row({ space: 10 }) {
              ForEach(this.labelOptions, (t: string) => {
                Button(t)
                  .type(ButtonType.Normal)
                  .height(32)
                  .fontSize(13)
                  .fontColor(this.label === t ? Color.White : '#111827')
                  .backgroundColor(this.label === t ? $r('app.color.address_manage_bg') : '#F3F4F6')
                  .borderRadius(16)
                  .onClick(() => {
                    this.label = t;
                  });
              });
            }

            Row() {
              Text('设为默认地址')
                .fontSize(14)
                .fontColor('#111827');
              Blank();
              Checkbox()
                .select(this.isDefault)
                .onChange((v: boolean) => this.isDefault = v);
            }
            .padding({ top: 4 });
          }
          .padding(14)
          .backgroundColor(Color.White)
          .borderRadius(12);

          if (this.isEdit) {
            Button('删除地址')
              .height(44)
              .fontSize(14)
              .fontColor(Color.White)
              .backgroundColor('#FF4D4F')
              .borderRadius(22)
              .onClick(async () => {
                await this.deleteCurrent();
              });
          }

          Column() {
          }
          .height(70);
        }
        .padding(12);
      }
      .layoutWeight(1)
      .backgroundColor('#F5F5F5');

      Row() {
        Button('保存')
          .layoutWeight(1)
          .height(46)
          .fontColor(Color.White)
          .backgroundColor($r('app.color.address_manage_bg'))
          .borderRadius(23)
          .onClick(async () => {
            await this.save();
          });
      }
      .padding({ left: 16, right: 16, top: 10, bottom: 14 })
      .backgroundColor('#F5F5F5');
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F5F5F5');
  }
}