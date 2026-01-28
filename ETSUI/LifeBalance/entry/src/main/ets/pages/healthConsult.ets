import { HealthTabData, HEALTH_TAB_DATA } from '../model/HealthConsultTabModel';
import HealthConsultItemList from './HealthConsult/HealthConsultItemList';

@Component
export struct KnowledgePage {
  @State tabBarArray: HealthTabData[] = HEALTH_TAB_DATA;
  @State searchText: string = '';
  @State currentTabIndex: number = 0;
  @State forceReload: boolean = false;

  // 1. 引入缩放变量
  @StorageProp('appFontScale') fontScale: number = 1.0;

  @Builder TabBuilder(index: number) {
    Column() {
      Text(this.tabBarArray[index].name)
        .height('100%')
        .padding({
          left: '2.2%',
          right: '2.2%'
        })
        .fontSize((this.currentTabIndex === index ? 20 : 16) * this.fontScale)
        .fontWeight(this.currentTabIndex === index ? 700 : 400)
    }
  }

  build() {
    Column() {
      Column() {
        Text('健康咨询')
          .fontSize(24 * this.fontScale)
          .fontWeight(FontWeight.Bold)
          .margin({ bottom: 8 })

        Search({ value:
        this.searchText,
          placeholder: '搜索：饮食、运动...'
        })
          .width('100%')
          .height(45)
          .onChange((v) => {
            this.searchText = v;
          })
      }
      .padding({
        top: 12,
        bottom: 8,
        left: 12,
        right: 12
      })
      .backgroundColor(Color.White)

      // 显示文章列表
      Tabs() {
        ForEach(this.tabBarArray, (tabsItem: HealthTabData) => {
          TabContent() {
            HealthConsultItemList({
              searchText: $searchText,
              forceReload: $forceReload,
              currentTabIndex: tabsItem.id,
            })
          }
          .tabBar(this.TabBuilder(tabsItem.id))
        }, (item: HealthTabData) => item.id.toString());
      }
      .barHeight('7.2%')
      .barMode(BarMode.Scrollable)
      .barWidth('100%')
      .onChange((index: number) => {
        this.currentTabIndex = index;
        this.forceReload = true;
      })
      .vertical(false)
      .layoutWeight(1)
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F1F3F5')
  }
}
