/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
/**
 * @file PetManagementPro.ets
 * @description 高级宠物基地管理系统 - 旗舰增强版 (Swiper 架构全量版)
 * * 交互规范：
 * 1. 单击列表项：进入三段式沉浸 Swiper 详情卡片
 * 2. 详情页长按：触发编辑表单
 * 3. 关联数据：包含基础信息、日常动态记录、花销账单统计
 */

import promptAction from '@ohos.promptAction';
import { DatabaseManager } from '../models/DatabaseManager';
import { Pet } from '../models/PetModel';

// --- 业务模型定义 (用于支撑多页数据显示) ---

interface PetHistoryRecord {
  id: number;
  type: string;
  date: string;
  content: string;
}

interface PetFinancialCost {
  id: number;
  amount: number;
  category: string;
  date: string;
}

// ============================================================================
// --- 1. 全局业务配置与枚举定义 ---
// ============================================================================

enum GenderType {
  BOY = '公',
  GIRL = '母',
  SECRET = '保密'
}

class ThemeConfig {
  static readonly PRIMARY_COLOR: string = '#007DFF';
  static readonly SECONDARY_COLOR: string = '#63B3ED';
  static readonly ERROR_COLOR: string = '#FF4550';
  static readonly PAGE_BG: string = '#F7FAFC';
  static readonly CARD_BG: string = '#FFFFFF';
  static readonly TEXT_MAIN: string = '#1A202C';
  static readonly TEXT_SUB: string = '#718096';
  static readonly RADIUS_LG: number = 32;
  static readonly RADIUS_MD: number = 20;
  static readonly SHADOW_VAL: string = '#15000000';
}

// ============================================================================
// --- 2. 样式扩展声明 (样式大礼包) ---
// ============================================================================

@Extend(Text)
function MainTitleStyle() {
  .fontSize(30)
  .fontWeight(FontWeight.Bolder)
  .fontColor(ThemeConfig.TEXT_MAIN)
  .letterSpacing(1.5)
}

@Extend(Text)
function SubTitleStyle() {
  .fontSize(14)
  .fontColor(ThemeConfig.TEXT_SUB)
  .lineHeight(20)
}

@Extend(TextInput)
function StandardInputStyle() {
  .width('100%')
  .height(55)
  .backgroundColor('#EDF2F7')
  .borderRadius(ThemeConfig.RADIUS_MD)
  .padding({
    left: 20,
    right: 20,
    top: 0,
    bottom: 0
  })
}

// ============================================================================
// --- 3. 详情卡片组件 (Detail Dialog - 重点升级区) ---
// ============================================================================

@CustomDialog
struct PetDetailDialog {
  /** 弹窗控制引擎 */
  controller: CustomDialogController = new CustomDialogController({ builder: '' })
  /** 核心宠物数据模型 */
  @Prop pet: Pet
  // --- 状态跟踪变量 ---
  @State currentSwiperIndex: number = 0
  @State historyRecords: PetHistoryRecord[] = []
  @State financeCosts: PetFinancialCost[] = []
  /** 长按触发的编辑回调逻辑 */
  onLongPressEdit: () => void = () => {
    console.info('[Detail] 默认长按逻辑被调用');
  }

  /**
   * 生命周期钩子：组件即将显示
   */
  async aboutToAppear() {
    console.info('[Lifecycle] 正在为宠物 ' + this.pet.name + ' 初始化详情页');

    try {
      // 适配变量解构约束
      const allData = await Promise.all([
        DatabaseManager.getInstance().getRecentRecordsByPetId(this.pet.id),
        DatabaseManager.getInstance().getRecentCosts(this.pet.id)// 确保此方法返回的是数组
      ]);

      // 手动索引赋值
      this.historyRecords = allData[0];
      this.financeCosts = allData[1];
    } catch (error) {
      console.error('Init Data Failed: ' + JSON.stringify(error));
    }
  }

