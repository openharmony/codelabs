import { preferences } from '@kit.ArkData';
import { BusinessError, print } from '@kit.BasicServicesKit';
import { videoRecording, stopRecording } from '../videoControl/temp';
import { common } from '@kit.AbilityKit';

// import { UIAbility } from '@kit.AbilityKit';
// import window from '@ohos.window';


let options: preferences.Options = { name: "VideoOptions" }
let dataPreference: preferences.Preferences | null = null;

// dataPreference = preferences.getPreferencesSync(, options);

// let context = getContext(this) as common.UIAbilityContext

// class EntryAbility extends UIAbility {
//   onWindowStageCreate(windowStage: window.WindowStage): void {
//     let options: preferences.Options = { name: 'VideoSetting' };
//     dataPreference = preferences.getPreferencesSync(this.context, options);
//   }
// }

@Entry
@Component
export struct Setting {
  @State message: string = 'Hello World';
  @State audioSource: string = '';
  @State videoSource: string = '';
  @State outputFormat: string = 'MP4';
  @State resolution: string = '1280x720';
  @State frameRate: string = '30FPS';

  public StartPreference() {
    dataPreference = preferences.getPreferencesSync(this.getUIContext().getHostContext(), options);
  }

  public FlushAll() {
    if (dataPreference == null) {
      console.log("注意空值")
    }
    dataPreference?.putSync('audioSource', this.audioSource);
    if (dataPreference?.hasSync('audioSource')) {
      console.log("完成写入1");
    }
    dataPreference?.putSync('videoSource', this.videoSource);
    if (dataPreference?.hasSync('videoSource')) {
      console.log("完成写入2");
    }
    dataPreference?.putSync('outputFormat', this.outputFormat);
    if (dataPreference?.hasSync('outputFormat')) {
      console.log("完成写入3");
    }
    dataPreference?.putSync('resolution', this.resolution);
    if (dataPreference?.hasSync('resolution')) {
      console.log("完成写入4");
    }
    dataPreference?.putSync('frameRate', this.frameRate);
    if (dataPreference?.hasSync('frameRate')) {
      console.log("完成写入5");
    }
    dataPreference?.flush((err: BusinessError) => {
      if (err) {
        console.error("fail to flush.code = " + err.code + ",message =" + err.message)
        return;
      }
    });
    console.log('数据保存完成')
  }

  public FlushCheck(target: string) {
    let Sp1: preferences.ValueType | undefined = dataPreference?.getSync(target, "string");
    Sp1 = Sp1?.toString()
    console.log('this start up ' + Sp1);
    return;
  }

  public GetPreferences() {
    try {
      let temp1 = this.audioSource
      this.audioSource = dataPreference?.getSync('audioSource', 'fault').toString() || "";
      if (this.audioSource == 'fault') {
        this.audioSource = temp1;
        console.log("audioSource空");
      }
      temp1 = this.videoSource
      this.videoSource = dataPreference?.getSync('videoSource', 'fault').toString() || "";
      if (this.videoSource == 'fault') {
        this.videoSource = temp1
        console.log("videoSource空");
      }
      temp1 = this.outputFormat
      this.outputFormat = dataPreference?.getSync('outputFormat', 'fault').toString() || "";
      if (this.outputFormat == 'fault') {
        this.outputFormat = temp1
        console.log("outputFormat空");
      }
      temp1 = this.resolution
      this.resolution = dataPreference?.getSync('resolution', 'fault').toString() || "";
      if (this.resolution == 'fault') {
        this.resolution = temp1
        console.log("resolution空");
      }
      temp1 = this.frameRate
      this.frameRate = dataPreference?.getSync('frameRate', 'fault').toString() || "";
      if (this.frameRate == 'fault') {
        this.frameRate = temp1
        console.log('frameRate空');
      }
      console.info("读取成功！")
    } catch (error) {
      console.log("读取失败！")
    }
    return
  }

  aboutToAppear(): void {
    this.StartPreference()
    this.GetPreferences()
  }

