import router from '@ohos.router';
import promptAction from '@ohos.promptAction';
import common from '@ohos.app.ability.common';
import RdbHelper from '../database/RdbHelper';
import User from '../model/User';
import { BusinessError } from '@ohos.base';
import PreferenceUtil from '../utils/PreferenceUtil'; // 1. 引入工具类

/**
 * LoginPage.ets
 * 用户登录页面
 * 【关键更新】增加了自动登录勾选和用户信息保存逻辑
 */
@Entry
@Component
struct LoginPage {
  // --- 状态变量 ---
  @State usernameStr: string = '';
  @State passwordStr: string = '';
  @State isRemember: boolean = false; // 这里的“记住密码”我们用来作为“自动登录”的开关

  // 数据库工具类
  private rdbHelper: RdbHelper | null = null;

  /**
   * 页面初始化
   */
  aboutToAppear() {
    try {
      let context = getContext(this) as common.UIAbilityContext;
      this.rdbHelper = new RdbHelper(context);

      // 初始化数据库
      this.rdbHelper.initRdbStore((isSuccess: boolean) => {
        if (!isSuccess) {
          this.showToast('数据库异常，请重启应用');
        }
      });

      // 2. 初始化偏好设置工具 (为了后续保存登录状态)
      PreferenceUtil.init(context);

    } catch (err) {
      console.error(`[LoginPage] init failed: ${JSON.stringify(err)}`);
    }
  }

  /**
   * 通用 Toast 提示
   */
  showToast(msg: string) {
    try {
      promptAction.showToast({
        message: msg,
        duration: 2000,
        bottom: 100
      });
    } catch (error) {
      console.error(`[LoginPage] showToast error: ${JSON.stringify(error)}`);
    }
  }

  /**
   * 处理登录逻辑
   */
  handleLogin() {
    if (this.usernameStr === '' || this.passwordStr === '') {
      this.showToast('请输入用户名和密码');
      return;
    }

    if (!this.rdbHelper) {
      this.showToast('系统初始化中...');
      return;
    }

    // 查询数据库
    this.rdbHelper.queryUser(this.usernameStr).then((user: User | null) => {
      if (user) {
        // 用户存在，校验密码
        if (user.getPassword() === this.passwordStr) {
          this.showToast('登录成功！欢迎回来');

          // ==========================================
          // 👇👇👇 【新增】保存自动登录信息 👇👇👇
          if (this.isRemember) {
            // 如果勾选了“记住密码/自动登录”，将用户信息保存到 Preferences
            PreferenceUtil.saveLoginInfo(user.getId(), user.getUsername());
          }
          // ==========================================

          // 登录成功，跳转到主页
          setTimeout(() => {
            try {
              router.replaceUrl({
                url: 'pages/Index',
                params: {
                  username: user.getUsername(), // 传参
                  userId: user.getId()
                }
              });
            } catch (err) {
              console.error(`[LoginPage] router error: ${JSON.stringify(err)}`);
            }
          }, 1000);

        } else {
          this.showToast('密码错误，请重试');
        }
      } else {
        this.showToast('用户不存在，请先注册');
      }
    }).catch((err: BusinessError) => {
      console.error(`Login failed: ${err}`);
      this.showToast('登录出错，请稍后重试');
    });
  }

  build() {
    Column() {
      // --- 标题区域 ---
      Text('Account Manager')
        .fontSize(24)
        .fontWeight(FontWeight.Bold)
        .fontColor(0x333333)
        .margin({ top: 80, bottom: 50 })

      // --- 输入区域 ---
      Column({ space: 15 }) {
        // 用户名
        TextInput({ placeholder: '请输入用户名' })
          .width('100%')
          .height(50)
          .backgroundColor(0xF5F5F5)
          .borderRadius(25)
          .padding({ left: 20 })
          .onChange((value: string) => {
            this.usernameStr = value;
          })

        // 密码
        TextInput({ placeholder: '请输入密码' })
          .width('100%')
          .height(50)
          .backgroundColor(0xF5F5F5)
          .borderRadius(25)
          .padding({ left: 20 })
          .type(InputType.Password)
          .onChange((value: string) => {
            this.passwordStr = value;
          })
      }
      .width('85%')

      // --- 辅助功能区 ---
      Row() {
        Row() {
          // 这个 Checkbox 现在控制是否“自动登录”
          Checkbox({ name: 'remember', group: 'login' })
            .select(this.isRemember)
            .selectedColor(0x007DFF)
            .onChange((value: boolean) => {
              this.isRemember = value;
            })
          // 为了更明确，你可以把文案改成“下次自动登录”
          Text('下次自动登录')
            .fontSize(14)
            .fontColor(Color.Gray)
            .onClick(() => {
              this.isRemember = !this.isRemember;
            })
        }

        Blank()

        Text('忘记密码？')
          .fontSize(14)
          .fontColor(0x007DFF)
          // 👇 必须有这个 onClick 跳转 👇
          .onClick(() => {
            try {
              router.pushUrl({ url: 'pages/ForgotPasswordPage' });
            } catch (e) {
              console.error(`Nav error: ${e}`);
            }
          })
      }
      .width('80%')
      .margin({ top: 15, bottom: 30 })

      // --- 登录按钮 ---
      Button('登 录')
        .width('85%')
        .height(50)
        .borderRadius(25)
        .backgroundColor(0x007DFF)
        .fontSize(18)
        .onClick(() => {
          this.handleLogin();
        })

      // --- 注册引导 ---
      Row() {
        Text('还没有账号？ ')
          .fontSize(14)
          .fontColor(Color.Gray)

        Text('立即注册')
          .fontSize(14)
          .fontColor(0x007DFF)
          .fontWeight(FontWeight.Medium)
          .onClick(() => {
            try {
              router.pushUrl({
                url: 'pages/RegisterPage'
              });
            } catch (err) {
              console.error(`Navigation failed: ${err}`);
            }
          })
      }
      .margin({ top: 20 })

    }
    .width('100%')
    .height('100%')
    .backgroundColor(Color.White)
  }
}