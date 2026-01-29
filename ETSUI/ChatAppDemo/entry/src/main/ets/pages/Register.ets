import picker from '@ohos.file.picker'
import Prompt from '@system.prompt'
import router from '@ohos.router'
import { dbManager } from '../utils/DatabaseManager';
import { UserModel } from '../viewmodel/UserModel'
// import { BusinessError } from '@kit.BasicServicesKit';
import { LocationPickerDialog } from '../components/LocationPickerDialog';

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
      const countryName = params.country || '中国';
      const provinceName = params.province;
      this.country = `${countryName} ${provinceName}`;

      // 顺手清理掉 params，防止下次进入页面误触发（代码严谨性加分）
      // router.getParams() 在某些版本是只读的，通常 back 传参是一次性的
    }
  }

  build() {
    Row() {
      Column() {
        Text('手机号注册')
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
          MyTextInput({ label: '昵称', placeholder: '例如: 陈晨', text: $nickName, type: InputType.Normal })
          // MyTextInput({ label: '国家/地区', placeholder: '中国大陆(+86)', text: $country, type: InputType.Normal })
          Column() {
            Row() {
              Text('国家/地区')
                .width(80)
                .fontSize(18)

              Text(this.country === '' ? '请选择地区' : this.country)
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
          MyTextInput({ label: '手机号', placeholder: '请填写手机号', text: $phoneNumber, type: InputType.Number })
          MyTextInput({ label: '密码', placeholder: '请填写密码', text: $password, type: InputType.Password })

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
              Span('我已阅读同意')
                .fontSize(14)
                .fontColor(Color.Grey)
              Span('《软件许可及服务协议》')
                .onClick(()=>{
                  router.pushUrl({ url: 'pages/LicenseAgreement' });
                })
                .fontColor(Color.Blue)
                .fontSize(14)
            }

            Text('本页面手机的信息仅用于注册账号')
              .fontColor(Color.Grey)
              .fontSize(14)
          }
        }.alignItems(VerticalAlign.Top)
        .margin({
          top: 60
        })

        Row() {
          Button('同意并继续')
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
                err += '昵称不能为空;'
              }
              if (this.country.trim() == '') {
                err += '国家不能为空;'
              }
              if (this.phoneNumber.trim() == '') {
                err += '手机号不能为空;'
              }else if (this.phoneNumber.length !== 11 || !/^\d+$/.test(this.phoneNumber)) {
                // 长度不是11位，或者 包含非数字字符
                err += '请输入正确的11位数字手机号;';
              }

              if (this.password.trim() == '') {
                err += '密码不能为空;'
              }

              if (!this.isAgree) {
                err += '请先同意注册协议'
              }
              if (err != '') {
                Prompt.showToast({
                  message: err
                })
                return
              }

              // --- 手机号重复校验 ---
              try {
                const isExists = await dbManager.user?.isPhoneNumberExists(this.phoneNumber);
                if (isExists) {
                  Prompt.showToast({ message: '该手机号已注册，请直接登录' });
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

                Prompt.showToast({ message: '注册成功' })

                // 延迟一下再返回，确保用户能看到提示
                setTimeout(() => {
                  router.back()
                }, 1000)

              } catch (e) {
                Prompt.showToast({ message: '数据库写入失败' })
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
  @Prop label: string
  @Prop placeholder: string
  @Link text: string
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