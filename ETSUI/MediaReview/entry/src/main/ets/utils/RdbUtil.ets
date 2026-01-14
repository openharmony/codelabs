/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

import relationalStore from '@ohos.data.relationalStore'
import cryptoFramework from '@ohos.security.cryptoFramework'

export class DbReview {
  id: string
  userName: string
  avatarKey: string
  rating: number
  content: string
  createdAt: string
  likeCount: number

  constructor(id: string, userName: string, avatarKey: string, rating: number, content: string, createdAt: string, likeCount: number) {
    this.id = id
    this.userName = userName
    this.avatarKey = avatarKey
    this.rating = rating
    this.content = content
    this.createdAt = createdAt
    this.likeCount = likeCount
  }
}

export class DbPost {
  id: string
  userName: string
  avatarKey: string
  content: string
  tags: string
  createdAt: string
  likeCount: number
  commentCount: number
  isLiked: boolean = false

  constructor(id: string, userName: string, avatarKey: string, content: string, tags: string, createdAt: string, likeCount: number, commentCount: number) {
    this.id = id
    this.userName = userName
    this.avatarKey = avatarKey
    this.content = content
    this.tags = tags
    this.createdAt = createdAt
    this.likeCount = likeCount
    this.commentCount = commentCount
  }
}

export class DbComment {
  id: string
  postId: string
  userName: string
  avatarKey: string
  content: string
  createdAt: string

  constructor(id: string, postId: string, userName: string, avatarKey: string, content: string, createdAt: string) {
    this.id = id
    this.postId = postId
    this.userName = userName
    this.avatarKey = avatarKey
    this.content = content
    this.createdAt = createdAt
  }
}

export class UserInfo {
  id: string
  name: string
  avatarKey: string

  constructor(id: string, name: string, avatarKey: string) {
    this.id = id
    this.name = name
    this.avatarKey = avatarKey
  }
}

export interface FavItem {
  mediaId: string
  type: string
}

interface UserMemItem {
  name: string
  avatarKey: string
  saltHex: string
  hashHex: string
  iterations: number
}

export interface UserStats {
  reviewCount: number
  favCount: number
  bookReviewCount: number
  movieReviewCount: number
}

export interface RatingStats {
  rating: number
  count: number
  sum: number
}

export interface StarStats {
  counts: number[]
  total: number
}

export class RdbUtil {
  // 数据层统一封装：优先走 RDB，异常或无库时用内存兜底
  private static store: relationalStore.RdbStore | null = null
  
  // opening 用于避免并发重复打开数据库
  private static opening: Promise<relationalStore.RdbStore | null> | null = null

  // 下方为内存缓存：用于无库场景与提升体验
  private static memory: Map<string, DbReview[]> = new Map()
  private static favMemory: Map<string, FavItem[]> = new Map()
  private static userMemory: Map<string, UserMemItem> = new Map()
  private static commentMemory: Map<string, DbComment[]> = new Map()
  private static postLikeMemory: Set<string> = new Set()
  private static postMemory: DbPost[] = [
    new DbPost('p1', '秋本丽子', 'qblz', '今年打算多读一些数学类的书，大家有什么推荐的吗？', '阅读计划,求推荐', '2026-01-12', 0, 0),
    new DbPost('p2', '拟宝珠缠', 'nbzc', '终于把《时间简史》看完了！强烈安利没看过的朋友赶紧冲！我要去二刷了！', '物理,宇宙', '2026-01-11', 0, 0),
    new DbPost('p3', '居间惠', 'jjh', '重温了经典电影《猛龙过江》，太好看了！', '经典电影,影评', '2026-01-10', 0, 0),
    new DbPost('p4', '中村由利', 'zcyl', '周末窝在沙发上听着雨声看书，真的是最惬意的时光。不需要多快的阅读速度，只是享受文字流淌在心间的感觉。', '生活片段,阅读时光', '2026-01-09', 0, 0)
  ]
  
  private static upsertMemory(bookId: string, review: DbReview): void {
    const list = RdbUtil.memory.get(bookId) ?? []
    const next: DbReview[] = []
    let replaced = false
    for (let i = 0; i < list.length; i++) {
      const it = list[i]
      if (it.id === review.id) {
        next.push(review)
        replaced = true
      } else {
        next.push(it)
      }
    }
    if (!replaced) {
      next.unshift(review)
    }
    RdbUtil.memory.set(bookId, next)
  }

  private static readMemory(bookId: string): DbReview[] {
    const list = RdbUtil.memory.get(bookId)
    return list ? [...list] : []
  }

  private static readonly DB_NAME: string = 'media_review.db'
  private static readonly TABLE: string = 'reviews'
  private static readonly CREATE_SQL: string =
    'CREATE TABLE IF NOT EXISTS reviews (' +
    'id TEXT PRIMARY KEY, ' +
    'bookId TEXT NOT NULL, ' +
    'userName TEXT, ' +
    'avatarKey TEXT, ' +
    'rating INTEGER, ' +
    'content TEXT, ' +
    'createdAt TEXT, ' +
    'likeCount INTEGER' +
    ')'

