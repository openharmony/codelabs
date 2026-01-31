/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


import router from '@ohos.router';
import { PostItem, CommentItem, initPostData } from '../viewmodel/PostModel';
import { dbManager } from '../utils/DatabaseManager';
import { UserModel } from '../viewmodel/UserModel'
import { promptAction } from '@kit.ArkUI';

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

  // Post.ets 内部方法
  async submitComment() {
    // 1. 校验输入内容
    if (this.tempComment.trim().length === 0) return;

    // 2. 校验登录状态（确保评论能关联到人）
    if (!this.currentUser) {
      promptAction.showToast({ message: $r('app.string.toast_login_required') });
      return;
    }

    let index = this.posts.findIndex(p => p.id === this.editingPostId);
    if (index !== -1) {
      // 3. 构造与账号绑定的评论对象
      const newComment: CommentItem = {
        // 这里的字段名需对应你的 CommentItem 接口定义
        userId: Number(this.currentUser.phoneNumber), // 使用手机号作为唯一标识
        userName: this.currentUser.nickName || '匿名用户', // 绑定当前昵称
        commentContent: this.tempComment
      };

      // 4. 更新内存数据
      // 注意：由于 @Observed/@ObjectLink 机制，建议先备份再推送
      this.posts[index].comments.push(newComment);

      try {
        // 5. 同步到数据库
        if (dbManager.postHandler) {
          await dbManager.postHandler.updatePost(this.posts[index]);
          promptAction.showToast({ message: $r('app.string.toast_comment_success') });
        }
      } catch (e) {
        console.error('Comment Error:', JSON.stringify(e));
        promptAction.showToast({ message: $r('app.string.toast_comment_fail') });
      }

      // 6. 刷新 UI 并关闭输入框
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
          Text($r('app.string.title_post_square')).fontSize(22).fontWeight(FontWeight.Bold)
          Button($r('app.string.btn_publish_post'), { type: ButtonType.Capsule })
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
          Column() { Text($r('app.string.label_empty_list')).fontColor('#999') }.layoutWeight(1).justifyContent(FlexAlign.Center)
        }
      }
      .width('100%').height('100%').backgroundColor('#F5F5F5')

      // 评论输入弹窗
      if (this.editingPostId !== -1) {
        Column() {
          Rect().width('100%').layoutWeight(1).fillOpacity(0).onClick(() => this.closeCommentInput())
          Row() {
            TextInput({ placeholder: $r('app.string.hint_write_comment'), text: this.tempComment })
              .layoutWeight(1).onChange(v => this.tempComment = v)
            Button($r('app.string.btn_send')).margin({ left: 10 }).onClick(() => this.submitComment())
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