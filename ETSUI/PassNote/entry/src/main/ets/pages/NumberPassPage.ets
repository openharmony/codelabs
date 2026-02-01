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
 * 模块名称: 数字密码认证页面 (NumberPassPage)
 * --------------------------------------------------------------------------------------------
 * 模块描述:
 * 本页面实现了应用程序的数字 PIN 码认证功能。
 * 它是传统的身份验证方式，要求用户输入 6 位数字密码进行比对。
 *
 * 核心功能:
 * 1. 自定义数字键盘: 纯 ArkUI 实现的软键盘，包含数字键、删除键和清空键。
 * 2. 密码输入显示: 支持以密文（圆点）或明文方式显示输入内容。
 * 3. 自动验证: 当输入达到 6 位时自动触发验证流程。
 * 4. 安全机制: 实现连续失败 5 次后的暂时锁定机制，并提供“忘记密码”入口。
 * 5. 交互反馈: 提供 Toast 提示、震动（注释中预留）以及丰富的按压动效。
 *
 * 技术栈:
 * - ArkUI 声明式 UI (Column, Row, Button, Text, Circle)。
 * - 状态管理 (@State)。
 * - 异步流程控制 (Promise/Async/Await)。
 * - 弹窗控制器 (CustomDialogController)。
 * ============================================================================================
 */

// 导入页面路由模块
// 用于管理页面栈，实现返回 (back) 和跳转 (pushUrl) 操作
// pages/NumberPassPage.ets
import router from '@ohos.router';

// 导入 ArkUI 核心能力包
// PromptAction: 用于显示 Toast 轻提示
// UIContext: 用于获取当前 UI 实例的上下文环境
import { PromptAction, UIContext } from '@kit.ArkUI';

// 导入自定义配置模块
// PassType: 认证类型枚举 (NUMBER)
// PasswordManager: 负责密码的比对校验与持久化存储
// RouterUtil: 路由辅助工具类
import { PassType, PasswordManager, RouterUtil } from '../common/Config';

// 导入系统提供的警告对话框组件
// 用于构建模态弹窗 (Modal Dialog)
import { AlertDialog } from '@kit.ArkUI';

// 导入旧版提示模块 (兼容性保留)
// 在某些旧版本 SDK 中可能仍需使用此模块，建议优先使用 this.uiContext.getPromptAction()
import promptAction from '@ohos.promptAction';

/**
 * 组件名称: NumberPassPage
 * 描述: 数字密码认证入口页面
 */
// 装饰器: @Entry 标记此组件为页面入口，可被路由系统加载
@Entry
  // 装饰器: @Component 标记此结构体为 ArkUI 自定义组件
@Component
export struct NumberPassPage {
  // 属性: authType
  // 描述: 当前页面的认证类型，默认为数字密码 (NUMBER)。
  // 该值可能会在 aboutToAppear 中根据路由参数进行更新。
  // 从路由参数获取认证类型
  private authType: PassType = PassType.NUMBER;

  /**
   * 状态变量区 (State Variables)
   * 这里的变量变化会自动触发 UI 刷新。
   */

  // 状态变量: password
  // 描述: 存储当前用户输入的密码字符串。
  // 初始为空字符串，最大长度为 6。
  @State password: string = ''; // 当前输入的密码

  // 状态变量: isVerifying
  // 描述: 标记当前是否正在进行后台验证。
  // true: 显示加载状态（如黄色光标），禁用输入。
  // false: 允许输入。
  @State isVerifying: boolean = false; // 是否正在验证

  // 状态变量: showPassword
  // 描述: 控制密码显示模式。
  // true: 显示明文数字。
  // false: 显示圆点占位符。
  @State showPassword: boolean = false; // 是否显示密码（明文）

  // 状态变量: attempts
  // 描述: 记录当前会话中连续验证失败的次数。
  @State attempts: number = 0; // 尝试次数

  // 状态变量: maxAttempts
  // 描述: 允许的最大失败次数，超过此值将触发锁定弹窗。
  @State maxAttempts: number = 5; // 最大尝试次数