  private static readonly CREATE_USER_SQL: string =
    'CREATE TABLE IF NOT EXISTS user_profile (' +
    'id TEXT PRIMARY KEY, ' +
    'name TEXT, ' +
    'avatarKey TEXT' +
    ')'

  private static readonly USERS_TABLE: string = 'users'
  private static readonly PASSWORD_ITERATIONS: number = 10000
  private static readonly SALT_LEN: number = 16

  private static readonly CREATE_USERS_SQL: string =
    'CREATE TABLE IF NOT EXISTS users (' +
    'name TEXT PRIMARY KEY, ' +
    'avatarKey TEXT, ' +
    'saltHex TEXT, ' +
    'hashHex TEXT, ' +
    'iterations INTEGER' +
    ')'

  private static readonly CREATE_FAV_SQL: string =
    'CREATE TABLE IF NOT EXISTS user_favorites (' +
    'id TEXT PRIMARY KEY, ' +
    'userName TEXT, ' +
    'mediaId TEXT, ' +
    'type TEXT, ' +
    'createdAt TEXT' +
    ')'

  private static readonly CREATE_POSTS_SQL: string =
    'CREATE TABLE IF NOT EXISTS posts (' +
    'id TEXT PRIMARY KEY, ' +
    'userName TEXT, ' +
    'avatarKey TEXT, ' +
    'content TEXT, ' +
    'tags TEXT, ' +
    'createdAt TEXT, ' +
    'likeCount INTEGER, ' +
    'commentCount INTEGER' +
    ')'

  private static readonly CREATE_COMMENTS_SQL: string =
    'CREATE TABLE IF NOT EXISTS comments (' +
    'id TEXT PRIMARY KEY, ' +
    'postId TEXT, ' +
    'userName TEXT, ' +
    'avatarKey TEXT, ' +
    'content TEXT, ' +
    'createdAt TEXT' +
    ')'

  // 打开数据库并确保表结构存在
  private static async openStore(context: Context): Promise<relationalStore.RdbStore | null> {
    try {
      const config: relationalStore.StoreConfig = {
        name: RdbUtil.DB_NAME,
        securityLevel: relationalStore.SecurityLevel.S1
      }
      const store: relationalStore.RdbStore = await relationalStore.getRdbStore(context, config)
      await store.executeSql(RdbUtil.CREATE_SQL)
      try {
        await store.executeSql(`ALTER TABLE ${RdbUtil.TABLE} ADD COLUMN avatarKey TEXT`, [])
      } catch (e) {
      }
      await store.executeSql(RdbUtil.CREATE_USER_SQL)
      await store.executeSql(RdbUtil.CREATE_USERS_SQL)
      await store.executeSql(RdbUtil.CREATE_FAV_SQL)
      await store.executeSql(RdbUtil.CREATE_POSTS_SQL)
      await store.executeSql(RdbUtil.CREATE_COMMENTS_SQL)
      
      await RdbUtil.initMockPosts(store)

      return store
    } catch (e) {
      return null
    }
  }

  // 获取单例 store（带并发保护）
  private static async getStore(context: Context): Promise<relationalStore.RdbStore | null> {
    if (RdbUtil.store) return RdbUtil.store
    if (RdbUtil.opening) return await RdbUtil.opening

    RdbUtil.opening = RdbUtil.openStore(context)
    const s = await RdbUtil.opening
    RdbUtil.opening = null
    RdbUtil.store = s
    return s
  }

  // 评分/评价：写入或更新
  static async upsertReview(context: Context, bookId: string, review: DbReview): Promise<boolean> {
    RdbUtil.upsertMemory(bookId, review)
    try {
      const store = await RdbUtil.getStore(context)
      if (!store) return false
      const args: relationalStore.ValueType[] = [
        review.id,
        bookId,
        review.userName,
        review.avatarKey,
        review.rating,
        review.content,
        review.createdAt,
        review.likeCount
      ]
      await store.executeSql(
        `INSERT OR REPLACE INTO ${RdbUtil.TABLE} (id, bookId, userName, avatarKey, rating, content, createdAt, likeCount) VALUES (?, ?, ?, ?, ?, ?, ?, ?)`,
        args
      )
      return true
    } catch (e) {
      return false
    }
  }

  private static deleteMemory(bookId: string, reviewId: string): void {
    const list = RdbUtil.memory.get(bookId)
    if (!list) return
    const next: DbReview[] = []
    for (let i = 0; i < list.length; i++) {
      const it = list[i]
      if (it.id !== reviewId) {
        next.push(it)
      }
    }
    if (next.length > 0) {
      RdbUtil.memory.set(bookId, next)
    } else {
      RdbUtil.memory.delete(bookId)
    }
  }

  private static bytesToHex(bytes: Uint8Array): string {
    let out = ''
    for (let i = 0; i < bytes.length; i++) {
      out += bytes[i].toString(16).padStart(2, '0')
    }
    return out
  }

  private static hexToBytes(hex: string): Uint8Array {
    let s = hex.trim().toLowerCase()
    if (s.length % 2 === 1) {
      s = '0' + s
    }
    const len = Math.floor(s.length / 2)
    const out = new Uint8Array(len)
    for (let i = 0; i < len; i++) {
      out[i] = parseInt(s.slice(i * 2, i * 2 + 2), 16)
    }
    return out
  }

