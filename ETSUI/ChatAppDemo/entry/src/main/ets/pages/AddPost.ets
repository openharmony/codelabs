// entry/src/main/ets/pages/AddPost.ets
import router from '@ohos.router';
import promptAction from '@ohos.promptAction';
import { photoAccessHelper } from '@kit.MediaLibraryKit';
import { PostItem } from '../viewmodel/PostModel';
import { dbManager } from '../utils/DatabaseManager';
import { UserModel } from '../viewmodel/UserModel';

@Entry
@Component
struct AddPost {
  @State content: string = '';
  @State selectImage: string = '';
  @StorageLink('globalPosts') posts: PostItem[] = [];

  // 登录时保存的手机号，用于关联发布者身份
  @StorageLink('currentUser') currentUser: UserModel | null = null;

  /**
   * 发布动态逻辑
   */
  // AddPost.ets 中的核心插入逻辑补充
  async publish() {
    if (!this.currentUser)
    {
      promptAction.showToast({ message: '请先登录后再发表' });
      return;
    }
    if (!dbManager.postHandler) {
      promptAction.showToast({ message: '数据库初始化失败' });
      console.error('Database Error: postHandler is null');
      return;
    }

    const newPost = new PostItem(
      Date.now(),
      this.currentUser.avatarPath || '', // 确保不是 undefined
      this.currentUser.nickName || '匿名', // 确保不是 undefined
      '刚刚',
      this.content || '',
      0,
      false,
      [],
      this.selectImage || '' // 确保不是 undefined
    );

    if (dbManager.postHandler) {
      try {
        // 1. 核心修改：接收 insertPost 返回的数据库真实 ID
        const realId = await dbManager.postHandler.insertPost(newPost, this.currentUser.phoneNumber || '');

        // 2. 关键：将真实 ID 回填给对象
        newPost.id = realId;

        // 3. 再更新 UI 内存数组，这样点赞和评论就能找到正确的 ID 了
        this.posts = [newPost, ...this.posts];

        router.back();
      } catch (e) {
        console.error('PublishError', JSON.stringify(e));
      }
    }
  }
  /**
   * 选择图片
   */
  async selectPhoto() {
    try {
      let photoPicker = new photoAccessHelper.PhotoViewPicker();
      let result = await photoPicker.select({
        MIMEType: photoAccessHelper.PhotoViewMIMETypes.IMAGE_TYPE,
        maxSelectNumber: 1
      });
      if (result.photoUris.length > 0) {
        this.selectImage = result.photoUris[0];
      }
    } catch (err) {
      console.error('选择图片失败:', JSON.stringify(err));
    }
  }

  build() {
    Column() {
      // --- 顶部导航 ---
      Row() {
        Text('取消')
          .fontSize(16)
          .fontColor('#666')
          .onClick(() => router.back())

        Text('发表动态')
          .fontSize(18)
          .fontWeight(FontWeight.Bold)

        Button('发表', { type: ButtonType.Capsule })
          .width(70)
          .height(32)
          .enabled(this.content.trim().length > 0 || this.selectImage !== '')
          .onClick(() => this.publish())
      }
      .width('100%')
      .justifyContent(FlexAlign.SpaceBetween)
      .padding(16)

      // --- 输入区 ---
      TextArea({ placeholder: '这一刻的想法...', text: this.content })
        .height(180)
        .margin({ bottom: 20 })
        .onChange(v => this.content = v)
        .backgroundColor(Color.Transparent)
        .placeholderColor('#999')

      // --- 图片预览/选择区 ---
      Column() {
        if (this.selectImage) {
          Stack({ alignContent: Alignment.TopEnd }) {
            Image(this.selectImage)
              .width('100%')
              .height(300)
              .borderRadius(8)
              .objectFit(ImageFit.Contain)

            // 删除图片按钮
            Button('✕')
              .width(24)
              .height(24)
              .backgroundColor('rgba(0,0,0,0.5)')
              .onClick(() => this.selectImage = '')
              .margin({ top: 8, right: 8 })
          }
        } else {
          // 添加图片占位按钮
          Column() {
            Text('+')
              .fontSize(40)
              .fontColor('#CCC')
            Text('上传图片')
              .fontSize(14)
              .fontColor('#999')
          }
          .width(100)
          .height(100)
          .backgroundColor('#F5F5F5')
          .borderRadius(8)
          .justifyContent(FlexAlign.Center)
          .onClick(() => this.selectPhoto())
          .alignSelf(ItemAlign.Start)
        }
      }
      .width('100%')
      .padding({ left: 16, right: 16 })

      Blank()
    }
    .width('100%')
    .height('100%')
    .backgroundColor(Color.White)
  }
}