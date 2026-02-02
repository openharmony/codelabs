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

import relationalStore from '@ohos.data.relationalStore';
import { PhotoModel } from '../model/PhotoModel';
import Constants from '../common/constants/Constants';
import common from '@ohos.app.ability.common';

class PhotoService {
  // 数据库操作实例
  private rdbStore: relationalStore.RdbStore | null = null;

  /**
   * 初始化数据库
   * @param context 应用上下文
   */
  initDB(context: common.UIAbilityContext): Promise<void> {
    const config: relationalStore.StoreConfig = {
      name: Constants.RDB_NAME,
      securityLevel: relationalStore.SecurityLevel.S1
    };

    return new Promise((resolve, reject) => {
      relationalStore.getRdbStore(context, config, (err, store) => {
        if (err) {
          console.error(`[PhotoService] 获取数据库失败: ${err.code}, ${err.message}`);
          reject(err);
          return;
        }

        this.rdbStore = store;
        // 如果表不存在，则创建表
        if (this.rdbStore) {
          this.rdbStore.executeSql(Constants.CREATE_TABLE_SQL);
          console.info('[PhotoService] 数据库初始化成功，表已就绪');
        }
        resolve();
      });
    });
  }

  /**
   * 插入一张图片数据
   * @param photo 图片对象 (id可传0，因为是自增的)
   * @returns Promise<number> 返回插入行的 rowId
   */
  insert(photo: PhotoModel): Promise<number> {
    if (!this.rdbStore) {
      console.error('[PhotoService] 数据库未初始化');
      return Promise.reject('DB not initialized');
    }

    const valueBucket: relationalStore.ValuesBucket = {};

    valueBucket[Constants.COL_NAME] = photo.name;
    valueBucket[Constants.COL_PATH] = photo.path;
    valueBucket[Constants.COL_CATEGORY] = photo.category;
    valueBucket[Constants.COL_CREATE_TIME] = photo.createTime;
    valueBucket[Constants.COL_TAGS] = photo.tags;

    return this.rdbStore.insert(Constants.PHOTO_TABLE_NAME, valueBucket);
  }

  /**
   * 查询所有图片
   * @returns Promise<PhotoModel[]> 图片列表
   */
  queryAll(): Promise<PhotoModel[]> {
    if (!this.rdbStore) {
      return Promise.reject('DB not initialized');
    }

    // 构建查询条件 (这里查询所有)
    let predicates = new relationalStore.RdbPredicates(Constants.PHOTO_TABLE_NAME);
    // 按创建时间倒序排列 (最新的在最前)
    predicates.orderByDesc(Constants.COL_CREATE_TIME);

    return this.rdbStore.query(predicates).then((resultSet) => {
      let count = resultSet.rowCount;
      console.info(`[PhotoService] 查询到 ${count} 条记录`);

      let result: PhotoModel[] = [];
      if (count === 0) {
        resultSet.close();
        return result;
      }

      resultSet.goToFirstRow();
      do {
        // 从结果集中提取数据
        const id = resultSet.getDouble(resultSet.getColumnIndex(Constants.COL_ID));
        const name = resultSet.getString(resultSet.getColumnIndex(Constants.COL_NAME));
        const path = resultSet.getString(resultSet.getColumnIndex(Constants.COL_PATH));
        const category = resultSet.getString(resultSet.getColumnIndex(Constants.COL_CATEGORY));
        const createTime = resultSet.getDouble(resultSet.getColumnIndex(Constants.COL_CREATE_TIME));
        const tags = resultSet.getString(resultSet.getColumnIndex(Constants.COL_TAGS));

        result.push(new PhotoModel(id, name, path, category, createTime, tags));
      } while (resultSet.goToNextRow());

      resultSet.close();
      return result;
    });
  }

  // ==========================================
  // 新增：搜索和分类功能
  // ==========================================

  /**
   * 根据分类查询图片
   * @param category 分类名称
   * @returns Promise<PhotoModel[]> 图片列表
   */
  queryByCategory(category: string): Promise<PhotoModel[]> {
    if (!this.rdbStore) {
      console.error('[PhotoService] 数据库未初始化');
      return Promise.reject('DB not initialized');
    }

    let predicates = new relationalStore.RdbPredicates(Constants.PHOTO_TABLE_NAME);
    predicates.equalTo(Constants.COL_CATEGORY, category);
    predicates.orderByDesc(Constants.COL_CREATE_TIME);

    return this.rdbStore.query(predicates).then((resultSet) => {
      return this.parseResultSet(resultSet);
    });
  }

  /**
   * 搜索图片（支持名称、分类和标签搜索）
   * @param keyword 搜索关键词
   * @returns Promise<PhotoModel[]> 图片列表
   */
  searchPhotos(keyword: string): Promise<PhotoModel[]> {
    if (!this.rdbStore) {
      console.error('[PhotoService] 数据库未初始化');
      return Promise.reject('DB not initialized');
    }

    if (!keyword || keyword.trim() === '') {
      return this.queryAll();
    }

    let predicates = new relationalStore.RdbPredicates(Constants.PHOTO_TABLE_NAME);

    // ==========================================
    // 🔧 修复：搜索名称、分类或标签包含关键词的图片
    // ==========================================
    predicates.like(Constants.COL_NAME, `%${keyword}%`)
      .or()
      .like(Constants.COL_CATEGORY, `%${keyword}%`) // 🆕 添加分类搜索
      .or()
      .like(Constants.COL_TAGS, `%${keyword}%`);

    predicates.orderByDesc(Constants.COL_CREATE_TIME);

    return this.rdbStore.query(predicates).then((resultSet) => {
      return this.parseResultSet(resultSet);
    });
  }

