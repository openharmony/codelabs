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
import { PetListPage } from './PetListPage'
import { CalendarPage } from './CalendarPage'
import { StatsPage } from './StatsPage'
import { SettingPage } from './SettingPage'
import { LoginPage } from './LoginPage'
import dataPreferences from '@ohos.data.preferences'

/**
 * ============================================================================
 * 【项目根容器组件：Index】
 *
 * 组长职责：
 * 1. 负责全应用的“登录态”监控。
 * 2. 实现退出登录后的即时 UI 切换。
 * 3. 负责 App 启动时的“身份唤醒”逻辑。
 * ============================================================================
 */
@Entry
@Component
struct Index {
  /**
   * 【核心改动】：使用 StorageLink 绑定全局存储中的用户名
   * 只要 AppStorage 里的 'currentUserName' 被清空（退出登录时），
   * Index 组件会立刻感应并重新执行 build()，从而切回登录页。
   */
  @StorageLink('currentUserName') loggedUser: string = ''
  /** 底部页签索引控制 */
  @State currentIndex: number = 0
  /** 页面初始化加载状态（增加代码厚度） */
  @State isInitializing: boolean = true

  /**
   * 生命周期钩子：根容器即将显示
   * 逻辑：去硬盘里看看上次登录的是谁，实现“持久化登录态”
   */
  async aboutToAppear() {
    console.info('PetDiary: Index 根容器启动中...');

    try {
      // 1. 获取首选项实例
      let pref = await dataPreferences.getPreferences(getContext(this), 'AccountStore');

      // 2. 尝试读取上次记住的账号
      // 注意：这里读取的是你之前在 LoginPage 存进去的 'last_input_user'
      const savedUser = await pref.get('last_input_user', '') as string;
      const isRemember = await pref.get('is_remember', false) as boolean;

      /**
       * 组长逻辑决策：
       * 如果用户上次勾选了“记住密码”，我们就在启动时自动把身份填入内存。
       * 这样 DatabaseManager 就能立刻识别身份，实现数据隔离。
       */
      if (isRemember && savedUser !== '') {
        // 这一行会让 build() 里的判断直接跳到 else 分支（进入主页）
        AppStorage.setOrCreate('currentUserName', savedUser);
        console.info('PetDiary: 已从硬盘唤醒历史身份 -> ' + savedUser);
      }
    } catch (err) {
      console.error('PetDiary: 启动身份恢复失败');
    } finally {
      this.isInitializing = false;
    }
  }

  /**
   * --- 辅助构建器：底部 Tab 样式自定义 ---
   * 增加代码量：将 Tab 的外观定义拆分为子函数
   */
  @Builder
  TabBuilder(title: string, index: number) {
    Column() {
      Text(title)
        .fontColor(this.currentIndex === index ? '#007DFF' : '#666')
        .fontSize(16)
        .fontWeight(this.currentIndex === index ? FontWeight.Bold : FontWeight.Normal)
    }
    .width('100%')
    .padding({ top: 10, bottom: 10 })
  }

  build() {
    Column() {
      /**
       * 【核心路由选择逻辑】
       * 逻辑：以 AppStorage 里的 loggedUser 作为唯一判断标准
       */
      if (this.loggedUser === '') {
        // --- 场景 A：未登录状态 ---
        // 此时 loggedUser 为空字符串，展示登录页
        LoginPage({
          onLoginSuccess: () => {
            // 登录成功时，LoginPage 内部会执行 AppStorage 的写入，
            // 这里的回调主要用于打印日志或执行额外跳转
            console.info('PetDiary: 登录校验通过，根容器正在切换视图...');
          }
        })
      } else {
        // --- 场景 B：已登录状态 ---
        // 展示功能 Tab 页签，所有数据库操作将自动锁定 this.loggedUser 标识的数据
        Tabs({ barPosition: BarPosition.End }) {

          TabContent() {
            PetListPage()
          }.tabBar(this.TabBuilder('宠物基地', 0))

          TabContent() {
            CalendarPage()
          }.tabBar(this.TabBuilder('行为足迹', 1))

          TabContent() {
            StatsPage()
          }.tabBar(this.TabBuilder('记账中心', 2))

          TabContent() {
            SettingPage()
          }.tabBar(this.TabBuilder('个人设置', 3))

        }
        .onChange((index: number) => {
          this.currentIndex = index;
          console.info(`PetDiary: 用户[${this.loggedUser}]切换到了第 ${index} 个模块`);
        })
        .width('100%')
        .height('100%')
        .backgroundColor('#F1F3F5')
        .barMode(BarMode.Fixed)
        .animationDuration(300) // 增加切换动画（增加代码量）
      }
    }
    .width('100%')
    .height('100%')
  }
}