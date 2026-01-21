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
/**
 * 数据筛选和排序页面
 */
import { LifeHubData, HealthRecord, MoodRecord, Goal, Skill, TravelPlan, Diary } from '../../model/LifeHubData';
import { router } from '@kit.ArkUI';

interface ModuleOption {
  value: 'health' | 'goal' | 'mood' | 'learning' | 'travel' | 'diary';
  label: string;
  icon: string;
}

interface ItemWithDate {
  date?: string;
  createTime?: number;
}

interface ItemWithTitle {
  title?: string;
  name?: string;
}

interface ItemWithValue {
  value?: number;
  progress?: number;
  level?: number;
}

interface ItemWithId {
  id?: number;
}

@Entry
@Component
export struct FilterMain {
  private lifeHubData: LifeHubData = new LifeHubData();
  @State selectedModule: 'health' | 'goal' | 'mood' | 'learning' | 'travel' | 'diary' = 'health';
  @State sortBy: 'date' | 'name' | 'value' = 'date';
  @State sortOrder: 'asc' | 'desc' = 'desc';
  @State filterType: string = 'all';
  @State filterStatus: string = 'all';
  @State startDate: string = '';
  @State endDate: string = '';
  @State filteredData: Array<HealthRecord | MoodRecord | Goal | Skill | TravelPlan | Diary> = [];
  @State isLoading: boolean = false;
  private modules: ModuleOption[] = [
    { value: 'health', label: '健康记录', icon: '💚' },
    { value: 'goal', label: '目标', icon: '🎯' },
    { value: 'mood', label: '心情', icon: '😊' },
    { value: 'learning', label: '学习', icon: '📚' },
    { value: 'travel', label: '旅行', icon: '✈️' },
    { value: 'diary', label: '日记', icon: '📝' }
  ];

  aboutToAppear() {
    const context = getContext(this);
    if (context) {
      this.lifeHubData.setContext(context);
    }
    this.loadData();
  }

  async loadData() {
    this.isLoading = true;
    try {
      await this.applyFilters();
    } catch (error) {
      console.error('FilterMain: 加载数据失败:', error);
    } finally {
      this.isLoading = false;
    }
  }

