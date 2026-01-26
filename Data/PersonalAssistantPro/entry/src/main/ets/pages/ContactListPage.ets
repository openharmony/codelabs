/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
// src/main/ets/pages/ContactListPage.ets
import { Contact, ContactModel } from '../model/ContactModel';
import { ContactViewModel } from '../viewmodel/ContactViewModel'; // 📝 [新增] 引入 ViewModel
import { Logger } from '../common/utils/Logger';
import { emitter } from '@kit.BasicServicesKit';
import promptAction from '@ohos.promptAction'; // 📝 [新增] 用于弹窗提示
import { CommonConstants } from '../common/constants/CommonConstants';

const logger = new Logger('ContactListPage');
const EVENT_ID_REFRESH = 1001;

@Component
export struct ContactListPage {
  @State contacts: Array<Contact> = [];
  @State keyword: string = '';
  // 列表版本号，用于辅助强制刷新
  @State listVersion: number = 0;
  @Consume('pageStack') pageStack: NavPathStack;
  dialogController: CustomDialogController = new CustomDialogController({
    builder: AddContactDialog({
      onConfirm: (name: string, phone: string, relation: string, email: string) => {
        this.addContact(name, phone, relation, email);
      }
    }),
    alignment: DialogAlignment.Bottom,
    autoCancel: true
  });
  // 📝 [新增] 实例化 ViewModel
  private contactViewModel = new ContactViewModel();

  get contactCountInfo(): Resource {
    const count: number = this.contacts.length;
    return $r('app.plural.contact_count_info', count, count);
  }

  aboutToAppear(): void {
    logger.info('【生命周期】ContactListPage aboutToAppear');
    this.loadContacts();

    logger.info(`【事件】注册监听 ID: ${CommonConstants.EVENT_ID_REFRESH}`);
    emitter.on({ eventId: CommonConstants.EVENT_ID_REFRESH }, () => {
      logger.info('【事件】收到刷新信号，延迟 50ms 执行刷新...');
      setTimeout(() => {
        this.loadContacts();
      }, 50);
    });
  }

  aboutToDisappear(): void {
    logger.info('【生命周期】ContactListPage aboutToDisappear');
    emitter.off(CommonConstants.EVENT_ID_REFRESH);
  }

  async loadContacts(): Promise<void> {
    logger.info('【数据】正在从数据库加载...');
    try {
      let list: Contact[];
      if (this.keyword && this.keyword.trim().length > 0) {
        list = await ContactModel.search(this.keyword);
      } else {
        list = await ContactModel.queryAll();
      }

      this.contacts = [];
      setTimeout(() => {
        this.contacts = Array.isArray(list) ? list : [];
        this.listVersion++;
        logger.info(`【数据】刷新完成，当前条数: ${this.contacts.length}`);
      }, 0);

    } catch (err) {
      logger.error(`【数据】加载异常: ${JSON.stringify(err)}`);
      this.contacts = [];
    }
  }

  async addContact(name: string, phone: string, relation: string, email: string): Promise<void> {
    // 📝 [修改] 使用 ViewModel 进行国际化校验
    const errorKey = this.contactViewModel.validateContact(name, phone);
    if (errorKey) {
      // 动态拼接 app.string.xxx
      promptAction.showToast({ message: $r(`app.string.${errorKey}`) });
      return;
    }

    // 📝 [修改] 处理默认关系 (国际化)
    // 如果用户没填关系，手动获取 "朋友" 的本地化字符串
    let finalRelation = relation;
    if (!finalRelation || finalRelation.trim() === '') {
      try {
        finalRelation = getContext(this).resourceManager.getStringSync($r('app.string.contact_default_relation').id);
      } catch (e) {
        finalRelation = 'Friend'; // 兜底
      }
    }

    const newContact: Contact = new Contact(name, phone, finalRelation, email);
    if (await ContactModel.insert(newContact) > 0) {
      this.keyword = '';
      await this.loadContacts();
      promptAction.showToast({ message: $r('app.string.toast_add_success') });
    }
  }

  async deleteContact(index: number, id: number): Promise<void> {
    if (await ContactModel.deleteById(id)) {
      this.contacts.splice(index, 1);
    }
  }

  build() {
    Column() {
      this.CustomHeader()
      this.SearchBar()

      GridRow({ columns: { sm: 4, md: 8, lg: 12 }, gutter: { x: 12, y: 12 } }) {
        GridCol({ span: { sm: 4, md: 6, lg: 8 }, offset: { sm: 0, md: 1, lg: 2 } }) {
          Column() {
            if (this.contacts.length === 0) {
              Column() {
                Text($r('app.string.contact_empty_list'))
                  .fontColor(Color.Gray)
                  .fontSize(16)
                  .margin({ top: 100 })
              }
              .width('100%').alignItems(HorizontalAlign.Center)
            } else {
              List({ space: 10 }) {
                ForEach(this.contacts, (item: Contact, index: number) => {
                  this.ContactItem(item, index);
                }, (item: Contact) => {
                  return JSON.stringify(item);
                })
              }
              .width('100%').layoutWeight(1).padding({ left: 16, right: 16, bottom: 10 })
            }
          }
          .width('100%').height('100%')
        }
      }
      .layoutWeight(1)
    }
    .width('100%').height('100%').backgroundColor('#F1F3F5')
  }

