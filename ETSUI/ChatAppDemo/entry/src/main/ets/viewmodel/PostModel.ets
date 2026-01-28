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