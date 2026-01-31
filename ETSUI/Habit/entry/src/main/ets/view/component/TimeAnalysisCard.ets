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
/**
 * 打卡时段分析卡片 (TimeAnalysisCard)
 * ------------------------------------------------
 * 修改记录：
 * 1. 将 TimeBar 从 @Builder 改为独立的 @Component 组件，解决 UI 不刷新的 Bug。
 * 2. 增加 @Prop 和 @Watch 确保数据流向的绝对正确。
 */
@Component
export struct TimeAnalysisCard {
  // 1. 核心数据源，加上 @Watch 监听变化
  @Prop @Watch('analyze') timeStamps: number[] = [];
  // 2. 统计状态
  @State morningCount: number = 0;
  @State afternoonCount: number = 0;
  @State nightCount: number = 0;
  @State totalCount: number = 0; // 新增：用于传给子组件计算百分比
  // 3. 标签状态
  @State userTag: string = '计算中...';

  // 组件加载时先算一次
  aboutToAppear() {
    this.analyze();
  }

  // 核心分析逻辑 (改良版)
  analyze() {
    console.info(`[TimeCard] 开始分析，收到了 ${this.timeStamps.length} 条数据`);

    // 1. 更新总数
    this.totalCount = this.timeStamps.length;

    if (this.totalCount === 0) {
      this.userTag = '暂无数据';
      this.morningCount = 0;
      this.afternoonCount = 0;
      this.nightCount = 0;
      return;
    }

    // 2. 统计各时段
    let m = 0, a = 0, n = 0;
    this.timeStamps.forEach(ts => {
      const date = new Date(ts);
      const hour = date.getHours();
      if (hour >= 4 && hour < 12) {
        m++;
      } else if (hour >= 12 && hour < 18) {
        a++;
      } else {
        n++;
      }
    });

    // 3. 更新 UI 状态
    this.morningCount = m;
    this.afternoonCount = a;
    this.nightCount = n;

    // 4. 🆕 改良后的标签判定逻辑
    const max = Math.max(m, a, n);

    // 判断哪些时段并列第一
    const isM = (m === max);
    const isA = (a === max);
    const isN = (n === max);

    if (isM && isA && isN) {
      // 三者都一样
      this.userTag = '⚖️ 时间管理大师';
    } else if (isM && isA) {
      // 早晨 + 下午 并列
      this.userTag = '🏃 全天在线';
    } else if (isM && isN) {
      // 早晨 + 深夜 并列 (起得早睡得晚)
      this.userTag = '⚡ 究极卷王';
    } else if (isA && isN) {
      // 下午 + 深夜 并列
      this.userTag = '🌙 越夜越嗨';
    } else if (isM) {
      // 仅早晨最多
      this.userTag = '🌞 早起鸟';
    } else if (isA) {
      // 仅下午最多
      this.userTag = '☕ 下午茶达人';
    } else {
      // 仅深夜最多
      this.userTag = '🦉 夜猫子';
    }

    console.info(`[TimeCard] 判定标签为: ${this.userTag}`);
  }

  build() {
    Column() {
      // --- 顶部标题栏 ---
      Row() {
        Text('时段分布')
          .fontSize(16)
          .fontWeight(FontWeight.Bold)
          .fontColor('#333')

        Blank()

        // 标签
        if (this.totalCount > 0) {
          Text(this.userTag)
            .fontSize(10)
            .fontColor(Color.White)
            .backgroundColor('#FF9F0A')
            .padding({
              left: 8,
              right: 8,
              top: 2,
              bottom: 2
            })
            .borderRadius(10)
        }
      }
      .width('100%')
      .margin({ bottom: 15 })

      // --- 进度条列表 ---
      Column() {
        // 使用独立的子组件，通过 @Prop 传递数据，强制刷新
        TimeBarItem({
          label: '早晨',
          count: this.morningCount,
          total: this.totalCount,
          color: '#FFCC00'
        })

        TimeBarItem({
          label: '下午',
          count: this.afternoonCount,
          total: this.totalCount,
          color: '#FF9500'
        })

        TimeBarItem({
          label: '深夜',
          count: this.nightCount,
          total: this.totalCount,
          color: '#5856D6'
        })
      }
    }
    .width('100%')
    .padding(20)
    .backgroundColor(Color.White)
    .borderRadius(16)
    .shadow({
      radius: 5,
      color: '#05000000',
      offsetX: 2,
      offsetY: 2
    })
  }
}

/**
 * 🆕 提取出来的子组件：单条进度条
 * 使用 @Component 定义，确保接收到新的 @Prop 时必定刷新 UI
 */
@Component
struct TimeBarItem {
  @Prop label: string;
  @Prop count: number; // 接收具体的次数
  @Prop total: number; // 接收总数
  @Prop color: string;

  // 计算百分比的辅助方法
  getPercent(): number {
    if (this.total === 0) {
      return 0;
    }
    return (this.count / this.total) * 100;
  }

  build() {
    Row() {
      // 左侧标签
      Text(this.label)
        .fontSize(12)
        .fontColor('#666')
        .width(40)
        .fontWeight(FontWeight.Regular)

      // 中间进度条
      Stack({ alignContent: Alignment.Start }) {
        // 底槽
        Row()
          .width('100%')
          .height(8)
          .backgroundColor('#F1F3F5')
          .borderRadius(4)

        // 进度
        Row()
          .width(`${this.getPercent()}%`)
          .height(8)
          .backgroundColor(this.color)
          .borderRadius(4)
          .animation({ duration: 600, curve: Curve.FastOutSlowIn })
      }
      .layoutWeight(1)
      .margin({ left: 10, right: 10 })

      // 右侧具体次数
      Text(`${this.count}次`)
        .fontSize(12)
        .fontColor('#333')
        .width(35)
        .textAlign(TextAlign.End)
    }
    .width('100%')
    .margin({ bottom: 12 })
  }
}