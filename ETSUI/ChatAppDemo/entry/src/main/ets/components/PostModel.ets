// PostModel.ets
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

  // 修复：构造函数现在接收 8 个参数 (image 是可选的，放在最后)
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

export function initPostData() {
  const defaultPosts = [
    new PostItem(1, $r('app.media.user3'), '小哈', '2小时前', '今天天气真好，出门散步啦～',
      28, false, [{ userId: 101, userName: '小萌', commentContent: '好羡慕！' }], undefined),
    new PostItem(2, $r('app.media.user2'), '程序员小李', '昨天 18:30', '终于写完了ArkTS的页面',
      156, true, [], $r('app.media.user6'))
  ];

  // 检查是否为空或不存在
  let currentPosts = AppStorage.Get<PostItem[]>('globalPosts');
  if (!currentPosts || currentPosts.length === 0) {
    AppStorage.SetOrCreate('globalPosts', defaultPosts);
  }
}