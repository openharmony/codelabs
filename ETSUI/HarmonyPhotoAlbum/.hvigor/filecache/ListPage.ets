/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/*
 * 修复后的 ListPage.ets
 * 强制使用数据库数据，显示搜索栏和分类筛选
 * ✅ 支持返回首页功能
 */

import router from '@ohos.router';
import Constants from '../common/constants/Constants';
import { SearchBar } from '../components/SearchBar';
import { CategoryFilter } from '../components/CategoryFilter';
import PhotoService from '../service/PhotoService';
import { PhotoModel } from '../model/PhotoModel';
import { LazyImage } from '../components/LazyImage';
import Logger from '../common/utils/Logger';

@Entry
@Component
struct ListPage {
  // 状态变量
  @State searchText: string = '';
  @State selectedCategory: string = Constants.ALL_CATEGORY;
  @State displayPhotos: PhotoModel[] = [];
  @State isLoading: boolean = true;
  @StorageLink('selectedIndex') selectedIndex: number = 0;

  // ==========================================
  // 🔧 修复：直接使用数据库数据
  // ==========================================
  aboutToAppear() {
    console.info('[ListPage] ========== 页面加载开始 ==========');
    this.loadPhotos();
  }

  // ==========================================
  // 🆕 页面显示时刷新数据（从详情页返回时）
  // ==========================================
  onPageShow() {
    console.info('[ListPage] 页面显示，刷新数据');
    this.loadPhotos();
  }

  build() {
    Navigation() {
      Column() {
        // ==========================================
        // ✅ 搜索栏（始终显示）
        // ==========================================
        SearchBar({
          searchText: $searchText,
          onSearch: (text: string) => {
            console.info(`[ListPage] 搜索关键词: ${text}`);
            this.handleSearch(text);
          }
        })
          .margin({ left: 16, right: 16 })

        // ==========================================
        // ✅ 分类筛选（始终显示）
        // ==========================================
        CategoryFilter({
          selectedCategory: $selectedCategory,
          onCategoryChange: (category: string) => {
            console.info(`[ListPage] 切换分类: ${category}`);
            this.handleCategoryChange(category);
          }
        })

        // ==========================================
        // 图片网格
        // ==========================================
        if (this.isLoading) {
          this.LoadingView();
        } else if (this.displayPhotos.length > 0) {
          this.PhotoGrid();
        } else {
          this.EmptyView();
        }
      }
      .width('100%')
      .height('100%')
      .backgroundColor('#F8F8F8')
    }
    .title(Constants.PAGE_TITLE)
    .hideBackButton(false) // ✅ 显示返回按钮（系统默认返回到上一页）
    .titleMode(NavigationTitleMode.Mini)
    // ==========================================
    // 🆕 添加工具栏按钮（提供额外的返回方式）
    // ==========================================
    .toolbarConfiguration([
      {
        value: '返回首页',
        action: () => {
          console.info('[ListPage] 工具栏-返回首页');
          router.back();
        }
      }
    ])
  }

  // ==========================================
  // 图片网格
  // ==========================================
  @Builder
  PhotoGrid() {
    Grid() {
      ForEach(this.displayPhotos, (photo: PhotoModel, index: number) => {
        GridItem() {
          this.PhotoItem(photo, index);
        }
        .width(Constants.FULL_PERCENT)
        .aspectRatio(1)
      }, (photo: PhotoModel) => photo.id.toString())
    }
    .columnsTemplate(Constants.GRID_COLUMNS_TEMPLATE)
    .rowsGap(Constants.LIST_ITEM_SPACE)
    .columnsGap(Constants.LIST_ITEM_SPACE)
    .layoutWeight(1)
    .cachedCount(10)
  }

