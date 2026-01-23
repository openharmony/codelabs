/*
 * Copyright (c) 2026.
 *
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
export class Contact {
  id: string = '';       // 用于UI和路由的字符串ID
  key: string = '';      // 用于API调用的key（从系统获取）
  name: string = '';
  phone: string = '';
  email: string = '';
  remark: string = '';

  constructor(name: string, phone: string) {
    this.name = name;
    this.phone = phone;
  }
}
