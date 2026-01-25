
/*
 * Copyright (c) 2026 Huawei Co., Ltd.
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
import { promptAction } from '@kit.ArkUI';
import common from '@ohos.app.ability.common';
import picker from '@ohos.file.picker';
import image from '@ohos.multimedia.image';
import fs from '@ohos.file.fs';
import { Item, ItemUseStatus, DEFAULT_CATEGORY_OPTIONS } from '../model/Item';
import ItemRdb from '../data/ItemRdb';
import { StatisticsPage } from './StatisticsPage';
import { MinePage } from './MinePage';
import { itemStore } from '../store';

interface SelectOption {
  value: string;
  icon?: Resource;
}

@Component
struct FormCard {
  @Prop title: string = "";
  @BuilderParam content: () => void = this.emptyBuilder;

  @Builder emptyBuilder() {}

  build() {
    Column() {
      Text(this.title)
        .fontSize(14)
        .fontColor("#666666")
        .margin({ bottom: 8 })
        .width("100%")

      Column() {
        this.content()
      }
      .width("100%")
      .backgroundColor(Color.White)
      .borderRadius(12)
      .padding(12)
      .shadow({ radius: 4, color: "#0D000000", offsetY: 2 })
    }
    .width("100%")
    .margin({ bottom: 16 })
  }
}

@Entry
@Component
struct LightItemNote {
  @State currentTab: number = 0;
  @State itemList: Item[] = [];
  @State searchText: string = "";
  @State editingItemId: number = -1;

  // 图片预览状态 - 关键修改
  @State showImagePreview: boolean = false;
  @State previewImagePath: string = "";
  @State imageScale: number = 1; // 新增：图片缩放比例
  @State imageOffsetX: number = 0; // 新增：图片X轴偏移
  @State imageOffsetY: number = 0; // 新增：图片Y轴偏移

  // 表单状态
  @State itemName: string = "";
  @State itemCategory: string = "";
  @State itemPrice: string = "";
  @State itemLocation: string = "";
  @State itemNote: string = "";
  @State itemUseStatus: ItemUseStatus = "在用";
  @State selectedDate: Date = new Date();
  @State dateStr: string = "";
  @State nameError: string = "";
  @State priceError: string = "";
  @State itemImagePath: string = "";

  private itemCategories: string[] = DEFAULT_CATEGORY_OPTIONS;
  private itemUseStatusOptions: ItemUseStatus[] = ["在用", "闲置", "已丢弃"];
  private context = getContext(this) as common.UIAbilityContext;

  aboutToAppear(): void {
    const now = new Date();
    this.dateStr = this.formatDate(now);
    this.initDatabase();
    itemStore.onDataCleared(this.dataClearedCallback);
    itemStore.onItemCountChange(this.itemCountChangedCallback);
  }

  aboutToDisappear(): void {
    itemStore.offDataCleared(this.dataClearedCallback);
    itemStore.offItemCountChange(this.itemCountChangedCallback);
  }

  private async initDatabase(): Promise<void> {
    try {
      await ItemRdb.getRdbStore(this.context);
      await this.refreshItemList();
      await itemStore.updateItemCount();
    } catch (err) {
      console.error("数据库初始化失败:", err);
      promptAction.showToast({ message: '数据初始化失败', duration: 2000 });
    }
  }

  private formatDate(date: Date): string {
    return `${date.getFullYear()}/${date.getMonth() + 1}/${date.getDate()}`;
  }

  private async refreshItemList(): Promise<void> {
    if (!ItemRdb.isInitialized()) {
      console.warn("数据库未初始化，无法加载数据");
      return;
    }

    try {
      const items = await ItemRdb.queryAll();
      this.itemList = Array.from(items);
      console.info(`列表刷新成功，共${this.itemList.length}条数据`);
    } catch (err) {
      console.error("加载物品列表失败:", err);
      promptAction.showToast({ message: '加载数据失败', duration: 2000 });
    }
  }

  private showDatePicker(): void {
    DatePickerDialog.show({
      start: new Date("2010-1-1"),
      end: new Date("2100-12-31"),
      selected: this.selectedDate,
      onAccept: (value: DatePickerResult) => {
        const year = value.year;
        const month = value.month;
        const day = value.day;
        if (year !== undefined && month !== undefined && day !== undefined) {
          this.selectedDate = new Date(year, month, day);
          this.dateStr = this.formatDate(this.selectedDate);
        }
      }
    })
  }

  private validateForm(): boolean {
    let isValid = true;

    if (this.itemName.trim() === "") {
      this.nameError = "物品名称不能为空";
      isValid = false;
    } else {
      this.nameError = "";
    }

    if (this.itemPrice === "") {
      this.priceError = "价格不能为空";
      isValid = false;
    } else if (isNaN(Number(this.itemPrice)) || Number(this.itemPrice) < 0) {
      this.priceError = "请输入有效的价格";
      isValid = false;
    } else {
      this.priceError = "";
    }

    return isValid;
  }

  private getCategoryOptions(): SelectOption[] {
    const options: SelectOption[] = [];
    for (let i = 0; i < this.itemCategories.length; i++) {
      options.push({ value: this.itemCategories[i] });
    }
    return options;
  }

  private async selectItemImage(): Promise<void> {
    try {
      const photoPicker = new picker.PhotoViewPicker();
      const selectOptions = new picker.PhotoSelectOptions();
      selectOptions.maxSelectNumber = 1;
      selectOptions.MIMEType = picker.PhotoViewMIMETypes.IMAGE_TYPE;

      const result = await photoPicker.select(selectOptions);

      if (result.photoUris && result.photoUris.length > 0) {
        const imageUri = result.photoUris[0];
        this.itemImagePath = imageUri;
        promptAction.showToast({ message: '图片选择成功', duration: 1500 });
      }
    } catch (err) {
      console.error("选择图片失败:", err);
      promptAction.showToast({ message: '图片选择失败，请重试', duration: 2000 });
    }
  }

  private removeItemImage(): void {
    this.itemImagePath = "";
    promptAction.showToast({ message: '图片已移除', duration: 1500 });
  }

  // 图片预览 - 关键修改
  private previewImage(item: Item): void {
    if (item.imagePath) {
      this.previewImagePath = item.imagePath;
      this.showImagePreview = true;
      this.imageScale = 1; // 重置缩放
      this.imageOffsetX = 0; // 重置偏移
      this.imageOffsetY = 0;
    } else {
      promptAction.showToast({ message: '该物品暂无图片', duration: 1500 });
    }
  }

  // 关闭预览 - 关键修改
  private closeImagePreview(): void {
    this.showImagePreview = false;
    this.previewImagePath = "";
    this.imageScale = 1;
    this.imageOffsetX = 0;
    this.imageOffsetY = 0;
  }

  // 图片缩放手势 - 新增
  private onPinchGesture(event: GestureEvent): void {
    if (event.scale !== undefined) {
      this.imageScale = Math.max(0.5, Math.min(3, this.imageScale * event.scale));
    }
  }

  // 图片平移手势 - 新增
  private onPanGesture(event: GestureEvent): void {
    if (event.offsetX !== undefined && event.offsetY !== undefined) {
      this.imageOffsetX += event.offsetX;
      this.imageOffsetY += event.offsetY;
    }
  }

  private startEdit(item: Item): void {
    this.editingItemId = item.id;
    this.itemName = item.name;
    this.itemCategory = item.category;
    this.itemPrice = item.price.toString();
    this.itemLocation = item.location;
    this.itemNote = item.note;
    this.itemUseStatus = item.useStatus;
    this.dateStr = item.buyDate;
    this.selectedDate = new Date(item.buyDate);
    this.itemImagePath = item.imagePath || "";

    this.currentTab = 0;
    promptAction.showToast({ message: '进入编辑模式', duration: 1500 });
  }

  private async addItem(): Promise<void> {
    if (!this.validateForm()) {
      promptAction.showToast({ message: '请检查表单填写', duration: 2000 });
      return;
    }

    const itemData: Item = {
      id: this.editingItemId !== -1 ? this.editingItemId : Date.now(),
      name: this.itemName.trim(),
      category: this.itemCategory || "其他",
      buyDate: this.dateStr,
      price: Number(this.itemPrice),
      location: this.itemLocation.trim(),
      note: this.itemNote.trim(),
      useStatus: this.itemUseStatus,
      isLent: false,
      lendTo: "",
      returnDate: "",
      isPrecious: false,
      maintenanceDate: "",
      imagePath: this.itemImagePath
    };

    try {
      if (this.editingItemId !== -1) {
        await ItemRdb.update(itemData);
        await itemStore.updateItemCount();
        promptAction.showToast({ message: '物品修改成功！', duration: 2000 });
      } else {
        await ItemRdb.insert(itemData);
        await itemStore.updateItemCount();
        promptAction.showToast({ message: '物品添加成功！', duration: 2000 });
      }

      this.itemList = [];
      setTimeout(async () => {
        await this.refreshItemList();
        this.itemList = [...this.itemList];
      }, 100);

      this.resetForm();
    } catch (err) {
      console.error("操作失败:", err);
      promptAction.showToast({
        message: this.editingItemId !== -1 ? '修改失败，请重试' : '添加失败，请重试',
        duration: 2000
      });
    }
  }

  private resetForm(): void {
    this.itemName = "";
    this.itemCategory = "";
    this.itemPrice = "";
    this.itemLocation = "";
    this.itemNote = "";
    this.itemUseStatus = "在用";
    this.selectedDate = new Date();
    this.dateStr = this.formatDate(this.selectedDate);
    this.nameError = "";
    this.priceError = "";
    this.itemImagePath = "";
    this.editingItemId = -1;
  }

  private async deleteItem(id: number): Promise<void> {
    AlertDialog.show({
      title: '确认删除',
      message: '确定要删除该物品记录吗？',
      primaryButton: {
        value: '取消',
        action: () => {}
      },
      secondaryButton: {
        value: '删除',
        fontColor: Color.Red,
        action: async () => {
          try {
            await ItemRdb.delete(id);
            await itemStore.updateItemCount();
            this.itemList = this.itemList.filter(item => item.id !== id);
            setTimeout(async () => {
              await this.refreshItemList();
            }, 100);
            promptAction.showToast({ message: '已删除', duration: 2000 });
          } catch (err) {
            console.error("删除物品失败:", err);
            promptAction.showToast({ message: '删除失败，请重试', duration: 2000 });
          }
        }
      }
    });
  }

  private dataClearedCallback = (): void => {
    console.info('接收到数据清空事件，刷新列表');
    this.refreshItemList();
  };

  private itemCountChangedCallback = (count: number): void => {
    console.info(`物品数量变化: ${count}个，刷新列表`);
    this.refreshItemList();
  };

  @Builder itemEnd(item: Item) {
    Row() {
      Button("编辑")
        .type(ButtonType.Normal)
        .width(80)
        .height("100%")
        .backgroundColor("#4A6CF7")
        .fontColor(Color.White)
        .onClick(() => {
          this.startEdit(item);
        })
      Button("删除")
        .type(ButtonType.Normal)
        .width(80)
        .height("100%")
        .backgroundColor(Color.Red)
        .fontColor(Color.White)
        .onClick(() => {
          this.deleteItem(item.id);
        })
    }
    .padding({ left: 12 })
    .justifyContent(FlexAlign.End)
  }

  private getFilteredItems(): Item[] {
    const keyword = this.searchText.trim().toLowerCase();
    if (!keyword) {
      return this.itemList;
    }

    const filteredItems: Item[] = [];
    for (let i = 0; i < this.itemList.length; i++) {
      const item = this.itemList[i];
      if (item.name.toLowerCase().includes(keyword) ||
      item.category.toLowerCase().includes(keyword) ||
      item.location.toLowerCase().includes(keyword) ||
      item.note.toLowerCase().includes(keyword) ||
      item.useStatus.toLowerCase().includes(keyword)) {
        filteredItems.push(item);
      }
    }
    return filteredItems;
  }

  private getStatusColor(status: ItemUseStatus): string {
    switch (status) {
      case '在用': return "#34C759";
      case '闲置': return "#FF9500";
      case '已丢弃': return "#8E8E93";
      default: return "#333333";
    }
  }

  @Builder TabBuilder(title: string, index: number, icon: string) {
    Column() {
      Text(icon)
        .fontSize(22)
        .margin({ bottom: 4 })
        .opacity(this.currentTab === index ? 1 : 0.5)
        .animation({ duration: 300 })
      Text(title)
        .fontSize(10)
        .fontColor(this.currentTab === index ? "#4A6CF7" : "#999999")
        .fontWeight(this.currentTab === index ? FontWeight.Bold : FontWeight.Normal)
        .animation({ duration: 300 })
    }
    .justifyContent(FlexAlign.Center)
    .width('100%')
    .height('100%')
    .backgroundColor(Color.White)
  }

  private getCategoryColor(category: string): string {
    const colors: Record<string, string> = {
      "数码产品": "#007AFF",
      "衣物": "#FF9500",
      "书籍": "#5856D6",
      "厨具": "#5AC8FA",
      "家具": "#FFCC00",
      "美妆": "#FF2D55",
      "食品": "#34C759",
      "其他": "#8E8E93"
    };
    return colors[category] || "#4A6CF7";
  }

  private getStatusBgColor(status: ItemUseStatus): string {
    switch (status) {
      case '在用': return "#E6F4EA";
      case '闲置': return "#FFF0E0";
      case '已丢弃': return "#F2F2F7";
      default: return "#F5F5F5";
    }
  }

  build() {
    Column() {
      Row() {
        Text(this.editingItemId !== -1 ? "编辑物品" : "轻物志")
          .fontSize(20)
          .fontWeight(FontWeight.Bold)
          .fontColor(Color.White)
      }
      .width("100%")
      .height(44)
      .linearGradient({
        angle: 90,
        colors: [[0x4A6CF7, 0.0], [0x4E54C8, 1.0]]
      })
      .padding({ left: 16, right: 16 })
      .alignItems(VerticalAlign.Center)
      .justifyContent(FlexAlign.Center)

      Tabs({ index: this.currentTab, barPosition: BarPosition.End }) {
        TabContent() {
          Scroll() {
            Column() {
              FormCard({ title: "物品名称 *" }) {
                Column() {
                  TextInput({ placeholder: "请输入物品名称", text: this.itemName })
                    .fontSize(16)
                    .width("100%")
                    .height(48)
                    .backgroundColor(Color.Transparent)
                    .onChange((value: string) => {
                      this.itemName = value;
                      this.validateForm();
                    })

                  if (this.nameError) {
                    Text(this.nameError)
                      .fontSize(12)
                      .fontColor("#F53F3F")
                      .margin({ top: 4 })
                  }
                }
              }

              FormCard({ title: "物品分类" }) {
                Select(this.getCategoryOptions())
                  .value(this.itemCategory || "请选择分类")
                  .font({ size: 16 })
                  .fontColor(this.itemCategory ? "#333333" : "#999999")
                  .selectedOptionFont({ size: 16 })
                  .optionFont({ size: 16 })
                  .width("100%")
                  .height(48)
                  .onSelect((index: number, value: string) => {
                    this.itemCategory = value;
                  })
              }

              FormCard({ title: "购买日期" }) {
                Row() {
                  Text(this.dateStr)
                    .fontSize(16)
                    .fontColor("#333333")
                  Blank()
                  Text("📅")
                    .fontSize(20)
                }
                .width("100%")
                .height(40)
                .alignItems(VerticalAlign.Center)
                .onClick(() => {
                  this.showDatePicker();
                })
              }

              FormCard({ title: "价格（元）*" }) {
                Column() {
                  TextInput({ placeholder: "0.00", text: this.itemPrice })
                    .fontSize(16)
                    .fontColor("#333333")
                    .width("100%")
                    .height(48)
                    .backgroundColor(Color.Transparent)
                    .type(InputType.Number)
                    .onChange((value: string) => {
                      const filtered = value.replace(/[^0-9.]/g, '');
                      const parts = filtered.split('.');
                      if (parts.length > 2) {
                        this.itemPrice = parts[0] + '.' + parts.slice(1).join('');
                      } else {
                        this.itemPrice = filtered;
                      }
                      this.validateForm();
                    })

                  if (this.priceError) {
                    Text(this.priceError)
                      .fontSize(12)
                      .fontColor("#F53F3F")
                      .margin({ top: 4 })
                  }
                }
              }

              FormCard({ title: "存放位置" }) {
                TextInput({ placeholder: "例如：衣柜第一层", text: this.itemLocation })
                  .fontSize(16)
                  .fontColor("#333333")
                  .width("100%")
                  .height(48)
                  .backgroundColor(Color.Transparent)
                  .onChange((value: string) => {
                    this.itemLocation = value;
                  })
              }

              FormCard({ title: "使用状态" }) {
                Flex({ wrap: FlexWrap.Wrap }) {
                  ForEach(this.itemUseStatusOptions, (status: ItemUseStatus) => {
                    Row() {
                      Radio({ value: status, group: "itemUseStatus" })
                        .checked(this.itemUseStatus === status)
                        .onChange((isChecked: boolean) => {
                          if (isChecked) {
                            this.itemUseStatus = status;
                          }
                        })
                      Text(status)
                        .fontSize(14)
                        .fontColor("#333333")
                        .onClick(() => {
                          this.itemUseStatus = status;
                        })
                    }
                    .margin({ right: 16, bottom: 8 })
                  })
                }
              }

              FormCard({ title: "物品图片" }) {
                Column() {
                  if (this.itemImagePath) {
                    Stack() {
                      Image(this.itemImagePath)
                        .width("100%")
                        .height(200)
                        .objectFit(ImageFit.Cover)
                        .borderRadius(8)

                      Button("🗑️")
                        .width(32)
                        .height(32)
                        .borderRadius(16)
                        .backgroundColor(Color.Red)
                        .fontColor(Color.White)
                        .position({ right: 8, top: 8 })
                        .onClick(() => this.removeItemImage())
                    }
                  } else {
                    Column() {
                      Text("📷")
                        .fontSize(40)
                        .margin({ bottom: 8 })
                      Text("点击上传物品图片（可选）")
                        .fontSize(14)
                        .fontColor("#999999")
                    }
                    .width("100%")
                    .height(200)
                    .backgroundColor("#F5F7FA")
                    .borderRadius(8)
                    .justifyContent(FlexAlign.Center)
                    .alignItems(HorizontalAlign.Center)
                    .onClick(() => this.selectItemImage())
                  }
                }
              }

              FormCard({ title: "备注" }) {
                TextArea({ placeholder: "写点什么...", text: this.itemNote })
                  .fontSize(14)
                  .width("100%")
                  .height(80)
                  .backgroundColor("#F5F7FA")
                  .borderRadius(8)
                  .onChange((value: string) => {
                    this.itemNote = value;
                  })
              }

              Button(this.editingItemId !== -1 ? "保存修改" : "添加物品")
                .width("100%")
                .height(50)
                .linearGradient({
                  angle: 90,
                  colors: [[0x4A6CF7, 0.0], [0x4E54C8, 1.0]]
                })
                .fontColor(Color.White)
                .fontSize(18)
                .fontWeight(FontWeight.Bold)
                .borderRadius(25)
                .shadow({ radius: 8, color: "#4D4A6CF7", offsetY: 4 })
                .onClick(async () => await this.addItem())
                .margin({
                  top: 16,
                  bottom: this.editingItemId !== -1 ? 16 : 32
                })

              if (this.editingItemId !== -1) {
                Button("取消修改")
                  .width("100%")
                  .height(50)
                  .backgroundColor("#F0F2F5")
                  .fontColor("#666666")
                  .fontSize(18)
                  .fontWeight(FontWeight.Bold)
                  .borderRadius(25)
                  .onClick(() => {
                    this.resetForm();
                  })
                  .margin({ bottom: 32 })
              }
            }
            .padding(16)
          }
          .width("100%")
          .height("100%")
          .backgroundColor("#F9FAFB")
        }
        .tabBar(this.TabBuilder("添加", 0, "➕"))

        TabContent() {
          Column() {
            Row() {
              Search({ value: this.searchText, placeholder: '搜索物品名称、分类、位置' })
                .layoutWeight(1)
                .height(40)
                .backgroundColor(Color.White)
                .placeholderColor('#999999')
                .placeholderFont({ size: 14 })
                .textFont({ size: 14 })
                .onChange((value: string) => {
                  this.searchText = value;
                })
            }
            .width("100%")
            .padding({ left: 16, right: 16, top: 12, bottom: 8 })

            if (this.getFilteredItems().length === 0) {
              Column() {
                Text("📦")
                  .fontSize(60)
                  .margin({ bottom: 16 })
                Text("还没有添加物品哦，快去添加吧")
                  .fontSize(14)
                  .fontColor("#999999")
              }
              .width("100%")
              .height("60%")
              .justifyContent(FlexAlign.Center)
              .alignItems(HorizontalAlign.Center)
            } else {
              List({ space: 0 }) {
                ForEach(this.getFilteredItems(), (item: Item) => {
                  ListItem() {
                    Row() {
                      Column() {
                          Stack() {
                            Text(item.category.substring(0, 1))
                              .fontSize(18)
                              .fontWeight(FontWeight.Bold)
                              .fontColor(Color.White)

                        }
                      }
                      .width(40)
                      .height(40)
                      .borderRadius(20)
                      .backgroundColor(this.getCategoryColor(item.category))
                      .justifyContent(FlexAlign.Center)
                      .alignItems(HorizontalAlign.Center)
                      .margin({ right: 12 })

                      Column({ space: 4 }) {
                        Row({ space: 8 }) {
                          Text(item.name)
                            .fontSize(16)
                            .fontWeight(FontWeight.Medium)
                            .fontColor("#333333")
                            .maxLines(1)
                            .textOverflow({ overflow: TextOverflow.Ellipsis })
                            .layoutWeight(1)

                          Text(item.useStatus)
                            .fontSize(10)
                            .fontColor(this.getStatusColor(item.useStatus))
                            .backgroundColor(this.getStatusBgColor(item.useStatus))
                            .padding({ left: 4, right: 4, top: 2, bottom: 2 })
                            .borderRadius(4)
                            .flexShrink(0)
                        }
                        .width("100%")

                        Text(`${item.buyDate} · ¥${item.price.toFixed(2)}`)
                          .fontSize(12)
                          .fontColor("#999999")
                          .width("100%")
                          .maxLines(1)
                          .textOverflow({ overflow: TextOverflow.Ellipsis })

                        if (item.location) {
                          Text(`位置：${item.location}`)
                            .fontSize(12)
                            .fontColor("#666666")
                            .width("100%")
                            .maxLines(1)
                            .textOverflow({ overflow: TextOverflow.Ellipsis })
                        }

                        if (item.note) {
                          Text(item.note)
                            .fontSize(12)
                            .fontColor("#666666")
                            .width("100%")
                            .maxLines(1)
                            .textOverflow({ overflow: TextOverflow.Ellipsis })
                            .margin({ top: 4 })
                        }
                      }
                      .layoutWeight(1)
                    }
                    .width("100%")
                    .padding(16)
                    .backgroundColor(Color.White)
                    .borderRadius(12)
                    .shadow({ radius: 4, color: "#0D000000", offsetY: 2 })
                    .onClick(() => this.previewImage(item))
                  }
                  .swipeAction({ end: this.itemEnd(item) })
                  .margin({ bottom: 12 })
                }, (item: Item) => item.id.toString())
              }
              .width("100%")
              .layoutWeight(1)
              .padding({ left: 16, right: 16, bottom: 16 })
              .scrollBar(BarState.Auto)
            }
          }
          .width("100%")
          .height("100%")
          .backgroundColor("#F9FAFB")
        }
        .tabBar(this.TabBuilder("物品列表", 1, "📋"))

        TabContent() {
          StatisticsPage({ itemList: this.itemList })
        }
        .tabBar(this.TabBuilder("统计", 2, "📊"))

        TabContent() {
          MinePage()
        }
        .tabBar(this.TabBuilder("我的", 3, "👤"))
      }

      .barWidth("100%")
      .barHeight(60)
      .backgroundColor(Color.White)
      .layoutWeight(1)
      .onChange((index: number) => {
        this.currentTab = index;
      })


      if (this.showImagePreview) {
        Stack({ alignContent: Alignment.Center }) {
          // 背景遮罩
          Column()
            .width('100%')
            .height('100%')
            .backgroundColor('rgba(0,0,0,0.85)')
            .zIndex(1);

          // 关闭按钮（最高层级 - 优化定位）
          Button('X')
            .width(60)
            .height(40)
            .borderRadius(20)
            .backgroundColor(Color.Red)
            .fontColor(Color.White)
            .fontSize(18)
            .fontWeight(FontWeight.Bold)
            .zIndex(3)
            // 关键修改：改用 right/top 固定内边距，而非百分比x
            .position({ right: 16, top: 24 })
            .onClick(() => {
              console.log('关闭按钮点击，更新状态');
              this.showImagePreview = false;
              this.previewImagePath = '';
              this.imageScale = 1;
              this.imageOffsetX = 0;
              this.imageOffsetY = 0;
            });

          // 图片预览
          Image(this.previewImagePath)
            .width('90%')
            .height('70%')
            .objectFit(ImageFit.Contain)
            .borderRadius(8)
            .scale({ x: this.imageScale, y: this.imageScale })
            .translate({ x: this.imageOffsetX, y: this.imageOffsetY })
            .zIndex(2)
            .gesture(
              PinchGesture()
                .onActionUpdate((event: GestureEvent) => {
                  if (event.scale) {
                    this.imageScale = Math.max(0.5, Math.min(3, this.imageScale * event.scale));
                  }
                })
            )
            .gesture(
              PanGesture()
                .onActionUpdate((event: GestureEvent) => {
                  if (event.offsetX && event.offsetY) {
                    this.imageOffsetX += event.offsetX;
                    this.imageOffsetY += event.offsetY;
                  }
                })
            );
        }
        .width('100%')
        .height('100%')
        .position({ left: 0, top: 0 })
        .zIndex(999);
      }
    }
    .width("100%")
    .height("100%")
    .backgroundColor("#F9FAFB")
  }
}