import { router, promptAction } from '@kit.ArkUI';
import { Product, User } from '../model/DataModels';

type CommentStore = Record<string, CommentItem[]>;

interface CommentItem {
  id: string;
  userName: string;
  vip?: string;
  content: string;
  createTime: number;
  location?: string;
  likeCount: number;
  liked: boolean;
  replyToName?: string;
  parentId?: string;
}

@Entry
@Component
// 评论列表页面
struct CommentList {
  @StorageLink('currentUser') currentUser: User | undefined = undefined;

  @State product: Product = new Product(0, '', 0, '', '', '', 0);
  @State comments: CommentItem[] = [];
  @State inputText: string = '';

  @State replyToId: string = '';
  @State replyToName: string = '';

  aboutToAppear() {
    const params = router.getParams() as Record<string, Object>;
    if (params && params['product']) {
      this.product = params['product'] as Product;
    }

    this.loadComments();
  }

  private getProductKey(): string {
    return (this.product?.id ?? 0).toString();
  }

  private getStore(): CommentStore {
    const store = AppStorage.get('commentStore') as CommentStore | undefined;
    return store ? store : {};
  }

  private setStore(store: CommentStore) {
    AppStorage.setOrCreate('commentStore', store);
  }

  private formatTime(ts: number): string {
    const now = new Date().getTime();
    const diff = Math.max(0, now - ts);
    const minute = 60 * 1000;
    const hour = 60 * minute;
    const day = 24 * hour;
    const month = 30 * day;

    if (diff < 2 * minute) return '刚刚';
    if (diff < hour) return `${Math.max(1, Math.floor(diff / minute))}分钟前`;
    if (diff < day) return `${Math.max(1, Math.floor(diff / hour))}小时前`;
    if (diff < month) return `${Math.max(1, Math.floor(diff / day))}天前`;
    return `${Math.max(1, Math.floor(diff / month))}个月前`;
  }

  private getUserAvatar(userName: string): Resource {
    switch ((userName || '').toString()) {
      case '秋本丽子':
        return $r('app.media.qblz');
      case '拟宝珠缠':
        return $r('app.media.nbzc');
      case '中村由利':
        return $r('app.media.zcyl');
      case '居间惠':
        return $r('app.media.jjh');
      default:
        return $r('app.media.user');
    }
  }

  private buildSeedComments(): CommentItem[] {
    const now = new Date().getTime();
    const base = now - 30 * 24 * 60 * 60 * 1000;
    const base2 = now - 60 * 24 * 60 * 60 * 1000;

    const name = (this.product?.name || '').toString();

    return [
      {
        id: `${now}-1`,
        userName: '秋本丽子',
        vip: '5VIP',
        content: name ? `这${name}真的挺不错的` : '还行',
        createTime: base,
        location: '浙江',
        likeCount: 2,
        liked: false
      },
      {
        id: `${now}-2`,
        userName: '拟宝珠缠',
        content: '挺满意的',
        createTime: base,
        location: '北京',
        likeCount: 1,
        liked: false
      },
      {
        id: `${now}-3`,
        userName: '中村由利',
        content: name ? `${name}还可以` : '挺好的',
        createTime: base2,
        location: '广东',
        likeCount: 0,
        liked: false
      },
      {
        id: `${now}-4`,
        userName: '居间惠',
        content: '回复@中村由利：还挺不错的',
        createTime: base,
        location: '北京',
        likeCount: 1,
        liked: false,
        replyToName: '中村由利',
        parentId: `${now}-3`
      }
    ];
  }

  private ensureSeeded(store: CommentStore, key: string) {
    const list = store[key];
    if (list && list.length > 0) return;
    store[key] = this.buildSeedComments();
  }

  // 加载评论
  private loadComments() {
    const key = this.getProductKey();
    const store = this.getStore();
    this.ensureSeeded(store, key);
    this.setStore(store);
    this.comments = (store[key] || []).slice();
  }

  private persist() {
    const key = this.getProductKey();
    const store = this.getStore();
    store[key] = this.comments.slice();
    this.setStore(store);
  }

