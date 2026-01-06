// File: entry/src/main/ets/pages/ContactDetailPage.ets
import { Contact } from '../model/ContactModel';
import { SettingModel } from '../model/SettingModel'; // ✅ 引入配置模型
import { BiometricService } from '../services/BiometricService'; // ✅ 引入生物认证服务
import promptAction from '@ohos.promptAction';

@Component
export struct ContactDetailPage {
  // 接收页面路由传递过来的参数
  @State contact: Contact = new Contact('', '', '');

  // 🔒 新增：隐私与解锁状态
  @State isPrivacyOn: boolean = false; // 总开关：是否开启了隐私保护
  @State isUnlocked: boolean = false;  // 当前状态：是否已解锁查看

  private settingModel = new SettingModel();

  // 获取头像文字
  private getAvatarText(): string {
    const name = this.contact.name || '';
    return name.length > 0 ? name.substring(0, 1).toUpperCase() : '?';
  }

  // ✅ 生命周期：检查隐私设置
  async aboutToAppear() {
    // 1. 读取设置：用户是否在“我的”里开启了隐私锁？
    this.isPrivacyOn = await this.settingModel.isBiometricEnabled();

    // 2. 逻辑判断：如果没开锁，默认就是“已解锁”状态，直接看
    // 只有“开启了隐私锁”且“尚未验证”，isUnlocked 才是 false
    if (!this.isPrivacyOn) {
      this.isUnlocked = true;
    }
  }

  // ✅ 核心业务：触发指纹解锁
  async unlock() {
    const success = await BiometricService.getInstance().authenticate();
    if (success) {
      this.isUnlocked = true; // 改变状态，UI 会自动刷新显示真号码
      promptAction.showToast({ message: $r('app.string.unlock_success') });
    } else {
      promptAction.showToast({ message: $r('app.string.unlock_fail') });
    }
  }

  build() {
    NavDestination() {
      GridRow({
        columns: { sm: 4, md: 8, lg: 12 },
        gutter: { x: 12, y: 12 }
      }) {
        GridCol({
          span: { sm: 4, md: 6, lg: 8 },
          offset: { sm: 0, md: 1, lg: 2 }
        }) {
          Column() {
            // --- 头部区域 ---
            Column() {
              Stack({ alignContent: Alignment.Center }) {
                Circle({ width: 80, height: 80 }).fill('#E5F1FF')
                Text(this.getAvatarText())
                  .fontSize(30)
                  .fontWeight(FontWeight.Bold)
                  .fontColor('#007DFF')
              }
              .margin({ top: 40, bottom: 16 })

              Text(this.contact.name)
                .fontSize(24)
                .fontWeight(FontWeight.Bold)
                .fontColor(Color.Black)
                .margin({ bottom: 8 })

              Text(this.contact.relation)
                .fontSize(14)
                .fontColor(Color.Gray)
                .padding({ left: 12, right: 12, top: 4, bottom: 4 })
                .backgroundColor('#EFEFEF')
                .borderRadius(16)
            }
            .width('100%')
            .alignItems(HorizontalAlign.Center)

            // --- 详情列表区域 ---
            List() {
              // 📞 电话 (核心修改点：根据解锁状态显示不同内容)
              ListItem() {
                Row() {
                  Text($r('app.string.contact_detail_phone')) // "电话"
                    .width(80)
                    .fontColor(Color.Gray)
                    .fontSize(16)

                  if (this.isUnlocked) {
                    // ✅ 情况 A：已解锁/没开锁 -> 显示真实号码
                    Text(this.contact.phone)
                      .fontWeight(FontWeight.Regular)
                      .fontSize(16)
                      .fontColor(Color.Black)
                      .layoutWeight(1)
                      .textAlign(TextAlign.End)
                  } else {
                    // 🔒 情况 B：未解锁 -> 显示掩码 + 按钮
                    Row() {
                      // 138****1234
                      Text(this.contact.phone.replace(/(\d{3})\d{4}(\d{4})/, '$1****$2'))
                        .fontSize(16)
                        .fontColor(Color.Black)
                        .layoutWeight(1)
                        .textAlign(TextAlign.End)
                        .margin({ right: 8 })

                      // 查看按钮
                      Button($r('app.string.btn_view')) // "查看"
                        .type(ButtonType.Capsule)
                        .height(24)
                        .fontSize(12)
                        .backgroundColor('#007DFF')
                        .onClick(() => {
                          this.unlock(); // 点击触发指纹
                        })
                    }
                    .layoutWeight(1)
                    .justifyContent(FlexAlign.End)
                  }
                }
                .width('100%')
                .padding(16)
                .backgroundColor(Color.White)
              }

              // 📧 邮箱 (普通展示)
              this.DetailItem(
                $r('app.string.contact_detail_email'),
                this.contact.email || $r('app.string.contact_detail_empty')
              )

              // 🔗 关系 (普通展示)
              this.DetailItem(
                $r('app.string.contact_detail_relation'),
                this.contact.relation
              )
            }
            .width('100%')
            .borderRadius(16)
            .margin({ top: 30 })
            .backgroundColor(Color.White)
            .divider({
              strokeWidth: 0.5,
              color: '#F0F0F0',
              startMargin: 16,
              endMargin: 16
            })
          }
          .width('100%')
          .height('100%')
          .padding({ left: 16, right: 16 })
        }
      }
      .width('100%')
      .height('100%')
      .backgroundColor('#F1F3F5')
    }
    .title($r('app.string.contact_detail_title'))
    .backgroundColor('#F1F3F5')
  }

  // 普通列表项 Builder (保持原样)
  @Builder DetailItem(label: Resource, value: string | Resource) {
    ListItem() {
      Row() {
        Text(label)
          .width(80)
          .fontColor(Color.Gray)
          .fontSize(16)

        Text(value)
          .fontWeight(FontWeight.Regular)
          .fontSize(16)
          .fontColor(Color.Black)
          .layoutWeight(1)
          .textAlign(TextAlign.End)
          .textOverflow({ overflow: TextOverflow.Ellipsis })
          .maxLines(1)
      }
      .width('100%')
      .padding(16)
      .backgroundColor(Color.White)
    }
  }
}