  private getAvatarText(name: string): string {
    const safe: string = (name ?? '').trim();
    return safe.length > 0 ? safe.substring(0, 1) : '?';
  }

  @Builder
  private DeleteAction(index: number, id: number): void {
    Row() {
      Button($r('app.string.btn_delete'))
        .type(ButtonType.Normal)
        .backgroundColor(Color.Red)
        .fontColor(Color.White)
        .constraintSize({ minWidth: 80, minHeight: 48 })
        .onClick(() => {
          void this.deleteContact(index, id);
        })
    }
    .height('100%')
    .justifyContent(FlexAlign.End)
  }

  @Builder
  private CustomHeader(): void {
    Row() {
      Text($r('app.string.contact_title'))
        .fontSize(24).fontWeight(FontWeight.Bold)
      Blank()
      Button({ type: ButtonType.Circle, stateEffect: true }) {
        Text('+').fontSize(24).fontColor(Color.Black)
      }
      .width(40).height(40).backgroundColor(Color.Transparent)
      .onClick(() => {
        this.dialogController.open();
      })
    }
    .width('100%').height(56).padding({ left: 16, right: 16 }).alignItems(VerticalAlign.Center)
  }

  @Builder
  private SearchBar(): void {
    Row() {
      Search({ value: this.keyword, placeholder: $r('app.string.contact_search_placeholder') })
        .width('100%').height(40).backgroundColor(Color.White)
        .onChange((value: string) => {
          this.keyword = value;
          this.loadContacts();
        })
    }
    .width('100%').padding({ left: 16, right: 16, bottom: 8 })
  }

  @Builder
  private ContactItem(item: Contact, index: number): void {
    ListItem() {
      Row() {
        Stack({ alignContent: Alignment.Center }) {
          Circle({ width: 40, height: 40 }).fill('#E5F1FF')
          Text(this.getAvatarText(item.name)).fontSize(16).fontColor('#007DFF')
        }

        Column() {
          Text(item.name ?? '').fontSize(16).fontWeight(FontWeight.Medium)
          // 📝 [修改] 使用 Resource 格式化
          // Text(`${item.relation ?? ''} | ${item.phone ?? ''}`) 改为使用资源模板
          Text($r('app.string.contact_relation_phone_format', item.relation ?? '', item.phone ?? ''))
            .fontSize(14).fontColor('#999999').margin({ top: 4 })
        }
        .alignItems(HorizontalAlign.Start).margin({ left: 12 }).layoutWeight(1)
      }
      .width('100%')
      .padding(12)
      .backgroundColor(Color.White)
      .borderRadius(12)
      .onClick(() => {
        logger.info(`【操作】进入详情页: ${item.name}`);
        this.pageStack.pushPath({ name: 'ContactDetailPage', param: item });
      })
    }
    .swipeAction({
      end: () => {
        this.DeleteAction(index, item.id);
      }
    })
  }
}

@CustomDialog
struct AddContactDialog {
  controller?: CustomDialogController;
  @State name: string = '';
  @State phone: string = '';
  @State relation: string = '';
  @State email: string = '';
  onConfirm: (name: string, phone: string, relation: string, email: string) => void = () => {
  };

  build() {
    Column() {
      Text($r('app.string.contact_btn_add')).fontSize(20).fontWeight(FontWeight.Bold).margin({ top: 20, bottom: 20 })
      TextInput({ placeholder: $r('app.string.contact_label_name') }).onChange((v) => {
        this.name = v;
      }).margin({ bottom: 12 }).width('90%')
      TextInput({ placeholder: $r('app.string.contact_label_phone') }).onChange((v) => {
        this.phone = v;
      }).margin({ bottom: 12 }).width('90%')
      TextInput({ placeholder: $r('app.string.contact_label_email') }).onChange((v) => {
        this.email = v;
      }).margin({ bottom: 12 }).width('90%')
      TextInput({ placeholder: $r('app.string.contact_label_relation') }).onChange((v) => {
        this.relation = v;
      }).margin({ bottom: 20 }).width('90%')
      Row() {
        Button($r('app.string.common_cancel')).backgroundColor(Color.Transparent).fontColor(Color.Gray).onClick(() => {
          this.controller?.close();
        })
        Button($r('app.string.common_add')).onClick(() => {
          this.onConfirm(this.name, this.phone, this.relation, this.email);
          this.controller?.close();
        })
      }
      .justifyContent(FlexAlign.SpaceEvenly).width('100%').margin({ bottom: 20 })
    }
    .backgroundColor(Color.White).borderRadius(24).width('100%')
  }
}