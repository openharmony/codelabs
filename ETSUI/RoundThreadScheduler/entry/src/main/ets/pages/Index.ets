import { SharedState } from '../model/SharedState'
import { RoundScheduler } from '../scheduler/RoundScheduler'

@Entry
@Component
struct Index {
  private shared: SharedState = new SharedState()
  private scheduler: RoundScheduler = new RoundScheduler(this.shared)

  build() {
    Column() {
      Text('ArkTS 多线程安全调度示例')
        .fontSize(22)
        .margin(20)

      Button('启动线程调度')
        .onClick(() => {
          this.scheduler.start()
        })

      Text(`当前结果: ${this.shared.getValue()}`)
        .margin(20)
    }
  }
}
