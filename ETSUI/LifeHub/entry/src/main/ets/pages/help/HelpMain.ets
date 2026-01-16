/**
 * 帮助和教程页面
 */
import { router } from '@kit.ArkUI';

interface HelpItem {
  title: string;
  content: string;
  icon: string;
  category: string;
}

@Entry
@Component
export struct HelpMain {
  @State helpItems: HelpItem[] = [
    {
      title: '快速开始',
      content: 'LifeHub是一个综合性的个人生活管理应用。首次使用建议先添加一些基础数据，如健康记录、目标等。',
      icon: '🚀',
      category: '入门'
    },
    {
      title: '健康记录',
      content: '在健康模块中，你可以记录体重、血压、血糖、运动时长和睡眠时间。建议每天记录，以便追踪健康趋势。',
      icon: '💚',
      category: '健康'
    },
    {
      title: '目标管理',
      content: '创建目标时，建议将大目标分解为小任务，并设置合理的截止日期。定期更新进度，保持动力。',
      icon: '🎯',
      category: '目标'
    },
    {
      title: '心情记录',
      content: '每天记录心情可以帮助你了解情绪变化。建议在固定时间记录，如每天晚上。',
      icon: '😊',
      category: '心情'
    },
    {
      title: '技能学习',
      content: '添加技能后，可以追踪学习进度。建议设置目标等级，并定期更新当前等级。',
      icon: '📚',
      category: '学习'
    },
    {
      title: '旅行规划',
      content: '创建旅行计划时，可以设置预算和实际花费，帮助控制旅行开支。',
      icon: '✈️',
      category: '旅行'
    },
    {
      title: '日记功能',
      content: '写日记时可以记录心情、添加标签。建议定期回顾日记，了解生活变化。',
      icon: '📝',
      category: '日记'
    },
    {
      title: '数据统计',
      content: '在统计页面可以查看各模块的详细数据和分析。帮助了解生活趋势和模式。',
      icon: '📊',
      category: '统计'
    },
    {
      title: '统一搜索',
      content: '使用搜索功能可以快速找到所有模块的数据。支持关键词搜索标题、内容等。',
      icon: '🔍',
      category: '搜索'
    },
    {
      title: '成就系统',
      content: '完成各种任务可以解锁成就徽章。查看成就系统了解如何解锁更多成就。',
      icon: '🏆',
      category: '成就'
    },
    {
      title: '数据备份',
      content: '定期备份数据可以防止数据丢失。建议每周备份一次，保存到安全的地方。',
      icon: '📦',
      category: '备份'
    },
    {
      title: '提醒功能',
      content: '设置提醒可以帮助你养成记录习惯。可以设置每天、工作日或周末提醒。',
      icon: '⏰',
      category: '提醒'
    },
    {
      title: '标签管理',
      content: '使用标签可以更好地分类和管理数据。在记录时添加标签，方便后续查找。',
      icon: '🏷️',
      category: '标签'
    },
    {
      title: '生活报告',
      content: '生成周报或月报可以了解一段时间内的生活情况。报告包含各模块的统计和分析。',
      icon: '📄',
      category: '报告'
    }
  ];

  @State selectedCategory: string = '全部';
  @State expandedItems: Set<number> = new Set();

  private categories = ['全部', '入门', '健康', '目标', '心情', '学习', '旅行', '日记', '统计', '搜索', '成就', '备份', '提醒', '标签', '报告'];

  build() {
    Column() {
      // 标题栏
      Row() {
        Text('帮助中心')
          .fontSize(24)
          .fontWeight(FontWeight.Bold)
        
        Blank()
      }
      .width('100%')
      .padding(20)
      .backgroundColor('#f0f0f0')

      // 分类筛选
      Scroll() {
        Row() {
          ForEach(this.categories, (category: string) => {
            Button(category)
              .fontSize(14)
              .height(36)
              .backgroundColor(this.selectedCategory === category ? '#007DFF' : '#f0f0f0')
              .fontColor(this.selectedCategory === category ? Color.White : '#666')
              .borderRadius(18)
              .margin({ right: 10 })
              .onClick(() => {
                this.selectedCategory = category;
              })
          })
        }
        .width('100%')
        .padding({ left: 15, right: 15, top: 10, bottom: 10 })
      }
      .scrollBar(BarState.Auto)
      .scrollable(ScrollDirection.Horizontal)

      // 帮助列表
      Scroll() {
        Column() {
          ForEach(this.getFilteredItems(), (item: HelpItem, index: number) => {
            this.buildHelpItem(item, index)
          })
        }
        .width('100%')
        .padding(15)
      }
      .layoutWeight(1)
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#f5f5f5')
  }

  @Builder
  buildHelpItem(item: HelpItem, index: number) {
    Column() {
      Row() {
        Text(item.icon)
          .fontSize(32)
          .width(50)
        
        Column() {
          Text(item.title)
            .fontSize(18)
            .fontWeight(FontWeight.Bold)
            .fontColor('#333')
            .textAlign(TextAlign.Start)
            .width('100%')

          Row() {
            Text(item.category)
              .fontSize(12)
              .fontColor('#666')
              .padding({ left: 8, right: 8, top: 4, bottom: 4 })
              .backgroundColor('#f0f0f0')
              .borderRadius(10)
          }
          .width('100%')
          .margin({ top: 5 })
        }
        .layoutWeight(1)
        .alignItems(HorizontalAlign.Start)
        .padding({ left: 15 })

        Text(this.expandedItems.has(index) ? '▲' : '▼')
          .fontSize(16)
          .fontColor('#999')
      }
      .width('100%')
      .onClick(() => {
        if (this.expandedItems.has(index)) {
          this.expandedItems.delete(index);
        } else {
          this.expandedItems.add(index);
        }
      })

      if (this.expandedItems.has(index)) {
        Text(item.content)
          .fontSize(14)
          .fontColor('#666')
          .lineHeight(24)
          .textAlign(TextAlign.Start)
          .width('100%')
          .margin({ top: 15, left: 65 })
      }
    }
    .width('100%')
    .padding(15)
    .backgroundColor(Color.White)
    .borderRadius(12)
    .margin({ bottom: 10 })
  }

  getFilteredItems(): HelpItem[] {
    if (this.selectedCategory === '全部') {
      return this.helpItems;
    }
    return this.helpItems.filter(item => item.category === this.selectedCategory);
  }
}
