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
/**
 * 图片数据模型
 * 对应数据库中的表结构
 */
export class PhotoModel {
    /** 主键 ID (数据库自动生成) */
    id: number;
    /** 图片名称 */
    name: string;
    /** 沙箱路径 (例如: /data/.../files/img_123.jpg) */
    path: string;
    /** 分类 (例如: 生活, 风景) */
    category: string;
    /** 创建时间戳 */
    createTime: number;
    /** 标签 (例如: #开心 #旅行) */
    tags: string;
    constructor(id: number, name: string, path: string, category: string, createTime: number, tags: string) {
        this.id = id;
        this.name = name;
        this.path = path;
        this.category = category;
        this.createTime = createTime;
        this.tags = tags;
    }
    /**
     * 获取标签数组
     * @returns 标签数组（去除 # 符号和空格）
     */
    getTagArray(): string[] {
        if (!this.tags || this.tags.trim() === '') {
            return [];
        }
        // 按空格分割，移除 # 符号，过滤空字符串
        return this.tags
            .split(/\s+/)
            .map(tag => tag.replace('#', '').trim())
            .filter(tag => tag.length > 0);
    }
    /**
     * 设置标签数组
     * @param tagArray 标签数组
     */
    setTagArray(tagArray: string[]) {
        this.tags = tagArray.map(tag => `#${tag}`).join(' ');
    }
    /**
     * 检查是否包含指定标签
     * @param keyword 关键词
     * @returns 是否包含
     */
    hasTag(keyword: string): boolean {
        const tagArray = this.getTagArray();
        const lowerKeyword = keyword.toLowerCase();
        return tagArray.some(tag => tag.toLowerCase().includes(lowerKeyword));
    }
    /**
     * 检查名称或标签是否包含关键词
     * @param keyword 搜索关键词
     * @returns 是否匹配
     */
    matchesKeyword(keyword: string): boolean {
        const lowerKeyword = keyword.toLowerCase();
        const nameMatch = this.name.toLowerCase().includes(lowerKeyword);
        const tagMatch = this.hasTag(keyword);
        return nameMatch || tagMatch;
    }
}
