// SPDX-License-Identifier: MIT
// pages/MyBusinessCard.ets
import { BusinessCardData } from '../model/BusinessCardData';
import { router } from '@kit.ArkUI';

@Entry
@Component
struct MyBusinessCard {
  // 使用ArkUI的LocalStorage
  @StorageLink('businessCardData') cardData: BusinessCardData = {
    name: '',
    phone: '',
    email: '',
    company: '',
    position: '',
    department: '',
    address: '',
    website: '',
    notes: '这是我的个人名片',
    showInList: true,
    contactId: ''
  };

  // 编辑状态
  @State isEditing: boolean = false;
  @State isSaving: boolean = false;
  @State showSuccessMessage: boolean = false;

  // 页面显示时加载数据
  aboutToAppear() {
    console.log('MyBusinessCard: 页面即将显示');
    // LocalStorage会自动管理数据，这里不需要手动加载
    // 如果是第一次访问，检查是否需要初始化默认数据
    if (this.cardData.name === '' && this.cardData.contactId === '') {
      this.initDefaultCardData();
    }
  }

  // 初始化默认名片数据
  initDefaultCardData() {
    const defaultData: BusinessCardData = {
      name: '我的名片',
      phone: '',
      email: '',
      company: '',
      position: '',
      department: '',
      address: '',
      website: '',
      notes: '这是我的个人名片',
      showInList: true,
      contactId: ''
    };

    // 手动设置每个字段，避免使用spread操作符
    this.cardData.name = defaultData.name;
    this.cardData.phone = defaultData.phone;
    this.cardData.email = defaultData.email;
    this.cardData.company = defaultData.company;
    this.cardData.position = defaultData.position;
    this.cardData.department = defaultData.department;
    this.cardData.address = defaultData.address;
    this.cardData.website = defaultData.website;
    this.cardData.notes = defaultData.notes;
    this.cardData.showInList = defaultData.showInList;
    this.cardData.contactId = defaultData.contactId;
  }

  // 保存名片数据到LocalStorage
  saveCardDataToLocal(): boolean {
    try {
      // LocalStorage会自动保存，这里直接返回true
      console.log('MyBusinessCard: 名片数据已自动保存到LocalStorage');
      return true;
    } catch (error) {
      console.error('MyBusinessCard: 保存名片数据失败:', error);
      return false;
    }
  }

  // 开始编辑 - 直接进入编辑模式
  startEditing() {
    this.isEditing = true;
    console.log('MyBusinessCard: 进入编辑模式');
  }

  // 取消编辑 - 退出编辑模式
  cancelEditing() {
    this.isEditing = false;
    console.log('MyBusinessCard: 取消编辑');
  }

  // 保存名片
  async saveBusinessCard() {
    console.log('MyBusinessCard: 开始保存名片');

    // 验证必填字段
    if (!this.cardData.name || this.cardData.name.trim() === '') {
      console.error('MyBusinessCard: 姓名不能为空');
      this.showAlert('提示', '姓名不能为空');
      return;
    }

    this.isSaving = true;

    // 使用setTimeout模拟异步保存
    setTimeout(() => {
      try {
        // LocalStorage会自动保存，这里只是显示成功消息
        console.log('MyBusinessCard: 保存成功');
        this.showSuccessMessage = true;
        this.isEditing = false;

        // 3秒后隐藏成功消息
        setTimeout(() => {
          this.showSuccessMessage = false;
        }, 3000);

        // 提示用户
        this.showAlert('保存成功', '名片信息已保存。\n\n如需添加到系统通讯录，请手动操作。');
      } catch (error) {
        console.error('MyBusinessCard: 保存异常:', error);
        const errorMsg = String(error);
        this.showAlert('保存异常', errorMsg);
      } finally {
        this.isSaving = false;
      }
    }, 500);
  }

  // 删除名片
  deleteBusinessCard() {
    console.log('MyBusinessCard: 开始删除名片');

    this.showConfirmDialog('确认删除', '确定要删除您的名片吗？', () => {
      try {
        // 重置名片数据为默认值
        this.initDefaultCardData();

        console.log('MyBusinessCard: 删除成功');
        this.showAlert('提示', '名片删除成功');
      } catch (error) {
        console.error('MyBusinessCard: 删除异常:', error);
        const errorMsg = String(error);
        this.showAlert('删除异常', errorMsg);
      }
    });
  }

