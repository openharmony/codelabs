import { FOOD_DATABASE, FoodCategory, FoodItem, MealTime, getFoodsByCategory } from '../../model/DietModel';
import promptAction from '@ohos.promptAction';

@CustomDialog
export struct FoodSelectorDialog {
  controller: CustomDialogController;
  onConfirm: (item: FoodItem, count: number, meal: MealTime) => void = () => {};

  @State currentCategoryIndex: number = 0;
  @State selectedMeal: MealTime = MealTime.SNACK;
  @State expandedId: number = -1;
  @State count: number = 1;

  // 1. 引入全局字体缩放
  @StorageProp('appFontScale') fontScale: number = 1.0;

  @State keyword: string = '';
  @State searchResults: FoodItem[] = [];

  @State customName: string = ''
  @State customCalories: string = ''
  @State customUnit: string = '份'

  private categories: FoodCategory[] = [
    FoodCategory.STAPLE, FoodCategory.MEAT, FoodCategory.VEGETABLE,
    FoodCategory.FRUIT, FoodCategory.DRINK, FoodCategory.SNACK, FoodCategory.CUSTOM
  ];

  private mealTypes: MealTime[] = [
    MealTime.BREAKFAST, MealTime.LUNCH, MealTime.DINNER, MealTime.SNACK
  ];

  aboutToAppear() {
    this.selectedMeal = this.guessMealTime();
  }

  guessMealTime(): MealTime {
    const hour = new Date().getHours();
    if (hour >= 5 && hour < 10) return MealTime.BREAKFAST;
    if (hour >= 10 && hour < 15) return MealTime.LUNCH;
    if (hour >= 15 && hour < 20) return MealTime.DINNER;
    return MealTime.SNACK;
  }

  handleSearch(value: string) {
    this.keyword = value;
    if (value.trim() === '') {
      this.searchResults = [];
    } else {
      this.searchResults = FOOD_DATABASE.filter(item => item.name.includes(value));
    }
    this.expandedId = -1;
  }

  build() {
    Column() {
      // 1. 标题栏
      Row() {
        Text('添加饮食')
          .fontSize(20 * this.fontScale)
          .fontWeight(FontWeight.Bold)
        Blank()
        Text('关闭')
          .fontSize(14 * this.fontScale)
          .fontColor('#999')
          .onClick(() => this.controller.close())
      }.width('100%').padding(16)

      // 2. 餐别选择
      Row({ space: 10 }) {
        ForEach(this.mealTypes, (meal: MealTime) => {
          Text(meal)
            .fontSize(12 * this.fontScale)
            .fontColor(this.selectedMeal === meal ? Color.White : '#007DFF')
            .backgroundColor(this.selectedMeal === meal ? '#007DFF' : '#F0F7FF')
            .padding({
              left: 12,
              right: 12,
              top: 6,
              bottom: 6
            })
            .borderRadius(14)
            .onClick(() => this.selectedMeal = meal)
        })
      }.width('100%').padding({
        left: 16,
        right: 16,
        bottom: 10
      }).justifyContent(FlexAlign.SpaceBetween)

      // 3. 【核心修复】搜索栏适配
      Search({
        placeholder: '搜索食物名称...',
        value: this.keyword
      })
        .width('90%')
        // 高度也需要随字体适度变大，否则字大了框装不下
        .height(40 * this.fontScale)
        .margin({ bottom: 10 })
        // 【关键】设置输入文字大小
        .textFont({ size: 14 * this.fontScale })
        // 【关键】设置提示文字（placeholder）大小
        .placeholderFont({ size: 14 * this.fontScale })
        .onChange((value) => {
          this.handleSearch(value);
        })

      Divider()

      // 4. 内容区域
      if (this.keyword.length > 0) {
        if (this.searchResults.length === 0) {
          Column() {
            Image($r('app.media.icon'))
              .width(60)
              .opacity(0.2)
              .grayscale(1)
              .margin({top: 50})
            Text('未找到相关食物')
              .fontSize(14 * this.fontScale)
              .fontColor('#999')
              .margin({ top: 10 })
          }
          .height(450)
          .width('100%')
        } else {
          this.FoodListBuilder(this.searchResults)
        }
      } else {
        Tabs({ index: this.currentCategoryIndex }) {
          ForEach(this.categories, (cat: FoodCategory, index: number) => {
            TabContent() {
              if (cat === FoodCategory.CUSTOM) {
                this.CustomInputForm()
              } else {
                this.FoodListBuilder(getFoodsByCategory(cat))
              }
            }
            .tabBar(this.TabItem(cat, index))
          })
        }
        .barMode(BarMode.Scrollable)
        .height(450)
        .onChange((index) => {
          this.currentCategoryIndex = index
          this.expandedId = -1;
        })
      }
    }
    .backgroundColor(Color.White).borderRadius(24)
  }