  private static concatBytes(a: Uint8Array, b: Uint8Array): Uint8Array {
    const out = new Uint8Array(a.length + b.length)
    out.set(a, 0)
    out.set(b, a.length)
    return out
  }

  private static utf8Bytes(input: string): Uint8Array {
    const out: number[] = []
    let i = 0
    while (i < input.length) {
      let codePoint = input.charCodeAt(i)
      if (codePoint >= 0xd800 && codePoint <= 0xdbff && i + 1 < input.length) {
        const next = input.charCodeAt(i + 1)
        if (next >= 0xdc00 && next <= 0xdfff) {
          codePoint = 0x10000 + ((codePoint - 0xd800) << 10) + (next - 0xdc00)
          i += 2
        } else {
          i += 1
        }
      } else {
        i += 1
      }

      if (codePoint <= 0x7f) {
        out.push(codePoint)
      } else if (codePoint <= 0x7ff) {
        out.push(0xc0 | (codePoint >> 6))
        out.push(0x80 | (codePoint & 0x3f))
      } else if (codePoint <= 0xffff) {
        out.push(0xe0 | (codePoint >> 12))
        out.push(0x80 | ((codePoint >> 6) & 0x3f))
        out.push(0x80 | (codePoint & 0x3f))
      } else {
        out.push(0xf0 | (codePoint >> 18))
        out.push(0x80 | ((codePoint >> 12) & 0x3f))
        out.push(0x80 | ((codePoint >> 6) & 0x3f))
        out.push(0x80 | (codePoint & 0x3f))
      }
    }
    return new Uint8Array(out)
  }

  private static async sha256(data: Uint8Array): Promise<Uint8Array> {
    const md = cryptoFramework.createMd('SHA256')
    await md.update({ data: data })
    const out = await md.digest()
    return out.data
  }

  private static async randomBytes(len: number): Promise<Uint8Array> {
    const r = cryptoFramework.createRandom()
    const out = await r.generateRandom(len)
    return out.data
  }

  private static async derivePasswordHash(password: string, salt: Uint8Array, iterations: number): Promise<Uint8Array> {
    const pass: Uint8Array = RdbUtil.utf8Bytes(password)
    let h = await RdbUtil.sha256(RdbUtil.concatBytes(salt, pass))
    for (let i = 1; i < iterations; i++) {
      h = await RdbUtil.sha256(RdbUtil.concatBytes(h, salt))
    }
    return h
  }

  // 收藏：增/删/查
  static async addFavorite(context: Context, userName: string, mediaId: string, type: string): Promise<boolean> {
    const list = RdbUtil.favMemory.get(userName) ?? []
    if (!list.find(it => it.mediaId === mediaId)) {
      list.unshift({ mediaId, type })
      RdbUtil.favMemory.set(userName, list)
    }

    try {
      const store = await RdbUtil.getStore(context)
      if (!store) return true
      const id = `${userName}_${mediaId}`
      const now = new Date().toISOString()
      await store.executeSql(
        'INSERT OR REPLACE INTO user_favorites (id, userName, mediaId, type, createdAt) VALUES (?, ?, ?, ?, ?)',
        [id, userName, mediaId, type, now]
      )
      return true
    } catch (e) {
      return true
    }
  }

  static async removeFavorite(context: Context, userName: string, mediaId: string): Promise<boolean> {
    const list = RdbUtil.favMemory.get(userName)
    if (list) {
      const next: FavItem[] = []
      for (const item of list) {
        if (item.mediaId !== mediaId) {
          next.push(item)
        }
      }
      RdbUtil.favMemory.set(userName, next)
    }

    try {
      const store = await RdbUtil.getStore(context)
      if (!store) return true
      const id = `${userName}_${mediaId}`
      await store.executeSql('DELETE FROM user_favorites WHERE id = ?', [id])
      return true
    } catch (e) {
      return true
    }
  }

  static async isFavorite(context: Context, userName: string, mediaId: string): Promise<boolean> {
    const list = RdbUtil.favMemory.get(userName)
    if (list && list.find(it => it.mediaId === mediaId)) {
      return true
    }

    let rs: relationalStore.ResultSet | null = null
    try {
      const store = await RdbUtil.getStore(context)
      if (!store) return false
      const id = `${userName}_${mediaId}`
      rs = await store.querySql('SELECT id FROM user_favorites WHERE id = ?', [id])
      const exists = rs.rowCount > 0
      return exists
    } catch (e) {
      return false
    } finally {
      if (rs) rs.close()
    }
  }

