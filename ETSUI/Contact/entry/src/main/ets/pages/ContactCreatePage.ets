import router from '@ohos.router'
import promptAction from '@ohos.promptAction'
import { BusinessError } from '@ohos.base'
import { Contact } from '../model/Contact'
import { ContactService } from '../service/ContactService'

@Entry
@Component
struct ContactCreatePage {
  @State name: string = ''
  @State phone: string = ''
  @State email: string = ''
  @State remark: string = ''
  @State isSubmitting: boolean = false

  async handleSave() {
    if (!this.name.trim() || !this.phone.trim()) {
      promptAction.showToast({ message: '姓名和手机号不能为空' })
      return
    }

    this.isSubmitting = true
    try {
      const contact = new Contact(this.name.trim(), this.phone.trim())
      contact.email = this.email.trim()
      contact.remark = this.remark.trim()

      console.info('[ContactCreatePage] 准备添加联系人:', contact)

      // 添加联系人并获取系统生成的 key
      const newKey = await ContactService.addContact(contact)
      console.info('[ContactCreatePage] 添加成功，系统key:', newKey)

      promptAction.showToast({ message: '保存成功' })

      setTimeout(() => {
        router.back()
      }, 500)

    } catch (err) {
      const error = err as BusinessError;
      console.error('[ContactCreatePage] 保存失败:', error)
      promptAction.showToast({
        message: `保存失败: ${error.message || '未知错误'}`
      })
    } finally {
      this.isSubmitting = false
    }
  }

  build() {
    Navigation() {
      Column() {
        List() {
          ListItem() {
            this.InputItem(
              '姓名',
              '请输入姓名',
              this.name,
              (val) => this.name = val
            )
          }

          ListItem() {
            this.InputItem(
              '手机',
              '请输入手机号',
              this.phone,
              (val) => this.phone = val,
              InputType.PhoneNumber
            )
          }

          ListItem() {
            this.InputItem(
              '邮箱',
              '请输入邮箱（选填）',
              this.email,
              (val) => this.email = val,
              InputType.Email
            )
          }

          ListItem() {
            this.InputItem(
              '备注',
              '请输入备注（选填）',
              this.remark,
              (val) => this.remark = val
            )
          }
        }
        .divider({ strokeWidth: 1, color: '#E5E5E5', startMargin: 16, endMargin: 16 })
        .borderRadius(12)
        .backgroundColor(Color.White)
        .margin(16)

        Button('保存联系人')
          .width('90%')
          .height(44)
          .margin({ top: 20 })
          .enabled(!this.isSubmitting)
          .onClick(() => this.handleSave())
      }
      .width('100%')
      .height('100%')
      .backgroundColor('#F5F5F5')
    }
    .title('新建联系人')
    .titleMode(NavigationTitleMode.Mini)
  }

  /**
   * 通用输入项组件
   */
  @Builder
  InputItem(
    label: string,
    placeholder: string,
    value: string,
    onValueChange: (val: string) => void,
    type: InputType = InputType.Normal
  ) {
    Row() {
      Text(label)
        .width(64)
        .fontSize(16)
        .fontColor(Color.Black)

      TextInput({ placeholder: placeholder, text: value })
        .type(type)
        .layoutWeight(1)
        .backgroundColor(Color.Transparent)
        .onChange(onValueChange)
    }
    .padding({ left: 16, right: 16 })
    .height(56)
    .width('100%')
  }
}