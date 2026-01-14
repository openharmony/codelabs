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

import { relationalStore } from '@kit.ArkData';
import { hilog } from '@kit.PerformanceAnalysisKit';
import { Constants } from '../common/Constants';
import { User, Product, CartItem, Address, Order, OrderItem, OrderWithItems, FavoriteItem, FootprintItem } from '../model/DataModels';
import common from '@ohos.app.ability.common';

// 数据库工具类：管理SQLite数据库及表结构
export class RdbUtil {
  private static rdbStore: relationalStore.RdbStore | null = null;
  private static context: common.Context | null = null;

  private static memUsers: User[] = [];
  private static memProducts: Product[] = [];
  private static memCart: CartItem[] = [];
  private static memAddresses: Address[] = [];
  private static memFavorites: FavoriteItem[] = [];
  private static memFootprints: FootprintItem[] = [];
  private static memOrders: Order[] = [];
  private static memOrderItems: OrderItem[] = [];

  private static readonly fallbackMediaKey: string = 'app.media.startIcon';

  static normalizeMediaKey(src: string | Resource): string {
    if (typeof src === 'string') {
      return src;
    }
    return '';
  }

  static resolveImageSource(src: string | Resource): string | Resource {
    if (typeof src !== 'string') {
      return src;
    }
    const key = (src || '').trim();
    if (!key || key.indexOf('[object Object]') >= 0) {
      return $r(RdbUtil.fallbackMediaKey);
    }
    if (key.startsWith('app.media.')) {
      return $r(key);
    }
    return key;
  }

  static resolveProductImageSource(productId: number): string | Resource {
    if (!Number.isFinite(productId) || productId <= 0) {
      return $r(RdbUtil.fallbackMediaKey);
    }
    const mem = RdbUtil.memProducts.find(p => p.id === productId);
    if (mem && mem.image) {
      return mem.image;
    }
    return $r(RdbUtil.fallbackMediaKey);
  }

  // 初始化数据库
  static async initDb(context: common.Context): Promise<void> {
    RdbUtil.context = context;
    if (RdbUtil.rdbStore) return;

    const config: relationalStore.StoreConfig = {
      name: Constants.DB_NAME,
      securityLevel: relationalStore.SecurityLevel.S1
    };

    try {
      RdbUtil.rdbStore = await relationalStore.getRdbStore(context, config);
      await RdbUtil.createTables();
      hilog.info(0x0000, 'RdbUtil', 'Database initialized successfully');
    } catch (err) {
      hilog.error(0x0000, 'RdbUtil', 'Init DB failed: %{public}s', JSON.stringify(err));
    }
  }

  private static async getStore(): Promise<relationalStore.RdbStore | null> {
    if (RdbUtil.rdbStore) {
      return RdbUtil.rdbStore;
    }
    if (RdbUtil.context) {
      hilog.warn(0x0000, 'RdbUtil', 'rdbStore is null, attempting to re-init with saved context');
      await RdbUtil.initDb(RdbUtil.context);
      return RdbUtil.rdbStore;
    }
    hilog.error(0x0000, 'RdbUtil', 'rdbStore is null and context is not set. Operation failed.');
    return null;
  }

  // 创建数据库表
  private static async createTables() {
    if (!RdbUtil.rdbStore) return;
    try {
      const sqlUser = `CREATE TABLE IF NOT EXISTS ${Constants.TABLE_USER} (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        username TEXT,
        password TEXT,
        phone TEXT,
        createTime INTEGER,
        voucherBalance REAL DEFAULT 500
      )`;
      await RdbUtil.rdbStore.executeSql(sqlUser);

      try {
        await RdbUtil.rdbStore.executeSql(`ALTER TABLE ${Constants.TABLE_USER} ADD COLUMN voucherBalance REAL DEFAULT 500`);
      } catch (e) {
      }

      const sqlProduct = `CREATE TABLE IF NOT EXISTS ${Constants.TABLE_PRODUCT} (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        name TEXT,
        price REAL,
        image TEXT,
        category TEXT,
        description TEXT,
        stock INTEGER
      )`;
      await RdbUtil.rdbStore.executeSql(sqlProduct);

      const sqlCart = `CREATE TABLE IF NOT EXISTS ${Constants.TABLE_CART} (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        productId INTEGER,
        productName TEXT,
        price REAL,
        count INTEGER,
        selected INTEGER,
        image TEXT
      )`;
      await RdbUtil.rdbStore.executeSql(sqlCart);

      const sqlAddress = `CREATE TABLE IF NOT EXISTS ${Constants.TABLE_ADDRESS} (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        userId INTEGER,
        name TEXT,
        phone TEXT,
        province TEXT,
        city TEXT,
        detail TEXT,
        isDefault INTEGER,
        label TEXT
      )`;
      await RdbUtil.rdbStore.executeSql(sqlAddress);

      const sqlOrder = `CREATE TABLE IF NOT EXISTS ${Constants.TABLE_ORDER} (
        id TEXT PRIMARY KEY,
        userId INTEGER,
        totalAmount REAL,
        createTime INTEGER,
        status INTEGER,
        signature TEXT
      )`;
      await RdbUtil.rdbStore.executeSql(sqlOrder);

      const sqlOrderItem = `CREATE TABLE IF NOT EXISTS ${Constants.TABLE_ORDER_ITEM} (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        orderId TEXT,
        productId INTEGER,
        productName TEXT,
        price REAL,
        count INTEGER,
        image TEXT
      )`;
      await RdbUtil.rdbStore.executeSql(sqlOrderItem);

      const sqlFavorite = `CREATE TABLE IF NOT EXISTS ${Constants.TABLE_FAVORITE} (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        userId INTEGER,
        productId INTEGER,
        createTime INTEGER
      )`;
      await RdbUtil.rdbStore.executeSql(sqlFavorite);

      const sqlFootprint = `CREATE TABLE IF NOT EXISTS ${Constants.TABLE_FOOTPRINT} (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        userId INTEGER,
        productId INTEGER,
        createTime INTEGER
      )`;
      await RdbUtil.rdbStore.executeSql(sqlFootprint);
    } catch (err) {
      hilog.error(0x0000, 'RdbUtil', 'Create tables failed: %{public}s', JSON.stringify(err));
    }
  }

