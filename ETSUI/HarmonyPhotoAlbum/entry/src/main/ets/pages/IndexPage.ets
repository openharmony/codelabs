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
import { PhotoModel } from '../model/PhotoModel';
import PhotoService from '../service/PhotoService';
import { LazyImage } from '../components/LazyImage';
import Logger from '../common/utils/Logger';
import AuthService from '../service/AuthService';
import { ImagePicker } from '../components/ImagePicker';
import { GALLERY_ITEMS, GalleryItem } from '../model/ResourceGallery';

// 定义数据接口
interface PhotoData {
  filePath: string;
  fileName: string;
  category: string;
  tags: string;
  createTime: number;
}

// 顶部轮播预置图片数据结构（仅用于 UI 展示，不入库）
interface PreloadCarouselItem {
  src: Resource;
  name: string;
  category: string;
  tags: string;
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

  // 隐私保险箱鉴权相关状态
  @State hasPin: boolean = false;          // 是否已经设置过 PIN
  @State isAuthed: boolean = false;        // 当前会话是否已通过鉴权
  @State pinInput: string = '';            // 输入的 PIN
  @State pinConfirmInput: string = '';     // 确认 PIN（首次设置时使用）
  @State isSettingPin: boolean = false;    // 当前是否处于首次设置 PIN 流程
  @State pinErrorMsg: string = '';         // PIN 相关错误提示

