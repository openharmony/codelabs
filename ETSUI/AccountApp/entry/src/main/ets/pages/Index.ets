import router from '@ohos.router';
import promptAction from '@ohos.promptAction';
import common from '@ohos.app.ability.common';
import pasteboard from '@ohos.pasteboard';
import Want from '@ohos.app.ability.Want';
import { BusinessError } from '@ohos.base';
import RdbHelper from '../database/RdbHelper';
import AppAccount from '../model/AppAccount';
import PreferenceUtil from '../utils/PreferenceUtil';

@Entry
@Component
struct Index {
  @State currentUsername: string = 'User';
  @State currentUserId: number = 0;
  @State searchText: string = '';
  @State accountList: Array<AppAccount> = [];
  @State allAccounts: Array<AppAccount> = [];
  @State isVisibleMap: Record<number, boolean> = {};

  // --- 【找回】健康体检状态 ---
  @State securityScore: number = 100;
  @State weakCount: number = 0;
  @State duplicateCount: number = 0;

  private rdbHelper: RdbHelper | null = null;

  aboutToAppear(): void {
    // 1. 尝试从路由参数获取
    try {
      let params = router.getParams() as Record<string, Object>;
      if (params) {
        if (params['username']) this.currentUsername = params['username'] as string;
        if (params['userId']) this.currentUserId = params['userId'] as number;
      }
    } catch (e) { console.error(`Params error: ${JSON.stringify(e)}`); }

    let context = getContext(this) as common.UIAbilityContext;

    // 2. 兜底逻辑：自动登录
    if (this.currentUserId === 0) {
      PreferenceUtil.init(context).then(async () => {
        const info = await PreferenceUtil.getLoginInfo();
        if (info) {
          this.currentUsername = info.username;
          this.currentUserId = info.userId;
          this.loadData();
        }
      });
    }

    try {
      this.rdbHelper = new RdbHelper(context);
      this.rdbHelper.initRdbStore((isSuccess: boolean) => {
        if (isSuccess) this.loadData();
      });
    } catch (e) { console.error(`DB error: ${JSON.stringify(e)}`); }
  }

  onPageShow(): void {
    this.loadData();
  }

  loadData(): void {
    if (!this.rdbHelper || this.currentUserId === 0) return;
    this.rdbHelper.queryAppAccounts(this.currentUserId).then((list) => {
      this.allAccounts = list;
      // 【找回】加载完数据后，立即进行体检
      this.analyzeSecurity(list);
      this.filterData();
    });
  }

  // --- 【找回】核心功能：密码健康分析 ---
  analyzeSecurity(list: Array<AppAccount>): void {
    let weak = 0;
    let score = 100;

    // 1. 检查重复密码
    const pwdMap = new Map<string, number>();
    let duplicates = 0;

    list.forEach(item => {
      const pwd = item.getPassword();

      // 统计重复
      if (pwdMap.has(pwd)) {
        let count = pwdMap.get(pwd);
        if (count !== undefined) {
          pwdMap.set(pwd, count + 1);
        }
      } else {
        pwdMap.set(pwd, 1);
      }

      // 检查弱密码 (长度<6 或 纯数字/纯字母)
      const isShort = pwd.length < 6;
      const isSimple = /^[0-9]+$/.test(pwd) || /^[a-zA-Z]+$/.test(pwd);
      if (isShort || isSimple) {
        weak++;
      }
    });

    // 计算重复数
    pwdMap.forEach((count) => {
      if (count > 1) duplicates += (count - 1);
    });

    // 扣分逻辑
    score -= (weak * 10);
    score -= (duplicates * 15);
    if (score < 0) score = 0;

    // 更新状态
    this.weakCount = weak;
    this.duplicateCount = duplicates;
    this.securityScore = score;
  }

  filterData(): void {
    let keyword = this.searchText.trim().toLowerCase();
    if (keyword === '') {
      this.accountList = this.allAccounts;
    } else {
      this.accountList = this.allAccounts.filter((item) => {
        return item.getPlatformName().toLowerCase().includes(keyword) ||
        item.getAccountName().toLowerCase().includes(keyword);
      });
    }
  }

  safeToast(msg: string): void {
    try { promptAction.showToast({ message: msg }); } catch (e) {}
  }

  getAvatarColor(name: string): string {
    const n = name.toLowerCase();
    if (n.includes('微信') || n.includes('wechat')) return '#07C160';
    if (n.includes('支付宝') || n.includes('alipay')) return '#1677FF';
    if (n.includes('淘宝') || n.includes('taobao')) return '#FF5000';
    if (n.includes('京东') || n.includes('jd')) return '#E1251B';
    if (n.includes('qq')) return '#12B7F5';
    if (n.includes('抖音') || n.includes('tiktok')) return '#1C1C1C';
    if (n.includes('google') || n.includes('谷歌')) return '#DB4437';
    if (n.includes('bilibili')) return '#FB7299';

    const colors = ['#F56A00', '#7265E6', '#FFBF00', '#00A2AE', '#9F4D95', '#1FCEC3'];
    let index = 0;
    if (name.length > 0) {
      index = name.charCodeAt(0) % colors.length;
    }
    return colors[index];
  }

