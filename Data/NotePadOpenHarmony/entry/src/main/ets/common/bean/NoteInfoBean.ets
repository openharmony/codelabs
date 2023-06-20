/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License,Version 2.0 (the "License");
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
  constructor(noteObject : NotesInfoBean) {
    this.noteId = noteObject.noteId;
    this.title = noteObject.title;
    this.folder = noteObject.folder;
    this.time = noteObject.time;
    this.isFavorite = noteObject.isFavorite;
    this.imageArr = noteObject.imageArr;
    this.content = noteObject.content;
  }
}