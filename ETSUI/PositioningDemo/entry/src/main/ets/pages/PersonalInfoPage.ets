import router from '@ohos.router';
import { UserService } from '../service/UserService';
import { UserInfo, calculateBMI, getBMICategory } from '../model/UserModel';
import { common } from '@kit.AbilityKit';
import promptAction from '@ohos.promptAction';

@Entry
@Component
struct PersonalInfoPage {
  @State userInfo: UserInfo = new UserInfo();
  @State nickname: string = '';
  @State signature: string = '';
  @State gender: string = '未设置';
  @State age: string = '';
  @State userHeight: string = '';
  @State userWeight: string = '';
  @State bmi: number = 0;
  @State bmiCategory: string = '未设置';

  private userService: UserService = new UserService();

  async aboutToAppear() {
    const context = getContext(this) as common.UIAbilityContext;
    await this.userService.init(context);
    this.userInfo = await this.userService.getUserInfo();
    this.nickname = this.userInfo.nickname;
    this.signature = this.userInfo.signature;
    this.gender = this.userInfo.gender;
    this.age = this.userInfo.age > 0 ? this.userInfo.age.toString() : '';
    this.userHeight = this.userInfo.height > 0 ? this.userInfo.height.toString() : '';
    this.userWeight = this.userInfo.weight > 0 ? this.userInfo.weight.toString() : '';
    this.calculateBMI();
  }

  build() {
    Column() {
      this.TopNavBar()
      Scroll() {
        Column() {
          this.SectionTitle('基本信息')
          this.InputItem('昵称', this.nickname, '请输入昵称', (value: string) => {
            this.nickname = value;
          })
          this.InputItem('个性签名', this.signature, '请输入个性签名', (value: string) => {
            this.signature = value;
          })

          // 性别选择（内联实现，确保 @State 绑定）
          Row() {
            Text('性别')
              .fontSize(16)
              .fontColor('#333333')
            Blank()
            Text(this.gender)
              .fontSize(16)
              .fontColor(this.gender === '未设置' ? '#999999' : '#333333')
              .margin({ right: 8 })
            Text('>')
              .fontSize(16)
              .fontColor('#CCCCCC')
          }
          .width('100%')
          .padding(16)
          .backgroundColor('#FFFFFF')
          .borderRadius(12)
          .margin({ bottom: 8 })
          .onClick(() => {
            this.showGenderDialog();
          })

          this.InputItem('年龄', this.age, '请输入年龄', (value: string) => {
            this.age = value;
          }, true)

          this.SectionTitle('身体数据')
          this.InputItem('身高', this.userHeight, '请输入身高（cm）', (value: string) => {
            this.userHeight = value;
            this.calculateBMI();
          }, true)
          this.InputItem('体重', this.userWeight, '请输入体重（kg）', (value: string) => {
            this.userWeight = value;
            this.calculateBMI();
          }, true)

          if (this.bmi > 0) {
            this.BMICard()
          }

          Button('保存')
            .width('100%')
            .height(48)
            .fontSize(16)
            .fontColor('#FFFFFF')
            .backgroundColor('#4ECDC4')
            .borderRadius(12)
            .margin({ top: 24, bottom: 24 })
            .onClick(() => {
              this.saveUserInfo();
            })
        }
        .padding(16)
      }
      .layoutWeight(1)
      .backgroundColor('#F5F5F5')
    }
    .width('100%')
    .height('100%')
  }

  @Builder
  TopNavBar() {
    Row() {
      Text('< 返回')
        .fontSize(16)
        .fontColor('#4ECDC4')
        .onClick(() => {
          router.back();
        })
      Blank()
      Text('编辑资料')
        .fontSize(20)
        .fontWeight(FontWeight.Bold)
        .fontColor('#333333')
      Blank()
      Text('     ')
    }
    .width('100%')
    .padding(16)
    .backgroundColor('#FFFFFF')
  }

  @Builder
  SectionTitle(title: string) {
    Text(title)
      .fontSize(14)
      .fontColor('#999999')
      .fontWeight(FontWeight.Bold)
      .width('100%')
      .margin({ top: 16, bottom: 8, left: 4 })
  }

  @Builder
  InputItem(label: string, value: string, placeholder: string, onChange: (value: string) => void, isNumber: boolean = false) {
    Column() {
      Text(label)
        .fontSize(14)
        .fontColor('#666666')
        .width('100%')
        .margin({ bottom: 8 })
      TextInput({ text: value, placeholder: placeholder })
        .width('100%')
        .height(44)
        .fontSize(16)
        .fontColor('#333333')
        .backgroundColor('#F8F9FA')
        .borderRadius(8)
        .type(isNumber ? InputType.Number : InputType.Normal)
        .onChange(onChange)
    }
    .width('100%')
    .padding(16)
    .backgroundColor('#FFFFFF')
    .borderRadius(12)
    .margin({ bottom: 8 })
    .alignItems(HorizontalAlign.Start)
  }

