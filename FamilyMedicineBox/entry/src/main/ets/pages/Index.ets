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

// 定义添加分类的弹窗
@CustomDialog
struct AddCategoryDialog {
  controller: CustomDialogController;
  @State newCategoryName: string = '';
  // 回调函数，通知父组件刷新
  confirm: () => void = () => {
  };

  build() {
    Column() {
      Text('添加新分类')
        .fontSize(20)
        .fontWeight(FontWeight.Bold)
        .margin({ top: 20, bottom: 20 })

      TextInput({ placeholder: '请输入分类名称 (如: 眼药水)' })
        .height(50)
        .width('90%')
        .backgroundColor('#F5F5F5')
        .borderRadius(10)
        .onChange((value: string) => {
          this.newCategoryName = value;
        })

      Row() {
        Button('取消')
          .onClick(() => {
            this.controller.close();
          })
          .backgroundColor('#F5F5F5')
          .fontColor('#666666')
          .width('40%')

        Button('确定')
          .onClick(() => {
            if (this.newCategoryName.trim() === '') {
              try {
                promptAction.showToast({ message: '分类名不能为空' });
              } catch (e) {
              }
              return;
            }
            // 调用 MockData 添加分类
            MockData.addCategory(this.newCategoryName);
            try {
              promptAction.showToast({ message: '添加成功: ' + this.newCategoryName });
            } catch (e) {
            }
            this.confirm(); // 刷新界面
            this.controller.close();
          })
          .backgroundColor('#4A90E2')
          .width('40%')
      }
      .justifyContent(FlexAlign.SpaceAround)
      .width('100%')
      .margin({ top: 30, bottom: 20 })
    }
    .backgroundColor(Color.White)
    .borderRadius(20)
    .width('80%')
  }
}

@Entry
@Component
struct Index {
  @State expiredCount: number = 0;
  @State totalCount: number = 0;
  @State categoryList: string[] = [];
  // 初始化弹窗控制器
  dialogController: CustomDialogController = new CustomDialogController({
    builder: AddCategoryDialog({
      confirm: () => {
        // 弹窗关闭后的回调：重新获取分类列表
        this.categoryList = MockData.getCategories();
      }
    }),
    autoCancel: true
  });

  // 页面显示时刷新数据 (关键点：解决添加后不更新的问题)
  onPageShow() {
    this.refreshData();
  }

  refreshData() {
    this.categoryList = MockData.getCategories();
    this.totalCount = MockData.getAllMedicines().length;
    this.expiredCount = MockData.getExpiredOrNearMedicines().length;
  }

  build() {
    Stack({ alignContent: Alignment.Bottom }) {
      Scroll() {
        Column() {
          this.SearchHeader()
          this.HeaderCard()
          this.CategorySection()
          Blank().height(120)
        }
        .width('100%')
        .backgroundColor('#F5F6FA')
      }
      .edgeEffect(EdgeEffect.Spring)

      this.BottomBar()
    }
    .width('100%')
    .height('100%')
  }

  @Builder
  SearchHeader() {
    Row() {
      Image($r('app.media.startIcon')).width(20).height(20).margin({ right: 10 })
      Text('搜药品 / 查功效...').fontSize(16).fontColor('#999999')
    }
    .width('90%')
    .height(40)
    .backgroundColor('#EFEFEF')
    .borderRadius(20)
    .padding({ left: 15 })
    .margin({ top: 10, bottom: 10 })
    .onClick(() => {
      router.pushUrl({ url: 'pages/AllMedicinesPage' });
    })
  }

  @Builder
  HeaderCard() {
    Column() {
      Row() {
        Column() {
          Text('家庭药箱 · 守护健康').fontSize(18).fontWeight(FontWeight.Bold).fontColor(Color.White)
          Text('定期清理过期药，安全常备').fontSize(14).fontColor('#E3E7FF').margin({ top: 4 })
        }
        .alignItems(HorizontalAlign.Start)

        Blank()
        Stack({ alignContent: Alignment.Center }) {
          Circle({ width: 36, height: 36 }).fill(Color.White)
          Text('+').fontSize(24).fontWeight(FontWeight.Bold).fontColor('#6A5CFF')
        }
        .onClick(() => {
          router.pushUrl({ url: 'pages/ContactAddPage' });
        })
      }
      .width('100%')

      Row() {
        Column() {
          Text(this.expiredCount.toString()).fontSize(28).fontWeight(FontWeight.Bold).fontColor('#FFD54F')
          Text('临期/过期 >').fontSize(12).fontColor('#EDE7FF').margin({ top: 5 })
        }
        .onClick(() => {
          router.pushUrl({ url: 'pages/ExpiryWarningPage' });
        })

        Divider().height(30).width(1).backgroundColor('#FFFFFF44').margin({ left: 30, right: 30 })

        Column() {
          Text(this.totalCount.toString()).fontSize(28).fontWeight(FontWeight.Bold).fontColor(Color.White)
          Text('库存总数 >').fontSize(12).fontColor('#EDE7FF').margin({ top: 5 })
        }
        .onClick(() => {
          router.pushUrl({ url: 'pages/AllMedicinesPage' });
        })
      }
      .margin({ top: 25 }).width('100%').justifyContent(FlexAlign.Center)
    }
    .padding(25)
    .width('92%')
    .height(180)
    .borderRadius(24)
    .linearGradient({ direction: GradientDirection.RightBottom, colors: [['#5B7FFF', 0.0], ['#6A5CFF', 1.0]] })
    .shadow({ radius: 10, color: '#6A5CFF44', offsetY: 10 })
    .margin({ top: 10, bottom: 20 })
  }

