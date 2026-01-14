import { RdbUtil } from '../utils/RdbUtil';
import { Product, CartItem, User } from '../model/DataModels';
import { promptAction, router } from '@kit.ArkUI';
import { CartView } from './CartView';
import { MineView } from './MineView';

@Entry
@Component
// 首页：包含商品列表、购物车和个人中心
struct Index {
  @StorageLink('currentUser') currentUser: User | undefined = undefined;

  @State currentIndex: number = 0;
  @State productList: Product[] = [];
  @State shownProductList: Product[] = [];
  @State searchKeyword: string = '';
  private tabsController: TabsController = new TabsController();

  // 处理Tab跳转逻辑
  private applyTargetTab() {
    const raw = AppStorage.get('indexTargetTab') as number | undefined;
    const next = raw === undefined ? NaN : Number(raw);
    if (!Number.isFinite(next) || next < 0) return;

    if (next === 1 && !this.currentUser) {
      promptAction.showToast({ message: '请先登录' });
      router.pushUrl({ url: 'pages/LoginPage' });
      AppStorage.setOrCreate('indexTargetTab', -1);
      return;
    }

    this.currentIndex = next;
    this.tabsController.changeIndex(next);
    AppStorage.setOrCreate('indexTargetTab', -1);
  }

  async aboutToAppear() {
    this.applyTargetTab();
    await this.loadProducts();
  }

  // 加载商品数据
  async loadProducts() {
    let products = await RdbUtil.getAllProducts();
    const changed = await this.initMockData(products);
    if (changed) {
      products = await RdbUtil.getAllProducts();
    }
    this.productList = products.slice();
    this.applySearch(this.searchKeyword);
  }

  // 初始化模拟数据
  async initMockData(existingProducts: Product[] = []): Promise<boolean> {
    const buildKey = (p: Product): string => {
      const name = (p.name || '').toString();
      const category = (p.category || '').toString();
      const price = Number(p.price);
      const desc = (p.description || '').toString();
      return `${name}||${category}||${price}||${desc}`;
    };
    const existingKeySet: Set<string> = new Set<string>(existingProducts.map(buildKey));

    const mockProducts: Product[] = [
      new Product(0, '智能手机 Pro', 5999, $r('app.media.smartphone'), '电子', '高性能旗舰手机', 50),
      new Product(0, '蓝牙耳机', 299, $r('app.media.earphones'), '电子', '降噪入耳式耳机', 100),
      new Product(0, '纯棉T恤', 99, $r('app.media.shirt'), '服饰', '舒适透气', 200),
      new Product(0, '休闲运动鞋', 399, $r('app.media.sneaker'), '服饰', '轻便耐磨', 80),
      new Product(0, '每日坚果', 129, $r('app.media.nut'), '食品', '健康零食', 150),
      new Product(0, '有机牛奶', 68, $r('app.media.milk'), '食品', '高钙营养', 120),
      new Product(0, '榨汁机', 159, $r('app.media.juicer'), '家电', '榨汁，鲜榨', 60),
      new Product(0, '水杯', 50, $r('app.media.bottle'), '生活用品', '便携', 100),
      new Product(0, 'GeForce RTX 5080', 12559, $r('app.media.gpu'), '电子', '显卡', 40),
      new Product(0, '牙膏', 20, $r('app.media.toothpaste'), '生活用品', '清洁牙齿', 70),
    ];

    let inserted = false;
    for (const p of mockProducts) {
      const key = buildKey(p);
      if (existingKeySet.has(key)) {
        continue;
      }
      await RdbUtil.insertProduct(p);
      inserted = true;
      existingKeySet.add(key);
    }
    return inserted;
  }

  @Builder
  TabBuilder(title: Resource, targetIndex: number, selectedImg: Resource, normalImg: Resource) {
    Column() {
      Image(this.currentIndex === targetIndex ? selectedImg : normalImg)
        .size({ width: 25, height: 25 })
      Text(title)
        .fontColor(this.currentIndex === targetIndex ? '#1698CE' : '#6B6B6B')
        .fontSize(10)
    }
    .width('100%')
    .height(50)
    .justifyContent(FlexAlign.Center)
    .onClick(() => {
      if (targetIndex === 1 && !this.currentUser) {
        promptAction.showToast({ message: '请先登录' });
        router.pushUrl({ url: 'pages/LoginPage' });
        return;
      }
      this.currentIndex = targetIndex;
      this.tabsController.changeIndex(this.currentIndex);
    })
  }

  private applySearch(keyword: string): number {
    const kw = (keyword || '').trim().toLowerCase();
    if (!kw) {
      this.shownProductList = this.productList.slice();
      return this.shownProductList.length;
    }

    const filtered = this.productList.filter((p: Product) => {
      const name = (p.name || '').toString().toLowerCase();
      const desc = (p.description || '').toString().toLowerCase();
      const cat = (p.category || '').toString().toLowerCase();
      return name.includes(kw) || desc.includes(kw) || cat.includes(kw);
    });

    this.shownProductList = filtered.slice();
    return filtered.length;
  }

