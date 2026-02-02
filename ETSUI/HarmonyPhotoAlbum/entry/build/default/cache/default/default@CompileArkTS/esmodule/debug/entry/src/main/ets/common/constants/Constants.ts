/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License,Version 2.0 (the "License");
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
export default class Constants {
    /**
     * banner list
     */
    static readonly BANNER_IMG_LIST: Array<Resource> = [
        { "id": 16777244, "type": 20000, params: [], "bundleName": "com.example.electronicalbum", "moduleName": "entry" },
        { "id": 16777232, "type": 20000, params: [], "bundleName": "com.example.electronicalbum", "moduleName": "entry" },
        { "id": 16777234, "type": 20000, params: [], "bundleName": "com.example.electronicalbum", "moduleName": "entry" },
        { "id": 16777240, "type": 20000, params: [], "bundleName": "com.example.electronicalbum", "moduleName": "entry" }
    ];
    /**
     * scene list
     */
    static readonly SCENE_LIST: Array<Resource> = [
        { "id": 16777244, "type": 20000, params: [], "bundleName": "com.example.electronicalbum", "moduleName": "entry" },
        { "id": 16777245, "type": 20000, params: [], "bundleName": "com.example.electronicalbum", "moduleName": "entry" },
        { "id": 16777243, "type": 20000, params: [], "bundleName": "com.example.electronicalbum", "moduleName": "entry" }
    ];
    /**
     * men list
     */
    static readonly MEN_LIST: Array<Resource> = [
        { "id": 16777240, "type": 20000, params: [], "bundleName": "com.example.electronicalbum", "moduleName": "entry" },
        { "id": 16777241, "type": 20000, params: [], "bundleName": "com.example.electronicalbum", "moduleName": "entry" },
        { "id": 16777242, "type": 20000, params: [], "bundleName": "com.example.electronicalbum", "moduleName": "entry" }
    ];
    /**
     * food list
     */
    static readonly FOOD_LIST: Array<Resource> = [
        { "id": 16777233, "type": 20000, params: [], "bundleName": "com.example.electronicalbum", "moduleName": "entry" },
        { "id": 16777232, "type": 20000, params: [], "bundleName": "com.example.electronicalbum", "moduleName": "entry" },
    ];
    /**
     * life list
     */
    static readonly LIFE_LIST: Array<Resource> = [
        { "id": 16777235, "type": 20000, params: [], "bundleName": "com.example.electronicalbum", "moduleName": "entry" },
        { "id": 16777234, "type": 20000, params: [], "bundleName": "com.example.electronicalbum", "moduleName": "entry" },
        { "id": 16777236, "type": 20000, params: [], "bundleName": "com.example.electronicalbum", "moduleName": "entry" },
        { "id": 16777237, "type": 20000, params: [], "bundleName": "com.example.electronicalbum", "moduleName": "entry" },
        { "id": 16777238, "type": 20000, params: [], "bundleName": "com.example.electronicalbum", "moduleName": "entry" },
        { "id": 16777239, "type": 20000, params: [], "bundleName": "com.example.electronicalbum", "moduleName": "entry" }
    ];
    /**
     * index page img arr
     */
    static readonly IMG_ARR: Resource[][] = [
        new Array<Resource>().concat(Constants.SCENE_LIST, Constants.LIFE_LIST, Constants.MEN_LIST),
        new Array<Resource>().concat(Constants.MEN_LIST, Constants.LIFE_LIST, Constants.SCENE_LIST),
        new Array<Resource>().concat(Constants.FOOD_LIST, Constants.SCENE_LIST, Constants.SCENE_LIST),
        new Array<Resource>().concat(Constants.LIFE_LIST, Constants.FOOD_LIST, Constants.MEN_LIST)
    ];
    /**
     * title font weight
     */
    static readonly TITLE_FONT_WEIGHT: number = 500;
    /**
     * aspect ratio
     */
    static readonly BANNER_ASPECT_RATIO: number = 1.5;
    /**
     * animate duration
     */
    static readonly BANNER_ANIMATE_DURATION: number = 300;
    /**
     * share delay
     */
    static readonly SHARE_TRANSITION_DELAY: number = 100;
    /**
     * aspect ratio
     */
    static readonly STACK_IMG_RATIO: number = 0.7;
    /**
     * item space
     */
    static readonly LIST_ITEM_SPACE: number = 2;
    /**
     * cache size
     */
    static readonly CACHE_IMG_SIZE: number = 4;
    /**
     * cache list
     */
    static readonly CACHE_IMG_LIST: string[] = ['', '', '', ''];
    /**
     * title
     */
    static readonly PAGE_TITLE: string = '电子相册';
    /**
     *  router param
     */
    static readonly PARAM_PHOTO_ARR_KEY: string = 'photoArr';
    /**
     *  selected index
     */
    static readonly SELECTED_INDEX_KEY: string = 'selectedIndex';
    /**
     * grid column template
     */
    static readonly GRID_COLUMNS_TEMPLATE: string = '1fr 1fr 1fr 1fr';
    /**
     * index page columns template
     */
    static readonly INDEX_COLUMNS_TEMPLATE: string = '1fr 1fr';
    /**
     *  percent
     */
    static readonly FULL_PERCENT: string = '100%';
    /**
     * photo item percent
     */
    static readonly PHOTO_ITEM_PERCENT: string = '90%';
    /**
     * show count
     */
    static readonly SHOW_COUNT: number = 8;
    /**
     * default width
     */
    static readonly DEFAULT_WIDTH: number = 360;
    /**
     * padding
     */
    static readonly PHOTO_ITEM_PADDING: number = 8;
    /**
     * offset
     */
    static readonly PHOTO_ITEM_OFFSET: number = 16;
    /**
     * item opacity offset
     */
    static readonly ITEM_OPACITY_OFFSET: number = 0.2;
    /**
     * double number
     */
    static readonly DOUBLE_NUMBER: number = 2;
    /**
     * list page url
     */
    static readonly URL_LIST_PAGE: string = 'pages/ListPage';
    /**
     * detail list page url
     */
    static readonly URL_DETAIL_LIST_PAGE: string = 'pages/DetailListPage';
    /**
     * detail page url
     */
    static readonly URL_DETAIL_PAGE: string = 'pages/DetailPage';
    /**
     * index page tag
     */
    static readonly TAG_INDEX_PAGE: string = 'IndexPage push error ';
    /**
     * list page tag
     */
    static readonly TAG_LIST_PAGE: string = 'ListPage push error ';
    /**
     * detail list page tag
     */
    static readonly TAG_DETAIL_PAGE: string = 'DetailListPage push error ';
    // ==========================================
    // 图片缓存 / 占位图 / 动画相关新增常量
    // ==========================================
    /** 最大内存缓存图片数量（ImageCache 使用） */
    static readonly IMAGE_CACHE_MAX_SIZE: number = 100;
    /**
     * 占位图与错误图（这里先用应用 icon 做兜底，可根据需要替换为独立资源）
     */
    static readonly PLACEHOLDER_IMAGE: Resource = { "id": 16777246, "type": 20000, params: [], "bundleName": "com.example.electronicalbum", "moduleName": "entry" };
    static readonly ERROR_IMAGE: Resource = { "id": 16777246, "type": 20000, params: [], "bundleName": "com.example.electronicalbum", "moduleName": "entry" };
    /**
     * 缩放相关默认常量
     */
    static readonly MIN_ZOOM_SCALE: number = 1;
    static readonly MAX_ZOOM_SCALE: number = 3;
    /**
     * 判定左右翻页的拖拽比例阈值（相对于当前图片宽度）
     */
    static readonly SWIPE_THRESHOLD_RATIO: number = 0.25;
    /**
     * 通用动画时长（毫秒）
     */
    static readonly TRANSITION_DURATION_SHORT: number = 150;
    static readonly TRANSITION_DURATION_MEDIUM: number = 300;
    static readonly TRANSITION_DURATION_LONG: number = 500;
    // ==========================================
    // 新增：数据库相关配置
    // ==========================================
    /** 数据库文件名 */
    static readonly RDB_NAME = 'PhotoAlbum.db';
    /** 数据库表名 */
    static readonly PHOTO_TABLE_NAME = 'photo_table';
    /** 数据库版本 */
    static readonly RDB_VERSION = 1;
    /**
     * 建表 SQL 语句
     * id: 主键，自增
     * name: 图片名称
     * path: 图片路径
     * category: 分类
     * createTime: 创建时间
     * tags: 标签
     */
    static readonly CREATE_TABLE_SQL = `
    CREATE TABLE IF NOT EXISTS photo_table (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      name TEXT NOT NULL,
      path TEXT NOT NULL,
      category TEXT,
      createTime INTEGER,
      tags TEXT
    )
  `;
    /** 数据库列名定义 (方便后续查询使用) */
    static readonly COL_ID = 'id';
    static readonly COL_NAME = 'name';
    static readonly COL_PATH = 'path';
    static readonly COL_CATEGORY = 'category';
    static readonly COL_CREATE_TIME = 'createTime';
    static readonly COL_TAGS = 'tags';
    // ==========================================
    // 新增：分类相关常量
    // ==========================================
    /**
     * 预定义分类列表
     */
    static readonly CATEGORIES: string[] = ['风景', '人物', '动物', '建筑', '美食', '生活', '其他'];
    /**
     * 默认分类
     */
    static readonly DEFAULT_CATEGORY: string = '其他';
    /**
     * 全部分类标识
     */
    static readonly ALL_CATEGORY: string = '全部';
}
