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

export interface CommentItem {
  userId: number;
  userName: string;
  commentContent: string;
}

@Observed
export class PostItem {
  id: number;
  avatar: string | Resource;
  nickname: string;
  publishTime: string;
  content: string;
  image?: string | Resource;
  likeCount: number;
  isLiked: boolean;
  comments: Array<CommentItem>;

  constructor(id: number, avatar: string | Resource, nickname: string, publishTime: string,
    content: string, likeCount: number, isLiked: boolean, comments: Array<CommentItem>,
    image?: string | Resource) {
    this.id = id;
    this.avatar = avatar;
    this.nickname = nickname;
    this.publishTime = publishTime;
    this.content = content;
    this.likeCount = likeCount;
    this.isLiked = isLiked;
    this.comments = comments;
    this.image = image;
  }
}

/**
 * 修改后的初始化函数：不再添加默认数据
 */
export function initPostData() {
  let currentPosts = AppStorage.Get<PostItem[]>('globalPosts');
  if (!currentPosts) {
    // 初始状态设为空数组
    AppStorage.SetOrCreate('globalPosts', []);
  }
}