  build() {
    Column() {
      // --- 顶部视觉大图固定层 ---
      Stack({ alignContent: Alignment.TopEnd }) {
        Image($r('app.media.icon'))
          .width('100%')
          .height(350)
          .objectFit(ImageFit.Cover)
          .borderRadius({
            topLeft: ThemeConfig.RADIUS_LG,
            topRight: ThemeConfig.RADIUS_LG
          })
          .shadow({ radius: 15, color: ThemeConfig.SHADOW_VAL, offsetY: 10 })

        // 浮动关闭按钮
        Circle({ width: 36, height: 36 })
          .fill('#50000000')
          .margin({ top: 20, right: 20 })
          .overlay(this.CloseIconContentBuilder(), {
            align: Alignment.Center
          })
          .onClick(() => {
            this.controller.close();
          })

        // 分页指示徽章
        this.PageIndexIndicator()
      }

      // --- 左右滑动核心 Swiper 容器 ---
      Swiper() {
        // 第 1 页：基础静态资料
        this.BaseInfoPageBuilder()

        // 第 2 页：动态日常记录
        this.DailyRecordPageBuilder()

        // 第 3 页：财务花销记录
        this.FinancialPageBuilder()
      }
      .index(this.currentSwiperIndex)
      .autoPlay(false)
      .loop(false)
      .indicator(false) // 使用自定义指示器
      .curve(Curve.EaseInOut)
      .onChange((index: number) => {
        this.currentSwiperIndex = index;
        console.info('[Swiper] 用户切换至第 ' + (index + 1) + ' 页');
      })
      .width('100%')
      .layoutWeight(1)
      .backgroundColor(Color.White)
      .borderRadius({
        bottomLeft: ThemeConfig.RADIUS_LG,
        bottomRight: ThemeConfig.RADIUS_LG
      })
    }
    .width('95%')
    .height('85%')
    .gesture(
      LongPressGesture({ repeat: false, duration: 800 })
        .onAction(() => {
          console.info('[Gesture] 详情页长按触发成功');
          this.controller.close();
          this.onLongPressEdit();
        })
    )
  }

  @Builder
  PageIndexIndicator() {
    Row() {
      Text(`${this.currentSwiperIndex + 1}`)
        .fontSize(14)
        .fontWeight(FontWeight.Bold)
        .fontColor(Color.White)
      Text(' / 3')
        .fontSize(12)
        .fontColor('#CCCCCC')
    }
    .padding({
      left: 12,
      right: 12,
      top: 6,
      bottom: 6
    })
    .backgroundColor('#A0000000')
    .borderRadius(15)
    .margin({ top: 20, right: 75 })
  }

  // --- 详情页内的 Builder 组件群  ---

  @Builder
  BaseInfoPageBuilder() {
    Column() {
      // 头部名称行
      Row() {
        Text(this.pet.name)
          .fontSize(28)
          .fontWeight(FontWeight.Bold)
          .fontColor(ThemeConfig.TEXT_MAIN)
        Text(this.pet.gender === '公' ? ' ♂' : ' ♀')
          .fontSize(22)
          .fontColor(this.pet.gender === '公' ? '#3182CE' : '#E53E3E')
          .margin({ left: 15 })
      }.width('100%').margin({ bottom: 25 })

      // 详细字段展开
      this.InfoItemRow('当前品种', this.pet.species)
      this.InfoItemRow('出生日期', this.pet.birthday)
      this.InfoItemRow('统计岁数', `${this.pet.age} 岁`)
      this.InfoItemRow('系统 ID', `ID-${this.pet.id?.toString().slice(-8)}`)

      Blank()

      Text('⬅️ 左右滑动查看更多动态记录 ➡️')
        .fontSize(12)
        .fontColor(ThemeConfig.TEXT_SUB)
        .opacity(0.6)
        .margin({ bottom: 20 })
    }
    .padding(30)
    .width('100%')
    .height('100%')
  }