  // 添加新用户
  static async addUser(user: User): Promise<number> {
    user.id = RdbUtil.memUsers.length + 1;
    RdbUtil.memUsers.push(user);
    hilog.info(0x0000, 'RdbUtil', 'Memory: addUser success, total: %{public}d', RdbUtil.memUsers.length);

    const store = await RdbUtil.getStore();
    if (!store) return user.id;
    
    try {
      const valueBucket: relationalStore.ValuesBucket = {
        username: user.username,
        password: user.password,
        phone: user.phone,
        createTime: user.createTime,
        voucherBalance: user.voucherBalance
      };
      const id = await store.insert(Constants.TABLE_USER, valueBucket);
      hilog.info(0x0000, 'RdbUtil', 'RDB: addUser success, id: %{public}d', id);
      return id;
    } catch (err) {
      hilog.error(0x0000, 'RdbUtil', 'RDB: addUser failed: %{public}s', JSON.stringify(err));
      return user.id;
    }
  }

  static async getUser(username: string): Promise<User | null> {
    let user: User | null = null;
    const store = await RdbUtil.getStore();
    
    if (store) {
      try {
        let predicates = new relationalStore.RdbPredicates(Constants.TABLE_USER);
        predicates.equalTo('username', username);
        let resultSet = await store.query(predicates);
        if (resultSet.goToFirstRow()) {
          const balCol = resultSet.getColumnIndex('voucherBalance');
          const bal = balCol >= 0 ? resultSet.getDouble(balCol) : 500;
          user = new User(
            resultSet.getLong(resultSet.getColumnIndex('id')),
            resultSet.getString(resultSet.getColumnIndex('username')),
            resultSet.getString(resultSet.getColumnIndex('password')),
            resultSet.getString(resultSet.getColumnIndex('phone')),
            resultSet.getLong(resultSet.getColumnIndex('createTime')),
            bal
          );
          resultSet.close();
        } else {
            resultSet.close();
        }
      } catch (err) {
        hilog.error(0x0000, 'RdbUtil', 'RDB: getUser failed: %{public}s', JSON.stringify(err));
      }
    }

    if (!user) {
      const memUser = RdbUtil.memUsers.find(u => u.username === username);
      if (memUser) {
        hilog.info(0x0000, 'RdbUtil', 'Memory: getUser found user: %{public}s', username);
        return memUser;
      }
    }

    return user;
  }

  static async getUserById(userId: number): Promise<User | null> {
    const store = await RdbUtil.getStore();

    if (store) {
      try {
        let predicates = new relationalStore.RdbPredicates(Constants.TABLE_USER);
        predicates.equalTo('id', userId);
        let resultSet = await store.query(predicates);
        if (resultSet.goToFirstRow()) {
          const balCol = resultSet.getColumnIndex('voucherBalance');
          const bal = balCol >= 0 ? resultSet.getDouble(balCol) : 500;

          const user = new User(
            resultSet.getLong(resultSet.getColumnIndex('id')),
            resultSet.getString(resultSet.getColumnIndex('username')),
            resultSet.getString(resultSet.getColumnIndex('password')),
            resultSet.getString(resultSet.getColumnIndex('phone')),
            resultSet.getLong(resultSet.getColumnIndex('createTime')),
            bal
          );
          resultSet.close();
          return user;
        }
        resultSet.close();
      } catch (err) {
        hilog.error(0x0000, 'RdbUtil', 'RDB: getUserById failed: %{public}s', JSON.stringify(err));
      }
    }

    const memUser = RdbUtil.memUsers.find(u => u.id === userId);
    return memUser ? memUser : null;
  }

  static async insertProduct(product: Product): Promise<void> {
    const store = await RdbUtil.getStore();

    if (store) {
      try {
        const valueBucket: relationalStore.ValuesBucket = {
          name: product.name,
          price: product.price,
          image: RdbUtil.normalizeMediaKey(product.image),
          category: product.category,
          description: product.description,
          stock: product.stock
        };
        const id = await store.insert(Constants.TABLE_PRODUCT, valueBucket);
        product.id = Number(id);
      } catch (err) {
        hilog.error(0x0000, 'RdbUtil', 'RDB: insertProduct failed: %{public}s', JSON.stringify(err));
        product.id = RdbUtil.memProducts.length + 1;
      }
    } else {
      product.id = RdbUtil.memProducts.length + 1;
    }

    const nextMemId = RdbUtil.memProducts.reduce((maxId, p) => Math.max(maxId, p.id), 0) + 1;
    if (product.id <= 0 || RdbUtil.memProducts.some(p => p.id === product.id)) {
      product.id = nextMemId;
    }
    RdbUtil.memProducts.push(product);
  }

