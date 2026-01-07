// 导入必要的模块
// UIAbility 是一个基础类，用于定义应用的 UI 能力
import {UIAbility} from '@kit.AbilityKit';
// 提供窗口相关的功能：
import {window} from '@kit.ArkUI';
// 提供权限管理相关的功能：
import {abilityAccessCtrl,Permissions} from '@kit.AbilityKit';
// 提供日志记录功能：
import {hilog} from '@kit.PerformanceAnalysisKit';
// 定义一个继承自 UIAbility 的类 EntryAbility
export default class EntryAbility extends UIAbility{
    // 生命周期方法，当窗口阶段创建时调用
  async onWindowStageCreate(windowStage:window.WindowStage):Promise<void>{
    // 请求权限，确保应用有权限访问联系人
    await this.requestPermissions(['ohos.permission.READ_CONTACTS','ohos.permission.WRITE_CONTACTS']);
    // 加载主页面
    windowStage.loadContent('pages/Index',(err)=>{
      if(err.code){
        // 如果加载失败，记录错误日志
        hilog.error(0x0000,'testTag','Failed to load the content. Cause: %{public}s',JSON.stringify(err)??'');
        return;
      }
      // 如果加载成功，记录成功日志
      hilog.info(0x0000,'testTag','Succeeded in loading the content.');
    });
  }

  //请求权限
  async requestPermissions(permissions:Permissions[]):Promise<void>{
    // 创建权限管理器实例
    const atManager = abilityAccessCtrl.createAtManager();
    try{
      // 请求用户授予指定的权限
      const result=await atManager.requestPermissionsFromUser(this.context,permissions);
      // 检查result中的授权结果
      if(result.authResults[0]===0){
        console.log('联系人权限已授予');
      }
    }catch(error){
      console.error('权限请求失败:',error);
    }
  }
}