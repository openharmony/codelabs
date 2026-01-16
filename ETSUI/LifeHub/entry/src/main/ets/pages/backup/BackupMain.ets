/**
 * 数据备份和恢复页面
 */
import { LifeHubData, HealthRecord, Goal, MoodRecord, Skill, TravelPlan, Diary } from '../../model/LifeHubData';

interface BackupData {
  healthRecords: HealthRecord[];
  goals: Goal[];
  moodRecords: MoodRecord[];
  skills: Skill[];
  travelPlans: TravelPlan[];
  diaries: Diary[];
}

interface Backup {
  version: string;
  timestamp: number;
  data: BackupData;
}

@Entry
@Component
export struct BackupMain {
  private lifeHubData: LifeHubData = new LifeHubData();
  
  @State backupData: string = '';
  @State isBackingUp: boolean = false;
  @State isRestoring: boolean = false;
  @State lastBackupTime: string = '从未备份';
  @State restoreText: string = '';

  aboutToAppear() {
    const context = getContext(this);
    if (context) {
      this.lifeHubData.setContext(context);
    }
    this.loadLastBackupTime();
  }

  async loadLastBackupTime() {
    // TODO: 从本地存储加载最后备份时间
  }

  build() {
    Column() {
      // 标题栏
      Row() {
        Text('数据备份')
          .fontSize(24)
          .fontWeight(FontWeight.Bold)
        
        Blank()
      }
      .width('100%')
      .padding(20)
      .backgroundColor('#f0f0f0')

      Scroll() {
        Column() {
          // 备份功能
          Text('数据备份')
            .fontSize(18)
            .fontWeight(FontWeight.Bold)
            .width('100%')
            .textAlign(TextAlign.Start)
            .padding({ left: 20, top: 20, bottom: 10 })

          Column() {
            Row() {
              Column() {
                Text('📦')
                  .fontSize(40)
                  .margin({ bottom: 10 })
                Text('备份所有数据')
                  .fontSize(16)
                  .fontWeight(FontWeight.Medium)
                  .fontColor('#333')
                Text('将数据导出为JSON格式')
                  .fontSize(14)
                  .fontColor('#666')
                  .margin({ top: 5 })
                Text(`最后备份: ${this.lastBackupTime}`)
                  .fontSize(12)
                  .fontColor('#999')
                  .margin({ top: 10 })
              }
              .alignItems(HorizontalAlign.Center)
              .width('100%')
            }
            .width('100%')
            .padding(20)

            Button('开始备份')
              .width('90%')
              .height(50)
              .fontSize(16)
              .fontWeight(FontWeight.Bold)
              .backgroundColor('#007DFF')
              .fontColor(Color.White)
              .borderRadius(25)
              .enabled(!this.isBackingUp)
              .onClick(() => {
                this.performBackup();
              })
              .margin({ top: 10, bottom: 20 })
          }
          .width('100%')
          .backgroundColor(Color.White)
          .borderRadius(12)
          .margin({ left: 15, right: 15, bottom: 20 })

          // 恢复功能
          Text('数据恢复')
            .fontSize(18)
            .fontWeight(FontWeight.Bold)
            .width('100%')
            .textAlign(TextAlign.Start)
            .padding({ left: 20, top: 20, bottom: 10 })

          Column() {
            Text('⚠️ 警告：恢复数据将覆盖当前所有数据，请谨慎操作！')
              .fontSize(14)
              .fontColor('#FF5722')
              .width('100%')
              .textAlign(TextAlign.Start)
              .padding(15)
              .backgroundColor('#FFF3E0')
              .borderRadius(8)
              .margin({ bottom: 15 })

            Text('粘贴备份数据（JSON格式）')
              .fontSize(14)
              .fontColor('#666')
              .width('100%')
              .textAlign(TextAlign.Start)
              .margin({ bottom: 10 })

            TextArea({ 
              placeholder: '在此粘贴备份的JSON数据...', 
              text: this.restoreText 
            })
              .width('100%')
              .height(200)
              .fontSize(14)
              .padding(15)
              .backgroundColor('#f8f8f8')
              .borderRadius(8)
              .onChange((value: string) => {
                this.restoreText = value;
              })

            Button('开始恢复')
              .width('90%')
              .height(50)
              .fontSize(16)
              .fontWeight(FontWeight.Bold)
              .backgroundColor('#FF5722')
              .fontColor(Color.White)
              .borderRadius(25)
              .enabled(!this.isRestoring && this.restoreText.length > 0)
              .onClick(() => {
                this.performRestore();
              })
              .margin({ top: 15, bottom: 20 })
          }
          .width('100%')
          .backgroundColor(Color.White)
          .borderRadius(12)
          .margin({ left: 15, right: 15, bottom: 20 })

          // 备份数据预览
          if (this.backupData.length > 0) {
            Column() {
              Row() {
                Text('备份数据预览')
                  .fontSize(16)
                  .fontWeight(FontWeight.Medium)
                  .fontColor('#333')
                
                Blank()
                
                Button('复制')
                  .fontSize(14)
                  .height(32)
                  .backgroundColor('#007DFF')
                  .fontColor(Color.White)
                  .onClick(() => {
                    // TODO: 复制到剪贴板
                  })
              }
              .width('100%')
              .margin({ bottom: 10 })

              Text(this.backupData)
                .fontSize(12)
                .fontColor('#666')
                .width('100%')
                .maxLines(10)
                .textOverflow({ overflow: TextOverflow.Ellipsis })
                .padding(15)
                .backgroundColor('#f8f8f8')
                .borderRadius(8)
            }
            .width('100%')
            .padding(15)
            .backgroundColor(Color.White)
            .borderRadius(12)
            .margin({ left: 15, right: 15, bottom: 20 })
          }

          // 使用说明
          Column() {
            Text('📖 使用说明')
              .fontSize(16)
              .fontWeight(FontWeight.Bold)
              .width('100%')
              .textAlign(TextAlign.Start)
              .margin({ bottom: 15 })

            Text('1. 定期备份数据可以防止数据丢失')
              .fontSize(14)
              .fontColor('#666')
              .width('100%')
              .textAlign(TextAlign.Start)
              .margin({ bottom: 10 })

            Text('2. 备份数据为JSON格式，可以保存到安全的地方')
              .fontSize(14)
              .fontColor('#666')
              .width('100%')
              .textAlign(TextAlign.Start)
              .margin({ bottom: 10 })

            Text('3. 恢复数据前请确保备份数据格式正确')
              .fontSize(14)
              .fontColor('#666')
              .width('100%')
              .textAlign(TextAlign.Start)
              .margin({ bottom: 10 })

            Text('4. 恢复操作不可逆，请谨慎操作')
              .fontSize(14)
              .fontColor('#FF5722')
              .width('100%')
              .textAlign(TextAlign.Start)
          }
          .width('100%')
          .padding(20)
          .backgroundColor('#FFF9C4')
          .borderRadius(12)
          .margin({ left: 15, right: 15, bottom: 20 })
        }
        .width('100%')
      }
      .layoutWeight(1)
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#f5f5f5')
  }

  async performBackup() {
    this.isBackingUp = true;
    try {
      // 获取所有数据
      const healthRecords = await this.lifeHubData.getHealthRecords(undefined, 10000);
      const goals = await this.lifeHubData.getAllGoals();
      const moodRecords = await this.lifeHubData.getMoodRecords(10000);
      const skills = await this.lifeHubData.getAllSkills();
      const travelPlans = await this.lifeHubData.getAllTravelPlans();
      const diaries = await this.lifeHubData.getAllDiaries();

      const backup: Backup = {
        version: '1.0',
        timestamp: Date.now(),
        data: {
          healthRecords,
          goals,
          moodRecords,
          skills,
          travelPlans,
          diaries
        }
      };

      this.backupData = JSON.stringify(backup, null, 2);
      this.lastBackupTime = new Date().toLocaleString();
      
      // TODO: 保存到本地文件或分享
      console.log('备份完成:', this.backupData);
    } catch (error) {
      console.error('BackupMain: 备份失败:', error);
    } finally {
      this.isBackingUp = false;
    }
  }

  async performRestore() {
    if (!this.restoreText) return;

    this.isRestoring = true;
    try {
      const backup = JSON.parse(this.restoreText) as Backup;
      
      if (!backup.data) {
        throw new Error('备份数据格式错误');
      }

      // TODO: 实现数据恢复逻辑
      // 1. 清除现有数据
      // 2. 导入备份数据
      
      console.log('恢复完成');
    } catch (error) {
      console.error('BackupMain: 恢复失败:', error);
    } finally {
      this.isRestoring = false;
    }
  }
}
