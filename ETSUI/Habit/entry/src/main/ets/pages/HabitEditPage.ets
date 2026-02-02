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
import router from '@ohos.router';
import promptAction from '@ohos.promptAction';
import Habit from '../common/bean/Habit';
import { ThemeColorPicker } from '../view/component/ColorPicker';
import HabitRepository from '../model/repository/HabitRepository';
import Category from '../common/bean/Category';
import CategoryRepository from '../model/repository/CategoryRepository';
import { CategoryDialog } from '../view/component/CategoryDialog';
import { ReminderHelper } from '../model/ReminderHelper';

@Entry
@Component
struct HabitEditPage {
  // === 编辑模式状态 ===
  @State editHabitId: number = -1; // 如果大于0，说明是编辑模式
  @State oldReminderId: number = -1; // 记录旧的闹钟ID，用于取消
  // === 表单状态 ===
  @State habitName: string = '';
  @State targetCount: string = '1';
  @State selectedColor: string = '#FF5733';
  @State isSubmitting: boolean = false;
  // 分类状态
  @State categories: Category[] = [];
  @State selectedCategoryId: number = 2;
  // 提醒状态
  @State isReminderOn: boolean = false;
  @State reminderTime: Date = new Date('2023-01-01T08:00:00');
  dialogController: CustomDialogController = new CustomDialogController({
    builder: CategoryDialog({
      confirm: (text: string) => {
        this.addCategory(text);
      }
    }),
    alignment: DialogAlignment.Center,
    autoCancel: true
  });

  aboutToAppear() {
    this.refreshCategories();

    // 🕵️‍♂️ 检查是否有传过来的 ID (编辑模式)
    const params = router.getParams() as Record<string, Object>;
    if (params && params['habitId']) {
      this.editHabitId = params['habitId'] as number;
      this.loadHabitDetail(this.editHabitId);
    }
  }

  // 加载详情并回显
  async loadHabitDetail(id: number) {
    let habit = await HabitRepository.queryById(id);
    if (habit) {
      this.habitName = habit.name;
      this.targetCount = habit.targetCount.toString();
      this.selectedColor = habit.color;
      this.selectedCategoryId = habit.categoryId;
      this.oldReminderId = habit.reminderId; // 记住旧闹钟

      // 回显提醒时间
      if (habit.reminderTime && habit.reminderTime.length > 0) {
        this.isReminderOn = true;
        // 解析 "08:30"
        let parts = habit.reminderTime.split(':');
        if (parts.length === 2) {
          let d = new Date();
          d.setHours(parseInt(parts[0]), parseInt(parts[1]));
          this.reminderTime = d;
        }
      }
    }
  }

  refreshCategories() {
    CategoryRepository.queryAll().then(list => {
      this.categories = list.filter(c => c.id !== 1);
      if (this.categories.length > 0 && this.selectedCategoryId === 1 && this.editHabitId === -1) {
        this.selectedCategoryId = this.categories[0].id;
      }
    });
  }

  addCategory(name: string) {
    const randomColor = '#' + Math.floor(Math.random() * 16777215).toString(16);
    CategoryRepository.insert(name, randomColor).then((newId) => {
      this.refreshCategories();
      setTimeout(() => {
        this.selectedCategoryId = newId;
      }, 100);
    });
  }