  // 获取所有商品
  static async getAllProducts(): Promise<Product[]> {
    if (RdbUtil.memProducts.length > 0) {
      hilog.info(0x0000, 'RdbUtil', 'Memory: getAllProducts returning %{public}d items', RdbUtil.memProducts.length);
      return RdbUtil.memProducts;
    }

    let products: Product[] = [];
    const store = await RdbUtil.getStore();

    if (store) {
      try {
        let predicates = new relationalStore.RdbPredicates(Constants.TABLE_PRODUCT);
        let resultSet = await store.query(predicates);
        while (resultSet.goToNextRow()) {
          products.push(new Product(
            resultSet.getLong(resultSet.getColumnIndex('id')),
            resultSet.getString(resultSet.getColumnIndex('name')),
            resultSet.getDouble(resultSet.getColumnIndex('price')),
            resultSet.getString(resultSet.getColumnIndex('image')),
            resultSet.getString(resultSet.getColumnIndex('category')),
            resultSet.getString(resultSet.getColumnIndex('description')),
            resultSet.getLong(resultSet.getColumnIndex('stock'))
          ));
        }
        resultSet.close();
      } catch (e) {
        hilog.error(0x0000, 'RdbUtil', 'RDB: getAllProducts failed: %{public}s', JSON.stringify(e));
      }
    }

    return products;
  }

  // 添加商品到购物车
  static async addToCart(item: CartItem): Promise<void> {
    const existingMem = RdbUtil.memCart.find(c => c.productId === item.productId);
    if (existingMem) {
      existingMem.count += item.count;
      existingMem.selected = true;
    } else {
      item.id = RdbUtil.memCart.length + 1;
      RdbUtil.memCart.push(item);
    }

    const store = await RdbUtil.getStore();
    if (!store) return;

    try {
      const queryPred = new relationalStore.RdbPredicates(Constants.TABLE_CART);
      queryPred.equalTo('productId', item.productId);
      const rs = await store.query(queryPred);

      if (rs.goToFirstRow()) {
        const id = rs.getLong(rs.getColumnIndex('id'));
        const currentCount = rs.getLong(rs.getColumnIndex('count'));
        rs.close();

        const valueBucket: relationalStore.ValuesBucket = {
          count: currentCount + item.count,
          selected: 1,
          image: RdbUtil.normalizeMediaKey(item.image)
        };
        const updatePred = new relationalStore.RdbPredicates(Constants.TABLE_CART);
        updatePred.equalTo('id', id);
        await store.update(valueBucket, updatePred);
        return;
      }

      rs.close();
      const valueBucket: relationalStore.ValuesBucket = {
        productId: item.productId,
        productName: item.productName,
        price: item.price,
        count: item.count,
        selected: item.selected ? 1 : 0,
        image: RdbUtil.normalizeMediaKey(item.image)
      };
      await store.insert(Constants.TABLE_CART, valueBucket);
    } catch (err) {
      hilog.error(0x0000, 'RdbUtil', 'RDB: addToCart failed: %{public}s', JSON.stringify(err));
    }
  }

  static async getCartItems(): Promise<CartItem[]> {
    let items: CartItem[] = [];
    const store = await RdbUtil.getStore();
    if (store) {
        try {
            let predicates = new relationalStore.RdbPredicates(Constants.TABLE_CART);
            let resultSet = await store.query(predicates);
            while (resultSet.goToNextRow()) {
              items.push(new CartItem(
                resultSet.getLong(resultSet.getColumnIndex('id')),
                resultSet.getLong(resultSet.getColumnIndex('productId')),
                resultSet.getString(resultSet.getColumnIndex('productName')),
                resultSet.getDouble(resultSet.getColumnIndex('price')),
                resultSet.getLong(resultSet.getColumnIndex('count')),
                resultSet.getLong(resultSet.getColumnIndex('selected')) === 1,
                RdbUtil.resolveProductImageSource(resultSet.getLong(resultSet.getColumnIndex('productId')))
              ));
            }
            resultSet.close();
        } catch (e) {}
    }

    if (items.length === 0 && RdbUtil.memCart.length > 0) {
        return RdbUtil.memCart;
    }
    return items;
  }
  
  static async updateCartItem(item: CartItem): Promise<void> {
    const idx = RdbUtil.memCart.findIndex(c => c.id === item.id);
    if (idx !== -1) {
        RdbUtil.memCart[idx] = item;
    }

    const store = await RdbUtil.getStore();
    if (!store) return;
    const valueBucket: relationalStore.ValuesBucket = {
      count: item.count,
      selected: item.selected ? 1 : 0
    };
    let predicates = new relationalStore.RdbPredicates(Constants.TABLE_CART);
    predicates.equalTo('id', item.id);
    await store.update(valueBucket, predicates);
  }
  
  static async deleteCartItem(id: number): Promise<void> {
     RdbUtil.memCart = RdbUtil.memCart.filter(c => c.id !== id);

     const store = await RdbUtil.getStore();
     if (!store) return;
     let predicates = new relationalStore.RdbPredicates(Constants.TABLE_CART);
     predicates.equalTo('id', id);
     await store.delete(predicates);
  }