  static async getUserFavorites(context: Context, userName: string): Promise<FavItem[]> {
    const memList = RdbUtil.favMemory.get(userName) ?? []
    
    let rs: relationalStore.ResultSet | null = null
    try {
      const store = await RdbUtil.getStore(context)
      if (!store) {
        return memList
      }
      rs = await store.querySql(
        'SELECT mediaId, type FROM user_favorites WHERE userName = ? ORDER BY createdAt DESC',
        [userName]
      )
      const out: FavItem[] = []
      if (rs.rowCount > 0 && rs.goToFirstRow()) {
        const idxId = rs.getColumnIndex('mediaId')
        const idxType = rs.getColumnIndex('type')
        do {
          const item: FavItem = {
            mediaId: rs.getString(idxId),
            type: rs.getString(idxType)
          }
          out.push(item)
        } while (rs.goToNextRow())
      }
      
      const combined: FavItem[] = [...out]
      for (const m of memList) {
        if (!combined.find(it => it.mediaId === m.mediaId)) {
          combined.unshift(m)
        }
      }
      
      if (combined.length > 0) {
        RdbUtil.favMemory.set(userName, combined)
      }
      
      return combined
    } catch (e) {
      return memList
    } finally {
      if (rs) rs.close()
    }
  }

  // 用户：注册/登录/当前用户维护
  static async registerUser(context: Context, name: string, password: string, avatarKey: string): Promise<boolean> {
    const n = name.trim()
    if (n.length <= 0) return false
    if (password.length <= 0) return false

    if (RdbUtil.userMemory.has(n)) {
      return false
    }

    let rs: relationalStore.ResultSet | null = null
    try {
      const store = await RdbUtil.getStore(context)
      if (!store) {
        const salt = await RdbUtil.randomBytes(RdbUtil.SALT_LEN)
        const hash = await RdbUtil.derivePasswordHash(password, salt, RdbUtil.PASSWORD_ITERATIONS)
        RdbUtil.userMemory.set(n, {
          name: n,
          avatarKey: avatarKey,
          saltHex: RdbUtil.bytesToHex(salt),
          hashHex: RdbUtil.bytesToHex(hash),
          iterations: RdbUtil.PASSWORD_ITERATIONS
        })
        return true
      }

      rs = await store.querySql(`SELECT name FROM ${RdbUtil.USERS_TABLE} WHERE name = ?`, [n])
      const exists = rs.rowCount > 0
      rs.close()
      rs = null
      if (exists) return false

      const salt = await RdbUtil.randomBytes(RdbUtil.SALT_LEN)
      const hash = await RdbUtil.derivePasswordHash(password, salt, RdbUtil.PASSWORD_ITERATIONS)

      const saltHex = RdbUtil.bytesToHex(salt)
      const hashHex = RdbUtil.bytesToHex(hash)
      
      RdbUtil.userMemory.set(n, {
        name: n,
        avatarKey: avatarKey,
        saltHex: saltHex,
        hashHex: hashHex,
        iterations: RdbUtil.PASSWORD_ITERATIONS
      })

      await store.executeSql(
        `INSERT INTO ${RdbUtil.USERS_TABLE} (name, avatarKey, saltHex, hashHex, iterations) VALUES (?, ?, ?, ?, ?)`,
        [n, avatarKey, saltHex, hashHex, RdbUtil.PASSWORD_ITERATIONS]
      )
      return true
    } catch (e) {
      if (!RdbUtil.userMemory.has(n)) {
         const salt = await RdbUtil.randomBytes(RdbUtil.SALT_LEN)
         const hash = await RdbUtil.derivePasswordHash(password, salt, RdbUtil.PASSWORD_ITERATIONS)
         RdbUtil.userMemory.set(n, {
          name: n,
          avatarKey: avatarKey,
          saltHex: RdbUtil.bytesToHex(salt),
          hashHex: RdbUtil.bytesToHex(hash),
          iterations: RdbUtil.PASSWORD_ITERATIONS
        })
      }
      return true
    } finally {
      if (rs) rs.close()
    }
  }

  static async loginUser(context: Context, name: string, password: string): Promise<UserInfo | null> {
    const n = name.trim()
    if (n.length <= 0) return null
    if (password.length <= 0) return null

    const memUser = RdbUtil.userMemory.get(n)
    if (memUser) {
       const salt = RdbUtil.hexToBytes(memUser.saltHex)
       const calc = await RdbUtil.derivePasswordHash(password, salt, memUser.iterations)
       if (RdbUtil.bytesToHex(calc) === memUser.hashHex) {
         await RdbUtil.setCurrentUser(context, n, memUser.avatarKey)
         return new UserInfo('current', n, memUser.avatarKey)
       }
    }

    let rs: relationalStore.ResultSet | null = null
    try {
      const store = await RdbUtil.getStore(context)
      if (!store) {
        return null
      }

      rs = await store.querySql(
        `SELECT name, avatarKey, saltHex, hashHex, iterations FROM ${RdbUtil.USERS_TABLE} WHERE name = ?`,
        [n]
      )
      if (rs.rowCount <= 0 || !rs.goToFirstRow()) {
        return null
      }

      const idxAvatar = rs.getColumnIndex('avatarKey')
      const idxSalt = rs.getColumnIndex('saltHex')
      const idxHash = rs.getColumnIndex('hashHex')
      const idxIter = rs.getColumnIndex('iterations')

      const avatarKey = rs.getString(idxAvatar)
      const saltHex = rs.getString(idxSalt).trim().toLowerCase()
      const hashHex = rs.getString(idxHash).trim().toLowerCase()
      let iterations = Number(rs.getLong(idxIter))
      if (!(iterations > 0)) {
        iterations = RdbUtil.PASSWORD_ITERATIONS
      }

      const salt = RdbUtil.hexToBytes(saltHex)
      const calc = await RdbUtil.derivePasswordHash(password, salt, iterations)
      if (RdbUtil.bytesToHex(calc) !== hashHex) {
        return null
      }
      
      RdbUtil.userMemory.set(n, {
        name: n,
        avatarKey: avatarKey,
        saltHex: saltHex,
        hashHex: hashHex,
        iterations: iterations
      })

      const ok = await RdbUtil.setCurrentUser(context, n, avatarKey)
      return ok ? new UserInfo('current', n, avatarKey) : null
    } catch (e) {
      return null
    } finally {
      if (rs) rs.close()
    }
  }