  @Builder
  TabItem(name: string, index: number) {
    Text(name)
      .fontSize((this.currentCategoryIndex === index ? 16 : 14) * this.fontScale)
      .fontColor(this.currentCategoryIndex === index ? '#007DFF' : '#666')
      .fontWeight(this.currentCategoryIndex === index ? FontWeight.Bold : FontWeight.Normal)
      .padding(10)
  }

  @Builder
  FoodListBuilder(items: FoodItem[]) {
    List() {
      ForEach(items, (item: FoodItem) => {
        ListItem() {
          Column() {
            // 常规行
            Row() {
              Column({ space: 4 }) {
                Text(item.name)
                  .fontSize(16 * this.fontScale)
                  .fontColor('#333')

                if (this.keyword.length > 0) {
                  Text(`${item.category} | 单位: ${item.unit}`)
                    .fontSize(10 * this.fontScale)
                    .fontColor('#999')
                } else {
                  Text(`单位: ${item.unit}`)
                    .fontSize(10 * this.fontScale)
                    .fontColor('#999')
                }
              }.alignItems(HorizontalAlign.Start)
              Blank()
              Text(`${item.calories} kcal`)
                .fontSize(16 * this.fontScale)
                .fontWeight(this.expandedId === item.id ? FontWeight.Bold : FontWeight.Normal)
                .fontColor(this.expandedId === item.id ? '#007DFF' : '#333')
            }
            .width('100%')
            .padding({
              left: 16,
              right: 16,
              top: 12,
              bottom: 12
            })
            .backgroundColor(this.expandedId === item.id ? '#F0F7FF' : Color.White)
            .onClick(() => {
              animateTo({
                duration: 300,
                curve: Curve.FastOutSlowIn
              },
                () => {
                if (this.expandedId === item.id) {
                  this.expandedId = -1;
                } else {
                  this.expandedId = item.id;
                  this.count = 1;
                }
              })
            })

            // 展开内容
            if (this.expandedId === item.id) {
              Column() {
                Divider().color('#E6F7FF')
                Row() {
                  NutritionPieChart({
                    protein: item.protein,
                    fat: item.fat,
                    carbs: item.carbs
                  })
                    .width(80)
                    .height(80)

                  Column({ space: 8 }) {
                    this.NutrientText(
                      '蛋白质',
                      item.protein,
                      '#FF4D4F'
                    )
                    this.NutrientText(
                      '脂肪',
                      item.fat,
                      '#FAAD14'
                    )
                    this.NutrientText(
                      '碳水',
                      item.carbs,
                      '#007DFF'
                    )
                  }
                  .layoutWeight(1)
                  .padding({ left: 20 })
                  .alignItems(HorizontalAlign.Start)

                  Column() {
                    Text(`${Math.round(item.calories * this.count)}`)
                      .fontSize(24 * this.fontScale)
                      .fontWeight(FontWeight.Bold)
                      .fontColor('#007DFF')
                    Text('kcal')
                      .fontSize(12 * this.fontScale)
                      .fontColor('#999')
                  }
                }
                .width('100%')
                .padding(16)

                Row({ space: 10 }) {
                  Text('份量')
                    .fontSize(14 * this.fontScale)
                    .fontColor('#666')
                  Counter() {
                    Text(this.count.toFixed(1))
                      .fontSize(14 * this.fontScale)
                      .fontWeight(FontWeight.Bold)
                  }
                  .onInc(() => {
                    if (this.count < 10) {
                      this.count += 0.5
                    }
                  })
                  .onDec(() => {
                    if (this.count > 0.5) {
                      this.count -= 0.5
                    }
                  })
                  .height(30)

                  Blank()

                  Button('添加')
                    .height(32)
                    .fontSize(14 * this.fontScale)
                    .backgroundColor('#007DFF')
                    .onClick(() => {
                      this.onConfirm(
                        item,
                        this.count,
                        this.selectedMeal
                      );
                      this.controller.close();
                    })
                }
                .width('100%')
                .padding({
                  left: 16,
                  right: 16,
                  bottom: 16
                })
              }
              .backgroundColor('#F0F7FF')
            }
          }
        }
      })
    }
    .width('100%')
    .height('100%')
  }

