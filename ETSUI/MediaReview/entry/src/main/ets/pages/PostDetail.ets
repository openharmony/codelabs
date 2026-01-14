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

import router from '@ohos.router'
import { RdbUtil, DbPost, DbComment, UserInfo } from '../utils/RdbUtil'
import promptAction from '@ohos.promptAction'
import Context from '@ohos.app.ability.common'

@Entry
@Component
struct PostDetail {
  // 帖子详情页：读取路由参数中的帖子，加载评论列表，支持发布/删除评论
  @State post: DbPost | null = null
  @State comments: DbComment[] = []
  @State inputContent: string = ''
  @State userName: string = ''
  @State userAvatarKey: string = 'user'

  // 页面回到前台时刷新用户信息
  onPageShow() {
    this.loadUser()
  }

  // 首次进入页面：初始化帖子数据并拉取评论
  async aboutToAppear() {
    const params = router.getParams() as Record<string, Object>
    if (params && params['post']) {
      const p = params['post'] as DbPost
      this.post = new DbPost(
        p.id, p.userName, p.avatarKey, p.content, p.tags, p.createdAt, p.likeCount, p.commentCount
      )
    }

    await this.loadUser()
    if (this.post) {
      await this.loadComments()
    }
  }

  // 获取当前登录用户（用于评论身份与删除权限判断）
  async loadUser() {
    const ctx = getContext(this) as Context
    const u = await RdbUtil.getCurrentUser(ctx)
    if (u) {
      this.userName = u.name
      this.userAvatarKey = u.avatarKey
    }
  }

  // 加载当前帖子的评论列表
  async loadComments() {
    if (!this.post) return
    const ctx = getContext(this) as Context
    this.comments = await RdbUtil.getComments(ctx, this.post.id)
  }

  // 发布评论：未登录引导登录，成功后刷新列表与数量
  async submitComment() {
    if (this.userName === '') {
      promptAction.showToast({ message: '请先登录后评论' })
      router.pushUrl({
        url: 'pages/MyPage',
        params: {
          tip: '请先登录后评论',
          backOnSuccess: true,
          isPageMode: true
        }
      })
      return
    }
    if (this.inputContent.trim() === '') {
      promptAction.showToast({ message: '请输入内容' })
      return
    }
    if (!this.post) return

    const ctx = getContext(this) as Context
    const now = new Date()
    const comment = new DbComment(
      `c_${now.getTime()}`,
      this.post.id,
      this.userName,
      this.userAvatarKey,
      this.inputContent,
      now.toISOString().split('T')[0]
    )

    const success = await RdbUtil.addComment(ctx, comment)
    if (success) {
      this.inputContent = ''
      if (this.post) {
        this.post.commentCount++
        this.post = new DbPost(
          this.post.id, this.post.userName, this.post.avatarKey, this.post.content,
          this.post.tags, this.post.createdAt, this.post.likeCount, this.post.commentCount
        )
      }
      await this.loadComments()
      promptAction.showToast({ message: '评论成功' })
    } else {
      promptAction.showToast({ message: '评论失败' })
    }
  }

  // 删除评论：仅删除成功时更新数量并刷新列表
  async deleteComment(comment: DbComment) {
    const ctx = getContext(this) as Context
    const success = await RdbUtil.deleteComment(ctx, comment)
    if (success) {
      promptAction.showToast({ message: '已删除' })
      if (this.post) {
        this.post.commentCount = Math.max(0, this.post.commentCount - 1)
        this.post = new DbPost(
          this.post.id, this.post.userName, this.post.avatarKey, this.post.content,
          this.post.tags, this.post.createdAt, this.post.likeCount, this.post.commentCount
        )
      }
      await this.loadComments()
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

  build() {
    Column() {
      Row() {
        Text('<')
          .fontSize(24)
          .padding({ left: 16, right: 16 })
          .onClick(() => router.back())
        
        Text('详情')
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .layoutWeight(1)
          .textAlign(TextAlign.Center)
          .margin({ right: 56 })
      }
      .width('100%')
      .height(56)
      .backgroundColor('#FFF')
      .border({ width: { bottom: 0.5 }, color: '#EEE' })

      if (this.post) {
        List() {
          ListItem() {
            Column({ space: 12 }) {
              Row({ space: 10 }) {
                Image(this.avatarRes(this.post.avatarKey))
                  .width(40)
                  .height(40)
                  .borderRadius(20)
                
                Column({ space: 4 }) {
                  Text(this.post.userName)
                    .fontSize(15)
                    .fontWeight(FontWeight.Bold)
                  Text(this.post.createdAt)
                    .fontSize(12)
                    .fontColor('#BBB')
                }
              }
              .width('100%')
              .alignItems(VerticalAlign.Center)

              Text(this.post.content)
                .fontSize(16)
                .lineHeight(24)
                .width('100%')

              if (this.post.tags) {
                Row({ space: 8 }) {
                  ForEach(this.post.tags.split(','), (tag: string) => {
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

              Row({ space: 6 }) {
              Text(`全部评论`)
                .fontSize(14)
                .fontWeight(FontWeight.Bold)
              Text(`(${this.post.commentCount})`)
                  .fontSize(14)
                  .fontColor('#7A6F66')
              }
              .margin({ top: 16, bottom: 8 })
              .alignItems(VerticalAlign.Center)
            }
            .padding(16)
            .backgroundColor('#FFF')
            .alignItems(HorizontalAlign.Start)
          }

          ForEach(this.comments, (c: DbComment) => {
            ListItem() {
              Row({ space: 10 }) {
                Image(this.avatarRes(c.avatarKey))
                  .width(32)
                  .height(32)
                  .borderRadius(16)
                  .margin({ top: 4 })
                
                Column({ space: 4 }) {
                  Row() {
                    Text(c.userName)
                      .fontSize(13)
                      .fontColor('#666')
                    Blank()
                    Text(c.createdAt)
                      .fontSize(11)
                      .fontColor('#CCC')
                  }
                  .width('100%')

                  Text(c.content)
                    .fontSize(14)
                    .fontColor('#333')
                    .lineHeight(20)

                  if (c.userName === this.userName) {
                    Row() {
                      Blank()
                      Text('删除')
                        .fontSize(12)
                        .fontColor('#FF4040')
                        .onClick(() => this.deleteComment(c))
                    }
                    .width('100%')
                    .margin({ top: 4 })
                  }
                }
                .layoutWeight(1)
                .alignItems(HorizontalAlign.Start)
              }
              .padding({ left: 16, right: 16, top: 12, bottom: 12 })
              .width('100%')
              .alignItems(VerticalAlign.Top)
              .border({ width: { bottom: 0.5 }, color: '#F5F5F5' })
            }
          })
          
          ListItem() {
            Column().height(80)
          }
        }
        .layoutWeight(1)
        .backgroundColor('#FFF')
      }

      Row({ space: 12 }) {
        TextInput({ placeholder: '说点什么...', text: this.inputContent })
          .layoutWeight(1)
          .backgroundColor('#F5F5F5')
          .onChange((v) => this.inputContent = v)
        
        Button('发送')
          .fontSize(14)
          .height(32)
          .backgroundColor('#111')
          .fontColor('#FFF')
          .onClick(() => this.submitComment())
      }
      .padding(12)
      .backgroundColor('#FFF')
      .border({ width: { top: 0.5 }, color: '#EAEAEA' })
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#FFF')
  }
}