  private static currentUser: UserInfo | null = null

  static async getCurrentUser(context: Context): Promise<UserInfo | null> {
    if (RdbUtil.currentUser) return RdbUtil.currentUser
    try {
      const store = await RdbUtil.getStore(context)
      if (!store) return null
      const rs: relationalStore.ResultSet = await store.querySql(
        'SELECT id, name, avatarKey FROM user_profile WHERE id = ?',
        ['current']
      )
      if (rs.rowCount > 0 && rs.goToFirstRow()) {
        const u = new UserInfo(rs.getString(0), rs.getString(1), rs.getString(2))
        rs.close()
        RdbUtil.currentUser = u
        return u
      }
      rs.close()
      return null
    } catch (e) {
      return null
    }
  }

  static async setCurrentUser(context: Context, name: string, avatarKey: string): Promise<boolean> {
    const u = new UserInfo('current', name, avatarKey)
    RdbUtil.currentUser = u
    try {
      const store = await RdbUtil.getStore(context)
      if (!store) return false
      const args: relationalStore.ValueType[] = [u.id, u.name, u.avatarKey]
      await store.executeSql(
        'INSERT OR REPLACE INTO user_profile (id, name, avatarKey) VALUES (?, ?, ?)',
        args
      )
      return true
    } catch (e) {
      return false
    }
  }

  static async clearCurrentUser(context: Context): Promise<boolean> {
    RdbUtil.currentUser = null
    try {
      const store = await RdbUtil.getStore(context)
      if (!store) return false
      await store.executeSql('DELETE FROM user_profile WHERE id = ?', ['current'])
      return true
    } catch (e) {
      return false
    }
  }

  static async deleteReview(context: Context, bookId: string, reviewId: string): Promise<boolean> {
    RdbUtil.deleteMemory(bookId, reviewId)
    try {
      const store = await RdbUtil.getStore(context)
      if (!store) return false
      await store.executeSql(
        `DELETE FROM ${RdbUtil.TABLE} WHERE bookId = ? AND id = ?`,
        [bookId, reviewId]
      )
      return true
    } catch (e) {
      return false
    }
  }

  // 评价查询与评分统计
  static async queryReviews(context: Context, bookId: string): Promise<DbReview[] | null> {
    let rs: relationalStore.ResultSet | null = null
    try {
      const store = await RdbUtil.getStore(context)
      if (!store) {
        return RdbUtil.readMemory(bookId)
      }

      rs = await store.querySql(
        `SELECT id, userName, avatarKey, rating, content, createdAt, likeCount FROM ${RdbUtil.TABLE} WHERE bookId = ? ORDER BY createdAt DESC, id DESC`,
        [bookId]
      )

      const out: DbReview[] = []
      if (rs.rowCount <= 0) {
        const mem = RdbUtil.readMemory(bookId)
        return mem.length > 0 ? mem : out
      }
      if (!rs.goToFirstRow()) {
        return out
      }

      const idxId: number = rs.getColumnIndex('id')
      const idxUser: number = rs.getColumnIndex('userName')
      const idxAvatar: number = rs.getColumnIndex('avatarKey')
      const idxRating: number = rs.getColumnIndex('rating')
      const idxContent: number = rs.getColumnIndex('content')
      const idxCreated: number = rs.getColumnIndex('createdAt')
      const idxLike: number = rs.getColumnIndex('likeCount')

      do {
        out.push(new DbReview(
          rs.getString(idxId),
          rs.getString(idxUser),
          rs.getString(idxAvatar),
          Number(rs.getLong(idxRating)),
          rs.getString(idxContent),
          rs.getString(idxCreated),
          Number(rs.getLong(idxLike))
        ))
      } while (rs.goToNextRow())

      if (out.length > 0) {
        RdbUtil.memory.set(bookId, [...out])
      }
      return out
    } catch (e) {
      const mem = RdbUtil.readMemory(bookId)
      return mem.length > 0 ? mem : null
    } finally {
      if (rs) {
        rs.close()
      }
    }
  }

