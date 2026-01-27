/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
import dataPreferences from '@ohos.data.preferences';
import common from '@ohos.app.ability.common';

// 便签接口
export interface Note {
  id: string;
  title: string;      // 标题
  content: string;    // 内容
  isPinned: boolean;  // 是否置顶
  updateTime: string; // 更新时间
}

export class NoteManager {
  private static readonly PREF_NAME = 'MyNotesPref';
  private static readonly KEY_NOTES = 'notes_data';
  private static context: common.UIAbilityContext;

  // 初始化上下文
  static init(context: common.UIAbilityContext) {
    NoteManager.context = context;
  }

  // 获取所有便签（带排序逻辑）
  static async getNotes(): Promise<Note[]> {
    try {
      const pref = await dataPreferences.getPreferences(NoteManager.context, NoteManager.PREF_NAME);
      const dataStr = await pref.get(NoteManager.KEY_NOTES, '[]') as string;
      let notes = JSON.parse(dataStr) as Note[];

      // 排序逻辑：
      // 1. isPinned (true) 排在前面
      // 2. 如果置顶状态相同，则 id (时间戳) 大的排在前面（即最新的在前面）
      notes.sort((a, b) => {
        if (a.isPinned && !b.isPinned) return -1; // a置顶，b不置顶 -> a在前
        if (!a.isPinned && b.isPinned) return 1;  // a不置顶，b置顶 -> b在前
        // 如果都置顶或都不置顶，按ID(时间)倒序
        return b.id.localeCompare(a.id);
      });

      return notes;
    } catch (err) {
      console.error('获取便签失败:', err);
      return [];
    }
  }

  // 搜索便签 (新增功能)
  static async searchNotes(keyword: string): Promise<Note[]> {
    const allNotes = await NoteManager.getNotes();
    if (!keyword || keyword.trim() === '') {
      return allNotes;
    }
    // 过滤标题或内容包含关键字的项
    return allNotes.filter(note =>
    note.title.includes(keyword) || note.content.includes(keyword)
    );
  }

  // 保存便签
  static async saveNote(note: Note): Promise<boolean> {
    try {
      // 获取当前列表（不带排序的原始数据，为了查找方便，其实用getNotes也可以）
      let notes = await NoteManager.getNotes();
      const index = notes.findIndex(n => n.id === note.id);

      if (index !== -1) {
        notes[index] = note; // 更新
      } else {
        notes.push(note); // 新增（顺序无所谓，因为getNotes读取时会重排）
      }

      const pref = await dataPreferences.getPreferences(NoteManager.context, NoteManager.PREF_NAME);
      await pref.put(NoteManager.KEY_NOTES, JSON.stringify(notes));
      await pref.flush();
      return true;
    } catch (err) {
      console.error('保存失败:', err);
      return false;
    }
  }

  // 删除便签
  static async deleteNote(id: string): Promise<boolean> {
    try {
      let notes = await NoteManager.getNotes();
      notes = notes.filter(n => n.id !== id);

      const pref = await dataPreferences.getPreferences(NoteManager.context, NoteManager.PREF_NAME);
      await pref.put(NoteManager.KEY_NOTES, JSON.stringify(notes));
      await pref.flush();
      return true;
    } catch (err) {
      return false;
    }
  }
}