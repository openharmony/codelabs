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

// SettingsPage.ets
import router from '@ohos.router';
import promptAction from '@ohos.promptAction';
import preferences from '@ohos.data.preferences';
import common from '@ohos.app.ability.common';

// 在应用入口文件(如EntryAbility.ets)中添加初始化：
// AppStorage.SetOrCreate('isRecommendOn', false);

@Entry
@Component
struct SettingsPage {
  // 状态管理
  @State cacheSize: string = "7.62TB"
  @StorageLink('isRecommendOn') isRecommendOn: boolean = false // 本地初始化
  @State showRunImage: boolean = false

  // 存储相关
  private pref: preferences.Preferences | null = null
  private context: common.UIAbilityContext = getContext(this) as common.UIAbilityContext

  // 生命周期：组件创建时加载设置
  async aboutToAppear() {
    try {
      this.pref = await preferences.getPreferences(this.context, 'userSettings')
      const savedValue = await this.pref.get('isRecommendOn', false)
      this.isRecommendOn = Boolean(savedValue) // 类型安全转换
    } catch (err) {
      console.error('Preferences加载失败: ' + err)
    }
  }

  // 生命周期：组件销毁时保存设置
  async aboutToDisappear() {
    if (this.pref) {
      await this.pref.put('isRecommendOn', this.isRecommendOn)
      await this.pref.flush()
    }
  }

  // 清除缓存对话框
  showClearCacheConfirm() {
    AlertDialog.show({
      title: '清除缓存',
      message: '确定要清除所有缓存数据吗？',
      primaryButton: {
        value: '取消',
        action: () => console.log('取消清除缓存'),
        backgroundColor: '#F5F5F5',
        fontColor: '#666666'
      },
      secondaryButton: {
        value: '确认清除',
        action: () => this.clearCacheAction(),
        backgroundColor: '#FF0000',
        fontColor: '#FFFFFF'
      },
      alignment: DialogAlignment.Bottom,
      cancel: () => console.log('对话框取消')
    })
  }

  // 执行清除操作
  clearCacheAction() {
    this.showRunImage = true
    setTimeout(() => {
      this.showRunImage = false
      promptAction.showToast({
        message: '清除成功！',
        duration: 2000
      })
      this.cacheSize = '0B'
    }, 3500)
  }

  // 退出登录确认对话框
  showLogoutConfirm() {
    AlertDialog.show({
      title: '退出登录',
      message: '确定要退出当前账号吗？',
      primaryButton: {
        value: '取消',
        action: () => console.log('点击了取消'),
        backgroundColor: '#666666',
        fontColor: '#FFFFFF'
      },
      secondaryButton: {
        value: '确定退出',
        action: () => this.performLogout(),
        backgroundColor: '#F5F5F5',
        fontColor: '#FF0000'
      },
      cancel: () => console.log('对话框取消')
    })
  }

  // 执行退出登录
  performLogout() {
    router.clear()
    router.replaceUrl({
      url: 'pages/login'
    })
  }