  static async getRatingStats(context: Context, bookId: string): Promise<RatingStats> {
    const defaultStats: RatingStats = { rating: 0, count: 0, sum: 0 }
    
    const calcFromMemory = (): RatingStats => {
      const list = RdbUtil.memory.get(bookId) || []
      const valid = list.filter(r => r.rating > 0)
      const count = valid.length
      const sum = valid.reduce((acc, r) => acc + r.rating, 0)
      const rating = count > 0 ? sum / count : 0
      return { rating, count, sum }
    }

    try {
      const store = await RdbUtil.getStore(context)
      const memStats = calcFromMemory()
      if (!store) {
        return memStats
      }
      
      const sql = `SELECT AVG(rating) as avgRating, SUM(rating) as sumRating, COUNT(*) as count FROM ${RdbUtil.TABLE} WHERE bookId = ? AND rating > 0`
      const rs = await store.querySql(sql, [bookId])
      
      let rating = 0
      let count = 0
      let sum = 0
      
      if (rs.goToFirstRow()) {
        rating = rs.getDouble(rs.getColumnIndex('avgRating'))
        count = Number(rs.getLong(rs.getColumnIndex('count')))
        sum = rs.getDouble(rs.getColumnIndex('sumRating'))
      }
      rs.close()
      
      if (memStats.count > count) {
        return memStats
      }
      return { rating: rating || 0, count: count, sum: sum || 0 }
    } catch (e) {
      return calcFromMemory()
    }
  }

  static computeBaseDistribution(rating10: number, ratingCount: number): number[] {
    const avgStar = Math.max(1, Math.min(5, rating10 / 2))
    const p = (() => {
      const r = rating10
      if (r >= 9.0) return [2, 6, 12, 25, 55]
      if (r >= 8.5) return [4, 8, 15, 28, 45]
      if (r >= 8.0) return [7, 10, 18, 30, 35]
      return [15, 15, 20, 25, 25]
    })()
    const counts = [0, 0, 0, 0, 0]
    let sumCounts = 0
    for (let i = 0; i < 5; i++) {
      counts[i] = Math.max(0, Math.round(p[i] * ratingCount / 100))
      sumCounts += counts[i]
    }
    if (sumCounts !== ratingCount) {
      let diff = ratingCount - sumCounts
      const dir = diff > 0 ? 1 : -1
      diff = Math.abs(diff)
      for (let k = 0; k < diff; k++) {
        for (let i = 4; i >= 0 && k < diff; i--) {
          if (dir > 0) {
            counts[i]++
          } else if (counts[i] > 0) {
            counts[i]--
          }
          k++
        }
      }
    }
    const curAvg = counts.reduce((acc, c, i) => acc + c * (i + 1), 0) / Math.max(1, ratingCount)
    let need = Math.round((avgStar - curAvg) * ratingCount)
    while (need > 0) {
      let changed = false
      for (let i = 4; i >= 1 && need > 0; i--) {
        const j = i - 1
        if (counts[j] > 0) {
          counts[j]--
          counts[i]++
          need--
          changed = true
        }
      }
      if (!changed) break
    }
    while (need < 0) {
      let changed = false
      for (let i = 0; i <= 3 && need < 0; i++) {
        const j = i + 1
        if (counts[j] > 0) {
          counts[j]--
          counts[i]++
          need++
          changed = true
        }
      }
      if (!changed) break
    }
    return counts
  }

  static async getStarStats(context: Context, bookId: string): Promise<StarStats> {
    const list = await RdbUtil.queryReviews(context, bookId)
    const counts = [0, 0, 0, 0, 0]
    if (list) {
      for (const r of list) {
        if (r.rating > 0 && r.rating <= 5) {
          counts[r.rating - 1]++
        }
      }
    }
    const total = counts.reduce((a, b) => a + b, 0)
    return { counts, total }
  }

  static isPostLiked(userName: string, postId: string): boolean {
    if (!userName) return false
    return RdbUtil.postLikeMemory.has(`${userName}_${postId}`)
  }

  static async togglePostLike(context: Context, userName: string, postId: string): Promise<boolean> {
    if (!userName) return false
    const key = `${userName}_${postId}`
    const like = !RdbUtil.postLikeMemory.has(key)
    if (like) RdbUtil.postLikeMemory.add(key); else RdbUtil.postLikeMemory.delete(key)

    const memPost = RdbUtil.postMemory.find(p => p.id === postId)
    if (memPost) {
      memPost.likeCount = Math.max(0, (memPost.likeCount || 0) + (like ? 1 : -1))
    }

    try {
      const store = await RdbUtil.getStore(context)
      if (!store) return true
      const predicates = new relationalStore.RdbPredicates('posts')
      predicates.equalTo('id', postId)
      const rs = await store.query(predicates)
      let count = 0
      if (rs.goToFirstRow()) {
        count = Number(rs.getLong(rs.getColumnIndex('likeCount')))
      }
      rs.close()
      const newCount = Math.max(0, count + (like ? 1 : -1))
      const bucket: relationalStore.ValuesBucket = { likeCount: newCount }
      await store.update(bucket, predicates)
      return true
    } catch (e) {
      return true
    }
  }

