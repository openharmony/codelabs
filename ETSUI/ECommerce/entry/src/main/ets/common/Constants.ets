/* 
  * Copyright (c) 2026 Huawei Device Co., Ltd. 
  * Licensed under the Apache License, Version 2.0 (the "License"); 
  * you may not use this file except in compliance with the License. 
  * You may obtain a copy of the License at 
  * 
  *     `http://www.apache.org/licenses/LICENSE-2.0`  
  * 
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
  * See the License for the specific language governing permissions and 
  * limitations under the License. 
*/

export class Constants {
  static readonly DB_NAME = 'e-commerce.db';
  static readonly DB_VERSION = 1;

  static readonly TABLE_USER = 'user';
  static readonly TABLE_PRODUCT = 'product';
  static readonly TABLE_CART = 'cart';
  static readonly TABLE_ADDRESS = 'address';
  static readonly TABLE_ORDER = 'orders';
  static readonly TABLE_ORDER_ITEM = 'order_item';
  static readonly TABLE_FAVORITE = 'favorite';
  static readonly TABLE_FOOTPRINT = 'footprint';

  static readonly KEY_USER_SESSION = 'user_session';
}