  /**
   * 组合搜索：按分类和关键词搜索
   * @param category 分类（传 '全部' 表示不限分类）
   * @param keyword 搜索关键词
   * @returns Promise<PhotoModel[]> 图片列表
   */
  searchPhotosByCategoryAndKeyword(category: string, keyword: string): Promise<PhotoModel[]> {
    if (!this.rdbStore) {
      console.error('[PhotoService] 数据库未初始化');
      return Promise.reject('DB not initialized');
    }

    console.info(`[PhotoService] 🔍 组合搜索 - 分类: "${category}", 关键词: "${keyword}"`);

    let predicates = new relationalStore.RdbPredicates(Constants.PHOTO_TABLE_NAME);

    // ==========================================
    // 🔧 修复：分类和关键词搜索逻辑
    // ==========================================

    // 情况1：只选择分类，无关键词 → 按分类查询
    if ((category && category !== '全部') && (!keyword || keyword.trim() === '')) {
      predicates.equalTo(Constants.COL_CATEGORY, category);
      console.info(`[PhotoService] 📂 按分类查询: ${category}`);
    }
    // 情况2：选择"全部"，有关键词 → 全局搜索
    else if ((category === '全部' || !category) && keyword && keyword.trim() !== '') {
      predicates.beginWrap()
        .like(Constants.COL_NAME, `%${keyword}%`)
        .or()
        .like(Constants.COL_CATEGORY, `%${keyword}%`) // 🆕 添加分类搜索
        .or()
        .like(Constants.COL_TAGS, `%${keyword}%`)
        .endWrap();
      console.info(`[PhotoService] 🔍 全局搜索: ${keyword}`);
    }
    // 情况3：既有分类又有关键词 → 先筛选分类，再搜索
    else if ((category && category !== '全部') && (keyword && keyword.trim() !== '')) {
      predicates.equalTo(Constants.COL_CATEGORY, category)
        .and()
        .beginWrap()
        .like(Constants.COL_NAME, `%${keyword}%`)
        .or()
        .like(Constants.COL_TAGS, `%${keyword}%`)
        .endWrap();
      console.info(`[PhotoService] 🔍📂 分类+搜索: ${category} + ${keyword}`);
    }
    // 情况4：都为空 → 查询全部
    else {
      console.info(`[PhotoService] 📋 查询全部`);
    }

    predicates.orderByDesc(Constants.COL_CREATE_TIME);

    return this.rdbStore.query(predicates).then((resultSet) => {
      const result = this.parseResultSet(resultSet);
      console.info(`[PhotoService] ✅ 查询结果: ${result.length} 条`);
      return result;
    });
  }

  /**
   * 获取所有分类列表（去重）
   * @returns Promise<string[]> 分类列表
   */
  getCategories(): Promise<string[]> {
    if (!this.rdbStore) {
      console.error('[PhotoService] 数据库未初始化');
      return Promise.reject('DB not initialized');
    }

    let predicates = new relationalStore.RdbPredicates(Constants.PHOTO_TABLE_NAME);

    return this.rdbStore.query(predicates, [Constants.COL_CATEGORY]).then((resultSet) => {
      let categories: string[] = [];
      let count = resultSet.rowCount;

      if (count === 0) {
        resultSet.close();
        return categories;
      }

      resultSet.goToFirstRow();
      do {
        const category = resultSet.getString(resultSet.getColumnIndex(Constants.COL_CATEGORY));
        if (category && category.trim() !== '' && !categories.includes(category)) {
          categories.push(category);
        }
      } while (resultSet.goToNextRow());

      resultSet.close();
      return categories;
    });
  }

  /**
   * 解析查询结果集（提取公共逻辑）
   * @param resultSet 查询结果集
   * @returns PhotoModel[] 图片列表
   */
  private parseResultSet(resultSet: relationalStore.ResultSet): PhotoModel[] {
    let count = resultSet.rowCount;
    console.info(`[PhotoService] 查询到 ${count} 条记录`);

    let result: PhotoModel[] = [];
    if (count === 0) {
      resultSet.close();
      return result;
    }

    resultSet.goToFirstRow();
    do {
      const id = resultSet.getDouble(resultSet.getColumnIndex(Constants.COL_ID));
      const name = resultSet.getString(resultSet.getColumnIndex(Constants.COL_NAME));
      const path = resultSet.getString(resultSet.getColumnIndex(Constants.COL_PATH));
      const category = resultSet.getString(resultSet.getColumnIndex(Constants.COL_CATEGORY));
      const createTime = resultSet.getDouble(resultSet.getColumnIndex(Constants.COL_CREATE_TIME));
      const tags = resultSet.getString(resultSet.getColumnIndex(Constants.COL_TAGS));

      result.push(new PhotoModel(id, name, path, category, createTime, tags));
    } while (resultSet.goToNextRow());

    resultSet.close();
    return result;
  }
}
// 导出单例对象，方便外部直接调用
export default new PhotoService();