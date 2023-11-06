/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

export class NoteIdBean {
  noteId: string;

  constructor(noteId: string) {
    this.noteId = noteId;
  }
}

/**
 * Folder type enum.
 */
export enum FolderType {
  All = 'all',
  Work = 'work',
  Personal = 'personal'
}

/**
 * NotesInfo item information.
 */
export class NotesInfoBean {
  /**
   * Note Id.
   */
  noteId: string;

  /**
   * Note title.
   */
  title: string;

  /**
   * Folder type.
   */
  folder: FolderType;

  /**
   * Create time.
   */
  time: string;

  /**
   * Collect note.
   */
  isFavorite: boolean;

  /**
   * Note images.
   */
  imageArr: string[];

  /**
   * Note content.
   */
  content: string;

  /**
   * constructor.
   */
  constructor(noteId: string, title: string, folder: FolderType, time: string, isFavorite: boolean, imageArr: string[],
              content: string) {
    this.noteId = noteId;
    this.title = title;
    this.folder = folder;
    this.time = time;
    this.isFavorite = isFavorite;
    this.imageArr = imageArr;
    this.content = content;
  }
}

export const notesItemArr: NotesInfoBean[] = [
  new NotesInfoBean('1646121972443', '早晨锻炼身体', FolderType.Personal, '7:30:58', true, ['cook_picture.png'],
    'Long run 3 km'),
  new NotesInfoBean('1646121972444', 'Breakfast', FolderType.Personal, '8:00:12', false, ['dinner_food.png'],
    'Milk and bread'),
  new NotesInfoBean('1646121972445', 'Go to work', FolderType.Work, '8:30:36', false, ['drink_coffee.png'],
    'Take the 11th bus'),
  new NotesInfoBean('1646121972446', 'Lunches', FolderType.Personal, '12:20:45', true, ['cook_picture.png'],
    'Sweet and sour ribs potato shreds'),
  new NotesInfoBean('1646121972447', 'Computer Lessons', FolderType.Work, '3:30:06', true, ['dinner_food.png'],
    'Learning HarmonyOS')
];

export const noteIdArray: NoteIdBean[] = [new NoteIdBean('1646121972443'), new NoteIdBean('1646121972444'),
  new NoteIdBean('1646121972445'), new NoteIdBean('1646121972446'), new NoteIdBean('1646121972447')];