  aboutToAppear() {
    console.info('[IndexPage] 页面加载,正在初始化数据库...');
    const context = getContext(this) as common.UIAbilityContext;

    // 1. 初始化隐私保险箱 PIN 状态
    AuthService.hasPin(context)
      .then((has: boolean) => {
        this.hasPin = has;
        // 如果还没有设置 PIN，则进入设置流程
        this.isSettingPin = !has;
      })
      .catch((err: Error) => {
        console.error('[IndexPage] 读取 PIN 状态失败: ' + JSON.stringify(err));
      });

    // 2. 预初始化数据库（保持原有数据结构不变）
    PhotoService.initDB(context)
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

  /**
   * 首次设置 PIN 码（隐私保险箱初始化）
   */
  private handleSetPin() {
    this.pinErrorMsg = '';

    if (this.pinInput.length !== 4 || !/^\d{4}$/.test(this.pinInput)) {
      this.pinErrorMsg = 'PIN 码必须是 4 位数字';
      return;
    }

    if (this.pinInput !== this.pinConfirmInput) {
      this.pinErrorMsg = '两次输入的 PIN 码不一致';
      return;
    }

    const context = getContext(this) as common.UIAbilityContext;
    AuthService.setPin(context, this.pinInput)
      .then(() => {
        this.hasPin = true;
        this.isAuthed = true;
        this.isSettingPin = false;
        this.pinErrorMsg = '';
        this.pinConfirmInput = '';
        // 鉴权成功后刷新一次数据
        this.refreshData();
      })
      .catch((err: Error) => {
        console.error('[IndexPage] 设置 PIN 失败: ' + JSON.stringify(err));
        this.pinErrorMsg = '保存 PIN 失败，请稍后重试';
      });
  }

  /**
   * 校验已有 PIN，解锁隐私相册
   */
  private handleVerifyPin() {
    this.pinErrorMsg = '';

    if (this.pinInput.length !== 4 || !/^\d{4}$/.test(this.pinInput)) {
      this.pinErrorMsg = 'PIN 码必须是 4 位数字';
      return;
    }

    const context = getContext(this) as common.UIAbilityContext;
    AuthService.verifyPin(context, this.pinInput)
      .then((ok: boolean) => {
        if (ok) {
          this.isAuthed = true;
          this.pinErrorMsg = '';
          this.refreshData();
        } else {
          this.pinErrorMsg = 'PIN 码错误，请重试';
        }
      })
      .catch((err: Error) => {
        console.error('[IndexPage] 校验 PIN 失败: ' + JSON.stringify(err));
        this.pinErrorMsg = '校验失败，请稍后重试';
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

  /**
   * 获取用于网格展示的照片列表（排除轮播中的图片）
   */
  private getGridPhotos(): PhotoModel[] {
    if (!this.dbPhotoList || this.dbPhotoList.length === 0) {
      return [];
    }
    // 从轮播后的照片开始显示
    const carouselCount: number = Math.min(Constants.SHOW_COUNT, this.dbPhotoList.length);
    return this.dbPhotoList.slice(carouselCount);
  }

  /**
   * 顶部轮播预置图片（来自 resources/base/media），用于在没有用户照片时先展示几张示例图
   * 不写入数据库，保持现有图片数据模式不变
   * 这里统一使用 ResourceGallery 中的全量资源，保证与查询页、电子相框使用同一套图库
   */
  private getPreloadCarouselImages(): Array<PreloadCarouselItem> {
    // 取前4张作为轮播图
    const carouselCount = Math.min(4, GALLERY_ITEMS.length);
    return GALLERY_ITEMS.slice(0, carouselCount).map((item: GalleryItem) => {
      const preload: PreloadCarouselItem = {
        src: item.src,
        name: item.name,
        category: item.category,
        tags: item.tags
      };
      return preload;
    });
  }

  /**
   * 获取用于网格展示的预置图片（排除轮播中的图片）
   */
  private getPreloadGridImages(): Array<PreloadCarouselItem> {
    // 从第5张开始作为网格图片
    const carouselCount = Math.min(4, GALLERY_ITEMS.length);
    return GALLERY_ITEMS.slice(carouselCount).map((item: GalleryItem) => {
      const preload: PreloadCarouselItem = {
        src: item.src,
        name: item.name,
        category: item.category,
        tags: item.tags
      };
      return preload;
    });
  }
  @Builder
  FeaturedCarousel() {
    // 1. 用户照片与预置图片都没有时，显示占位
    if (this.getCarouselPhotos().length === 0 && this.getPreloadCarouselImages().length === 0) {
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
      // 2. 仅有预置图片（数据库还没有照片）时，先用预置图做轮播
    } else if (this.getCarouselPhotos().length === 0 && this.getPreloadCarouselImages().length > 0) {
      Swiper(this.swiperController) {
        ForEach(this.getPreloadCarouselImages(), (photo: PreloadCarouselItem, index: number) => {
          Stack({ alignContent: Alignment.BottomStart }) {
            Image(photo.src)
              .width(Constants.FULL_PERCENT)
              .aspectRatio(Constants.BANNER_ASPECT_RATIO)
              .objectFit(ImageFit.Cover)
              .borderRadius($r('app.float.img_border_radius'))
              .backgroundColor('#F0F0F0')

            Column() {
              Text(photo.name || '示例图片')
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
            // 预置图片点击后也进入大图浏览页
            this.navigateToPreloadDetail(index);
          })
        })
      }
      .autoPlay(this.getPreloadCarouselImages().length > 1)
      .loop(this.getPreloadCarouselImages().length > 1)
      .duration(Constants.BANNER_ANIMATE_DURATION)
      .margin($r('app.float.grid_padding'))
      .borderRadius($r('app.float.img_border_radius'))
      .clip(true)
      // 3. 正常情况：使用数据库中的照片做轮播，行为保持不变
    } else {
      Swiper(this.swiperController) {
        ForEach(this.getCarouselPhotos(), (photo: PhotoModel, index: number) => {
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
      .autoPlay(this.getCarouselPhotos().length > 1)
      .loop(this.getCarouselPhotos().length > 1)
      .duration(Constants.BANNER_ANIMATE_DURATION)
      .margin($r('app.float.grid_padding'))
      .borderRadius($r('app.float.img_border_radius'))
      .clip(true)
    }
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

          Blank().layoutWeight(1)

          // 进入电子相框分类选择页（先选分类，再跳转对应幻灯片播放页）
          Button('电子相框')
            .height(32)
            .padding({ left: 12, right: 12 })
            .fontSize(12)
            .backgroundColor('#007DFF')
            .fontColor(Color.White)
            .onClick(() => {
              router.pushUrl({
                url: Constants.URL_FRAME_CATEGORY_PAGE
              }).catch((err: Error) => {
                console.error('[IndexPage] 跳转电子相框分类页失败: ' + JSON.stringify(err));
              });
            })
        }
        .height($r('app.float.navi_bar_height'))
        .width(Constants.FULL_PERCENT)
        .padding({ left: $r('app.float.title_padding'), right: 16 })

        // 2. 顶部滚动照片
        this.FeaturedCarousel()

        // 3. 图片网格
        if (this.dbPhotoList.length === 0) {
          // 数据库为空时，显示预置图片网格（排除轮播中的图片）
          if (this.getPreloadGridImages().length > 0) {
            Grid() {
              ForEach(this.getPreloadGridImages(), (photo: PreloadCarouselItem, index: number) => {
                GridItem() {
                  this.PreloadPhotoItem(photo, index);
                }
                .width(Constants.FULL_PERCENT)
                .aspectRatio(1)
              }, (photo: PreloadCarouselItem, index?: number) => `preload_${index}`)
            }
            .columnsTemplate(Constants.INDEX_COLUMNS_TEMPLATE)
            .columnsGap($r('app.float.grid_padding'))
            .rowsGap($r('app.float.grid_padding'))
            .padding($r('app.float.grid_padding'))
            .width(Constants.FULL_PERCENT)
            .layoutWeight(1)
          } else {
            // 完全没有图片时的占位
            Column() {
              Text('📷')
                .fontSize(60)
                .opacity(0.3)
                .margin({ top: 50 })

              Text('相册空空如也')
                .fontSize(16)
                .fontColor(Color.Gray)
                .margin({ top: 20 })

              Text('这是一个基于沙箱的隐私相册')
                .fontSize(14)
                .fontColor('#CCCCCC')
                .margin({ top: 10 })
            }
            .layoutWeight(1)
            .width('100%')
            .justifyContent(FlexAlign.Center)
          }
        } else {
          // 数据库有照片时，显示除轮播外的照片
          Grid() {
            ForEach(this.getGridPhotos(), (photo: PhotoModel, index: number) => {
              GridItem() {
                // 注意：这里的index是网格中的索引，需要加上轮播数量才是dbPhotoList中的真实索引
                this.PhotoItem(photo, index + Math.min(Constants.SHOW_COUNT, this.dbPhotoList.length));
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

  @Builder
  PreloadPhotoItem(photo: PreloadCarouselItem, index: number) {
    Stack({ alignContent: Alignment.BottomStart }) {
      Image(photo.src)
        .width('100%')
        .height('100%')
        .objectFit(ImageFit.Cover)
        .borderRadius(8)

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
      // 点击预置网格图片，跳转到详情页（index需要加上轮播图数量）
      this.navigateToPreloadGridDetail(index);
    })
  }

  // 页面显示时（从ListPage返回触发）
  onPageShow() {
    console.info('[IndexPage] 页面显示，刷新数据');
    this.currentTabIndex = 0;
    if (this.isAuthed) {
      this.refreshData();
    }
  }

  // 主构建函数：先进行 PIN 鉴权，成功后再进入主界面
  build() {
    if (!this.isAuthed) {
      this.AuthContent();
    } else {
      this.MainTabs();
    }
  }

  // 鉴权页面（隐私安全保险箱）
  @Builder
  AuthContent() {
    Column({ space: 24 }) {
      Blank().layoutWeight(1)

      Column({ space: 12 }) {
        Text('隐私安全保险箱')
          .fontSize(26)
          .fontWeight(FontWeight.Bold)
          .fontColor('#222222')

        if (!this.hasPin || this.isSettingPin) {
          Text('首次使用，请设置 4 位数字 PIN 码')
            .fontSize(14)
            .fontColor('#666666')
        } else {
          Text('请输入 4 位数字 PIN 码解锁相册')
            .fontSize(14)
            .fontColor('#666666')
        }
      }

      // 输入区域
      if (!this.hasPin || this.isSettingPin) {
        // 设置 PIN
        Column({ space: 12 }) {
          TextInput({
            placeholder: '设置 4 位 PIN',
            text: this.pinInput
          })
            .type(InputType.Number)
            .maxLength(4)
            .onChange((value: string) => {
              this.pinInput = value;
            })

          TextInput({
            placeholder: '再次输入以确认',
            text: this.pinConfirmInput
          })
            .type(InputType.Number)
            .maxLength(4)
            .onChange((value: string) => {
              this.pinConfirmInput = value;
            })

          Button('保存并进入相册')
            .width('100%')
            .height(44)
            .backgroundColor('#007DFF')
            .fontColor(Color.White)
            .onClick(() => this.handleSetPin())
        }
      } else {
        // 校验 PIN
        Column({ space: 12 }) {
          TextInput({
            placeholder: '输入 4 位 PIN',
            text: this.pinInput
          })
            .type(InputType.Number)
            .maxLength(4)
            .onChange((value: string) => {
              this.pinInput = value;
            })

          Button('解锁相册')
            .width('100%')
            .height(44)
            .backgroundColor('#007DFF')
            .fontColor(Color.White)
            .onClick(() => this.handleVerifyPin())
        }
      }

      if (this.pinErrorMsg) {
        Text(this.pinErrorMsg)
          .fontSize(12)
          .fontColor(Color.Red)
      }

      Blank().layoutWeight(1)

      Text('所有照片均保存在应用沙箱中，系统图库无法访问。')
        .fontSize(12)
        .fontColor('#999999')
        .textAlign(TextAlign.Center)
        .width('100%')
    }
    .padding(24)
    .width('100%')
    .height('100%')
    .backgroundColor('#F5F5F5')
  }

  // 主界面：Tabs + 相册首页/搜索页
  @Builder
  MainTabs() {
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

  /**
   * 跳转到预置轮播图片的详情列表页
   * 使用与用户照片相同的 DetailListPage，大图浏览体验一致
   */
  private navigateToPreloadDetail(index: number) {
    this.selectedIndex = index;
    // 只包含轮播图片
    const photoArr = this.getPreloadCarouselImages().map((item: PreloadCarouselItem) => item.src);

    router.pushUrl({
      url: Constants.URL_DETAIL_LIST_PAGE,
      params: {
        photoArr: photoArr
      }
    }).catch((err: Error) => {
      console.error(`${Constants.TAG_INDEX_PAGE}${err.message}`);
    });
  }

  /**
   * 跳转到预置网格图片的详情列表页
   * 包含所有预置图片（轮播+网格），并设置正确的起始索引
   */
  private navigateToPreloadGridDetail(index: number) {
    // 计算真实索引：网格图片的索引需要加上轮播图片数量
    const carouselCount = Math.min(4, GALLERY_ITEMS.length);
    this.selectedIndex = carouselCount + index;
    
    // 包含所有预置图片（轮播+网格）
    const allPreloadImages = GALLERY_ITEMS.map((item: GalleryItem) => item.src);

    router.pushUrl({
      url: Constants.URL_DETAIL_LIST_PAGE,
      params: {
        photoArr: allPreloadImages
      }
    }).catch((err: Error) => {
      console.error(`${Constants.TAG_INDEX_PAGE}${err.message}`);
    });
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