  @Builder
  NutrientText(
    label: string,
    val: number,
    color: string
  ) {
    Row() {
      Circle({
        width: 6,
        height: 6
      })
        .fill(color)
        .margin({ right: 6 })
      Text(`${label} ${val}g`)
        .fontSize(12 * this.fontScale)
        .fontColor('#666')
    }
  }

  @Builder
  CustomInputForm() {
    Column({ space: 15 }) {
      Text('手动输入食物信息')
        .fontSize(14 * this.fontScale)
        .fontColor('#999')
        .alignSelf(ItemAlign.Start)
      TextInput({
        placeholder: '食物名称',
        text: this.customName
      })
        .onChange(v => this.customName = v)
        .height(45)
      Row({ space: 10 }) {
        TextInput({
          placeholder: '热量 (kcal)',
          text: this.customCalories
        })
          .type(InputType.Number)
          .onChange(v => this.customCalories = v)
          .layoutWeight(1)
          .height(45)
        TextInput({
          placeholder: '单位',
          text: this.customUnit
        })
          .onChange(v => this.customUnit = v)
          .layoutWeight(1)
          .height(45)
      }
      Button('确认添加')
        .width('100%')
        .margin({ top: 10 })
        .onClick(() => this.handleCustomAdd())
    }
    .padding(20)
    .height('100%')
    .justifyContent(FlexAlign.Start)
  }

  handleCustomAdd() {
    if (!this.customName || !this.customCalories) {
      promptAction.showToast({ message: '请输入名称和热量' });
      return;
    }
    const customItem: FoodItem = {
      id: new Date().getTime(),
      name: this.customName,
      calories: Number(this.customCalories),
      unit: this.customUnit || '份',
      category: FoodCategory.CUSTOM,
      protein: 0,
      fat: 0,
      carbs: 0
    };
    this.onConfirm(
      customItem,
      1,
      this.selectedMeal
    );
    this.controller.close();
  }
}

@Component
struct NutritionPieChart {
  @Prop protein: number;
  @Prop fat: number;
  @Prop carbs: number;
  private settings: RenderingContextSettings = new RenderingContextSettings(true);
  private context: CanvasRenderingContext2D = new CanvasRenderingContext2D(this.settings);
  build() {
    Canvas(this.context)
      .width('100%')
      .height('100%')
      .onReady(() => this.drawPie())
  }
  drawPie() {
    const total = this.protein + this.fat + this.carbs;
    if (total <= 0) {
      this.drawSlice(
        this.context.width / 2,
        this.context.height / 2,
        this.context.width / 2,
        0,
        2 * Math.PI, '#EEE'
      );
      return;
    }
    const cx = this.context.width / 2,
      cy = this.context.height / 2,
      r = Math.min(cx, cy);
    let startAngle = -0.5 * Math.PI;

    const pAngle = (this.protein / total) * 2 * Math.PI;
    this.drawSlice(
      cx,
      cy,
      r,
      startAngle,
      startAngle + pAngle,
      '#FF4D4F'
    );
    startAngle += pAngle;

    const fAngle = (this.fat / total) * 2 * Math.PI;
    this.drawSlice(
      cx,
      cy,
      r,
      startAngle,
      startAngle + fAngle,
      '#FAAD14'
    );
    startAngle += fAngle;

    const cAngle = (this.carbs / total) * 2 * Math.PI;
    this.drawSlice(
      cx,
      cy,
      r,
      startAngle,
      startAngle + cAngle,
      '#007DFF'
    );

    this.context.beginPath(); this.context.arc(
      cx,
      cy,
      r * 0.6,
      0,
      2 * Math.PI
    );
    this.context.fillStyle = '#F0F7FF';
    this.context.fill();
  }
  drawSlice(
    cx: number,
    cy: number,
    r: number,
    start: number,
    end: number,
    color: string
  ) {
    this.context.beginPath();
    this.context.moveTo(cx, cy);
    this.context.arc(cx, cy, r, start, end);
    this.context.closePath();
    this.context.fillStyle = color;
    this.context.fill();
  }
}