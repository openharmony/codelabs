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
 * 相似度计算工具类
 * 提供多种字符串相似度算法实现
 */

/**
 * 相似度计算器类
 * 封装各种相似度计算算法
 */
export class SimilarityCalculator {

  /**
   * Levenshtein 编辑距离算法
   * 计算两个字符串之间的最小编辑操作数
   * @param str1 字符串1
   * @param str2 字符串2
   * @returns 编辑距离
   */
  static levenshteinDistance(str1: string, str2: string): number {
    const m = str1.length;
    const n = str2.length;

    // 创建距离矩阵
    const dp: number[][] = Array.from({ length: m + 1 }, () =>
      Array.from({ length: n + 1 }, () => 0)
    );

    // 初始化第一行和第一列
    for (let i = 0; i <= m; i++) {
      dp[i][0] = i;
    }
    for (let j = 0; j <= n; j++) {
      dp[0][j] = j;
    }

    // 填充矩阵
    for (let i = 1; i <= m; i++) {
      for (let j = 1; j <= n; j++) {
        if (str1[i - 1] === str2[j - 1]) {
          dp[i][j] = dp[i - 1][j - 1];
        } else {
          dp[i][j] = Math.min(
            dp[i - 1][j] + 1,     // 删除
            dp[i][j - 1] + 1,     // 插入
            dp[i - 1][j - 1] + 1  // 替换
          );
        }
      }
    }

    return dp[m][n];
  }

  /**
   * 基于 Levenshtein 距离的相似度
   * @param str1 字符串1
   * @param str2 字符串2
   * @returns 相似度 (0-1)
   */
  static levenshteinSimilarity(str1: string, str2: string): number {
    if (str1 === str2) return 1;
    if (str1.length === 0 || str2.length === 0) return 0;

    const distance = this.levenshteinDistance(str1, str2);
    const maxLength = Math.max(str1.length, str2.length);

    return 1 - distance / maxLength;
  }

  /**
   * Jaro 相似度算法
   * 适用于短字符串比较
   * @param str1 字符串1
   * @param str2 字符串2
   * @returns 相似度 (0-1)
   */
  static jaroSimilarity(str1: string, str2: string): number {
    if (str1 === str2) return 1;
    if (str1.length === 0 || str2.length === 0) return 0;

    const matchWindow = Math.floor(Math.max(str1.length, str2.length) / 2) - 1;
    const str1Matches: boolean[] = new Array(str1.length).fill(false);
    const str2Matches: boolean[] = new Array(str2.length).fill(false);

    let matches = 0;
    let transpositions = 0;

    // 查找匹配字符
    for (let i = 0; i < str1.length; i++) {
      const start = Math.max(0, i - matchWindow);
      const end = Math.min(i + matchWindow + 1, str2.length);

      for (let j = start; j < end; j++) {
        if (str2Matches[j] || str1[i] !== str2[j]) continue;
        str1Matches[i] = true;
        str2Matches[j] = true;
        matches++;
        break;
      }
    }

    if (matches === 0) return 0;

    // 计算换位数
    let k = 0;
    for (let i = 0; i < str1.length; i++) {
      if (!str1Matches[i]) continue;
      while (!str2Matches[k]) k++;
      if (str1[i] !== str2[k]) transpositions++;
      k++;
    }

    return (
      matches / str1.length +
      matches / str2.length +
      (matches - transpositions / 2) / matches
    ) / 3;
  }

  /**
   * Jaro-Winkler 相似度算法
   * Jaro 算法的改进版，对前缀相同的字符串给予更高权重
   * @param str1 字符串1
   * @param str2 字符串2
   * @param prefixScale 前缀权重因子 (默认0.1)
   * @returns 相似度 (0-1)
   */
  static jaroWinklerSimilarity(str1: string, str2: string, prefixScale: number = 0.1): number {
    const jaroSim = this.jaroSimilarity(str1, str2);

    // 计算公共前缀长度（最多4个字符）
    let prefixLength = 0;
    const maxPrefix = Math.min(4, str1.length, str2.length);
    for (let i = 0; i < maxPrefix; i++) {
      if (str1[i] === str2[i]) {
        prefixLength++;
      } else {
        break;
      }
    }

    return jaroSim + prefixLength * prefixScale * (1 - jaroSim);
  }

