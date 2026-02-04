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

@Entry
@Component
struct Index {
  // ======================
  // UI 状态
  // ======================
  @State statusText: string = 'Ready';
  @State currentRound: number = 0;
  @State totalValue: number = 0;
  @State logs: string[] = [];
  @State running: boolean = false;

  private totalWorkers: number = 10;
  private totalRounds: number = 10;

  // ======================
  // 状态颜色
  // ======================
  private getStatusColor(): string {
    switch (this.statusText) {
      case 'Ready':
        return '#888888';
      case 'Running':
        return '#FFC107';
      case 'Done':
        return '#4CAF50';
      case 'Error':
        return '#F44336';
      default:
        return '#000000';
    }
  }

  // ======================
  // UI 构建
  // ======================
  build() {
    Column() {
      Text('ArkTS 多任务轮次安全调度示例')
        .fontSize(22)
        .fontWeight(FontWeight.Bold)
        .margin({ bottom: 20 })

      Text(`状态：${this.statusText}`)
        .fontSize(16)
        .fontColor(this.getStatusColor())
        .margin({ bottom: 10 })

      Text(`当前轮次：${this.currentRound}`)
        .fontSize(16)
        .margin({ bottom: 5 })

      Text(`当前累计值：${this.totalValue}`)
        .fontSize(16)
        .margin({ bottom: 20 })

      Row() {
        Button(this.running ? '执行中...' : '开始执行')
          .enabled(!this.running)
          .width('45%')
          .height(44)
          .margin(5)
          .onClick(() => { this.startSchedule(); })

        Button('重置')
          .width('45%')
          .height(44)
          .margin(5)
          .onClick(() => { this.resetUI(); })
      }
      .margin({ bottom: 20 })

      Text('Logs:')
        .fontSize(18)
        .margin({ bottom: 5 })

      ForEach(this.logs, (log: string, index: number) => {
        Text(log)
          .fontSize(14)
          .padding({ bottom: 2 })
          .key(index.toString())
      })
    }
    .width('100%')
    .height('100%')
    .justifyContent(FlexAlign.Center)
    .alignItems(HorizontalAlign.Center)
    .padding(20)
  }

  // ======================
  // 执行调度
  // ======================
  private async startSchedule(): Promise<void> {
    if (this.running) return;

    this.running = true;
    this.statusText = 'Running';
    this.currentRound = 0;
    this.totalValue = 0;
    this.logs = [];

    try {
      for (let round = 0; round < this.totalRounds; round++) {
        this.currentRound = round + 1;
        const workerLogs: string[] = [];

        // 模拟并发执行，每个 worker 随机延迟
        const tasks: Promise<void>[] = [];
        for (let w = 0; w < this.totalWorkers; w++) {
          tasks.push(this.executeWorker(w, workerLogs));
        }

        // 等待本轮所有 worker 完成
        await Promise.all(tasks);

        // 输出本轮日志
        this.appendLog(`Round ${round + 1}: ${workerLogs.join(', ')}`);
      }

      this.statusText = 'Done';
      this.appendLog(`Scheduler finished, total: ${this.totalValue}`);
    } catch (err) {
      this.statusText = 'Error';
      this.appendLog(`Scheduler execution error: ${err}`);
    } finally {
      this.running = false;
    }
  }

  // ======================
  // 执行单个 worker
  // ======================
  private async executeWorker(workerId: number, workerLogs: string[]): Promise<void> {
    // 模拟随机延时
    await this.delay(Math.floor(Math.random() * 50 + 10));

    // 每个 worker 增量 1
    const increment = 1;
    this.totalValue += increment;

    workerLogs.push(`W${workerId}+${increment} -> total ${this.totalValue}`);
  }

  // ======================
  // 延时辅助函数
  // ======================
  private async delay(ms: number): Promise<void> {
    return new Promise<void>(resolve => setTimeout(resolve, ms));
  }

  // ======================
  // 日志输出
  // ======================
  private appendLog(msg: string): void {
    const timestamp = new Date().toLocaleTimeString();
    this.logs.push(`[${timestamp}] ${msg}`);

    // 保持最多50条日志
    if (this.logs.length > 50) this.logs.shift();
  }

  // ======================
  // 重置 UI
  // ======================
  private resetUI(): void {
    this.statusText = 'Ready';
    this.currentRound = 0;
    this.totalValue = 0;
    this.logs = [];
    this.running = false;
  }
}
