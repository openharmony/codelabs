import { promptAction } from '@kit.ArkUI';
import { dataManager, HistoryItem } from '../manager/DataManager';

// Builder函数，用于Navigation路由
@Builder
export function HistoryPageBuilder() {
  HistoryPage();
}

@Component
export struct HistoryPage {
  // 获取全局NavPathStack
  pathStack: NavPathStack = AppStorage.get('pathStack') as NavPathStack;
  
  // 历史记录列表
  @State historyList: HistoryItem[] = [];
  @State groupedHistory: Map<string, HistoryItem[]> = new Map();
  @State searchKeyword: string = '';
  @State isEditing: boolean = false;
  @State selectedItems: Set<number> = new Set();
  
  // 深色模式
  @StorageLink('darkMode') darkMode: boolean = false;

  aboutToAppear() {
    console.info('=== HistoryPage aboutToAppear ===');
    this.loadHistory();
  }

  // 加载历史记录
  loadHistory() {
    console.info('=== HistoryPage loadHistory 开始 ===');
    if (this.searchKeyword.trim()) {
      this.historyList = dataManager.searchHistory(this.searchKeyword);
      // 搜索时不分组
      this.groupedHistory = new Map([['搜索结果', this.historyList]]);
      console.info(`搜索结果: ${this.historyList.length}条`);
    } else {
      this.historyList = dataManager.getHistoryList();
      this.groupedHistory = dataManager.getHistoryByDate();
      console.info(`加载历史记录: ${this.historyList.length}条`);
      console.info(`分组数量: ${this.groupedHistory.size}`);
    }
  }

  // 格式化时间
  formatTime(timestamp: number): string {
    const date = new Date(timestamp);
    const hours = date.getHours().toString().padStart(2, '0');
    const minutes = date.getMinutes().toString().padStart(2, '0');
    return `${hours}:${minutes}`;
  }

  // 格式化日期
  formatDate(timestamp: number): string {
    const date = new Date(timestamp);
    return `${date.getMonth() + 1}月${date.getDate()}日`;
  }

