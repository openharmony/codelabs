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
import { MockData } from '../model/MockData';

@Entry
@Component
struct ContactAddPage {
  @State medicineName: string = '';
  @State selectedCategory: string = '请选择分类';
  @State categoryOptions: SelectOption[] = []; // 动态分类选项
  @State selectedDate: Date = new Date();
  @State selectedDateStr: string = '点击选择日期';
  @State selectedUsage: string = '一日三次';
  @State location: string = '';
  @State isBeforeMeal: boolean = false;
  @State isAfterMeal: boolean = true;

  aboutToAppear() {
    // 页面加载时，把 MockData 里的分类转成下拉菜单需要的格式
    const categories = MockData.getCategories();
    this.categoryOptions = categories.map((c) => {
      return { value: c } as SelectOption;
    });
  }

  build() {
    Column() {
      // 头部
      Row() {
        Button('<').onClick(() => router.back()).backgroundColor(Color.Transparent).fontColor(Color.Black).fontSize(24)
        Text('添加药品').fontSize(20).fontWeight(FontWeight.Bold).margin({ left: 12 })
      }
      .width('100%').padding(16)

      Scroll() {
        Column() {
          // 药品名称
          Text('药品名称').fontSize(14).fontColor('#666').width('100%').margin({ bottom: 8 })
          TextInput({ placeholder: '例如：999感冒灵' })
            .height(50)
            .backgroundColor('#F5F6FA')
            .borderRadius(8)
            .onChange((val) => this.medicineName = val)
            .margin({ bottom: 20 })

          // 功能分类 (动态)
          Text('功能分类').fontSize(14).fontColor('#666').width('100%').margin({ bottom: 8 })
          Select(this.categoryOptions) // 使用动态选项
            .value(this.selectedCategory)
            .width('100%')
            .height(50)
            .backgroundColor('#F5F6FA')
            .borderRadius(8)
            .onSelect((index, value) => {
              this.selectedCategory = value;
            })
            .margin({ bottom: 20 })

          // 有效期
          Text('有效期至').fontSize(14).fontColor('#666').width('100%').margin({ bottom: 8 })
          Row() {
            Image($r('app.media.startIcon')).width(20).height(20).margin({ right: 10 })
            Text(this.selectedDateStr).fontSize(16).fontColor('#333')
          }
          .width('100%')
          .height(50)
          .backgroundColor('#F5F6FA')
          .borderRadius(8)
          .padding({ left: 12 })
          .onClick(() => {
            DatePickerDialog.show({
              start: new Date(),
              end: new Date("2035-12-31"),
              selected: this.selectedDate,
              onAccept: (value: DatePickerResult) => {
                // 修复点：添加默认值 (??) 防止 undefined 报错
                const year = value.year ?? new Date().getFullYear();
                const monthIndex = value.month ?? new Date().getMonth();
                const day = value.day ?? new Date().getDate();

                // 拼接日期字符串 YYYY-MM-DD (月份需要+1)
                const displayMonth = monthIndex + 1;
                this.selectedDateStr =
                  `${year}-${displayMonth < 10 ? '0' + displayMonth : displayMonth}-${day < 10 ? '0' + day : day}`;

                // 更新 Date 对象
                this.selectedDate.setFullYear(year, monthIndex, day);
              }
            })
          })
          .margin({ bottom: 20 })

          // 用法用量
          Text('用法用量').fontSize(14).fontColor('#666').width('100%').margin({ bottom: 8 })
          Select([{ value: '一日一次' }, { value: '一日两次' }, { value: '一日三次' }, { value: '疼痛时服' }])
            .value(this.selectedUsage)
            .width('100%')
            .height(50)
            .backgroundColor('#F5F6FA')
            .borderRadius(8)
            .onSelect((index, value) => {
              this.selectedUsage = value;
            })
            .margin({ bottom: 20 })

          Row() {
            Radio({ value: 'before', group: 'meal' }).checked(this.isBeforeMeal)
              .onChange((checked) => {
                this.isBeforeMeal = checked;
                this.isAfterMeal = !checked;
              })
            Text('饭前服用').margin({ right: 20 })

            Radio({ value: 'after', group: 'meal' }).checked(this.isAfterMeal)
              .onChange((checked) => {
                this.isAfterMeal = checked;
                this.isBeforeMeal = !checked;
              })
            Text('饭后服用')
          }
          .width('100%').margin({ bottom: 20 })

          // 存放位置
          Text('存放位置').fontSize(14).fontColor('#666').width('100%').margin({ bottom: 8 })
          TextInput({ placeholder: '例如：客厅电视柜' })
            .height(50)
            .backgroundColor('#F5F6FA')
            .borderRadius(8)
            .onChange((val) => this.location = val)
            .margin({ bottom: 40 })

          // 按钮
          Button('确认入库')
            .width('100%').height(50)
            .backgroundColor('#4A90E2')
            .onClick(() => {
              // 1. 校验
              if (this.medicineName === '') {
                try {
                  promptAction.showToast({ message: '请输入药品名称' });
                } catch (e) {
                }
                return;
              }
              if (this.selectedCategory === '请选择分类') {
                try {
                  promptAction.showToast({ message: '请选择分类' });
                } catch (e) {
                }
                return;
              }
              if (this.selectedDateStr === '点击选择日期') {
                try {
                  promptAction.showToast({ message: '请选择有效期' });
                } catch (e) {
                }
                return;
              }

              // 2. 存入 MockData
              MockData.addMedicine(
                this.medicineName,
                this.selectedCategory,
                this.selectedDateStr,
                this.selectedUsage,
                this.location
              );

              try {
                promptAction.showToast({ message: '入库成功！' });
              } catch (e) {
              }
              router.back(); // 返回上一页
            })
        }
        .padding(16)
      }
    }
    .height('100%')
    .backgroundColor(Color.White)
  }
}