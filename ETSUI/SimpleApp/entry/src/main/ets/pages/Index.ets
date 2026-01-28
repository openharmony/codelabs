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
import { common } from '@kit.AbilityKit';
import { BusinessError } from '@kit.BasicServicesKit';
import { SystemAppLauncher } from '../utils/SystemAppLauncher';
import { HistoryManager, AppUsageRecord } from '../utils/HistoryManager';

class FeatureItem {
  icon: string = '';
  imagePath?: Resource = undefined;
  title: string = '';
  color: string = '';
  
  constructor(icon: string, title: string, color: string, imagePath?: Resource) {
    this.icon = icon;
    this.title = title;
    this.color = color;
    this.imagePath = imagePath;
  }
}

// 颜色工具函数
function darkenColor(color: string, factor: number): string {
  // 简单的颜色变暗实现（针对十六进制颜色）
  if (color.startsWith('#')) {
    const hex = color.slice(1);
    const num = parseInt(hex, 16);
    const amt = Math.round(2.55 * factor * 100);
    const R = (num >> 16) - amt;
    const G = (num >> 8 & 0x00FF) - amt;
    const B = (num & 0x0000FF) - amt;
    return "#" + (0x1000000 + (R < 255 ? R < 1 ? 0 : R : 255) * 0x10000 +
      (G < 255 ? G < 1 ? 0 : G : 255) * 0x100 +
      (B < 255 ? B < 1 ? 0 : B : 255)).toString(16).slice(1);
  }
  return color;
}

// 历史记录弹窗组件
@CustomDialog
struct HistoryDialog {
  controller: CustomDialogController;
  @State historyList: AppUsageRecord[] = [];

  aboutToAppear() {
    this.loadHistoryRecords();
  }

  private async loadHistoryRecords() {
    try {
      const context = getContext(this) as common.UIAbilityContext;
      const records = await HistoryManager.getRecords(context);
      this.historyList = records;
    } catch (error) {
      console.error('加载历史记录失败:', error);
    }
  }

  // 处理历史记录项点击
  private async handleHistoryItemClick(item: AppUsageRecord): Promise<void> {
    try {
      const context = getContext(this) as common.UIAbilityContext;
      
      // 根据应用名称启动相应应用
      switch (item.appName) {
        case '通讯':
          SystemAppLauncher.launchContacts(context);
          break;
        case '相机':
          SystemAppLauncher.launchCamera(context);
          break;
        case '备忘录':
          SystemAppLauncher.launchMemo(context);
          break;
        case '照片':
          SystemAppLauncher.launchGallery(context);
          break;
        case '设置':
          SystemAppLauncher.launchSettings(context);
          break;
        default:
          console.log('未知应用:', item.appName);
      }
      
      // 关闭弹窗
      this.controller.close();
      console.log('启动应用:', item.appName);
      
    } catch (error) {
      console.error('启动应用失败:', error);
    }
  }

