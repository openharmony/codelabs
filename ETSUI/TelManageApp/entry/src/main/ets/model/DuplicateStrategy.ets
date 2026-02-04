/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-20
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * 联系人去重策略枚举与配置
 * 定义多种去重检测策略，支持灵活配置
 */

/**
 * 去重策略类型枚举
 * 定义不同的重复检测维度
 */
export enum DuplicateStrategyType {
  // 精确匹配策略
  EXACT_PHONE = 'EXACT_PHONE',           // 电话号码完全相同
  EXACT_NAME = 'EXACT_NAME',             // 姓名完全相同
  EXACT_EMAIL = 'EXACT_EMAIL',           // 邮箱完全相同

  // 模糊匹配策略
  FUZZY_PHONE = 'FUZZY_PHONE',           // 电话号码模糊匹配（忽略格式差异）
  FUZZY_NAME = 'FUZZY_NAME',             // 姓名模糊匹配（相似度阈值）
  SIMILAR_PINYIN = 'SIMILAR_PINYIN',     // 拼音相似匹配

  // 组合策略
  PHONE_AND_NAME = 'PHONE_AND_NAME',     // 电话+姓名同时匹配
  MULTI_FIELD = 'MULTI_FIELD',           // 多字段综合评分

  // 智能策略
  SMART_DETECT = 'SMART_DETECT'          // 智能检测（基于多维度加权）
}

/**
 * 去重严格程度枚举
 */
export enum DuplicateStrictLevel {
  STRICT = 'STRICT',       // 严格模式：只有完全匹配才视为重复
  NORMAL = 'NORMAL',       // 普通模式：一定相似度即视为重复
  LOOSE = 'LOOSE'          // 宽松模式：较低相似度即视为重复
}

/**
 * 合并冲突解决策略
 */
export enum MergeConflictResolution {
  KEEP_FIRST = 'KEEP_FIRST',             // 保留第一个值
  KEEP_LAST = 'KEEP_LAST',               // 保留最后一个值
  KEEP_LONGEST = 'KEEP_LONGEST',         // 保留最长的值
  KEEP_NEWEST = 'KEEP_NEWEST',           // 保留最新的值
  MERGE_ALL = 'MERGE_ALL',               // 合并所有值
  ASK_USER = 'ASK_USER'                  // 询问用户
}

/**
 * 字段权重配置接口
 * 用于多字段综合评分策略
 */
export interface FieldWeightConfig {
  phoneWeight: number;      // 电话号码权重 (0-100)
  nameWeight: number;       // 姓名权重 (0-100)
  emailWeight: number;      // 邮箱权重 (0-100)
  organizationWeight: number; // 组织/公司权重 (0-100)
}

/**
 * 去重策略配置接口
 */
export interface DuplicateStrategyConfig {
  // 基本配置
  strategyType: DuplicateStrategyType;
  strictLevel: DuplicateStrictLevel;

  // 相似度阈值配置
  phoneSimThreshold: number;     // 电话相似度阈值 (0-1)
  nameSimThreshold: number;      // 姓名相似度阈值 (0-1)
  emailSimThreshold: number;     // 邮箱相似度阈值 (0-1)
  overallSimThreshold: number;   // 综合相似度阈值 (0-1)

  // 字段权重
  fieldWeights: FieldWeightConfig;

  // 合并策略
  conflictResolution: MergeConflictResolution;

  // 高级配置
  ignorePhoneFormat: boolean;    // 是否忽略电话格式（+86、空格、-等）
  ignoreCaseInName: boolean;     // 姓名比较是否忽略大小写
  ignoreWhitespace: boolean;     // 是否忽略空白字符
  enablePinyinMatch: boolean;    // 是否启用拼音匹配
}

/**
 * 默认去重策略配置
 */
export const DEFAULT_DUPLICATE_STRATEGY_CONFIG: DuplicateStrategyConfig = {
  strategyType: DuplicateStrategyType.SMART_DETECT,
  strictLevel: DuplicateStrictLevel.NORMAL,

  phoneSimThreshold: 0.9,
  nameSimThreshold: 0.8,
  emailSimThreshold: 0.95,
  overallSimThreshold: 0.75,

  fieldWeights: {
    phoneWeight: 40,
    nameWeight: 35,
    emailWeight: 15,
    organizationWeight: 10
  },

  conflictResolution: MergeConflictResolution.KEEP_LONGEST,

  ignorePhoneFormat: true,
  ignoreCaseInName: true,
  ignoreWhitespace: true,
  enablePinyinMatch: false
};

/**
 * 辅助函数：创建默认配置的副本
 */
