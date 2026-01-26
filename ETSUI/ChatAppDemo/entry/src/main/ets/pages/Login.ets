import { router, Prompt } from '@kit.ArkUI';
import { dbManager } from '../utils/DatabaseManager';
import { UserModel } from '../viewmodel/UserModel';

@Entry
@Component
struct Login {
  @State phoneNumber: string = '';
  @State password: string = '';

  // 使用 @StorageLink 绑定全局变量。
  // 一旦此变量被赋值，全应用所有关联页面都会感知到
  @StorageLink('currentUser') currentUser: UserModel | null = null;

  build() {
    Column({ space: 20 }) {
      Text('欢迎登录')
        .fontSize(30)
        .fontWeight(FontWeight.Bold)
        .margin({ top: 50, bottom: 30 })

      TextInput({ placeholder: '请输入手机号', text: this.phoneNumber })
        .type(InputType.PhoneNumber)
        .onChange((v) => this.phoneNumber = v)
        .width('90%')

      TextInput({ placeholder: '请输入密码', text: this.password })
        .type(InputType.Password)
        .onChange((v) => this.password = v)
        .width('90%')

      Button('登录')
        .width('90%')
        .height(50)
        .onClick(async () => {
          await this.handleLogin();
        })

      Text('还没有账号？点击注册')
        .fontColor(Color.Blue)
        .onClick(() => {
          router.pushUrl({ url: 'pages/Register' });
        })
    }
    .width('100%')
    .height('100%')
  }

  async handleLogin() {
    if (this.phoneNumber === '' || this.password === '') {
      Prompt.showToast({ message: '请填写完整信息' });
      return;
    }

    // 1. 从数据库查询用户
    const user = await dbManager.user?.queryUserByPhone(this.phoneNumber);

    if (user && user.password === this.password) {
      // 2. 登录成功：更新 AppStorage
      // 这行代码执行后，currentUser 就不再是 null，PersistentStorage 会同步将其存入磁盘
      this.currentUser = user;

      Prompt.showToast({ message: '登录成功' });

      // 3. 跳转到主页 (假设路径为 pages/Index)
      setTimeout(() => {
        router.replaceUrl({ url: 'pages/Main' });
      }, 1000);

    } else {
      Prompt.showToast({ message: '手机号或密码错误' });
    }
  }
}