  build() {
    NavDestination() {
      Column() {
        // 自定义标题栏
        Row() {
          // 返回按钮
          Image($r('app.media.back'))
            .width(24)
            .height(24)
            .fillColor(this.darkMode ? '#FFFFFF' : '#000000')
            .margin({ left: 16 })
            .onClick(() => {
              this.pathStack.pop();
            })
          
          Text('历史记录')
            .fontSize(18)
            .fontColor(this.darkMode ? '#FFFFFF' : '#000000')
            .fontWeight(FontWeight.Medium)
            .margin({ left: 16 })
          
          Blank()
          
          // 编辑/完成按钮
          Button(this.isEditing ? '完成' : '编辑')
            .fontSize(14)
            .backgroundColor(Color.Transparent)
            .fontColor('#007AFF')
            .margin({ right: 16 })
            .onClick(() => {
              this.isEditing = !this.isEditing;
              if (!this.isEditing) {
                this.selectedItems.clear();
              }
            })
        }
        .width('100%')
        .height(56)
        .backgroundColor(this.darkMode ? '#1C1C1E' : '#FFFFFF')
        .alignItems(VerticalAlign.Center)
        
        // 搜索框
        Row() {
          Image($r('app.media.search'))
            .width(16)
            .height(16)
            .fillColor('#8E8E93')
            .margin({ left: 8 })
          
          TextInput({ placeholder: '搜索历史记录', text: this.searchKeyword })
            .layoutWeight(1)
            .height(36)
            .fontSize(14)
            .backgroundColor(Color.Transparent)
            .placeholderColor('#8E8E93')
            .fontColor(this.darkMode ? '#FFFFFF' : '#000000')
            .onChange((value: string) => {
              this.searchKeyword = value;
              this.loadHistory();
            })
          
          if (this.searchKeyword.length > 0) {
            Image($r('app.media.close'))
              .width(16)
              .height(16)
              .fillColor('#8E8E93')
              .margin({ right: 8 })
              .onClick(() => {
                this.searchKeyword = '';
                this.loadHistory();
              })
          }
        }
        .width('100%')
        .height(36)
        .margin({ left: 16, right: 16, top: 8, bottom: 8 })
        .padding({ left: 4, right: 4 })
        .backgroundColor(this.darkMode ? '#2C2C2E' : '#F2F2F7')
        .borderRadius(8)
        
        // 批量操作栏（编辑模式下显示）
        if (this.isEditing && this.selectedItems.size > 0) {
          Row() {
            Button('删除选中 (' + this.selectedItems.size + ')')
              .fontSize(14)
              .backgroundColor('#FF3B30')
              .fontColor('#FFFFFF')
              .borderRadius(8)
              .onClick(() => {
                this.deleteSelectedItems();
              })
            
            Blank()
            
            Button('全选')
              .fontSize(14)
              .backgroundColor(Color.Transparent)
              .fontColor('#007AFF')
              .onClick(() => {
                this.selectAllItems();
              })
          }
          .width('100%')
          .height(44)
          .padding({ left: 16, right: 16 })
          .backgroundColor(this.darkMode ? '#2C2C2E' : '#F2F2F7')
        }
        
        // 历史记录列表
        if (this.historyList.length === 0) {
          // 空状态
          Column() {
            Image($r('app.media.history'))
              .width(64)
              .height(64)
              .fillColor('#8E8E93')
              .opacity(0.5)
            
            Text(this.searchKeyword ? '未找到匹配的历史记录' : '暂无浏览历史')
              .fontSize(16)
              .fontColor('#8E8E93')
              .margin({ top: 16 })
            
            Text('您访问的网页将显示在这里')
              .fontSize(14)
              .fontColor('#8E8E93')
              .margin({ top: 8 })
              .visibility(this.searchKeyword ? Visibility.None : Visibility.Visible)
          }
          .width('100%')
          .layoutWeight(1)
          .justifyContent(FlexAlign.Center)
        } else {
          List() {
            // 按日期分组显示
            ForEach(Array.from(this.groupedHistory.keys()), (dateLabel: string) => {
              ListItemGroup({ header: this.dateHeader(dateLabel) }) {
                ForEach(this.groupedHistory.get(dateLabel) || [], (item: HistoryItem) => {
                  ListItem() {
                    Row() {
                      // 编辑模式下显示选择框
                      if (this.isEditing) {
                        Checkbox()
                          .select(this.selectedItems.has(item.id))
                          .selectedColor('#007AFF')
                          .margin({ right: 12 })
                          .onChange((value: boolean) => {
                            if (value) {
                              this.selectedItems.add(item.id);
                            } else {
                              this.selectedItems.delete(item.id);
                            }
                            // 触发刷新
                            this.selectedItems = new Set(this.selectedItems);
                          })
                      }
                      
                      // 网站图标
                      Column() {
                        Image($r('app.media.web'))
                          .width(20)
                          .height(20)
                          .fillColor('#FFFFFF')
                      }
                      .width(40)
                      .height(40)
                      .borderRadius(8)
                      .backgroundColor(this.getIconColor(item.url))
                      .justifyContent(FlexAlign.Center)
                      .margin({ right: 12 })
                      
                      // 标题和URL
                      Column({ space: 4 }) {
                        Text(item.title || '无标题')
                          .fontSize(16)
                          .fontColor(this.darkMode ? '#FFFFFF' : '#000000')
                          .maxLines(1)
                          .textOverflow({ overflow: TextOverflow.Ellipsis })
                          .width('100%')
                        
                        Row({ space: 8 }) {
                          Text(item.url)
                            .fontSize(12)
                            .fontColor('#8E8E93')
                            .maxLines(1)
                            .textOverflow({ overflow: TextOverflow.Ellipsis })
                            .layoutWeight(1)
                          
                          if (item.visitCount > 1) {
                            Text(`访问${item.visitCount}次`)
                              .fontSize(11)
                              .fontColor('#8E8E93')
                          }
                        }
                        .width('100%')
                      }
                      .alignItems(HorizontalAlign.Start)
                      .layoutWeight(1)
                      
                      // 访问时间
                      if (!this.isEditing) {
                        Text(this.formatTime(item.visitTime))
                          .fontSize(12)
                          .fontColor('#8E8E93')
                          .margin({ left: 8 })
                      }
                    }
                    .width('100%')
                    .padding({ left: 16, right: 16, top: 12, bottom: 12 })
                    .backgroundColor(this.darkMode ? '#2C2C2E' : '#FFFFFF')
                  }
                  .onClick(() => {
                    if (!this.isEditing) {
                      // 点击打开URL
                      this.openUrl(item.url);
                    }
                  })
                  .gesture(
                    LongPressGesture()
                      .onAction(() => {
                        this.showItemMenu(item);
                      })
                  )
                }, (item: HistoryItem) => item.id.toString())
              }
            }, (dateLabel: string) => dateLabel)
          }
          .width('100%')
          .layoutWeight(1)
          .divider({
            strokeWidth: 0.5,
            color: this.darkMode ? '#38383A' : '#E5E5EA',
            startMargin: this.isEditing ? 80 : 68,
            endMargin: 0
          })
        }
        
        // 底部清空按钮
        if (this.historyList.length > 0 && !this.isEditing) {
          Button('清空所有历史记录')
            .fontSize(14)
            .backgroundColor(Color.Transparent)
            .fontColor('#FF3B30')
            .width('100%')
            .height(50)
            .border({ width: { top: 0.5 }, color: this.darkMode ? '#38383A' : '#E5E5EA' })
            .onClick(() => {
              this.showClearAllConfirm();
            })
        }
      }
      .width('100%')
      .height('100%')
      .backgroundColor(this.darkMode ? '#1C1C1E' : '#F2F2F7')
    }
    .hideTitleBar(true)
  }
  
