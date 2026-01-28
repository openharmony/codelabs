import router from '@ohos.router';
import { VIDEO_DATA, VideoItem } from '../../model/videomodel';

@Entry
@Component
export struct MeditationVideoPage {

  // 1. 【新增】引入全局字体缩放变量
  @StorageProp('appFontScale') fontScale: number = 1.0;

  build() {
    Column() {

      // ===== 顶部栏 =====
      Row() {
        Text('正念视频')
          .fontSize(24 * this.fontScale)
          .fontWeight(FontWeight.Bold)
      }
      .width('100%')
      .padding(20)

      // ===== 视频列表 =====
      List({ space: 12 }) {
        ForEach(VIDEO_DATA, (item: VideoItem) => {
          ListItem() {
            Row({ space: 12 }) {

              // ---- 封面 ----
              Image(item.cover)
                .width(120)
                .height(72)
                .borderRadius(8)
                .objectFit(ImageFit.Cover)

              // ---- 文本 ----
              Column({ space: 6 }) {
                Text(item.title)
                  .fontSize(16 * this.fontScale)
                  .fontWeight(FontWeight.Medium)
                  .fontColor(Color.Black)

                Text(item.desc)
                  .fontSize(13 * this.fontScale)
                  .fontColor('#666')
                  .maxLines(2)
                  .textOverflow({ overflow: TextOverflow.Ellipsis })
              }
              .layoutWeight(1)
            }
            .padding(12)
            .backgroundColor('#F2F2F7')
            .borderRadius(12)
            .onClick(() => {
              router.pushUrl({
                url: 'pages/Meditation/MeditationVideoPlayPage',
                params: item
              })
            })
          }
        }, (item: VideoItem) => item.id)
      }
      .layoutWeight(1)
      .padding({ left: 16, right: 16 })
    }
    .width('100%')
    .height('100%')
    .backgroundColor(Color.White)
  }
}