function createDefaultConfig(): DuplicateStrategyConfig {
  const config: DuplicateStrategyConfig = {
    strategyType: DEFAULT_DUPLICATE_STRATEGY_CONFIG.strategyType,
    strictLevel: DEFAULT_DUPLICATE_STRATEGY_CONFIG.strictLevel,
    phoneSimThreshold: DEFAULT_DUPLICATE_STRATEGY_CONFIG.phoneSimThreshold,
    nameSimThreshold: DEFAULT_DUPLICATE_STRATEGY_CONFIG.nameSimThreshold,
    emailSimThreshold: DEFAULT_DUPLICATE_STRATEGY_CONFIG.emailSimThreshold,
    overallSimThreshold: DEFAULT_DUPLICATE_STRATEGY_CONFIG.overallSimThreshold,
    fieldWeights: {
      phoneWeight: DEFAULT_DUPLICATE_STRATEGY_CONFIG.fieldWeights.phoneWeight,
      nameWeight: DEFAULT_DUPLICATE_STRATEGY_CONFIG.fieldWeights.nameWeight,
      emailWeight: DEFAULT_DUPLICATE_STRATEGY_CONFIG.fieldWeights.emailWeight,
      organizationWeight: DEFAULT_DUPLICATE_STRATEGY_CONFIG.fieldWeights.organizationWeight
    },
    conflictResolution: DEFAULT_DUPLICATE_STRATEGY_CONFIG.conflictResolution,
    ignorePhoneFormat: DEFAULT_DUPLICATE_STRATEGY_CONFIG.ignorePhoneFormat,
    ignoreCaseInName: DEFAULT_DUPLICATE_STRATEGY_CONFIG.ignoreCaseInName,
    ignoreWhitespace: DEFAULT_DUPLICATE_STRATEGY_CONFIG.ignoreWhitespace,
    enablePinyinMatch: DEFAULT_DUPLICATE_STRATEGY_CONFIG.enablePinyinMatch
  };
  return config;
}

/**
 * 预设策略配置工厂
 * 提供常用的策略配置预设
 */
export class DuplicateStrategyPresets {
  /**
   * 严格去重配置
   * 只有完全相同才视为重复
   */
  static getStrictConfig(): DuplicateStrategyConfig {
    const config: DuplicateStrategyConfig = createDefaultConfig();
    config.strategyType = DuplicateStrategyType.EXACT_PHONE;
    config.strictLevel = DuplicateStrictLevel.STRICT;
    config.phoneSimThreshold = 1.0;
    config.nameSimThreshold = 1.0;
    config.emailSimThreshold = 1.0;
    config.overallSimThreshold = 0.95;
    config.ignorePhoneFormat = false;
    config.ignoreCaseInName = false;
    return config;
  }

  /**
   * 宽松去重配置
   * 适用于清理大量重复联系人
   */
  static getLooseConfig(): DuplicateStrategyConfig {
    const config: DuplicateStrategyConfig = createDefaultConfig();
    config.strategyType = DuplicateStrategyType.SMART_DETECT;
    config.strictLevel = DuplicateStrictLevel.LOOSE;
    config.phoneSimThreshold = 0.7;
    config.nameSimThreshold = 0.6;
    config.emailSimThreshold = 0.8;
    config.overallSimThreshold = 0.5;
    config.enablePinyinMatch = true;
    return config;
  }

  /**
   * 仅电话去重配置
   * 只根据电话号码判断重复
   */
  static getPhoneOnlyConfig(): DuplicateStrategyConfig {
    const config: DuplicateStrategyConfig = createDefaultConfig();
    config.strategyType = DuplicateStrategyType.FUZZY_PHONE;
    config.fieldWeights = {
      phoneWeight: 100,
      nameWeight: 0,
      emailWeight: 0,
      organizationWeight: 0
    };
    return config;
  }

  /**
   * 姓名+电话组合去重配置
   */
  static getNamePhoneComboConfig(): DuplicateStrategyConfig {
    const config: DuplicateStrategyConfig = createDefaultConfig();
    config.strategyType = DuplicateStrategyType.PHONE_AND_NAME;
    config.fieldWeights = {
      phoneWeight: 50,
      nameWeight: 50,
      emailWeight: 0,
      organizationWeight: 0
    };
    return config;
  }
}

/**
 * 策略类型描述映射
 */
export const STRATEGY_TYPE_DESCRIPTIONS: Map<DuplicateStrategyType, string> = new Map([
  [DuplicateStrategyType.EXACT_PHONE, '电话精确匹配'],
  [DuplicateStrategyType.EXACT_NAME, '姓名精确匹配'],
  [DuplicateStrategyType.EXACT_EMAIL, '邮箱精确匹配'],
  [DuplicateStrategyType.FUZZY_PHONE, '电话模糊匹配'],
  [DuplicateStrategyType.FUZZY_NAME, '姓名模糊匹配'],
  [DuplicateStrategyType.SIMILAR_PINYIN, '拼音相似匹配'],
  [DuplicateStrategyType.PHONE_AND_NAME, '电话+姓名组合'],
  [DuplicateStrategyType.MULTI_FIELD, '多字段综合评分'],
  [DuplicateStrategyType.SMART_DETECT, '智能检测']
]);

/**
 * 严格程度描述映射
 */
export const STRICT_LEVEL_DESCRIPTIONS: Map<DuplicateStrictLevel, string> = new Map([
  [DuplicateStrictLevel.STRICT, '严格模式 - 仅完全匹配'],
  [DuplicateStrictLevel.NORMAL, '普通模式 - 平衡准确率'],
  [DuplicateStrictLevel.LOOSE, '宽松模式 - 更多匹配']
]);