  build() {
    List() {
      ListItem() {
        Text('设置录像参数')
          .fontSize(30)
          .width('100%')
          .fontWeight(FontWeight.Bold)
          .margin(20)
          .textAlign(TextAlign.Center)
      }

      ListItem() {
        Column() {
          Text('1. 音频源：麦克风')
            .fontSize(20)
            .margin({ left: 20, top: 10, bottom: 10 })
          TextInput({ placeholder: "请选择麦克风源", text: this.audioSource })
            .placeholderColor(Color.Gray)
            .width('100%')
            .height(40)
            .margin(20)
            .fontSize(20)
            .onChange((value: string) => {
              this.audioSource = value;
              this.FlushAll()
            })
        }
      }

      ListItem() {
        Column() {
          Text('2. 视频源：摄像头')
            .fontSize(20)
            .margin({ left: 20, top: 10, bottom: 10 })
          TextInput({ placeholder: "请选择摄像头源", text: this.videoSource })
            .placeholderColor(Color.Gray)
            .width('100%')
            .height(40)
            .margin(20)
            .fontSize(20)
            .onChange((value: string) => {
              this.videoSource = value;
              this.FlushAll()
            })
        }
      }

      ListItem() {
        Column() {
          Text('3. 输出格式：')
            .fontSize(20)
            .margin({ left: 20, top: 10, bottom: 10 })
          Select([{ value: "MP4" },
            { value: "AVI" },
            { value: "GIF" }])
            .selected(this.outputFormat === 'MP4' ? 0 : this.outputFormat === 'AVI' ? 1 : 2)
            .value(this.outputFormat)
            .font({ size: 16, weight: 500 })
            .fontColor('#182431')
            .selectedOptionFont({ size: 16, weight: 400 })
            .optionFont({ size: 16, weight: 400 })
            .width("100%")
            .height(40)
            .onSelect((index: number) => {
              this.outputFormat = index === 0 ? 'MP4' : index === 1 ? 'AVI' : 'GIF';
              this.FlushAll()
            })
        }
      }

      ListItem() {
        Column() {
          Text('4. 分辨率：')
            .fontSize(20)
            .margin({ left: 20, top: 10, bottom: 10 })
          Select([{ value: "1280x720" },
            { value: "1920x1080" },
            { value: "2560x1280" }])
            .selected(this.resolution === '1280x720' ? 0 : this.resolution === '1920x1080' ? 1 : 2)
            .value(this.resolution)
            .font({ size: 16, weight: 500 })
            .fontColor('#182431')
            .selectedOptionFont({ size: 16, weight: 400 })
            .optionFont({ size: 16, weight: 400 })
            .width("100%")
            .height(40)
            .onSelect((index: number) => {
              this.resolution = index === 0 ? '1280x720' : index === 1 ? '1920x1080' : '2560x1280';
              this.FlushAll()
            })
        }
      }

      ListItem() {
        Column() {
          Text('5. 帧率：')
            .fontSize(20)
            .margin({ left: 20, top: 10, bottom: 10 })
          Select([{ value: "30FPS" },
            { value: "60FPS" },
            { value: "120FPS" }])
            .selected(this.frameRate === '30FPS' ? 0 : this.frameRate === '60FPS' ? 1 : 2)
            .value(this.frameRate)
            .font({ size: 16, weight: 500 })
            .fontColor('#182431')
            .selectedOptionFont({ size: 16, weight: 400 })
            .optionFont({ size: 16, weight: 400 })
            .width("100%")
            .height(40)
            .onSelect((index: number) => {
              this.frameRate = index === 0 ? '30FPS' : index === 1 ? '60FPS' : '120FPS';
              this.FlushAll()
            })
        }
      }

      ListItem() {
        Row() {
          Button("测试").onClick((event: ClickEvent) => {
            // AlertDialog.show({ message: '测试按钮被点击' })
            // 使用状态变量获取输入框内容
            console.log('音频源：', this.audioSource);
            console.log('视频源：', this.videoSource);
            console.log('输出格式：', this.outputFormat);
            console.log('分辨率：', this.resolution);
            console.log('帧率：', this.frameRate);
            this.FlushAll()
            // this.FlushCheck()
          })
          Button('测试2').onClick((event: ClickEvent) => {
            this.FlushCheck("audioSource")
            this.FlushCheck("videoSource")
          })
          Button("开始录制").onClick(async () => {
            let context = this.getUIContext().getHostContext() as common.Context;
            console.log("check1s");
            await videoRecording(context);
            console.log("check2s");
          })
          Button("停止录制").onClick(async () => {
            try {
              await stopRecording();
              // 显示录制结果提示
              AlertDialog.show({
                title: '录制完成',
                message: '视频已保存至图库',
                confirm: {
                  value: '确定',
                  action: () => console.log('确认按钮点击')
                }
              });
            } catch (error) {
              let err = error as BusinessError;
              console.error(`停止录制失败，错误码: ${err.code}, 信息: ${err.message}`);
              AlertDialog.show({ message: `停止录制失败: ${err.message}` });
            }
          })
        }
      }
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#f5f5f5')
    .padding({ top: 10, bottom: 10 })
  }
}