  /**
   * 工具实例初始化
   */
  // 获取 UI 上下文
  // getUIContext() 是 Component 内部方法，获取当前组件所属的 UIContext
  uiContext: UIContext = this.getUIContext();

  // 获取 PromptAction 实例
  // 用于显示 Toast 消息，替代全局的 promptAction
  promptAction: PromptAction = this.uiContext.getPromptAction();

  /**
   * 弹窗控制器: dialogControllerConfirm
   * 描述: 验证失败达到上限时的阻断弹窗。
   * 只有当 attempts >= maxAttempts 时触发。
   */
  dialogControllerConfirm: CustomDialogController = new CustomDialogController({
    // 使用系统预置的 AlertDialog 构建器
    builder: AlertDialog({
      primaryTitle: '验证失败',
      content: `您已连续失败${this.maxAttempts}次，请稍后再试`,
      primaryButton: {
        value: '返回',
        fontColor:'#007DFF',
        // 点击动作: 强制返回上一页
        action: () => {this.uiContext.getRouter().back();
        },
      }
    }),
  })

  /**
   * 弹窗控制器: dialogForgotPassword
   * 描述: “忘记密码”时的选项弹窗。
   * 提供重置指引或切换其他认证方式。
   */
  dialogForgotPassword: CustomDialogController = new CustomDialogController({
    builder: AlertDialog({
      primaryTitle: '忘记密码？',
      content: '1. 请联系管理员重置密码\n2. 或使用其他认证方式登录',
      // 主按钮: 切换到统一认证入口页
      primaryButton: {
        value: '使用其他方式',
        fontColor:'#007DFF',
        action: () => {router.pushUrl({
          url: 'pages/PasswordEntry'
        });},
      },
      // 次要按钮: 放弃操作，返回主页
      secondaryButton: {
        value:'取消',
        fontColor:'#999',
        action: () => {router.pushUrl({
          url: 'pages/Index'
        });}
      }
    }),
  })

  /**
   * 生命周期: aboutToAppear
   * 描述: 组件即将挂载显示时触发。
   * 职责: 解析路由参数，初始化 authType。
   */
  aboutToAppear() {
    // 从路由参数获取认证类型
    // router.getParams() 返回路由跳转时携带的参数对象
    const params = router.getParams() as Record<string, string>;
    // 参数校验，防止空指针
    if (params && params.authType) {
      this.authType = params.authType as PassType;
    }
  }

  /**
   * 事件处理: handleNumberClick
   * 描述: 处理数字键盘点击事件。
   * 逻辑:
   * 1. 检查长度是否未满 6 位。
   * 2. 追加数字到 password 字符串。
   * 3. 如果长度达到 6 位，自动触发验证。
   *
   * @param num - 点击的数字字符 ('0'-'9')
   */
  // 数字按钮点击处理
  handleNumberClick(num: string) {
    // 长度限制检查
    if (this.password.length < 6) {
      // 字符串拼接
      this.password += num;

      // 输入满6位后自动验证
      if (this.password.length === 6) {
        this.verifyPassword();
      }
    }
  }

  /**
   * 事件处理: handleDelete
   * 描述: 处理删除按钮点击事件。
   * 逻辑: 删除 password 字符串的最后一个字符。
   */
  // 删除最后一个字符
  handleDelete() {
    if (this.password.length > 0) {
      // slice(0, -1) 截取从开头到倒数第一个字符之前的子串
      this.password = this.password.slice(0, -1);
    }
  }

  /**
   * 事件处理: handleClear
   * 描述: 处理清空按钮点击事件。
   * 逻辑: 将 password 重置为空字符串。
   */
  // 清空输入
  handleClear() {
    this.password = '';
  }

  /**
   * 事件处理: togglePasswordVisibility
   * 描述: 切换密码的明文/密文显示状态。
   */
  // 切换密码显示/隐藏
  togglePasswordVisibility() {
    this.showPassword = !this.showPassword;
  }

