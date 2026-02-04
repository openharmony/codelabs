/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-20
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


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