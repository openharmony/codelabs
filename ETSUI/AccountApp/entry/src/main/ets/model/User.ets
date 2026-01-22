import relationalStore from '@ohos.data.relationalStore'; // 【新增】必须导入这个包

/**
 * User.ets
 * 用户实体类
 * 用于定义应用中的用户信息结构，对应数据库中的 User 表
 */
export default class User {
  // 定义私有属性
  private id: number;
  private username: string;
  private password: string;
  private phone: string;
  private email: string;
  private avatar: string;
  private createTime: number;

  /**
   * 构造函数
   * @param username 用户名
   * @param password 密码
   * @param phone 手机号
   */
  constructor(username: string, password: string, phone: string = '') {
    this.id = 0;
    this.username = username;
    this.password = password;
    this.phone = phone;
    this.email = '';
    this.avatar = '/common/images/default_avatar.png';
    this.createTime = new Date().getTime();
  }

  // ==========================================
  // Getter 和 Setter 方法
  // ==========================================

  public getId(): number {
    return this.id;
  }

  public setId(id: number): void {
    this.id = id;
  }

  public getUsername(): string {
    return this.username;
  }

  public setUsername(username: string): void {
    this.username = username;
  }

  public getPassword(): string {
    return this.password;
  }

  public setPassword(password: string): void {
    this.password = password;
  }

  public getPhone(): string {
    return this.phone;
  }

  public setPhone(phone: string): void {
    this.phone = phone;
  }

  public getEmail(): string {
    return this.email;
  }

  public setEmail(email: string): void {
    this.email = email;
  }

  public getCreateTime(): number {
    return this.createTime;
  }

  /**
   * 将当前对象转换为用于数据库存储的键值对
   * 【修改点】这里将 : any 修改为 : relationalStore.ValuesBucket
   */
  public toValuesBucket(): relationalStore.ValuesBucket {
    return {
      'username': this.username,
      'password': this.password,
      'phone': this.phone,
      'email': this.email,
      'avatar': this.avatar,
      'create_time': this.createTime
    };
  }
}