  /**
   * @description 渲染日常档案页面 (Swiper 第二页)
   * 业务逻辑：若记录过多，仅通过 UI 列表展示最近 5 条
   */
  @Builder
  DailyRecordPageBuilder(): void {
    Column() {
      // --- 页面标题区 ---
      Row() {
        Column() {
          Text('📔 最近动态')
            .fontSize(22)
            .fontWeight(FontWeight.Bold)
            .fontColor(ThemeConfig.TEXT_MAIN)

          Text(`展示最近的 ${this.historyRecords.length} 条轨迹`)
            .fontSize(12)
            .fontColor(ThemeConfig.TEXT_SUB)
            .margin({ top: 4 })
        }.alignItems(HorizontalAlign.Start)

        Blank()

        // 模拟添加按钮
        Circle({ width: 32, height: 32 })
          .fill('#F0F7FF')
          .overlay(this.BluePlusIconBuilder(), {
            align: Alignment.Center
          })
          .onClick(() => {
            promptAction.showToast({ message: '跳转至全量记录页...' });
          })
      }
      .width('100%')
      .margin({
        top: 0,
        right: 0,
        bottom: 25,
        left: 0
      })

      // --- 列表渲染区 ---
      if (this.historyRecords.length > 0) {
        List({ space: 15 }) {
          // 逻辑：即使数据源较多，ForEach 也只处理前 5 项 (双重防御)
          ForEach(this.historyRecords.slice(0, 5), (item: PetHistoryRecord) => {
            ListItem() {
              this.SingleRecordCardBuilder(item)
            }
          }, (item: PetHistoryRecord) => item.id.toString())

          // 列表底部占位，增加滑动空间感
          ListItem() {
            this.ListFooterGuideBuilder()
          }
        }
        .width('100%')
        .layoutWeight(1)
        .edgeEffect(EdgeEffect.Spring) // 增加回弹效果
      } else {
        // 空状态处理
        this.EmptyDataBuilder('暂无动态记录')
      }
    }
    .padding({
      top: 30,
      right: 30,
      bottom: 30,
      left: 30
    })
    .width('100%')
    .height('100%')
  }

  /**
   * @description 渲染财务花销统计页面 (Swiper 第三页)
   * 业务需求：若记录过多，在 UI 列表中仅保留最近 5 条
   * @returns void
   */
  @Builder
  FinancialPageBuilder(): void {
    Column() {
      // --- 头部：账单标题与总计统计区 ---
      Row() {
        Column() {
          Text('💰 账单统计')
            .fontSize(22)
            .fontWeight(FontWeight.Bold)
            .fontColor(ThemeConfig.TEXT_MAIN)

          Text(`仅展示最近 5 笔消费明细`)
            .fontSize(11)
            .fontColor(ThemeConfig.TEXT_SUB)
            .margin({ top: 4 })
        }.alignItems(HorizontalAlign.Start)

        Blank()

        // 动态计算总支出显示
        Text(`总支出: ￥${this.CalculateTotalFinance()}`)
          .fontSize(14)
          .fontColor(ThemeConfig.ERROR_COLOR)
          .fontWeight(FontWeight.Medium)
          .backgroundColor('#FFF5F5')
          .padding({
            left: 10,
            right: 10,
            top: 5,
            bottom: 5
          })
          .borderRadius(8)
      }
      .width('100%')
      .margin({
        top: 0,
        right: 0,
        bottom: 25,
        left: 0
      })

      // --- 核心列表容器 ---
      if (this.financeCosts.length > 0) {
        List({ space: 12 }) {
          // 逻辑双保险：通过 slice(0, 5) 强制截断数组
          ForEach(this.financeCosts.slice(0, 5), (item: PetFinancialCost) => {
            ListItem() {
              this.SingleFinanceItemBuilder(item)
            }
          }, (item: PetFinancialCost) => item.id.toString())

          // 列表尾部装饰
          ListItem() {
            this.FinanceListFooterBuilder()
          }
        }
        .width('100%')
        .layoutWeight(1)
        .edgeEffect(EdgeEffect.Spring)
      } else {
        // 调用统一的空状态构建器（之前已修复）
        this.EmptyDataBuilder('尚无相关账单记录')
      }
    }
    .padding({
      top: 30,
      right: 30,
      bottom: 30,
      left: 30
    })
    .width('100%')
    .height('100%')
  }

