/*
 * Copyright (c) 2026 Beijing Institute of Technology
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

export interface Book {
  id: string
  title: string
  author: string
  year: string
  publisher: string
  tags: string[]
  rating: number
  ratingCount: number
  summary: string
  coverRes: Resource
  otherEditions?: number
  badgeText?: string
  baseRating?: number
  baseRatingCount?: number
}

export const initialBooks: Book[] = [
  {
    id: 'book_java_4',
    title: 'Java 编程思想（第4版）',
    author: 'Bruce Eckel',
    year: '2007',
    publisher: '机械工业出版社',
    tags: ['计算机'],
    rating: 9.1,
    ratingCount: 100,
    summary: '经典 Java 入门与进阶读物，覆盖语言特性、集合、并发与最佳实践。',
    coverRes: $r('app.media.java'),
    otherEditions: 9,
    badgeText: '殿堂级'
  },
  {
    id: 'book_deep_learning',
    title: '深度学习：基础与概念',
    author: 'Christopher M. Bishop',
    year: '2025',
    publisher: '人民邮电出版社',
    tags: ['计算机'],
    rating: 8.9,
    ratingCount: 100,
    summary: '系统梳理深度学习的核心概念、理论基础与经典架构，适合打基础与查漏补缺。',
    coverRes: $r('app.media.dl'),
    otherEditions: 12
  },
  {
    id: 'book_llm_from_scratch',
    title: '从零构建大模型',
    author: 'Sebastian Raschka',
    year: '2025',
    publisher: '人民邮电出版社',
    tags: ['计算机'],
    rating: 9.3,
    ratingCount: 100,
    summary: '从数据、训练到推理与优化的全流程实战指南，配套示例清晰，适合边做边学。',
    coverRes: $r('app.media.llm'),
    otherEditions: 6,
    badgeText: '硬核干货'
  },
  {
    id: 'book_clean_code',
    title: '代码整洁之道',
    author: 'Robert C. Martin',
    year: '2010',
    publisher: '人民邮电出版社',
    tags: ['计算机'],
    rating: 8.7,
    ratingCount: 100,
    summary: '以大量代码示例讨论命名、函数、注释、测试与重构，帮助建立可维护代码的直觉。',
    coverRes: $r('app.media.clean_code'),
    otherEditions: 18
  },
  {
    id: 'book_brief_history_of_time',
    title: '时间简史',
    author: 'Stephen Hawking',
    year: '2011',
    publisher: '湖南科学技术出版社',
    tags: ['科学技术'],
    rating: 8.3,
    ratingCount: 100,
    summary: '用通俗语言谈宇宙学的关键问题：时间、黑洞、基本粒子与宇宙的起源。',
    coverRes: $r('app.media.time'),
    otherEditions: 84
  },
  {
    id: 'book_math_girls_5',
    title: '数学女孩5：伽罗瓦理论',
    author: '结城浩',
    year: '2021',
    publisher: '人民邮电出版社',
    tags: ['数学'],
    rating: 9.4,
    ratingCount: 100,
    summary: '日本数学会强力推荐，在动人的故事中亲近数学，在青春的浪漫中理解数学。',
    coverRes: $r('app.media.math_girl_5'),
    badgeText: '入门首选'
  },
  {
    id: 'book_linear_algebra',
    title: '线性代数（第5版）',
    author: 'Gilbert Strang',
    year: '2019',
    publisher: '清华大学出版社',
    tags: ['数学'],
    rating: 9.4,
    ratingCount: 100,
    summary: '线性代数的经典教材，注重理解与应用，配合 MIT 公开课食用更佳。',
    coverRes: $r('app.media.linear_algebra'),
    badgeText: '殿堂级'
  },
  {
    id: 'book_understanding_dl',
    title: '理解深度学习',
    author: 'Simon J.D. Prince',
    year: '2025',
    publisher: '清华大学出版社',
    tags: ['计算机'],
    rating: 9.2,
    ratingCount: 100,
    summary: '从深度学习的基础概念到前沿技术，全面解析，适合初学者与进阶者。',
    coverRes: $r('app.media.understand_dl'),
    badgeText: '硬核干货'
  },
  {
    id: 'book_physics_evolution',
    title: '物理学的进化',
    author: 'Albert Einstein / Leopold Infeld',
    year: '2020',
    publisher: '商务印书馆',
    tags: ['科学技术'],
    rating: 9.9,
    ratingCount: 100,
    summary: '物理学科普名著，介绍物理学基本概念的演变。',
    coverRes: $r('app.media.the_evolution_of_physics'),
    badgeText: '殿堂级'
  },
  {
    id: 'book_prml',
    title: '模式识别与机器学习',
    author: 'Christopher M. Bishop',
    year: '2025',
    publisher: '人民邮电出版社',
    tags: ['计算机'],
    rating: 9.5,
    ratingCount: 100,
    summary: '机器学习领域的圣经级教材，贝叶斯方法的集大成者。',
    coverRes: $r('app.media.ml'),
    badgeText: '殿堂级'
  },
  {
    id: 'book_men_of_math',
    title: '数学大师：从芝诺到庞加莱',
    author: 'Eric Temple Bell',
    year: '2018',
    publisher: '上海科技教育出版社',
    tags: ['数学'],
    rating: 8.9,
    ratingCount:100,
    summary: '生动讲述历史上伟大数学家的生平与成就，激励无数人的数学启蒙书。',
    coverRes: $r('app.media.math_master')
  },
  {
    id: 'book_math_girls_2',
    title: '数学女孩2：费马大定理',
    author: '结城浩',
    year: '2015',
    publisher: '人民邮电出版社',
    tags: ['数学'],
    rating: 9.3,
    ratingCount: 100,
    summary: '用青春故事讲述费马大定理的证明历程，数学科普神作。',
    coverRes: $r('app.media.math_girl_2'),
    badgeText: '殿堂级'
  }
]