  build() {
    Column() {
      // 标题栏
      Row() {
        Text('最近使用')
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .layoutWeight(1)
        
        Button() {
          Text('×')
            .fontSize(20)
            .fontColor('#666666')
        }
        .width(32)
        .height(32)
        .backgroundColor(Color.Transparent)
        .onClick(() => {
          this.controller.close();
        })
      }
      .width('100%')
      .padding({ left: 16, right: 16, top: 16, bottom: 12 })

      // 历史记录列表
      if (this.historyList.length > 0) {
        Column() {
          ForEach(this.historyList, (item: AppUsageRecord, index: number) => {
            Row() {
              // 应用图标 - 根据应用类型显示不同图标
              Circle({ width: 36, height: 36 })
                .fill(this.getAppColor(item.appName))
                .margin({ right: 12 })
                
              Column({ space: 4 }) {
                Text(item.appName)
                  .fontSize(16)
                  .fontWeight(FontWeight.Medium)
                  .textAlign(TextAlign.Start)
                Text(HistoryManager.formatTime(item.timestamp))
                  .fontSize(12)
                  .fontColor('#666666')
                  .textAlign(TextAlign.Start)
              }
              .layoutWeight(1)
              
              // 右侧箭头指示
              Text('→')
                .fontSize(16)
                .fontColor('#999999')
            }
            .width('100%')
            .height(56)
            .padding({ left: 16, right: 16 })
            .backgroundColor(index % 2 === 0 ? '#F8F8F8' : Color.White)
            .borderRadius(8)
            .stateStyles({
              pressed: {
                .backgroundColor('#E8E8E8')
                .scale({ x: 0.98, y: 0.98 })
              }
            })
            .onClick(() => {
              this.handleHistoryItemClick(item);
            })
          })
        }
        .width('100%')
        .height(300)
        .margin({ bottom: 16 })
      } else {
        Column() {
          Text('暂无使用记录')
            .fontSize(14)
            .fontColor('#999999')
            .margin({ top: 40, bottom: 40 })
        }
        .width('100%')
        .alignItems(HorizontalAlign.Center)
      }

      // 底部操作按钮
      Row() {
        Button('清除记录')
        .layoutWeight(1)
        .height(40)
        .backgroundColor('#FF3B30')
        .fontColor(Color.White)
        .borderRadius(20)
        .margin({ right: 8 })
        .onClick(async () => {
          try {
            const context = getContext(this) as common.UIAbilityContext;
            await HistoryManager.clearRecords(context);
            this.historyList = [];
            console.log('历史记录已清除');
          } catch (error) {
            console.error('清除记录失败:', error);
          }
        })

        Button('关闭')
        .layoutWeight(1)
        .height(40)
        .backgroundColor('#007AFF')
        .fontColor(Color.White)
        .borderRadius(20)
        .margin({ left: 8 })
        .onClick(() => {
          this.controller.close();
        })
      }
      .width('100%')
      .padding({ left: 16, right: 16, bottom: 16 })
    }
    .width(340)
    .backgroundColor(Color.White)
    .borderRadius(16)
    .shadow({
      radius: 20,
      color: '#40000000',
      offsetX: 0,
      offsetY: 4
    })
  }

  // 根据应用名称获取颜色
  private getAppColor(appName: string): string {
    const colorMap: Record<string, string> = {
      '通讯': '#FF9500',
      '相机': '#007AFF',
      '备忘录': '#FFD700',
      '照片': '#FF2D55',
      '设置': '#8E8E93'
    };
    return colorMap[appName] || '#007AFF';
  }
}

@Entry
@Component
struct Index {
  // 状态变量
  @State userName: string = '用户';

  @State progressValue: number = 0.6;
  @State switchValue: boolean = true;
  @State selectedTab: number = 0;
  @State listData: string[] = ['项目1', '项目2', '项目3', '项目4', '项目5'];
  @State isExpanded: boolean = false;
  
  // 历史记录弹窗控制器
  historyDialogController: CustomDialogController = new CustomDialogController({
    builder: HistoryDialog({}),
    alignment: DialogAlignment.TopEnd,
    offset: { dx: -16, dy: 80 },
    customStyle: true
  });
  
  // 获取按下状态的颜色
  getPressedColor(color: string): string {
    // 简单的颜色映射
    const colorMap: Record<string, string> = {
      '#FF6B6B': '#D45A5A',
      '#4ECDC4': '#42A9A1',
      '#45B7D1': '#3A9AB4',
      '#FF9500': '#D67E00',
      '#34C759': '#2CA74B',
      '#5856D6': '#4A49B4'
    };
    return colorMap[color] || color;
  }

  // 处理功能卡片点击
  async handleFeatureClick(featureName: string): Promise<void> {
    const context = this.getUIContext()?.getHostContext() as common.UIAbilityContext;
    
    if (!context) {
      console.error('上下文获取失败');
      return;
    }
    
    // 记录使用历史
    const record: AppUsageRecord = {
      appName: featureName,
      timestamp: Date.now(),
      packageName: this.getPackageNameByFeature(featureName)
    };
    
    try {
      await HistoryManager.addRecord(context, record);
      console.log('使用记录已保存:', featureName);
    } catch (error) {
      console.error('保存使用记录失败:', error);
    }
    
    switch (featureName) {
      case '通讯':
        SystemAppLauncher.launchContacts(context);
        break;
      case '相机':
        SystemAppLauncher.launchCamera(context);
        break;
      case '备忘录':
        SystemAppLauncher.launchMemo(context);
        break;
      case '照片':
        // 启动图库应用
        SystemAppLauncher.launchGallery(context);
        break;
      case '设置':
        SystemAppLauncher.launchSettings(context);
        break;
      default:
        console.log('未知功能:', featureName);
    }
  }
  
