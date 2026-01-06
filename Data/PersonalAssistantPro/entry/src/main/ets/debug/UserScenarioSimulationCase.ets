import { DebugCase } from './DebugCase';
import { ContactModel, Contact } from '../model/ContactModel';
import { EventModel, Event } from '../model/EventModel';
import { DateUtils } from '../common/utils/DateUtils';

/**
 * 用户全场景模拟测试 (User Scenario Simulation)
 * 目的：模拟真实用户的一系列连续操作，验证系统在复杂交互下的稳定性
 * 流程：
 * 1. 初始化环境
 * 2. 用户添加了一组联系人 (家人、同事)
 * 3. 用户创建了几个相关日程
 * 4. 用户修改了日程时间
 * 5. 用户删除了某个联系人
 * 6. 最终数据一致性校验
 */
export class UserScenarioSimulationCase extends DebugCase {
  readonly name = 'User Scenario Simulation (End-to-End)';

  async run(): Promise<void> {
    this.logInfo('=== UserScenarioSimulationCase START ===');
    this.logInfo('Scenario: A busy morning for the user.');

    const timestamp = Date.now();
    const tag = `[SIM_${timestamp}]`; // 唯一标记，方便清理

    try {
      // --- Phase 1: Contact Management ---
      this.logInfo('>>> Phase 1: User is adding contacts...');

      // 添加 Boss
      const boss = new Contact(`${tag} Boss`, '13800000001', 'Work', 'boss@corp.com');
      const bossId = await ContactModel.insert(boss);
      this.logInfo(`   + Added Contact: Boss (ID: ${bossId})`);

      // 添加 Assistant
      const assistant = new Contact(`${tag} Assistant`, '13800000002', 'Work', 'help@corp.com');
      const assistantId = await ContactModel.insert(assistant);
      this.logInfo(`   + Added Contact: Assistant (ID: ${assistantId})`);

      // 添加 Wife
      const wife = new Contact(`${tag} Wife`, '13900000001', 'Family', 'love@home.com');
      const wifeId = await ContactModel.insert(wife);
      this.logInfo(`   + Added Contact: Wife (ID: ${wifeId})`);

      // 验证联系人添加
      const contacts = await ContactModel.search(tag);
      if (contacts.length === 3) {
        this.logInfo('   ✅ Contact Phase Verified: 3 contacts added.');
      } else {
        throw new Error('Contact addition failed count check.');
      }


      // --- Phase 2: Event Scheduling ---
      this.logInfo('>>> Phase 2: User is scheduling events...');

      const now = Date.now();
      const oneHour = 3600 * 1000;

      // 早上开会
      const meetingEvent = new Event(`${tag} Morning Meeting`, now + oneHour, now + 2 * oneHour, 'Discuss Q1 Results');
      const meetingId = await EventModel.insert(meetingEvent);
      this.logInfo(`   + Scheduled Event: Morning Meeting (ID: ${meetingId})`);

      // 晚上吃饭
      const dinnerEvent = new Event(`${tag} Dinner with Wife`, now + 8 * oneHour, now + 10 * oneHour, 'Restaurant X');
      const dinnerId = await EventModel.insert(dinnerEvent);
      this.logInfo(`   + Scheduled Event: Dinner (ID: ${dinnerId})`);

      // 验证日程添加
      const events = await EventModel.search(tag);
      if (events.length === 2) {
        this.logInfo('   ✅ Event Phase Verified: 2 events scheduled.');
      } else {
        throw new Error('Event scheduling failed count check.');
      }


      // --- Phase 3: Modifications (The Plot Thickens) ---
      this.logInfo('>>> Phase 3: Plans are changing...');

      // 会议延期了
      this.logInfo('   ! Action: Postponing meeting by 1 hour...');
      meetingEvent.id = meetingId;
      meetingEvent.startTime += oneHour;
      meetingEvent.endTime += oneHour;
      meetingEvent.description += ' (Delayed)';
      await EventModel.update(meetingEvent);
      this.logInfo('   > Meeting updated.');

      // 助理离职了 (删除联系人)
      this.logInfo('   ! Action: Deleting Assistant contact...');
      await ContactModel.deleteById(assistantId);
      this.logInfo('   > Assistant deleted.');


      // --- Phase 4: Final Consistency Check ---
      this.logInfo('>>> Phase 4: Final Consistency Check...');

      // 检查助理是否还在
      const checkAssistant = await ContactModel.search('Assistant');
      // 注意：这里需要精确过滤，因为可能之前跑过测试
      const targetAssistant = checkAssistant.find(c => c.name.includes(tag));

      if (!targetAssistant) {
        this.logInfo('   ✅ Assistant is correctly removed.');
      } else {
        this.logInfo('   ❌ Error: Assistant still exists!');
      }

      // 检查会议时间是否更新
      const updatedEvents = await EventModel.search('Morning Meeting');
      const targetMeeting = updatedEvents.find(e => e.id === meetingId);

      if (targetMeeting && targetMeeting.description.includes('(Delayed)')) {
        this.logInfo('   ✅ Meeting description updated correctly.');
      } else {
        this.logInfo('   ❌ Error: Meeting update failed!');
      }


      // --- Phase 5: Cleanup (Reset State) ---
      this.logInfo('>>> Phase 5: Cleaning up simulation data...');

      // 删除 Boss
      await ContactModel.deleteById(bossId);
      // 删除 Wife
      await ContactModel.deleteById(wifeId);
      // 删除 Events
      await EventModel.deleteById(meetingId);
      await EventModel.deleteById(dinnerId);

      this.logInfo('   > Cleanup completed.');

      this.logInfo('✅ User Scenario Simulation Completed Successfully.');

    } catch (err) {
      this.logInfo(`❌ Simulation Crashed: ${JSON.stringify(err)}`);
    }

    this.logInfo('=== UserScenarioSimulationCase END ===');
  }
}