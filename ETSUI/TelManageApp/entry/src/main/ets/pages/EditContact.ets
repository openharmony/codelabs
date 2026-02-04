/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-20
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/*
* 该文件用于编辑联系人信息的页面逻辑文件
* 文件定义了联系人编辑页面的功能和界面布局。
* 它通过 ContactData 类与联系人数据交互
* 支持加载联系人信息、编辑联系人姓名、电话号码和邮箱，以及保存或删除联系人。
* 同时，
* 文件中包含了页面的生命周期方法、状态管理和用户界面构建逻辑
*/


// 联系人数据管理类
import { ContactData } from '../model/ContactData';
// 提供联系人相关的操作接口
import { contact } from '@kit.ContactsKit';
// 提供页面路由功能
import { router } from '@kit.ArkUI';

@Entry
@Component
struct EditContact {
  // 联系人数据管理实例
  private contactData: ContactData = new ContactData();
  // 当前联系人唯一标识符
  private contactKey: string = '';
  /*
  * 定义状态变量
  * 当前联系人信息、姓名、电话号码、邮箱
  * 是否正在加载、是否正在保存、是否显示删除确认对话框
  */
  @State contactInfo: contact.Contact | null = null;
  @State name: string = '';
  @State phone: string = '';
  @State email: string = '';
  @State isLoading: boolean = true;
  @State isSaving: boolean = false;
  @State showDeleteConfirm: boolean = false;

  // 生命周期方法，页面即将显示时调用
  aboutToAppear() {
    // 直接获取当前页面Context
    this.contactData.setContext(getContext(this));
    // 获取页面传递的参数
    const params = router.getParams() as Record<string, string>;
    this.contactKey = params?.contactKey || '';
    // 如果有联系人Key，则加载联系人信息
    if (this.contactKey) {
      this.loadContact();
    } else {
      // 无联系人Key，直接结束加载状态：
      this.isLoading = false;
    }
  }

  // 加载联系人信息
  async loadContact() {
    this.isLoading = true;
    // 根据 ID 查询联系人
    this.contactInfo = await this.contactData.queryContactById(this.contactKey);
    // 如果查询到联系人信息，则更新状态
    if (this.contactInfo) {
      this.name = this.contactInfo.name?.fullName || '';
      this.phone = this.contactInfo.phoneNumbers?.[0]?.phoneNumber || '';
      this.email = this.contactInfo.emails?.[0]?.email || '';
    }
    this.isLoading = false;
  }

