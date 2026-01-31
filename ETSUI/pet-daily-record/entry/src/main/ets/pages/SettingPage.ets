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
import promptAction from '@ohos.promptAction';
import dataPreferences from '@ohos.data.preferences';

/**
 * ============================================================================
 * 【个人设置页面组件】
 * 核心功能：多用户个性化昵称设置与账号注销系统
 * ============================================================================
 */
@Component
export struct SettingPage {
  /** 最终展示的昵称 */
  @State nickname: string = '萌宠主'
  /** 文本框输入的临时内容 */
  @State inputNickname: string = ''
  /** 同步全局登录的用户名 */
  @StorageLink('currentUserName') loggedUser: string = 'admin'

  /**
   * 生命周期：显示组件时立即同步硬盘数据
   */
  async aboutToAppear() {
    await this.syncUserPreference();
  }

  /**
   * --- UI 构建器：个人资料展示区域 ---
   */
  @Builder
  UserCard() {
    Column({ space: 12 }) {
      Text('🐾')
        .fontSize(60)
        .margin({ top: 20 })

      Text(this.nickname)
        .fontSize(22)
        .fontWeight(FontWeight.Bold)
        .fontColor('#333333')

      Row() {
        Text('身份ID: ').fontSize(12).fontColor('#999')
        Text(this.loggedUser).fontSize(12).fontColor('#666').fontWeight(FontWeight.Medium)
      }
      .backgroundColor('#F5F7FA')
      .padding({
        left: 12,
        right: 12,
        top: 4,
        bottom: 4
      })
      .borderRadius(8)
      .margin({ bottom: 20 })
    }
    .width('92%')
    .backgroundColor(Color.White)
    .borderRadius(20)
    .shadow({ radius: 10, color: '#08000000', offsetY: 5 })
  }

  build() {
    Column() {
      // 1. 顶部自定义标题
      Row() {
        Text('设置中心')
          .fontSize(24)
          .fontWeight(FontWeight.Bold)
          .fontColor('#1A1A1A')
      }
      .width('100%')
      .padding({ left: 25, top: 40, bottom: 20 })

      // 2. 资料卡片
      this.UserCard()

      // 3. 昵称修改表单区域
      Column({ space: 12 }) {
        Text('修改我的昵称').fontSize(14).fontColor('#666').width('100%')

        Row({ space: 10 }) {
          TextInput({ text: this.inputNickname, placeholder: '取个好听的名字...' })
            .layoutWeight(1)
            .height(48)
            .backgroundColor('#F5F7FA')
            .borderRadius(10)
            .onChange((val: string) => {
              this.inputNickname = val;
            })

          Button('保存')
            .onClick(() => {
              this.handleSaveNickname();
            })
            .backgroundColor('#4CAF50')
            .height(48)
            .padding({ left: 20, right: 20 })
            .borderRadius(10)
        }
        .width('100%')
      }
      .width('92%')
      .padding(20)
      .backgroundColor(Color.White)
      .borderRadius(20)
      .margin({ top: 20 })

      // 占据多余空间
      Blank().layoutWeight(1)

      // 4. 退出操作区
      Column() {
        Button('退出当前账号')
          .width('85%')
          .height(50)
          .backgroundColor('#FFF1F0')
          .fontColor('#F5222D')
          .fontSize(16)
          .fontWeight(FontWeight.Medium)
          .borderRadius(15)
          .margin({ bottom: 40 })
          .onClick(() => {
            this.triggerLogoutProcess();
          })
      }
      .width('100%')
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F7F8FA')
  }

  /**
   * 业务逻辑：从磁盘加载属于当前用户的个性化配置
   */
  private async syncUserPreference() {
    try {
      console.info('PetDiary: 正在加载用户 [' + this.loggedUser + '] 的个性化配置');
      let pref = await dataPreferences.getPreferences(getContext(this), 'AccountStore');

      /**
       * 组长设计点：使用动态 Key 避免账号间数据污染
       * 存储格式如：nickname_admin, nickname_zqh
       */
      const savedName = await pref.get('nickname_' + this.loggedUser, '萌宠主') as string;

      this.nickname = savedName;
      this.inputNickname = savedName;

      console.info('PetDiary: 昵称配置加载完成 -> ' + savedName);
    } catch (e) {
      console.error('PetDiary: 配置加载异常');
    }
  }

  /**
   * 业务逻辑：持久化保存自定义昵称
   */
  private async handleSaveNickname() {
    // 1. 基础校验
    if (this.inputNickname.trim() === '') {
      promptAction.showToast({ message: '昵称不能为空' });
      return;
    }

    if (this.inputNickname.length > 10) {
      promptAction.showToast({ message: '昵称太长了(限10字)' });
      return;
    }

    try {
      // 2. 写入硬盘
      let pref = await dataPreferences.getPreferences(getContext(this), 'AccountStore');
      await pref.put('nickname_' + this.loggedUser, this.inputNickname);
      await pref.flush(); // 必须落盘

      // 3. 更新界面状态
      this.nickname = this.inputNickname;
      promptAction.showToast({ message: '修改成功 ✨' });
      console.info('PetDiary: 用户昵称已更新为 -> ' + this.nickname);

    } catch (err) {
      console.error('PetDiary: 昵称保存失败');
      promptAction.showToast({ message: '系统繁忙，保存失败' });
    }
  }

  /**
   * 业务逻辑：执行退出登录流程
   */
  private triggerLogoutProcess() {
    AlertDialog.show({
      title: '注销确认',
      message: '确定要退出当前账号 [' + this.loggedUser + '] 吗？',
      primaryButton: {
        value: '返回',
        action: () => {
          console.info('User cancelled logout.');
        }
      },
      secondaryButton: {
        value: '退出登录',
        fontColor: Color.Red,
        action: () => {
          /**
           * 【组长必看核心动作】
           * 这一行代码会清空全局身份，直接导致 Index.ets 的路由守卫生效，
           * 将整个 UI 树切回 LoginPage，实现完美的退出效果。
           */
          AppStorage.setOrCreate('currentUserName', '');

          console.info('PetDiary: Session cleared. Identity card destroyed.');
          promptAction.showToast({ message: '已安全退出' });
        }
      }
    })
  }
}