  @Builder
  CategorySection() {
    Column() {
      Row() {
        Text('按分类查看').fontSize(18).fontWeight(FontWeight.Bold)
        Blank()
        Row() {
          Text('+ ').fontSize(16).fontColor('#4A90E2').fontWeight(FontWeight.Bold)
          Text('添加分类').fontSize(14).fontColor('#4A90E2')
        }
        .padding({
          left: 10,
          right: 10,
          top: 5,
          bottom: 5
        })
        .backgroundColor('#EBF2FF').borderRadius(15)
        .onClick(() => {
          this.dialogController.open();
        })
      }
      .width('92%').margin({ bottom: 15 })

      // 动态分类列表 (使用 Flex 自动换行)
      Flex({ wrap: FlexWrap.Wrap, justifyContent: FlexAlign.SpaceBetween }) {
        ForEach(this.categoryList, (item: string) => {
          Column() {
            Stack({ alignContent: Alignment.Center }) {
              Circle({ width: 50, height: 50 }).fill(Color.White)
              // 根据名称简单分配图标，或者统一图标
              Text(this.getIconForCategory(item)).fontSize(28)
            }

            Text(item)
              .fontSize(15).fontWeight(FontWeight.Medium).fontColor('#333333').margin({ top: 10 })
          }
          .width('48%')
          .height(120)
          .backgroundColor(Color.White)
          .borderRadius(20)
          .margin({ bottom: 12 })
          .justifyContent(FlexAlign.Center)
          .alignItems(HorizontalAlign.Center)
          .onClick(() => {
            router.pushUrl({ url: 'pages/CategoryListPage', params: { category: item } });
          })
        })
      }
      .width('92%')
    }
  }

  getIconForCategory(name: string): string {
    if (name.includes('感冒')) {
      return '🌡️';
    }
    if (name.includes('消炎')) {
      return '💊';
    }
    if (name.includes('肠胃')) {
      return '🍵';
    }
    if (name.includes('外伤')) {
      return '🩹';
    }
    if (name.includes('慢性')) {
      return '❤️';
    }
    if (name.includes('维')) {
      return '🥗';
    }
    return '📦'; // 默认图标
  }

  @Builder
  BottomBar() {
    Stack({ alignContent: Alignment.Bottom }) {
      Row() {
        Column() {
          Image($r('app.media.startIcon')).width(24).height(24).fillColor('#F1C40F')
          Text('发现').fontSize(12).margin({ top: 4 }).fontColor('#666666')
        }
        .width('35%').height('100%').justifyContent(FlexAlign.Center)
        .onClick(() => {
          router.pushUrl({ url: 'pages/ProfilePage' });
        })

        Blank().width('30%')

        Column() {
          Image($r('app.media.startIcon')).width(24).height(24)
          Text('设置').fontSize(12).margin({ top: 4 }).fontColor('#666666')
        }
        .width('35%').height('100%').justifyContent(FlexAlign.Center)
        .onClick(() => {
          router.pushUrl({ url: 'pages/ChangePasswordPage' });
        })
      }
      .height(70)
      .width('100%')
      .backgroundColor(Color.White)
      .shadow({ radius: 20, color: '#00000010', offsetY: -5 })
      .borderRadius({ topLeft: 20, topRight: 20 })

      Button({ type: ButtonType.Circle, stateEffect: true }) {
        Text('+').fontSize(40).fontColor(Color.White).fontWeight(FontWeight.Lighter)
      }
      .width(70)
      .height(70)
      .backgroundColor('#4A90E2')
      .position({ x: '41%', y: -35 })
      .shadow({ radius: 10, color: '#4A90E266', offsetY: 5 })
      .onClick(() => {
        router.pushUrl({ url: 'pages/ContactAddPage' });
      })
    }
    .width('100%').height(100)
  }
}