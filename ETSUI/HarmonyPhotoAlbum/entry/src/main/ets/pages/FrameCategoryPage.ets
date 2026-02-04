import router from '@ohos.router';
import Constants from '../common/constants/Constants';

@Entry
@Component
struct FrameCategoryPage {
  @State selectedCategory: string = '全部';

  private categories: string[] = ['全部', '风景', '生活', '美食', '人物'];

  private goBack() {
    router.back();
  }

  private goToFramePage(category: string) {
    this.selectedCategory = category;
    router.pushUrl({
      url: Constants.URL_FRAME_PAGE,
      params: {
        frameCategory: category
      }
    }).catch((err: Error) => {
      console.error('[FrameCategoryPage] 跳转 FramePage 失败: ' + JSON.stringify(err));
    });
  }

  @Builder
  CategoryItem(label: string) {
    Button(label)
      .width('100%')
      .height(40)
      .padding({ left: 12, right: 12 })
      .fontSize(14)
      .backgroundColor(this.selectedCategory === label ? '#007DFF' : '#FFFFFF')
      .fontColor(this.selectedCategory === label ? Color.White : '#333333')
      .borderRadius(8)
      .border({
        width: 1,
        color: this.selectedCategory === label ? '#007DFF' : '#DDDDDD'
      })
      .onClick(() => {
        this.goToFramePage(label);
      })
  }

  build() {
    Column({ space: 16 }) {
      // 顶部返回 + 标题
      Row({ space: 12 }) {
        Button({ type: ButtonType.Circle }) {
          Text('<')
            .fontSize(18)
            .fontColor(Color.Black)
        }
        .backgroundColor('#F0F0F0')
        .width(36)
        .height(36)
        .onClick(() => this.goBack())

        Column() {
          Text('选择电子相框分类')
            .fontSize(20)
            .fontWeight(FontWeight.Medium)
            .fontColor('#222222')

          Text('选择一个分类后，将进入全屏幻灯片播放页面')
            .fontSize(12)
            .fontColor('#888888')
        }
      }
      .padding({ top: 24, left: 16, right: 16 })

      // 分类列表
      Column({ space: 12 }) {
        ForEach(this.categories, (c: string) => {
          this.CategoryItem(c);
        }, (c: string) => c)
      }
      .padding({ left: 16, right: 16, top: 8 })

      Blank().layoutWeight(1)

      Text('提示：播放时可以在 FramePage 中调整轮播间隔')
        .fontSize(12)
        .fontColor('#AAAAAA')
        .padding({ left: 16, right: 16, bottom: 24 })
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F5F5F5')
  }
}
