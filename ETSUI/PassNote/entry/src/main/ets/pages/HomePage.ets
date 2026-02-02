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
 * ============================================================================================
 * 模块名称: 应用主页 (HomePage)
 * --------------------------------------------------------------------------------------------
 * 模块描述:
 * 本页面是用户通过身份验证（密码、指纹、人脸等）后进入的第一个核心功能页。
 * 它是应用的主仪表盘 (Dashboard)，承载了核心功能的入口。
 *
 * 主要功能:
 * 1. 欢迎反馈: 向用户展示登录成功的状态和欢迎语。
 * 2. 导航入口: 提供跳转至“个人中心”等子功能页面的入口。
 * 3. 会话管理: 提供“退出登录”功能，清理路由栈并返回登录页。
 * ============================================================================================
 */

// 导入页面路由模块
// 该模块用于实现应用内不同页面之间的跳转、返回以及路由栈的管理
import router from '@ohos.router';

/**
 * 装饰器: @Entry
 * 描述: 标记 HomePage 结构体为页面的入口组件。
 * 只有被 @Entry 装饰的组件才能作为独立的页面被 router.pushUrl 访问。
 */
@Entry
  /**
   * 装饰器: @Component
   * 描述: 标记 HomePage 为一个 ArkUI 自定义组件。
   * 这意味着它拥有生命周期方法（如 aboutToAppear）和 build() 构建函数。
   */
@Component
export struct HomePage {

  /**
   * 状态变量: welcomeMessage
   * 类型: string
   * 含义: 页面顶部显示的欢迎标语。
   * 特性: 被 @State 装饰，意味着如果修改此变量，关联的 UI 组件（Text）会自动重新渲染。
   */
  @State welcomeMessage: string = '欢迎回来！';

  /**
   * 方法名称: goToProfile
   * 功能描述: 跳转到个人中心页面。
   *
   * 逻辑:
   * 使用 router.pushUrl 方法将 'pages/ProfilePage' 压入页面栈顶。
   * 这种方式允许用户在个人中心页面通过点击“返回”按钮回到当前的主页。
   */
  // 跳转到个人中心
  goToProfile() {
    router.pushUrl({
      url: 'pages/ProfilePage'
    });
  }

  /**
   * 方法名称: logout
   * 功能描述: 执行退出登录操作。
   *
   * 逻辑:
   * 1. router.clear(): 这是一个关键操作。它会清空当前的页面路由栈。
   * 这样做的目的是防止用户退出后，还能通过物理返回键或手势滑回主页，确保安全性。
   * 2. router.pushUrl(...): 导航回 'pages/Index' (通常是登录/启动页)。
   */
  // 退出登录
  logout() {
    // 清空路由历史栈，防止回退
    router.clear();
    // 跳转回登录入口
    router.pushUrl({
      url: 'pages/Index'
    });
  }

  /**
   * 方法名称: build
   * 功能描述: 定义组件的 UI 结构。
   *
   * 布局结构:
   * 使用 Column 垂直布局容器，将欢迎语、状态文本、功能按钮依次从上到下排列。
   */
  build() {
    // 创建一个垂直方向的容器，子组件之间的间距设置为 20vp
    Column({ space: 20 }) {

      /**
       * 组件: Text (欢迎语)
       * 内容: 绑定 this.welcomeMessage 状态变量
       */
      Text(this.welcomeMessage)
        .fontSize(24)                    // 设置字体大小为 24fp
        .fontWeight(FontWeight.Bold)     // 设置字体粗细为加粗
        .margin({ top: 50 })             // 设置顶部外边距为 50vp，使其不紧贴屏幕顶部

      /**
       * 组件: Text (成功提示)
       * 内容: 固定文本 '身份认证成功！'
       */
      Text('身份认证成功！')
        .fontSize(18)                    // 设置字体大小为 18fp
        .fontColor('#4CAF50')            // 设置字体颜色为绿色 (#4CAF50)，传达成功/安全的语义
        .margin({ bottom: 50 })          // 设置底部外边距为 50vp，与下方按钮区隔开

      // 这里可以添加应用的具体功能
      // 例如：便签、设置等

      /**
       * 组件: Button (个人中心)
       * 交互: 点击触发 goToProfile 方法
       */
      Button('个人中心')
        .width('80%')                    // 设置按钮宽度为父容器宽度的 80%
        .height(50)                      // 设置按钮高度为 50vp
        .backgroundColor('#007DFF')      // 设置背景色为品牌蓝 (#007DFF)
        .fontColor(Color.White)          // 设置文字颜色为白色
        .onClick(() => this.goToProfile()) // 绑定点击事件处理函数

      /**
       * 组件: Button (退出登录)
       * 交互: 点击触发 logout 方法
       */
      Button('退出登录')
        .width('80%')                    // 设置按钮宽度为父容器宽度的 80%
        .height(50)                      // 设置按钮高度为 50vp
        .backgroundColor('#FF3B30')      // 设置背景色为警示红 (#FF3B30)，提示这是一个破坏性/退出操作
        .fontColor(Color.White)          // 设置文字颜色为白色
        .onClick(() => this.logout())    // 绑定点击事件处理函数
    }
    .width('100%')   // Column 容器宽度占满屏幕
    .height('100%')  // Column 容器高度占满屏幕
  }
}