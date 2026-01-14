/*
 * Copyright (c) 2026 Beijing Institute of Technology
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

import { RdbUtil, DbPost, UserInfo } from '../utils/RdbUtil'
import promptAction from '@ohos.promptAction'
import Context from '@ohos.app.ability.common'
import router from '@ohos.router'

@Component
export struct CommunityComp {
  // 广场组件：展示帖子列表，支持发帖/点赞/删帖
  @State private posts: DbPost[] = []
  @State private showPostModal: boolean = false
  @State private newPostContent: string = ''
  @State private newPostTags: string = ''
  @State private userName: string = ''
  @State private userAvatarKey: string = 'user'
  
  // 外部触发刷新（例如登录态变化后）
  @Prop @Watch('onTriggerChange') updateTrigger: number = 0

  public onNeedLogin?: (tip?: string) => void

  // 进入组件时加载用户与帖子
  async aboutToAppear() {
    await this.loadUser()
    await this.loadPosts()
  }

  // 收到外部刷新信号后重新拉取数据
  private onTriggerChange() {
    this.loadUser()
    this.loadPosts()
  }

  // 读取当前登录用户
  private async loadUser() {
    const ctx = getContext(this) as Context
    const u = await RdbUtil.getCurrentUser(ctx)
    if (u) {
      this.userName = u.name
      this.userAvatarKey = u.avatarKey
    } else {
      this.userName = ''
      this.userAvatarKey = 'user'
    }
  }

  // 拉取帖子列表，并带上当前用户的点赞状态
  private async loadPosts() {
    const ctx = getContext(this) as Context
    this.posts = await RdbUtil.getPosts(ctx, this.userName)
  }

  // 发布帖子：校验登录与内容，成功后刷新列表并关闭弹窗
  private async submitPost() {
    if (this.userName === '') {
      promptAction.showToast({ message: '请先登录' })
      return
    }
    if (this.newPostContent.trim() === '') {
      promptAction.showToast({ message: '请输入内容' })
      return
    }

    const ctx = getContext(this) as Context
    const now = new Date()
    const p = new DbPost(
      `post_${now.getTime()}`,
      this.userName,
      this.userAvatarKey,
      this.newPostContent,
      this.newPostTags,
      now.toISOString().split('T')[0],
      0,
      0
    )
    
    const ok = await RdbUtil.createPost(ctx, p)
    if (ok) {
      this.newPostContent = ''
      this.newPostTags = ''
      this.showPostModal = false
      await this.loadPosts()
      promptAction.showToast({ message: '发布成功' })
    } else {
      promptAction.showToast({ message: '发布失败' })
    }
  }

  // 删除帖子：只允许删除自己的帖子，未登录则引导登录
  private async deletePost(post: DbPost) {
    if (this.userName === '') {
      if (this.onNeedLogin) {
        this.onNeedLogin('请登录后删除帖子')
      } else {
        router.pushUrl({ url: 'pages/MyPage', params: { tip: '请登录后删除帖子', backOnSuccess: true, isPageMode: true } })
      }
      return
    }
    if (post.userName !== this.userName) {
      promptAction.showToast({ message: '只能删除自己的帖子' })
      return
    }
    const ctx = getContext(this) as Context
    const ok = await RdbUtil.deletePost(ctx, post.id)
    if (ok) {
      await this.loadPosts()
      promptAction.showToast({ message: '删除成功' })
    } else {
      promptAction.showToast({ message: '删除失败' })
    }
  }

  private avatarRes(key: string): Resource {
    if (key === 'qblz') return $r('app.media.qblz')
    if (key === 'nbzc') return $r('app.media.nbzc')
    if (key === 'jjh') return $r('app.media.jjh')
    if (key === 'zcyl') return $r('app.media.zcyl')
    return $r('app.media.user')
  }

  @Builder postCard(post: DbPost) {
    Column({ space: 12 }) {
      Column({ space: 12 }) {
        Row({ space: 10 }) {
          Image(this.avatarRes(post.avatarKey))
            .width(40)
            .height(40)
            .borderRadius(20)
            .objectFit(ImageFit.Cover)
          
          Column({ space: 4 }) {
            Text(post.userName)
              .fontSize(15)
              .fontWeight(FontWeight.Bold)
              .fontColor('#333')
          }
          .layoutWeight(1)
          .alignItems(HorizontalAlign.Start)

          Text(post.createdAt)
            .fontSize(12)
            .fontColor('#BBB')
        }

        Text(post.content)
          .fontSize(15)
          .fontColor('#333')
          .lineHeight(24)
          .maxLines(5)
          .textOverflow({ overflow: TextOverflow.Ellipsis })

        if (post.tags && post.tags.length > 0) {
          Row({ space: 8 }) {
            ForEach(post.tags.split(','), (tag: string) => {
               if (tag.trim().length > 0) {
                 Text(tag.trim())
                   .fontSize(12)
                   .fontColor('#4C8FA9')
                   .backgroundColor('#F0F7FA')
                   .padding({ left: 10, right: 10, top: 4, bottom: 4 })
                   .borderRadius(12)
               }
            })
          }
        }
      }
      .width('100%')
      .onClick(() => {
        router.pushUrl({
          url: 'pages/PostDetail',
          params: { post: post }
        })
      })

      Row() {
        Row({ space: 4 }) {
           Image($r('app.media.comment'))
             .width(14)
             .height(14)
           Text(`${post.commentCount}`)
             .fontSize(12)
             .fontColor('#666')
        }
        Blank().layoutWeight(1)
        Row({ space: 6 }) {
           Row() {
             Text('点赞')
               .fontSize(12)
               .fontColor(post.isLiked ? '#D14343' : '#666')
           }
           .padding({ left: 8, right: 8, top: 4, bottom: 4 })
           .backgroundColor(post.isLiked ? '#FDEBEC' : '#F5F5F5')
           .borderRadius(12)
           .onClick(async () => {
               if (this.userName === '') {
                 if (this.onNeedLogin) {
                   this.onNeedLogin('请登录后点赞')
                 } else {
                   router.pushUrl({
                     url: 'pages/MyPage',
                     params: { tip: '请登录后点赞', backOnSuccess: true, isPageMode: true }
                   })
                 }
                 return
               }
               const ctx = getContext(this) as Context
               await RdbUtil.togglePostLike(ctx, this.userName, post.id)
               await this.loadPosts()
             })
           Text(`${post.likeCount}`)
             .fontSize(12)
             .fontColor(post.isLiked ? '#D14343' : '#666')
           if (this.userName === post.userName && this.userName !== '') {
             Blank().width(8)
             Text('删除')
               .fontSize(12)
               .fontColor('#FF4040')
               .onClick(async () => {
                 await this.deletePost(post)
               })
           }
        }
      }
      .alignItems(VerticalAlign.Center)
      .padding({ top: 8 })
    }
    .padding(16)
    .backgroundColor('#FFFFFF')
    .borderRadius(12)
    .width('100%')

  }

  build() {
    Stack({ alignContent: Alignment.BottomEnd }) {
      Column() {
         Text('广场')
           .fontSize(20)
           .fontWeight(FontWeight.Bold)
           .width('100%')
           .padding(16)
           .backgroundColor('#FFFFFF')
           .border({ width: { bottom: 0.5 }, color: '#EAEAEA' })

         List({ space: 12 }) {
           ForEach(this.posts, (p: DbPost) => {
             ListItem() {
               this.postCard(p)
             }
           })
         }
         .layoutWeight(1)
         .padding({ left: 16, right: 16, top: 16, bottom: 80 })
      }
      .width('100%')
      .height('100%')
      .backgroundColor('#F5F5F5')

      Button({ type: ButtonType.Circle, stateEffect: true }) {
        Text('+')
          .fontSize(30)
          .fontColor(Color.White)
          .fontWeight(FontWeight.Lighter)
          .margin({ bottom: 4 })
      }
      .width(56)
      .height(56)
      .margin({ right: 24, bottom: 15 })
      .shadow({ radius: 6, color: 0x40000000, offsetY: 4 })
      .backgroundColor('#111')
      .onClick(() => {
         if (this.userName === '') {
           if (this.onNeedLogin) {
             this.onNeedLogin()
           } else {
             promptAction.showToast({ message: '请先登录' })
           }
           return
         }
         this.showPostModal = true
      })

      if (this.showPostModal) {
         Stack() {
           Column()
             .width('100%')
             .height('100%')
             .backgroundColor('rgba(0,0,0,0.5)')
             .onClick(() => this.showPostModal = false)

           Column({ space: 16 }) {
              Text('发布动态')
                .fontSize(18)
                .fontWeight(FontWeight.Bold)
              
              TextArea({ placeholder: '分享你的读书/观影感受...', text: this.newPostContent })
                .height(120)
                .onChange((v) => this.newPostContent = v)
              
              TextInput({ placeholder: '标签 (用逗号分隔)', text: this.newPostTags })
                .onChange((v) => this.newPostTags = v)

              Row({ space: 16 }) {
                Button('取消')
                  .backgroundColor('#F5F5F5')
                  .fontColor('#333')
                  .onClick(() => this.showPostModal = false)
                  .layoutWeight(1)
                
                Button('发布')
                  .backgroundColor('#111')
                  .fontColor('#FFF')
                  .onClick(() => this.submitPost())
                  .layoutWeight(1)
              }
           }
           .padding(20)
           .backgroundColor('#FFF')
           .borderRadius(16)
           .width('85%')
         }
         .width('100%')
         .height('100%')
         .alignContent(Alignment.Center)
         .zIndex(999)
      }
    }
    .width('100%')
    .height('100%')
  }
}