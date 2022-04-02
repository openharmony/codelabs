import router from "@system.router";
import {contact} from "@ohos/vcard";
import {VCardBuilder} from "@ohos/vcard";
import {VCardConfig} from "@ohos/vcard";

@Entry
@Component
struct FlexExample {
  private scroller: Scroller = new Scroller() //滚动
  @State name: string= "zhangsan"//router.getParams() == null ? "姓名"
  @State OrganizationName: string= "chengmai"
  @State phonename: string= "13537292684"//电话号码
  @State emails : string= "qq.q@qq.com"//电子邮件
  @State note : string= "程序员苦逼"//备注
  @State PostalAddreSSA  : string= "wuhan"//地址
  @State website : string= "baidu"//网站
  @State EventA: string="20000000" //生日
  @State index: string= '手机'//手机下拉框
  @State index1: string= '私人'//邮箱下拉框
  @State index2: string= '住宅'//住宅下拉框
  @State fd: number = 0

  build() {
    Column({space:5}) {
      Scroll(this.scroller) {
        Column({ space: 5 }) {
          Stack() {
            Text('编辑联系人')
              .fontSize(30)
              .fontWeight(FontWeight.Bold)
              .align(Alignment.Center)
            Stack({ alignContent: Alignment.End }) {
              Button("保存",{ type: ButtonType.Normal, stateEffect: true })
                .borderRadius(12)
                .backgroundColor(0x317aff)
                .fontSize(30)
                .margin({right:12})
                .onClick(() =>
                {
                  console.info('onClick start')
                  //姓名模块
                  let name = new contact.Name()
                  let contactall = new contact.Contact()
                  contactall.name = name
                  contactall.name.fullName = this.name // 将名字存入name类中fullname

                  //单位
                  let Organization = new contact.Organization()
                  contactall.organization = Organization
                  contactall.organization.name = this.OrganizationName

                  //手机模块12
                  let phoneNumber = new contact.PhoneNumber()
                  phoneNumber.labelName = this.index
                  phoneNumber.labelId = parseInt(this.index)
                  phoneNumber.phoneNumber = this.phonename
                  contactall.phoneNumbers = [phoneNumber] //只能传一个，不能更改或者添加

                  //邮件模块
                  let Email = new contact.Email()
                  Email.labelName = this.index1
                  Email.labelId = parseInt(this.index1)
                  Email.email = this.emails
                  contactall.emails = [Email]

                  //备注
                  let note = new contact.Note()
                  contactall.note = note
                  contactall.note.noteContent = this.note

                  //住宅地址
                  let PostalAddreSS = new contact.PostalAddress()
                  PostalAddreSS.labelId = parseInt(this.index2)
                  PostalAddreSS.region = this.PostalAddreSSA
                  contactall.postalAddresses = [PostalAddreSS]

                  //网站
                  let website = new contact.Website()
                  contactall.websites = [website]
                  contactall.websites[0].website = this.website

                  //生日
                  let event = new contact.Event()
                  event.eventDate = this.EventA
                  contactall.events = [event]

                  let vcardBuilder = new VCardBuilder(VCardConfig.VCARD_TYPE_V21_GENERIC,"UTF-8")
                  vcardBuilder.appendNameProperties(contactall)
                  vcardBuilder.appendOrganizations(contactall)
                  vcardBuilder.appendPhones(contactall,null)
                  vcardBuilder.appendEmails(contactall)
                  vcardBuilder.appendNotes(contactall)
                  vcardBuilder.appendPostals(contactall)
                  vcardBuilder.appendWebsites(contactall)
                  vcardBuilder.appendEvents(contactall)
                  router.push({
                    uri:"pages/there",
                    params: {
                      cvs: vcardBuilder.toString()
                    }
                  })
                })
            }.width('100%')
          }
          .height(50)
          .width('100%')
          //姓名填写
          Flex({ justifyContent: FlexAlign.Start, alignItems: ItemAlign.Center }) { //1
            Image($rawfile('ic_public_contacts.png')) //图片
              .objectFit(ImageFit.Contain)
              .height(30)
              .width(30)
              .margin({top:5})
            TextInput({ placeholder:"输入姓名", text: this.name })
              .placeholderFont({size: "14fp",weight: "100"})
              .height(45)
              .width("100%")
              .backgroundColor(Color.White)
              .onChange((name) => {
                this.name = name
              })
          }
          .height(60)
          .width('90%')
          .padding({left:10})
          .backgroundColor(Color.White)
          .borderStyle(BorderStyle.Dashed)
          .borderWidth(0)
          .borderColor(Color.Red)
          .borderRadius(15)

          //单位
          Flex({ justifyContent: FlexAlign.Start, alignItems: ItemAlign.Center }) {
            Image($rawfile('ic_edit_company.png'))
              .objectFit(ImageFit.Contain)
              .height(30)
              .width(30)
              .margin({ top: 5 })

            TextInput({ placeholder:"公司",  text: this.OrganizationName })
              .placeholderFont({size: "14fp",weight: "100"})
              .height(45)
              .width("100%")
              .onChange((OrganizationName) => {
                this.OrganizationName = OrganizationName
              })
          }
          .height(60)
          .width('90%')
          .padding({left:10})
          .backgroundColor(Color.White)
          .borderStyle(BorderStyle.Dashed)
          .borderWidth(0)
          .borderColor(Color.Red)
          .borderRadius(15)

          //手机
          Flex({ justifyContent: FlexAlign.Start, alignItems: ItemAlign.Center }){
            Flex({ justifyContent: FlexAlign.Start, alignItems: ItemAlign.Center}) {
              Image($rawfile('ic_public_phone.png'))
                .objectFit(ImageFit.Contain)
                .height(30)
                .width(30)
                .margin({ top: 5 })
              Text(this.index)
                .width(80)
                .height(20)
                .fontSize(16)
                .margin({left:5})
            }
            .width('45%')

            //下拉菜单
            TextInput({ placeholder:"电话号码",  text: this.phonename })
              .placeholderFont({size: "14fp",weight: "40"})
              .width('55%')
              .type(InputType.Number)
              .onChange((value) => {
                this.phonename = value
              })
          } .height(60)
          .width('90%')
          .padding({left:10})
          .backgroundColor(Color.White)
          .borderStyle(BorderStyle.Dashed)
          .borderWidth(0)
          .borderColor(Color.Red)
          .borderRadius(15)
          //電子郵件
          Flex({ justifyContent: FlexAlign.Start, alignItems: ItemAlign.Center }){
            Flex({ justifyContent: FlexAlign.Start, alignItems: ItemAlign.Center}){
              Image($rawfile('ic_public_email.png'))
                .objectFit(ImageFit.Contain)
                .height(30)
                .width(30)
                .margin({ top: 5 })
              Text(this.index1)
                .width(80)
                .height(40)
                .fontSize(16)
                .margin({left:5})
            }
            .width('45%')
            //下拉菜单
            TextInput({ placeholder:"电子邮件",  text: this.emails })
              .placeholderFont({size: "14fp",weight: "40"})
              .width('55%')
              .type(InputType.Email)
              .onChange((value) => {
                this.emails = value
              })
          } .height(60)
          .width('90%')
          .padding({left:10})
          .backgroundColor(Color.White)
          .borderStyle(BorderStyle.Dashed)
          .borderWidth(0)
          .borderColor(Color.Red)
          .borderRadius(15)
          //备注
          Flex({ justifyContent: FlexAlign.Start, alignItems: ItemAlign.Center }){
            Flex({ justifyContent: FlexAlign.Start, alignItems: ItemAlign.Center}){
              Image($rawfile('ic_public_drawer.png'))
                .objectFit(ImageFit.Contain)
                .height(30)
                .width(30)
                .margin({ top: 5 })
              Text("备注")
                .width(80)
                .height(40)
                .fontSize(16)
                .margin({left:5})
            }.width('45%')
            TextInput({ placeholder:"备注",  text: this.note })
              .placeholderFont({size: "14fp",weight: "40"})
              .width('55%')
              .onChange((value) => {
                this.note = value
              })
          } .height(60)
          .width('90%')
          .padding({left:10})
          .backgroundColor(Color.White)
          .borderStyle(BorderStyle.Dashed)
          .borderWidth(0)
          .borderColor(Color.Red)
          .borderRadius(15)

          //住宅
          Flex({ justifyContent: FlexAlign.Start, alignItems: ItemAlign.Center }){
            Flex({ justifyContent: FlexAlign.Start, alignItems: ItemAlign.Center}){
              Image($rawfile('ic_public_phone.png'))
                .objectFit(ImageFit.Contain)
                .height(30)
                .width(30)
                .margin({ top: 5 })
              Text(this.index2)
                .width(80)
                .height(40)
                .fontSize(16)
                .margin({left:5})
            }
            .width('45%')
            //下拉菜单
            TextInput({ placeholder:"地址",  text: this.PostalAddreSSA })
              .placeholderFont({size: "14fp",weight: "40"})
              .width('55%')
              .onChange((value) => {
                this.PostalAddreSSA = value
              })
          } .height(60)
          .width('90%')
          .padding({left:10})
          .backgroundColor(Color.White)
          .borderStyle(BorderStyle.Dashed)
          .borderWidth(0)
          .borderColor(Color.Red)
          .borderRadius(15)

          //网址
          Flex({ justifyContent: FlexAlign.Start, alignItems: ItemAlign.Center }){
            Flex({ justifyContent: FlexAlign.Start, alignItems: ItemAlign.Center}) {
              Image($rawfile('ic_public_worldclock.png'))
                .objectFit(ImageFit.Contain)
                .height(30)
                .width(30)
                .margin({ top: 5 })
              Text('网站')
                .width(80)
                .height(40)
                .fontSize(16)
                .margin({left:5})
            }.width('45%')
            //下拉菜单
            TextInput({ placeholder:"网站",  text: this.website })
              .placeholderFont({size: "14fp",weight: "40"})
              .width('55%')
              .onChange((value) => {
                this.website = value
              })
          } .height(60)
          .width('90%')
          .padding({left:10})
          .backgroundColor(Color.White)
          .borderStyle(BorderStyle.Dashed)
          .borderWidth(0)
          .borderColor(Color.Red)
          .borderRadius(15)

          //生日
          Flex({ justifyContent: FlexAlign.Start, alignItems: ItemAlign.Center }){
            Flex({ justifyContent: FlexAlign.Start, alignItems: ItemAlign.Center}){
              Image($rawfile('ic_Edit_Birthday.png'))
                .objectFit(ImageFit.Contain)
                .height(30)
                .width(30)
                .margin({ top: 5 })
              Text('生日')
                .width(80)
                .height(40)
                .fontSize(16)
                .margin({left:5})
            }
            .width('45%')
            //下拉菜单
            TextInput({ placeholder:"日期",  text: this.EventA })
              .placeholderFont({size: "14fp",weight: "40"})
              .width("55%")
              .type(InputType.Number)
              .onChange((value) => {
                this.EventA = value
              })
          } .height(60)
          .width('90%')
          .padding({left:10})
          .backgroundColor(Color.White)
          .borderStyle(BorderStyle.Dashed)
          .borderWidth(0)
          .borderColor(Color.Red)
          .borderRadius(15)
        }.width('100%').margin({ top: 5 })
      }
    }.width('100%').backgroundColor('#FFf1f3f5').height('100%')
  }
}