  @Builder
  InfoItemRow(label: string, value: string) {
    Column() {
      Row() {
        Text(label)
          .fontSize(14)
          .fontColor(ThemeConfig.TEXT_SUB)
        Blank()
        Text(value)
          .fontSize(16)
          .fontWeight(FontWeight.Medium)
          .fontColor(ThemeConfig.TEXT_MAIN)
      }.width('100%').height(50)

      Divider()
        .color('#F1F3F5')
        .strokeWidth(1)
    }
  }

  /**
   * @description 专用构建器：渲染详情页关闭按钮的内部图标
   * 用于解决直接传递组件对象导致的类型不匹配错误 (Argument of type 'TextAttribute'...)
   */
  @Builder
  private CloseIconContentBuilder() {
    Text('✕')
      .fontSize(16)
      .fontWeight(FontWeight.Bold)
      .fontColor(Color.White)
      .textAlign(TextAlign.Center)
      .lineHeight(16) // 显式设置行高增加行数并保证居中
  }

  /**
   * @description 缺省状态构建器
   * 当关联的日常记录或花销账单为空时，渲染此标准化的视觉占位图
   * 修复报错: Property 'EmptyDataBuilder' does not exist
   * @param tipMessage 界面显示的提示文字
   */
  @Builder
  private EmptyDataBuilder(tipMessage: string): void {
    Column() {
      // 1. 装饰性容器：使用 Stack 叠加图标背景
      Stack({ alignContent: Alignment.Center }) {
        // 背景圆环
        Circle({ width: 80, height: 80 })
          .fill('#F7FAFC')

        // 核心图标：使用默认资源或占位图
        Image($r('app.media.icon'))
          .width(40)
          .height(40)
          .opacity(0.15)
          .grayscale(1) // 灰度处理，更显高级
      }
      .margin({
        top: 40,
        bottom: 20
      })

      // 2. 主提示文本
      Text(tipMessage)
        .fontSize(15)
        .fontWeight(FontWeight.Medium)
        .fontColor(ThemeConfig.TEXT_SUB)
        .letterSpacing(1)

      // 3. 辅助引导文本
      Text('暂无相关历史档案更新')
        .fontSize(12)
        .fontColor('#CBD5E0')
        .margin({ top: 8 })
        .lineHeight(18)

      // 4. 占位填充：确保空状态在容器中居中且有足够存在感
      Blank()
        .height(20)
    }
    .width('100%')
    .height('100%')
    .justifyContent(FlexAlign.Center)
    .padding({
      top: 20,
      bottom: 50,
      left: 20,
      right: 20
    })
  }

  @Builder
  private BluePlusIconBuilder(): void {
    Text('+')
      .fontSize(18)
      .fontWeight(FontWeight.Medium)
      .fontColor(ThemeConfig.PRIMARY_COLOR)
      .textAlign(TextAlign.Center)
      .lineHeight(18)
      .width('100%')
  }

  /**
   * @description 单条记录卡片构建器
   */
  @Builder
  private SingleRecordCardBuilder(item: PetHistoryRecord): void {
    Column({ space: 10 }) {
      Row() {
        // 类型标签
        Text(item.type)
          .fontSize(11)
          .fontColor(Color.White)
          .fontWeight(FontWeight.Medium)
          .backgroundColor(ThemeConfig.SECONDARY_COLOR)
          .padding({
            left: 8,
            right: 8,
            top: 3,
            bottom: 3
          })
          .borderRadius(6)

        Blank()

        // 日期展示
        Text(item.date)
          .fontSize(12)
          .fontColor(ThemeConfig.TEXT_SUB)
      }
      .width('100%')

      // 内容正文
      Text(item.content)
        .fontSize(14)
        .fontColor(ThemeConfig.TEXT_MAIN)
        .lineHeight(22)
        .maxLines(3) // 限制行数以保证美观
        .textOverflow({ overflow: TextOverflow.Ellipsis })
    }
    .padding(20)
    .backgroundColor('#F8FAFC')
    .borderRadius(16)
    .width('100%')
    .shadow({ radius: 8, color: '#05000000', offsetY: 4 })
  }

