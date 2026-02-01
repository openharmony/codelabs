/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-20
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
import router from '@ohos.router';
import { PlanService } from '../service/PlanService';
import { HistoryService } from '../service/HistoryService';
import { Plan, PlanStatus, DailyTask } from '../model/PlanModel';
import { common } from '@kit.AbilityKit';
import promptAction from '@ohos.promptAction';

@Entry
@Component
struct PlanDetailPage {
  @State plan: Plan | null = null;
  @State isEditMode: boolean = false;
  @State isLoaded: boolean = false;

  // 编辑模式下的表单数据
  @State editName: string = '';
  @State editDescription: string = '';
  @State editGoal: string = '';
  @State editStartDate: string = '';
  @State editEndDate: string = '';
  @State editTasks: DailyTask[] = [];

  private planId: string = '';
  private historyService: HistoryService = new HistoryService();
  private planService: PlanService = new PlanService(this.historyService);

  async aboutToAppear() {
    const params = router.getParams() as Record<string, Object>;
    this.planId = (params?.planId as string) || '';
    
    const context = getContext(this) as common.UIAbilityContext;
    await this.historyService.init(context);
    await this.planService.init(context);
    this.isLoaded = true;
    
    if (this.planId) {
      await this.loadPlan();
    } else {
      // 新建计划
      this.isEditMode = true;
      this.initNewPlan();
    }
  }

  onPageShow() {
    if (this.isLoaded && this.planId) {
      this.loadPlan();
    }
  }

  async loadPlan() {
    if (!this.planId) {
      return;
    }
    
    const loadedPlan = await this.planService.getPlanById(this.planId);
    if (loadedPlan) {
      this.plan = loadedPlan;
      this.editName = loadedPlan.name;
      this.editDescription = loadedPlan.description;
      this.editGoal = loadedPlan.goal;
      this.editStartDate = this.formatDate(loadedPlan.startDate);
      this.editEndDate = this.formatDate(loadedPlan.endDate);
      this.editTasks = [...loadedPlan.dailyTasks];
    }
  }

  initNewPlan() {
    this.plan = null;
    this.editName = '';
    this.editDescription = '';
    this.editGoal = '';
    const today = new Date();
    const nextWeek = new Date(today);
    nextWeek.setDate(today.getDate() + 7);
    this.editStartDate = this.formatDateInput(today);
    this.editEndDate = this.formatDateInput(nextWeek);
    const defaultTask = new DailyTask();
    defaultTask.distanceTarget = 1000; // 默认 1 公里
    this.editTasks = [defaultTask];
  }

