          // 导入核心模块
import media from '@ohos.multimedia.media';
import fileIo from '@ohos.file.fs';
import { promptAction } from '@kit.ArkUI';
import { preferences } from '@kit.ArkData';

@Entry
@Component
export struct AudioRecordPage {
  // 声明AVRecorder实例（音视频录制管理类）
  avRecorder?: media.AVRecorder;
  // 文件句柄（用于关联录音文件）
  fd?: number;
  // 录音文件保存路径
  filePath?: string;

  // 用户首选项相关属性
  @State audioBitrate: number = 10000;
  @State audioChannels: number = 1;
  @State audioSampleRate: number = 48000;
  private options: preferences.Options = { name: "AudioOptions" };
  private dataPreference: preferences.Preferences | null = null;

  /**
   * 开始录音核心方法
   */
  async startRecord() {
    try {
      // 1. 步骤1：创建本地文件，用于接收录音数据
      const ctx = this.getUIContext().getHostContext();
      // 拼接文件路径：应用私有目录 + 时间戳 + .m4a（确保文件名唯一）
      this.filePath = `${ctx?.filesDir}/${Date.now()}.m4a`;
      // 以“创建+读写”模式打开文件，获取文件句柄（fd）
      const file = fileIo.openSync(
        this.filePath,
        fileIo.OpenMode.CREATE | fileIo.OpenMode.READ_WRITE
      );
      this.fd = file.fd;
      let show:string = this.fd.toString()
      console.log("sp1"+show);

      // 2. 步骤2：配置录音参数（关键！参数错误会导致录音失败）
      const recordConfig: media.AVRecorderConfig = {
        // 音频源：麦克风（唯一支持的音频源类型）
        audioSourceType: media.AudioSourceType.AUDIO_SOURCE_TYPE_MIC,
        // 录音质量配置
        profile: {
          audioBitrate: this.audioBitrate,
          audioChannels: this.audioChannels,
          audioCodec: media.CodecMimeType.AUDIO_AAC,
          audioSampleRate: this.audioSampleRate,
          fileFormat: media.ContainerFormatType.CFT_MPEG_4A // 封装格式：仅支持m4a
        },
        // 关联录音文件：通过fd://协议绑定文件句柄
        url: `fd://${this.fd}`
      };

      // 3. 步骤3：初始化AVRecorder并开始录音
      this.avRecorder = await media.createAVRecorder(); // 创建AVRecorder实例
      await this.avRecorder.prepare(recordConfig); // 初始化录音配置
      await this.avRecorder.start(); // 启动录音
      promptAction.showToast({ message: '开始录音...' });
    } catch (error) {
      console.error('开始录音失败：', error);
      promptAction.showToast({ message: '录音启动失败，请检查权限' });
    }
  }

  /**
   * 结束录音核心方法
   */
  async stopRecord() {
    try {
      if (!this.avRecorder || !this.fd) {
        promptAction.showToast({ message: '未开始录音' });
        return;
      }

      // 1. 停止录音
      await this.avRecorder.stop();
      // 2. 释放AVRecorder资源（关键：避免内存泄露）
      await this.avRecorder.release();
      // 3. 关闭文件句柄（避免文件被占用）
      fileIo.closeSync(this.fd);

      promptAction.showToast({ message: `录音已保存：${this.filePath}` });
      console.log('录音文件路径：', this.filePath);

      // 清空实例，避免重复操作
      this.avRecorder = undefined;
      this.fd = undefined;
    } catch (error) {
      console.error('结束录音失败：', error);
      promptAction.showToast({ message: '录音停止失败' });
    }
  }

  // 初始化首选项
  StartPreference() {
    this.dataPreference = preferences.getPreferencesSync(this.getUIContext().getHostContext(), this.options);
  }

  // 保存所有设置
  FlushAll() {
    this.dataPreference?.putSync('audioBitrate', this.audioBitrate.toString());
    this.dataPreference?.putSync('audioChannels', this.audioChannels.toString());
    this.dataPreference?.putSync('audioSampleRate', this.audioSampleRate.toString());
    this.dataPreference?.flushSync();
  }

  // 读取首选项
  GetPreferences() {
    this.audioBitrate = parseInt(this.dataPreference?.getSync('audioBitrate', '10000').toString() || '10000');
    this.audioChannels = parseInt(this.dataPreference?.getSync('audioChannels', '1').toString() || '1');
    this.audioSampleRate = parseInt(this.dataPreference?.getSync('audioSampleRate', '48000').toString() || '48000');
  }

  aboutToAppear(): void {
    this.StartPreference();
    this.GetPreferences();
  }

  build() {
    List() {
      ListItem() {
        Text('音频录制设置')
          .fontSize(30)
          .width('100%')
          .fontWeight(FontWeight.Bold)
          .margin(20)
          .textAlign(TextAlign.Center)
      }

      ListItem() {
        Column() {
          Text('音频比特率：')
            .fontSize(20)
            .margin({ left: 20, top: 10, bottom: 10 })
          Select([
            { value: "64000" },
            { value: "96000" },
            { value: "128000" }
          ])
          .selected(this.audioBitrate === 64000 ? 0 : this.audioBitrate === 96000 ? 1 : 2)
          .value(this.audioBitrate.toString())
          .onSelect((index: number) => {
            this.audioBitrate = index === 0 ? 64000 : index === 1 ? 96000 : 128000;
            this.FlushAll();
          })
        }
      }

      ListItem() {
        Column() {
          Text('声道数：')
            .fontSize(20)
            .margin({ left: 20, top: 10, bottom: 10 })
          Select([
            { value: "1" },
            { value: "2" }
          ])
          .selected(this.audioChannels === 1 ? 0 : 1)
          .value(this.audioChannels.toString())
          .onSelect((index: number) => {
            this.audioChannels = index === 0 ? 1 : 2;
            this.FlushAll();
          })
        }
      }

      ListItem() {
        Column() {
          Text('采样率：')
            .fontSize(20)
            .margin({ left: 20, top: 10, bottom: 10 })
          Select([
            { value: "44100" },
            { value: "48000" },
            { value: "96000" }
          ])
          .selected(this.audioSampleRate === 44100 ? 0 : this.audioSampleRate === 48000 ? 1 : 2)
          .value(this.audioSampleRate.toString())
          .onSelect((index: number) => {
            this.audioSampleRate = index === 0 ? 44100 : index === 1 ? 48000 : 96000;
            this.FlushAll();
          })
        }
      }

      ListItem() {
        Row({ space: 20 }) {
          Button('开始录音')
            .backgroundColor('#007DFF')
            .fontColor('#FFFFFF')
            .onClick(() => {
              this.startRecord();
            });

          Button('结束录音')
            .backgroundColor('#FF4444')
            .fontColor('#FFFFFF')
            .onClick(() => {
              this.stopRecord();
            });
        }
        .justifyContent(FlexAlign.Center)
        .width('100%')
        .margin({ top: 20 })
      }
    }
    .padding(40)
    .height('100%')
    .width('100%')
  }
}