  /**
   * @description 列表底部引导
   */
  @Builder
  private ListFooterGuideBuilder(): void {
    Row() {
      Divider().layoutWeight(1).color('#EDF2F7')
      Text(' 仅展示最近 5 条记录 ')
        .fontSize(11)
        .fontColor(ThemeConfig.TEXT_SUB)
        .opacity(0.5)
      Divider().layoutWeight(1).color('#EDF2F7')
    }
    .width('100%')
    .margin({ top: 10, bottom: 20 })
  }

  /**
   * @description 内部辅助：计算当前显示的账单总金额 (额外增加逻辑行数)
   */
  private CalculateTotalFinance(): string {
    let total: number = 0;
    // 仅统计前 5 条的总额
    this.financeCosts.slice(0, 5).forEach((item) => {
      total += item.amount;
    });
    return total.toFixed(2);
  }

  /**
   * @description 独立构建器：单笔财务明细卡片
   */
  @Builder
  private SingleFinanceItemBuilder(item: PetFinancialCost): void {
    Row() {
      // 左侧：类别与时间
      Column({ space: 6 }) {
        Text(item.category)
          .fontSize(16)
          .fontWeight(FontWeight.Bold)
          .fontColor(ThemeConfig.TEXT_MAIN)

        Text(item.date)
          .fontSize(12)
          .fontColor(ThemeConfig.TEXT_SUB)
      }
      .alignItems(HorizontalAlign.Start)

      Blank()

      // 右侧：金额显示
      Text(`-￥${item.amount.toFixed(2)}`)
        .fontSize(18)
        .fontWeight(FontWeight.Bolder)
        .fontColor('#D53F8C')
    }
    .padding({
      top: 18,
      bottom: 18,
      left: 20,
      right: 20
    })
    .backgroundColor('#FFF5F7')
    .borderRadius(18)
    .width('100%')
    .shadow({
      radius: 6,
      color: '#0A000000',
      offsetY: 3
    })
  }

  /**
   * @description 独立构建器：财务列表底部占位
   */
  @Builder
  private FinanceListFooterBuilder(): void {
    Column() {
      Divider()
        .color('#FDE2E4')
        .strokeWidth(1)
        .margin({ top: 10, bottom: 15 })

      Text('以上为近期财务摘要')
        .fontSize(10)
        .fontColor('#CCCCCC')
        .textAlign(TextAlign.Center)
        .width('100%')
    }
    .width('100%')
    .padding({ bottom: 20 })
  }
}

// ============================================================================
// --- 4. 录入/编辑表单组件 (Add/Edit Dialog) ---
// ============================================================================

@CustomDialog
export struct PetAddDialog {
  controller: CustomDialogController = new CustomDialogController({ builder: '' })
  @Prop editPet: Pet | null = null
  @State name: string = ''
  @State species: string = ''
  @State gender: string = GenderType.BOY
  @State birthday: string = '2024-01-01'
  @State photoPath: string | undefined = undefined;
  confirm: (name: string, species: string, gender: string, bday: string, photo: string, id?: number) => void = () => {
  }

  aboutToAppear() {
    if (this.editPet) {
      console.info('[Form] 加载既有数据至表单');
      this.name = this.editPet.name;
      this.species = this.editPet.species;
      this.gender = this.editPet.gender;
      this.birthday = this.editPet.birthday;
      this.photoPath = this.editPet.photoPath ?? '';
    }
  }