  // 结算：扣库存 + 生成订单 + 写入订单明细 + 删除购物车选中项
  // 同时扣减用户代金券余额（余额不足则直接失败）
  static async checkout(items: CartItem[], userId: number): Promise<boolean> {
    const store = await RdbUtil.getStore();
    hilog.info(
      0x0000,
      'RdbUtil',
      'Checkout start: userId=%{public}d items=%{public}d store=%{public}s memProducts=%{public}d memCart=%{public}d',
      userId,
      items.length,
      store ? 'yes' : 'no',
      RdbUtil.memProducts.length,
      RdbUtil.memCart.length
    );

    let useRdb = store !== null;
    if (store && items.length > 0) {
      const probeProductId = items[0].productId;
      const memProbe = RdbUtil.memProducts.find(p => p.id === probeProductId);
      let probeFound = false;
      let stockCol = -1;
      let probeStock = -1;

      try {
        let probePred = new relationalStore.RdbPredicates(Constants.TABLE_PRODUCT);
        probePred.equalTo('id', probeProductId);
        const probeRs = await store.query(probePred);
        if (probeRs.goToFirstRow()) {
          probeFound = true;
          stockCol = probeRs.getColumnIndex('stock');
          if (stockCol >= 0) {
            probeStock = probeRs.getLong(stockCol);
          }
        }
        probeRs.close();
      } catch (e) {
        hilog.warn(0x0000, 'RdbUtil', 'Checkout probe failed: %{public}s', JSON.stringify(e));
        useRdb = false;
      }

      const memStock = memProbe ? memProbe.stock : -1;
      hilog.info(
        0x0000,
        'RdbUtil',
        'Checkout probe: productId=%{public}d found=%{public}s stockCol=%{public}d rdbStock=%{public}d memStock=%{public}d',
        probeProductId,
        probeFound ? 'yes' : 'no',
        stockCol,
        probeStock,
        memStock
      );

      if (!probeFound || stockCol < 0) {
        useRdb = false;
      }

      if (memProbe && memProbe.stock > 0 && probeStock === 0) {
        useRdb = false;
      }
    }

    if (store && useRdb) {
      try {
        // RDB 模式：使用事务保证“扣余额/扣库存/生成订单/清理购物车”原子性
        store.beginTransaction();

        const totalAmount = items.reduce((sum, i) => sum + i.price * i.count, 0);

        const userPred = new relationalStore.RdbPredicates(Constants.TABLE_USER);
        userPred.equalTo('id', userId);
        const urs = await store.query(userPred);
        if (!urs.goToFirstRow()) {
          urs.close();
          store.rollBack();
          return false;
        }
        const balCol = urs.getColumnIndex('voucherBalance');
        const balance = balCol >= 0 ? urs.getDouble(balCol) : 500;
        urs.close();

        if (balance < totalAmount) {
          // 余额不足：直接回滚事务
          store.rollBack();
          return false;
        }

        const userUpdate: relationalStore.ValuesBucket = { voucherBalance: balance - totalAmount };
        await store.update(userUpdate, userPred);

        for (const item of items) {
          hilog.info(
            0x0000,
            'RdbUtil',
            'Checkout item: cartId=%{public}d productId=%{public}d count=%{public}d',
            item.id,
            item.productId,
            item.count
          );

          let predicates = new relationalStore.RdbPredicates(Constants.TABLE_PRODUCT);
          predicates.equalTo('id', item.productId);
          const rs = await store.query(predicates);
          if (!rs.goToFirstRow()) {
            rs.close();
            hilog.warn(0x0000, 'RdbUtil', 'Checkout RDB: product not found, productId=%{public}d', item.productId);
            store.rollBack();
            return false;
          }

          const stockCol = rs.getColumnIndex('stock');
          if (stockCol < 0) {
            rs.close();
            hilog.error(0x0000, 'RdbUtil', 'Checkout RDB: missing stock column. Did you recreate DB?');
            store.rollBack();
            return false;
          }

          const stock = rs.getLong(stockCol);
          rs.close();
          hilog.info(0x0000, 'RdbUtil', 'Checkout RDB: productId=%{public}d stock=%{public}d need=%{public}d', item.productId, stock, item.count);

          if (stock < item.count) {
          // 库存不足：回滚事务
          store.rollBack();
          return false;
        }

          const updateBucket: relationalStore.ValuesBucket = { stock: stock - item.count };
          await store.update(updateBucket, predicates);
        }

        const now = new Date().getTime();
        const orderId = now.toString();

        const orderBucket: relationalStore.ValuesBucket = {
          id: orderId,
          userId: userId,
          totalAmount: totalAmount,
          createTime: now,
          status: 1,
          signature: 'signed'
        };
        await store.insert(Constants.TABLE_ORDER, orderBucket);

        for (const item of items) {
          const itemBucket: relationalStore.ValuesBucket = {
            orderId: orderId,
            productId: item.productId,
            productName: item.productName,
            price: item.price,
            count: item.count,
            image: RdbUtil.normalizeMediaKey(item.image)
          };
          await store.insert(Constants.TABLE_ORDER_ITEM, itemBucket);
        }

        const memOrder = new Order(orderId, userId, totalAmount, now, 1, 'signed');
        RdbUtil.memOrders.push(memOrder);
        for (const item of items) {
          RdbUtil.memOrderItems.push(
            new OrderItem(
              RdbUtil.memOrderItems.length + 1,
              orderId,
              item.productId,
              item.productName,
              item.price,
              item.count,
              item.image
            )
          );
        }

        for (const item of items) {
           let cartPred = new relationalStore.RdbPredicates(Constants.TABLE_CART);
           cartPred.equalTo('id', item.id);
           await store.delete(cartPred);
        }

        store.commit();

        const memUser = RdbUtil.memUsers.find(u => u.id === userId);
        if (memUser) {
          memUser.voucherBalance = balance - totalAmount;
        }

        for (const item of items) {
          const memProduct = RdbUtil.memProducts.find(p => p.id === item.productId);
          if (memProduct) {
            memProduct.stock -= item.count;
          }
        }

        RdbUtil.memCart = RdbUtil.memCart.filter(c => !items.find(i => i.id === c.id));

        return true;
      } catch (err) {
        hilog.error(0x0000, 'RdbUtil', 'Checkout failed: %{public}s', JSON.stringify(err));
        store.rollBack();
        return false;
      }
    }

    hilog.info(0x0000, 'RdbUtil', 'Checkout: Running in Memory Mode');

    const totalAmount = items.reduce((sum, i) => sum + i.price * i.count, 0);
    const memUser = RdbUtil.memUsers.find(u => u.id === userId);
    if (!memUser) {
      return false;
    }
    if (memUser.voucherBalance < totalAmount) {
      return false;
    }

    for (const item of items) {
      const product = RdbUtil.memProducts.find(p => p.id === item.productId);
      if (!product) {
        hilog.warn(0x0000, 'RdbUtil', 'Checkout Memory: product not found, productId=%{public}d', item.productId);
        return false;
      }
      hilog.info(
        0x0000,
        'RdbUtil',
        'Checkout Memory: productId=%{public}d stock=%{public}d need=%{public}d',
        item.productId,
        product.stock,
        item.count
      );
      if (product.stock < item.count) {
        return false;
      }
    }

    for (const item of items) {
      const product = RdbUtil.memProducts.find(p => p.id === item.productId);
      if (product) {
        product.stock -= item.count;
      }
    }

    memUser.voucherBalance -= totalAmount;

    const now = new Date().getTime();
    const orderId = now.toString();

    RdbUtil.memOrders.push(new Order(orderId, userId, totalAmount, now, 1, 'signed'));
    for (const item of items) {
      RdbUtil.memOrderItems.push(
        new OrderItem(
          RdbUtil.memOrderItems.length + 1,
          orderId,
          item.productId,
          item.productName,
          item.price,
          item.count,
          item.image
        )
      );
    }

    RdbUtil.memCart = RdbUtil.memCart.filter(c => !items.find(i => i.id === c.id));
    return true;
  }