  private doSearch() {
    const kw = (this.searchKeyword || '').trim();
    if (!kw) {
      this.applySearch('');
      promptAction.showToast({ message: '已清除搜索' });
      return;
    }

    const count = this.applySearch(kw);
    if (count === 0) {
      promptAction.showToast({ message: '未找到相关商品' });
    }
  }

  @Builder
  HomeSearchBar() {
    Row({ space: 10 }) {
      TextInput({ placeholder: '搜索商品', text: this.searchKeyword })
        .layoutWeight(1)
        .height(34)
        .fontSize(14)
        .backgroundColor(Color.Transparent)
        .padding({ left: 0, right: 0 })
        .onChange((v: string) => {
          this.searchKeyword = v;
        });

      Button('搜索')
        .type(ButtonType.Normal)
        .height(34)
        .fontSize(14)
        .fontColor(Color.White)
        .backgroundColor($r('app.color.mine_background'))
        .borderRadius(17)
        .padding({ left: 16, right: 16 })
        .onClick(() => {
          this.doSearch();
        });
    }
    .width('100%')
    .height(42)
    .padding({ left: 12, right: 6 })
    .backgroundColor(Color.White)
    .border({ width: 1, color: $r('app.color.mine_background') })
    .borderRadius(21)
    .margin({ left: 12, right: 12, top: 12, bottom: 6 });
  }

  @Builder
  HomeView() {
    Column() {
      this.HomeSearchBar();

      Text($r('app.string.home_title'))
        .fontSize(20)
        .fontWeight(FontWeight.Bold)
        .margin({ top: 6, bottom: 10 })

      Grid() {
        ForEach(this.shownProductList, (item: Product) => {
          GridItem() {
            Column() {
              Image(item.image)
                .width('100%')
                .height(150)
                .borderRadius(12)
                .objectFit(ImageFit.Cover)

              Column({ space: 6 }) {
                Text(item.name)
                  .fontSize(14)
                  .fontWeight(FontWeight.Bold)
                  .fontColor('#111827')
                  .maxLines(2)
                  .textOverflow({ overflow: TextOverflow.Ellipsis })

                Text(item.description)
                  .fontSize(12)
                  .fontColor('#6B7280')
                  .maxLines(1)
                  .textOverflow({ overflow: TextOverflow.Ellipsis })

                Row({ space: 8 }) {
                  Text(`¥${item.price}`)
                    .fontSize(16)
                    .fontWeight(FontWeight.Bold)
                    .fontColor('#FF5000');

                  Text(`库存${item.stock}`)
                    .fontSize(10)
                    .fontColor('#1698CE')
                    .backgroundColor('#E6F7FF')
                    .padding({ left: 6, right: 6, top: 2, bottom: 2 })
                    .borderRadius(10);

                  Blank();
                }
              }
              .padding({ left: 10, right: 10, top: 10, bottom: 12 })
            }
            .backgroundColor(Color.White)
            .borderRadius(16)
            .shadow({ radius: 8, color: '#12000000', offsetY: 2 })
            .onClick(() => {
              router.pushUrl({
                url: 'pages/ProductDetail',
                params: { product: item }
              });
            })
          }
        })
      }
      .columnsTemplate('1fr 1fr')
      .columnsGap(10)
      .rowsGap(10)
      .width('100%')
      .layoutWeight(1)
      .padding({ left: 12, right: 12 })
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F1F3F5')
  }

  @Builder
  CartItemSwipe(item: CartItem) {
  }

  build() {
    Tabs({ barPosition: BarPosition.End, controller: this.tabsController }) {
      TabContent() {
        this.HomeView()
      }
      .tabBar(this.TabBuilder($r('app.string.tab_home'), 0, $r('app.media.home'), $r('app.media.home')))

      TabContent() {
        CartView({
          currentIndex: this.currentIndex,
          onSwitchTab: (index: number) => {
            this.currentIndex = index;
            this.tabsController.changeIndex(index);
          }
        })
      }
      .tabBar(this.TabBuilder($r('app.string.tab_cart'), 1, $r('app.media.cart5'), $r('app.media.cart5')))

      TabContent() {
        MineView()
      }
      .tabBar(this.TabBuilder($r('app.string.tab_mine'), 2, $r('app.media.mine'), $r('app.media.mine')))
    }
    .onChange((index: number) => {
      this.currentIndex = index;
      if (index === 0) {
        this.loadProducts();
      }
    })
    .onVisibleAreaChange([0.0, 1.0], (isVisible: boolean, currentRatio: number) => {
      if (isVisible) {
        this.applyTargetTab();
      }
    })
  }
}