  build() {
    Scroll() {
      Column({ space: 20 }) {
        Text(this.editPet ? '更新档案信息' : '新成员登记')
          .MainTitleStyle()
          .fontSize(24)
          .margin({ top: 10, bottom: 5 })

        this.PhotoSelectionUI()

        this.LabelText('基本昵称')
        TextInput({ placeholder: '例如：小乐', text: this.name })
          .StandardInputStyle()
          .onChange(v => this.name = v)

        this.LabelText('宠物品种')
        TextInput({ placeholder: '例如：拉布拉多', text: this.species })
          .StandardInputStyle()
          .onChange(v => this.species = v)

        Row({ space: 15 }) {
          Column() {
            this.LabelText('性别')
            Select([{ value: '公' }, { value: '母' }])
              .value(this.gender)
              .onSelect((i, v) => this.gender = v)
              .width('100%')
              .height(55)
              .backgroundColor('#EDF2F7')
              .borderRadius(16)
          }.layoutWeight(1)

          Column() {
            this.LabelText('出生日期')
            Button(this.birthday)
              .width('100%')
              .height(55)
              .backgroundColor('#EDF2F7')
              .fontColor(ThemeConfig.TEXT_MAIN)
              .borderRadius(16)
              .onClick(() => {
                DatePickerDialog.show({
                  start: new Date('2000-01-01'),
                  end: new Date(),
                  selected: new Date(this.birthday),
                  onAccept: (v: DatePickerResult) => {
                    this.birthday = `${v.year}-${v.month! + 1}-${v.day}`;
                  }
                })
              })
          }.layoutWeight(1.2)
        }.width('100%')

        this.SubmitButtonsUI()
      }
      .padding(30)
    }
    .backgroundColor(Color.White)
    .borderRadius(ThemeConfig.RADIUS_LG)
    .width('95%')
  }

  @Builder
  PhotoSelectionUI() {
    // 1. 显式设置 Stack 宽高，确保 alignContent 生效
    Stack({ alignContent: Alignment.BottomEnd }) {

      // 2. 根据 photoPath 是否有有效值来决定显示什么
      if (this.photoPath && this.photoPath !== '') {
        Image(this.photoPath)
          .width(120)
          .height(120)
          .borderRadius(60)
          .objectFit(ImageFit.Cover)
      } else {
        // 默认头像状态
        Image($r('app.media.icon'))
          .width(120)
          .height(120)
          .borderRadius(60)
          .opacity(0.6)
          .backgroundColor('#F0F0F0') // 增加背景色，防止透明图标看不见
      }

      // 3. 右下角的小图标
      // 确保它在 Image 之后，这样它就在最上层
      Circle({ width: 36, height: 36 })
        .fill(ThemeConfig.PRIMARY_COLOR)
        .margin({ right: 4, bottom: 4 }) // 稍微偏移一下，别贴死边
        .overlay(this.UploadPlusIconBuilder(), {
          align: Alignment.Center
        })
    }
    .width(120) // 约束容器大小
    .height(120)
    .onClick(() => {
      console.info('[Interaction] 点击恢复默认图标');
      this.photoPath = ''; // 赋值为空字符串，绕过数据库 ResourceStr 报错
      promptAction.showToast({ message: '已设置为默认头像' });
    })
    .gesture(
      LongPressGesture({ repeat: false, duration: 600 })
        .onAction(() => {
          console.info('[Debug] 恢复默认头像');
          promptAction.showToast({ message: '已设置默认头像' });
          // 设为空字符串，这样上面的 if 就会走 else 分支显示 $r('app.media.icon')
          this.photoPath = '';
        })
    )
  }

  @Builder
  SubmitButtonsUI() {
    Row({ space: 15 }) {
      Button('算了吧')
        .layoutWeight(1)
        .height(55)
        .backgroundColor('#F7FAFC')
        .fontColor(ThemeConfig.TEXT_SUB)
        .borderRadius(16)
        .onClick(() => this.controller.close())
      Button('确认保存')
        .layoutWeight(1)
        .height(55)
        .backgroundColor(ThemeConfig.PRIMARY_COLOR)
        .borderRadius(16)
        .onClick(() => {
          if (!this.name) {
            return promptAction.showToast({ message: '请输入宠物名称' });
          } // 仅校验名称
          // 头像路径为空时，传递空字符串（后续渲染会自动显示默认头像）
          this.confirm(this.name, this.species, this.gender, this.birthday, this.photoPath || '', this.editPet?.id);
          this.controller.close();
        })
    }
    .margin({ top: 15 })
  }

  @Builder
  LabelText(txt: string) {
    Text(txt)
      .fontSize(13)
      .fontColor(ThemeConfig.TEXT_SUB)
      .width('100%')
      .margin({ left: 4 })
  }

