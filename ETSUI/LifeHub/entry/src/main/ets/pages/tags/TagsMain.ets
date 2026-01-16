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
 * 标签管理页面
 */
import { LifeHubData, Tag } from '../../model/LifeHubData';

interface TagWithCount extends Tag {
  count: number;
}

interface ColorOption {
  value: string;
}

interface CategoryOption {
  value: 'all' | 'health' | 'goal' | 'mood' | 'learning' | 'diary';
  label: string;
}

@Entry
@Component
export struct TagsMain {
  private lifeHubData: LifeHubData = new LifeHubData();
  
  @State tags: TagWithCount[] = [];
  @State isLoading: boolean = true;
  @State showAddDialog: boolean = false;
  @State newTagName: string = '';
  @State newTagColor: string = '#007DFF';
  @State newTagCategory: 'all' | 'health' | 'goal' | 'mood' | 'learning' | 'diary' = 'all';

  private colors: ColorOption[] = [
    { value: '#007DFF' }, { value: '#4CAF50' }, { value: '#FF9800' }, { value: '#E91E63' }, 
    { value: '#9C27B0' }, { value: '#00BCD4' }, { value: '#FF5722' }, { value: '#795548' }
  ];

  private categories: CategoryOption[] = [
    { value: 'all', label: '全部' },
    { value: 'health', label: '健康' },
    { value: 'goal', label: '目标' },
    { value: 'mood', label: '心情' },
    { value: 'learning', label: '学习' },
    { value: 'diary', label: '日记' }
  ];

  aboutToAppear() {
    const context = getContext(this);
    if (context) {
      this.lifeHubData.setContext(context);
    }
    this.loadTags();
  }

  async loadTags() {
    this.isLoading = true;
    try {
      const allTags = await this.lifeHubData.getAllTags();
      // 统计每个标签的使用次数
      const tagsWithCount = await this.updateTagCounts(allTags);
      this.tags = tagsWithCount;
    } catch (error) {
      console.error('TagsMain: 加载标签失败:', error);
      this.tags = [];
    } finally {
      this.isLoading = false;
    }
  }

  async updateTagCounts(tags: Tag[]): Promise<TagWithCount[]> {
    try {
      const healthRecords = await this.lifeHubData.getHealthRecords(undefined, 10000);
      const goals = await this.lifeHubData.getAllGoals();
      const moodRecords = await this.lifeHubData.getMoodRecords(10000);
      const diaries = await this.lifeHubData.getAllDiaries();

      const tagsWithCount: TagWithCount[] = [];
      // 统计标签使用次数
      for (let i = 0; i < tags.length; i++) {
        const tag = tags[i];
        let count = 0;
        
        if (tag.category === 'all' || tag.category === 'health') {
          for (let j = 0; j < healthRecords.length; j++) {
            const r = healthRecords[j];
            if (r.note && r.note.includes(`#${tag.name}`)) {
              count++;
            }
          }
        }
        
        if (tag.category === 'all' || tag.category === 'goal') {
          for (let j = 0; j < goals.length; j++) {
            const g = goals[j];
            if (g.description && g.description.includes(`#${tag.name}`)) {
              count++;
            }
          }
        }
        
        if (tag.category === 'all' || tag.category === 'mood') {
          for (let j = 0; j < moodRecords.length; j++) {
            const m = moodRecords[j];
            if (m.tags && m.tags.includes(tag.name)) {
              count++;
            }
          }
        }
        
        if (tag.category === 'all' || tag.category === 'diary') {
          for (let j = 0; j < diaries.length; j++) {
            const d = diaries[j];
            if (d.tags && d.tags.includes(tag.name)) {
              count++;
            }
          }
        }
        
        const tagWithCount: TagWithCount = {
          id: tag.id,
          name: tag.name,
          color: tag.color,
          category: tag.category,
          createTime: tag.createTime,
          count: count
        };
        tagsWithCount.push(tagWithCount);
      }
      return tagsWithCount;
    } catch (error) {
      console.error('TagsMain: 更新标签统计失败:', error);
      const defaultTags: TagWithCount[] = [];
      for (let i = 0; i < tags.length; i++) {
        const tag = tags[i];
        defaultTags.push({
          id: tag.id,
          name: tag.name,
          color: tag.color,
          category: tag.category,
          createTime: tag.createTime,
          count: 0
        });
      }
      return defaultTags;
    }
  }