  // 根据功能名称获取包名
  private getPackageNameByFeature(featureName: string): string {
    const packageMap: Record<string, string> = {
      '通讯': 'com.ohos.contacts',
      '相机': 'com.ohos.camera',
      '备忘录': 'com.ohos.note',
      '照片': 'com.huawei.hmos.gallery',
      '设置': 'com.huawei.hmos.settings'
    };
    return packageMap[featureName] || '';
  }
  

  
  // 显示最近使用记录
  showRecentUsage(): void {
    this.historyDialogController.open();
    console.log('显示最近使用记录');
  }
  
  // 构建函数
  build() {
    Scroll() {
      Column() {
        // 1. 顶部导航栏
        this.buildTopBar()
        
      
        
        // 3. 轮播图区域
        this.buildBannerSection()
        
        // 4. 功能卡片区域
        this.buildFeatureCards()
        
        // 5. 数据展示区域
        this.buildDataSection()
        
        // 6. 交互组件区域
        this.buildInteractiveSection()
        
        // 7. 列表展示区域
        this.buildListSection()
        
        // 8. 底部信息区域
        this.buildFooterSection()
      }
      .width('100%')
      .alignItems(HorizontalAlign.Start)
    }
    .scrollable(ScrollDirection.Vertical)
    .scrollBar(BarState.Auto)
    .backgroundColor('#F5F5F5')
  }
  
  // 顶部导航栏
  @Builder
  buildTopBar() {
    Row() {
      Column() {
        Text('欢迎回来')
          .fontSize(14)
          .fontColor('#666666')
        Text(this.userName)
          .fontSize(20)
          .fontWeight(FontWeight.Bold)
          .fontColor('#333333')
      }
      .layoutWeight(1)
      
      // 四叶草按钮 - 最近使用功能
      Button() {
        Text('🍀')
          .fontSize(20)
      }
      .width(40)
      .height(40)
      .backgroundColor('#FFFFFF')
      .borderRadius(20)
      .border({ width: 1, color: '#E0E0E0' })
      .margin({ left: 8 })
      .onClick(() => {
        this.showRecentUsage();
      })
    }
    .width('100%')
    .padding({ left: 16, right: 16, top: 12, bottom: 12 })
    .backgroundColor('#FFFFFF')
  }
  

  
  // 轮播图区域 - 水平滚动展示
  @Builder
  buildBannerSection() {
    Column() {
      Row() {
        Text('热门推荐')
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .layoutWeight(1)
        
        Text('查看更多')
          .fontSize(14)
          .fontColor('#007AFF')
          .onClick(() => {
            console.log('查看更多推荐');
          })
      }
      .width('100%')
      .padding({ left: 16, right: 16 })
      
      // 水平滚动容器
      Scroll() {
        Row({ space: 12 }) {
          // 增加更多推荐项，展示滚动效果
          this.buildBannerItem('新品上市', '#FF6B6B', '限时优惠，全场8折起')
          this.buildBannerItem('热门活动', '#4ECDC4', '参与有奖，赢取好礼')
          this.buildBannerItem('技术分享', '#45B7D1', '学习交流，共同进步')
          this.buildBannerItem('社区动态', '#FF9500', '最新资讯，不容错过')
          this.buildBannerItem('产品更新', '#34C759', '功能优化，体验升级')
          this.buildBannerItem('用户反馈', '#5856D6', '意见收集，改进产品')
        }
        .padding({ left: 16, right: 16, top: 8, bottom: 8 })
      }
      .scrollable(ScrollDirection.Horizontal)
      .scrollBar(BarState.Auto)
      .height(140)
      .width('100%')
    }
  }
  
