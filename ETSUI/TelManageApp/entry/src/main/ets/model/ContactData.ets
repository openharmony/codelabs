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

// 提供联系人相关的操作接口
import {contact} from '@kit.ContactsKit';
// 提供业务错误类型
import {BusinessError} from '@kit.BasicServicesKit';
// 提供应用上下文对象
import {Context} from '@kit.AbilityKit';

// 定义 ContactData 类，用于管理联系人数据
export class ContactData{
  private context:Context|null=null;  // 保存上下文对象
  // 构造函数，接收可选的 Context 参数
  constructor(context?:Context){
    if(context){
      this.context=context;
      console.log('ContactData: 使用传入的Context');
    }else{
      console.warn('ContactData: 未传入Context，某些操作可能失败');
    }
  }

  // 设置Context的方法
  setContext(context:Context):void{
    this.context=context;
    console.log('ContactData: Context已设置');
  }

  // 检查Context是否有效
  private checkContext():boolean{
    if(!this.context){
      console.error('ContactData: Context未设置，无法执行操作');
      return false;
    }
    return true;
  }

  // 查询所有联系人
  async queryAllContacts():Promise<contact.Contact[]>{
    // 首先检查Context是否有效
    if(!this.checkContext()){
      throw new Error('Context未设置，无法查询联系人');
    }
    return new Promise((resolve, reject)=>{
      try{
        contact.queryContacts(this.context,(err,data)=>{
          if(err){
            console.error('Failed to query contacts. Cause: '+JSON.stringify(err));
            reject(err);
            return;
          }
          console.log(`Succeeded in querying contacts. Data length:${data.length}`);
          resolve(data);
        });
      }catch(error){
        console.error(`An unexpected error occurred: ${error.message}`);
        reject(error);
      }
    });
  }

  // 根据ID查询联系人
  async queryContactById(key:string):Promise<contact.Contact|null>{
    if(!this.checkContext()){
      return null;
    }
    try{
      console.log('ContactData: 开始根据ID查询联系人，key:',key);
      const contactData=await contact.queryContact(this.context!,key);
      console.log('ContactData: 根据ID查询联系人成功');
      return contactData;
    } catch(error){
      console.error('ContactData: 根据ID查询联系人失败. Code: '+(error as BusinessError).code,'错误详情:',error);
      return null;
    }
  }

  // 添加联系人
  async addContact(contactInfo:contact.Contact):Promise<number>{
    if(!this.checkContext()){
      return -1;
    }
    try{
      console.log('ContactData: 开始添加联系人，联系人信息:',JSON.stringify(contactInfo));
      const contactId=await contact.addContact(this.context!,contactInfo);
      console.log('ContactData: 添加联系人返回ID:',contactId);

      if(contactId===-1){
        console.error('ContactData: 添加联系人失败，返回ID为-1');
        await this.debugContactSystem();
      }else{
        console.log('ContactData: 添加联系人成功，ID:',contactId);
      }
      return contactId;
    }catch(error){
      console.error('ContactData: 添加联系人异常. Code: '+(error as BusinessError).code,'错误详情:',error);
      return -1;
    }
  }

  // 调试联系人系统状态
  async debugContactSystem():Promise<void>{
    if(!this.checkContext()){
      return;
    }
    try{
      console.log('ContactData: 开始调试联系人系统');
      const allContacts=await this.queryAllContacts();
      console.log('ContactData: 系统当前联系人数量:',allContacts.length);
    }catch(error){
      console.error('ContactData: 调试联系人系统失败:',error);
    }
  }

  // 更新联系人
  async updateContact(contactInfo:contact.Contact):Promise<void>{
    if(!this.checkContext()){
      throw new Error('Context未设置');
    }
    try{
      console.log('ContactData: 开始更新联系人，联系人信息:',JSON.stringify(contactInfo));
      await contact.updateContact(this.context!,contactInfo);
      console.log('ContactData: 更新联系人成功');
    }catch(error){
      console.error('ContactData: 更新联系人失败. Code: '+(error as BusinessError).code,'错误详情:',error);
      throw new Error(`更新联系人失败:${(error as BusinessError).code}`);
    }
  }

  // 删除联系人
  async deleteContact(key:string):Promise<void>{
    if(!this.checkContext()){
      throw new Error('Context未设置');
    }
    try{
      console.log('ContactData: 开始删除联系人，key:',key);
      await contact.deleteContact(this.context!,key);
      console.log('ContactData: 删除联系人成功');
    }catch(error){
      console.error('ContactData: 删除联系人失败. Code: '+(error as BusinessError).code,'错误详情:',error);
      throw new Error(`删除联系人失败: ${(error as BusinessError).code}`);
    }
  }

  // 根据电话号码查询联系人
  async queryContactsByPhone(phoneNumber:string):Promise<contact.Contact[]>{
    if(!this.checkContext()){
      return [];
    }
    try{
      console.log('ContactData: 开始根据电话号码查询联系人，号码:',phoneNumber);
      const contacts=await contact.queryContactsByPhoneNumber(this.context!,phoneNumber);
      console.log('ContactData: 根据电话号码查询联系人成功，数量:',contacts.length);
      return contacts;
    }catch(error){
      console.error('ContactData: 根据电话号码查询联系人失败. Code: ' + (error as BusinessError).code,'错误详情:',error);
      return [];
    }
  }

  // 创建联系人对象
  createContact(name:string,phoneNumber:string,email?:string):contact.Contact{
    console.log('ContactData: 创建联系人对象，姓名:', name, '电话:', phoneNumber, '邮箱:', email);
    const contactObj:contact.Contact={
      name:{
        fullName: name
      },
      phoneNumbers:[{
        phoneNumber:phoneNumber,
        labelId:contact.PhoneNumber.NUM_MOBILE
      }]
    };
    if(email && email.trim()!== ''){
      contactObj.emails=[{
        email:email,
        labelId:contact.Email.EMAIL_HOME
      }];
    }
    console.log('ContactData: 创建的联系人对象:',JSON.stringify(contactObj));
    return contactObj;
  }

  /**
   * 批量删除联系人。
   * 输入：keys 联系人 key 列表。
   * 输出：批量删除结果，包含成功数量与失败列表。
   */
  async deleteContacts(keys: string[]): Promise<BatchDeleteResult> {
    const result: BatchDeleteResult = { successCount: 0, failedKeys: [] };
    if (!this.checkContext()) {
      result.failedKeys = keys.slice();
      return result;
    }

    for (const key of keys) {
      const success = await this.tryDeleteContact(key);
      if (success) {
        result.successCount += 1;
      } else {
        result.failedKeys.push(key);
      }
    }

    return result;
  }

  /**
   * 尝试删除单个联系人并返回结果。
   * 输入：key 联系人 key。
   * 输出：成功返回 true，否则 false。
   */
  private async tryDeleteContact(key: string): Promise<boolean> {
    try {
      await contact.deleteContact(this.context!, key);
      return true;
    } catch (error) {
      console.error('ContactData: 批量删除联系人失败. Code: ' + (error as BusinessError).code, '错误详情:', error);
      return false;
    }
  }
}

/**
 * 批量删除结果结构。
 * 输入：无。
 * 输出：successCount 成功数量，failedKeys 失败 key 列表。
 */
export interface BatchDeleteResult {
  successCount: number;
  failedKeys: string[];
}