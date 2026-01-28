// entry/src/main/ets/viewmodel/Message.ets

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