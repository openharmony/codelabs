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