  // 查询订单列表（含明细）：优先走内存缓存；若缓存为空再读数据库
  // 这样能兼容预览器/无数据库场景，也减少频繁 IO
  static async getOrderDetails(userId: number, status?: number): Promise<OrderWithItems[]> {
    const memOrders = RdbUtil.memOrders
      .filter(o => o.userId === userId)
      .filter(o => status === undefined ? true : o.status === status)
      .slice()
      .sort((a, b) => b.createTime - a.createTime);

    if (memOrders.length > 0) {
      return memOrders.map(o => {
        const items = RdbUtil.memOrderItems.filter(i => i.orderId === o.id).slice();
        return new OrderWithItems(o, items);
      });
    }

    const store = await RdbUtil.getStore();
    if (!store) return [];

    let orders: Order[] = [];
    try {
      const pred = new relationalStore.RdbPredicates(Constants.TABLE_ORDER);
      pred.equalTo('userId', userId);
      if (status !== undefined) {
        pred.equalTo('status', status);
      }
      const rs = await store.query(pred);
      while (rs.goToNextRow()) {
        orders.push(new Order(
          rs.getString(rs.getColumnIndex('id')),
          rs.getLong(rs.getColumnIndex('userId')),
          rs.getDouble(rs.getColumnIndex('totalAmount')),
          rs.getLong(rs.getColumnIndex('createTime')),
          rs.getLong(rs.getColumnIndex('status')),
          rs.getString(rs.getColumnIndex('signature'))
        ));
      }
      rs.close();
    } catch (err) {
      hilog.error(0x0000, 'RdbUtil', 'RDB: getOrderDetails orders failed: %{public}s', JSON.stringify(err));
      return [];
    }

    orders.sort((a, b) => b.createTime - a.createTime);

    const result: OrderWithItems[] = [];
    for (const o of orders) {
      let items: OrderItem[] = [];
      try {
        const ip = new relationalStore.RdbPredicates(Constants.TABLE_ORDER_ITEM);
        ip.equalTo('orderId', o.id);
        const irs = await store.query(ip);
        while (irs.goToNextRow()) {
          items.push(new OrderItem(
            irs.getLong(irs.getColumnIndex('id')),
            irs.getString(irs.getColumnIndex('orderId')),
            irs.getLong(irs.getColumnIndex('productId')),
            irs.getString(irs.getColumnIndex('productName')),
            irs.getDouble(irs.getColumnIndex('price')),
            irs.getLong(irs.getColumnIndex('count')),
            irs.getString(irs.getColumnIndex('image'))
          ));
        }
        irs.close();
      } catch (err) {
        hilog.error(0x0000, 'RdbUtil', 'RDB: getOrderDetails items failed: %{public}s', JSON.stringify(err));
      }
      result.push(new OrderWithItems(o, items));
    }

    return result;
  }

  static async getOrderDetail(orderId: string, userId: number): Promise<OrderWithItems | undefined> {
    const mem = RdbUtil.memOrders.find(o => o.id === orderId && o.userId === userId);
    if (mem) {
      const items = RdbUtil.memOrderItems.filter(i => i.orderId === orderId).slice();
      return new OrderWithItems(mem, items);
    }

    const store = await RdbUtil.getStore();
    if (!store) return undefined;

    try {
      const pred = new relationalStore.RdbPredicates(Constants.TABLE_ORDER);
      pred.equalTo('id', orderId);
      pred.equalTo('userId', userId);
      const rs = await store.query(pred);
      if (!rs.goToFirstRow()) {
        rs.close();
        return undefined;
      }

      const o = new Order(
        rs.getString(rs.getColumnIndex('id')),
        rs.getLong(rs.getColumnIndex('userId')),
        rs.getDouble(rs.getColumnIndex('totalAmount')),
        rs.getLong(rs.getColumnIndex('createTime')),
        rs.getLong(rs.getColumnIndex('status')),
        rs.getString(rs.getColumnIndex('signature'))
      );
      rs.close();

      const items: OrderItem[] = [];
      const ip = new relationalStore.RdbPredicates(Constants.TABLE_ORDER_ITEM);
      ip.equalTo('orderId', o.id);
      const irs = await store.query(ip);
      while (irs.goToNextRow()) {
        items.push(new OrderItem(
          irs.getLong(irs.getColumnIndex('id')),
          irs.getString(irs.getColumnIndex('orderId')),
          irs.getLong(irs.getColumnIndex('productId')),
          irs.getString(irs.getColumnIndex('productName')),
          irs.getDouble(irs.getColumnIndex('price')),
          irs.getLong(irs.getColumnIndex('count')),
          irs.getString(irs.getColumnIndex('image'))
        ));
      }
      irs.close();

      return new OrderWithItems(o, items);
    } catch (err) {
      hilog.error(0x0000, 'RdbUtil', 'RDB: getOrderDetail failed: %{public}s', JSON.stringify(err));
      return undefined;
    }
  }

