// entry/src/main/ets/viewmodel/Message.ets

export default interface Message {
  id: string;
  avatar: Resource;
  nickName: string;
  content: string; // 聊天具体内容
  time: string;
  isSelf: boolean; // 区分是否为自己发送
}