  @Builder
  PhotoItem(photo: PhotoModel, index: number) {
    Stack({ alignContent: Alignment.BottomStart }) {
      // 图片 - 🔧 修复：添加 file:// 前缀
      LazyImage({
        src: 'file://' + photo.path,
        width: '100%',
        height: '100%',
        objectFit: ImageFit.Cover,
        placeholder: Constants.PLACEHOLDER_IMAGE,
        errorHolder: Constants.ERROR_IMAGE,
        cornerRadius: 8
      })

      // 图片信息叠加层
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

  // ==========================================
  // 加载中视图
  // ==========================================
  @Builder
  LoadingView() {
    Column() {
      LoadingProgress()
        .width(40)
        .height(40)
        .color('#007DFF')

      Text('加载中...')
        .fontSize(14)
        .fontColor('#999999')
        .margin({ top: 16 })
    }
    .layoutWeight(1)
    .justifyContent(FlexAlign.Center)
  }

  // ==========================================
  // 空状态视图
  // ==========================================
  @Builder
  EmptyView() {
    Column() {
      Text('📷')
        .fontSize(60)
        .opacity(0.4)

      Text('暂无图片')
        .fontSize(14)
        .fontColor('#999999')
        .margin({ top: 16 })

      Text(this.searchText ? '试试其他搜索词' : '快去添加图片吧')
        .fontSize(12)
        .fontColor('#CCCCCC')
        .margin({ top: 8 })

      // ==========================================
      // 🆕 添加返回首页按钮
      // ==========================================
      if (!this.searchText && this.displayPhotos.length === 0) {
        Button('返回首页上传')
          .fontSize(14)
          .margin({ top: 20 })
          .onClick(() => {
            router.back();
          })
      }
    }
    .layoutWeight(1)
    .justifyContent(FlexAlign.Center)
  }

  // ==========================================
  // 数据加载与搜索逻辑
  // ==========================================
  private loadPhotos() {
    console.info('[ListPage] 开始加载图片...');
    this.isLoading = true;

    PhotoService.queryAll()
      .then((photos: PhotoModel[]) => {
        this.displayPhotos = photos;
        this.isLoading = false;
        console.info(`[ListPage] ✅ 加载成功，共 ${photos.length} 张图片`);

        // 👇 如果数据库为空，提示用户
        if (photos.length === 0) {
          console.warn('[ListPage] ⚠️ 数据库中没有图片，请先上传图片');
        }
      })
      .catch((err: Error) => {
        console.error(`[ListPage] ❌ 加载失败: ${err.message}`);
        Logger.showError('图片加载失败，请稍后重试');
        this.displayPhotos = [];
        this.isLoading = false;
      });
  }

  private handleSearch(keyword: string) {
    console.info(`[ListPage] 执行搜索，关键词: "${keyword}", 分类: "${this.selectedCategory}"`);
    this.isLoading = true;

    PhotoService.searchPhotosByCategoryAndKeyword(this.selectedCategory, keyword)
      .then((photos: PhotoModel[]) => {
        this.displayPhotos = photos;
        this.isLoading = false;
        console.info(`[ListPage] ✅ 搜索成功，找到 ${photos.length} 张图片`);
      })
      .catch((err: Error) => {
        console.error(`[ListPage] ❌ 搜索失败: ${err.message}`);
        Logger.showError('搜索失败，请稍后重试');
        this.displayPhotos = [];
        this.isLoading = false;
      });
  }

  private handleCategoryChange(category: string) {
    console.info(`[ListPage] 切换分类到: "${category}"`);
    this.isLoading = true;

    PhotoService.searchPhotosByCategoryAndKeyword(category, this.searchText)
      .then((photos: PhotoModel[]) => {
        this.displayPhotos = photos;
        this.isLoading = false;
        console.info(`[ListPage] ✅ 分类切换成功，共 ${photos.length} 张图片`);
      })
      .catch((err: Error) => {
        console.error(`[ListPage] ❌ 分类切换失败: ${err.message}`);
        Logger.showError('切换分类失败，请稍后重试');
        this.displayPhotos = [];
        this.isLoading = false;
      });
  }

  private navigateToDetail(index: number) {
    this.selectedIndex = index;
    const photoPaths = this.displayPhotos.map(photo => 'file://' + photo.path); // 🔧 添加 file:// 前缀

    console.info(`[ListPage] 跳转到详情页，索引: ${index}`);

    router.pushUrl({
      url: Constants.URL_DETAIL_LIST_PAGE,
      params: {
        photoArr: photoPaths,
      }
    }).catch((err: Error) => {
      console.error(`${Constants.TAG_LIST_PAGE}${err.message}`);
    });
  }

}