  async applyFilters() {
    let data: Array<HealthRecord | MoodRecord | Goal | Skill | TravelPlan | Diary> = [];

    // 加载数据
    if (this.selectedModule === 'health') {
      const healthData = await this.lifeHubData.getHealthRecords(undefined, 10000);
      if (this.filterType !== 'all') {
        for (let i = 0; i < healthData.length; i++) {
          if (healthData[i].type === this.filterType) {
            data.push(healthData[i]);
          }
        }
      } else {
        data = healthData;
      }
    } else if (this.selectedModule === 'goal') {
      const goalData = await this.lifeHubData.getAllGoals();
      if (this.filterStatus !== 'all') {
        for (let i = 0; i < goalData.length; i++) {
          if (goalData[i].status === this.filterStatus) {
            data.push(goalData[i]);
          }
        }
      } else {
        data = goalData;
      }
    } else if (this.selectedModule === 'mood') {
      data = await this.lifeHubData.getMoodRecords(10000);
    } else if (this.selectedModule === 'learning') {
      data = await this.lifeHubData.getAllSkills();
    } else if (this.selectedModule === 'travel') {
      const travelData = await this.lifeHubData.getAllTravelPlans();
      if (this.filterStatus !== 'all') {
        for (let i = 0; i < travelData.length; i++) {
          if (travelData[i].status === this.filterStatus) {
            data.push(travelData[i]);
          }
        }
      } else {
        data = travelData;
      }
    } else if (this.selectedModule === 'diary') {
      data = await this.lifeHubData.getAllDiaries();
    }

    // 日期筛选
    if (this.startDate && this.endDate) {
      const filtered: Array<HealthRecord | MoodRecord | Goal | Skill | TravelPlan | Diary> = [];
      for (let i = 0; i < data.length; i++) {
        const item = data[i];
        let itemDate: string = '';
        const itemWithDate = item as ItemWithDate;
        if (this.hasProperty(item, 'date') && typeof itemWithDate.date === 'string') {
          itemDate = itemWithDate.date;
        } else if (this.hasProperty(item, 'createTime') && typeof itemWithDate.createTime === 'number') {
          itemDate = this.formatDate(new Date(itemWithDate.createTime));
        } else {
          itemDate = this.formatDate(new Date());
        }
        if (itemDate >= this.startDate && itemDate <= this.endDate) {
          filtered.push(item);
        }
      }
      data = filtered;
    }

    // 排序
    data.sort((a: HealthRecord | MoodRecord | Goal | Skill | TravelPlan | Diary,
      b: HealthRecord | MoodRecord | Goal | Skill | TravelPlan | Diary) => {
      let compareValue = 0;

      if (this.sortBy === 'date') {
        let dateA = 0;
        let dateB = 0;
        const aWithDate = a as ItemWithDate;
        const bWithDate = b as ItemWithDate;
        if (this.hasProperty(a, 'date') && typeof aWithDate.date === 'string') {
          dateA = new Date(aWithDate.date).getTime();
        } else if (this.hasProperty(a, 'createTime') && typeof aWithDate.createTime === 'number') {
          dateA = aWithDate.createTime;
        }
        if (this.hasProperty(b, 'date') && typeof bWithDate.date === 'string') {
          dateB = new Date(bWithDate.date).getTime();
        } else if (this.hasProperty(b, 'createTime') && typeof bWithDate.createTime === 'number') {
          dateB = bWithDate.createTime;
        }
        compareValue = dateA - dateB;
      } else if (this.sortBy === 'name') {
        let nameA = '';
        let nameB = '';
        const aWithTitle = a as ItemWithTitle;
        const bWithTitle = b as ItemWithTitle;
        if (this.hasProperty(a, 'title') && typeof aWithTitle.title === 'string') {
          nameA = aWithTitle.title;
        } else if (this.hasProperty(a, 'name') && typeof aWithTitle.name === 'string') {
          nameA = aWithTitle.name;
        }
        if (this.hasProperty(b, 'title') && typeof bWithTitle.title === 'string') {
          nameB = bWithTitle.title;
        } else if (this.hasProperty(b, 'name') && typeof bWithTitle.name === 'string') {
          nameB = bWithTitle.name;
        }
        compareValue = nameA.toLowerCase().localeCompare(nameB.toLowerCase());
      } else if (this.sortBy === 'value') {
        let valueA = 0;
        let valueB = 0;
        const aWithValue = a as ItemWithValue;
        const bWithValue = b as ItemWithValue;
        if (this.hasProperty(a, 'value') && typeof aWithValue.value === 'number') {
          valueA = aWithValue.value;
        } else if (this.hasProperty(a, 'progress') && typeof aWithValue.progress === 'number') {
          valueA = aWithValue.progress;
        } else if (this.hasProperty(a, 'level') && typeof aWithValue.level === 'number') {
          valueA = aWithValue.level;
        }
        if (this.hasProperty(b, 'value') && typeof bWithValue.value === 'number') {
          valueB = bWithValue.value;
        } else if (this.hasProperty(b, 'progress') && typeof bWithValue.progress === 'number') {
          valueB = bWithValue.progress;
        } else if (this.hasProperty(b, 'level') && typeof bWithValue.level === 'number') {
          valueB = bWithValue.level;
        }
        compareValue = valueA - valueB;
      }

      return this.sortOrder === 'asc' ? compareValue : -compareValue;
    });

    this.filteredData = data;
  }

