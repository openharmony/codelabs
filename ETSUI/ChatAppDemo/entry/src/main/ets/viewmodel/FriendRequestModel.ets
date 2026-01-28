export interface FriendRequestModel {
  id?: number;
  fromPhone: string; // 申请人手机号
  toPhone: string;   // 被申请人手机号
  fromName: string;  // 申请人昵称
  message: string;   // 验证消息
  status: number;    // 0:待处理, 1:已通过, 2:已拒绝
}