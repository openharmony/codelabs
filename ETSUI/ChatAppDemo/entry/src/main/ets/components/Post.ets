import router from '@ohos.router';
import { PostItem, CommentItem, initPostData } from './PostModel';

@Component
export struct Post {
  // 使用 @StorageLink 与全局数据同步
  @StorageLink('globalPosts') posts: PostItem[] = [];

  // 评论输入相关状态
  @State editingPostId: number = -1;
  @State tempComment: string = '';

  aboutToAppear() {
    // 确保组件加载时 AppStorage 有初始数据
    initPostData();
    this.posts = AppStorage.Get<PostItem[]>('globalPosts') || [];
  }

  // 点赞逻辑：通过 ID 找到帖子并更新
  toggleLike(postId: number) {
    let index = this.posts.findIndex(p => p.id === postId);
    if (index !== -1) {
      // 1. 修改属性
      this.posts[index].isLiked = !this.posts[index].isLiked;
      this.posts[index].likeCount += this.posts[index].isLiked ? 1 : -1;

      // 2. 核心操作：替换数组中的该对象实例，确保 ObjectLink 收到通知
      let newPost = this.posts[index];
      this.posts.splice(index, 1, newPost);

      // 3. 同步回 AppStorage (如果需要全局持久化)
      AppStorage.SetOrCreate('globalPosts', this.posts);
    }
  }

  // 提交评论逻辑
  submitComment() {
    if (this.tempComment.trim().length === 0) return;

    let index = this.posts.findIndex(p => p.id === this.editingPostId);
    if (index !== -1) {
      const newComment: CommentItem = {
        userId: 999, // 模拟当前用户ID
        userName: '我',
        commentContent: this.tempComment
      };
      this.posts[index].comments.push(newComment);
      this.posts = [...this.posts]; // 触发更新
    }
    this.closeCommentInput();
  }

  closeCommentInput() {
    this.editingPostId = -1;
    this.tempComment = '';
  }

  build() {
    Stack({ alignContent: Alignment.Bottom }) {
      Column() {
        // --- 顶部导航栏 ---
        Row() {
          Text('动态广场').fontSize(22).fontWeight(FontWeight.Bold)
          Button('发布动态', { type: ButtonType.Capsule })
            .backgroundColor('#007DFF')
            .height(30)
            .fontSize(14)
            .onClick(() => {
              router.pushUrl({ url: 'pages/AddPost' })
            })
        }
        .width('100%')
        .justifyContent(FlexAlign.SpaceBetween)
        .padding(16)
        .backgroundColor(Color.White)

        // --- 动态列表 ---
        List() {
          ForEach(this.posts, (item: PostItem) => {
            ListItem() {
              PostItemComponent({
                post: item,
                // 传递点赞回调
                onLikeHandler: (id: number): void => {
                  this.toggleLike(id);
                },
                onCommentHandler: (id: number): void => {
                  this.editingPostId = id;
                }
              })
            }
            .margin({ bottom: 12, left: 12, right: 12 })
            .backgroundColor(Color.White)
            .padding(16)
            .borderRadius(12)
          }, (item: PostItem) => item.id.toString() + item.isLiked + item.comments.length)
        }
        .layoutWeight(1)
        .width('100%')
        .backgroundColor('#F5F5F5')
        .edgeEffect(EdgeEffect.Spring) // 弹性滚动
      }
      .width('100%')
      .height('100%')

      // --- 底部评论输入弹窗 ---
      if (this.editingPostId !== -1) {
        Column() {
          // 遮罩层：点击空白处关闭
          Rect().width('100%').layoutWeight(1).fillOpacity(0).onClick(() => this.closeCommentInput())

          Row() {
            TextInput({ placeholder: '写下你的评论...', text: this.tempComment })
              .layoutWeight(1)
              .onChange(v => this.tempComment = v)
              .onSubmit(() => this.submitComment())

            Button('发送')
              .margin({ left: 10 })
              .onClick(() => this.submitComment())
          }
          .width('100%')
          .padding(12)
          .backgroundColor(Color.White)
          .border({ width: { top: 1 }, color: '#EEE' })
        }
        .width('100%')
        .height('100%')
      }
    }
  }
}

// --- 子组件：单个帖子卡片 ---
@Component
struct PostItemComponent {
  @ObjectLink post: PostItem;
  onLikeHandler: (id: number) => void = (id) => {};
  onCommentHandler: (id: number) => void = (id) => {};

  build() {
    Column() {
      // 1. 个人信息
      Row() {
        Image(this.post.avatar).width(40).height(40).borderRadius(20).margin({ right: 10 })
        Column() {
          Text(this.post.nickname).fontSize(15).fontWeight(FontWeight.Bold)
          Text(this.post.publishTime).fontSize(11).fontColor('#999')
        }.alignItems(HorizontalAlign.Start)
      }.width('100%').margin({ bottom: 10 })

      // 2. 文字内容
      Text(this.post.content).fontSize(14).width('100%').margin({ bottom: 10 })

      // 3. 图片展示 (适配 URI 和 Resource)
      if (this.post.image) {
        Image(this.post.image)
          .width('100%')
          .height(250)
          .objectFit(ImageFit.Cover)
          .borderRadius(8)
          .margin({ bottom: 10 })
      }

      // 4. 底部按钮
      Row() {
        // 点赞
        Row() {
          Image(this.post.isLiked ? $r('app.media.post_liked') : $r('app.media.post_like'))
            .width(18).height(18)
          Text(`${this.post.likeCount}`).fontSize(13).margin({ left: 4 })
        }
        .onClick(() => this.onLikeHandler(this.post.id))

        // 评论
        Row() {
          Image($r('app.media.post_message')).width(18).height(18)
          Text(`${this.post.comments.length}`).fontSize(13).margin({ left: 4 })
        }
        .margin({ left: 24 })
        .onClick(() => this.onCommentHandler(this.post.id))
      }
      .width('100%')
      .justifyContent(FlexAlign.End)
      .margin({ top: 8 })

      // 5. 评论列表区域
      if (this.post.comments.length > 0) {
        Column() {
          ForEach(this.post.comments, (c: CommentItem) => {
            Row() {
              Text(`${c.userName}: `).fontSize(12).fontWeight(FontWeight.Bold).fontColor('#555')
              Text(c.commentContent).fontSize(12).fontColor('#333')
            }
            .width('100%')
            .margin({ top: 4 })
          })
        }
        .width('100%')
        .padding(8)
        .backgroundColor('#F7F7F7')
        .borderRadius(4)
        .margin({ top: 10 })
      }
    }
    .alignItems(HorizontalAlign.Start)
  }
}