// entry/src/main/ets/pages/ContactListPage.ets
import { Contact, ContactModel } from '../model/ContactModel';
import { Logger } from '../common/utils/Logger';

const logger = new Logger('ContactListPage');

@Component
export struct ContactListPage {
  @State contacts: Array<Contact> = [];
  // ✅ [新增] 搜索关键词状态
  @State keyword: string = '';

  @Consume('pageStack') pageStack: NavPathStack;

  dialogController: CustomDialogController = new CustomDialogController({
    builder: AddContactDialog({
      onConfirm: (name: string, phone: string, relation: string) => {
        this.addContact(name, phone, relation);
      }
    }),
    alignment: DialogAlignment.Bottom,
    autoCancel: true
  });

  aboutToAppear(): void {
    void this.loadContacts();
  }

  get contactCountInfo(): Resource {
    const count: number = this.contacts.length;
    return $r('app.plural.contact_count_info', count, count);
  }

  private getAvatarText(name: string): string {
    const safe: string = (name ?? '').trim();
    return safe.length > 0 ? safe.substring(0, 1) : '?';
  }

  // ✅ [修改] 加载逻辑支持搜索
  async loadContacts(): Promise<void> {
    try {
      if (this.keyword && this.keyword.trim().length > 0) {
        // 搜索模式
        const list = await ContactModel.search(this.keyword);
        this.contacts = Array.isArray(list) ? list : [];
      } else {
        // 默认模式
        const list = await ContactModel.queryAll();
        this.contacts = Array.isArray(list) ? list : [];
      }
    } catch {
      logger.error('loadContacts failed');
      this.contacts = [];
    }
  }

  async addContact(name: string, phone: string, relation: string): Promise<void> {
    if (!name || !phone) return;
    const newContact: Contact = new Contact(name, phone, relation);
    if (await ContactModel.insert(newContact) > 0) {
      // 添加成功后，清空搜索并刷新
      this.keyword = '';
      await this.loadContacts();
    }
  }

