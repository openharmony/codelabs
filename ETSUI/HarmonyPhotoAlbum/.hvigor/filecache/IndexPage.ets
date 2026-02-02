/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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
  @StorageLink('selectedIndex') selectedIndex: number = 0;

  // Tab导航相关状态
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
        setTimeout(() => {
          this.infoDialogController.open();
        }, 100);
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
        confirm: {
          value: '棒', action: () => {}
        }
      });
    }).catch((err: Error) => {
      console.error('[IndexPage] 保存失败: ' + JSON.stringify(err));
      Logger.showError('保存失败，数据库写入错误');
    });
  }

  // ==========================================
  // 顶部滚动照片组件（基于 PhotoModel，与下方网格对齐）
  // ==========================================

  /**
   * 获取用于轮播展示的照片列表
   * 这里直接取 dbPhotoList 前 N 张，这样索引与网格完全一致
   */
  private getCarouselPhotos(): PhotoModel[] {
    if (!this.dbPhotoList || this.dbPhotoList.length === 0) {
      return [];
    }
    const count: number = Math.min(Constants.SHOW_COUNT, this.dbPhotoList.length);
    return this.dbPhotoList.slice(0, count);
  }

  @Builder
  FeaturedCarousel() {
    const photos = this.getCarouselPhotos();

    // 无数据时显示占位图
    if (photos.length === 0) {
      Column() {
        Image(Constants.PLACEHOLDER_IMAGE)
          .width(Constants.FULL_PERCENT)
          .aspectRatio(Constants.BANNER_ASPECT_RATIO)
          .objectFit(ImageFit.Cover)
          .borderRadius($r('app.float.img_border_radius'))
          .backgroundColor('#F0F0F0')

        Text('暂无精选照片，去添加一张吧～')
          .fontSize(14)
          .fontColor('#999999')
          .margin({ top: 12 })
      }
      .padding($r('app.float.grid_padding'))
      return;
    }

    Swiper(this.swiperController) {
      ForEach(photos, (photo: PhotoModel, index: number) => {
        Stack({ alignContent: Alignment.BottomStart }) {
          LazyImage({
            src: 'file://' + photo.path,
            widthValue: Constants.FULL_PERCENT,
            heightValue: Constants.FULL_PERCENT,
            objectFit: ImageFit.Cover,
            placeholder: Constants.PLACEHOLDER_IMAGE,
            errorHolder: Constants.ERROR_IMAGE,
            cornerRadius: $r('app.float.img_border_radius')
          })

          Column() {
            Text(photo.name || '未命名图片')
              .fontSize(20)
              .fontWeight(FontWeight.Medium)
              .fontColor(Color.White)
              .maxLines(1)
              .textOverflow({ overflow: TextOverflow.Ellipsis })

            if (photo.category) {
              Text(photo.category)
                .fontSize(12)
                .fontColor(Color.White)
                .padding({ left: 12, right: 12, top: 4, bottom: 4 })
                .backgroundColor('rgba(0,0,0,0.4)')
                .borderRadius(999)
                .margin({ top: 12 })
            }

            if (photo.tags) {
              Text(photo.tags)
                .fontSize(10)
                .fontColor('#EEEEEE')
                .margin({ top: 6 })
                .maxLines(1)
                .textOverflow({ overflow: TextOverflow.Ellipsis })
            }
          }
          .padding(20)
          .width(Constants.FULL_PERCENT)
          .linearGradient({
            direction: GradientDirection.Bottom,
            colors: [[0x00000000, 0.0], [0xAA000000, 1.0]]
          })
        }
        .width(Constants.FULL_PERCENT)
        .aspectRatio(Constants.BANNER_ASPECT_RATIO)
        .clip(true)
        .onClick(() => {
          // 由于 photos 是 dbPhotoList 的头部切片，index 与 dbPhotoList 中一致
          this.navigateToDetail(index);
        })
      }, (photo: PhotoModel) => photo.id.toString())
    }
    .autoPlay(photos.length > 1)
    .loop(photos.length > 1)
    .duration(Constants.BANNER_ANIMATE_DURATION)
    .margin($r('app.float.grid_padding'))
    .borderRadius($r('app.float.img_border_radius'))
    .clip(true)
  }

  // ==========================================
  // 首页内容构建器
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

        // 2. 顶部滚动照片
        this.FeaturedCarousel()

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
            ForEach(this.dbPhotoList, (photo: PhotoModel, index: number) => {
              GridItem() {
                this.PhotoItem(photo, index);
              }
              .width(Constants.FULL_PERCENT)
              .aspectRatio(1)
            }, (photo: PhotoModel) => photo.id.toString())
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

  @Builder
  PhotoItem(photo: PhotoModel, index: number) {
    Stack({ alignContent: Alignment.BottomStart }) {
      LazyImage({
        src: 'file://' + photo.path,
        widthValue: '100%',
        heightValue: '100%',
        objectFit: ImageFit.Cover,
        placeholder: Constants.PLACEHOLDER_IMAGE,
        errorHolder: Constants.ERROR_IMAGE,
        cornerRadius: 8
      })

      Column() {
        Text(photo.name)
          .fontSize(12)
          .fontColor(Color.White)
          .maxLines(1)
          .textOverflow({ overflow: TextOverflow.Ellipsis })
          .width('100%')
          .textAlign(TextAlign.Start)

        if (photo.category) {
          Text(photo.category)
            .fontSize(10)
            .fontColor(Color.White)
            .padding({ left: 6, right: 6, top: 2, bottom: 2 })
            .backgroundColor('rgba(0, 0, 0, 0.5)')
            .borderRadius(4)
            .margin({ top: 4 })
        }
      }
      .width('100%')
      .padding(8)
      .linearGradient({
        direction: GradientDirection.Bottom,
        colors: [[0x00000000, 0.0], [0xAA000000, 1.0]]
      })
      .alignItems(HorizontalAlign.Start)
    }
    .width('100%')
    .height('100%')
    .onClick(() => {
      this.navigateToDetail(index);
    })
  }

  // 页面显示时（从ListPage返回触发）
  onPageShow() {
    console.info('[IndexPage] 页面显示，刷新数据');
    this.currentTabIndex = 0;
    this.refreshData();
  }

  // 主构建函数 - 使用Tabs组件
  build() {
    Tabs({ index: this.currentTabIndex }) {
      // Tab 1: 首页
      TabContent() {
        this.HomeContent()
      }
      .tabBar(this.TabBuilder(0, '首页', '🏠'))

      // Tab 2: 搜索页
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
    .barPosition(BarPosition.End)
    .scrollable(false)
    .animationDuration(300)
    .onChange((index: number) => {
      console.info(`[IndexPage] 切换到 Tab ${index}`);
      this.currentTabIndex = index;

      if (index === 1) {
        router.pushUrl({
          url: 'pages/ListPage'
        }).then(() => {
          console.info('[IndexPage] 成功跳转到搜索页');
          this.currentTabIndex = 0;
        }).catch((err: Error) => {
          console.error(`[IndexPage] 跳转失败: ${err.message}`);
          this.currentTabIndex = 0;
        });
      }
    })
  }

  // Tab图标构建器
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

  private navigateToDetail(index: number) {
    this.selectedIndex = index;
    const photoPaths = this.dbPhotoList.map(photo => 'file://' + photo.path);

    console.info(`[IndexPage] 跳转到详情页，索引: ${index}`);

    router.pushUrl({
      url: Constants.URL_DETAIL_LIST_PAGE,
      params: {
        photoArr: photoPaths,
      }
    }).catch((err: Error) => {
      console.error(`${Constants.TAG_INDEX_PAGE}${err.message}`);
    });
  }
}

