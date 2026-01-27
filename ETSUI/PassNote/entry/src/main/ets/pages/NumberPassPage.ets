// 数字密码页（C+D组负责）
// pages/NumberPassPage.ets
import router from '@ohos.router';
import { PromptAction, UIContext } from '@kit.ArkUI';
import { PassType, PasswordManager, RouterUtil } from '../common/Config';
import { AlertDialog } from '@kit.ArkUI';
import promptAction from '@ohos.promptAction';
@Entry
@Component
export struct NumberPassPage {
  // 从路由参数获取认证类型
  private authType: PassType = PassType.NUMBER;

  @State password: string = ''; // 当前输入的密码
  @State isVerifying: boolean = false; // 是否正在验证
  @State showPassword: boolean = false; // 是否显示密码（明文）
  @State attempts: number = 0; // 尝试次数
  @State maxAttempts: number = 5; // 最大尝试次数
  uiContext: UIContext = this.getUIContext();
  promptAction: PromptAction = this.uiContext.getPromptAction();
  dialogControllerConfirm: CustomDialogController = new CustomDialogController({
    builder: AlertDialog({
      primaryTitle: '验证失败',
      content: `您已连续失败${this.maxAttempts}次，请稍后再试`,
      primaryButton: {
        value: '返回',
        fontColor:'#007DFF',
        action: () => {this.uiContext.getRouter().back();
        },
      }
    }),
  })
  dialogForgotPassword: CustomDialogController = new CustomDialogController({
    builder: AlertDialog({
      primaryTitle: '忘记密码？',
      content: '1. 请联系管理员重置密码\n2. 或使用其他认证方式登录',
      primaryButton: {
        value: '使用其他方式',
        fontColor:'#007DFF',
        action: () => {router.pushUrl({
          url: 'pages/PasswordEntry'
        });},
      }, secondaryButton: {
      value:'取消',
        fontColor:'#999',
        action: () => {router.pushUrl({
          url: 'pages/Index'
        });}
    }
    }),
  })


  aboutToAppear() {
    // 从路由参数获取认证类型
    const params = router.getParams() as Record<string, string>;
    if (params && params.authType) {
      this.authType = params.authType as PassType;
    }
  }

  // 数字按钮点击处理
  handleNumberClick(num: string) {
    if (this.password.length < 6) {
      this.password += num;

      // 输入满6位后自动验证
      if (this.password.length === 6) {
        this.verifyPassword();
      }
    }
  }

  // 删除最后一个字符
  handleDelete() {
    if (this.password.length > 0) {
      this.password = this.password.slice(0, -1);
    }
  }

  // 清空输入
  handleClear() {
    this.password = '';
  }

  // 切换密码显示/隐藏
  togglePasswordVisibility() {
    this.showPassword = !this.showPassword;
  }

  // 验证密码
  async verifyPassword() {
    if (this.password.length !== 6) {
      this.promptAction.openToast({
        message: '请输入6位数字密码',
        duration: 2000
      });
      return;
    }


    this.isVerifying = true;

    // 模拟网络延迟
    await new Promise<void>(resolve => setTimeout(resolve, 300));

    try {
      // 【修改点 1】：增加了 getContext(this) 参数
      // 原调用：PasswordManager.verifyPassword(this.authType, this.password)
      const isValid = await PasswordManager.verifyPassword(
        getContext(this),
        this.authType,
        this.password
      );

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
      // 注意：如果是 HarmonyOS API 9+，通常使用 promptAction.showToast
      promptAction.showToast({
        message: '验证出错，请重试',
        duration: 2000
      });
    } finally {
      this.isVerifying = false;
    }
  }

  // 验证成功处理
  async handleSuccess() {
    // 【修改点 2】：增加了 getContext(this) 参数
    // 原调用：PasswordManager.setLastUsedAuth(this.authType)
    await PasswordManager.setLastUsedAuth(getContext(this), this.authType);

    // 显示成功提示
    try {
      promptAction.showToast({
        message: '验证成功！',
        duration: 1000
      });
    } catch (error) {
      console.error('显示“验证成功”失败:', error);
    }

    // 跳转到主页
    await new Promise<void>(resolve => setTimeout(resolve, 300));
    RouterUtil.goToHome();
  }