  @Builder
  buildBannerItem(title: string, color: string, subtitle: string) {
    Column({ space: 8 }) {
      Text(title)
        .fontSize(16)
        .fontColor('#FFFFFF')
        .fontWeight(FontWeight.Bold)
        .textAlign(TextAlign.Start)
        .maxLines(1)
        .textOverflow({ overflow: TextOverflow.Ellipsis })
      
      Text(subtitle)
        .fontSize(12)
        .fontColor('#FFFFFF')
        .opacity(0.9)
        .textAlign(TextAlign.Start)
        .maxLines(2)
        .textOverflow({ overflow: TextOverflow.Ellipsis })
        
      // 添加指示箭头和更多信息
      Row() {
        Text('立即查看')
          .fontSize(12)
          .fontColor('#FFFFFF')
          .opacity(0.8)
        Text('→')
          .fontSize(14)
          .fontColor('#FFFFFF')
          .opacity(0.8)
          .margin({ left: 4 })
      }
      .width('100%')
      .justifyContent(FlexAlign.End)
      .margin({ top: 8 })
    }
    .width(280)
    .height(120)
    .backgroundColor(color)
    .borderRadius(16)
    .padding(16)
    .justifyContent(FlexAlign.SpaceBetween)
    // 添加阴影效果
    .shadow({
      radius: 8,
      color: '#40000000',
      offsetX: 2,
      offsetY: 2
    })
    // 添加点击效果
    .stateStyles({
      pressed: {
        .backgroundColor(this.getPressedColor(color))
        .scale({ x: 0.98, y: 0.98 })
      }
    })
    .onClick(() => {
      console.log('点击推荐项:', title);
    })
  }
  
  // 功能卡片区域
  @Builder
  buildFeatureCards() {
    Column() {
      Text('常用功能')
        .fontSize(18)
        .fontWeight(FontWeight.Bold)
        .alignSelf(ItemAlign.Start)
        .margin({ left: 16, bottom: 12 })
      
      // 网格布局
      Grid() {
        ForEach([
          new FeatureItem('📱', '通讯', '#FF9500'),
          new FeatureItem('📸', '相机', '#007AFF'),
          new FeatureItem('📝', '备忘录', '#FFD700'),
          new FeatureItem('📚', '照片', '#FF2D55', $r('app.media.ic_learning_image')),
          new FeatureItem('⚙️', '设置', '#8E8E93')
        ], (item: FeatureItem, index: number) => {
          GridItem() {
            Column() {
              if (item.imagePath) {
                // 使用图片显示 - 优化学习功能图标显示
                Image(item.imagePath)
                  .width(item.title === '照片' ? 36 : 32)  // 照片图标稍大
                  .height(item.title === '照片' ? 36 : 32)
                  .margin({ bottom: 8 })
                  .objectFit(ImageFit.Contain)
                  .interpolation(ImageInterpolation.High)  // 高质量插值
                  .renderMode(ImageRenderMode.Original)    // 保持原始质量
              } else {
                // 使用文本图标显示
                Text(item.icon)
                  .fontSize(24)
                  .margin({ bottom: 8 })
              }
              Text(item.title)
                .fontSize(12)
                .fontColor('#666666')
            }
            .width('100%')
            .height(80)
            .backgroundColor('#FFFFFF')
            .borderRadius(12)
            .justifyContent(FlexAlign.Center)
            .alignItems(HorizontalAlign.Center)
            .onClick(() => {
              this.handleFeatureClick(item.title);
            })
          }
        })
      }
      .columnsTemplate('1fr 1fr')
      .rowsTemplate('1fr 1fr 1fr')
      .columnsGap(16)
      .rowsGap(16)
      .width('100%')
      .height(280)
      .padding({ left: 16, right: 16 })
    }
    .margin({ top: 16 })
  }
  
  // 数据展示区域
  @Builder
  buildDataSection() {
    Column() {
      Text('数据统计')
        .fontSize(18)
        .fontWeight(FontWeight.Bold)
        .alignSelf(ItemAlign.Start)
        .margin({ left: 16, bottom: 12 })
      
      Row() {
        this.buildDataCard('今日步数', '8,542', '步', '#34C759')
        this.buildDataCard('心率', '72', 'bpm', '#FF3B30')
        this.buildDataCard('睡眠', '7.2', '小时', '#007AFF')
      }
      .width('100%')
      .height(100)
      .padding({ left: 16, right: 16 })
      .justifyContent(FlexAlign.SpaceBetween)
    }
    .margin({ top: 16 })
  }
  