  /**
   * N-gram 相似度算法
   * 基于子串共现计算相似度
   * @param str1 字符串1
   * @param str2 字符串2
   * @param n N-gram 的 N 值 (默认2)
   * @returns 相似度 (0-1)
   */
  static ngramSimilarity(str1: string, str2: string, n: number = 2): number {
    if (str1 === str2) return 1;
    if (str1.length < n || str2.length < n) {
      return str1 === str2 ? 1 : 0;
    }

    const getNgrams = (str: string): Set<string> => {
      const ngrams = new Set<string>();
      for (let i = 0; i <= str.length - n; i++) {
        ngrams.add(str.substring(i, i + n));
      }
      return ngrams;
    };

    const ngrams1 = getNgrams(str1);
    const ngrams2 = getNgrams(str2);

    // 计算交集
    let intersection = 0;
    ngrams1.forEach(gram => {
      if (ngrams2.has(gram)) intersection++;
    });

    // 计算 Jaccard 相似度
    const union = ngrams1.size + ngrams2.size - intersection;
    return union > 0 ? intersection / union : 0;
  }

  /**
   * 余弦相似度（基于字符频率）
   * @param str1 字符串1
   * @param str2 字符串2
   * @returns 相似度 (0-1)
   */
  static cosineSimilarity(str1: string, str2: string): number {
    if (str1 === str2) return 1;
    if (str1.length === 0 || str2.length === 0) return 0;

    // 构建字符频率向量
    const getCharFrequency = (str: string): Map<string, number> => {
      const freq = new Map<string, number>();
      for (const char of str) {
        freq.set(char, (freq.get(char) || 0) + 1);
      }
      return freq;
    };

    const freq1 = getCharFrequency(str1);
    const freq2 = getCharFrequency(str2);

    // 计算点积
    let dotProduct = 0;
    freq1.forEach((count, char) => {
      if (freq2.has(char)) {
        dotProduct += count * freq2.get(char)!;
      }
    });

    // 计算向量模长
    let magnitude1 = 0;
    freq1.forEach(count => { magnitude1 += count * count; });
    magnitude1 = Math.sqrt(magnitude1);

    let magnitude2 = 0;
    freq2.forEach(count => { magnitude2 += count * count; });
    magnitude2 = Math.sqrt(magnitude2);

    return dotProduct / (magnitude1 * magnitude2);
  }

  /**
   * 电话号码专用相似度
   * 忽略格式差异（+86、空格、-等）
   * @param phone1 电话号码1
   * @param phone2 电话号码2
   * @returns 相似度 (0-1)
   */
  static phoneNumberSimilarity(phone1: string, phone2: string): number {
    // 标准化电话号码：只保留数字
    const normalize = (phone: string): string => {
      return phone.replace(/[^\d]/g, '');
    };

    const normalized1 = normalize(phone1);
    const normalized2 = normalize(phone2);

    if (normalized1 === normalized2) return 1;
    if (normalized1.length === 0 || normalized2.length === 0) return 0;

    // 处理国际区号差异
    const removeCountryCode = (phone: string): string => {
      // 移除常见国际区号
      if (phone.startsWith('86') && phone.length > 11) {
        return phone.substring(2);
      }
      if (phone.startsWith('0086') && phone.length > 13) {
        return phone.substring(4);
      }
      if (phone.startsWith('1') && phone.length === 11) {
        return phone;
      }
      return phone;
    };

    const cleaned1 = removeCountryCode(normalized1);
    const cleaned2 = removeCountryCode(normalized2);

    if (cleaned1 === cleaned2) return 1;

    // 检查是否一个是另一个的后缀
    if (cleaned1.endsWith(cleaned2) || cleaned2.endsWith(cleaned1)) {
      return 0.9;
    }

    // 使用 Levenshtein 相似度
    return this.levenshteinSimilarity(cleaned1, cleaned2);
  }

  /**
   * 邮箱专用相似度
   * 分别比较用户名和域名部分
   * @param email1 邮箱1
   * @param email2 邮箱2
   * @returns 相似度 (0-1)
   */
  static emailSimilarity(email1: string, email2: string): number {
    const normalize = (email: string): string => {
      return email.toLowerCase().trim();
    };

    const normalized1 = normalize(email1);
    const normalized2 = normalize(email2);

    if (normalized1 === normalized2) return 1;
    if (!normalized1.includes('@') || !normalized2.includes('@')) {
      return this.levenshteinSimilarity(normalized1, normalized2);
    }

    const [user1, domain1] = normalized1.split('@');
    const [user2, domain2] = normalized2.split('@');

    // 域名相同的情况下重点比较用户名
    if (domain1 === domain2) {
      return 0.3 + 0.7 * this.jaroWinklerSimilarity(user1, user2);
    }

    // 用户名相同的情况下
    if (user1 === user2) {
      return 0.5 + 0.5 * this.jaroWinklerSimilarity(domain1, domain2);
    }

    // 都不同的情况
    const userSim = this.jaroWinklerSimilarity(user1, user2);
    const domainSim = this.jaroWinklerSimilarity(domain1, domain2);

    return userSim * 0.6 + domainSim * 0.4;
  }