  build() {
    Column() {
      // 1. 顶部导航栏
      Row() {
        Button({ type: ButtonType.Circle, stateEffect: true }) {
          Text('<').fontSize(26).fontWeight(FontWeight.Bold).fontColor('#333').margin({ bottom: 2 })
        }
        .width(40).height(40).backgroundColor('#F5F5F5')
        .onClick(() => {
          router.back();
        })

        // 动态标题：编辑还是新建？
        Text(this.editHabitId > 0 ? "编辑习惯" : "新建习惯")
          .fontSize(20).fontWeight(FontWeight.Bold).margin({ left: 10 })
      }
      .width('100%').padding(15).backgroundColor('#FFFFFF')
      .shadow({ radius: 2, color: '#0F000000', offsetY: 1 })

      // 2. 表单主体
      Scroll() {
        Column({ space: 24 }) {

          Column() {
            this.InputGroup("习惯名称", "例如：每天晨跑 3 公里", (val) => {
              this.habitName = val;
            })
          }
          .width('100%')
          .onClick(() => {
            // 这个空的点击事件是为了解决某些情况下的焦点问题 (Hack)
          })

          // 注意：InputGroup 需要在 build 下面定义，这里直接用 TextInput 也行，为了复用之前的 InputGroup Builder
          Column() {
            Text("每日目标次数").fontSize(14).fontColor('#666').width('100%').margin({ bottom: 8 })
            TextInput({ placeholder: "例如：1", text: this.targetCount }) // 绑定 text 实现回显
              .type(InputType.Number)
              .height(50)
              .backgroundColor(Color.White)
              .borderRadius(12)
              .padding({ left: 16, right: 16 })
              .border({ width: 1, color: '#E0E0E0' })
              .onChange((val) => {
                this.targetCount = val;
              })
          }

          // 分类选择器
          Column() {
            Text("所属分类").fontSize(14).fontColor('#666').width('100%').margin({ bottom: 8 })
            Flex({ wrap: FlexWrap.Wrap }) {
              ForEach(this.categories, (item: Category) => {
                Text(item.name)
                  .fontSize(14)
                  .fontColor(this.selectedCategoryId === item.id ? Color.White : '#666')
                  .backgroundColor(this.selectedCategoryId === item.id ? item.color : '#F0F0F0')
                  .padding({
                    left: 16,
                    right: 16,
                    top: 8,
                    bottom: 8
                  })
                  .borderRadius(20)
                  .margin({ right: 10, bottom: 10 })
                  .onClick(() => {
                    this.selectedCategoryId = item.id;
                  })
              }, (item: Category) => JSON.stringify(item))

              Text('+')
                .fontSize(16)
                .fontColor('#007DFF')
                .backgroundColor('#E6F2FF')
                .padding({
                  left: 20,
                  right: 20,
                  top: 8,
                  bottom: 8
                })
                .borderRadius(20)
                .margin({ bottom: 10 })
                .border({ width: 1, color: '#007DFF', style: BorderStyle.Dashed })
                .onClick(() => {
                  this.dialogController.open();
                })
            }
          }

          // 提醒设置
          Column() {
            Row() {
              Text("每日提醒").fontSize(14).fontColor('#666')
              Blank()
              Toggle({ type: ToggleType.Switch, isOn: this.isReminderOn })
                .onChange((isOn) => {
                  this.isReminderOn = isOn;
                })
            }
            .width('100%').margin({ bottom: this.isReminderOn ? 10 : 20 })

            if (this.isReminderOn) {
              Row() {
                Column() {
                  Text("提醒时间").fontSize(14).fontColor('#999').margin({ bottom: 8 })
                  Text(`${this.reminderTime.getHours().toString().padStart(2, '0')}点${this.reminderTime.getMinutes()
                    .toString()
                    .padStart(2, '0')}分`)
                    .fontSize(32).fontColor('#007DFF').fontWeight(FontWeight.Bold)
                }
                .alignItems(HorizontalAlign.Start).layoutWeight(1)

                Column() {
                  Row() {
                    Text('时').fontSize(12).fontColor('#999').textAlign(TextAlign.Center).layoutWeight(1)
                    Text('分').fontSize(12).fontColor('#999').textAlign(TextAlign.Center).layoutWeight(1)
                  }
                  .width(140)

                  TimePicker({ selected: this.reminderTime })
                    .useMilitaryTime(true).height(100).width(140)
                    .onChange((date: TimePickerResult) => {
                      let d = new Date();
                      d.setHours(date.hour, date.minute);
                      this.reminderTime = d;
                    })
                }
              }
              .width('100%')
              .padding(20)
              .backgroundColor(Color.White)
              .borderRadius(16)
              .margin({ bottom: 20 })
              .shadow({ radius: 4, color: '#1A000000', offsetY: 2 })
              .alignItems(VerticalAlign.Center)
            }
          }

          // 颜色选择器
          ThemeColorPicker({ selectedColor: $selectedColor })

          // 提交按钮
          Button(this.isSubmitting ? "正在保存..." : (this.editHabitId > 0 ? "保存修改" : "创建习惯"))
            .width('100%')
            .height(50)
            .fontSize(18)
            .fontWeight(FontWeight.Medium)
            .backgroundColor(this.selectedColor)
            .margin({ top: 30 })
            .enabled(!this.isSubmitting)
            .onClick(() => {
              this.submitForm();
            })

        }
        .padding(20).width('100%')
      }
      .layoutWeight(1).scrollBar(BarState.Off)
    }
    .height('100%').backgroundColor('#F1F3F5')
  }

  // 这里的 InputGroup 用于习惯名称的复用
  @Builder
  InputGroup(title: string, placeholder: string, onChange: (val: string) => void) {
    Column() {
      Text(title).fontSize(14).fontColor('#666666').width('100%').margin({ bottom: 8 })
      TextInput({ placeholder: placeholder, text: this.habitName }) // 绑定 text 回显
        .height(50)
        .fontSize(16)
        .backgroundColor(Color.White)
        .borderRadius(12)
        .padding({ left: 16, right: 16 })
        .border({ width: 1, color: '#E0E0E0' })
        .onChange((value: string) => {
          onChange(value);
        })
    }
  }

  async submitForm() {
    if (this.isSubmitting) {
      return;
    }
    if (this.habitName.trim().length === 0) {
      promptAction.showToast({ message: '请输入习惯名称' });
      return;
    }
    let count = parseInt(this.targetCount);
    if (isNaN(count) || count <= 0) {
      promptAction.showToast({ message: '目标次数必须大于 0' });
      return;
    }

    this.isSubmitting = true;

    // 1. 准备数据对象
    let habit = new Habit(this.habitName, this.selectedColor, count, this.selectedCategoryId);

    // 2. 处理提醒逻辑
    // 如果是编辑模式，且之前有闹钟，先把旧的关掉
    if (this.editHabitId > 0 && this.oldReminderId > 0) {
      await ReminderHelper.cancelReminder(this.oldReminderId);
      habit.reminderId = -1;
      habit.reminderTime = '';
    }

    if (this.isReminderOn) {
      let hour = this.reminderTime.getHours();
      let minute = this.reminderTime.getMinutes();
      habit.reminderTime = `${hour.toString().padStart(2, '0')}:${minute.toString().padStart(2, '0')}`;
      try {
        let newId = await ReminderHelper.publishReminder(this.habitName, hour, minute);
        habit.reminderId = newId;
      } catch (e) {
        console.error("注册提醒失败");
      }
    }

    try {
      if (this.editHabitId > 0) {
        // 🚨 编辑模式：调用 Update
        habit.id = this.editHabitId;
        await HabitRepository.update(habit);
        promptAction.showToast({ message: '修改成功' });
      } else {
        // ✨ 新建模式：调用 Insert
        await HabitRepository.insert(habit);
        promptAction.showToast({ message: '创建成功' });
      }
      router.back();
    } catch (err) {
      console.error("保存失败: " + JSON.stringify(err));
      this.isSubmitting = false;
    }
  }
}