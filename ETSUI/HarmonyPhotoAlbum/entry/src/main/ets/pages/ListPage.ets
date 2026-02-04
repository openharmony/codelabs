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
import { GALLERY_ITEMS, GalleryItem } from '../model/ResourceGallery';

// 搜索列表用到的统一展示模型（包含数据库图片 + 预置资源图）
type SearchPhotoSrc = string | Resource;

interface SearchPhotoItem {
  id: number;           // 对于预置图使用负数 ID，避免与数据库冲突
  name: string;
  category: string;
  tags: string;
  src: SearchPhotoSrc;  // 数据库图片: 'file://' + 路径；预置图片: 资源 ID
  isPreload: boolean;   // 是否为预置资源图片
}

@Entry
@Component
struct ListPage {
  // 状态变量
  @State searchText: string = '';
  @State selectedCategory: string = Constants.ALL_CATEGORY;
  @State displayPhotos: SearchPhotoItem[] = [];
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
      ForEach(this.displayPhotos, (photo: SearchPhotoItem, index: number) => {
        GridItem() {
          this.PhotoItem(photo, index);
        }
        .width(Constants.FULL_PERCENT)
        .aspectRatio(1)
      }, (photo: SearchPhotoItem) => photo.id.toString())
    }
    .columnsTemplate(Constants.GRID_COLUMNS_TEMPLATE)
    .rowsGap(Constants.LIST_ITEM_SPACE)
    .columnsGap(Constants.LIST_ITEM_SPACE)
    .layoutWeight(1)
    .cachedCount(10)
  }

  @Builder
  PhotoItem(photo: SearchPhotoItem, index: number) {
    Stack({ alignContent: Alignment.BottomStart }) {
      LazyImage({
        // 统一从 src 字段读取，数据库图片为 'file://' 路径，预置图片为资源 ID
        src: photo.src,
        widthValue: '100%',
        heightValue: '100%',
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

    // 查询数据库，保留原有逻辑
    PhotoService.queryAll()
      .then((photos: PhotoModel[]) => {
        const dbItems: Array<SearchPhotoItem> = photos.map((photo: PhotoModel) => this.mapDbPhotoToSearchItem(photo));

        // 使用统一资源图库作为预置数据，保证与首页/电子相框一致
        const preloadItems: Array<SearchPhotoItem> = this.getPreloadSearchItemsFromGallery();

        this.displayPhotos = dbItems.concat(preloadItems);
        this.isLoading = false;
        console.info(`[ListPage] ✅ 加载成功，DB: ${photos.length} 条，预置: ${preloadItems.length} 条`);

        if (photos.length === 0) {
          console.warn('[ListPage] ⚠️ 数据库中没有图片，仅展示预置图库');
        }
      })
      .catch((err: Error) => {
        console.error(`[ListPage] ❌ 加载失败: ${err.message}`);
        Logger.showError('图片加载失败，请稍后重试');

        // 即使 DB 失败，也使用预置资源图库保证搜索页可用
        this.displayPhotos = this.getPreloadSearchItemsFromGallery();
        this.isLoading = false;
      });
  }

  private handleSearch(keyword: string) {
    console.info(`[ListPage] 执行搜索，关键词: "${keyword}", 分类: "${this.selectedCategory}"`);
    this.isLoading = true;

    PhotoService.searchPhotosByCategoryAndKeyword(this.selectedCategory, keyword)
      .then((photos: PhotoModel[]) => {
        const dbItems: Array<SearchPhotoItem> = photos.map((photo: PhotoModel) => this.mapDbPhotoToSearchItem(photo));

        // 将符合当前搜索条件的预置图库一并加入
        const preloadItems: Array<SearchPhotoItem> = this.filterPreloadBy(this.selectedCategory, keyword);

        this.displayPhotos = dbItems.concat(preloadItems);
        this.isLoading = false;
        console.info(`[ListPage] ✅ 搜索成功，DB: ${photos.length} 条，预置: ${preloadItems.length} 条`);
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
        const dbItems: Array<SearchPhotoItem> = photos.map((photo: PhotoModel) => this.mapDbPhotoToSearchItem(photo));

        const preloadItems: Array<SearchPhotoItem> = this.filterPreloadBy(category, this.searchText);

        this.displayPhotos = dbItems.concat(preloadItems);
        this.isLoading = false;
        console.info(`[ListPage] ✅ 分类切换成功，DB: ${photos.length} 条，预置: ${preloadItems.length} 条`);
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
    // 统一从 displayPhotos.src 构建详情页图片数组，
    // 数据库图片为 'file://' 路径字符串，预置图片为资源 ID
    const photoArr = this.displayPhotos.map((photo: SearchPhotoItem) => photo.src);

    console.info(`[ListPage] 跳转到详情页，索引: ${index}`);

    router.pushUrl({
      url: Constants.URL_DETAIL_LIST_PAGE,
      params: {
        photoArr: photoArr,
      }
    }).catch((err: Error) => {
      console.error(`${Constants.TAG_LIST_PAGE}${err.message}`);
    });
  }

  /**
   * 将数据库中的 PhotoModel 转换为用于搜索展示的 SearchPhotoItem
   */
  private mapDbPhotoToSearchItem(photo: PhotoModel): SearchPhotoItem {
    const src: string = 'file://' + photo.path;
    const item: SearchPhotoItem = {
      id: photo.id,
      name: photo.name,
      category: photo.category,
      tags: photo.tags,
      src: src,
      isPreload: false
    };
    return item;
  }

  // ==========================================
  // 预置资源图：参与搜索页展示
  // 与首页顶部轮播保持一致的名称 / 分类 / 标签
  // ==========================================
  // 使用统一资源图库构造预置搜索项
  private getPreloadSearchItemsFromGallery(): Array<SearchPhotoItem> {
    return GALLERY_ITEMS.map((item: GalleryItem, index: number) => {
      const searchItem: SearchPhotoItem = {
        id: -1000 - index, // 负数 ID，避免与数据库冲突
        name: item.name,
        category: item.category,
        tags: item.tags,
        src: item.src,
        isPreload: true
      };
      return searchItem;
    });
  }

  /**
   * 根据当前分类和关键词，从预置图片中筛选符合条件的项
   */
  private filterPreloadBy(category: string, keyword: string): Array<SearchPhotoItem> {
    let result: Array<SearchPhotoItem> = this.getPreloadSearchItemsFromGallery();

    // 分类过滤（"全部" 表示不过滤）
    if (category && category !== Constants.ALL_CATEGORY) {
      result = result.filter((item: SearchPhotoItem) => item.category === category);
    }

    // 关键词过滤（名称 / 分类 / 标签 模糊匹配）
    if (keyword && keyword.trim() !== '') {
      const lowerKeyword: string = keyword.toLowerCase();
      result = result.filter((item: SearchPhotoItem) => {
        const nameMatch: boolean = item.name.toLowerCase().includes(lowerKeyword);
        const categoryMatch: boolean = item.category.toLowerCase().includes(lowerKeyword);
        const tagsMatch: boolean = item.tags.toLowerCase().includes(lowerKeyword);
        return nameMatch || categoryMatch || tagsMatch;
      });
    }

    return result;
  }

}