  getAvatarText(name: string): string {
    if (!name) return '?';
    return name.substring(0, 1).toUpperCase();
  }

  jumpToTargetApp(platformName: string, password: string): void {
    try {
      let data = pasteboard.createData(pasteboard.MIMETYPE_TEXT_PLAIN, password);
      let systemPasteboard = pasteboard.getSystemPasteboard();
      systemPasteboard.setData(data).then(() => {
        this.safeToast('密码已复制 (30秒后清除)');
        setTimeout(() => {
          try {
            let sysPb = pasteboard.getSystemPasteboard();
            sysPb.clear();
          } catch(e) {}
        }, 30000);
      }).catch((err: BusinessError) => {
        console.error(`Paste failed: ${err.message}`);
      });
    } catch (e) {
      console.error(`Paste error: ${JSON.stringify(e)}`);
    }

    let context = getContext(this) as common.UIAbilityContext;
    if (platformName.startsWith('http') || platformName.startsWith('www') || platformName.includes('.com')) {
      let url = platformName.startsWith('http') ? platformName : 'https://' + platformName;
      let wantInfo: Want = { action: 'ohos.want.action.viewData', entities: ['entity.system.browsable'], uri: url };
      try { context.startAbility(wantInfo).catch((err: BusinessError) => {}); } catch (e) {}
      return;
    }

    let targetBundleName = '';
    if (platformName.includes('微信')) targetBundleName = 'com.tencent.mm';
    else if (platformName.includes('淘宝')) targetBundleName = 'com.taobao.taobao';
    else if (platformName.includes('支付宝')) targetBundleName = 'com.eg.android.AlipayGphone';
    else if (platformName.includes('抖音')) targetBundleName = 'com.ss.android.ugc.aweme';
    else if (platformName.includes('QQ') || platformName.includes('qq')) targetBundleName = 'com.tencent.mobileqq';

    if (targetBundleName !== '') {
      let wantInfo: Want = { bundleName: targetBundleName, abilityName: 'EntryAbility' };
      try { context.startAbility(wantInfo).catch(() => this.safeToast('未检测到应用')); } catch (e) {}
    }
  }

  handleDelete(id: number): void {
    try {
      AlertDialog.show({
        title: '删除确认',
        message: '确定要删除这条记录吗？',
        primaryButton: { value: '取消', action: () => {} },
        secondaryButton: {
          value: '删除', fontColor: Color.Red,
          action: () => {
            this.rdbHelper?.deleteAppAccount(id).then((rows) => {
              if (rows > 0) {
                this.safeToast('已删除');
                this.loadData();
              }
            });
          }
        }
      });
    } catch (e) {}
  }

  toggleVisibility(id: number): void {
    let newMap: Record<number, boolean> = JSON.parse(JSON.stringify(this.isVisibleMap));
    newMap[id] = !newMap[id];
    this.isVisibleMap = newMap;
  }

  @Builder SwipeMenu(item: AppAccount) {
    Row() {
      Button() {
        Column() { Text('编辑').fontColor(Color.White).fontSize(14) }.justifyContent(FlexAlign.Center)
      }
      .width(60).height('100%').backgroundColor(0x007DFF).type(ButtonType.Normal)
      .onClick(() => {
        try {
          router.pushUrl({
            url: 'pages/AccountInputPage',
            params: { userId: this.currentUserId, id: item.getId(), platform: item.getPlatformName(), account: item.getAccountName(), password: item.getPassword() }
          });
        } catch (e) {}
      })

      Button() { Text('删除').fontColor(Color.White).fontSize(14) }
      .width(60).height('100%').backgroundColor(Color.Red).type(ButtonType.Normal)
      .onClick(() => { this.handleDelete(item.getId()); })
    }
    .height('100%')
  }

  // --- 【找回】健康评分卡片 UI ---
  @Builder SecurityCard() {
    Column() {
      Row() {
        Column() {
          Text('安全评分')
            .fontSize(14).fontColor(Color.White).opacity(0.9)
          Text(`${this.securityScore}`)
            .fontSize(40).fontWeight(FontWeight.Bold).fontColor(Color.White)
            .margin({ top: 5 })
        }.alignItems(HorizontalAlign.Start)

        Blank()

        // 右侧状态
        Column({ space: 8 }) {
          Row() {
            Text('⚠️ 弱密码: ').fontSize(12).fontColor(Color.White).opacity(0.9)
            Text(`${this.weakCount}`).fontSize(14).fontWeight(FontWeight.Bold).fontColor(this.weakCount > 0 ? 0xFFC107 : Color.White)
          }
          Row() {
            Text('♻️ 重复项: ').fontSize(12).fontColor(Color.White).opacity(0.9)
            Text(`${this.duplicateCount}`).fontSize(14).fontWeight(FontWeight.Bold).fontColor(this.duplicateCount > 0 ? 0xFF5252 : Color.White)
          }
        }.alignItems(HorizontalAlign.End)
      }
      .width('100%')
    }
    .width('100%')
    .padding(20)
    .backgroundColor(this.securityScore >= 90 ? 0x07C160 : (this.securityScore >= 60 ? 0xFFC107 : 0xFF5252))
    .borderRadius(16)
    .shadow({ radius: 10, color: 0x33000000, offsetY: 5 })
    .margin({ bottom: 20 })
  }

