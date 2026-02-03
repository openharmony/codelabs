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
/**
 * Index.ets
 *
 * UI 页面：
 * - 点击按钮启动调度
 * - 展示执行状态
 * - 展示最终结果
 */

import { RoundThreadScheduler } from '../scheduler/RoundThreadScheduler';

@Entry
@Component
struct Index {
  // ======================
  // UI State
  // ======================
  @State statusText: string = '未开始';
  @State resultText: string = '--';
  @State running: boolean = false;

  private scheduler: RoundThreadScheduler | null = null;

  // ======================
  // UI 构建
  // ======================
  build() {
    Column() {
      // 标题
      Text('ArkTS 多任务轮次安全调度示例')
        .fontSize(22)
        .fontWeight(FontWeight.Bold)
        .margin({ bottom: 20 })

      // 状态展示
      Text(`状态：${this.statusText}`)
        .fontSize(16)
        .margin({ bottom: 10 })

      Text(`最终结果：${this.resultText}`)
        .fontSize(16)
        .margin({ bottom: 30 })

      // 开始按钮
      Button(this.running ? '执行中...' : '开始执行')
        .enabled(!this.running)
        .width('60%')
        .height(44)
        .onClick(async () => {
          await this.startSchedule();
        })
    }
    .width('100%')
    .height('100%')
    .justifyContent(FlexAlign.Center)
    .alignItems(HorizontalAlign.Center)
    .padding(20)
  }

  // ======================
  // 业务逻辑
  // ======================
  private async startSchedule(): Promise<void> {
    if (this.running) {
      return;
    }

    this.running = true;
    this.statusText = '执行中...';
    this.resultText = '--';

    // 10 个 worker，10 轮
    this.scheduler = new RoundThreadScheduler(10, 10);

    try {
      const result = await this.scheduler.start();
      this.resultText = result.toString();
      this.statusText = '执行完成';
    } catch (err) {
      this.statusText = '执行失败';
      this.resultText = 'error';
      console.error('调度执行异常:', err);
    } finally {
      this.running = false;
    }
  }
}