  build() {
    Column() {
      this.TopNavBar()
      
      Scroll() {
        Column() {
          if (this.isEditMode) {
            this.EditForm()
          } else if (this.plan) {
            this.PlanDetailView()
          }
        }
        .padding(16)
      }
      .layoutWeight(1)
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F8F9FA')
  }

  @Builder
  TopNavBar() {
    Row() {
      Text('< 返回')
        .fontSize(16)
        .fontColor('#4ECDC4')
        .onClick(() => {
          router.back();
        })

      Blank()

      Text(this.isEditMode ? (this.planId ? '编辑计划' : '新建计划') : '计划详情')
        .fontSize(20)
        .fontWeight(FontWeight.Bold)
        .fontColor('#333333')

      Blank()

      if (!this.isEditMode && this.plan) {
        Text('编辑')
          .fontSize(16)
          .fontColor('#4ECDC4')
          .onClick(() => {
            this.isEditMode = true;
          })
      } else if (this.isEditMode) {
        Text('保存')
          .fontSize(16)
          .fontColor('#4ECDC4')
          .onClick(() => {
            this.savePlan();
          })
      }
    }
    .width('100%')
    .padding(16)
    .backgroundColor('#FFFFFF')
  }

  @Builder
  EditForm() {
    Column() {
      // 计划名称
      Column() {
        Text('计划名称')
          .fontSize(14)
          .fontColor('#666666')
          .width('100%')
          .margin({ bottom: 8 })
        TextInput({ placeholder: '请输入计划名称', text: this.editName })
          .width('100%')
          .height(44)
          .backgroundColor('#FFFFFF')
          .borderRadius(8)
          .padding(12)
          .onChange((value: string) => {
            this.editName = value;
          })
      }
      .width('100%')
      .margin({ bottom: 16 })

      // 计划描述
      Column() {
        Text('计划描述')
          .fontSize(14)
          .fontColor('#666666')
          .width('100%')
          .margin({ bottom: 8 })
        TextArea({ placeholder: '请输入计划描述', text: this.editDescription })
          .width('100%')
          .height(80)
          .backgroundColor('#FFFFFF')
          .borderRadius(8)
          .padding(12)
          .onChange((value: string) => {
            this.editDescription = value;
          })
      }
      .width('100%')
      .margin({ bottom: 16 })

      // 计划目标
      Column() {
        Text('计划目标')
          .fontSize(14)
          .fontColor('#666666')
          .width('100%')
          .margin({ bottom: 8 })
        TextInput({ placeholder: '例如：减重5kg、提升耐力等', text: this.editGoal })
          .width('100%')
          .height(44)
          .backgroundColor('#FFFFFF')
          .borderRadius(8)
          .padding(12)
          .onChange((value: string) => {
            this.editGoal = value;
          })
      }
      .width('100%')
      .margin({ bottom: 16 })

      // 开始日期
      Column() {
        Text('开始日期')
          .fontSize(14)
          .fontColor('#666666')
          .width('100%')
          .margin({ bottom: 8 })
        TextInput({ placeholder: 'YYYY-MM-DD', text: this.editStartDate })
          .width('100%')
          .height(44)
          .backgroundColor('#FFFFFF')
          .borderRadius(8)
          .padding(12)
          .onChange((value: string) => {
            this.editStartDate = value;
          })
      }
      .width('100%')
      .margin({ bottom: 16 })

      // 结束日期
      Column() {
        Text('结束日期')
          .fontSize(14)
          .fontColor('#666666')
          .width('100%')
          .margin({ bottom: 8 })
        TextInput({ placeholder: 'YYYY-MM-DD', text: this.editEndDate })
          .width('100%')
          .height(44)
          .backgroundColor('#FFFFFF')
          .borderRadius(8)
          .padding(12)
          .onChange((value: string) => {
            this.editEndDate = value;
          })
      }
      .width('100%')
      .margin({ bottom: 16 })

      // 每日任务
      Column() {
        Row() {
          Text('每日任务')
            .fontSize(16)
            .fontWeight(FontWeight.Bold)
            .fontColor('#333333')
          Blank()
          Text('+ 添加任务')
            .fontSize(14)
            .fontColor('#4ECDC4')
            .onClick(() => {
              const task = new DailyTask();
              task.distanceTarget = 1000; // 默认 1 公里
              this.editTasks.push(task);
            })
        }
        .width('100%')
        .margin({ bottom: 12 })

        ForEach(this.editTasks, (task: DailyTask, index: number) => {
          this.TaskEditor(task, index)
        }, (task: DailyTask, index: number) => `task_${index}`)
      }
      .width('100%')
      .padding(16)
      .backgroundColor('#FFFFFF')
      .borderRadius(12)
      .margin({ bottom: 16 })
    }
    .width('100%')
  }

  @Builder
  TaskEditor(task: DailyTask, index: number) {
    Column() {
      Row() {
        Text(`任务 ${index + 1}`)
          .fontSize(14)
          .fontWeight(FontWeight.Bold)
          .fontColor('#333333')
        Blank()
        Text('删除')
          .fontSize(12)
          .fontColor('#FF6B6B')
          .onClick(() => {
            this.editTasks.splice(index, 1);
          })
      }
      .width('100%')
      .margin({ bottom: 8 })

      // 各类型目标同时展示
      Column() {
        // 距离目标
        Row() {
          Text('距离目标(米):')
            .fontSize(12)
            .fontColor('#666666')
            .margin({ right: 8 })
          TextInput({
            placeholder: '例如 1000',
            text: task.distanceTarget > 0 ? task.distanceTarget.toString() : ''
          })
            .width('100%')
            .height(36)
            .backgroundColor('#F8F9FA')
            .borderRadius(6)
            .padding(8)
            .onChange((value: string) => {
              this.updateTaskDistanceTarget(index, parseFloat(value) || 0);
            })
        }
        .width('100%')
        .margin({ bottom: 8 })

        // 时长目标（单位：分钟）
        Row() {
          Text('时长目标(分钟):')
            .fontSize(12)
            .fontColor('#666666')
            .margin({ right: 8 })
          TextInput({
            placeholder: '例如 30（分钟，可不填）',
            text: task.durationTarget > 0 ? task.durationTarget.toString() : ''
          })
            .width('100%')
            .height(36)
            .backgroundColor('#F8F9FA')
            .borderRadius(6)
            .padding(8)
            .onChange((value: string) => {
              this.updateTaskDurationTarget(index, parseFloat(value) || 0);
            })
        }
        .width('100%')
        .margin({ bottom: 8 })

        // 卡路里目标
        Row() {
          Text('卡路里目标:')
            .fontSize(12)
            .fontColor('#666666')
            .margin({ right: 8 })
          TextInput({
            placeholder: '可不填',
            text: task.caloriesTarget > 0 ? task.caloriesTarget.toString() : ''
          })
            .width('100%')
            .height(36)
            .backgroundColor('#F8F9FA')
            .borderRadius(6)
            .padding(8)
            .onChange((value: string) => {
              this.updateTaskCaloriesTarget(index, parseFloat(value) || 0);
            })
        }
        .width('100%')
        .margin({ bottom: 8 })

        // 次数目标
        Row() {
          Text('次数目标:')
            .fontSize(12)
            .fontColor('#666666')
            .margin({ right: 8 })
          TextInput({
            placeholder: '可不填',
            text: task.recordsTarget > 0 ? task.recordsTarget.toString() : ''
          })
            .width('100%')
            .height(36)
            .backgroundColor('#F8F9FA')
            .borderRadius(6)
            .padding(8)
            .onChange((value: string) => {
              this.updateTaskRecordsTarget(index, parseFloat(value) || 0);
            })
        }
        .width('100%')
      }
      .width('100%')
    }
    .width('100%')
    .padding(12)
    .backgroundColor('#F8F9FA')
    .borderRadius(8)
    .margin({ bottom: 12 })
  }

  @Builder
  PlanDetailView() {
    if (this.plan) {
      Column() {
      // 计划基本信息
      Column() {
        Text(this.plan.name)
          .fontSize(24)
          .fontWeight(FontWeight.Bold)
          .fontColor('#333333')
          .width('100%')
          .margin({ bottom: 8 })

        if (this.plan.description) {
          Text(this.plan.description)
            .fontSize(14)
            .fontColor('#666666')
            .width('100%')
            .margin({ bottom: 8 })
        }

        Text(`目标: ${this.plan.goal}`)
          .fontSize(14)
          .fontColor('#4ECDC4')
          .width('100%')
          .margin({ bottom: 12 })

        Row() {
          Text(this.getStatusText(this.plan.status))
            .fontSize(12)
            .fontColor(this.getStatusColor(this.plan.status))
            .padding({ left: 8, right: 8, top: 4, bottom: 4 })
            .backgroundColor(this.getStatusBgColor(this.plan.status))
            .borderRadius(12)
        }
        .width('100%')
      }
      .width('100%')
      .padding(20)
      .backgroundColor('#FFFFFF')
      .borderRadius(16)
      .margin({ bottom: 16 })

      // 进度统计
      Column() {
        Text('进度统计')
          .fontSize(16)
          .fontWeight(FontWeight.Bold)
          .fontColor('#333333')
          .width('100%')
          .margin({ bottom: 16 })

        Row() {
          Column() {
            Text(`${this.plan.completedDays}`)
              .fontSize(32)
              .fontWeight(FontWeight.Bold)
              .fontColor('#4ECDC4')
              .margin({ bottom: 4 })
            Text('已完成天数')
              .fontSize(12)
              .fontColor('#999999')
          }
          .alignItems(HorizontalAlign.Center)
          .layoutWeight(1)

          Column() {
            Text(`${this.plan.totalDays}`)
              .fontSize(32)
              .fontWeight(FontWeight.Bold)
              .fontColor('#333333')
              .margin({ bottom: 4 })
            Text('总天数')
              .fontSize(12)
              .fontColor('#999999')
          }
          .alignItems(HorizontalAlign.Center)
          .layoutWeight(1)

          Column() {
            Text(`${this.plan.completionRate}%`)
              .fontSize(32)
              .fontWeight(FontWeight.Bold)
              .fontColor('#FFA502')
              .margin({ bottom: 4 })
            Text('完成度')
              .fontSize(12)
              .fontColor('#999999')
          }
          .alignItems(HorizontalAlign.Center)
          .layoutWeight(1)
        }
        .width('100%')
        .margin({ bottom: 16 })

        // 进度条
        Stack() {
          Progress({
            value: this.plan.completionRate,
            total: 100,
            type: ProgressType.Linear
          })
          .width('100%')
          .height(8)
          .color('#4ECDC4')
          .backgroundColor('#E0E0E0')
        }
        .width('100%')
        .height(8)

        Row() {
          Text(`开始: ${this.formatDate(this.plan.startDate)}`)
            .fontSize(12)
            .fontColor('#999999')
          Blank()
          Text(`结束: ${this.formatDate(this.plan.endDate)}`)
            .fontSize(12)
            .fontColor('#999999')
        }
        .width('100%')
        .margin({ top: 12 })
      }
      .width('100%')
      .padding(20)
      .backgroundColor('#FFFFFF')
      .borderRadius(16)
      .margin({ bottom: 16 })

      // 每日任务
      Column() {
        Text('每日任务')
          .fontSize(16)
          .fontWeight(FontWeight.Bold)
          .fontColor('#333333')
          .width('100%')
          .margin({ bottom: 16 })

        ForEach(this.plan.dailyTasks, (task: DailyTask, index: number) => {
          this.TaskView(task, index)
        }, (task: DailyTask, index: number) => `task_${index}`)
      }
      .width('100%')
      .padding(20)
      .backgroundColor('#FFFFFF')
      .borderRadius(16)
      .margin({ bottom: 16 })

      // 删除按钮
      if (this.plan.status !== PlanStatus.COMPLETED) {
        Button('删除计划')
          .width('100%')
          .height(44)
          .fontColor('#FFFFFF')
          .backgroundColor('#FF6B6B')
          .borderRadius(8)
          .onClick(() => {
            this.deletePlan();
          })
      }
    }
    .width('100%')
    }
  }

  @Builder
  TaskView(task: DailyTask, index: number) {
    Column() {
      Row() {
        Text(`任务 ${index + 1}`)
          .fontSize(14)
          .fontWeight(FontWeight.Bold)
          .fontColor('#333333')
        Blank()
        Text(task.isCompleted ? '✓ 已完成' : '未完成')
          .fontSize(12)
          .fontColor(task.isCompleted ? '#4CAF50' : '#999999')
      }
      .width('100%')
      .margin({ bottom: 8 })

      // 各类型目标与完成情况
      if (task.distanceTarget > 0) {
        Row() {
          Text(`距离目标: ${task.distanceTarget.toFixed(0)} m`)
            .fontSize(12)
            .fontColor('#666666')
          Blank()
          Text(`已完成: ${task.distanceCompleted.toFixed(0)} m`)
            .fontSize(12)
            .fontColor(task.distanceCompleted >= task.distanceTarget ? '#4CAF50' : '#999999')
        }
        .width('100%')
        .margin({ bottom: 4 })
      }

      if (task.durationTarget > 0) {
        Row() {
          Text(`时长目标: ${this.formatMinutesToHourMin(task.durationTarget)}`)
            .fontSize(12)
            .fontColor('#666666')
          Blank()
          Text(`已完成: ${this.formatDurationMsToHourMin(task.durationCompleted)}`)
            .fontSize(12)
            .fontColor(this.isDurationCompleted(task.durationTarget, task.durationCompleted) ? '#4CAF50' : '#999999')
        }
        .width('100%')
        .margin({ bottom: 4 })
      }

      if (task.caloriesTarget > 0) {
        Row() {
          Text(`卡路里目标: ${task.caloriesTarget.toFixed(0)} kcal`)
            .fontSize(12)
            .fontColor('#666666')
          Blank()
          Text(`已完成: ${task.caloriesCompleted.toFixed(0)} kcal`)
            .fontSize(12)
            .fontColor(task.caloriesCompleted >= task.caloriesTarget ? '#4CAF50' : '#999999')
        }
        .width('100%')
        .margin({ bottom: 4 })
      }

      if (task.recordsTarget > 0) {
        Row() {
          Text(`次数目标: ${task.recordsTarget.toFixed(0)} 次`)
            .fontSize(12)
            .fontColor('#666666')
          Blank()
          Text(`已完成: ${task.recordsCompleted.toFixed(0)} 次`)
            .fontSize(12)
            .fontColor(task.recordsCompleted >= task.recordsTarget ? '#4CAF50' : '#999999')
        }
        .width('100%')
        .margin({ bottom: 4 })
      }

      // 整体任务进度条：如果至少有一个目标，使用所有目标完成情况综合
      if (this.hasAnyTarget(task)) {
        Stack() {
          Progress({
            value: this.getTaskOverallProgress(task),
            total: 100,
            type: ProgressType.Linear
          })
          .width('100%')
          .height(4)
          .color(task.isCompleted ? '#4CAF50' : '#4ECDC4')
          .backgroundColor('#E0E0E0')
        }
        .width('100%')
        .height(4)
        .margin({ top: 8 })
      }
    }
    .width('100%')
    .padding(12)
    .backgroundColor('#F8F9FA')
    .borderRadius(8)
    .margin({ bottom: 12 })
  }

  async savePlan() {
    if (!this.editName.trim()) {
      promptAction.showToast({
        message: '请输入计划名称',
        duration: 2000
      });
      return;
    }

    if (!this.editStartDate || !this.editEndDate) {
      promptAction.showToast({
        message: '请选择开始和结束日期',
        duration: 2000
      });
      return;
    }

    const startDate = this.parseDate(this.editStartDate);
    const endDate = this.parseDate(this.editEndDate);

    if (!startDate || !endDate) {
      promptAction.showToast({
        message: '日期格式不正确，请使用 YYYY-MM-DD 格式',
        duration: 2000
      });
      return;
    }

    if (endDate <= startDate) {
      promptAction.showToast({
        message: '结束日期必须晚于开始日期',
        duration: 2000
      });
      return;
    }

    if (this.editTasks.length === 0) {
      promptAction.showToast({
        message: '请至少添加一个每日任务',
        duration: 2000
      });
      return;
    }

    // 校验每个每日任务：至少有一个类型目标 > 0
    for (let i = 0; i < this.editTasks.length; i++) {
      const task = this.editTasks[i];
      const hasAnyTarget = (task.distanceTarget > 0) ||
        (task.durationTarget > 0) ||
        (task.caloriesTarget > 0) ||
        (task.recordsTarget > 0);
      if (!hasAnyTarget) {
        promptAction.showToast({
          message: `第 ${i + 1} 个每日任务至少需要设置一个目标`,
          duration: 2000
        });
        return;
      }
    }

    try {
      let plan: Plan;
      
      if (this.planId && this.plan) {
        // 更新现有计划
        plan = this.plan;
        plan.name = this.editName;
        plan.description = this.editDescription;
        plan.goal = this.editGoal;
        plan.startDate = startDate.getTime();
        plan.endDate = endDate.getTime();
        plan.dailyTasks = this.editTasks;
        plan.totalDays = plan.calculateTotalDays();
        
        await this.planService.updatePlan(plan);
        promptAction.showToast({
          message: '计划已更新',
          duration: 2000
        });
      } else {
        // 创建新计划
        plan = new Plan(
          this.editName,
          this.editDescription,
          this.editGoal,
          startDate.getTime(),
          endDate.getTime(),
          this.editTasks
        );
        
        await this.planService.createPlan(plan);
        promptAction.showToast({
          message: '计划已创建',
          duration: 2000
        });
      }

      this.isEditMode = false;
      this.plan = plan;
      this.planId = plan.id;
      
      // 延迟返回，让用户看到成功提示
      setTimeout(() => {
        router.back();
      }, 500);
    } catch (err) {
      console.error('保存计划失败:', err);
      promptAction.showToast({
        message: '保存失败，请重试',
        duration: 2000
      });
    }
  }

  async deletePlan() {
    if (!this.planId) {
      return;
    }

    promptAction.showDialog({
      title: '确认删除',
      message: '确定要删除这个训练计划吗？',
      buttons: [
        {
          text: '取消',
          color: '#999999'
        },
        {
          text: '删除',
          color: '#FF6B6B'
        }
      ]
    }).then((result) => {
      if (result.index === 1) {
        // 点击了删除按钮
        this.planService.deletePlan(this.planId).then(() => {
          promptAction.showToast({
            message: '计划已删除',
            duration: 2000
          });
          router.back();
        }).catch((err: Error) => {
          console.error('删除计划失败:', err);
          promptAction.showToast({
            message: '删除失败，请重试',
            duration: 2000
          });
        });
      }
    });
  }

  // 更新任务目标值（多类型）
  updateTaskDistanceTarget(index: number, value: number): void {
    const newTasks = [...this.editTasks];
    const oldTask = newTasks[index];
    const task = new DailyTask();
    task.distanceTarget = value;
    task.durationTarget = oldTask.durationTarget;
    task.caloriesTarget = oldTask.caloriesTarget;
    task.recordsTarget = oldTask.recordsTarget;
    task.distanceCompleted = oldTask.distanceCompleted;
    task.durationCompleted = oldTask.durationCompleted;
    task.caloriesCompleted = oldTask.caloriesCompleted;
    task.recordsCompleted = oldTask.recordsCompleted;
    task.isCompleted = oldTask.isCompleted;
    newTasks[index] = task;
    this.editTasks = newTasks;
  }

  updateTaskDurationTarget(index: number, value: number): void {
    const newTasks = [...this.editTasks];
    const oldTask = newTasks[index];
    const task = new DailyTask();
    task.distanceTarget = oldTask.distanceTarget;
    task.durationTarget = value;
    task.caloriesTarget = oldTask.caloriesTarget;
    task.recordsTarget = oldTask.recordsTarget;
    task.distanceCompleted = oldTask.distanceCompleted;
    task.durationCompleted = oldTask.durationCompleted;
    task.caloriesCompleted = oldTask.caloriesCompleted;
    task.recordsCompleted = oldTask.recordsCompleted;
    task.isCompleted = oldTask.isCompleted;
    newTasks[index] = task;
    this.editTasks = newTasks;
  }

  updateTaskCaloriesTarget(index: number, value: number): void {
    const newTasks = [...this.editTasks];
    const oldTask = newTasks[index];
    const task = new DailyTask();
    task.distanceTarget = oldTask.distanceTarget;
    task.durationTarget = oldTask.durationTarget;
    task.caloriesTarget = value;
    task.recordsTarget = oldTask.recordsTarget;
    task.distanceCompleted = oldTask.distanceCompleted;
    task.durationCompleted = oldTask.durationCompleted;
    task.caloriesCompleted = oldTask.caloriesCompleted;
    task.recordsCompleted = oldTask.recordsCompleted;
    task.isCompleted = oldTask.isCompleted;
    newTasks[index] = task;
    this.editTasks = newTasks;
  }

  updateTaskRecordsTarget(index: number, value: number): void {
    const newTasks = [...this.editTasks];
    const oldTask = newTasks[index];
    const task = new DailyTask();
    task.distanceTarget = oldTask.distanceTarget;
    task.durationTarget = oldTask.durationTarget;
    task.caloriesTarget = oldTask.caloriesTarget;
    task.recordsTarget = value;
    task.distanceCompleted = oldTask.distanceCompleted;
    task.durationCompleted = oldTask.durationCompleted;
    task.caloriesCompleted = oldTask.caloriesCompleted;
    task.recordsCompleted = oldTask.recordsCompleted;
    task.isCompleted = oldTask.isCompleted;
    newTasks[index] = task;
    this.editTasks = newTasks;
  }

  // 是否有任意一个目标被设置
  hasAnyTarget(task: DailyTask): boolean {
    return task.distanceTarget > 0 ||
      task.durationTarget > 0 ||
      task.caloriesTarget > 0 ||
      task.recordsTarget > 0;
  }

  // 计算任务整体进度（0-100）
  getTaskOverallProgress(task: DailyTask): number {
    let totalTargets = 0;
    let completedTargets = 0;

    if (task.distanceTarget > 0) {
      totalTargets += 1;
      if (task.distanceCompleted >= task.distanceTarget) {
        completedTargets += 1;
      }
    }

    if (task.durationTarget > 0) {
      totalTargets += 1;
      if (this.isDurationCompleted(task.durationTarget, task.durationCompleted)) {
        completedTargets += 1;
      }
    }

    if (task.caloriesTarget > 0) {
      totalTargets += 1;
      if (task.caloriesCompleted >= task.caloriesTarget) {
        completedTargets += 1;
      }
    }

    if (task.recordsTarget > 0) {
      totalTargets += 1;
      if (task.recordsCompleted >= task.recordsTarget) {
        completedTargets += 1;
      }
    }

    if (totalTargets === 0) {
      return 0;
    }

    return Math.min(100, Math.round((completedTargets / totalTargets) * 100));
  }

  // 时长：将“分钟数”格式化为“X小时Y分”
  formatMinutesToHourMin(minutes: number): string {
    const totalMinutes = Math.max(0, Math.floor(minutes));
    const hours = Math.floor(totalMinutes / 60);
    const remainMinutes = totalMinutes % 60;
    if (hours <= 0) {
      return `${remainMinutes} 分钟`;
    }
    if (remainMinutes <= 0) {
      return `${hours} 小时`;
    }
    return `${hours} 小时 ${remainMinutes} 分钟`;
  }

  // 时长：将毫秒格式化为“X小时Y分”
  formatDurationMsToHourMin(durationMs: number): string {
    const totalMinutes = Math.max(0, Math.floor(durationMs / 60000));
    return this.formatMinutesToHourMin(totalMinutes);
  }

  // 时长是否达成（durationTarget 以分钟为单位，durationCompleted 以毫秒为单位）
  isDurationCompleted(targetMinutes: number, completedMs: number): boolean {
    if (targetMinutes <= 0) {
      return false;
    }
    const targetMs = targetMinutes * 60 * 1000;
    return completedMs >= targetMs;
  }

  formatDate(timestamp: number): string {
    const date = new Date(timestamp);
    const year = date.getFullYear();
    const month = String(date.getMonth() + 1).padStart(2, '0');
    const day = String(date.getDate()).padStart(2, '0');
    return `${year}-${month}-${day}`;
  }

  formatDateInput(date: Date): string {
    const year = date.getFullYear();
    const month = String(date.getMonth() + 1).padStart(2, '0');
    const day = String(date.getDate()).padStart(2, '0');
    return `${year}-${month}-${day}`;
  }

  parseDate(dateStr: string): Date | null {
    const parts = dateStr.split('-');
    if (parts.length !== 3) {
      return null;
    }
    const year = parseInt(parts[0]);
    const month = parseInt(parts[1]) - 1;
    const day = parseInt(parts[2]);
    const date = new Date(year, month, day);
    if (isNaN(date.getTime())) {
      return null;
    }
    return date;
  }

  getStatusText(status: PlanStatus): string {
    switch (status) {
      case PlanStatus.ACTIVE:
        return '进行中';
      case PlanStatus.COMPLETED:
        return '已完成';
      case PlanStatus.PAUSED:
        return '已暂停';
      case PlanStatus.CANCELLED:
        return '已取消';
      default:
        return '未知';
    }
  }

  getStatusColor(status: PlanStatus): string {
    switch (status) {
      case PlanStatus.ACTIVE:
        return '#4ECDC4';
      case PlanStatus.COMPLETED:
        return '#4CAF50';
      case PlanStatus.PAUSED:
        return '#FFA502';
      case PlanStatus.CANCELLED:
        return '#999999';
      default:
        return '#666666';
    }
  }

  getStatusBgColor(status: PlanStatus): string {
    switch (status) {
      case PlanStatus.ACTIVE:
        return '#4ECDC420';
      case PlanStatus.COMPLETED:
        return '#4CAF5020';
      case PlanStatus.PAUSED:
        return '#FFA50220';
      case PlanStatus.CANCELLED:
        return '#99999920';
      default:
        return '#66666620';
    }
  }
}