  @Builder
  buildDataCard(title: string, value: string, unit: string, color: string) {
    Column() {
      Text(title)
        .fontSize(12)
        .fontColor('#666666')
      Row({ space: 2 }) {
        Text(value)
          .fontSize(20)
          .fontWeight(FontWeight.Bold)
          .fontColor(color)
        Text(unit)
          .fontSize(12)
          .fontColor('#666666')
          .alignSelf(ItemAlign.End)
          .margin({ bottom: 4 })
      }
      Progress({
        value: this.progressValue,
        total: 1.0,
        type: ProgressType.Linear
      })
        .width('100%')
        .height(4)
        .color(color)
    }
    .width('30%')
    .height('100%')
    .backgroundColor('#FFFFFF')
    .borderRadius(12)
    .padding(12)
    .justifyContent(FlexAlign.SpaceBetween)
  }
  
  // 交互组件区域
  @Builder
  buildInteractiveSection() {
    Column() {
      Text('交互组件')
        .fontSize(18)
        .fontWeight(FontWeight.Bold)
        .alignSelf(ItemAlign.Start)
        .margin({ left: 16, bottom: 12 })
      
      Column() {
        // 开关组件
        Row() {
          Text('通知开关')
            .fontSize(16)
            .layoutWeight(1)
          Toggle({ type: ToggleType.Switch, isOn: this.switchValue })
            .onChange((value: boolean) => {
              this.switchValue = value;
              console.log('开关状态:', value);
            })
        }
        .width('100%')
        .padding(16)
        .backgroundColor('#FFFFFF')
        .borderRadius(12)
        

        

      }
      .width('100%')
      .padding({ left: 16, right: 16 })
    }
    .margin({ top: 16 })
  }
  
  // 列表展示区域
  @Builder
  buildListSection() {
    Column() {
      Text('最新动态')
        .fontSize(18)
        .fontWeight(FontWeight.Bold)
        .alignSelf(ItemAlign.Start)
        .margin({ left: 16, bottom: 12 })
      
      Column() {
        ForEach(this.listData, (item: string, index: number) => {
          this.buildListItem(item, index)
        })
      }
      .width('100%')
      .padding({ left: 16, right: 16 })
    }
    .margin({ top: 16 })
  }
  
  @Builder
  buildListItem(title: string, index: number) {
    Row() {
      Circle({ width: 40, height: 40 })
        .fill('#007AFF')
        .margin({ right: 12 })
      
      Column({ space: 4 }) {
        Text(title)
          .fontSize(16)
          .fontWeight(FontWeight.Medium)
        Text('这是第' + (index + 1) + '个项目的描述信息')
          .fontSize(12)
          .fontColor('#666666')
          .maxLines(1)
          .textOverflow({ overflow: TextOverflow.Ellipsis })
      }
      .layoutWeight(1)
      
      Text('08:30')
        .fontSize(12)
        .fontColor('#999999')
    }
    .width('100%')
    .height(60)
    .padding(12)
    .backgroundColor('#FFFFFF')
    .borderRadius(8)
    .margin({ bottom: 8 })
    .onClick(() => {
      console.log('点击列表项:', title);
    })
  }
  
  // 底部信息区域
  @Builder
  buildFooterSection() {
    Column() {
      Divider()
        .strokeWidth(1)
        .color('#E0E0E0')
        .margin({ top: 24, bottom: 16 })
      
      Text('OpenHarmony 综合界面示例')
        .fontSize(14)
        .fontColor('#999999')
      
      Text('版本 1.0.0 • 2026年')
        .fontSize(12)
        .fontColor('#CCCCCC')
        .margin({ top: 4 })
    }
    .width('100%')
    .padding({ bottom: 24 })
    .alignItems(HorizontalAlign.Center)
  }
}