  /**
   * @description 专属构建器：用于渲染上传区域的加号图标
   * 修复报错：Argument of type 'TextAttribute' is not assignable...
   */
  @Builder
  private UploadPlusIconBuilder() {
    Text('+')
      .fontSize(26) // 稍微加大一点，视觉更协调
      .fontWeight(FontWeight.Medium)
      .fontColor(Color.White)
      .textAlign(TextAlign.Center)
      .width('100%')
      .height('100%')
  }
}

// ============================================================================
// --- 5. 主页面逻辑 (Main List Page) ---
// ============================================================================

@Entry
@Component
export struct PetListPage {
  @State pets: Pet[] = [];
  dialogController: CustomDialogController | null = null;

  async aboutToAppear() {
    await this.refreshDataList();
  }

  async refreshDataList() {
    const db = DatabaseManager.getInstance();
    let resultSet = await db.queryAllPets();
    let temp: Pet[] = [];

    // 1. 安全解析数据库数据
    if (resultSet !== undefined) {
      try {
        if (resultSet.rowCount > 0) {
          while (resultSet.goToNextRow()) {
            const bday = resultSet.getString(resultSet.getColumnIndex('birthday'));
            temp.push({
              id: resultSet.getLong(resultSet.getColumnIndex('id')),
              name: resultSet.getString(resultSet.getColumnIndex('name')),
              species: resultSet.getString(resultSet.getColumnIndex('species')),
              gender: resultSet.getString(resultSet.getColumnIndex('gender')),
              birthday: bday,
              photoPath: resultSet.getString(resultSet.getColumnIndex('photoPath')),
              age: new Date().getFullYear() - new Date(bday).getFullYear()
            });
          }
        }
      } catch (err) {
        console.error('解析宠物列表失败: ' + JSON.stringify(err));
      } finally {
        // 确保资源释放
        resultSet.close();
      }
    }

    // 3. 更新 UI 状态
    this.pets = temp;
  }

  /**
   * @description 唤起宠物高保真详情卡片
   * 修复报错: Function return type inference is limited (显式声明 : void)
   * @param item 当前选中的宠物数据对象
   * @returns void
   */
  public invokeDetailCard(item: Pet): void {
    console.info('[Interaction] 准备执行：唤起详情沉浸式卡片');

    // 1. 防御性编程：检查传入对象是否合法
    if (item === null || item === undefined) {
      console.error('[Error] 尝试唤起详情页时传入了无效的 Pet 对象');
      return;
    }

    // 2. 实例化弹窗控制器，显式展开所有配置属性
    this.dialogController = new CustomDialogController({
      // 显式指定 builder 闭包的返回类型
      builder: PetDetailDialog({
        pet: item,
        onLongPressEdit: (): void => {
          console.info('[Callback] 详情页长按反馈，正在重定向至编辑表单');
          this.invokeEditForm(item);
        }
      }),
      // 自动取消配置：点击遮罩层关闭弹窗
      autoCancel: true,
      // 自定义样式开关：开启后可全权控制弹窗 UI 边界
      customStyle: true,
      // 弹窗在屏幕中的对齐位置
      alignment: DialogAlignment.Center,
      // 弹窗相对于对齐位置的偏移量
      offset: { dx: 0, dy: 0 },
      // 遮罩层颜色配置
      maskColor: '#33000000'
    });

    // 3. 执行弹窗开启指令
    this.dialogController.open();
    console.info('[System] 详情卡片已成功渲染');
  }

