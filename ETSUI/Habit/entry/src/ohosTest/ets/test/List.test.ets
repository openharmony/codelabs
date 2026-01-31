import abilityTest from './Ability.test';
import streakLogicTest from './StreakLogic.test';  // 引入刚才创建的测试文件

export default function testsuite() {
  abilityTest();
  // 注册新增的连胜逻辑测试
  streakLogicTest();
}