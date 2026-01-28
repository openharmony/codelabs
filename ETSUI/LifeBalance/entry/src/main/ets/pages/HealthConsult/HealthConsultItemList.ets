import { HealthConsultWithFavoriteData, queryHealthConsultItem } from "../../model/HealthConsultModel";
import HealthConsultItem from "./HealthConsultItem";
import NoMoreLayout from "./NoMoreLayout";
import router from '@ohos.router';

/**
 * The health list component.
 */
@Component
export default struct HealthConsultItemList {
  @State itemArray: HealthConsultWithFavoriteData[] = [];
  @Watch('changeContent') @Link searchText: string;
  @Watch('changeContent') @Link forceReload: boolean;
  currentTabIndex: number = 0;

  // 1. 【新增】引入全局字体缩放
  @StorageProp('appFontScale') fontScale: number = 1.0;

  async changeContent() {
    this.itemArray = await queryHealthConsultItem(this.currentTabIndex, this.searchText);
    this.forceReload = false;
  }

  async aboutToAppear() {
    // Request health consult data.
    await this.changeContent();
  }

  build() {
    Column() {
      if (this.itemArray.length > 0) {
        this.ListLayout();
      } else {
        this.renderEmptyState();
      }
    }
    .width('100%')
    .height('100%')
    .justifyContent(FlexAlign.Center)
  }

  @Builder ListLayout() {
    List() {
      ForEach(this.itemArray, (item: HealthConsultWithFavoriteData) => {
        ListItem() {
          // 2. 将 fontScale 传给子组件
          HealthConsultItem({
            healthConsultData: item.healthConsultData,
            isFavorite: item.isFavorite,
            fontScale: this.fontScale
          })
            .onClick(e => {
              router.pushUrl({
                url: 'pages/HealthConsult/HealthConsultDetailPage',
                params: {
                  'id': item.healthConsultData.id
                }
              }) // 页面跳转
            })
        }
        .backgroundColor('#fff')
      }, (item: HealthConsultWithFavoriteData, index?: number) => JSON.stringify(item) + index)

      ListItem() {
        NoMoreLayout({ fontScale: this.fontScale })
      }
    }
    .width('95%')
    .height('100%')
    .margin({
      left: '2.5%',
      right: '2.5%'
    })
    .backgroundColor('#F1F3F5')
    .divider({
      color: '#E2E2E2',
      strokeWidth: 0.5,
      endMargin: '3.3%'
    })
  }

  @Builder
  renderEmptyState() {
    Column() {
      Image($r('app.media.icon')).width(60).opacity(0.2)
      // 4. 空状态文字适配
      Text('未找到相关健康知识')
        .fontSize(14 * this.fontScale)
        .fontColor('#999')
        .margin({ top: 10 })
    }
    .width('100%')
    .margin({ top: 100 })
  }
}
