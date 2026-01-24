// 新增联系人页面：表单输入 + 保存提交
import { ContactData } from '../model/ContactData';
import { contact } from '@kit.ContactsKit';
import { router } from '@kit.ArkUI';
import { Context } from '@kit.AbilityKit';

@Entry
@Component
struct AddContact {
  // 数据层实例与表单状态
  private contactData: ContactData = new ContactData();
  @State name: string = '';
  @State phone: string = '';
  @State email: string = '';
  @State isSaving: boolean = false;

  // 添加aboutToAppear生命周期获取Context
  aboutToAppear() {
    // 优先使用父页面传入的 Context，缺省时使用当前页面 Context
    // 从路由参数获取父页面传递的Context
    const params = router.getParams() as Record<string, Object>;
    if (params && params['parentContext']) {
      this.contactData.setContext(params['parentContext'] as Context);
      console.log('AddContact: 已设置Context');
    } else {
      // 如果没有传递，尝试获取当前Context
      this.contactData.setContext(getContext());
      console.log('AddContact: 使用默认Context');
    }
  }

  build() {
    Column() {
      // 标题栏
      Row() {
        // 返回上一页
        Button() {
          Stack({ alignContent: Alignment.Center }) {
            // 圆形背景
            Column() {
              // 空 Column 作为背景
            }
            .width(40)
            .height(40)
            .borderRadius(20)
            .backgroundColor(Color.White)
            .shadow({
              radius: 1,
              color: '#00000010',
              offsetX: 0,
              offsetY: 1
            })

            // 叉号
            Text('×')
              .fontSize(36)
              .fontWeight(FontWeight.Medium)
              .fontColor('#444')
          }
        }
        .width(50)
        .height(50)
        .backgroundColor(Color.Transparent)
        .onClick(() => {
          router.back();
        })

        Text('新建联系人')
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .textAlign(TextAlign.Center)
          .layoutWeight(1)

        Button() {
          Stack({ alignContent: Alignment.Center }) {
            // 圆形背景
            Column() {
              // 空 Column 作为背景
            }
            .width(40)
            .height(40)
            .borderRadius(20)
            .backgroundColor(this.name && this.phone ? '#007DFF' : '#d1d1d6')
            .shadow({
              radius: 2,
              color: this.name && this.phone ? '#007DFF30' : '#00000010',
              offsetX: 0,
              offsetY: 1
            })

            // 对号
            Text('✓')
              .fontSize(24)
              .fontWeight(FontWeight.Medium)
              .fontColor(Color.White)
          }
        }
        .width(50)
        .height(50)
        .backgroundColor(Color.Transparent)
        .enabled(!!this.name && !!this.phone && !this.isSaving)
        .onClick(() => {
          this.saveContact();
        })
        .opacity(this.name && this.phone ? 1 : 0.6)
      }
      .width('100%')
      .padding({ left: 12, right: 12, top: 20, bottom: 20 })
      // .backgroundColor('#F0F0F0')
      .borderRadius(4)

      // 表单内容
      Scroll() {
        Column() {
          // 头像区域
          Column() {
            // 已填姓名取首字母大写，否则显示“+”
            Text(this.name ? this.name.charAt(0).toUpperCase() : '+')
              .fontSize(100)
              .fontColor(Color.White)
              .textAlign(TextAlign.Center)
              .width(180)
              .height(180)
              .borderRadius(70)
              .backgroundColor('#007DFF')
          }
          .width('100%')
          .padding(15)
          .justifyContent(FlexAlign.Center)

          // 基本信息表单
          Column() {
            Text('基本信息')
              .fontSize(16)
              .fontWeight(FontWeight.Medium)
              .textAlign(TextAlign.Start)
              .width('100%')
              .margin({ bottom: 15 })

            // 姓名输入框
            Column() {
              Text('姓名')
                .fontSize(14)
                .fontColor('#666')
                .textAlign(TextAlign.Start)
                .width('100%')
                .margin({ bottom: 5 })

              TextInput({ placeholder: '请输入姓名（必填）' })
                .width('100%')
                .height(50)
                .fontSize(16)
                .backgroundColor('#f8f8f8')
                .borderRadius(5)
                .padding({ left: 10, right: 10 })
                // 去除空格后写入 name
                .onChange((value: string) => {
                  this.name = value.trim();
                })
            }
            .width('100%')
            .margin({ bottom: 15 })

            Divider()
              .width('100%')
              .strokeWidth(1)
              .color('#eee')

            // 电话输入框
            Column() {
              Text('电话号码')
                .fontSize(14)
                .fontColor('#666')
                .textAlign(TextAlign.Start)
                .width('100%')
                .margin({ bottom: 5, top: 15 })

              TextInput({ placeholder: '请输入电话号码（必填）' })
                .width('100%')
                .height(50)
                .fontSize(16)
                .backgroundColor('#f8f8f8')
                .borderRadius(5)
                .padding({ left: 10, right: 10 })
                .type(InputType.Number)
                // 去除空格后写入 phone
                .onChange((value: string) => {
                  this.phone = value.trim();
                })
            }
            .width('100%')
            .margin({ bottom: 15 })

            Divider()
              .width('100%')
              .strokeWidth(1)
              .color('#eee')

            // 邮箱输入框
            Column() {
              Text('邮箱地址')
                .fontSize(14)
                .fontColor('#666')
                .textAlign(TextAlign.Start)
                .width('100%')
                .margin({ bottom: 5, top: 15 })

              TextInput({ placeholder: '请输入邮箱地址（可选）' })
                .width('100%')
                .height(50)
                .fontSize(16)
                .backgroundColor('#f8f8f8')
                .borderRadius(5)
                .padding({ left: 10, right: 10 })
                // 去除空格后写入 email（可选）
                .onChange((value: string) => {
                  this.email = value.trim();
                })
            }
            .width('100%')
          }
          .width('100%')
          .padding(20)
          .backgroundColor(Color.White)
          .borderRadius(10)
          .shadow({ radius: 2, color: '#10000000', offsetX: 0, offsetY: 1 })
          .margin({ left: 20, right: 20 })
        }
      }
      .layoutWeight(1)

      // 加载指示器
      if (this.isSaving) {
        Column() {
          LoadingProgress()
            .width(40)
            .height(40)
          Text('保存中...')
            .fontSize(14)
            .margin({ top: 10 })
        }
        // 居中展示保存中状态
        .width('100%')
        .justifyContent(FlexAlign.Center)
        .margin({ bottom: 20 })
      }
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#f5f5f5')
  }

  async saveContact() {
    // 简单校验：姓名/电话必填
    if (!this.name || !this.phone) {
      return;
    }

    // 进入保存中，禁用按钮并显示进度
    this.isSaving = true;

    try {
      // 组装联系人对象并写入系统通讯录
      const newContact = this.contactData.createContact(this.name, this.phone, this.email);
      await this.contactData.addContact(newContact);
      console.error('添加联系人成功');
      // 成功后返回上一页
      router.back();
    } catch (error) {
      console.error('添加联系人失败:', error);
    } finally {
      // 恢复保存状态
      this.isSaving = false;
    }
  }
}