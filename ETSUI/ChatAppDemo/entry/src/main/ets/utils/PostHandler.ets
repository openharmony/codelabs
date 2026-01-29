// entry/src/main/ets/utils/PostHandler.ts
import { relationalStore } from '@kit.ArkData';
import { PostItem, CommentItem } from '../viewmodel/PostModel';

export class PostHandler {
  private store: relationalStore.RdbStore;

  constructor(store: relationalStore.RdbStore) {
    this.store = store;
  }

  /**
   * 关键修改：查询自己和好友的动态
   * @param myPhone 当前用户手机号
   * @param friendPhones 好友的手机号数组
   */
  /**
   * 严格过滤：只显示自己和已通过好友的动态
   */
  async queryTimeline(myPhone: string, friendPhones: string[]): Promise<PostItem[]> {
    let predicates = new relationalStore.RdbPredicates('Post');

    // 构造“白名单”：我自己 + 我通过的好友
    const allowedPhones = [myPhone, ...friendPhones];

    // 数据库过滤：owner 必须在白名单内
    predicates.in('owner', allowedPhones);
    predicates.orderByDesc('id'); // 最新发布的在前

    let resultSet = await this.store.query(predicates);
    const posts: PostItem[] = [];

    while (resultSet.goToNextRow()) {
      posts.push(new PostItem(
        resultSet.getLong(resultSet.getColumnIndex('id')),
        resultSet.getString(resultSet.getColumnIndex('avatar')),
        resultSet.getString(resultSet.getColumnIndex('nickname')),
        resultSet.getString(resultSet.getColumnIndex('publishTime')),
        resultSet.getString(resultSet.getColumnIndex('content')),
        resultSet.getLong(resultSet.getColumnIndex('likeCount')),
        resultSet.getLong(resultSet.getColumnIndex('isLiked')) === 1,
        JSON.parse(resultSet.getString(resultSet.getColumnIndex('comments'))),
        resultSet.getString(resultSet.getColumnIndex('image'))
      ));
    }
    resultSet.close();
    return posts;
  }

  // 插入动态
  async insertPost(post: PostItem, owner: string) {
    const valueBucket: relationalStore.ValuesBucket = {
      owner: owner,
      avatar: typeof post.avatar === 'string' ? post.avatar : '',
      nickname: post.nickname,
      publishTime: post.publishTime,
      content: post.content,
      image: typeof post.image === 'string' ? post.image : '',
      likeCount: post.likeCount,
      isLiked: post.isLiked ? 1 : 0,
      comments: JSON.stringify(post.comments) // 关键：数组转JSON字符串
    };
    return await this.store.insert('Post', valueBucket);
  }

  // 查询所有动态
  async queryAllPosts(): Promise<PostItem[]> {
    let predicates = new relationalStore.RdbPredicates('Post');
    predicates.orderByDesc('id'); // ID即时间戳，实现倒序排列
    let resultSet = await this.store.query(predicates);
    const posts: PostItem[] = [];

    while (resultSet.goToNextRow()) {
      posts.push(new PostItem(
        resultSet.getLong(resultSet.getColumnIndex('id')),
        resultSet.getString(resultSet.getColumnIndex('avatar')),
        resultSet.getString(resultSet.getColumnIndex('nickname')),
        resultSet.getString(resultSet.getColumnIndex('publishTime')),
        resultSet.getString(resultSet.getColumnIndex('content')),
        resultSet.getLong(resultSet.getColumnIndex('likeCount')),
        resultSet.getLong(resultSet.getColumnIndex('isLiked')) === 1,
        JSON.parse(resultSet.getString(resultSet.getColumnIndex('comments'))) as CommentItem[],
        resultSet.getString(resultSet.getColumnIndex('image'))
      ));
    }
    resultSet.close();
    return posts;
  }

  // 更新动态（点赞或评论）
  async updatePost(post: PostItem) {
    let predicates = new relationalStore.RdbPredicates('Post');
    predicates.equalTo('id', post.id);
    const valueBucket: relationalStore.ValuesBucket = {
      likeCount: post.likeCount,
      isLiked: post.isLiked ? 1 : 0,
      comments: JSON.stringify(post.comments)
    };
    return await this.store.update(valueBucket, predicates);
  }

  async toggleLike(postId: number, myPhone: string): Promise<number> {
    let predicates = new relationalStore.RdbPredicates('LikeRecord');
    predicates.equalTo('postId', postId).and().equalTo('userPhone', myPhone);
    let resultSet = await this.store.query(predicates);
    let hasLiked = resultSet.rowCount > 0;
    resultSet.close();

    if (hasLiked) {
      await this.store.delete(predicates);
      await this.store.executeSql(`UPDATE Post SET likeCount = CASE WHEN likeCount > 0 THEN likeCount - 1 ELSE 0 END WHERE id = ?`, [postId]);
    } else {
      await this.store.insert('LikeRecord', { postId: postId, userPhone: myPhone });
      await this.store.executeSql(`UPDATE Post SET likeCount = likeCount + 1 WHERE id = ?`, [postId]);
    }

    // 关键：重新查询最新的点赞数并返回
    let queryPredicates = new relationalStore.RdbPredicates('Post');
    queryPredicates.equalTo('id', postId);
    let postResult = await this.store.query(queryPredicates);
    postResult.goToFirstRow();
    let count = postResult.getLong(postResult.getColumnIndex('likeCount'));
    postResult.close();
    return count; // 返回真实的数字
  }
}