  // 社区：帖子与评论
  static async getPosts(context: Context, currentUserName?: string): Promise<DbPost[]> {
    try {
      const store = await RdbUtil.getStore(context)
      const user = currentUserName ?? (RdbUtil.currentUser ? RdbUtil.currentUser.name : undefined)
      if (!store) {
        const outList = [...RdbUtil.postMemory]
        if (user) {
          for (const p of outList) {
            p.isLiked = RdbUtil.isPostLiked(user, p.id)
          }
        }
        return outList
      }
      const predicates = new relationalStore.RdbPredicates('posts')
      predicates.orderByDesc('createdAt')
      let rs = await store.query(predicates)
      if (rs.rowCount === 0) {
        rs.close()
        await RdbUtil.initMockPosts(store)
        rs = await store.query(predicates)
      }
      const list: DbPost[] = []
      if (rs.goToFirstRow()) {
        do {
          const post = new DbPost(
            rs.getString(rs.getColumnIndex('id')),
            rs.getString(rs.getColumnIndex('userName')),
            rs.getString(rs.getColumnIndex('avatarKey')),
            rs.getString(rs.getColumnIndex('content')),
            rs.getString(rs.getColumnIndex('tags')),
            rs.getString(rs.getColumnIndex('createdAt')),
            rs.getLong(rs.getColumnIndex('likeCount')),
            rs.getLong(rs.getColumnIndex('commentCount'))
          )
          list.push(post)
        } while (rs.goToNextRow())
      }
      rs.close()
      const outList = list.length > 0 ? list : [...RdbUtil.postMemory]
      if (user) {
        for (const p of outList) {
          p.isLiked = RdbUtil.isPostLiked(user, p.id)
        }
      }
      return outList
    } catch (e) {
      const user = currentUserName ?? (RdbUtil.currentUser ? RdbUtil.currentUser.name : undefined)
      const outList = [...RdbUtil.postMemory]
      if (user) {
        for (const p of outList) {
          p.isLiked = RdbUtil.isPostLiked(user, p.id)
        }
      }
      return outList
    }
  }

  static async getComments(context: Context, postId: string): Promise<DbComment[]> {
    const mem = RdbUtil.commentMemory.get(postId) || []
    try {
      const store = await RdbUtil.getStore(context)
      if (!store) {
        return [...mem]
      }
      
      const predicates = new relationalStore.RdbPredicates('comments')
      predicates.equalTo('postId', postId)
      predicates.orderByDesc('createdAt')
      const rs = await store.query(predicates)
      
      const list: DbComment[] = []
      if (rs.goToFirstRow()) {
        do {
          list.push(new DbComment(
            rs.getString(rs.getColumnIndex('id')),
            rs.getString(rs.getColumnIndex('postId')),
            rs.getString(rs.getColumnIndex('userName')),
            rs.getString(rs.getColumnIndex('avatarKey')),
            rs.getString(rs.getColumnIndex('content')),
            rs.getString(rs.getColumnIndex('createdAt'))
          ))
        } while (rs.goToNextRow())
      }
      rs.close()
      
      const dbIds = new Set(list.map(c => c.id))
      const combined = [...list]
      for (const m of mem) {
        if (!dbIds.has(m.id)) {
          combined.push(m)
        }
      }
      
      combined.sort((a, b) => b.id.localeCompare(a.id))
      
      RdbUtil.commentMemory.set(postId, [...combined])
      
      return combined
    } catch (e) {
      return [...mem]
    }
  }

  static async addComment(context: Context, comment: DbComment): Promise<boolean> {
    const list = RdbUtil.commentMemory.get(comment.postId) || []
    list.unshift(comment)
    RdbUtil.commentMemory.set(comment.postId, list)

    const memPost = RdbUtil.postMemory.find(p => p.id === comment.postId)
    if (memPost) {
      memPost.commentCount = (memPost.commentCount || 0) + 1
    }

    try {
      const store = await RdbUtil.getStore(context)
      if (!store) return true
      
      const bucket: relationalStore.ValuesBucket = {
        id: comment.id,
        postId: comment.postId,
        userName: comment.userName,
        avatarKey: comment.avatarKey,
        content: comment.content,
        createdAt: comment.createdAt
      }
      await store.insert('comments', bucket)

      const predicates = new relationalStore.RdbPredicates('posts')
      predicates.equalTo('id', comment.postId)
      const rs = await store.query(predicates)
      let count = 0
      if (rs.goToFirstRow()) {
        count = Number(rs.getLong(rs.getColumnIndex('commentCount')))
      }
      rs.close()
      
      const newCount = count + 1
      const updateBucket: relationalStore.ValuesBucket = { commentCount: newCount }
      await store.update(updateBucket, predicates)

      return true
    } catch (e) {
      return true
    }
  }

