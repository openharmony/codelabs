/**
 * 用户模型
 */
export interface User {
  userId?: number; // 用户ID（自增）
  username: string; // 用户名（唯一）
  password: string; // 密码（实际项目建议加密，这里简化）
  createTime?: string; // 创建时间
  lastLoginTime?: string; // 最后登录时间
}

/**
 * 登录/注册表单状态
 */
export interface AuthFormState {
  username: string;
  password: string;
  confirmPassword: string; // 注册时的确认密码
  usernameError: string;
  passwordError: string;
  confirmPasswordError: string;
}