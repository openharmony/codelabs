/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
import HealthConsultFavoriteModel from './HealthConsultFavoriteModel';

export interface HealthConsultData {
  id: number;
  title: string;
  abstract: string;
  tabId: number;
  images: Resource[]
}

export const HEALTH_CONSULT_DATABASE: HealthConsultData[] = [
  {
    id: 1,
    title: "优质蛋白质食物清单：除了鸡胸肉，还有哪些选择？",
    abstract: "全面解析15种高性价比蛋白质来源，涵盖动物性与植物性蛋白。详细对比每100克食物中蛋白质含量、吸收率及氨基酸评分，特别标注适合素食者、乳糖不耐受人群的替代选项。附赠一周蛋白质搭配方案，讲解如何通过食物组合实现氨基酸互补，提升蛋白质利用率。",
    tabId: 1,
    images: [$rawfile('coverarticle1.jpg'), $rawfile('coverarticle3.jpg'), $rawfile('coverarticle5.jpg')]
  },
  {
    id: 2,
    title: "控糖饮食全攻略：如何识别隐藏糖分？",
    abstract: "深度剖析食品标签中超过50种糖分别名（如果葡糖浆、麦芽糖浆等），提供超市选购避坑指南。详细讲解血糖生成指数与血糖负荷概念的区别及应用，包含常见主食、水果的GL值对照表。提供四阶段减糖计划，从戒除含糖饮料到优化碳水化合物选择，配套20道低糖食谱。",
    tabId: 1,
    images: [$rawfile('coverarticle2.jpg'), $rawfile('coverarticle4.jpg')]
  },
  {
    id: 3,
    title: "微量元素补充指南：缺铁、缺锌的饮食解决方案",
    abstract: "针对贫血、免疫力下降等常见症状，提供详细的自我评估问卷。深入解析血红素铁与非血红素铁的吸收差异，列出促进与抑制铁吸收的食物组合表。包含针对不同人群（孕妇、儿童、老年人）的微量元素需求差异及补充方案，提供30种富含铁、锌、硒的食材料理创意。",
    tabId: 1,
    images: [$rawfile('coverarticle5.jpg'), $rawfile('coverarticle3.jpg')]
  },
  {
    id: 4,
    title: "居家力量训练入门：无需器械的全身激活方案",
    abstract: "从零开始建立力量训练体系，详细分解12个基础自重动作的标准姿势、呼吸节奏与常见错误纠正。包含针对上肢、核心、下肢的3套分级训练计划（初级/中级/高级），每周3次，每次30分钟。特别提供家庭安全训练环境布置建议及训练前后的动态拉伸与放松流程。",
    tabId: 2,
    images: [$rawfile('coverarticle6.jpg'), $rawfile('coverarticle4.jpg'), $rawfile('coverarticle2.jpg')]
  },
  {
    id: 5,
    title: "跑步伤痛预防：从跑姿分析到恢复策略",
    abstract: "系统讲解跑姿生物力学分析，通过步频、触地时间等数据优化跑步效率。深度解析髂胫束综合征、胫骨应力综合征等5种常见伤病的成因图谱。提供包含强化训练、灵活性改善、跑步计划调整在内的三维预防方案，以及伤后恢复期的交叉训练建议与重返跑道的标准流程。",
    tabId: 2,
    images: [$rawfile('coverarticle2.jpg'), $rawfile('coverarticle1.jpg')]
  },
  {
    id: 6,
    title: "高强度间歇训练（HIIT）的误区与科学方案",
    abstract: "破除“HIIT适合所有人”、“时间越短越好”等常见迷思，基于最新运动科学研究界定安全有效的人群范围。提供基于心率区间的强度监控方法，设计4套不同主题的HIIT方案（燃脂导向、心肺提升、力量耐力、时间紧缺型），每套包含详细的热身、主训、冷身结构及进阶降阶选择。",
    tabId: 2,
    images: [$rawfile('coverarticle1.jpg'), $rawfile('coverarticle3.jpg'), $rawfile('coverarticle5.jpg')]
  },
  {
    id: 7,
    title: "正念减压练习：5分钟办公室冥想指南",
    abstract: "提供一套完整的办公室场景正念练习体系，包含呼吸觉察、身体扫描、声音冥想等7种微型练习法。详细指导如何应对冥想过程中的杂念与烦躁情绪。包含将正念融入日常工作沟通、邮件处理、会议准备的具体技巧，附赠21天正念练习日历与进度追踪表。",
    tabId: 3,
    images: [$rawfile('coverarticle6.jpg'), $rawfile('coverarticle4.jpg'), $rawfile('coverarticle2.jpg')]
  },
  {
    id: 8,
    title: "睡眠质量提升方案：从环境调整到认知行为疗法",
    abstract: "基于睡眠卫生原则，提供卧室环境优化清单（光照、噪音、温湿度、寝具的科学参数）。详细教学刺激控制疗法与睡眠限制疗法的分步操作，帮助重建床与睡眠的联结。包含针对“睡前思维反刍”的认知重构技巧，以及针对不同失眠类型（入睡困难、易醒、早醒）的差异化应对策略。",
    tabId: 3,
    images: [$rawfile('coverarticle5.jpg'), $rawfile('coverarticle3.jpg')]
  },
  {
    id: 9,
    title: "高血压患者日常管理：饮食、运动与监测要点",
    abstract: "深度解读DASH饮食的实践细节，提供符合中国口味的低钠高钾食谱（精确到毫克钠含量）。详解安全有效的有氧运动与抗阻运动方案，包括心率监控与自觉劳累分级应用。教学正确的家庭血压监测流程（时间、姿势、记录方法）及血压日志解读，提供与医生沟通血压数据的技巧与复诊准备清单。",
    tabId: 4,
    images: [$rawfile('coverarticle1.jpg'), $rawfile('coverarticle3.jpg'), $rawfile('coverarticle5.jpg')]
  },
  {
    id: 10,
    title: "糖尿病饮食计算指南：如何科学规划碳水化合物？",
    abstract: "系统教学碳水化合物计数法，提供常见主食、水果、零食的碳水化合物换算表。详解血糖负荷概念在食物选择和份量控制中的应用。包含不同餐次（三餐、加餐）及不同场景（外出就餐、旅行）下的饮食规划模板。提供应对高血糖与低血糖的即时饮食调整方案及长期血糖趋势分析方法。",
    tabId: 4,
    images: [$rawfile('coverarticle4.jpg')]
  },
  {
    id: 11,
    title: "春季养肝正当时：饮食调理与情绪管理",
    abstract: "结合中医“春应肝而养生”理论，详细讲解春季肝气升发的特点及常见不适的调理思路。提供20种舒肝理气、养血柔肝的应季食材解析与烹饪建议，配套春季养肝茶饮与汤膳配方。包含针对春季情绪易波动的情绪调节技巧（如户外散步、舒展运动）与作息调整建议，助力身心同步焕新。",
    tabId: 5,
    images: [$rawfile('coverarticle5.jpg'), $rawfile('coverarticle3.jpg')]
  },
  {
    id: 12,
    title: "夏季防暑降温指南：科学补水与食物选择",
    abstract: "详解高温环境下人体水分与电解质的流失特点，提供基于出汗量的个性化补水公式。列出富含电解质且性质寒凉的天然食物清单，提供夏日清爽食谱（凉拌、蒸煮技巧）。包含不同人群（户外工作者、儿童、老年人）的防暑重点差异，以及识别与应对先兆中暑、轻度中暑的应急处理步骤。",
    tabId: 5,
    images: [$rawfile('coverarticle2.jpg'), $rawfile('coverarticle4.jpg')]
  },
  {
    id: 13,
    title: "久坐族肩颈自救指南：办公室微运动方案",
    abstract: "深度分析“上交叉综合征”的成因，提供自我体态评估方法。设计一套包含颈部灵活度、胸椎伸展、肩袖肌群强化、菱形肌激活在内的12个微运动组合，每个动作配有详细的办公室座椅版本和站立版本。包含工作间歇的定时提醒设置建议，以及办公桌、显示器、键盘的人体工学调整参数指南。",
    tabId: 6,
    images: [$rawfile('coverarticle2.jpg'), $rawfile('coverarticle4.jpg')]
  },
  {
    id: 14,
    title: "职场疲劳恢复：科学安排工作休息节奏",
    abstract: "引入“超日节律”概念，讲解人体专注力的自然波动周期。提供基于90-120分钟工作周期的深度工作与休息安排模板。详细介绍5种高效休息法（冥想休息、动态休息、自然接触休息等）及其实施场景。包含应对脑力疲劳与情绪疲劳的差异化恢复策略，以及周末科学休息与假期规划的建议。",
    tabId: 6,
    images: [$rawfile('coverarticle5.jpg'), $rawfile('coverarticle3.jpg')]
  },
  {
    id: 15,
    title: "视频会议综合征应对：缓解眼疲劳与姿势不良",
    abstract: "提供详细的视频会议环境设置指南（光线、摄像头角度、背景）。教学针对数码眼疲劳的“20-20-20”法则加强版练习及眼部放松按摩手法。包含长时间静坐时的动态微调姿势技巧，以及适合会议间隙进行的上半身拉伸序列。提供蓝牙耳机选择建议及通话时的嗓音保护技巧。",
    tabId: 6,
    images: [$rawfile('coverarticle1.jpg'), $rawfile('coverarticle3.jpg'), $rawfile('coverarticle5.jpg')]
  },
  {
    id: 16,
    title: "心肺复苏（CPR）操作要点：关键时刻的急救步骤",
    abstract: "以最新国际指南为准，分步图解演示成人、儿童、婴儿心肺复苏的全流程，重点强调胸外按压的深度、频率、回弹及最小化中断原则。详细讲解自动体外除颤器（AED）的获取、开启、贴片放置及电击全过程，包含特殊情境（溺水、触电）下的CPR调整要点。提供急救前的现场安全评估与求助方法。",
    tabId: 7,
    images: [$rawfile('coverarticle3.jpg'), $rawfile('coverarticle2.jpg')]
  },
  {
    id: 17,
    title: "家庭急救箱必备物品清单与使用指南",
    abstract: "根据不同家庭结构（有无老人、儿童、慢性病患者）定制三级急救箱配置清单（基础版、进阶版、专业版）。逐一讲解每种物品（如不同规格的绷带、敷料、止血带、体温计等）的适用场景、使用方法和注意事项。包含药品的定期检查与更换指南，以及制作家庭应急联系卡与医疗信息卡的建议。",
    tabId: 7,
    images: [$rawfile('coverarticle6.jpg'), $rawfile('coverarticle4.jpg'), $rawfile('coverarticle2.jpg')]
  }
]

