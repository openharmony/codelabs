import picker from '@ohos.file.picker'
import Prompt from '@system.prompt'
import router from '@ohos.router'
import { dbManager } from '../utils/DatabaseManager';
import { UserModel } from '../viewmodel/UserModel'
// import { BusinessError } from '@kit.BasicServicesKit';
import { LocationPickerDialog } from '../components/LocationPickerDialog';
import { promptAction } from '@kit.ArkUI';

@Entry
@Component
struct Register {
  @State avatarPath: string = ''
  @State nickName: string = ''
  @State country: string = ''
  @State phoneNumber: string = ''
  @State password: string = ''
  @State isAgree: boolean = false
  private photoPicker = new picker.PhotoViewPicker()

  // 定义弹窗控制器
  dialogController: CustomDialogController = new CustomDialogController({
    builder: LocationPickerDialog({
      confirm: (province: string, city: string) => {
        this.country = `中国 ${province} ${city}`
      }
    }),
    alignment: DialogAlignment.Bottom, // 底部弹出
    customStyle: true // 使用自定义样式
  })

  // 利用生命周期钩子
  onPageShow() {
    // 获取从 LocationSelectPage 返回时的参数
    const params = router.getParams() as Record<string, string>;
    if (params && params.province) {
      // 这里的 key 要和 LocationSelectPage 中 back 传的 key 一致
      const countryName = params.country || $r('app.string.label_china');
      const provinceName = params.province;
      this.country = `${countryName} ${provinceName}`;

      // 顺手清理掉 params，防止下次进入页面误触发（代码严谨性加分）
      // router.getParams() 在某些版本是只读的，通常 back 传参是一次性的
    }
  }

