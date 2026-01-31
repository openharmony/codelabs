import router from '@ohos.router';
import common from '@ohos.app.ability.common';
import Constants from '../common/constants/Constants';
import { ImagePicker } from '../components/ImagePicker';
import { PhotoModel } from '../model/PhotoModel';
import PhotoService from '../service/PhotoService';
import { LazyImage } from '../components/LazyImage';
import Logger from '../common/utils/Logger';

// 定义数据接口
interface PhotoData {
  filePath: string;
  fileName: string;
  category: string;
  tags: string;
  createTime: number;
}

// ==========================================
// 子组件:信息录入弹窗
// ==========================================
@CustomDialog
struct PhotoInfoDialog {
  controller: CustomDialogController;
  imagePath: string = '';
  @State category: string = '生活';
  @State tags: string = '';
  @State fileName: string = '';
  onSave: (data: PhotoData) => void = (data: PhotoData) => {};

  aboutToAppear() {
    this.fileName = 'IMG_' + new Date().getTime();
  }

  build() {
    Column({ space: 15 }) {
      Text('完善图片信息').fontSize(20).fontWeight(FontWeight.Bold)

      Image('file://' + this.imagePath)
        .height(150).width('100%').objectFit(ImageFit.Contain)
        .borderRadius(8).backgroundColor('#f0f0f0')

      TextInput({ placeholder: '图片名称', text: this.fileName })
        .onChange((value) => { this.fileName = value; })

      Row() {
        Text('分类:').fontSize(16)
        TextInput({ placeholder: '例如: 风景', text: this.category })
          .layoutWeight(1).onChange((value) => { this.category = value; })
      }.width('100%')

      Row() {
        Text('标签:').fontSize(16)
        TextInput({ placeholder: '例如: #旅行', text: this.tags })
          .layoutWeight(1).onChange((value) => { this.tags = value; })
      }.width('100%')

      Row({ space: 20 }) {
        Button('取消').backgroundColor('#E5E5E5').fontColor('#000')
          .onClick(() => { this.controller.close(); })
        Button('保存').onClick(() => {
          const finalData: PhotoData = {
            filePath: this.imagePath,
            fileName: this.fileName,
            category: this.category,
            tags: this.tags,
            createTime: new Date().getTime()
          };
          this.onSave(finalData);
          this.controller.close();
        })
      }
    }
    .padding(20).backgroundColor(Color.White).borderRadius(16).width('90%')
  }
}

// ==========================================
// 主页面:IndexPage (带底部Tab导航)
// ==========================================
@Entry
@Component
struct IndexPage {
  swiperController: SwiperController = new SwiperController();
  @State currentIndex: number = 0;
  @State tempSelectedPath: string = '';
  @State dbPhotoList: Array<PhotoModel> = [];

  // ==========================================
  // 🆕 Tab导航相关状态
  // ==========================================
  @State currentTabIndex: number = 0;

  aboutToAppear() {
    console.info('[IndexPage] 页面加载,正在初始化数据库...');
    PhotoService.initDB(getContext(this) as common.UIAbilityContext)
      .then(() => {
        console.info('[IndexPage] 数据库初始化完成,开始加载数据');
        this.refreshData();
      })
      .catch((err: Error) => {
        console.error('[IndexPage] 数据库启动失败: ' + JSON.stringify(err));
        Logger.showError('相册初始化失败，请稍后重试');
      });
  }

  refreshData() {
    PhotoService.queryAll().then((photos) => {
      this.dbPhotoList = photos;
      console.info(`[IndexPage] 列表已刷新,当前共有 ${this.dbPhotoList.length} 张照片`);
    });
  }

  infoDialogController: CustomDialogController = new CustomDialogController({
    builder: PhotoInfoDialog({
      imagePath: this.tempSelectedPath,
      onSave: (data: PhotoData) => {
        this.handleSaveData(data);
      }
    }),
    autoCancel: false,
    customStyle: true
  });

  pickerDialogController: CustomDialogController = new CustomDialogController({
    builder: ImagePicker({
      onImageSelected: (path: string) => {
        this.tempSelectedPath = path;
        setTimeout(() => { this.infoDialogController.open(); }, 100);
      }
    }),
    alignment: DialogAlignment.Bottom,
    customStyle: true
  });

  handleSaveData(data: PhotoData) {
    const newPhoto = new PhotoModel(0, data.fileName, data.filePath, data.category, data.createTime, data.tags);

    PhotoService.insert(newPhoto).then((rowId) => {
      console.info(`[IndexPage] 保存成功,新记录 ID: ${rowId}`);
      this.refreshData();

      AlertDialog.show({
        title: '保存成功',
        message: '图片已安全存入数据库!',
        confirm: { value: '棒', action: () => {} }
      });
    }).catch((err: Error) => {
      console.error('[IndexPage] 保存失败: ' + JSON.stringify(err));
      Logger.showError('保存失败，数据库写入错误');
    });
  }