// == 下方为最终用于显示的模型与查询实现 ==

export interface HealthConsultWithFavoriteData {
  healthConsultData: HealthConsultData
  isFavorite: boolean
}

export async function queryHealthConsultItem(tabId: number, keywords: string) {
  let keywordArr = keywords.split(' ').filter(Boolean).map(s => s.toLowerCase()); // 获取关键词，扔掉空串，转小写
  let tmpItemData: HealthConsultData[] = [];

  if (keywordArr.length === 0) {
    // 没有指定查询关键词
    if (tabId == 0) {
      // 全部类型
      tmpItemData = HEALTH_CONSULT_DATABASE;
    } else {
      // 指定类型
      tmpItemData = HEALTH_CONSULT_DATABASE.filter(item => {
        return item.tabId === tabId
      })
    }
  } else {
    if (tabId == 0) {
      // 全部类型
      tmpItemData = HEALTH_CONSULT_DATABASE.filter(item => {
        let titleLowerCase = item.title.toLowerCase();
        if (keywordArr.some(keyword => titleLowerCase.includes(keyword))) {
          return true;
        }
        let abstractLowerCase = item.abstract.toLowerCase();
        return keywordArr.some(keyword => abstractLowerCase.includes(keyword));
      });
    } else {
      // 指定类型
      tmpItemData = HEALTH_CONSULT_DATABASE.filter(item => {
        if (item.tabId !== tabId) {
          return false;
        }
        let titleLowerCase = item.title.toLowerCase();
        if (keywordArr.some(keyword => titleLowerCase.includes(keyword))) {
          return true;
        }
        let abstractLowerCase = item.abstract.toLowerCase();
        return keywordArr.some(keyword => abstractLowerCase.includes(keyword));
      })
    }
  }

  let finalData: HealthConsultWithFavoriteData[] = [];
  for (let i = 0; i < tmpItemData.length; i++) {
    finalData.push({
      healthConsultData: tmpItemData[i],
      isFavorite: await HealthConsultFavoriteModel.getFavorite(tmpItemData[i].id)
    })
  }

  finalData.sort((a, b) => {
    return Number(b.isFavorite) - Number(a.isFavorite)
  })
  return finalData
}
