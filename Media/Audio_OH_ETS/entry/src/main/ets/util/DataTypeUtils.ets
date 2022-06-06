/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
/*
 *数据类型工具类
 */

export default class DataTypeUtils {
  static isNumber(value) {
    return typeof value === 'number' && isFinite(value);
  }

  static isNull(value) {
    return typeof (value) == 'undefined'
  }

  static deepCopy(obj) {
    var newobj = obj.constructor === Array ? [] : {};
    if (typeof obj !== 'object') {
      return obj;
    } else {
      for (var i in obj) {
        if (typeof obj[i] === 'object') { //判断对象的这条属性是否为对象
          newobj[i] = this.deepCopy(obj[i]); //若是对象进行嵌套调用
        } else {
          newobj[i] = obj[i];
        }
      }
    }
    return newobj; //返回深度克隆后的对象
  }
}