  build() {
    Stack() {
      Column() {
        // 顶部标题栏
        Row() {
          Button({ type: ButtonType.Circle })
            .backgroundImage($r('app.media.return1'))
            .size({ width: 20 })
            .backgroundColor(Color.White)
            .backgroundImageSize(ImageSize.FILL)
            .onClick(() => {
              router.back()
            })

          Text('设置')
            .fontSize(24)
            .margin({ top: 1, left: 110, bottom: 20 })
            .width('100%')
            .fontWeight(FontWeight.Bold)
        }

        Divider()
          .color('#E0E0E0')
          .margin({ bottom: 20 })

        // 清除缓存条目
        Row() {
          Text('清除缓存')
            .fontSize(18)
            .layoutWeight(1)

          Text(this.cacheSize)
            .fontSize(16)
            .fontColor('#999999')
            .margin({ right: 10 })
        }
        .height(50)
        .width('100%')
        .onClick(() => {
          this.showClearCacheConfirm()
        })

        // 个性化推荐开关
        Row() {
          Text('个性化推荐')
            .fontSize(18)
            .layoutWeight(1)

          Toggle({ type: ToggleType.Switch, isOn: this.isRecommendOn })
            .onChange((isOn: boolean) => {
              this.isRecommendOn = isOn
              console.log('推荐开关状态：' + isOn)
            })
            .width(60)
            .height(30)
            .selectedColor('#007DFF')
            .switchPointColor('#FFFFFF')
        }
        .height(50)
        .width('100%')

        // 关于我们
        Row() {
          Text('关于我们')
            .fontSize(18)
            .layoutWeight(1)
        }
        .height(50)
        .width('100%')
        .onClick(() => {
          AlertDialog.show({
            title: '关于我们',
            message: '版本号：1.2.1 \n联系我们：17777777777 \n内容合作：111@1097657620@qq.com',
            confirm: {
              value: '确定',
              action: () => {
                console.log('关于我们对话框确定')
              }
            },
            cancel: () => {
              console.log('关于我们对话框取消')
            }
          })
        })

        // 用户协议（完整协议文本）
        Row() {
          Text('美食杰用户协议')
            .fontSize(18)
            .layoutWeight(1)
        }
        .height(50)
        .width('100%')
        .onClick(() => {
          AlertDialog.show({
            title: '用户协议',
            message: `掌上食谱app 用户协议
版本号：V1.2.1
生效日期：2026年01月21日

一、协议范围
1. 适用主体：本协议由运营方与用户共同签署，适用于用户对本平台全部功能的使用行为。
2. 关联服务：用户使用本平台关联的小程序、网页端或第三方合作服务时，同样受本协议约束。

二、账号与注册
1. 账号注册：
   - 用户需提供真实有效的手机号码完成注册，平台将通过短信验证码验证身份。
   - 禁止使用他人身份信息注册，由此引发的法律责任由用户自行承担。
2. 账号安全：
   - 用户需妥善保管账号密码，因账号泄露导致的损失由用户承担。
   - 发现账号异常应立即联系客服，平台将协助处理但不承担连带责任。

三、服务内容与规范
1. 核心功能：
   - 提供美食记录、餐厅评价、食谱分享等社交功能
   - 提供商家合作推广、优惠券发放等增值服务
2. 用户义务：
   - 不得发布违法、虚假、侵权或低俗内容（如伪造餐厅评分、传播未经授权的食谱）
   - 禁止利用平台进行商业营销（如批量发布广告、诱导关注第三方账号）
   - 尊重他人知识产权，未经授权不得转载或抄袭用户生成内容

四、知识产权与内容管理
1. 平台权利：
   - 本平台的软件代码、商标标识、运营数据等知识产权归运营方所有
   - 用户发布的内容（如照片、文字）视为授权平台在全球范围内免费使用、编辑、推广，但用户保留署名权
2. 内容处理：
   - 平台有权审核、删除违规内容，对多次违规用户永久封禁账号
   - 因用户内容引发的侵权纠纷，由用户承担全部法律责任

五、隐私保护
1. 信息收集：
   - 必要信息：注册手机号、设备标识符、地理位置（用于推荐附近餐厅）
   - 可选信息：相册访问权限（用于上传图片）、麦克风权限（用于语音搜索）
2. 数据使用：
   - 仅用于优化服务、个性化推荐及合规统计
   - 不会向第三方共享个人信息，法律强制要求除外
3. 儿童保护：
   - 未满14周岁用户需监护人同意方可使用，平台将采取特殊保护措施

六、免责声明
1. 服务风险：
   - 平台不保证服务无中断或数据无丢失，因网络故障、不可抗力等导致的损失不承担责任
   - 用户因使用平台内容（如餐厅推荐）产生的消费纠纷，由用户与商家自行解决
2. 第三方链接：
   - 平台可能包含第三方网站链接，其内容与本平台无关，用户需自行判断风险

七、协议变更与终止
1. 协议更新：
   - 平台有权单方面修改协议，更新内容将通过App公告或弹窗通知
   - 用户继续使用即视为接受新协议，否则应立即停止使用
2. 终止服务：
   - 用户可随时注销账号，但已发布内容仍受本协议约束
   - 平台有权因用户违规永久封禁账号，且不退还已支付费用（如有）

八、争议解决
1. 法律适用：本协议受中华人民共和国法律管辖
2. 争议处理：
   - 双方应先友好协商，协商不成可向运营方所在地人民法院提起诉讼

九、其他条款
1. 联系方式：
   - 客服邮箱：1097657620@qq.com
   - 地址：北京市海淀区紫竹院街道北京理工大学
2. 条款独立性：若某条款被认定无效，不影响其他条款的法律效力

十、特别提示
请用户务必仔细阅读本协议，尤其是加粗条款。如不同意任何内容，请勿注册或使用本平台。`,
            confirm: {
              value: '确定',
              action: () => {
                console.log('用户协议对话框确定')
              }
            },
            cancel: () => {
              console.log('用户协议对话框取消')
            }
          })
        })

        // 开发人员信息
        Row() {
          Text('开发人员')
            .fontSize(18)
            .layoutWeight(1)
        }
        .height(50)
        .width('100%')
        .onClick(() => {
          AlertDialog.show({
            title: '开发人员',
            message: '',
            confirm: {
              value: '确定',
              action: () => {
                console.log('开发人员对话框确定')
              }
            },
            cancel: () => {
              console.log('开发人员对话框取消')
            }
          })
        })

        // 退出登录按钮
        Button() {
          Text('退出登录')
            .fontSize(20)
            .fontColor('#FFFFFF')
            .fontWeight(FontWeight.Bold)
        }
        .width('100%')
        .height(50)
        .margin({ top: 280 })
        .backgroundColor(Color.Red)
        .borderRadius(0)
        .onClick(() => {
          this.showLogoutConfirm()
        })
      }
      .width('100%')
      .height('100%')
      .padding(20)

      // 清除缓存动画
      if (this.showRunImage) {
        Stack({ alignContent: Alignment.Center }) {
          Image($r('app.media.loading'))
            .width(150)
            .height(150)
        }
        .width('100%')
        .height('100%')
      }
    }
    .width('100%')
    .height('100%')
  }
}