  static async updateOrderStatus(orderId: string, status: number): Promise<boolean> {
    const mem = RdbUtil.memOrders.find(o => o.id === orderId);
    if (mem) {
      mem.status = status;
    }

    const store = await RdbUtil.getStore();
    if (!store) return mem !== undefined;

    try {
      const bucket: relationalStore.ValuesBucket = { status: status };
      const pred = new relationalStore.RdbPredicates(Constants.TABLE_ORDER);
      pred.equalTo('id', orderId);
      const changed = await store.update(bucket, pred);
      return Number(changed) > 0 || mem !== undefined;
    } catch (err) {
      hilog.error(0x0000, 'RdbUtil', 'RDB: updateOrderStatus failed: %{public}s', JSON.stringify(err));
      return mem !== undefined;
    }
  }

  static async isFavorite(userId: number, productId: number): Promise<boolean> {
    const mem = RdbUtil.memFavorites.some(f => f.userId === userId && f.productId === productId);

    const store = await RdbUtil.getStore();
    if (store) {
      try {
        const pred = new relationalStore.RdbPredicates(Constants.TABLE_FAVORITE);
        pred.equalTo('userId', userId);
        pred.equalTo('productId', productId);
        const rs = await store.query(pred);
        const ok = rs.goToFirstRow();
        rs.close();
        return ok || mem;
      } catch (err) {
        hilog.error(0x0000, 'RdbUtil', 'RDB: isFavorite failed: %{public}s', JSON.stringify(err));
        return mem;
      }
    }

    return mem;
  }

  static async addFavorite(userId: number, productId: number): Promise<void> {
    const exists = RdbUtil.memFavorites.some(f => f.userId === userId && f.productId === productId);
    if (!exists) {
      RdbUtil.memFavorites.push(new FavoriteItem(RdbUtil.memFavorites.length + 1, userId, productId, new Date().getTime()));
    }

    const store = await RdbUtil.getStore();
    if (!store) return;

    try {
      const already = await RdbUtil.isFavorite(userId, productId);
      if (already) return;

      const valueBucket: relationalStore.ValuesBucket = {
        userId,
        productId,
        createTime: new Date().getTime()
      };
      await store.insert(Constants.TABLE_FAVORITE, valueBucket);
    } catch (err) {
      hilog.error(0x0000, 'RdbUtil', 'RDB: addFavorite failed: %{public}s', JSON.stringify(err));
    }
  }

  static async removeFavorite(userId: number, productId: number): Promise<void> {
    RdbUtil.memFavorites = RdbUtil.memFavorites.filter(f => !(f.userId === userId && f.productId === productId));

    const store = await RdbUtil.getStore();
    if (!store) return;

    try {
      const pred = new relationalStore.RdbPredicates(Constants.TABLE_FAVORITE);
      pred.equalTo('userId', userId);
      pred.equalTo('productId', productId);
      await store.delete(pred);
    } catch (err) {
      hilog.error(0x0000, 'RdbUtil', 'RDB: removeFavorite failed: %{public}s', JSON.stringify(err));
    }
  }

  static async toggleFavorite(userId: number, productId: number): Promise<boolean> {
    const fav = await RdbUtil.isFavorite(userId, productId);
    if (fav) {
      await RdbUtil.removeFavorite(userId, productId);
      return false;
    }
    await RdbUtil.addFavorite(userId, productId);
    return true;
  }

  static async getFavoriteProducts(userId: number): Promise<Product[]> {
    const memFavorites = RdbUtil.memFavorites
      .filter(f => f.userId === userId)
      .slice()
      .sort((a, b) => b.createTime - a.createTime);

    if (memFavorites.length > 0) {
      let memProducts: Product[] = [];
      for (const it of memFavorites) {
        const p = RdbUtil.memProducts.find(p => p.id === it.productId);
        if (p) {
          memProducts.push(p);
        }
      }
      return memProducts;
    }

    const store = await RdbUtil.getStore();
    let favorites: FavoriteItem[] = [];

    if (store) {
      try {
        const pred = new relationalStore.RdbPredicates(Constants.TABLE_FAVORITE);
        pred.equalTo('userId', userId);
        const rs = await store.query(pred);
        if (rs.goToFirstRow()) {
          do {
            const productId = rs.getLong(rs.getColumnIndex('productId'));
            const createTime = rs.getLong(rs.getColumnIndex('createTime'));
            favorites.push(new FavoriteItem(0, userId, productId, createTime));
          } while (rs.goToNextRow());
        }
        rs.close();

        favorites.sort((a, b) => b.createTime - a.createTime);

        const productMap: Map<number, Product> = new Map<number, Product>();
        for (const it of favorites) {
          const pPred = new relationalStore.RdbPredicates(Constants.TABLE_PRODUCT);
          pPred.equalTo('id', it.productId);
          const prs = await store.query(pPred);
          if (prs.goToFirstRow()) {
            const p = new Product(
              prs.getLong(prs.getColumnIndex('id')),
              prs.getString(prs.getColumnIndex('name')),
              prs.getDouble(prs.getColumnIndex('price')),
              prs.getString(prs.getColumnIndex('image')),
              prs.getString(prs.getColumnIndex('category')),
              prs.getString(prs.getColumnIndex('description')),
              prs.getLong(prs.getColumnIndex('stock'))
            );
            productMap.set(p.id, p);
          }
          prs.close();
        }

        if (productMap.size < favorites.length && RdbUtil.memProducts.length > 0) {
          for (const it of favorites) {
            if (!productMap.has(it.productId)) {
              const p = RdbUtil.memProducts.find(p => p.id === it.productId);
              if (p) {
                productMap.set(p.id, p);
              }
            }
          }
        }

        let products: Product[] = [];
        for (const it of favorites) {
          const p = productMap.get(it.productId);
          if (p) {
            products.push(p);
          }
        }

        return products;
      } catch (err) {
        hilog.error(0x0000, 'RdbUtil', 'RDB: getFavoriteProducts failed: %{public}s', JSON.stringify(err));
      }
    }

    return [];
  }

