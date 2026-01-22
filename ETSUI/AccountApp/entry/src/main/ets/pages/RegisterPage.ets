import router from '@ohos.router';
import promptAction from '@ohos.promptAction';
import common from '@ohos.app.ability.common';
import RdbHelper from '../database/RdbHelper';
import User from '../model/User';
import { BusinessError } from '@ohos.base'; // 引入错误类型

/**
 * RegisterPage.ets
 * 用户注册页面
 * 包含表单输入、合法性校验以及调用数据库进行存储的逻辑
 */
@Entry
@Component
struct RegisterPage {
  // --- 状态变量 ---
  @State usernameStr: string = '';
  @State passwordStr: string = '';
  @State confirmPasswordStr: string = '';
  @State phoneStr: string = '';

  // 数据库帮助类实例
  private rdbHelper: RdbHelper | null = null;

  /**
   * 页面生命周期：即将显示时调用
   */
  aboutToAppear() {
    try {
      // 获取上下文
      let context = getContext(this) as common.UIAbilityContext;
      this.rdbHelper = new RdbHelper(context);

      // 初始化数据库
      this.rdbHelper.initRdbStore((isSuccess: boolean) => {
        if (isSuccess) {
          console.info('[RegisterPage] 数据库初始化成功');
        } else {
          console.error('[RegisterPage] 数据库初始化失败');
          this.showToast('系统错误：数据库初始化失败');
        }
      });
    } catch (error) {
      console.error(`[RegisterPage] aboutToAppear error: ${JSON.stringify(error)}`);
    }
  }

  /**
   * 显示简单的提示信息 (Toast)
   * 【修复点 1】加上 try-catch 消除警告
   */
  showToast(msg: string) {
    try {
      promptAction.showToast({
        message: msg,
        duration: 2000,
        bottom: 600
      });
    } catch (error) {
      console.error(`[RegisterPage] showToast error: ${JSON.stringify(error)}`);
    }
  }

  /**
   * 处理注册按钮点击事件
   */
  handleRegister() {
    // 1. 校验用户名
    if (this.usernameStr === '' || this.usernameStr.length < 2) {
      this.showToast('请输入有效的用户名（至少2位）');
      return;
    }

    // 2. 校验手机号
    if (this.phoneStr.length !== 11) {
      this.showToast('请输入11位手机号码');
      return;
    }

    // 3. 校验密码
    if (this.passwordStr === '') {
      this.showToast('密码不能为空');
      return;
    }

    // 4. 校验确认密码
    if (this.passwordStr !== this.confirmPasswordStr) {
      this.showToast('两次输入的密码不一致');
      return;
    }

    // 5. 校验数据库是否就绪
    if (!this.rdbHelper) {
      this.showToast('应用初始化中，请稍后再试');
      return;
    }

    // 6. 检查用户名是否存在
    this.rdbHelper.queryUser(this.usernameStr).then((existUser) => {
      if (existUser) {
        this.showToast('该用户名已被注册，请更换');
      } else {
        this.performInsert();
      }
    }).catch((err: BusinessError) => {
      console.error(`Query user failed: ${err}`);
    });
  }

  /**
   * 执行实际的数据库插入操作
   */
  performInsert() {
    if (!this.rdbHelper) return;

    // 创建新用户对象
    let newUser = new User(this.usernameStr, this.passwordStr, this.phoneStr);

    // 调用数据库插入
    this.rdbHelper.insertUser(newUser).then((rowId) => {
      if (rowId > 0) {
        this.showToast('注册成功！请登录');

        // 【修复点 1】加上 try-catch 消除 router 的警告
        setTimeout(() => {
          try {
            router.back();
          } catch (err) {
            console.error(`[RegisterPage] router back failed: ${JSON.stringify(err)}`);
          }
        }, 1000);
      } else {
        this.showToast('注册失败，请重试');
      }
    }).catch((err: BusinessError) => {
      console.error(`Insert user failed: ${err}`);
    });
  }

  build() {
    Column() {
      // --- 页面标题 ---
      // 【修复点 2】颜色使用 Color.X 或 16进制数字 消除颜色警告
      Text('欢迎注册')
        .fontSize(30)
        .fontWeight(FontWeight.Bold)
        .fontColor('#333333') // 如果这里还报黄，可以改成 Color.Black 或 0x333333
        .margin({ top: 50, bottom: 20 })
        .alignSelf(ItemAlign.Start)
        .padding({ left: 20 })

      Text('创建一个新账号以使用完整功能')
        .fontSize(16)
        .fontColor(Color.Gray) // 改用系统枚举
        .margin({ bottom: 40 })
        .alignSelf(ItemAlign.Start)
        .padding({ left: 20 })

      // --- 用户名输入框 ---
      TextInput({ placeholder: '请输入用户名' })
        .width('90%')
        .height(50)
        .backgroundColor('#F5F5F5') // 浅灰色背景
        .borderRadius(10)
        .margin({ bottom: 15 })
        .padding({ left: 15 })
        .onChange((value: string) => {
          this.usernameStr = value;
        })

      // --- 手机号输入框 ---
      TextInput({ placeholder: '请输入手机号' })
        .width('90%')
        .height(50)
        .backgroundColor('#F5F5F5')
        .borderRadius(10)
        .margin({ bottom: 15 })
        .padding({ left: 15 })
        .type(InputType.PhoneNumber)
        .onChange((value: string) => {
          this.phoneStr = value;
        })

      // --- 密码输入框 ---
      TextInput({ placeholder: '请输入密码' })
        .width('90%')
        .height(50)
        .backgroundColor('#F5F5F5')
        .borderRadius(10)
        .margin({ bottom: 15 })
        .padding({ left: 15 })
        .type(InputType.Password)
        .onChange((value: string) => {
          this.passwordStr = value;
        })

      // --- 确认密码输入框 ---
      TextInput({ placeholder: '请再次确认密码' })
        .width('90%')
        .height(50)
        .backgroundColor('#F5F5F5')
        .borderRadius(10)
        .margin({ bottom: 30 })
        .padding({ left: 15 })
        .type(InputType.Password)
        .onChange((value: string) => {
          this.confirmPasswordStr = value;
        })

      // --- 注册按钮 ---
      Button('立即注册')
        .width('90%')
        .height(50)
        .backgroundColor(0x007DFF) // 改用十六进制数字，消除警告
        .borderRadius(25)
        .fontSize(18)
        .onClick(() => {
          this.handleRegister();
        })

      // --- 底部跳转链接 ---
      Row() {
        Text('已有账号？ ')
          .fontColor(Color.Gray) // 改用系统枚举
          .fontSize(14)

        Text('去登录')
          .fontColor(0x007DFF) // 改用十六进制数字
          .fontSize(14)
          .fontWeight(FontWeight.Medium)
          .onClick(() => {
            try {
              router.back();
            } catch (err) {
              console.error(`Router error: ${err}`);
            }
          })
      }
      .margin({ top: 20 })

    }
    .width('100%')
    .height('100%')
    .backgroundColor(Color.White) // 改用系统枚举
  }
}