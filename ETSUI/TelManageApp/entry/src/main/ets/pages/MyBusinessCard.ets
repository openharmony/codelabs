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

  // 开始编辑
  startEditing() {
    this.isEditing = true;
    console.log('MyBusinessCard: 进入编辑模式');
  }

  // 取消编辑
  cancelEditing() {
    this.isEditing = false;
    // 重新从LocalStorage加载数据 - 重置为当前值来触发渲染
    // 通过重新赋值来触发更新
    const currentName = this.cardData.name;
    this.cardData.name = currentName;
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
            Column() {
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
                Column() {
                }
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
                Column() {
                }
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
                Column() {
                }
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
                Column() {
                }
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
      .padding({
        left: 12,
        right: 12,
        top: 20,
        bottom: 20
      })

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

            // 姓名
            this.buildCardItem('姓名', this.cardData.name, this.isEditing, (value) => {
              this.cardData.name = value;
            })

            // 电话
            this.buildCardItem('电话', this.cardData.phone, this.isEditing, (value) => {
              this.cardData.phone = value;
            })

            // 邮箱
            this.buildCardItem('邮箱', this.cardData.email, this.isEditing, (value) => {
              this.cardData.email = value;
            })

            // 公司信息标题
            Text('公司信息')
              .fontSize(16)
              .fontWeight(FontWeight.Medium)
              .textAlign(TextAlign.Start)
              .width('100%')
              .margin({ top: 20, bottom: 15 })

            // 公司
            this.buildCardItem('公司', this.cardData.company, this.isEditing, (value) => {
              this.cardData.company = value;
            })

            // 职位
            this.buildCardItem('职位', this.cardData.position, this.isEditing, (value) => {
              this.cardData.position = value;
            })

            // 部门
            this.buildCardItem('部门', this.cardData.department, this.isEditing, (value) => {
              this.cardData.department = value;
            })

            // 地址
            this.buildCardItem('地址', this.cardData.address, this.isEditing, (value) => {
              this.cardData.address = value;
            })

            // 网站
            this.buildCardItem('网站', this.cardData.website, this.isEditing, (value) => {
              this.cardData.website = value;
            })

            // 备注
            this.buildCardItem('备注', this.cardData.notes, this.isEditing, (value) => {
              this.cardData.notes = value;
            })

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
          .shadow({
            radius: 2,
            color: '#10000000',
            offsetX: 0,
            offsetY: 1
          })
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
            .margin({
              top: 20,
              left: 20,
              right: 20,
              bottom: 20
            })
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

  // 构建名片信息项
  @Builder
  buildCardItem(label: string, value: string, editable: boolean, onChange: (value: string) => void) {
    Column() {
      if (!editable) {
        // 预览模式
        Column() {
          Text(label)
            .fontSize(14)
            .fontColor('#666')
            .textAlign(TextAlign.Start)
            .width('100%')
            .margin({ bottom: 5 })

          Text(value || '未设置')
            .fontSize(16)
            .fontColor(value ? Color.Black : '#999')
            .textAlign(TextAlign.Start)
            .width('100%')
        }
        .padding(15)
      } else {
        // 编辑模式
        Column() {
          Text(label)
            .fontSize(14)
            .fontColor('#666')
            .textAlign(TextAlign.Start)
            .width('100%')
            .margin({ bottom: 5 })

          TextInput({ text: value, placeholder: `请输入${label}` })
            .fontSize(16)
            .width('100%')
            .backgroundColor('#f8f8f8')
            .borderRadius(5)
            .padding({ left: 10, right: 10 })
            .height(50)
            .onChange((text: string) => {
              onChange(text);
            })
        }
        .padding(15)
      }
    }
    .width('100%')
    .backgroundColor('#f8f8f8')
    .borderRadius(8)
    .margin({ bottom: 10 })
  }
}