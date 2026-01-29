// entry/src/main/ets/components/Post.ets
import router from '@ohos.router';
import { PostItem, CommentItem, initPostData } from '../viewmodel/PostModel';
import { dbManager } from '../utils/DatabaseManager';
import { UserModel } from '../viewmodel/UserModel'

@Component
export struct Post {
  @StorageLink('globalPosts') posts: PostItem[] = [];
  @State editingPostId: number = -1;
  @State tempComment: string = '';
  @StorageLink('currentUser') currentUser: UserModel | null = null;

  async aboutToAppear() {
    initPostData();
    await this.refreshTimeline();
  }

  async refreshTimeline() {
    // 确保从对象中提取手机号
    const myPhone = this.currentUser?.phoneNumber;

    if (!dbManager.postHandler || !dbManager.friend || !myPhone) {
      console.warn('Post: 缺少必要组件或未登录');
      return;
    }

    // 1. 获取好友手机号
    const friendPhones = await dbManager.friend.queryFriendsPhones(myPhone);

    // 2. 调用 PostHandler 的过滤查询
    this.posts = await dbManager.postHandler.queryTimeline(myPhone, friendPhones);
  }
  // 点赞逻辑（同步数据库）
  async toggleLike(postId: number) {
    if (!this.currentUser) return;
    const myPhone = this.currentUser.phoneNumber;

    if (dbManager.postHandler) {
      // 此时 result 是最新的点赞总数
      const newCount = await dbManager.postHandler.toggleLike(postId, myPhone);

      let index = this.posts.findIndex(p => p.id === postId);
      if (index !== -1) {
        this.posts[index].likeCount = newCount;
        // 这里的 isLiked 状态建议根据数据库的 hasLiked 逻辑同步，或者直接取反
        this.posts[index].isLiked = !this.posts[index].isLiked;
        this.posts = [...this.posts];
      }
    }
  }

  // 提交评论（同步数据库）
  async submitComment() {
    if (this.tempComment.trim().length === 0) return;
    let index = this.posts.findIndex(p => p.id === this.editingPostId);
    if (index !== -1) {
      const newComment: CommentItem = {
        userId: 999,
        userName: '我',
        commentContent: this.tempComment
      };
      this.posts[index].comments.push(newComment);

      // 更新数据库
      await dbManager.postHandler?.updatePost(this.posts[index]);

      this.posts = [...this.posts];
      this.closeCommentInput();
    }
  }

  closeCommentInput() {
    this.editingPostId = -1;
    this.tempComment = '';
  }

  build() {
    Stack({ alignContent: Alignment.Bottom }) {
      Column() {
        // 导航栏
        Row() {
          Text('动态广场').fontSize(22).fontWeight(FontWeight.Bold)
          Button('发布动态', { type: ButtonType.Capsule })
            .height(30)
            .onClick(() => router.pushUrl({ url: 'pages/AddPost' }))
        }
        .width('100%').padding(16).justifyContent(FlexAlign.SpaceBetween)

        if (this.posts.length > 0) {
          List() {
            ForEach(this.posts, (item: PostItem) => {
              ListItem() {
                PostItemComponent({
                  post: item,
                  onLikeHandler: (id: number): Promise<void> => this.toggleLike(id),
                  onCommentHandler: (id: number) => this.editingPostId = id
                })
              }
              .margin({ bottom: 12, left: 12, right: 12 })
            }, (item: PostItem) => item.id.toString() + item.isLiked + item.comments.length)
          }
          .layoutWeight(1).edgeEffect(EdgeEffect.Spring)
        } else {
          Column() { Text('空空如也...').fontColor('#999') }.layoutWeight(1).justifyContent(FlexAlign.Center)
        }
      }
      .width('100%').height('100%').backgroundColor('#F5F5F5')

      // 评论输入弹窗
      if (this.editingPostId !== -1) {
        Column() {
          Rect().width('100%').layoutWeight(1).fillOpacity(0).onClick(() => this.closeCommentInput())
          Row() {
            TextInput({ placeholder: '写下你的评论...', text: this.tempComment })
              .layoutWeight(1).onChange(v => this.tempComment = v)
            Button('发送').margin({ left: 10 }).onClick(() => this.submitComment())
          }
          .width('100%').padding(12).backgroundColor(Color.White)
        }
        .width('100%').height('100%')
      }
    }
  }
}

// 子组件：PostItemComponent (保持原样，仅做展示)
@Component
struct PostItemComponent {
  @ObjectLink post: PostItem;
  onLikeHandler: (id: number) => void = (id) => {};
  onCommentHandler: (id: number) => void = (id) => {};

  build() {
    Column() {
      // 用户信息
      Row() {
        Image(this.post.avatar).width(40).height(40).borderRadius(20).margin({ right: 10 })
        Column() {
          Text(this.post.nickname).fontSize(15).fontWeight(FontWeight.Bold)
          Text(this.post.publishTime).fontSize(11).fontColor('#999')
        }.alignItems(HorizontalAlign.Start)
      }.width('100%').margin({ bottom: 10 })

      Text(this.post.content).fontSize(14).width('100%').margin({ bottom: 10 })

      if (this.post.image) {
        Image(this.post.image).width('100%').height(250).objectFit(ImageFit.Cover).borderRadius(8)
      }

      // 点赞/评论 按钮
      Row() {
        Row() {
          Image(this.post.isLiked ? $r('app.media.post_liked') : $r('app.media.post_like')).width(18)
          Text(`${this.post.likeCount}`).fontSize(13).margin({ left: 4 })
        }.onClick(() => this.onLikeHandler(this.post.id))

        Row() {
          Image($r('app.media.post_message')).width(18)
          Text(`${this.post.comments.length}`).fontSize(13).margin({ left: 4 })
        }.margin({ left: 24 }).onClick(() => this.onCommentHandler(this.post.id))
      }.width('100%').justifyContent(FlexAlign.End).margin({ top: 12 })

      // 评论展示
      if (this.post.comments.length > 0) {
        Column() {
          ForEach(this.post.comments, (c: CommentItem) => {
            Text(`${c.userName}: ${c.commentContent}`).fontSize(12).margin({ top: 2 })
          })
        }.width('100%').padding(8).backgroundColor('#F7F7F7').margin({ top: 10 })
      }
    }.padding(16).backgroundColor(Color.White).borderRadius(12)
  }
}