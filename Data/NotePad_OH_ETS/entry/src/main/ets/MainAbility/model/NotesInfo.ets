/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

export enum FolderType {
  All = 'All notes', // 全部笔记
  Work = 'Work', // 工作
  Personal = 'Personal', // 个人
}

export class NotesInfo {
  noteId: string; // 备忘录ID
  title: string; // 标题
  folder: FolderType; // 文件夹类别
  time: string; // 生成时间
  isFavorite: boolean; // 是否收藏
  imageArr: string[]; // 笔记中的图片
  content: string; // 内容

  constructor(noteId:string, title: string, folder: FolderType, time: string, isFavorite: boolean,imageArr: string[], content: string) {
    this.noteId = noteId;
    this.title = title;
    this.folder = folder;
    this.time = time;
    this.isFavorite = isFavorite;
    this.imageArr = imageArr;
    this.content = content;
  }
}

export const notesInfoArr: any[] = [
  {
    "noteId": '1646121972443',
    "title": 'Morning exercise',
    "folder": FolderType.Personal,
    "content": 'Long run 3 km',
    "imageArr": ['test_image1.png'],
    "time": '7:30:58 AM',
    "isFavorite": true,
  },
  {
    "noteId": '1646121972444',
    "title": 'Breakfast',
    "folder": FolderType.Personal,
    "content": 'Milk and bread',
    "imageArr": ['test_image2.png'],
    "time": '8:00:12 AM',
    "isFavorite": false,
  },
  {
    "noteId": '1646121972445',
    "title": 'Go to work',
    "folder": FolderType.Work,
    "content": 'Take the 11th bus',
    "imageArr": ['test_image3.png'],
    "time": '8:30:36 AM',
    "isFavorite": false,
  },
  {
    "noteId": '1646121972446',
    "title": 'Lunches',
    "folder": FolderType.Personal,
    "content": 'Sweet and sour ribs' +
    ' potato shreds',
    "imageArr": ['test_image4.png'],
    "time": '12:20:45 AM',
    "isFavorite": true,
  },
  {
    "noteId": '1646121972447',
    "title": 'Computer Lessons',
    "folder": FolderType.Work,
    "content": 'Learning HarmonyOS',
    "imageArr": ['test_image5.png'],
    "time": '3:30:06 PM',
    "isFavorite": true,
  },
];

export const noteIdArray: any[] = [
  {
    "noteId": '1646121972443'
  },
  {
    "noteId": '1646121972444'
  },
  {
    "noteId": '1646121972445'
  },
  {
    "noteId": '1646121972446'
  },
  {
    "noteId": '1646121972447'
  },
]