  // 显示提示框
  showAlert(title: string, message: string) {
    AlertDialog.show({
      title: title,
      message: message,
      alignment: DialogAlignment.Center,
      primaryButton: {
        value: '确定',
        action: () => {
          console.log('用户确认提示');
        }
      }
    });
  }

  // 显示确认对话框
  showConfirmDialog(title: string, message: string, onConfirm: () => void) {
    AlertDialog.show({
      title: title,
      message: message,
      alignment: DialogAlignment.Center,
      primaryButton: {
        value: '取消',
        action: () => {
          console.log('用户取消操作');
        }
      },
      secondaryButton: {
        value: '确认',
        fontColor: '#ff3b30',
        action: () => {
          console.log('用户确认操作');
          onConfirm();
        }
      }
    });
  }

  // 返回上一页
  goBack() {
    router.back();
  }

  // 分享名片
  shareBusinessCard() {
    console.log('MyBusinessCard: 分享名片');
    this.showAlert('名片信息',
      '姓名: ' + (this.cardData.name || '未设置') + '\n' +
        '电话: ' + (this.cardData.phone || '未设置') + '\n' +
        '邮箱: ' + (this.cardData.email || '未设置') + '\n' +
        '公司: ' + (this.cardData.company || '未设置') + '\n' +
        '职位: ' + (this.cardData.position || '未设置') + '\n' +
        '\n您可以截图分享此信息。'
    );
  }

