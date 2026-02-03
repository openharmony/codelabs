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

import router from '@ohos.router';
import promptAction from '@ohos.promptAction';
import { MedicineItem, MockData } from '../model/MockData';

@Entry
@Component
struct MedicineDetailPage {
  @State medicineId: number = 0;
  @State item: MedicineItem | undefined = undefined;
  // 编辑模式状态
  @State isEditMode: boolean = false;
  // 表单数据
  @State editName: string = '';
  @State editCategory: string = '';
  @State editLocation: string = '';
  @State editUsage: string = '';
  @State editDate: string = '';

  aboutToAppear() {
    const params = router.getParams() as Record<string, number>;
    if (params && params['id']) {
      this.medicineId = params['id'];
      this.item = MockData.getMedicineById(this.medicineId);

      // 初始化表单数据
      if (this.item) {
        this.editName = this.item.name;
        this.editCategory = this.item.category;
        this.editLocation = this.item.location;
        this.editUsage = this.item.usage;
        this.editDate = this.item.expiryDate;
      }
    }
  }

  build() {
    Column() {
      // 导航栏
      Row() {
        Button('<').onClick(() => router.back()).backgroundColor(Color.Transparent).fontColor(Color.Black).fontSize(24)
        Text('药品详情').fontSize(20).fontWeight(FontWeight.Bold).margin({ left: 10 })
        Blank()
        // 编辑/保存 按钮
        Text(this.isEditMode ? '保存' : '编辑')
          .fontColor('#4A90E2')
          .fontSize(16)
          .onClick(() => {
            if (this.isEditMode) {
              // 保存逻辑
              this.saveChanges();
            } else {
              // 进入编辑模式
              this.isEditMode = true;
            }
          })
      }
      .width('100%').padding(16)

      if (this.item) {
        Scroll() {
          Column() {
            this.DetailItem('药品名称', this.editName, (val) => this.editName = val)
            this.DetailItem('功能分类', this.editCategory, (val) => this.editCategory = val) // 实际可以用下拉
            this.DetailItem('有效期至', this.editDate, (val) => this.editDate = val)
            this.DetailItem('用法用量', this.editUsage, (val) => this.editUsage = val)
            this.DetailItem('存放位置', this.editLocation, (val) => this.editLocation = val)

            // 状态显示
            if (!this.isEditMode) {
              Row() {
                Text('状态：').fontSize(16).fontColor('#666')
                Text(this.item.isExpired ? '已过期' : `剩余 ${this.item.daysRemaining} 天`)
                  .fontSize(16)
                  .fontColor(this.item.isExpired ? Color.Red : '#4CAF50')
                  .fontWeight(FontWeight.Bold)
              }
              .width('100%')
              .padding(16)
              .backgroundColor(Color.White)
              .margin({ bottom: 20 })
              .borderRadius(12)
            }

            // 删除按钮
            Button('删除该药品')
              .width('100%')
              .height(50)
              .backgroundColor('#FF5252')
              .margin({ top: 30 })
              .onClick(() => {
                AlertDialog.show({
                  title: '确认删除',
                  message: '确定要删除这个药品吗？操作无法撤销。',
                  primaryButton: {
                    value: '取消', action: () => {
                    }
                  },
                  secondaryButton: {
                    value: '删除',
                    fontColor: Color.Red,
                    action: () => {
                      MockData.deleteMedicine(this.medicineId);
                      try {
                        promptAction.showToast({ message: '删除成功' });
                      } catch (e) {
                      }
                      router.back();
                    }
                  }
                })
              })
          }
          .padding(16)
        }
      } else {
        Text('未找到药品信息').margin({ top: 50 })
      }
    }
    .height('100%').backgroundColor('#F5F6FA')
  }

  @Builder
  DetailItem(label: string, value: string, onChange: (val: string) => void) {
    Column() {
      Text(label).fontSize(14).fontColor('#888').margin({ bottom: 6 })
      if (this.isEditMode) {
        TextInput({ text: value })
          .height(40)
          .backgroundColor(Color.White)
          .borderRadius(8)
          .onChange(onChange)
      } else {
        Text(value).fontSize(16).fontColor('#333').fontWeight(FontWeight.Medium)
      }
    }
    .alignItems(HorizontalAlign.Start)
    .width('100%')
    .padding(12)
    .backgroundColor(this.isEditMode ? 'transparent' : Color.White)
    .borderRadius(12)
    .margin({ bottom: 12 })
  }

  saveChanges() {
    MockData.updateMedicine(
      this.medicineId,
      this.editName,
      this.editCategory,
      this.editDate,
      this.editUsage,
      this.editLocation
    );
    this.isEditMode = false;
    // 刷新当前页面显示的 item
    this.item = MockData.getMedicineById(this.medicineId);
    try {
      promptAction.showToast({ message: '修改已保存' });
    } catch (e) {
    }
  }
}