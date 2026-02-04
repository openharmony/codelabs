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

// 用户信息（登录态会存入 AppStorage(currentUser)）
export class User {
  id: number;
  username: string;
  password: string; // 已加密
  phone: string;
  createTime: number;
  voucherBalance: number; // Demo 代金券余额：结算时扣减

  constructor(id: number, username: string, password: string, phone: string, createTime: number, voucherBalance: number = 500) {
    this.id = id;
    this.username = username;
    this.password = password;
    this.phone = phone;
    this.createTime = createTime;
    this.voucherBalance = voucherBalance;
  }
}

// 商品信息（首页 mock 数据会写入数据库/内存）
export class Product {
  id: number;
  name: string;
  price: number;
  image: string | Resource;
  category: string;
  description: string;
  stock: number; // 库存：结算时扣减

  constructor(id: number, name: string, price: number, image: string | Resource, category: string, description: string, stock: number = 99) {
    this.id = id;
    this.name = name;
    this.price = price;
    this.image = image;
    this.category = category;
    this.description = description;
    this.stock = stock;
  }
}

export class CartItem {
  id: number;
  productId: number;
  productName: string;
  price: number;
  count: number;
  selected: boolean;
  image: string | Resource;

  constructor(id: number, productId: number, productName: string, price: number, count: number, selected: boolean, image: string | Resource) {
    this.id = id;
    this.productId = productId;
    this.productName = productName;
    this.price = price;
    this.count = count;
    this.selected = selected;
    this.image = image;
  }
}

export class Address {
  id: number;
  userId: number;
  name: string;
  phone: string;
  province: string;
  city: string;
  detail: string;
  isDefault: boolean;
  label: string;

  constructor(id: number, userId: number, name: string, phone: string, province: string, city: string, detail: string, isDefault: boolean, label: string) {
    this.id = id;
    this.userId = userId;
    this.name = name;
    this.phone = phone;
    this.province = province;
    this.city = city;
    this.detail = detail;
    this.isDefault = isDefault;
    this.label = label;
  }
}

// 订单主表（不含明细行）
export class Order {
  id: string;
  userId: number;
  totalAmount: number;
  createTime: number;
  status: number; // 0: 待付款 1: 待发货 2: 待收货 3: 待评价 4: 售后 5: 已取消 6: 已完成
  signature: string; // Demo 字段：完整性校验占位

  constructor(id: string, userId: number, totalAmount: number, createTime: number, status: number, signature: string) {
    this.id = id;
    this.userId = userId;
    this.totalAmount = totalAmount;
    this.createTime = createTime;
    this.status = status;
    this.signature = signature;
  }
}

export class OrderItem {
  id: number;
  orderId: string;
  productId: number;
  productName: string;
  price: number;
  count: number;
  image: string | Resource;

  constructor(id: number, orderId: string, productId: number, productName: string, price: number, count: number, image: string | Resource) {
    this.id = id;
    this.orderId = orderId;
    this.productId = productId;
    this.productName = productName;
    this.price = price;
    this.count = count;
    this.image = image;
  }
}

// 订单聚合模型：用于页面展示（订单 + 订单商品明细）
export class OrderWithItems {
  order: Order;
  items: OrderItem[];

  constructor(order: Order, items: OrderItem[]) {
    this.order = order;
    this.items = items;
  }
}

export class FavoriteItem {
  id: number;
  userId: number;
  productId: number;
  createTime: number;

  constructor(id: number, userId: number, productId: number, createTime: number) {
    this.id = id;
    this.userId = userId;
    this.productId = productId;
    this.createTime = createTime;
  }
}

export class FootprintItem {
  id: number;
  userId: number;
  productId: number;
  createTime: number;

  constructor(id: number, userId: number, productId: number, createTime: number) {
    this.id = id;
    this.userId = userId;
    this.productId = productId;
    this.createTime = createTime;
  }
}