  // 构建页面 UI
  build() {
    Stack() {
      // 主内容区域
      Column() {
        if (this.isLoading) {
          //// 显示加载状态
          Column() {
            LoadingProgress()
              .width(50)
              .height(50)
            Text('加载中...')
              .fontSize(16)
              .margin({ top: 10 })
          }
          .width('100%')
          .height('100%')
          .justifyContent(FlexAlign.Center)
        } else if (!this.contactInfo) {
          // 显示联系人不存在的提示
          Column() {
            Text('联系人不存在')
              .fontSize(18)
              .fontColor('#666')
              .margin({ bottom: 20 })

            Button('返回')
              .width(120)
              .height(40)
              .onClick(() => {
                router.back();
              })
          }
          .width('100%')
          .height('100%')
          .justifyContent(FlexAlign.Center)
        } else {
          // 标题栏
          Row() {
            Button('取消')
              .fontSize(16)
              .backgroundColor(Color.Transparent)
              .fontColor('#007DFF')
              .onClick(() => {
                router.back();
              })

            Text('编辑联系人')
              .fontSize(18)
              .fontWeight(FontWeight.Bold)
              .textAlign(TextAlign.Center)
              .layoutWeight(1)

            Button('保存')
              .fontSize(16)
              .backgroundColor(Color.Transparent)
              .fontColor(this.name && this.phone && !this.isSaving ? '#007DFF' : '#999')
              .enabled(!!this.name && !!this.phone && !this.isSaving)
              .onClick(() => {
                this.saveContact();
              })
          }
          .width('100%')
          .padding(20)
          .backgroundColor('#f0f0f0')

          // 表单内容
          Scroll() {
            Column() {
              // 头像区域
              Column() {
                Text(this.name ? this.name.charAt(0).toUpperCase() : '?')
                  .fontSize(40)
                  .fontColor(Color.White)
                  .textAlign(TextAlign.Center)
                  .width(80)
                  .height(80)
                  .borderRadius(40)
                  .backgroundColor('#007DFF')
              }
              .width('100%')
              .padding(30)
              .justifyContent(FlexAlign.Center)

              // 基本信息
              Column() {
                Text('基本信息')
                  .fontSize(16)
                  .fontWeight(FontWeight.Medium)
                  .textAlign(TextAlign.Start)
                  .width('100%')
                  .margin({ bottom: 15 })

                // 姓名
                Column() {
                  Text('姓名')
                    .fontSize(14)
                    .fontColor('#666')
                    .textAlign(TextAlign.Start)
                    .width('100%')
                    .margin({ bottom: 5 })

                  TextInput({ text: this.name, placeholder: '请输入姓名' })
                    .width('100%')
                    .height(50)
                    .fontSize(16)
                    .backgroundColor('#f8f8f8')
                    .borderRadius(5)
                    .onChange((value: string) => {
                      this.name = value.trim();
                    })
                }
                .width('100%')
                .margin({ bottom: 15 })

                // 电话
                Column() {
                  Text('电话号码')
                    .fontSize(14)
                    .fontColor('#666')
                    .textAlign(TextAlign.Start)
                    .width('100%')
                    .margin({ bottom: 5 })

                  TextInput({ text: this.phone, placeholder: '请输入电话号码' })
                    .width('100%')
                    .height(50)
                    .fontSize(16)
                    .backgroundColor('#f8f8f8')
                    .borderRadius(5)
                    .type(InputType.Number)
                    .onChange((value: string) => {
                      this.phone = value.trim();
                    })
                }
                .width('100%')
                .margin({ bottom: 15 })

                // 邮箱
                Column() {
                  Text('邮箱地址')
                    .fontSize(14)
                    .fontColor('#666')
                    .textAlign(TextAlign.Start)
                    .width('100%')
                    .margin({ bottom: 5 })

                  TextInput({ text: this.email, placeholder: '请输入邮箱地址（可选）' })
                    .width('100%')
                    .height(50)
                    .fontSize(16)
                    .backgroundColor('#f8f8f8')
                    .borderRadius(5)
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
              .shadow({
                radius: 2,
                color: '#10000000',
                offsetX: 0,
                offsetY: 1
              })
              .margin({ bottom: 20 })

              // 删除按钮
              Button('删除联系人')
                .width('100%')
                .height(50)
                .backgroundColor('#ff3b30')
                .fontColor(Color.White)
                .fontSize(16)
                .borderRadius(10)
                .onClick(() => {
                  this.showDeleteConfirm = true;
                })
            }
            .width('100%')
            .padding(20)
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
            .width('100%')
            .height(100)
            .justifyContent(FlexAlign.Center)
          }
        }
      }
      .width('100%')
      .height('100%')
      .backgroundColor('#f5f5f5')

      // 删除确认对话框 - 使用绝对定位覆盖在主内容上
      if (this.showDeleteConfirm) {
        Column() {
          // 半透明背景
          Blank()
            .width('100%')
            .height('100%')
            .backgroundColor('#000000')
            .opacity(0.5)
            .onClick(() => {
              this.showDeleteConfirm = false;
            })

          // 确认对话框内容
          Column() {
            Text('删除联系人')
              .fontSize(18)
              .fontWeight(FontWeight.Bold)
              .margin({ bottom: 10 })

            Text('确定要删除这个联系人吗？此操作不可撤销。')
              .fontSize(14)
              .textAlign(TextAlign.Center)
              .margin({ bottom: 20 })

            Row() {
              Button('取消')
                .layoutWeight(1)
                .height(40)
                .backgroundColor('#f0f0f0')
                .fontColor('#333')
                .onClick(() => {
                  this.showDeleteConfirm = false;
                })

              Button('删除')
                .layoutWeight(1)
                .height(40)
                .margin({ left: 10 })
                .backgroundColor('#ff3b30')
                .fontColor(Color.White)
                .onClick(() => {
                  this.showDeleteConfirm = false;
                  this.deleteContact();
                })
            }
            .width('100%')
            .justifyContent(FlexAlign.SpaceBetween)
          }
          .width('80%')
          .padding(20)
          .backgroundColor(Color.White)
          .borderRadius(10)
          .shadow({
            radius: 10,
            color: '#40000000',
            offsetX: 0,
            offsetY: 5
          })
        }
        .width('100%')
        .height('100%')
        .justifyContent(FlexAlign.Center)
        .alignItems(HorizontalAlign.Center)
        .position({ x: 0, y: 0 })
      }
    }
    .width('100%')
    .height('100%')
  }

  async saveContact() {
    if (!this.name || !this.phone || !this.contactInfo) {
      return;
    }

    this.isSaving = true;

    try {
      // 准备更新所需数据
      const updatedContact: contact.Contact = {
        id: this.contactInfo.id,
        key: this.contactInfo.key,
        name: {
          fullName: this.name
        },
        phoneNumbers: [{
          phoneNumber: this.phone,
          labelId: contact.PhoneNumber.NUM_MOBILE
        }],
        emails: this.email ? [{
          email: this.email,
          labelId: contact.Email.EMAIL_HOME
        }] : []
      };

      // 手动复制其他可能存在的属性
      if (this.contactInfo.portrait) {
        updatedContact.portrait = this.contactInfo.portrait;
      }
      if (this.contactInfo.organization) {
        updatedContact.organization = this.contactInfo.organization;
      }
      if (this.contactInfo.note) {
        updatedContact.note = this.contactInfo.note;
      }
      if (this.contactInfo.relations) {
        updatedContact.relations = this.contactInfo.relations;
      }
      if (this.contactInfo.imAddresses) {
        updatedContact.imAddresses = this.contactInfo.imAddresses;
      }
      if (this.contactInfo.websites) {
        updatedContact.websites = this.contactInfo.websites;
      }
      if (this.contactInfo.events) {
        updatedContact.events = this.contactInfo.events;
      }
      if (this.contactInfo.postalAddresses) {
        updatedContact.postalAddresses = this.contactInfo.postalAddresses;
      }
      await this.contactData.updateContact(updatedContact);
      // 保存成功，返回上一页
      router.back();
    } catch (error) {
      console.error('更新联系人失败:');
      // 这里可以添加Toast提示
    } finally {
      this.isSaving = false;
    }
  }

  async deleteContact() {
    try {
      await this.contactData.deleteContact(this.contactKey);
      // 删除成功，返回上一页
      router.back();
    } catch (error) {
      console.error('删除联系人失败:');
      // 这里可以添加Toast提示
    }
  }
}