  build() {
    Column() {
      // 标题栏
      Row() {
        Text('标签管理')
          .fontSize(24)
          .fontWeight(FontWeight.Bold)
        
        Blank()
        
        Button('+')
          .fontSize(20)
          .width(40)
          .height(40)
          .borderRadius(20)
          .backgroundColor('#007DFF')
          .fontColor(Color.White)
          .onClick(() => {
            this.showAddTagDialog();
          })
      }
      .width('100%')
      .padding(20)
      .backgroundColor('#f0f0f0')

      // 标签列表
      if (this.isLoading) {
        Column() {
          LoadingProgress()
            .width(50)
            .height(50)
            .margin({ top: 100 })
        }
        .width('100%')
        .layoutWeight(1)
        .justifyContent(FlexAlign.Center)
      } else if (this.tags.length === 0) {
        Column() {
          Text('🏷️')
            .fontSize(60)
            .margin({ bottom: 20 })
          Text('还没有标签')
            .fontSize(18)
            .fontColor('#666')
          Text('点击右上角"+"添加标签')
            .fontSize(14)
            .fontColor('#999')
            .margin({ top: 10 })
        }
        .width('100%')
        .layoutWeight(1)
        .justifyContent(FlexAlign.Center)
      } else {
        Scroll() {
          Column() {
            ForEach(this.tags, (tag: TagWithCount) => {
              this.buildTagCard(tag)
            })
          }
          .width('100%')
          .padding(15)
        }
        .layoutWeight(1)
      }

      // 添加标签对话框
      if (this.showAddDialog) {
        Column() {
          // 半透明背景
          Blank()
            .width('100%')
            .height('100%')
            .backgroundColor('#000000')
            .opacity(0.5)
            .onClick(() => {
              this.showAddDialog = false;
              this.newTagName = '';
              this.newTagColor = '#007DFF';
              this.newTagCategory = 'all';
            })

          // 对话框内容
          Column() {
            Text('添加标签')
              .fontSize(18)
              .fontWeight(FontWeight.Bold)
              .width('100%')
              .textAlign(TextAlign.Center)
              .margin({ bottom: 20 })

            TextInput({ placeholder: '输入标签名称', text: this.newTagName })
              .width('100%')
              .height(50)
              .fontSize(16)
              .padding({ left: 15, right: 15 })
              .backgroundColor('#f8f8f8')
              .borderRadius(8)
              .onChange((value: string) => {
                this.newTagName = value;
              })
              .margin({ bottom: 15 })

            Text('标签颜色')
              .fontSize(14)
              .fontWeight(FontWeight.Medium)
              .width('100%')
              .textAlign(TextAlign.Start)
              .margin({ bottom: 10 })

            Row() {
              ForEach(this.colors, (color: ColorOption) => {
                Column() {
                  Blank()
                }
                .width(35)
                .height(35)
                .borderRadius(18)
                .backgroundColor(color.value)
                .border({
                  width: this.newTagColor === color.value ? 2 : 0,
                  color: '#333'
                })
                .onClick(() => {
                  this.newTagColor = color.value;
                })
              })
            }
            .width('100%')
            .justifyContent(FlexAlign.SpaceAround)
            .margin({ bottom: 20 })

            Row() {
              Button('取消')
                .layoutWeight(1)
                .height(45)
                .fontSize(16)
                .backgroundColor('#f0f0f0')
                .fontColor('#333')
                .onClick(() => {
                  this.showAddDialog = false;
                  this.newTagName = '';
                  this.newTagColor = '#007DFF';
                  this.newTagCategory = 'all';
                })

              Button('添加')
                .layoutWeight(1)
                .height(45)
                .fontSize(16)
                .backgroundColor('#007DFF')
                .fontColor(Color.White)
                .margin({ left: 15 })
                .enabled(!!this.newTagName)
                .onClick(() => {
                  this.addTag();
                  this.showAddDialog = false;
                })
            }
            .width('100%')
          }
          .width('85%')
          .padding(20)
          .backgroundColor(Color.White)
          .borderRadius(12)
          .shadow({ radius: 10, color: '#40000000', offsetX: 0, offsetY: 5 })
        }
        .width('100%')
        .height('100%')
        .justifyContent(FlexAlign.Center)
        .alignItems(HorizontalAlign.Center)
        .position({ x: 0, y: 0 })
        .zIndex(1000)
      }
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#f5f5f5')
  }

  showAddTagDialog() {
    this.showAddDialog = true;
    this.newTagName = '';
    this.newTagColor = '#007DFF';
    this.newTagCategory = 'all';
  }

  async addTag() {
    if (!this.newTagName) {
      // 如果名称为空，不执行任何操作
      return;
    }

    try {
      const tag: Tag = {
        name: this.newTagName,
        color: this.newTagColor,
        category: this.newTagCategory,
        createTime: Date.now()
      };
      await this.lifeHubData.addTag(tag);
      this.newTagName = '';
      this.newTagColor = '#007DFF';
      this.newTagCategory = 'all';
      await this.loadTags();
    } catch (error) {
      console.error('TagsMain: 添加标签失败:', error);
    }
  }

  @Builder
  buildTagCard(tag: TagWithCount) {
    Row() {
      Row() {
        Text('#')
          .fontSize(16)
          .fontColor(tag.color)
        Text(tag.name)
          .fontSize(16)
          .fontWeight(FontWeight.Medium)
          .fontColor('#333')
      }
      .layoutWeight(1)
      .alignItems(VerticalAlign.Top)

      Row() {
        Text(this.getCategoryLabel(tag.category))
          .fontSize(12)
          .fontColor('#666')
          .padding({ left: 8, right: 8, top: 4, bottom: 4 })
          .backgroundColor('#f0f0f0')
          .borderRadius(10)
          .margin({ right: 10 })

        Text(`${tag.count}次`)
          .fontSize(14)
          .fontColor('#666')
      }

      Button('删除')
        .fontSize(14)
        .height(32)
        .backgroundColor('#FF5722')
        .fontColor(Color.White)
        .margin({ left: 10 })
        .onClick(() => {
          this.deleteTag(tag);
        })
    }
    .width('100%')
    .padding(15)
    .backgroundColor(Color.White)
    .borderRadius(12)
    .border({
      width: 2,
      color: tag.color
    })
    .margin({ bottom: 10 })
  }

  getCategoryLabel(category: string): string {
    const cat = this.categories.find(c => c.value === category);
    return cat?.label || category;
  }

  async deleteTag(tag: Tag) {
    try {
      await this.lifeHubData.deleteTag(tag.id!);
      await this.loadTags();
    } catch (error) {
      console.error('TagsMain: 删除标签失败:', error);
    }
  }
}
