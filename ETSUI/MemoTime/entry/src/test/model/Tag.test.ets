/**
 * Tag 模型单元测试
 */
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from '@ohos/hypium';
import { Tag, TagInit, DEFAULT_TAGS } from '../../main/ets/model/Tag';
import { relationalStore } from '@kit.ArkData';

export default function tagModelTest() {
  describe('TagModelTest', () => {
    
    describe('构造函数测试', () => {
      it('创建默认 Tag 实例', 0, () => {
        const tag = new Tag();
        
        expect(tag.id).assertContain('tag-');
        expect(tag.name).assertEqual('');
        expect(tag.color).assertEqual('#4ECDC4');
        expect(tag.icon).assertEqual('');
        expect(tag.scheduleCount).assertEqual(0);
        expect(tag.createdAt).assertLarger(0);
      });

      it('使用 TagInit 创建实例', 0, () => {
        const init: TagInit = {
          id: 'custom-tag-001',
          name: '工作',
          color: '#FF6B6B',
          icon: 'work_icon',
          scheduleCount: 5
        };
        
        const tag = new Tag(init);
        
        expect(tag.id).assertEqual('custom-tag-001');
        expect(tag.name).assertEqual('工作');
        expect(tag.color).assertEqual('#FF6B6B');
        expect(tag.icon).assertEqual('work_icon');
        expect(tag.scheduleCount).assertEqual(5);
      });

      it('部分初始化参数创建实例', 0, () => {
        const init: TagInit = {
          name: '会议',
          color: '#45B7D1'
        };
        
        const tag = new Tag(init);
        
        expect(tag.id).assertContain('tag-');  // 自动生成
        expect(tag.name).assertEqual('会议');
        expect(tag.color).assertEqual('#45B7D1');
        expect(tag.icon).assertEqual('');
        expect(tag.scheduleCount).assertEqual(0);
      });

      it('空初始化参数创建实例', 0, () => {
        const init: TagInit = {};
        
        const tag = new Tag(init);
        
        expect(tag.id).assertContain('tag-');
        expect(tag.name).assertEqual('');
        expect(tag.color).assertEqual('#4ECDC4');
      });
    });

    describe('toDbObject 方法测试', () => {
      it('转换为数据库对象', 0, () => {
        const tag = new Tag({
          id: 'db-tag-001',
          name: '学习',
          color: '#96CEB4',
          icon: 'study_icon',
          scheduleCount: 10
        });
        
        const dbObj = tag.toDbObject();
        
        expect(dbObj['id']).assertEqual('db-tag-001');
        expect(dbObj['name']).assertEqual('学习');
        expect(dbObj['color']).assertEqual('#96CEB4');
        expect(dbObj['icon']).assertEqual('study_icon');
        expect(dbObj['scheduleCount']).assertEqual(10);
        expect(dbObj['createdAt']).assertLarger(0);
      });
    });

    describe('fromDbObject 静态方法测试', () => {
      it('从数据库对象创建实例', 0, () => {
        const now = Date.now();
        const dbObj: relationalStore.ValuesBucket = {
          id: 'from-db-tag',
          name: '运动',
          color: '#FFEAA7',
          icon: 'sport_icon',
          scheduleCount: 8,
          createdAt: now
        };
        
        const tag = Tag.fromDbObject(dbObj);
        
        expect(tag.id).assertEqual('from-db-tag');
        expect(tag.name).assertEqual('运动');
        expect(tag.color).assertEqual('#FFEAA7');
        expect(tag.icon).assertEqual('sport_icon');
        expect(tag.scheduleCount).assertEqual(8);
        expect(tag.createdAt).assertEqual(now);
      });
    });

    describe('默认标签测试', () => {
      it('默认标签数量正确', 0, () => {
        expect(DEFAULT_TAGS.length).assertEqual(7);
      });

      it('默认标签包含工作标签', 0, () => {
        const workTag = DEFAULT_TAGS.find(tag => tag.id === 'tag-work');
        expect(workTag !== undefined).assertTrue();
        if (workTag !== undefined) {
          expect(workTag.name).assertEqual('工作');
          expect(workTag.color).assertEqual('#FF6B6B');
        }
      });

      it('默认标签包含会议标签', 0, () => {
        const meetingTag = DEFAULT_TAGS.find(tag => tag.id === 'tag-meeting');
        expect(meetingTag !== undefined).assertTrue();
        if (meetingTag !== undefined) {
          expect(meetingTag.name).assertEqual('会议');
          expect(meetingTag.color).assertEqual('#4ECDC4');
        }
      });

      it('默认标签包含学习标签', 0, () => {
        const studyTag = DEFAULT_TAGS.find(tag => tag.id === 'tag-study');
        expect(studyTag !== undefined).assertTrue();
        if (studyTag !== undefined) {
          expect(studyTag.name).assertEqual('学习');
          expect(studyTag.color).assertEqual('#45B7D1');
        }
      });

      it('所有默认标签都有唯一 ID', 0, () => {
        const ids = DEFAULT_TAGS.map(tag => tag.id);
        // 使用传统方式检查唯一性，避免使用 Set spread
        let isUnique = true;
        for (let i = 0; i < ids.length; i++) {
          for (let j = i + 1; j < ids.length; j++) {
            if (ids[i] === ids[j]) {
              isUnique = false;
              break;
            }
          }
        }
        expect(isUnique).assertTrue();
      });

      it('所有默认标签都有颜色', 0, () => {
        for (const tag of DEFAULT_TAGS) {
          expect(tag.color.length).assertLarger(0);
          expect(tag.color.startsWith('#')).assertTrue();
        }
      });
    });
  });
}