  /**
   * @description 唤起宠物录入或修改的交互表单
   * 修复报错: Function return type inference is limited (显式声明 : void)
   * @param target 待编辑的宠物对象，若不传则视为新增
   * @returns void
   */
  public invokeEditForm(target?: Pet): void {
    console.info('[Interaction] 准备执行：唤起档案录入表单');

    this.dialogController = new CustomDialogController({
      builder: PetAddDialog({
        editPet: target,
        // 显式声明回调函数返回类型为异步 Promise<void>
        confirm: async (name: string, spec: string, gen: string, bday: string, pic: string,
          id?: number): Promise<void> => {
          console.info('[Transaction] 收到表单保存指令，准备持久化数据...');
          const mgr = DatabaseManager.getInstance();

          try {
            if (id) {
              await mgr.updatePet(id, name, spec, bday, gen, pic);
              promptAction.showToast({ message: '档案已成功同步至本地库' });
            } else {
              await mgr.insertPet(name, spec, bday, gen, pic);
              promptAction.showToast({ message: '新成员已成功录入档案库' });
            }
            AppStorage.setOrCreate('petDataChangeSignal', Date.now());
            // 刷新主页列表数据流
            await this.refreshDataList();
          } catch (error) {
            console.error('[Database] 持久化过程发生异常: ' + JSON.stringify(error));
          }
        }
      }),
      autoCancel: true,
      customStyle: true,
      alignment: DialogAlignment.Center,
      offset: { dx: 0, dy: 0 },
      maskColor: '#33000000'
    });

    this.dialogController.open();
  }

  build() {
    Stack({ alignContent: Alignment.BottomEnd }) {
      Column() {
        // 头部
        Column() {
          Text('宠物基地')
            .MainTitleStyle()
            .margin({ top: 60, bottom: 8 })
          Text(`管理核心 · 已录入 ${this.pets.length} 份档案`).SubTitleStyle()
        }
        .width('100%')
        .alignItems(HorizontalAlign.Start)
        .padding({ left: 25, right: 25, bottom: 30 })

        // 列表
        List({ space: 16 }) {
          ForEach(this.pets, (item: Pet) => {
            ListItem() {
              this.PetCardBuilder(item)
            }
            .onClick(() => this.invokeDetailCard(item))
            .swipeAction({ end: this.DeleteActionBuilder(item) })
          }, (item: Pet) => item.id.toString())
        }
        .padding({ left: 20, right: 20, bottom: 120 })
        .layoutWeight(1)
      }
      .width('100%')
      .height('100%')
      .backgroundColor(ThemeConfig.PAGE_BG)

      // 悬浮按钮
      this.FloatingBtnUI()
    }
  }

  @Builder
  PetCardBuilder(item: Pet) {
    Row() {
      Image($r('app.media.icon'))
        .width(85)
        .height(85)
        .borderRadius(18)
        .margin(15)
        .objectFit(ImageFit.Cover)
      Column({ space: 6 }) {
        Row() {
          Text(item.name)
            .fontSize(20)
            .fontWeight(FontWeight.Bold)
          Text(item.gender === '公' ? ' ♂' : ' ♀')
            .fontColor(item.gender === '公' ? '#3182CE' : '#E53E3E')
            .margin({ left: 8 })
        }

        Text(item.species)
          .fontSize(14)
          .fontColor(ThemeConfig.TEXT_SUB)
          .backgroundColor('#EDF2F7')
          .padding({
            left: 8,
            right: 8,
            top: 2,
            bottom: 2
          })
          .borderRadius(6)
      }.alignItems(HorizontalAlign.Start)
      .layoutWeight(1)
    }
    .width('100%')
    .backgroundColor(Color.White)
    .borderRadius(24)
    .shadow({ radius: 10, color: '#08000000', offsetY: 4 })
  }

  @Builder
  DeleteActionBuilder(item: Pet) {
    Button() {
      Image($r('app.media.delete_list')).width(24).fillColor(Color.White)
    }
    .width(70)
    .height(85)
    .backgroundColor(ThemeConfig.ERROR_COLOR)
    .borderRadius(20)
    .margin({ left: 15 })
    .onClick(async () => {
      await DatabaseManager.getInstance().deletePet(item.id!);
      await this.refreshDataList();
    })
  }

  @Builder
  FloatingBtnUI() {
    Button('+ 新增')
      .width(120)
      .height(60)
      .borderRadius(30)
      .backgroundColor(ThemeConfig.PRIMARY_COLOR)
      .shadow({ radius: 20, color: '#40007DFF', offsetY: 10 })
      .margin({ right: 25, bottom: 40 })
      .onClick(() => this.invokeEditForm())
  }
}