  // ==========================================
  // 🆕 首页内容构建器
  // ==========================================
  @Builder
  HomeContent() {
    Stack({ alignContent: Alignment.BottomEnd }) {
      Column() {
        // 1. 标题栏
        Row() {
          Text($r('app.string.EntryAbility_label'))
            .fontSize($r('app.float.title_font_size'))
            .fontWeight(Constants.TITLE_FONT_WEIGHT)
        }
        .height($r('app.float.navi_bar_height'))
        .width(Constants.FULL_PERCENT)
        .padding({ left: $r('app.float.title_padding') })

        // 2. 轮播图
        Swiper(this.swiperController) {
          ForEach(Constants.BANNER_IMG_LIST, (item: Resource) => {
            Row() {
              Image(item).width(Constants.FULL_PERCENT).height(Constants.FULL_PERCENT)
            }
            .width(Constants.FULL_PERCENT).aspectRatio(Constants.BANNER_ASPECT_RATIO)
          }, (item: Resource, index?: number) => JSON.stringify(item) + index)
        }
        .autoPlay(true).loop(true)
        .margin($r('app.float.grid_padding'))
        .borderRadius($r('app.float.img_border_radius'))
        .clip(true)

        // 3. 图片网格
        if (this.dbPhotoList.length === 0) {
          Column() {
            Text('📷')
              .fontSize(60)
              .opacity(0.3)
              .margin({ top: 50 })

            Text('相册空空如也')
              .fontSize(16)
              .fontColor(Color.Gray)
              .margin({ top: 20 })

            Text('点击右下角 + 号添加照片')
              .fontSize(14)
              .fontColor('#CCCCCC')
              .margin({ top: 10 })
          }
          .layoutWeight(1)
          .width('100%')
          .justifyContent(FlexAlign.Center)
        } else {
          Grid() {
            ForEach(this.dbPhotoList, (item: PhotoModel) => {
              GridItem() {
                Column() {
                  LazyImage({
                    src: 'file://' + item.path,
                    widthValue: '100%',
                    heightValue: 100,
                    objectFit: ImageFit.Cover,
                    cornerRadius: 8,
                    placeholder: Constants.PLACEHOLDER_IMAGE,
                    errorHolder: Constants.ERROR_IMAGE
                  })

                  Text(item.name)
                    .fontSize(14)
                    .maxLines(1)
                    .textOverflow({ overflow: TextOverflow.Ellipsis })
                    .margin({ top: 5 })
                }
                .padding(5)
                .backgroundColor('#F5F5F5')
                .borderRadius(8)
              }
              .onClick(() => {
                console.info(`点击了图片: ${item.name}`);
              })
            }, (item: PhotoModel) => item.id.toString())
          }
          .columnsTemplate(Constants.INDEX_COLUMNS_TEMPLATE)
          .columnsGap($r('app.float.grid_padding'))
          .rowsGap($r('app.float.grid_padding'))
          .padding($r('app.float.grid_padding'))
          .width(Constants.FULL_PERCENT)
          .layoutWeight(1)
        }
      }
      .width(Constants.FULL_PERCENT)
      .height(Constants.FULL_PERCENT)

      // 4. 悬浮添加按钮
      Button({ type: ButtonType.Circle, stateEffect: true }) {
        Text('+')
          .fontSize(40)
          .fontColor(Color.White)
          .fontWeight(300)
          .margin({ bottom: 4 })
      }
      .width(60)
      .height(60)
      .margin({ right: 20, bottom: 30 })
      .backgroundColor('#007DFF')
      .shadow({ radius: 10, color: '#888888', offsetY: 5 })
      .onClick(() => {
        this.pickerDialogController.open();
      })
    }
    .width(Constants.FULL_PERCENT)
    .height(Constants.FULL_PERCENT)
  }

  // ==========================================
  // 🆕 页面显示时的生命周期（从ListPage返回时触发）
  // ==========================================
  onPageShow() {
    console.info('[IndexPage] 页面显示，刷新数据');
    this.currentTabIndex = 0; // 确保回到首页Tab
    this.refreshData(); // 刷新图片列表
  }

  // ==========================================
  // 🆕 主构建函数 - 使用Tabs组件
  // ==========================================
  build() {
    Tabs({ index: this.currentTabIndex }) {
      // ==========================================
      // Tab 1: 首页
      // ==========================================
      TabContent() {
        this.HomeContent()
      }
      .tabBar(this.TabBuilder(0, '首页', '🏠'))

      // ==========================================
      // Tab 2: 搜索页
      // ==========================================
      TabContent() {
        Column() {
          Text('正在跳转到搜索页...')
            .fontSize(16)
            .fontColor('#999999')
        }
        .width('100%')
        .height('100%')
        .justifyContent(FlexAlign.Center)
      }
      .tabBar(this.TabBuilder(1, '搜索', '🔍'))
    }
    .barPosition(BarPosition.End) // 底部导航
    .scrollable(false) // 禁止滑动切换
    .animationDuration(300) // 切换动画时长
    .onChange((index: number) => {
      console.info(`[IndexPage] 切换到 Tab ${index}`);
      this.currentTabIndex = index;

      // ==========================================
      // 🎯 关键:点击"搜索"Tab时跳转到ListPage
      // ==========================================
      if (index === 1) {
        router.pushUrl({
          url: 'pages/ListPage'
        }).then(() => {
          console.info('[IndexPage] 成功跳转到搜索页');
          // 跳转后立即切回首页Tab,避免UI错乱
          this.currentTabIndex = 0;
        }).catch((err: Error) => {
          console.error(`[IndexPage] 跳转失败: ${err.message}`);
          this.currentTabIndex = 0;
        });
      }
    })
  }

  // ==========================================
  // 🆕 Tab图标构建器
  // ==========================================
  @Builder
  TabBuilder(index: number, title: string, icon: string) {
    Column() {
      Text(icon)
        .fontSize(24)
        .fontColor(this.currentTabIndex === index ? '#007DFF' : '#666666')

      Text(title)
        .fontSize(12)
        .fontColor(this.currentTabIndex === index ? '#007DFF' : '#666666')
        .margin({ top: 4 })
    }
    .width('100%')
    .height(50)
    .justifyContent(FlexAlign.Center)
  }

}