  @Builder
  SelectItem(label: string, value: string, onClick: () => void) {
    Row() {
      Text(label)
        .fontSize(16)
        .fontColor('#333333')
      Blank()
      Text(value)
        .fontSize(16)
        .fontColor(value === '未设置' ? '#999999' : '#333333')
        .margin({ right: 8 })
      Text('>')
        .fontSize(16)
        .fontColor('#CCCCCC')
    }
    .width('100%')
    .padding(16)
    .backgroundColor('#FFFFFF')
    .borderRadius(12)
    .margin({ bottom: 8 })
    .onClick(onClick)
  }

  @Builder
  BMICard() {
    Column() {
      Row() {
        Text('BMI指数')
          .fontSize(16)
          .fontWeight(FontWeight.Bold)
          .fontColor('#333333')
        Blank()
        Text(this.bmi.toFixed(1))
          .fontSize(24)
          .fontWeight(FontWeight.Bold)
          .fontColor(this.getBMIColor(this.bmi))
      }
      .width('100%')
      .margin({ bottom: 8 })

      Row() {
        Text('健康状态:')
          .fontSize(14)
          .fontColor('#666666')
        Text(this.bmiCategory)
          .fontSize(14)
          .fontWeight(FontWeight.Bold)
          .fontColor(this.getBMIColor(this.bmi))
          .margin({ left: 8 })
          .padding({ left: 12, right: 12, top: 4, bottom: 4 })
          .backgroundColor(this.getBMIColor(this.bmi) + '20')
          .borderRadius(8)
      }
      .width('100%')
    }
    .width('100%')
    .padding(20)
    .backgroundColor('#FFFFFF')
    .borderRadius(16)
    .margin({ top: 16, bottom: 16 })
    .shadow({ radius: 8, color: '#10000000', offsetX: 0, offsetY: 2 })
  }

  showGenderDialog() {
    promptAction.showDialog({
      title: '选择性别',
      message: '',
      buttons: [
        { text: '男', color: '#4ECDC4' },
        { text: '女', color: '#4ECDC4' },
        { text: '未设置', color: '#999999' }
      ]
    }).then((result) => {
      if (result.index === 0) {
        this.gender = '男';
      } else if (result.index === 1) {
        this.gender = '女';
      } else {
        this.gender = '未设置';
      }
      // 调试：确认性别已更新
      promptAction.showToast({
        message: `性别已选择: ${this.gender}`,
        duration: 2000
      });
    });
  }

  calculateBMI() {
    const heightNum = parseFloat(this.userHeight);
    const weightNum = parseFloat(this.userWeight);
    if (heightNum > 0 && weightNum > 0) {
      this.bmi = calculateBMI(heightNum, weightNum);
      this.bmiCategory = getBMICategory(this.bmi);
    } else {
      this.bmi = 0;
      this.bmiCategory = '未设置';
    }
  }

  getBMIColor(bmi: number): string {
    if (bmi === 0) return '#999999';
    if (bmi < 18.5) return '#3498DB';
    if (bmi < 24) return '#2ECC71';
    if (bmi < 28) return '#F39C12';
    return '#E74C3C';
  }

  async saveUserInfo() {
    if (this.nickname.trim().length === 0) {
      promptAction.showToast({
        message: '请输入昵称',
        duration: 2000
      });
      return;
    }

    const ageNum = this.age.length > 0 ? parseInt(this.age) : 0;
    const heightNum = this.userHeight.length > 0 ? parseFloat(this.userHeight) : 0;
    const weightNum = this.userWeight.length > 0 ? parseFloat(this.userWeight) : 0;

    if (this.age.length > 0 && (isNaN(ageNum) || ageNum <= 0 || ageNum > 150)) {
      promptAction.showToast({
        message: '年龄输入不合法',
        duration: 2000
      });
      return;
    }

    this.userInfo.nickname = this.nickname.trim();
    this.userInfo.signature = this.signature.trim();
    this.userInfo.gender = this.gender;
    this.userInfo.age = ageNum;
    this.userInfo.height = heightNum;
    this.userInfo.weight = weightNum;

    const success = await this.userService.saveUserInfo(this.userInfo);

    if (success) {
      promptAction.showToast({
        message: '保存成功',
        duration: 2000
      });
      setTimeout(() => {
        router.back();
      }, 500);
    } else {
      promptAction.showToast({
        message: '保存失败',
        duration: 2000
      });
    }
  }
}