  build() {
    Column() {
      // 顶部导航栏
      Row() {
        // 返回按钮
        Button() {
          Stack({ alignContent: Alignment.Center }) {
            // 圆形背景
            Column() {}
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
          this.goBack();
        })

        Text('我的名片')
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .textAlign(TextAlign.Center)
          .layoutWeight(1)

        // 操作按钮
        if (!this.isEditing) {
          Row() {
            // 分享按钮
            Button() {
              Stack({ alignContent: Alignment.Center }) {
                Column() {}
                .width(40)
                .height(40)
                .borderRadius(20)
                .backgroundColor(Color.White)
                .shadow({
                  radius: 2,
                  color: '#00000010',
                  offsetX: 0,
                  offsetY: 1
                })

                Text('↗')
                  .fontSize(24)
                  .fontWeight(FontWeight.Medium)
                  .fontColor('#444')
              }
            }
            .width(50)
            .height(50)
            .backgroundColor(Color.Transparent)
            .onClick(() => {
              this.shareBusinessCard();
            })
            .margin({ right: 10 })

            // 编辑按钮
            Button() {
              Stack({ alignContent: Alignment.Center }) {
                Column() {}
                .width(40)
                .height(40)
                .borderRadius(20)
                .backgroundColor(Color.White)
                .shadow({
                  radius: 2,
                  color: '#00000010',
                  offsetX: 0,
                  offsetY: 1
                })

                Text('✎')
                  .fontSize(24)
                  .fontWeight(FontWeight.Medium)
                  .fontColor('#444')
              }
            }
            .width(50)
            .height(50)
            .backgroundColor(Color.Transparent)
            .onClick(() => {
              this.startEditing();
            })
          }
        } else {
          // 编辑模式下显示保存按钮
          Row() {
            // 取消按钮
            Button() {
              Stack({ alignContent: Alignment.Center }) {
                Column() {}
                .width(40)
                .height(40)
                .borderRadius(20)
                .backgroundColor(Color.White)
                .shadow({
                  radius: 2,
                  color: '#00000010',
                  offsetX: 0,
                  offsetY: 1
                })

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
              this.cancelEditing();
            })
            .margin({ right: 10 })

            // 保存按钮
            Button() {
              Stack({ alignContent: Alignment.Center }) {
                Column() {}
                .width(40)
                .height(40)
                .borderRadius(20)
                .backgroundColor(this.cardData.name ? '#007DFF' : '#d1d1d6')
                .shadow({
                  radius: 2,
                  color: this.cardData.name ? '#007DFF30' : '#00000010',
                  offsetX: 0,
                  offsetY: 1
                })

                Text('✓')
                  .fontSize(24)
                  .fontWeight(FontWeight.Medium)
                  .fontColor(Color.White)
              }
            }
            .width(50)
            .height(50)
            .backgroundColor(Color.Transparent)
            .enabled(!!this.cardData.name && !this.isSaving)
            .onClick(() => {
              this.saveBusinessCard();
            })
            .opacity(this.cardData.name ? 1 : 0.6)
          }
        }
      }
      .width('100%')
      .padding({ left: 12, right: 12, top: 20, bottom: 20 })

      // 内容区域
      Scroll() {
        Column() {
          // 保存成功提示
          if (this.showSuccessMessage) {
            Row() {
              Text('保存成功！')
                .fontSize(14)
                .fontColor('#34C759')
                .textAlign(TextAlign.Center)
                .layoutWeight(1)
            }
            .width('100%')
            .padding(10)
            .backgroundColor('#E8F5E9')
            .borderRadius(5)
            .margin({ bottom: 15, left: 20, right: 20 })
          }

          // 加载状态
          if (this.isSaving) {
            Row() {
              LoadingProgress()
                .width(30)
                .height(30)
              Text('正在保存...')
                .fontSize(14)
                .margin({ left: 10 })
                .fontColor('#666')
            }
            .width('100%')
            .justifyContent(FlexAlign.Center)
            .margin({ bottom: 20 })
          }

          // 头像区域
          Column() {
            Text(this.cardData.name ? this.cardData.name.charAt(0).toUpperCase() : '?')
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

          // 名片信息区域
          Column() {
            // 个人信息标题
            Text('个人信息')
              .fontSize(16)
              .fontWeight(FontWeight.Medium)
              .textAlign(TextAlign.Start)
              .width('100%')
              .margin({ bottom: 15 })

            // 姓名 - 在编辑模式下使用TextInput
            Column() {
              Text('姓名')
                .fontSize(14)
                .fontColor('#666')
                .textAlign(TextAlign.Start)
                .width('100%')
                .margin({ bottom: 5 })

              if (this.isEditing) {
                TextInput({ text: this.cardData.name, placeholder: '请输入姓名' })
                  .fontSize(16)
                  .width('100%')
                  .backgroundColor('#f8f8f8')
                  .borderRadius(5)
                  .padding({ left: 10, right: 10 })
                  .height(50)
                  .onChange((text: string) => {
                    this.cardData.name = text;
                  })
              } else {
                Text(this.cardData.name || '未设置')
                  .fontSize(16)
                  .fontColor(this.cardData.name ? Color.Black : '#999')
                  .textAlign(TextAlign.Start)
                  .width('100%')
              }
            }
            .width('100%')
            .backgroundColor('#f8f8f8')
            .borderRadius(8)
            .padding(15)
            .margin({ bottom: 10 })

            // 电话 - 在编辑模式下使用TextInput
            Column() {
              Text('电话')
                .fontSize(14)
                .fontColor('#666')
                .textAlign(TextAlign.Start)
                .width('100%')
                .margin({ bottom: 5 })

              if (this.isEditing) {
                TextInput({ text: this.cardData.phone, placeholder: '请输入电话' })
                  .fontSize(16)
                  .width('100%')
                  .backgroundColor('#f8f8f8')
                  .borderRadius(5)
                  .padding({ left: 10, right: 10 })
                  .height(50)
                  .onChange((text: string) => {
                    this.cardData.phone = text;
                  })
              } else {
                Text(this.cardData.phone || '未设置')
                  .fontSize(16)
                  .fontColor(this.cardData.phone ? Color.Black : '#999')
                  .textAlign(TextAlign.Start)
                  .width('100%')
              }
            }
            .width('100%')
            .backgroundColor('#f8f8f8')
            .borderRadius(8)
            .padding(15)
            .margin({ bottom: 10 })

            // 邮箱 - 在编辑模式下使用TextInput
            Column() {
              Text('邮箱')
                .fontSize(14)
                .fontColor('#666')
                .textAlign(TextAlign.Start)
                .width('100%')
                .margin({ bottom: 5 })

              if (this.isEditing) {
                TextInput({ text: this.cardData.email, placeholder: '请输入邮箱' })
                  .fontSize(16)
                  .width('100%')
                  .backgroundColor('#f8f8f8')
                  .borderRadius(5)
                  .padding({ left: 10, right: 10 })
                  .height(50)
                  .onChange((text: string) => {
                    this.cardData.email = text;
                  })
              } else {
                Text(this.cardData.email || '未设置')
                  .fontSize(16)
                  .fontColor(this.cardData.email ? Color.Black : '#999')
                  .textAlign(TextAlign.Start)
                  .width('100%')
              }
            }
            .width('100%')
            .backgroundColor('#f8f8f8')
            .borderRadius(8)
            .padding(15)
            .margin({ bottom: 10 })

            // 公司信息标题
            Text('公司信息')
              .fontSize(16)
              .fontWeight(FontWeight.Medium)
              .textAlign(TextAlign.Start)
              .width('100%')
              .margin({ top: 20, bottom: 15 })

            // 公司 - 在编辑模式下使用TextInput
            Column() {
              Text('公司')
                .fontSize(14)
                .fontColor('#666')
                .textAlign(TextAlign.Start)
                .width('100%')
                .margin({ bottom: 5 })

              if (this.isEditing) {
                TextInput({ text: this.cardData.company, placeholder: '请输入公司' })
                  .fontSize(16)
                  .width('100%')
                  .backgroundColor('#f8f8f8')
                  .borderRadius(5)
                  .padding({ left: 10, right: 10 })
                  .height(50)
                  .onChange((text: string) => {
                    this.cardData.company = text;
                  })
              } else {
                Text(this.cardData.company || '未设置')
                  .fontSize(16)
                  .fontColor(this.cardData.company ? Color.Black : '#999')
                  .textAlign(TextAlign.Start)
                  .width('100%')
              }
            }
            .width('100%')
            .backgroundColor('#f8f8f8')
            .borderRadius(8)
            .padding(15)
            .margin({ bottom: 10 })

            // 职位 - 在编辑模式下使用TextInput
            Column() {
              Text('职位')
                .fontSize(14)
                .fontColor('#666')
                .textAlign(TextAlign.Start)
                .width('100%')
                .margin({ bottom: 5 })

              if (this.isEditing) {
                TextInput({ text: this.cardData.position, placeholder: '请输入职位' })
                  .fontSize(16)
                  .width('100%')
                  .backgroundColor('#f8f8f8')
                  .borderRadius(5)
                  .padding({ left: 10, right: 10 })
                  .height(50)
                  .onChange((text: string) => {
                    this.cardData.position = text;
                  })
              } else {
                Text(this.cardData.position || '未设置')
                  .fontSize(16)
                  .fontColor(this.cardData.position ? Color.Black : '#999')
                  .textAlign(TextAlign.Start)
                  .width('100%')
              }
            }
            .width('100%')
            .backgroundColor('#f8f8f8')
            .borderRadius(8)
            .padding(15)
            .margin({ bottom: 10 })

            // 部门 - 在编辑模式下使用TextInput
            Column() {
              Text('部门')
                .fontSize(14)
                .fontColor('#666')
                .textAlign(TextAlign.Start)
                .width('100%')
                .margin({ bottom: 5 })

              if (this.isEditing) {
                TextInput({ text: this.cardData.department, placeholder: '请输入部门' })
                  .fontSize(16)
                  .width('100%')
                  .backgroundColor('#f8f8f8')
                  .borderRadius(5)
                  .padding({ left: 10, right: 10 })
                  .height(50)
                  .onChange((text: string) => {
                    this.cardData.department = text;
                  })
              } else {
                Text(this.cardData.department || '未设置')
                  .fontSize(16)
                  .fontColor(this.cardData.department ? Color.Black : '#999')
                  .textAlign(TextAlign.Start)
                  .width('100%')
              }
            }
            .width('100%')
            .backgroundColor('#f8f8f8')
            .borderRadius(8)
            .padding(15)
            .margin({ bottom: 10 })

            // 地址 - 在编辑模式下使用TextInput
            Column() {
              Text('地址')
                .fontSize(14)
                .fontColor('#666')
                .textAlign(TextAlign.Start)
                .width('100%')
                .margin({ bottom: 5 })

              if (this.isEditing) {
                TextInput({ text: this.cardData.address, placeholder: '请输入地址' })
                  .fontSize(16)
                  .width('100%')
                  .backgroundColor('#f8f8f8')
                  .borderRadius(5)
                  .padding({ left: 10, right: 10 })
                  .height(50)
                  .onChange((text: string) => {
                    this.cardData.address = text;
                  })
              } else {
                Text(this.cardData.address || '未设置')
                  .fontSize(16)
                  .fontColor(this.cardData.address ? Color.Black : '#999')
                  .textAlign(TextAlign.Start)
                  .width('100%')
              }
            }
            .width('100%')
            .backgroundColor('#f8f8f8')
            .borderRadius(8)
            .padding(15)
            .margin({ bottom: 10 })

            // 网站 - 在编辑模式下使用TextInput
            Column() {
              Text('网站')
                .fontSize(14)
                .fontColor('#666')
                .textAlign(TextAlign.Start)
                .width('100%')
                .margin({ bottom: 5 })

              if (this.isEditing) {
                TextInput({ text: this.cardData.website, placeholder: '请输入网站' })
                  .fontSize(16)
                  .width('100%')
                  .backgroundColor('#f8f8f8')
                  .borderRadius(5)
                  .padding({ left: 10, right: 10 })
                  .height(50)
                  .onChange((text: string) => {
                    this.cardData.website = text;
                  })
              } else {
                Text(this.cardData.website || '未设置')
                  .fontSize(16)
                  .fontColor(this.cardData.website ? Color.Black : '#999')
                  .textAlign(TextAlign.Start)
                  .width('100%')
              }
            }
            .width('100%')
            .backgroundColor('#f8f8f8')
            .borderRadius(8)
            .padding(15)
            .margin({ bottom: 10 })

            // 备注 - 在编辑模式下使用TextInput
            Column() {
              Text('备注')
                .fontSize(14)
                .fontColor('#666')
                .textAlign(TextAlign.Start)
                .width('100%')
                .margin({ bottom: 5 })

              if (this.isEditing) {
                TextInput({ text: this.cardData.notes, placeholder: '请输入备注' })
                  .fontSize(16)
                  .width('100%')
                  .backgroundColor('#f8f8f8')
                  .borderRadius(5)
                  .padding({ left: 10, right: 10 })
                  .height(50)
                  .onChange((text: string) => {
                    this.cardData.notes = text;
                  })
              } else {
                Text(this.cardData.notes || '未设置')
                  .fontSize(16)
                  .fontColor(this.cardData.notes ? Color.Black : '#999')
                  .textAlign(TextAlign.Start)
                  .width('100%')
              }
            }
            .width('100%')
            .backgroundColor('#f8f8f8')
            .borderRadius(8)
            .padding(15)
            .margin({ bottom: 10 })

            // 显示设置
            Row() {
              Text('在联系人列表中显示')
                .fontSize(16)
                .layoutWeight(1)

              Toggle({ type: ToggleType.Switch, isOn: this.cardData.showInList })
                .onChange((isOn: boolean) => {
                  this.cardData.showInList = isOn;
                })
                .enabled(this.isEditing)
            }
            .width('100%')
            .padding(15)
            .backgroundColor(Color.White)
            .borderRadius(8)
            .margin({ bottom: 10 })

            // 操作按钮区域
            if (!this.isEditing && this.cardData.name && this.cardData.name !== '我的名片') {
              Column() {
                Button('分享名片')
                  .width('100%')
                  .height(45)
                  .backgroundColor('#007DFF')
                  .fontColor(Color.White)
                  .borderRadius(8)
                  .onClick(() => {
                    this.shareBusinessCard();
                  })

                Button('删除名片')
                  .width('100%')
                  .height(45)
                  .backgroundColor('#ff3b30')
                  .fontColor(Color.White)
                  .borderRadius(8)
                  .margin({ top: 10 })
                  .onClick(() => {
                    this.deleteBusinessCard();
                  })
              }
              .width('100%')
              .margin({ top: 20 })
            }
          }
          .width('100%')
          .padding(20)
          .backgroundColor(Color.White)
          .borderRadius(10)
          .shadow({ radius: 2, color: '#10000000', offsetX: 0, offsetY: 1 })
          .margin({ left: 20, right: 20 })

          // 使用说明
          if (!this.isEditing) {
            Column() {
              Text('使用说明')
                .fontSize(16)
                .fontWeight(FontWeight.Medium)
                .textAlign(TextAlign.Start)
                .width('100%')
                .margin({ bottom: 10 })

              Text('1. 编辑并保存您的名片信息\n' +
                '2. 名片信息将自动保存\n' +
                '3. 您可以分享名片给他人\n' +
                '4. 如需添加到系统通讯录，请手动操作')
                .fontSize(14)
                .fontColor('#666')
                .textAlign(TextAlign.Start)
                .width('100%')
                .lineHeight(22)
            }
            .width('100%')
            .padding(20)
            .backgroundColor('#F8F8F8')
            .borderRadius(10)
            .margin({ top: 20, left: 20, right: 20, bottom: 20 })
          }
        }
        .width('100%')
      }
      .layoutWeight(1)
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#f5f5f5')
  }
}