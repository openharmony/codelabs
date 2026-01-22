/**
 * 单元测试入口文件
 * 注册所有测试模块
 */
import localUnitTest from './LocalUnit.test';
import scheduleModelTest from './model/Schedule.test';
import tagModelTest from './model/Tag.test';
import reminderModelTest from './model/Reminder.test';
import recordingModelTest from './model/Recording.test';

export default function testsuite() {
  // 基础测试
  localUnitTest();
  
  // 模型单元测试
  scheduleModelTest();
  tagModelTest();
  reminderModelTest();
  recordingModelTest();
}