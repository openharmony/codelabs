import router from '@ohos.router';
import { Routes } from '../constants/Routes';

@Entry
@Component
struct Index {
  onPageShow() {
    // 自动跳转到列表页
    router.replaceUrl({ url: Routes.CONTACT_LIST })
  }

  build() {
    Row() {
      Column() {
        Text('正在加载联系人...')
          .fontSize(20)
      }
      .width('100%')
    }
    .height('100%')
  }
}