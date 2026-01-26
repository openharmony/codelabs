import router from '@ohos.router';
import { photoAccessHelper } from '@kit.MediaLibraryKit';
import { BusinessError } from '@kit.BasicServicesKit';
import { PostItem } from '../components/PostModel';

@Entry
@Component
struct AddPost {
  @State content: string = '';
  @State selectImage: string = ''; // 存储选中的图片路径
  @StorageLink('globalPosts') posts: PostItem[] = [];

  // 选择图片的方法
  async selectPhoto() {
    try {
      let photoSelectOptions = new photoAccessHelper.PhotoSelectOptions();

      // 注意这里：是 PhotoViewMIMETypes (加S) 且 IMAGE_TYPE
      photoSelectOptions.MIMEType = photoAccessHelper.PhotoViewMIMETypes.IMAGE_TYPE;
      photoSelectOptions.maxSelectNumber = 1;

      let photoPicker = new photoAccessHelper.PhotoViewPicker();
      let photoSelectResult = await photoPicker.select(photoSelectOptions);

      if (photoSelectResult && photoSelectResult.photoUris.length > 0) {
        this.selectImage = photoSelectResult.photoUris[0];
      }
    } catch (err) {
      let error = err as BusinessError;
      console.error(`SelectPhoto failed: ${error.code} ${error.message}`);
    }
  }

  build() {
    Column() {
      // 顶部工具栏
      Row() {
        Text('取消').onClick(() => router.back()).fontSize(16)
        Text('发表动态').fontWeight(FontWeight.Bold).fontSize(18)
        Button('发表').onClick(() => {
          // 严格对应构造函数的 9 个参数
          const newPost = new PostItem(
            Date.now(),               // 1. id
            $r('app.media.user1'),    // 2. avatar
            '我的昵称',               // 3. nickname
            '刚刚',                   // 4. publishTime
            this.content,             // 5. content
            0,                        // 6. likeCount
            false,                    // 7. isLiked
            [],                       // 8. comments (现在是 CommentItem 类型的空数组)
            this.selectImage          // 9. image (可选参数)
          );

          this.posts = [newPost, ...this.posts];
          router.back();
        })
      }.width('100%').justifyContent(FlexAlign.SpaceBetween).padding(16)

      // 1. 缩小后的文字输入框
      TextArea({ placeholder: '分享新鲜事...', text: this.content })
        .height(120) // 给定固定高度，不再占据剩余所有空间
        .margin({ left: 16, right: 16, bottom: 20 })
        .onChange(v => this.content = v)
        .backgroundColor(Color.Transparent) // 让视觉更清爽

      // 2. 放大后的图片展示区域
      Scroll() { // 使用滚动容器，防止图片太大撑破布局
        Column() {
          if (this.selectImage) {
            Stack({ alignContent: Alignment.TopEnd }) {
              Image(this.selectImage)
                .width('90%') // 宽度占满大部分屏幕
                .aspectRatio(1) // 保持正方形，或者根据需要调整
                .borderRadius(12)
                .objectFit(ImageFit.Cover)

              // 删除按钮
              Button('✕')
                .width(30).height(30)
                .backgroundColor('rgba(0,0,0,0.5)')
                .margin({ top: 10, right: '7%' })
                .onClick(() => this.selectImage = '')
            }
            .width('100%')
            .displayPriority(1)
          } else {
            // 引导上传的框也做大一点
            Column() {
              Text('+').fontSize(60).fontColor('#BDC1C6').fontWeight(FontWeight.Lighter)
              Text('添加图片').fontSize(16).fontColor('#BDC1C6')
            }
            .width('90%')
            .aspectRatio(1.2) // 稍微扁一点的长方形大框
            .backgroundColor('#F7F8FA')
            .borderRadius(12)
            .border({ width: 1, color: '#E4E7ED', style: BorderStyle.Dashed })
            .justifyContent(FlexAlign.Center)
            .onClick(() => this.selectPhoto())
          }
        }
        .width('100%')
        .alignItems(HorizontalAlign.Center)
      }
      .layoutWeight(1) // 让图片区域占据剩下的屏幕空间
    }
    .width('100%')
    .height('100%')
    .backgroundColor(Color.White)
  }
}

// 简单的加号组件
@Component
struct IconAdd {
  build() {
    Text('+').fontSize(30).fontColor(Color.Gray)
  }
}