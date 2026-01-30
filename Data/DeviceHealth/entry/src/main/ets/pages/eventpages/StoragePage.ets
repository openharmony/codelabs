import BackContainer from '../../components/BackContainer';
import { DefaultStorageProvider, StorageStatus } from '../../providers/StorageProvider';
import { EventDB } from '../../database/EventDB';
import { EventLog, EventType } from '../../model/EventLog';

@Entry
@Component
struct StoragePage {
  @State freeGB: string = '0';
  @State historyLogs: EventLog[] = [];

  private readonly MOCK_TOTAL_GB: number = 256;
  private storageProvider: DefaultStorageProvider = new DefaultStorageProvider();

  async aboutToAppear() {
    await EventDB.init(getContext(this));
    this.refreshStorageInfo();
    this.refreshHistory();
  }

  // 获取当前存储信息
  async refreshStorageInfo() {
    const status: StorageStatus = await this.storageProvider.getStatus();
    if (status.freeBytes !== null) {
      const gb = status.freeBytes / (1024 * 1024 * 1024);
      this.freeGB = gb.toFixed(2);
    }
  }

  // 获取数据库历史记录
  async refreshHistory() {
    if (EventDB.isReady()) {
      const logs = await EventDB.query({
        type: EventType.STORAGE,
        limit: 15
      });
      this.historyLogs = logs;
    }
  }

  build() {
    Column() {
      BackContainer({ header: "存储管理" })

      // ========== 存储空间环形/条形图区域 ==========
      Column({ space: 20 }) {
        Text("可用存储空间")
          .fontSize(16)
          .fontColor('#666666')

        Stack() {
          // 进度条展示占用情况
          Progress({
            value: this.MOCK_TOTAL_GB - parseFloat(this.freeGB),
            total: this.MOCK_TOTAL_GB,
            type: ProgressType.Ring
          })
            .width(180)
            .height(180)
            .color('#F7B500')

          Column() {
            Text(this.freeGB)
              .fontSize(32)
              .fontWeight(FontWeight.Bold)
            Text("GB 可用")
              .fontSize(14)
              .fontColor('#999999')
          }
        }

        Text(`总空间: ${this.MOCK_TOTAL_GB} GB`)
          .fontSize(14)
          .fontColor('#333')
          .padding({ top: 10 })
      }
      .width('100%')
      .padding(30)
      .backgroundColor(Color.White)
      .margin({ bottom: 10 })

      // ========== 存储历史变更日志 ==========
      Column() {
        Row() {
          Text("存储清理/变更历史")
            .fontSize(16)
            .fontWeight(FontWeight.Bold)
          Blank()
          // 增加一个手动刷新按钮，补偿没有 subscribe 的不足
          Button("刷新")
            .fontSize(12)
            .height(24)
            .backgroundColor('#F7B500')
            .onClick(() => {
              this.refreshStorageInfo();
              this.refreshHistory();
            })
        }
        .width('100%')
        .padding(20)

        List() {
          ForEach(this.historyLogs, (log: EventLog) => {
            ListItem() {
              Row() {
                Image($r('app.media.ic_storage')).width(24).height(24).margin({ right: 12 })
                Column() {
                  Text(log.message).fontSize(14).fontWeight(FontWeight.Medium)
                  Text(new Date(log.timestamp).toLocaleString()).fontSize(12).fontColor('#999')
                }.alignItems(HorizontalAlign.Start)
              }
              .width('100%')
              .padding({ top: 12, bottom: 12, left: 20, right: 20 })
            }
          })
        }
        .divider({ strokeWidth: 0.5, color: '#EEEEEE' })
        .layoutWeight(1)
      }
      .width('100%')
      .backgroundColor(Color.White)
      .borderRadius({ topLeft: 24, topRight: 24 })
      .layoutWeight(1)
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F5F5F5')
  }
}