  build() {
    Column() {
      // 顶部
      Column() {
        Row() {
          Text('我的密码本').fontSize(22).fontWeight(FontWeight.Bold).fontColor(Color.White)
          Blank()

          // ⚙️ 设置按钮
          Row() {
            Text('⚙️ 设置')
              .fontSize(16).fontWeight(FontWeight.Bold).fontColor(Color.White)
          }
          .onClick(() => {
            try {
              router.pushUrl({ url: 'pages/SettingsPage' });
            } catch (e) {
              console.error(`Nav error: ${JSON.stringify(e)}`);
            }
          })
        }.width('100%').margin({ bottom: 10 })

        Text(`欢迎你，${this.currentUsername}`).fontSize(14).fontColor(Color.White).opacity(0.8).width('100%').margin({ bottom: 15 })

        Search({ value: this.searchText, placeholder: '搜索平台或账号...' })
          .width('100%').height(40).backgroundColor(Color.White).borderRadius(20)
          .onChange((value: string) => { this.searchText = value; this.filterData(); })
      }
      .width('100%').height(160).backgroundColor(0x007DFF).padding(20)

      // 列表区域 (用 Stack 覆盖方式让 List 可以滚到顶部下面)
      Column() {
        List({ space: 12 }) {
          // 1. 把“添加按钮”放进 List 的第一个 Item (Header)
          ListItem() {
            Button('+ 添加新账号')
              .width('100%').height(45).backgroundColor(Color.White).fontColor(0x007DFF)
              .border({ width: 1, color: 0x007DFF })
              .onClick(() => { try { router.pushUrl({ url: 'pages/AccountInputPage', params: { userId: this.currentUserId } }); } catch(e){} })
          }.margin({ top: 15, bottom: 5 })

          // 2. 【找回】显示体检卡片 (如果有数据的话)
          if (this.allAccounts.length > 0) {
            ListItem() {
              this.SecurityCard()
            }
          }

          // 3. 数据列表
          if (this.accountList.length === 0) {
            ListItem() {
              Column() {
                Text(this.searchText ? '未找到结果' : '空空如也').fontSize(16).fontColor(Color.Gray).margin({ top: 30 })
              }.width('100%').justifyContent(FlexAlign.Center)
            }
          } else {
            ForEach(this.accountList, (item: AppAccount) => {
              ListItem() {
                // 卡片主体
                Row() {
                  Stack({ alignContent: Alignment.Center }) {
                    Circle({ width: 50, height: 50 }).fill(this.getAvatarColor(item.getPlatformName()))
                    Text(this.getAvatarText(item.getPlatformName())).fontSize(20).fontColor(Color.White).fontWeight(FontWeight.Bold)
                  }.margin({ right: 15 })

                  Column() {
                    Row() {
                      Text(item.getPlatformName()).fontSize(18).fontWeight(FontWeight.Bold).fontColor(0x333333).layoutWeight(1)
                      Text('🚀').fontSize(16).backgroundColor(0xF0F8FF).padding(8).borderRadius(20)
                        .onClick(() => this.jumpToTargetApp(item.getPlatformName(), item.getPassword()))
                    }.width('100%').margin({ bottom: 8 })

                    Text(item.getAccountName()).fontSize(14).fontColor(Color.Gray).width('100%').margin({ bottom: 5 })
                      .onClick(() => {
                        try {
                          let data = pasteboard.createData(pasteboard.MIMETYPE_TEXT_PLAIN, item.getAccountName());
                          let sysPb = pasteboard.getSystemPasteboard();
                          sysPb.setData(data).then(() => this.safeToast(`账号已复制`));
                        } catch(e){}
                      })

                    Row() {
                      if (this.isVisibleMap[item.getId()]) {
                        Text(item.getPassword()).fontSize(16).fontColor(0x007DFF).fontWeight(FontWeight.Bold)
                      } else {
                        Text('••••••').fontSize(16).fontColor(Color.Black).letterSpacing(2)
                      }
                      Blank()
                      Text(this.isVisibleMap[item.getId()] ? '🙈' : '👁️').fontSize(18).padding(5)
                        .onClick(() => this.toggleVisibility(item.getId()))
                    }.width('100%')
                  }.layoutWeight(1)
                }
                .padding(15).backgroundColor(Color.White).borderRadius(12)
                .shadow({ radius: 5, color: 0xDDDDDD, offsetX: 2, offsetY: 2 })
              }
              .swipeAction({ end: this.SwipeMenu(item) })
            })
          }
        }
        .width('90%').height('100%').layoutWeight(1).edgeEffect(EdgeEffect.Spring)
        .padding({ left: 15, right: 15 }) // 给列表加点左右边距
      }
      .layoutWeight(1) // 填满剩余高度
      .backgroundColor(0xF0F2F5)
    }
    .width('100%').height('100%').backgroundColor(0xF0F2F5)
  }
}