  build() {
    Column() {
      // 标题栏
      Row() {
        Text('数据筛选')
          .fontSize(24)
          .fontWeight(FontWeight.Bold)

        Blank()
      }
      .width('100%')
      .padding(20)
      .backgroundColor('#f0f0f0')

      Scroll() {
        Column() {
          // 模块选择
          Text('选择模块')
            .fontSize(16)
            .fontWeight(FontWeight.Medium)
            .width('100%')
            .textAlign(TextAlign.Start)
            .padding({ left: 20, top: 20, bottom: 10 })

          Grid() {
            ForEach(this.modules, (module: ModuleOption) => {
              GridItem() {
                Column() {
                  Text(module.icon)
                    .fontSize(32)
                    .margin({ bottom: 8 })
                  Text(module.label)
                    .fontSize(14)
                    .fontColor(this.selectedModule === module.value ? '#007DFF' : '#666')
                }
                .width('100%')
                .height(100)
                .justifyContent(FlexAlign.Center)
                .backgroundColor(this.selectedModule === module.value ? '#E3F2FD' : '#f8f8f8')
                .borderRadius(8)
                .border({
                  width: this.selectedModule === module.value ? 2 : 0,
                  color: '#007DFF'
                })
                .onClick(() => {
                  this.selectedModule = module.value;
                  this.loadData();
                })
              }
            })
          }
          .columnsTemplate('1fr 1fr 1fr')
          .columnsGap(10)
          .rowsGap(10)
          .width('100%')
          .height(220)
          .margin({ left: 15, right: 15, bottom: 20 })

          // 筛选条件
          Text('筛选条件')
            .fontSize(16)
            .fontWeight(FontWeight.Medium)
            .width('100%')
            .textAlign(TextAlign.Start)
            .padding({ left: 20, top: 20, bottom: 10 })

          Column() {
            if (this.selectedModule === 'health') {
              this.buildHealthFilters()
            } else if (this.selectedModule === 'goal' || this.selectedModule === 'travel') {
              this.buildStatusFilters()
            }

            // 日期筛选
            Row() {
              Column() {
                Text('开始日期')
                  .fontSize(14)
                  .fontColor('#666')
                  .margin({ bottom: 5 })
                Text(this.startDate || '选择日期')
                  .fontSize(16)
                  .fontColor(this.startDate ? '#333' : '#999')
                  .padding(10)
                  .backgroundColor('#f8f8f8')
                  .borderRadius(8)
                  .onClick(() => {
                    // TODO: 打开日期选择器
                  })
              }
              .layoutWeight(1)
              .margin({ right: 10 })

              Column() {
                Text('结束日期')
                  .fontSize(14)
                  .fontColor('#666')
                  .margin({ bottom: 5 })
                Text(this.endDate || '选择日期')
                  .fontSize(16)
                  .fontColor(this.endDate ? '#333' : '#999')
                  .padding(10)
                  .backgroundColor('#f8f8f8')
                  .borderRadius(8)
                  .onClick(() => {
                    // TODO: 打开日期选择器
                  })
              }
              .layoutWeight(1)
            }
            .width('100%')
            .margin({ top: 15 })
          }
          .width('100%')
          .padding(15)
          .backgroundColor(Color.White)
          .borderRadius(12)
          .margin({ left: 15, right: 15, bottom: 20 })

          // 排序设置
          Text('排序设置')
            .fontSize(16)
            .fontWeight(FontWeight.Medium)
            .width('100%')
            .textAlign(TextAlign.Start)
            .padding({ left: 20, top: 20, bottom: 10 })

          Column() {
            Row() {
              Text('排序方式')
                .fontSize(16)
                .fontColor('#333')

              Blank()

              Row() {
                Button('日期')
                  .fontSize(14)
                  .height(32)
                  .backgroundColor(this.sortBy === 'date' ? '#007DFF' : '#f0f0f0')
                  .fontColor(this.sortBy === 'date' ? Color.White : '#666')
                  .onClick(() => {
                    this.sortBy = 'date';
                    this.loadData();
                  })
                  .margin({ right: 10 })

                Button('名称')
                  .fontSize(14)
                  .height(32)
                  .backgroundColor(this.sortBy === 'name' ? '#007DFF' : '#f0f0f0')
                  .fontColor(this.sortBy === 'name' ? Color.White : '#666')
                  .onClick(() => {
                    this.sortBy = 'name';
                    this.loadData();
                  })
                  .margin({ right: 10 })

                Button('数值')
                  .fontSize(14)
                  .height(32)
                  .backgroundColor(this.sortBy === 'value' ? '#007DFF' : '#f0f0f0')
                  .fontColor(this.sortBy === 'value' ? Color.White : '#666')
                  .onClick(() => {
                    this.sortBy = 'value';
                    this.loadData();
                  })
              }
            }
            .width('100%')
            .margin({ bottom: 15 })

            Row() {
              Text('排序顺序')
                .fontSize(16)
                .fontColor('#333')

              Blank()

              Row() {
                Button('升序')
                  .fontSize(14)
                  .height(32)
                  .backgroundColor(this.sortOrder === 'asc' ? '#007DFF' : '#f0f0f0')
                  .fontColor(this.sortOrder === 'asc' ? Color.White : '#666')
                  .onClick(() => {
                    this.sortOrder = 'asc';
                    this.loadData();
                  })
                  .margin({ right: 10 })

                Button('降序')
                  .fontSize(14)
                  .height(32)
                  .backgroundColor(this.sortOrder === 'desc' ? '#007DFF' : '#f0f0f0')
                  .fontColor(this.sortOrder === 'desc' ? Color.White : '#666')
                  .onClick(() => {
                    this.sortOrder = 'desc';
                    this.loadData();
                  })
              }
            }
            .width('100%')
          }
          .width('100%')
          .padding(15)
          .backgroundColor(Color.White)
          .borderRadius(12)
          .margin({ left: 15, right: 15, bottom: 20 })

          // 结果统计
          Row() {
            Text(`找到 ${this.filteredData.length} 条记录`)
              .fontSize(16)
              .fontWeight(FontWeight.Medium)
              .fontColor('#333')
          }
          .width('100%')
          .padding({ left: 20, right: 20, bottom: 10 })

          // 结果列表
          if (this.isLoading) {
            Column() {
              LoadingProgress()
                .width(40)
                .height(40)
                .margin({ top: 50 })
            }
            .width('100%')
            .height(200)
            .justifyContent(FlexAlign.Center)
          } else if (this.filteredData.length === 0) {
            Column() {
              Text('🔍')
                .fontSize(60)
                .margin({ bottom: 20 })
              Text('没有找到匹配的数据')
                .fontSize(18)
                .fontColor('#666')
            }
            .width('100%')
            .height(200)
            .justifyContent(FlexAlign.Center)
          } else {
            ForEach(this.filteredData.slice(0, 10),
              (item: HealthRecord | MoodRecord | Goal | Skill | TravelPlan | Diary, index: number) => {
                this.buildResultItem(item, index)
              }, (item: HealthRecord | MoodRecord | Goal | Skill | TravelPlan | Diary, index: number) => {
                const itemId = this.getItemId(item);
                const idStr = itemId ? itemId.toString() : '';
                return `${this.selectedModule}-${idStr}-${index}`;
              })
          }
        }
        .width('100%')
      }
      .layoutWeight(1)
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#f5f5f5')
  }

  @Builder
  buildHealthFilters() {
    Row() {
      Text('类型')
        .fontSize(14)
        .fontColor('#666')
        .margin({ right: 10 })

      Row() {
        Button('全部')
          .fontSize(12)
          .height(28)
          .backgroundColor(this.filterType === 'all' ? '#007DFF' : '#f0f0f0')
          .fontColor(this.filterType === 'all' ? Color.White : '#666')
          .onClick(() => {
            this.filterType = 'all';
            this.loadData();
          })
          .margin({ right: 8 })

        Button('体重')
          .fontSize(12)
          .height(28)
          .backgroundColor(this.filterType === 'weight' ? '#007DFF' : '#f0f0f0')
          .fontColor(this.filterType === 'weight' ? Color.White : '#666')
          .onClick(() => {
            this.filterType = 'weight';
            this.loadData();
          })
          .margin({ right: 8 })

        Button('运动')
          .fontSize(12)
          .height(28)
          .backgroundColor(this.filterType === 'exercise' ? '#007DFF' : '#f0f0f0')
          .fontColor(this.filterType === 'exercise' ? Color.White : '#666')
          .onClick(() => {
            this.filterType = 'exercise';
            this.loadData();
          })
      }
    }
    .width('100%')
  }

  @Builder
  buildStatusFilters() {
    Row() {
      Text('状态')
        .fontSize(14)
        .fontColor('#666')
        .margin({ right: 10 })

      Row() {
        Button('全部')
          .fontSize(12)
          .height(28)
          .backgroundColor(this.filterStatus === 'all' ? '#007DFF' : '#f0f0f0')
          .fontColor(this.filterStatus === 'all' ? Color.White : '#666')
          .onClick(() => {
            this.filterStatus = 'all';
            this.loadData();
          })
          .margin({ right: 8 })

        Button('进行中')
          .fontSize(12)
          .height(28)
          .backgroundColor(this.filterStatus === 'ongoing' ? '#007DFF' : '#f0f0f0')
          .fontColor(this.filterStatus === 'ongoing' ? Color.White : '#666')
          .onClick(() => {
            this.filterStatus = 'ongoing';
            this.loadData();
          })
          .margin({ right: 8 })

        Button('已完成')
          .fontSize(12)
          .height(28)
          .backgroundColor(this.filterStatus === 'completed' ? '#007DFF' : '#f0f0f0')
          .fontColor(this.filterStatus === 'completed' ? Color.White : '#666')
          .onClick(() => {
            this.filterStatus = 'completed';
            this.loadData();
          })
      }
    }
    .width('100%')
  }

  @Builder
  buildResultItem(item: HealthRecord | MoodRecord | Goal | Skill | TravelPlan | Diary, index: number) {
    Row() {
      Text(this.getModuleIcon(this.selectedModule))
        .fontSize(24)
        .width(40)

      Column() {
        Text(this.getItemTitle(item))
          .fontSize(16)
          .fontWeight(FontWeight.Medium)
          .textAlign(TextAlign.Start)
          .width('100%')
          .maxLines(1)
          .textOverflow({ overflow: TextOverflow.Ellipsis })

        Text(this.getItemDate(item))
          .fontSize(14)
          .fontColor('#666')
          .textAlign(TextAlign.Start)
          .width('100%')
          .margin({ top: 5 })
      }
      .layoutWeight(1)
      .alignItems(HorizontalAlign.Start)
      .padding({ left: 15 })
    }
    .width('100%')
    .padding(15)
    .backgroundColor(Color.White)
    .borderRadius(10)
    .margin({ left: 15, right: 15, bottom: 10 })
  }

  getModuleIcon(module: string): string {
    const icons: Record<string, string> = {
      'health': '💚',
      'goal': '🎯',
      'mood': '😊',
      'learning': '📚',
      'travel': '✈️',
      'diary': '📝'
    };
    return icons[module] || '📄';
  }

  hasProperty(obj: HealthRecord | MoodRecord | Goal | Skill | TravelPlan | Diary, prop: string): boolean {
    // 使用类型检查来验证属性是否存在
    // 由于 ArkTS 不支持 'in' 操作符，我们使用类型断言和 undefined 检查
    if (prop === 'date') {
      const itemWithDate = obj as ItemWithDate;
      return itemWithDate.date !== undefined;
    } else if (prop === 'createTime') {
      const itemWithDate = obj as ItemWithDate;
      return itemWithDate.createTime !== undefined;
    } else if (prop === 'title') {
      const itemWithTitle = obj as ItemWithTitle;
      return itemWithTitle.title !== undefined;
    } else if (prop === 'name') {
      const itemWithTitle = obj as ItemWithTitle;
      return itemWithTitle.name !== undefined;
    } else if (prop === 'value') {
      const itemWithValue = obj as ItemWithValue;
      return itemWithValue.value !== undefined;
    } else if (prop === 'progress') {
      const itemWithValue = obj as ItemWithValue;
      return itemWithValue.progress !== undefined;
    } else if (prop === 'level') {
      const itemWithValue = obj as ItemWithValue;
      return itemWithValue.level !== undefined;
    } else if (prop === 'id') {
      const itemWithId = obj as ItemWithId;
      return itemWithId.id !== undefined;
    }
    return false;
  }

  getItemTitle(item: HealthRecord | MoodRecord | Goal | Skill | TravelPlan | Diary): string {
    const itemWithTitle = item as ItemWithTitle;
    if (this.hasProperty(item, 'title') && typeof itemWithTitle.title === 'string') {
      return itemWithTitle.title;
    } else if (this.hasProperty(item, 'name') && typeof itemWithTitle.name === 'string') {
      return itemWithTitle.name;
    }
    return '未命名';
  }

  getItemDate(item: HealthRecord | MoodRecord | Goal | Skill | TravelPlan | Diary): string {
    const itemWithDate = item as ItemWithDate;
    if (this.hasProperty(item, 'date') && typeof itemWithDate.date === 'string') {
      return itemWithDate.date;
    } else if (this.hasProperty(item, 'createTime') && typeof itemWithDate.createTime === 'number') {
      return this.formatDate(new Date(itemWithDate.createTime));
    }
    return this.formatDate(new Date());
  }

  getItemId(item: HealthRecord | MoodRecord | Goal | Skill | TravelPlan | Diary): number | undefined {
    const itemWithId = item as ItemWithId;
    if (this.hasProperty(item, 'id') && typeof itemWithId.id === 'number') {
      return itemWithId.id;
    }
    return undefined;
  }

  formatDate(date: Date): string {
    const year = date.getFullYear();
    const month = String(date.getMonth() + 1).padStart(2, '0');
    const day = String(date.getDate()).padStart(2, '0');
    return `${year}-${month}-${day}`;
  }
}