  /**
   * 异步方法: verifyPassword
   * 描述: 执行密码验证核心逻辑。
   * 流程:
   * 1. 再次检查长度。
   * 2. 锁定 UI (isVerifying = true)。
   * 3. 模拟网络请求延迟。
   * 4. 调用 PasswordManager 进行比对。
   * 5. 处理结果。
   */
  // 验证密码
  async verifyPassword() {
    // 步骤 1: 长度校验
    if (this.password.length !== 6) {
      this.promptAction.openToast({
        message: '请输入6位数字密码',
        duration: 2000
      });
      return;
    }

    // 步骤 2: 开启加载状态
    this.isVerifying = true;

    // 步骤 3: 模拟网络延迟 (300ms)
    // 提升用户体验，避免验证过程过快导致闪烁
    await new Promise<void>(resolve => setTimeout(resolve, 300));

    try {
      // 步骤 4: 调用验证接口
      // 【修改点 1】：增加了 getContext(this) 参数
      // getContext(this) 获取当前 Ability 或 UIAbility 的 Context，用于访问首选项等系统资源
      // 原调用：PasswordManager.verifyPassword(this.authType, this.password)
      const isValid = await PasswordManager.verifyPassword(
        getContext(this),
        this.authType,
        this.password
      );

      // 步骤 5: 结果分发
      if (isValid) {
        // 验证成功
        console.log('验证成功');
        await this.handleSuccess();
      } else {
        // 验证失败
        console.log('验证失败');
        await this.handleFailure();
      }
    } catch (error) {
      // 异常处理
      // 注意：如果是 HarmonyOS API 9+，通常使用 promptAction.showToast
      promptAction.showToast({
        message: '验证出错，请重试',
        duration: 2000
      });
    } finally {
      // 步骤 6: 无论成功失败，解除加载状态
      this.isVerifying = false;
    }
  }

  /**
   * 异步方法: handleSuccess
   * 描述: 验证成功后的后续处理。
   * 职责:
   * 1. 持久化记录最后使用的认证方式。
   * 2. 弹出成功提示。
   * 3. 跳转至主页。
   */
  // 验证成功处理
  async handleSuccess() {
    // 步骤 1: 保存偏好设置
    // 【修改点 2】：增加了 getContext(this) 参数
    // 原调用：PasswordManager.setLastUsedAuth(this.authType)
    await PasswordManager.setLastUsedAuth(getContext(this), this.authType);

    // 步骤 2: 显示成功提示
    try {
      promptAction.showToast({
        message: '验证成功！',
        duration: 1000
      });
    } catch (error) {
      console.error('显示“验证成功”失败:', error);
    }

    // 步骤 3: 延迟跳转
    // 延迟 300ms 确保用户看到成功提示
    // 跳转到主页
    await new Promise<void>(resolve => setTimeout(resolve, 300));
    RouterUtil.goToHome();
  }

  /**
   * 异步方法: handleFailure
   * 描述: 验证失败后的后续处理。
   * 职责:
   * 1. 增加失败计数。
   * 2. 检查锁定条件。
   * 3. 提示剩余次数。
   * 4. 清空输入框。
   */
  // 验证失败处理
  async handleFailure() {
    // 步骤 1: 计数累加
    this.attempts++;

    // 步骤 2: 检查锁定
    if (this.attempts >= this.maxAttempts) {
      // 达到最大尝试次数
      // 弹出模态对话框，强制用户操作
      this.dialogControllerConfirm.open();

      // 重置状态
      this.password = '';
      this.attempts = 0;
      return;
    }

    // 步骤 3: 提示剩余次数
    const remaining = this.maxAttempts - this.attempts;
    this.promptAction.openToast({
      message: `密码错误，还剩${remaining}次尝试`,
      duration: 2000
    });

    // 步骤 4: 重置输入
    // 清空输入框，震动效果
    this.password = '';

    // 可以添加震动反馈（需要权限）
    // vibrator.vibrate({ duration: 100 });
  }

  /**
   * 方法: goBack
   * 描述: 返回上一页。
   */
  // 返回上一页
  goBack() {
    this.uiContext.getRouter().back();
  }

  /**
   * 方法: handleForgotPassword
   * 描述: 打开忘记密码弹窗。
   */
  // 忘记密码
  handleForgotPassword() {
    this.dialogForgotPassword.open();
  }

