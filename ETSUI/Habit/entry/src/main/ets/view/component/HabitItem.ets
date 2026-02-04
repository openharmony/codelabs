/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
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
import Habit from '../../common/bean/Habit';

import { promptAction } from '@kit.ArkUI';

@Component
export struct HabitItem {
  @ObjectLink habit: Habit;
  // 定义两个回调函数，给默认空函数防止报错

  clickDelete: (id: number) => void = (id: number) => {
  };
  onCheckIn: () => void = () => {
  };
  clickBackout: (id: number) => void = (id: number) => {
  };
  // 🆕 新增：点击卡片主体的回调 (去编辑)

  clickToEdit: () => void = () => {
  };

  build() {

    Row() {

      // ========= 👈 左侧区域：点击去编辑 =========

      Row() {

        // 1. 颜色条

        Column()

          .width(6)

          .height('100%')

          .backgroundColor(this.habit.isChecked ? '#E0E0E0' : this.habit.color)

          .borderRadius({ topLeft: 16, bottomLeft: 16 })


        // 2. 文字内容

        Column() {

          Row() {

            Text(this.habit.name)

              .fontSize(18)

              .fontWeight(FontWeight.Bold)

              .fontColor(this.habit.isChecked ? '#999' : '#333')

              .decoration({

                type: this.habit.isChecked ? TextDecorationType.LineThrough : TextDecorationType.None,

                color: '#999'

              })

              .maxLines(1)

              .textOverflow({ overflow: TextOverflow.Ellipsis })

              .layoutWeight(1)


            if (this.habit.reminderId > 0 && !this.habit.isChecked) {

              Text('⏰').fontSize(14).margin({ left: 5 })

            }

          }

          .width('100%')

          .margin({ bottom: 8 })


          Row() {

            Text(`今日已完成: ${this.habit.currentCount}/${this.habit.targetCount} 次`)

              .fontSize(12)

              .fontColor('#999')

              .backgroundColor('#F5F5F5')

              .padding({
                left: 8,
                right: 8,
                top: 2,
                bottom: 2
              })

              .borderRadius(4)


            Blank()


            if (this.habit.isChecked) {

              Text('已完成').fontSize(12).fontColor('#00CC66').fontWeight(FontWeight.Medium)

            }

          }

          .width('100%')

        }

        .layoutWeight(1) // 占满中间空间

        .padding({
          left: 15,
          right: 15,
          top: 15,
          bottom: 15
        })

      }

      .layoutWeight(1) // 让左侧区域占满剩余空间

      .height('100%')

      // 👇👇👇 关键：编辑事件加在这里 (左侧区域)

      .onClick(() => {

        this.clickToEdit();

      })


      // ========= 👉 右侧区域：点击打卡 =========

      Column() {

        Stack() {

          if (this.habit.isChecked) {

            // === A. 已完成状态 ===


            // 1. 绿色实心圆

            Circle({ width: 32, height: 32 })

              .fill('#00CC66')


            // 2. 矢量对勾 (修复写法)

            Polyline() // 👈 构造函数留空

              .points([[9, 16], [14, 21], [24, 11]]) // 👈 points 必须写在这里！

              .fill(Color.Transparent)

              .stroke(Color.White)

              .strokeWidth(3)

              .strokeLineCap(LineCapStyle.Round)

              .strokeLineJoin(LineJoinStyle.Round)

              .width(32)

              .height(32)


          } else {

            // === B. 未完成状态 ===

            Circle({ width: 32, height: 32 })

              .stroke(this.habit.color)

              .strokeWidth(4)

              .fill(Color.Transparent)

          }

        }

        // 动画曲线修复

        .animation({

          duration: 300,

          curve: Curve.FastOutSlowIn // 👈 改用标准曲线，防止报错

        })

      }

      .padding({
        right: 10,
        left: 10,
        top: 20,
        bottom: 20
      })

      .onClick(() => {

        this.onCheckIn();

      })

      Button({ type: ButtonType.Circle }) {

        Text('🗑️').fontSize(16)

      }
      .width(32)
      .height(32)

      .margin({
        right: 20,
        left: 10,
        top: 20,
        bottom: 20
      })

      .backgroundColor('#FFFFFF')

      .shadow({ radius: 4, color: '#22000000', offsetY: 2 })

      .onClick(() => {

        AlertDialog.show({

          title: '删除提醒',

          message: `确定要删除 "${this.habit.name}" 吗？\n删除后无法恢复，且会清空打卡记录。`,

          autoCancel: true,

          primaryButton: {
            value: '取消', action: () => {
            }
          },

          secondaryButton: { value: '确认删除', fontColor: Color.Red, action: () => this.clickDelete(this.habit.id) }

        });

      })

    }
    .width('100%')

    .height(80)

    .backgroundColor(Color.White)

    .borderRadius(16)

    .shadow({ radius: 8, color: '#0F000000', offsetY: 4 })

    .margin({ bottom: 2 })

    .stateStyles({

      pressed: {
        .scale({ x: 0.98, y: 0.98 }).opacity(0.8)
      },

      normal: {
        .scale({ x: 1, y: 1 }).opacity(1)
      }

    })

    .gesture(

      LongPressGesture({ repeat: false })

        .onAction(async () => {

          if (this.habit.currentCount > 0) {

            AlertDialog.show({

              title: '撤销提醒',

              message: `确定要撤销 "${this.habit.name}" 的上一次打卡吗？`,

              autoCancel: true,

              primaryButton: {
                value: '保留', action: () => {
                }
              },

              secondaryButton: { value: '撤销', fontColor: Color.Red, action: () => this.clickBackout(this.habit.id) }

            });

          } else {

            promptAction.showToast({ message: '今日没有打卡记录，无法撤销' });

          }

        })

    )

  }
}