  static async deleteComment(context: Context, comment: DbComment): Promise<boolean> {
    const list = RdbUtil.commentMemory.get(comment.postId)
    if (list) {
      const next = list.filter(c => c.id !== comment.id)
      RdbUtil.commentMemory.set(comment.postId, next)
    }

    const memPost = RdbUtil.postMemory.find(p => p.id === comment.postId)
    if (memPost) {
      memPost.commentCount = Math.max(0, (memPost.commentCount || 0) - 1)
    }

    try {
      const store = await RdbUtil.getStore(context)
      if (!store) return true

      const predicates = new relationalStore.RdbPredicates('comments')
      predicates.equalTo('id', comment.id)
      await store.delete(predicates)

      const postPredicates = new relationalStore.RdbPredicates('posts')
      postPredicates.equalTo('id', comment.postId)
      const rs = await store.query(postPredicates)
      let count = 0
      if (rs.goToFirstRow()) {
        count = Number(rs.getLong(rs.getColumnIndex('commentCount')))
      }
      rs.close()

      const newCount = Math.max(0, count - 1)
      const updateBucket: relationalStore.ValuesBucket = { commentCount: newCount }
      await store.update(updateBucket, postPredicates)

      return true
    } catch (e) {
      return true
    }
  }

  static async createPost(context: Context, post: DbPost): Promise<boolean> {
    RdbUtil.postMemory.unshift(post)
    try {
      const store = await RdbUtil.getStore(context)
      if (!store) return false
      const bucket: relationalStore.ValuesBucket = {
        id: post.id,
        userName: post.userName,
        avatarKey: post.avatarKey,
        content: post.content,
        tags: post.tags,
        createdAt: post.createdAt,
        likeCount: post.likeCount,
        commentCount: post.commentCount
      }
      await store.insert('posts', bucket)
      return true
    } catch (e) {
      return false
    }
  }

  static async deletePost(context: Context, postId: string): Promise<boolean> {
    RdbUtil.postMemory = RdbUtil.postMemory.filter(p => p.id !== postId)
    RdbUtil.commentMemory.delete(postId)
    try {
      const store = await RdbUtil.getStore(context)
      if (!store) return true
      const cPred = new relationalStore.RdbPredicates('comments')
      cPred.equalTo('postId', postId)
      await store.delete(cPred)
      const pPred = new relationalStore.RdbPredicates('posts')
      pPred.equalTo('id', postId)
      await store.delete(pPred)
      return true
    } catch (e) {
      return true
    }
  }

  static async getUserStats(context: Context, userName: string): Promise<UserStats> {
    const stats: UserStats = {
      reviewCount: 0,
      favCount: 0,
      bookReviewCount: 0,
      movieReviewCount: 0
    }

    try {
      const store = await RdbUtil.getStore(context)
      if (!store) return stats

      const favPredicates = new relationalStore.RdbPredicates('user_favorites')
      favPredicates.equalTo('userName', userName)
      const favResultSet = await store.query(favPredicates)
      stats.favCount = favResultSet.rowCount
      favResultSet.close()

      const reviewPredicates = new relationalStore.RdbPredicates('reviews')
      reviewPredicates.equalTo('userName', userName)
      const reviewResultSet = await store.query(reviewPredicates)
      stats.reviewCount = reviewResultSet.rowCount
      
      if (reviewResultSet.goToFirstRow()) {
        do {
          const bookId = reviewResultSet.getString(reviewResultSet.getColumnIndex('bookId'))
          if (bookId.startsWith('movie_')) {
            stats.movieReviewCount++
          } else {
            stats.bookReviewCount++
          }
        } while (reviewResultSet.goToNextRow())
      }
      reviewResultSet.close()

    } catch (e) {
      console.error('getUserStats error:', e)
    }
    return stats
  }

  private static async initMockPosts(store: relationalStore.RdbStore): Promise<void> {
    const rs = await store.query(new relationalStore.RdbPredicates('posts'))
    if (rs.rowCount === 0) {
      const posts: DbPost[] = [
        new DbPost('p1', '秋本丽子', 'qblz', '最近在整理书单，发现去年读的技术类书籍占比太高了。今年打算多读一些人文社科类的书，平衡一下思维模式。大家有什么推荐的吗？最好是历史或哲学入门级的。', '阅读计划,求推荐', '2026-01-12', 0, 0),
        new DbPost('p2', '拟宝珠缠', 'nbzc', '终于把《三体》看完了！太震撼了！尤其是最后那个...（不剧透了）。感觉自己的宇宙观都被刷新了。强烈安利没看过的朋友赶紧冲！我要去二刷了！', '三体,科幻,神作', '2026-01-11', 0, 0),
        new DbPost('p3', '居间惠', 'jjh', '重温了经典老电影《教父》，每一次看都有新的体会。年轻时看的是江湖义气，现在看的是家族责任和权力更迭的无奈。帕西诺的眼神戏真的绝了。', '经典电影,教父,影评', '2026-01-10', 0, 0),
        new DbPost('p4', '中村由利', 'zcyl', '周末窝在沙发上听着雨声看书，真的是最惬意的时光。不需要多快的阅读速度，只是享受文字流淌在心间的感觉。配上一杯热咖啡，完美。', '生活碎片,阅读时光', '2026-01-09', 0, 0)
      ]
      for (const p of posts) {
        const bucket: relationalStore.ValuesBucket = {
          id: p.id,
          userName: p.userName,
          avatarKey: p.avatarKey,
          content: p.content,
          tags: p.tags,
          createdAt: p.createdAt,
          likeCount: p.likeCount,
          commentCount: p.commentCount
        }
        await store.insert('posts', bucket)
      }
    }
    rs.close()
  }
}