  /**
   * UI 构建方法: build
   * 描述: 定义页面的 UI 结构。
   * 布局结构: Header -> PasswordDisplay -> NumberPad -> BottomActions
   */
  build() {
    // 垂直容器，无间距，占满全屏
    Column({ space: 0 }) {
      // 1. 顶部导航栏
      this.buildHeader()

      // 2. 密码显示区域 (圆点/明文)
      this.buildPasswordDisplay()

      // 3. 数字键盘区域
      this.buildNumberPad()

      // 4. 底部操作按钮区域
      this.buildBottomActions()
    }
    .width('100%')
    .height('100%')
    // 浅灰色背景
    .backgroundColor('#F5F5F5')
  }

  /**
   * 局部构建器: buildHeader
   * 描述: 顶部导航栏 UI。
   */
  // 构建顶部导航栏
  @Builder
  buildHeader() {
    Row({ space: 10 }) {
      // 返回按钮
      Button() {
        Text('←') // 左箭头符号
          .fontSize(20)
          .fontWeight(FontWeight.Bold)
      }
      .width(40)
      .height(40)
      .backgroundColor(Color.Transparent) // 透明背景
      .onClick(() => this.goBack())

      // 页面标题
      Text('数字密码验证')
        .fontSize(20)
        .fontWeight(FontWeight.Bold)
        .layoutWeight(1) // 占据剩余空间
        .textAlign(TextAlign.Center)

      // 占位符 (Blank)
      // 用于保持标题居中（左侧有返回按钮，右侧放置等宽占位符）
      // 占位，保持对称
      Blank()
        .width(40)
    }
    .width('100%')
    .padding({ left: 15, right: 15, top: 10, bottom: 10 })
    .backgroundColor(Color.White)
    // 底部阴影
    .shadow({
      radius: 2,
      color: '#E0E0E0',
      offsetX: 0,
      offsetY: 1
    })
  }

  /**
   * 局部构建器: buildPasswordDisplay
   * 描述: 密码输入状态展示区。
   * 包含: 提示文案、密码圆点/明文、显隐切换按钮。
   */
  // 构建密码显示区域
  @Builder
  buildPasswordDisplay() {
    Column({ space: 20 }) {
      // 提示标题
      Text('请输入6位数字密码')
        .fontSize(16)
        .fontColor('#666')
        .margin({ top: 30 })

      // 密码圆点指示器 (使用 Row 横向排列)
      Row({ space: 15 }) {
        // 循环生成 6 个圆点
        ForEach(Array.from({ length: 6 }), (item: number, index: number) => {
          if (index < this.password.length) {
            // 状态: 已输入的数字 (实心圆)
            Circle({ width: 20, height: 20 })
            // 验证中显示橙色，正常显示蓝色
              .fill(this.isVerifying ? '#FFA500' : '#007DFF')
              .opacity(this.isVerifying ? 0.7 : 1)
          } else {
            // 状态: 未输入的空位 (灰色圆)
            // 未输入的空位
            Circle({ width: 20, height: 20 })
              .fill('#E0E0E0')
          }
        })
      }
      .margin({ top: 10, bottom: 10 })

      // 密码明文显示（可选功能）
      // 仅在 showPassword 为 true 且有输入内容时显示
      if (this.showPassword && this.password) {
        Text(this.password)
          .fontSize(24)
          .fontWeight(FontWeight.Bold)
          .fontColor('#333')
          .margin({ top: 10 })
      }

      // 显示/隐藏切换按钮
      Row({ space: 5 }) {
        if (this.password.length > 0) {
          Button(this.showPassword ? '隐藏' : '显示')
            .fontSize(12)
            .width(60)
            .height(25)
            .backgroundColor('#E8F4FF')
            .fontColor('#007DFF')
            .onClick(() => this.togglePasswordVisibility())
        }
      }
    }
    .width('100%')
    .padding(20)
    .backgroundColor(Color.White)
    .margin({ top: 10, bottom: 10 })
  }

