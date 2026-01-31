/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

export default interface Message {
  id: string;
  avatar: ResourceStr;
  nickName: string;
  content: string; // 聊天具体内容
  time: string;
  isSelf: boolean; // 区分是否为自己发送
}

// 保留你之前列表页使用的接口，或者进行兼容处理
export interface ChatListItem {
  avatar: Resource;
  nickName: string;
  lastMsg: string;
  time: string;
}