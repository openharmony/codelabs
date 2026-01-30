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
 *
 */

/**
 * Recording 模型单元测试
 */
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from '@ohos/hypium';
import { Recording, RecordingInit } from '../../main/ets/model/Recording';
import { relationalStore } from '@kit.ArkData';

export default function recordingModelTest() {
  describe('RecordingModelTest', () => {
    
    describe('构造函数测试', () => {
      it('创建默认 Recording 实例', 0, () => {
        const recording = new Recording();
        
        expect(recording.id).assertContain('recording-');
        expect(recording.scheduleId).assertEqual('');
        expect(recording.filePath).assertEqual('');
        expect(recording.fileName).assertEqual('');
        expect(recording.duration).assertEqual(0);
        expect(recording.fileSize).assertEqual(0);
        expect(recording.format).assertEqual('aac');
        expect(recording.transcription).assertEqual('');
        expect(recording.createdAt).assertLarger(0);
      });

      it('使用 RecordingInit 创建实例', 0, () => {
        const init: RecordingInit = {
          id: 'recording-test-001',
          scheduleId: 'schedule-001',
          filePath: '/data/recordings/test.aac',
          fileName: 'test.aac',
          duration: 120,
          fileSize: 256000,
          format: 'mp3',
          transcription: '这是测试语音转文本内容'
        };
        
        const recording = new Recording(init);
        
        expect(recording.id).assertEqual('recording-test-001');
        expect(recording.scheduleId).assertEqual('schedule-001');
        expect(recording.filePath).assertEqual('/data/recordings/test.aac');
        expect(recording.fileName).assertEqual('test.aac');
        expect(recording.duration).assertEqual(120);
        expect(recording.fileSize).assertEqual(256000);
        expect(recording.format).assertEqual('mp3');
        expect(recording.transcription).assertEqual('这是测试语音转文本内容');
      });

      it('部分初始化参数创建实例', 0, () => {
        const init: RecordingInit = {
          scheduleId: 'schedule-partial',
          fileName: 'memo.aac'
        };
        
        const recording = new Recording(init);
        
        expect(recording.id).assertContain('recording-');
        expect(recording.scheduleId).assertEqual('schedule-partial');
        expect(recording.fileName).assertEqual('memo.aac');
        expect(recording.format).assertEqual('aac');  // 默认值
      });
    });

    describe('getDurationText 方法测试', () => {
      it('0秒时长显示', 0, () => {
        const recording = new Recording({ duration: 0 });
        expect(recording.getDurationText()).assertEqual('00:00');
      });

      it('30秒时长显示', 0, () => {
        const recording = new Recording({ duration: 30 });
        expect(recording.getDurationText()).assertEqual('00:30');
      });

      it('1分钟时长显示', 0, () => {
        const recording = new Recording({ duration: 60 });
        expect(recording.getDurationText()).assertEqual('01:00');
      });

      it('1分30秒时长显示', 0, () => {
        const recording = new Recording({ duration: 90 });
        expect(recording.getDurationText()).assertEqual('01:30');
      });

      it('5分钟时长显示', 0, () => {
        const recording = new Recording({ duration: 300 });
        expect(recording.getDurationText()).assertEqual('05:00');
      });

      it('10分15秒时长显示', 0, () => {
        const recording = new Recording({ duration: 615 });
        expect(recording.getDurationText()).assertEqual('10:15');
      });

      it('59分59秒时长显示', 0, () => {
        const recording = new Recording({ duration: 3599 });
        expect(recording.getDurationText()).assertEqual('59:59');
      });

      it('65分钟时长显示', 0, () => {
        const recording = new Recording({ duration: 3900 });
        expect(recording.getDurationText()).assertEqual('65:00');
      });
    });

    describe('getFileSizeText 方法测试', () => {
      it('小于1KB的文件大小', 0, () => {
        const recording = new Recording({ fileSize: 512 });
        expect(recording.getFileSizeText()).assertEqual('512 B');
      });

      it('1KB文件大小', 0, () => {
        const recording = new Recording({ fileSize: 1024 });
        expect(recording.getFileSizeText()).assertEqual('1.0 KB');
      });

      it('100KB文件大小', 0, () => {
        const recording = new Recording({ fileSize: 102400 });
        expect(recording.getFileSizeText()).assertEqual('100.0 KB');
      });

      it('500KB文件大小', 0, () => {
        const recording = new Recording({ fileSize: 512000 });
        expect(recording.getFileSizeText()).assertEqual('500.0 KB');
      });

      it('1MB文件大小', 0, () => {
        const recording = new Recording({ fileSize: 1048576 });
        expect(recording.getFileSizeText()).assertEqual('1.0 MB');
      });

      it('5.5MB文件大小', 0, () => {
        const recording = new Recording({ fileSize: 5767168 });
        expect(recording.getFileSizeText()).assertEqual('5.5 MB');
      });

      it('10MB文件大小', 0, () => {
        const recording = new Recording({ fileSize: 10485760 });
        expect(recording.getFileSizeText()).assertEqual('10.0 MB');
      });

      it('0字节文件大小', 0, () => {
        const recording = new Recording({ fileSize: 0 });
        expect(recording.getFileSizeText()).assertEqual('0 B');
      });
    });

    describe('toDbObject 方法测试', () => {
      it('转换为数据库对象', 0, () => {
        const recording = new Recording({
          id: 'db-recording-001',
          scheduleId: 'schedule-db',
          filePath: '/storage/audio/memo.aac',
          fileName: 'memo.aac',
          duration: 180,
          fileSize: 1536000,
          format: 'aac',
          transcription: '会议记录内容'
        });
        
        const dbObj = recording.toDbObject();
        
        expect(dbObj['id']).assertEqual('db-recording-001');
        expect(dbObj['scheduleId']).assertEqual('schedule-db');
        expect(dbObj['filePath']).assertEqual('/storage/audio/memo.aac');
        expect(dbObj['fileName']).assertEqual('memo.aac');
        expect(dbObj['duration']).assertEqual(180);
        expect(dbObj['fileSize']).assertEqual(1536000);
        expect(dbObj['format']).assertEqual('aac');
        expect(dbObj['transcription']).assertEqual('会议记录内容');
        expect(dbObj['createdAt']).assertLarger(0);
      });
    });

    describe('fromDbObject 静态方法测试', () => {
      it('从数据库对象创建实例', 0, () => {
        const now = Date.now();
        const dbObj: relationalStore.ValuesBucket = {
          id: 'from-db-recording',
          scheduleId: 'schedule-from-db',
          filePath: '/cache/recordings/voice.mp3',
          fileName: 'voice.mp3',
          duration: 245,
          fileSize: 2048000,
          format: 'mp3',
          transcription: '从数据库恢复的语音内容',
          createdAt: now
        };
        
        const recording = Recording.fromDbObject(dbObj);
        
        expect(recording.id).assertEqual('from-db-recording');
        expect(recording.scheduleId).assertEqual('schedule-from-db');
        expect(recording.filePath).assertEqual('/cache/recordings/voice.mp3');
        expect(recording.fileName).assertEqual('voice.mp3');
        expect(recording.duration).assertEqual(245);
        expect(recording.fileSize).assertEqual(2048000);
        expect(recording.format).assertEqual('mp3');
        expect(recording.transcription).assertEqual('从数据库恢复的语音内容');
        expect(recording.createdAt).assertEqual(now);
      });
    });

    describe('边界条件测试', () => {
      it('非常大的文件大小', 0, () => {
        const recording = new Recording({ fileSize: 1073741824 });  // 1GB
        expect(recording.getFileSizeText()).assertEqual('1024.0 MB');
      });

      it('非常长的录音时长', 0, () => {
        const recording = new Recording({ duration: 7200 });  // 2小时
        expect(recording.getDurationText()).assertEqual('120:00');
      });
    });
  });
}