  static async addFootprint(userId: number, productId: number): Promise<void> {
    const now = new Date().getTime();

    RdbUtil.memFootprints = RdbUtil.memFootprints.filter(f => !(f.userId === userId && f.productId === productId));
    RdbUtil.memFootprints.unshift(new FootprintItem(RdbUtil.memFootprints.length + 1, userId, productId, now));
    if (RdbUtil.memFootprints.length > 200) {
      RdbUtil.memFootprints = RdbUtil.memFootprints.slice(0, 200);
    }

    const store = await RdbUtil.getStore();
    if (!store) return;

    try {
      const pred = new relationalStore.RdbPredicates(Constants.TABLE_FOOTPRINT);
      pred.equalTo('userId', userId);
      pred.equalTo('productId', productId);
      await store.delete(pred);

      const valueBucket: relationalStore.ValuesBucket = {
        userId,
        productId,
        createTime: now
      };
      await store.insert(Constants.TABLE_FOOTPRINT, valueBucket);
    } catch (err) {
      hilog.error(0x0000, 'RdbUtil', 'RDB: addFootprint failed: %{public}s', JSON.stringify(err));
    }
  }

  static async removeFootprint(userId: number, productId: number): Promise<void> {
    RdbUtil.memFootprints = RdbUtil.memFootprints.filter(f => !(f.userId === userId && f.productId === productId));

    const store = await RdbUtil.getStore();
    if (!store) return;

    try {
      const pred = new relationalStore.RdbPredicates(Constants.TABLE_FOOTPRINT);
      pred.equalTo('userId', userId);
      pred.equalTo('productId', productId);
      await store.delete(pred);
    } catch (err) {
      hilog.error(0x0000, 'RdbUtil', 'RDB: removeFootprint failed: %{public}s', JSON.stringify(err));
    }
  }

  static async clearFootprints(userId: number): Promise<void> {
    RdbUtil.memFootprints = RdbUtil.memFootprints.filter(f => f.userId !== userId);

    const store = await RdbUtil.getStore();
    if (!store) return;

    try {
      const pred = new relationalStore.RdbPredicates(Constants.TABLE_FOOTPRINT);
      pred.equalTo('userId', userId);
      await store.delete(pred);
    } catch (err) {
      hilog.error(0x0000, 'RdbUtil', 'RDB: clearFootprints failed: %{public}s', JSON.stringify(err));
    }
  }

  static async getFootprintProducts(userId: number): Promise<Product[]> {
    const memFootprints = RdbUtil.memFootprints
      .filter(f => f.userId === userId)
      .slice()
      .sort((a, b) => b.createTime - a.createTime);

    if (memFootprints.length > 0) {
      let memProducts: Product[] = [];
      for (const it of memFootprints) {
        const p = RdbUtil.memProducts.find(p => p.id === it.productId);
        if (p) {
          memProducts.push(p);
        }
      }
      return memProducts;
    }

    const store = await RdbUtil.getStore();
    let footprints: FootprintItem[] = [];

    if (store) {
      try {
        const pred = new relationalStore.RdbPredicates(Constants.TABLE_FOOTPRINT);
        pred.equalTo('userId', userId);
        const rs = await store.query(pred);
        if (rs.goToFirstRow()) {
          do {
            const productId = rs.getLong(rs.getColumnIndex('productId'));
            const createTime = rs.getLong(rs.getColumnIndex('createTime'));
            footprints.push(new FootprintItem(0, userId, productId, createTime));
          } while (rs.goToNextRow());
        }
        rs.close();

        footprints.sort((a, b) => b.createTime - a.createTime);

        const productMap: Map<number, Product> = new Map<number, Product>();
        for (const it of footprints) {
          const pPred = new relationalStore.RdbPredicates(Constants.TABLE_PRODUCT);
          pPred.equalTo('id', it.productId);
          const prs = await store.query(pPred);
          if (prs.goToFirstRow()) {
            const p = new Product(
              prs.getLong(prs.getColumnIndex('id')),
              prs.getString(prs.getColumnIndex('name')),
              prs.getDouble(prs.getColumnIndex('price')),
              prs.getString(prs.getColumnIndex('image')),
              prs.getString(prs.getColumnIndex('category')),
              prs.getString(prs.getColumnIndex('description')),
              prs.getLong(prs.getColumnIndex('stock'))
            );
            productMap.set(p.id, p);
          }
          prs.close();
        }

        if (productMap.size < footprints.length && RdbUtil.memProducts.length > 0) {
          for (const it of footprints) {
            if (!productMap.has(it.productId)) {
              const p = RdbUtil.memProducts.find(p => p.id === it.productId);
              if (p) {
                productMap.set(p.id, p);
              }
            }
          }
        }

        let products: Product[] = [];
        for (const it of footprints) {
          const p = productMap.get(it.productId);
          if (p) {
            products.push(p);
          }
        }

        return products;
      } catch (err) {
        hilog.error(0x0000, 'RdbUtil', 'RDB: getFootprintProducts failed: %{public}s', JSON.stringify(err));
      }
    }

    return [];
  }

