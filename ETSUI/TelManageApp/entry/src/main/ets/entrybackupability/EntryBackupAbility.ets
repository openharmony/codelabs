// 提供日志记录功能
import {hilog} from '@kit.PerformanceAnalysisKit';
// 提供备份扩展能力和版本信息的相关功能：
import {BackupExtensionAbility,BundleVersion} from '@kit.CoreFileKit';

// 定义一个继承自 BackupExtensionAbility 的类 EntryBackupAbility
export default class EntryBackupAbility extends BackupExtensionAbility{
  async onBackup(){
    hilog.info(0x0000,'testTag','onBackup ok');
  }

  // 生命周期方法，当恢复操作触发时调用
  async onRestore(bundleVersion:BundleVersion){
    hilog.info(0x0000,'testTag','onRestore ok %{public}s',JSON.stringify(bundleVersion));
  }
}