  async deleteContact(index: number, id: number): Promise<void> {
    if (await ContactModel.deleteById(id)) {
      this.contacts.splice(index, 1);
    }
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
        .fontSize(24)
        .fontWeight(FontWeight.Bold)

      Blank()

      Button({ type: ButtonType.Circle, stateEffect: true }) {
        Text('+')
          .fontSize(24)
          .fontColor(Color.Black)
      }
      .width(40)
      .height(40)
      .backgroundColor(Color.Transparent)
      .onClick(() => {
        this.dialogController.open();
      })
    }
    .width('100%')
    .height(56)
    .padding({ left: 16, right: 16 })
    .alignItems(VerticalAlign.Center)
  }

  // ✅ [新增] 搜索栏组件
  @Builder
  private SearchBar(): void {
    Row() {
      Search({ value: this.keyword, placeholder: '搜索姓名、关系...' }) // 这里的 placeholder 建议稍后放入资源文件
        .width('100%')
        .height(40)
        .backgroundColor(Color.White)
        .placeholderColor(Color.Gray)
        .placeholderFont({ size: 14 })
        .textFont({ size: 14 })
        .onChange((value: string) => {
          this.keyword = value;
          // 实时搜索 (如果数据量大建议加防抖，目前直接查)
          this.loadContacts();
        })
        .onSubmit((value: string) => {
          this.keyword = value;
          this.loadContacts();
        })
    }
    .width('100%')
    .padding({ left: 16, right: 16, bottom: 8 })
  }

  @Builder
  private ContactItem(item: Contact, index: number): void {
    ListItem() {
      Row() {
        Stack({ alignContent: Alignment.Center }) {
          Circle({ width: 40, height: 40 }).fill('#E5F1FF')
          Text(this.getAvatarText(item.name))
            .fontSize(16)
            .fontColor('#007DFF')
        }

        Column() {
          Text(item.name ?? '')
            .fontSize(16)
            .fontWeight(FontWeight.Medium)
            .maxLines(1)
            .textOverflow({ overflow: TextOverflow.Ellipsis })

          Text(`${item.relation ?? ''} | ${item.phone ?? ''}`)
            .fontSize(14)
            .fontColor('#999999')
            .margin({ top: 4 })
            .maxLines(1)
            .textOverflow({ overflow: TextOverflow.Ellipsis })
        }
        .alignItems(HorizontalAlign.Start)
        .margin({ left: 12 })
        .layoutWeight(1)
      }
      .width('100%')
      .padding(12)
      .backgroundColor(Color.White)
      .borderRadius(12)
      .constraintSize({ minHeight: 48 })
      .onClick(() => {
        this.pageStack.pushPath({ name: 'ContactDetailPage', param: item });
      })
    }
    .swipeAction({
      end: () => {
        this.DeleteAction(index, item.id);
      }
    })
  }

  build() {
    Column() {
      this.CustomHeader()

      // ✅ [新增] 插入搜索栏
      this.SearchBar()

      GridRow({
        columns: { sm: 4, md: 8, lg: 12 },
        gutter: { x: 12, y: 12 }
      }) {
        GridCol({ span: { sm: 4, md: 6, lg: 8 }, offset: { sm: 0, md: 1, lg: 2 } }) {
          Column() {
            if (this.contacts.length === 0) {
              Column() {
                // 根据是否在搜索状态显示不同提示
                if (this.keyword.length > 0) {
                  Text('无搜索结果')
                    .fontColor(Color.Gray)
                    .fontSize(16)
                    .margin({ top: 100 })
                } else {
                  Text(this.contactCountInfo)
                    .fontColor(Color.Gray)
                    .fontSize(16)
                    .margin({ top: 100 })

                  Text($r('app.string.contact_empty_tip'))
                    .fontColor(Color.Gray)
                    .fontSize(14)
                    .margin({ top: 10 })
                }
              }
              .width('100%')
              .height('100%')
              .alignItems(HorizontalAlign.Center)
            } else {
              List({ space: 10 }) {
                ForEach(this.contacts, (item: Contact, index: number) => {
                  this.ContactItem(item, index);
                }, (item: Contact) => item.id.toString())
              }
              .width('100%')
              .layoutWeight(1)
              .padding({ left: 16, right: 16, bottom: 10 })
            }
          }
          .width('100%')
          .height('100%')
        }
      }
      .layoutWeight(1)
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F1F3F5')
  }
}

// Dialog 代码保持不变...
@CustomDialog
struct AddContactDialog {
  controller?: CustomDialogController;
  onConfirm: (name: string, phone: string, relation: string) => void = () => {};
  @State name: string = '';
  @State phone: string = '';
  @State relation: string = '';

  build() {
    Column() {
      Text($r('app.string.dialog_add_contact_title'))
        .fontSize(20)
        .fontWeight(FontWeight.Bold)
        .margin({ top: 20, bottom: 20 })

      TextInput({ placeholder: $r('app.string.input_name') }).onChange((v) => { this.name = v; }).margin({ bottom: 12 }).width('90%')
      TextInput({ placeholder: $r('app.string.input_phone') }).onChange((v) => { this.phone = v; }).margin({ bottom: 12 }).width('90%')
      TextInput({ placeholder: $r('app.string.input_relation') }).onChange((v) => { this.relation = v; }).margin({ bottom: 20 }).width('90%')

      Row() {
        Button($r('app.string.btn_cancel')).backgroundColor(Color.Transparent).fontColor(Color.Gray).onClick(() => { this.controller?.close(); })
        Button($r('app.string.btn_add')).onClick(() => { this.onConfirm(this.name, this.phone, this.relation); this.controller?.close(); })
      }
      .justifyContent(FlexAlign.SpaceEvenly).width('100%').margin({ bottom: 20 })
    }
    .backgroundColor(Color.White).borderRadius(24).width('100%')
  }
}