  build() {
    Row() {
      Column() {
        Text($r('app.string.title_phone_reg'))
          .fontSize(26)
          .fontWeight(500)

        Image(this.avatarPath)
          .size({ width: 100, height: 100 })
          .backgroundColor(Color.Grey)
          .onClick(async () => {
            let data = await this.photoPicker.select({
              MIMEType: picker.PhotoViewMIMETypes.IMAGE_TYPE,
              maxSelectNumber: 1
            })

            this.avatarPath = data.photoUris[0]

          })
          .margin({
            top: 20
          })

        Column() {
          MyTextInput({ label: $r('app.string.label_nickname'), placeholder: $r('app.string.hint_example_name'), text: $nickName, type: InputType.Normal })
          // MyTextInput({ label: $r('app.string.label_country_region'), placeholder: $r('app.string.label_mainland_china'), text: $country, type: InputType.Normal })
          Column() {
            Row() {
              Text($r('app.string.label_country_region'))
                .width(80)
                .fontSize(18)

              Text(this.country === '' ? $r('app.string.toast_select_region') : this.country)
                .fontSize(18)
                .fontColor(this.country === '' ? Color.Gray : Color.Black)
                .layoutWeight(1)
                .onClick(() => {
                  this.dialogController.open()
                })
            }
            .padding({ top: 10, bottom: 10 })

            Divider().margin({ bottom: 10 })
          }
          MyTextInput({ label: $r('app.string.label_phone'), placeholder: $r('app.string.hint_fill_phone'), text: $phoneNumber, type: InputType.Number })
          MyTextInput({ label: $r('app.string.label_pwd'), placeholder: $r('app.string.hint_fill_pwd'), text: $password, type: InputType.Password })

        }.width('80%')
        .justifyContent(FlexAlign.SpaceBetween)
        .margin({
          top: 30
        })

        Row() {
          Row() {
            Radio({
              group: 'agreement',
              value: 'agree'
            })
              .onChange((isChecked) => {
                this.isAgree = isChecked
              })
          }.width(40)
          .alignItems(VerticalAlign.Top)
          .offset({ x: 12, y: -8 })


          Column() {
            Text() {
              Span($r('app.string.label_read_agree'))
                .fontSize(14)
                .fontColor(Color.Grey)
              Span($r('app.string.label_license_agreement'))
                .onClick(()=>{
                  router.pushUrl({ url: 'pages/LicenseAgreement' });
                })
                .fontColor(Color.Blue)
                .fontSize(14)
            }

            Text($r('app.string.label_register_notice'))
              .fontColor(Color.Grey)
              .fontSize(14)
          }
        }.alignItems(VerticalAlign.Top)
        .margin({
          top: 60
        })

        Row() {
          Button($r('app.string.btn_agree_continue'))
            .type(ButtonType.Normal)
            .borderRadius(10)
            .margin({ top: 20 })
            .width(160)
            .height(50)
            .backgroundColor(this.isAgree ? Color.Green : $r('app.color.light_grey'))
            .fontColor(this.isAgree ? Color.White : Color.Grey)
            .onClick(async () => {
              let err = ''
              if (this.nickName.trim() == '') {
                //err += $r('app.string.error_nickname_empty')
                promptAction.showToast({
                  message: $r('app.string.error_nickname_empty')
                });
                return
              }
              if (this.country.trim() == '') {
                //err += $r('app.string.error_country_empty')
                promptAction.showToast({
                  message: $r('app.string.error_country_empty')
                });
                return
              }
              if (this.phoneNumber.trim() == '') {
                //err += $r('app.string.error_phone_empty')
                promptAction.showToast({
                  message: $r('app.string.error_phone_empty')
                });
                return
              }else if (this.phoneNumber.length !== 11 || !/^\d+$/.test(this.phoneNumber)) {
                // 长度不是11位，或者 包含非数字字符
                //err += $r('app.string.error_phone_format');
                promptAction.showToast({
                  message: $r('app.string.error_phone_format')
                });
                return
              }

              if (this.password.trim() == '') {
                //err += $r('app.string.error_pwd_empty')
                promptAction.showToast({
                  message: $r('app.string.error_pwd_empty')
                });
                return
              }

              if (!this.isAgree) {
                //err += $r('app.string.toast_agree_protocol')
                promptAction.showToast({
                  message: $r('app.string.toast_agree_protocol')
                });
                return
              }
              // if (err != '') {
              //   Prompt.showToast({
              //    message: err
              //   })
              //   return
              // }

              // --- 手机号重复校验 ---
              try {
                const isExists = await dbManager.user?.isPhoneNumberExists(this.phoneNumber);
                if (isExists) {
                  //Prompt.showToast({ message: $r('app.string.toast_already_registered').toString() });
                  promptAction.showToast({
                    message: $r('app.string.toast_already_registered')
                  });
                  return; // 发现重复，拦截后续操作
                }
              } catch (e) {
                console.error('校验重复时出错');
              }

              try {
                const userData: UserModel = {
                  avatarPath: this.avatarPath,
                  nickName: this.nickName,
                  country: this.country,
                  phoneNumber: this.phoneNumber,
                  password: this.password
                }

                // 1. 检查 dbManager 本身
                console.log('Test: dbManager exists?', !!dbManager);

                // 2. 检查子处理器是否为空 (最可疑的点)
                if (!dbManager.user) {
                  console.error('Test: dbManager.user is NULL! 插入指令未发出');
                  return;
                }

                const rowId = await dbManager.user?.insertUser(userData); // 3. 异步写入数据库
                console.log('Test: RowId returned:', rowId); // 如果这里返回了数字，说明存进去了

                //Prompt.showToast({ message: $r('app.string.toast_reg_success').toString() })
                promptAction.showToast({
                  message: $r('app.string.toast_reg_success'),
                  duration: 2000
                });
                // 延迟一下再返回，确保用户能看到提示
                setTimeout(() => {
                  router.back()
                }, 1000)

              } catch (e) {
                //Prompt.showToast({ message: $r('app.string.error_db_write').toString() })
                promptAction.showToast({
                  message: $r('app.string.error_db_write'),
                  duration: 2000
                });
                console.error('Insert error:', JSON.stringify(e))
              }

              const savedUser = await dbManager.user?.queryUserByPhone(this.phoneNumber);
              console.info('数据库里的用户昵称是: ' + savedUser?.nickName);
            })
        }
        .justifyContent(FlexAlign.Center)

        // Button('打印数据库全量数据')
        //   .onClick(async () => {
        //
        //     const allUsers = await dbManager.user?.queryAllUsers();
        //     console.log('Test: Total users after insert:', allUsers?.length);
        //
        //     if (allUsers && allUsers.length > 0) {
        //       console.info('======= 数据库用户列表 =======');
        //       allUsers.forEach((user, index) => {
        //         console.info(`[用户 ${index + 1}] 昵称: ${user.nickName}, 手机: ${user.phoneNumber}, 路径: ${user.avatarPath}`);
        //       });
        //       console.info('============================');
        //     } else {
        //       console.warn('数据库目前是空的哦！');
        //     }
        //   })

      }
      .width('100%')
      .justifyContent(FlexAlign.Center)
    }
    .height('100%')
  }
}

@Component
struct MyTextInput {
  @Prop label: ResourceStr
  @Prop placeholder: ResourceStr
  @Link text: ResourceStr
  type: InputType = InputType.Normal

  build() {
    Column() {
      Row() {
        Text(this.label)
          .width(80)
          .fontSize(18)

        TextInput({ placeholder: this.placeholder, text: this.text })
          .type(this.type)
          .fontSize(18).stateStyles({
          normal: {
            .border({
              width: 0,
              radius: 0
            })
            .backgroundColor(Color.White)
          },
          focused: {
            .backgroundColor(Color.White)
          },
          pressed: {
            .backgroundColor(Color.White)
          }
        })
          .onChange((value) => {
            this.text = value
          })
      }

      Divider()
        .margin({
          top: 10,
          bottom: 10
        })
    }
  }
}