  private ensureLoggedIn(): boolean {
    if (this.currentUser) return true;
    promptAction.showToast({ message: '请先登录' });
    router.pushUrl({ url: 'pages/LoginPage' });
    return false;
  }

  private isMyComment(c: CommentItem): boolean {
    const u = this.currentUser ? this.currentUser.username : '';
    if (!u) return false;
    return c.userName === u || c.userName === '我';
  }

  private toggleLikeById(id: string) {
    if (!this.ensureLoggedIn()) return;
    const idx = this.comments.findIndex(c => c.id === id);
    if (idx < 0) return;
    const c = this.comments[idx];
    const nextLiked = !c.liked;
    const nextCount = Math.max(0, c.likeCount + (nextLiked ? 1 : -1));
    const next: CommentItem = {
      id: c.id,
      userName: c.userName,
      vip: c.vip,
      content: c.content,
      createTime: c.createTime,
      location: c.location,
      likeCount: nextCount,
      liked: nextLiked,
      replyToName: c.replyToName
    };
    this.comments[idx] = next;
    this.comments = this.comments.slice();
    this.persist();
  }

  private deleteById(id: string) {
    if (!this.ensureLoggedIn()) return;

    const idx = this.comments.findIndex(c => c.id === id);
    if (idx < 0) return;

    const target = this.comments[idx];
    if (!this.isMyComment(target)) {
      promptAction.showToast({ message: '只能删除自己的评论' });
      return;
    }

    this.comments = this.comments.filter(c => c.id !== id && c.parentId !== id);
    this.persist();
    promptAction.showToast({ message: '已删除' });
  }

  private clearReplyTarget() {
    this.replyToId = '';
    this.replyToName = '';
  }

  private replyTo(id: string) {
    if (!this.ensureLoggedIn()) return;
    const c = this.comments.find(x => x.id === id);
    if (!c) return;
    this.replyToId = c.id;
    this.replyToName = c.userName;
  }

  // 发送评论
  private send() {
    if (!this.ensureLoggedIn()) return;

    const text = (this.inputText || '').trim();
    if (!text) return;

    const isReply = !!this.replyToId && !!this.replyToName;
    const now = new Date().getTime();
    const item: CommentItem = {
      id: `${now}`,
      userName: this.currentUser ? this.currentUser.username : '我',
      content: isReply ? `回复@${this.replyToName}：${text}` : text,
      createTime: now,
      location: '',
      likeCount: 0,
      liked: false,
      replyToName: isReply ? this.replyToName : undefined,
      parentId: isReply ? this.replyToId : undefined
    };

    const next = this.comments.slice();
    if (isReply) {
      const idx = next.findIndex(c => c.id === this.replyToId);
      if (idx >= 0) {
        next.splice(idx + 1, 0, item);
      } else {
        next.unshift(item);
      }
    } else {
      next.unshift(item);
    }

    this.comments = next;
    this.inputText = '';
    this.clearReplyTarget();
    this.persist();
  }

