import { VIDEO_DATA, VideoItem } from '../model/videomodel';

@Component
export struct MeditationPage {
  @State searchText: string = '';
  @State currentVideo: VideoItem = VIDEO_DATA[0];
  @State playRate: number = 1.0;
  private controller: VideoController = new VideoController();

  private rates: number[] = [
    0.75,
    1.0,
    1.25,
    1.5,
    2.0
  ];

  getFilteredVideos(): VideoItem[] {
    if (!this.searchText) {
      return VIDEO_DATA;
    }
    return VIDEO_DATA.filter(item =>
    item.title.toLowerCase().includes(this.searchText.toLowerCase())
    );
  }

  build() {
    Column() {
      Search({ placeholder: '搜索视频教程...', value: this.searchText })
        .width('90%')
        .height(40)
        .margin({
          top: 20,
          bottom: 10
        })
        .onChange((value: string) => {
          this.searchText = value;
        })

      Column() {
        Video({
          src: this.currentVideo.source,
          controller: this.controller,
          currentProgressRate: this.playRate
        })
          .width('100%')
          .height(240)
          .autoPlay(false)
          .controls(true)
          .objectFit(ImageFit.Contain)
          .backgroundColor(Color.Black)

        Row({ space: 10 }) {
          Text('倍速选择:').fontSize(14).fontColor('#666')
          ForEach(this.rates, (rate: number) => {
            Button(`${rate}x`)
              .fontSize(12)
              .height(28)
              .padding({
                left: 10,
                right: 10
              })
              .backgroundColor(this.playRate === rate ? '#007DFF' : '#E9E9E9')
              .fontColor(this.playRate === rate ? Color.White : '#333')
              .onClick(() => {
                this.playRate = rate;
              })
          })
        }
        .width('100%')
        .padding({
          left: 15,
          right: 15,
          top: 10,
          bottom: 5
        })

        Text(`正在播放: ${this.currentVideo.title}`)
          .fontSize(16)
          .fontWeight(FontWeight.Bold)
          .margin({
            top: 5,
            bottom: 10
          })
      }
      .width('100%')
      .backgroundColor(Color.White)
      .shadow({
        radius: 10,
        color: '#10000000'
      })

      List({ space: 10 }) {
        ForEach(this.getFilteredVideos(), (item: VideoItem) => {
          ListItem() {
            Row() {
              SymbolGlyph($r('sys.symbol.video_fill'))
                .fontSize(30)
                .fontColor(['#007DFF'])
                .margin({ right: 15 })

              Text(item.title)
                .fontSize(16)
                .layoutWeight(1)

              if (this.currentVideo.id === item.id) {
                Text('正在播放').fontSize(12).fontColor('#007DFF')
              }
            }
            .width('100%')
            .padding(15)
            .backgroundColor(this.currentVideo.id === item.id ? '#F0F7FF' : Color.White)
            .borderRadius(10)
            .onClick(() => {
              this.currentVideo = item;
              this.playRate = 1.0;
            })
          }
        }, (item: VideoItem) => item.id)
      }
      .width('100%')
      .layoutWeight(1)
      .padding(16)
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F1F3F5')
  }
}