  // 验证失败处理
  async handleFailure() {
    this.attempts++;

    if (this.attempts >= this.maxAttempts) {

      // 达到最大尝试次数
      this.dialogControllerConfirm.open();

      // 重置状态
      this.password = '';
      this.attempts = 0;
      return;
    }

    // 显示失败提示
    const remaining = this.maxAttempts - this.attempts;
    this.promptAction.openToast({
      message: `密码错误，还剩${remaining}次尝试`,
      duration: 2000
    });

    // 清空输入框，震动效果
    this.password = '';

    // 可以添加震动反馈（需要权限）
    // vibrator.vibrate({ duration: 100 });
  }

  // 返回上一页
  goBack() {
    this.uiContext.getRouter().back();
  }

  // 忘记密码
  handleForgotPassword() {
    this.dialogForgotPassword.open();
  }

  build() {
    Column({ space: 0 }) {
      // 顶部导航栏
      this.buildHeader()

      // 密码显示区域
      this.buildPasswordDisplay()

      // 数字键盘
      this.buildNumberPad()

      // 底部操作按钮
      this.buildBottomActions()
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F5F5F5')
  }

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
      .backgroundColor(Color.Transparent)
      .onClick(() => this.goBack())

      Text('数字密码验证')
        .fontSize(20)
        .fontWeight(FontWeight.Bold)
        .layoutWeight(1)
        .textAlign(TextAlign.Center)

      // 占位，保持对称
      Blank()
        .width(40)
    }
    .width('100%')
    .padding({ left: 15, right: 15, top: 10, bottom: 10 })
    .backgroundColor(Color.White)
    .shadow({
      radius: 2,
      color: '#E0E0E0',
      offsetX: 0,
      offsetY: 1
    })
  }

  // 构建密码显示区域
  @Builder
  buildPasswordDisplay() {
    Column({ space: 20 }) {
      // 标题
      Text('请输入6位数字密码')
        .fontSize(16)
        .fontColor('#666')
        .margin({ top: 30 })

      // 密码圆点显示
      Row({ space: 15 }) {
        ForEach(Array.from({ length: 6 }), (item: number, index: number) => {
          if (index < this.password.length) {
            // 已输入的数字
            Circle({ width: 20, height: 20 })
              .fill(this.isVerifying ? '#FFA500' : '#007DFF')
              .opacity(this.isVerifying ? 0.7 : 1)
          } else {
            // 未输入的空位
            Circle({ width: 20, height: 20 })
              .fill('#E0E0E0')
          }
        })
      }
      .margin({ top: 10, bottom: 10 })

      // 密码明文显示（可选）
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
        // 数字0
        this.buildNumberButton('0')

        // 删除按钮
        Button('⌫')
          .layoutWeight(1)
          .height(70)
          .backgroundColor(Color.White)
          .fontSize(24)
          .fontColor('#FF3B30')
          .borderRadius(10)
          .shadow({
            radius: 3,
            color: '#E0E0E0',
            offsetX: 0,
            offsetY: 2
          })
          .onClick(() => this.handleDelete())

        // 清空按钮
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

  // 构建单个数字按钮
  @Builder
  buildNumberButton(num: string) {
    Button(num)
      .layoutWeight(1)
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

  // 构建底部操作按钮
  @Builder
  buildBottomActions() {
    Column({ space: 15 }) {
      // 验证按钮
      Button(this.isVerifying ? '验证中...' : '验证')
        .width('90%')
        .height(50)
        .backgroundColor(this.password.length === 6 ? '#007DFF' : '#CCCCCC')
        .fontSize(18)
        .fontColor(Color.White)
        .borderRadius(25)
        .enabled(this.password.length === 6 && !this.isVerifying)
        .onClick(() => this.verifyPassword())

      // 忘记密码链接
      Button('忘记密码？')
        .width('90%')
        .height(40)
        .backgroundColor(Color.Transparent)
        .fontSize(14)
        .fontColor('#666')
        .onClick(() => this.handleForgotPassword())

      // 尝试次数提示
      if (this.attempts > 0) {
        Text(`已尝试 ${this.attempts}/${this.maxAttempts} 次`)
          .fontSize(12)
          .fontColor(this.attempts >= 3 ? '#FF3B30' : '#FF9500')
          .margin({ top: 10 })
      }
    }
    .width('100%')
    .padding(20)
    .margin({ top: 20 })
  }
}