  build() {
    Column() {
      Row() {
        Button({ type: ButtonType.Normal, stateEffect: true }) {
          Text('<')
            .fontSize(24)
            .fontColor('#111827');
        }
        .backgroundColor(Color.Transparent)
        .width(44)
        .height(44)
        .onClick(() => {
          router.back();
        });

        Text('评论')
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .fontColor('#111827');

        Blank();
      }
      .width('100%')
      .height(56)
      .padding({ left: 10, right: 10 })
      .backgroundColor(Color.White);

      List() {
        ForEach(this.comments, (c: CommentItem) => {
          ListItem() {
            Row({ space: 12 }) {
              Image(this.getUserAvatar(c.userName))
                .width((c.parentId || c.replyToName) ? 28 : 36)
                .height((c.parentId || c.replyToName) ? 28 : 36)
                .borderRadius((c.parentId || c.replyToName) ? 14 : 18)
                .backgroundColor('#F3F4F6')
                .margin({ top: 2 });

              Column({ space: 6 }) {
                Row({ space: 8 }) {
                  Text(c.userName)
                    .fontSize(13)
                    .fontColor('#9CA3AF');

                  if (c.vip) {
                    Text(c.vip)
                      .fontSize(10)
                      .fontColor('#D97706')
                      .backgroundColor('#FEF3C7')
                      .padding({ left: 6, right: 6, top: 2, bottom: 2 })
                      .borderRadius(8);
                  }
                }

                Text(c.content)
                  .fontSize(18)
                  .fontWeight(FontWeight.Medium)
                  .fontColor('#111827');

                Row({ space: 14 }) {
                  Text(this.formatTime(c.createTime))
                    .fontSize(12)
                    .fontColor('#9CA3AF');

                  if (c.location) {
                    Text(c.location)
                      .fontSize(12)
                      .fontColor('#9CA3AF');
                  }

                  Text('回复')
                    .fontSize(12)
                    .fontColor('#6B7280')
                    .onClick(() => {
                      this.replyTo(c.id);
                    });

                  if (this.isMyComment(c)) {
                    Text('删除')
                      .fontSize(12)
                      .fontColor('#EF4444')
                      .onClick(() => {
                        this.deleteById(c.id);
                      });
                  }

                  Blank();
                }
              }
              .layoutWeight(1);

              Row({ space: 6 }) {
                Text(c.liked ? '♥' : '♡')
                  .fontSize(18)
                  .fontColor(c.liked ? '#FF4D4F' : '#9CA3AF')
                  .onClick(() => {
                    this.toggleLikeById(c.id);
                  });

                if (c.likeCount > 0) {
                  Text(`${c.likeCount}`)
                    .fontSize(14)
                    .fontColor('#9CA3AF');
                }
              }
              .alignItems(VerticalAlign.Center);
            }
            .padding({ left: 16, right: 16, top: 14, bottom: 14 })
            .margin({ left: (c.parentId || c.replyToName) ? 38 : 0 });
          }
        });
      }
      .layoutWeight(1)
      .backgroundColor(Color.White);

      Column({ space: 8 }) {
        if (this.replyToId && this.replyToName) {
          Row({ space: 8 }) {
            Text(`回复 @${this.replyToName}`)
              .fontSize(12)
              .fontColor('#6B7280');

            Blank();

            Text('取消')
              .fontSize(12)
              .fontColor('#EF4444')
              .onClick(() => {
                this.clearReplyTarget();
              });
          }
          .width('100%')
          .padding({ left: 6, right: 6, top: 6 });
        }

        Row({ space: 10 }) {
          Image(this.currentUser ? this.getUserAvatar(this.currentUser.username) : $r('app.media.user'))
            .width(28)
            .height(28)
            .borderRadius(14)
            .backgroundColor('#F3F4F6');

          TextInput({
            placeholder: !this.currentUser ? '请先登录后评论' : (this.replyToName ? `回复 @${this.replyToName}` : '说说你的想法吧~'),
            text: this.inputText
          })
            .layoutWeight(1)
            .height(40)
            .backgroundColor('#F3F4F6')
            .borderRadius(20)
            .padding({ left: 12, right: 12 })
            .onChange((v: string) => {
              this.inputText = v;
            });

          Button('☺')
            .type(ButtonType.Normal)
            .width(40)
            .height(40)
            .backgroundColor(Color.Transparent)
            .fontColor('#6B7280')
            .onClick(() => {
              promptAction.showToast({ message: '表情功能开发中' });
            });

          Button('发送')
            .type(ButtonType.Normal)
            .height(40)
            .backgroundColor(this.currentUser && (this.inputText || '').trim() ? '#1698CE' : '#D1D5DB')
            .fontColor(Color.White)
            .borderRadius(20)
            .enabled(!!this.currentUser && !!(this.inputText || '').trim())
            .onClick(() => {
              this.send();
            });
        }
      }
      .width('100%')
      .padding({ left: 12, right: 12, top: 10, bottom: 10 })
      .backgroundColor(Color.White)
      .shadow({ radius: 10, color: '#1A000000', offsetY: -2 });
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F5F5F5');
  }
}