  // 日期分组标题
  @Builder
  dateHeader(label: string) {
    Text(label)
      .fontSize(14)
      .fontColor('#8E8E93')
      .width('100%')
      .height(32)
      .padding({ left: 16 })
      .backgroundColor(this.darkMode ? '#1C1C1E' : '#F2F2F7')
      .textAlign(TextAlign.Start)
  }
  
  // 根据URL生成图标颜色
  getIconColor(url: string): string {
    const colors = ['#FF6B6B', '#4ECDC4', '#45B7D1', '#FFA07A', '#98D8C8', '#F7DC6F', '#BB8FCE', '#85C1E9'];
    let hash = 0;
    for (let i = 0; i < url.length; i++) {
      hash = url.charCodeAt(i) + ((hash << 5) - hash);
    }
    return colors[Math.abs(hash) % colors.length];
  }
  
  // 打开URL
  openUrl(url: string) {
    AppStorage.setOrCreate('openUrl', url);
    this.pathStack.pop();
  }
  
  // 显示项目菜单
  showItemMenu(item: HistoryItem) {
    AlertDialog.show({
      title: '操作',
      message: item.title,
      primaryButton: {
        value: '删除',
        fontColor: '#FF3B30',
        action: () => {
          this.deleteHistoryItem(item.id);
        }
      },
      secondaryButton: {
        value: '取消',
        action: () => {}
      }
    });
  }
  
  // 删除单个历史记录
  async deleteHistoryItem(id: number) {
    await dataManager.deleteHistory(id);
    this.loadHistory();
    promptAction.showToast({ message: '已删除' });
  }
  
  // 删除选中项
  async deleteSelectedItems() {
    if (this.selectedItems.size === 0) {
      return;
    }
    
    AlertDialog.show({
      title: '确认删除',
      message: `确定要删除选中的 ${this.selectedItems.size} 条历史记录吗？`,
      primaryButton: {
        value: '删除',
        fontColor: '#FF3B30',
        action: async () => {
          await dataManager.deleteHistoryBatch(Array.from(this.selectedItems));
          this.selectedItems.clear();
          this.loadHistory();
          promptAction.showToast({ message: '删除成功' });
        }
      },
      secondaryButton: {
        value: '取消',
        action: () => {}
      }
    });
  }
  
  // 全选
  selectAllItems() {
    this.historyList.forEach(item => {
      this.selectedItems.add(item.id);
    });
    this.selectedItems = new Set(this.selectedItems);
  }
  
  // 显示清空确认对话框
  showClearAllConfirm() {
    AlertDialog.show({
      title: '清空历史记录',
      message: '确定要清空所有浏览历史吗？此操作不可恢复。',
      primaryButton: {
        value: '清空',
        fontColor: '#FF3B30',
        action: async () => {
          await dataManager.clearAllHistory();
          this.loadHistory();
          promptAction.showToast({ message: '历史记录已清空' });
        }
      },
      secondaryButton: {
        value: '取消',
        action: () => {}
      }
    });
  }
}