  /**
   * 中文姓名相似度
   * 针对中文姓名的特殊处理
   * @param name1 姓名1
   * @param name2 姓名2
   * @returns 相似度 (0-1)
   */
  static chineseNameSimilarity(name1: string, name2: string): number {
    const normalize = (name: string): string => {
      return name.replace(/\s+/g, '').toLowerCase();
    };

    const normalized1 = normalize(name1);
    const normalized2 = normalize(name2);

    if (normalized1 === normalized2) return 1;
    if (normalized1.length === 0 || normalized2.length === 0) return 0;

    // 检查是否是同一个人的不同称呼
    // 例如："张三" vs "张三丰", "小张" vs "张三"
    if (normalized1.includes(normalized2) || normalized2.includes(normalized1)) {
      const shorter = normalized1.length < normalized2.length ? normalized1 : normalized2;
      const longer = normalized1.length >= normalized2.length ? normalized1 : normalized2;
      return 0.5 + 0.5 * (shorter.length / longer.length);
    }

    // 中文姓名通常较短，使用字符级别的比较
    if (this.isChinese(normalized1) && this.isChinese(normalized2)) {
      // 检查姓是否相同（假设第一个字是姓）
      if (normalized1.length >= 1 && normalized2.length >= 1 && normalized1[0] === normalized2[0]) {
        // 姓相同，比较名
        const given1 = normalized1.substring(1);
        const given2 = normalized2.substring(1);
        const givenSim = this.levenshteinSimilarity(given1, given2);
        return 0.3 + 0.7 * givenSim;
      }
    }

    return this.jaroWinklerSimilarity(normalized1, normalized2);
  }

  /**
   * 判断字符串是否主要包含中文
   */
  static isChinese(str: string): boolean {
    const chineseCount = (str.match(/[\u4e00-\u9fa5]/g) || []).length;
    return chineseCount > str.length / 2;
  }

  /**
   * 综合相似度计算
   * 根据字符串特征选择最佳算法
   * @param str1 字符串1
   * @param str2 字符串2
   * @returns 相似度 (0-1)
   */
  static smartSimilarity(str1: string, str2: string): number {
    if (str1 === str2) return 1;
    if (str1.length === 0 || str2.length === 0) return 0;

    // 对于短字符串使用 Jaro-Winkler
    if (str1.length <= 5 && str2.length <= 5) {
      return this.jaroWinklerSimilarity(str1, str2);
    }

    // 对于中等长度使用综合方法
    const jaroWinkler = this.jaroWinklerSimilarity(str1, str2);
    const ngram = this.ngramSimilarity(str1, str2);
    const levenshtein = this.levenshteinSimilarity(str1, str2);

    // 加权平均
    return jaroWinkler * 0.4 + ngram * 0.3 + levenshtein * 0.3;
  }

  /**
   * 批量计算相似度矩阵
   * @param strings 字符串数组
   * @param algorithm 使用的算法
   * @returns 相似度矩阵
   */
  static computeSimilarityMatrix(
    strings: string[],
    algorithm: 'levenshtein' | 'jaro' | 'jaroWinkler' | 'ngram' | 'smart' = 'smart'
  ): number[][] {
    const n = strings.length;
    const matrix: number[][] = Array.from({ length: n }, () =>
      Array.from({ length: n }, () => 0)
    );

    const calcFunc = this.getAlgorithmFunction(algorithm);

    for (let i = 0; i < n; i++) {
      matrix[i][i] = 1; // 对角线为1
      for (let j = i + 1; j < n; j++) {
        const sim = calcFunc(strings[i], strings[j]);
        matrix[i][j] = sim;
        matrix[j][i] = sim;
      }
    }

    return matrix;
  }

  /**
   * 获取算法函数
   */
  private static getAlgorithmFunction(
    algorithm: 'levenshtein' | 'jaro' | 'jaroWinkler' | 'ngram' | 'smart'
  ): (str1: string, str2: string) => number {
    switch (algorithm) {
      case 'levenshtein':
        return this.levenshteinSimilarity.bind(this);
      case 'jaro':
        return this.jaroSimilarity.bind(this);
      case 'jaroWinkler':
        return this.jaroWinklerSimilarity.bind(this);
      case 'ngram':
        return this.ngramSimilarity.bind(this);
      case 'smart':
      default:
        return this.smartSimilarity.bind(this);
    }
  }
}