  static async addAddress(address: Address): Promise<void> {
    if (address.isDefault) {
      for (const a of RdbUtil.memAddresses) {
        if (a.userId === address.userId) {
          a.isDefault = false;
        }
      }
    }

    address.id = RdbUtil.memAddresses.length + 1;
    RdbUtil.memAddresses.push(address);

    const store = await RdbUtil.getStore();
    if (!store) return;

    try {
      if (address.isDefault) {
        const clearBucket: relationalStore.ValuesBucket = { isDefault: 0 };
        const clearPred = new relationalStore.RdbPredicates(Constants.TABLE_ADDRESS);
        clearPred.equalTo('userId', address.userId);
        await store.update(clearBucket, clearPred);
      }

      const valueBucket: relationalStore.ValuesBucket = {
        userId: address.userId,
        name: address.name,
        phone: address.phone,
        province: address.province,
        city: address.city,
        detail: address.detail,
        isDefault: address.isDefault ? 1 : 0,
        label: address.label
      };
      const id = await store.insert(Constants.TABLE_ADDRESS, valueBucket);
      address.id = Number(id);
    } catch (err) {
      hilog.error(0x0000, 'RdbUtil', 'RDB: addAddress failed: %{public}s', JSON.stringify(err));
    }
  }

  static async getAddresses(userId: number): Promise<Address[]> {
    let addresses: Address[] = [];
    const store = await RdbUtil.getStore();

    if (store) {
      try {
        let predicates = new relationalStore.RdbPredicates(Constants.TABLE_ADDRESS);
        predicates.equalTo('userId', userId);
        let resultSet = await store.query(predicates);
        while (resultSet.goToNextRow()) {
          addresses.push(new Address(
            resultSet.getLong(resultSet.getColumnIndex('id')),
            resultSet.getLong(resultSet.getColumnIndex('userId')),
            resultSet.getString(resultSet.getColumnIndex('name')),
            resultSet.getString(resultSet.getColumnIndex('phone')),
            resultSet.getString(resultSet.getColumnIndex('province')),
            resultSet.getString(resultSet.getColumnIndex('city')),
            resultSet.getString(resultSet.getColumnIndex('detail')),
            resultSet.getLong(resultSet.getColumnIndex('isDefault')) === 1,
            resultSet.getString(resultSet.getColumnIndex('label'))
          ));
        }
        resultSet.close();
      } catch (err) {
        hilog.error(0x0000, 'RdbUtil', 'RDB: getAddresses failed: %{public}s', JSON.stringify(err));
      }
    }

    if (addresses.length === 0) {
      return RdbUtil.memAddresses.filter(a => a.userId === userId).slice();
    }

    return addresses;
  }

  static async updateAddress(address: Address): Promise<void> {
    const idx = RdbUtil.memAddresses.findIndex(a => a.id === address.id);
    if (idx !== -1) {
      if (address.isDefault) {
        for (const a of RdbUtil.memAddresses) {
          if (a.userId === address.userId) {
            a.isDefault = false;
          }
        }
      }
      RdbUtil.memAddresses[idx] = address;
    }

    const store = await RdbUtil.getStore();
    if (!store) return;

    try {
      if (address.isDefault) {
        const clearBucket: relationalStore.ValuesBucket = { isDefault: 0 };
        const clearPred = new relationalStore.RdbPredicates(Constants.TABLE_ADDRESS);
        clearPred.equalTo('userId', address.userId);
        await store.update(clearBucket, clearPred);
      }

      const valueBucket: relationalStore.ValuesBucket = {
        name: address.name,
        phone: address.phone,
        province: address.province,
        city: address.city,
        detail: address.detail,
        isDefault: address.isDefault ? 1 : 0,
        label: address.label
      };
      const pred = new relationalStore.RdbPredicates(Constants.TABLE_ADDRESS);
      pred.equalTo('id', address.id);
      await store.update(valueBucket, pred);
    } catch (err) {
      hilog.error(0x0000, 'RdbUtil', 'RDB: updateAddress failed: %{public}s', JSON.stringify(err));
    }
  }

  static async deleteAddress(id: number): Promise<void> {
    RdbUtil.memAddresses = RdbUtil.memAddresses.filter(a => a.id !== id);

    const store = await RdbUtil.getStore();
    if (!store) return;

    try {
      const pred = new relationalStore.RdbPredicates(Constants.TABLE_ADDRESS);
      pred.equalTo('id', id);
      await store.delete(pred);
    } catch (err) {
      hilog.error(0x0000, 'RdbUtil', 'RDB: deleteAddress failed: %{public}s', JSON.stringify(err));
    }
  }

  static async setDefaultAddress(userId: number, id: number): Promise<void> {
    for (const a of RdbUtil.memAddresses) {
      if (a.userId === userId) {
        a.isDefault = a.id === id;
      }
    }

    const store = await RdbUtil.getStore();
    if (!store) return;

    try {
      store.beginTransaction();

      const clearBucket: relationalStore.ValuesBucket = { isDefault: 0 };
      const clearPred = new relationalStore.RdbPredicates(Constants.TABLE_ADDRESS);
      clearPred.equalTo('userId', userId);
      await store.update(clearBucket, clearPred);

      const setBucket: relationalStore.ValuesBucket = { isDefault: 1 };
      const setPred = new relationalStore.RdbPredicates(Constants.TABLE_ADDRESS);
      setPred.equalTo('id', id);
      await store.update(setBucket, setPred);

      store.commit();
    } catch (err) {
      hilog.error(0x0000, 'RdbUtil', 'RDB: setDefaultAddress failed: %{public}s', JSON.stringify(err));
      store.rollBack();
    }
  }
}