  /**
   * 局部构建器: buildNumberPad
   * 描述: 自定义数字键盘区域。
   * 布局: 3x4 网格 (3行数字 + 1行操作键)。
   */
  // 构建数字键盘
  @Builder
  buildNumberPad() {
    Column({ space: 15 }) {
      // 第一行：1 2 3
      this.buildNumberRow(['1', '2', '3'])

      // 第二行：4 5 6
      this.buildNumberRow(['4', '5', '6'])

      // 第三行：7 8 9
      this.buildNumberRow(['7', '8', '9'])

      // 第四行：0 删除 清空
      Row({ space: 15 }) {
        // 数字0 (左侧占位)
        this.buildNumberButton('0')

        // 删除按钮 (Backspace)
        Button('⌫')
          .layoutWeight(1) // 等比分配宽度
          .height(70)
          .backgroundColor(Color.White)
          .fontSize(24)
          .fontColor('#FF3B30') // 红色图标
          .borderRadius(10)
          .shadow({
            radius: 3,
            color: '#E0E0E0',
            offsetX: 0,
            offsetY: 2
          })
          .onClick(() => this.handleDelete())

        // 清空按钮 (Clear)
        Button('清空')
          .layoutWeight(1)
          .height(70)
          .backgroundColor(Color.White)
          .fontSize(18)
          .fontColor('#666')
          .borderRadius(10)
          .shadow({
            radius: 3,
            color: '#E0E0E0',
            offsetX: 0,
            offsetY: 2
          })
          .onClick(() => this.handleClear())
      }
      .width('100%')
    }
    .width('90%')
    .padding(20)
    .backgroundColor(Color.White)
    .borderRadius(20)
    .shadow({
      radius: 10,
      color: '#00000020',
      offsetX: 0,
      offsetY: 5
    })
    .margin({ left: 20, right: 20 })
  }

  /**
   * 局部构建器: buildNumberRow
   * 描述: 构建键盘的一行数字。
   * @param numbers - 数字字符串数组 (如 ['1', '2', '3'])
   */
  // 构建数字按钮行
  @Builder
  buildNumberRow(numbers: string[]) {
    Row({ space: 15 }) {
      ForEach(numbers, (num: string) => {
        this.buildNumberButton(num)
      })
    }
    .width('100%')
  }

  /**
   * 局部构建器: buildNumberButton
   * 描述: 构建单个数字按钮。
   * @param num - 显示的数字
   */
  // 构建单个数字按钮
  @Builder
  buildNumberButton(num: string) {
    Button(num)
      .layoutWeight(1) // 均分行宽度
      .height(70)
      .backgroundColor(Color.White)
      .fontSize(28)
      .fontWeight(FontWeight.Bold)
      .fontColor('#333')
      .borderRadius(10)
      .shadow({
        radius: 3,
        color: '#E0E0E0',
        offsetX: 0,
        offsetY: 2
      })
      .onClick(() => this.handleNumberClick(num))
  }

  /**
   * 局部构建器: buildBottomActions
   * 描述: 底部额外操作区。
   * 包含: 手动验证按钮、忘记密码、错误提示。
   */
  // 构建底部操作按钮
  @Builder
  buildBottomActions() {
    Column({ space: 15 }) {
      // 验证按钮 (通常满6位自动验证，此按钮为辅助)
      Button(this.isVerifying ? '验证中...' : '验证')
        .width('90%')
        .height(50)
        // 只有满6位且未验证时才激活为蓝色
        .backgroundColor(this.password.length === 6 ? '#007DFF' : '#CCCCCC')
        .fontSize(18)
        .fontColor(Color.White)
        .borderRadius(25)
        .enabled(this.password.length === 6 && !this.isVerifying)
        .onClick(() => this.verifyPassword())

      // 忘记密码链接 (透明按钮)
      Button('忘记密码？')
        .width('90%')
        .height(40)
        .backgroundColor(Color.Transparent)
        .fontSize(14)
        .fontColor('#666')
        .onClick(() => this.handleForgotPassword())

      // 错误次数提示
      // 仅在有错误记录时显示
      if (this.attempts > 0) {
        Text(`已尝试 ${this.attempts}/${this.maxAttempts} 次`)
          .fontSize(12)
          // 超过3次显示红色警示
          .fontColor(this.attempts >= 3 ? '#FF3B30' : '#FF9500')
          .margin({ top: 10 })
      }
    }
    .width('100%')
    .padding(20)
    .margin({ top: 20 })
  }
}