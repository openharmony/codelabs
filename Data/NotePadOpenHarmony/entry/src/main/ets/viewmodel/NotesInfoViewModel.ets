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

import { FolderType, NotesInfoBean } from '../common/bean/NoteInfoBean';
import { NoteIdBean } from '../common/bean/NoteIdBean';

export const notesItemArr: NotesInfoBean[] = [
  {
    'noteId': '1646121972443',
    'title': '早晨锻炼身体',
    'folder': FolderType.Personal,
    'content': 'Long run 3 km',
    'imageArr': ['cook_picture.png'],
    'time': '7:30:58',
    'isFavorite': true
  },
  {
    'noteId': '1646121972444',
    'title': 'Breakfast',
    'folder': FolderType.Personal,
    'content': 'Milk and bread',
    'imageArr': ['dinner_food.png'],
    'time': '8:00:12',
    'isFavorite': false
  },
  {
    'noteId': '1646121972445',
    'title': 'Go to work',
    'folder': FolderType.Work,
    'content': 'Take the 11th bus',
    'imageArr': ['drink_coffee.png'],
    'time': '8:30:36',
    'isFavorite': false
  },
  {
    'noteId': '1646121972446',
    'title': 'Lunches',
    'folder': FolderType.Personal,
    'content': 'Sweet and sour ribs potato shreds',
    'imageArr': ['cook_picture.png'],
    'time': '12:20:45',
    'isFavorite': true
  },
  {
    'noteId': '1646121972447',
    'title': 'Computer Lessons',
    'folder': FolderType.Work,
    'content': 'Learning HarmonyOS',
    'imageArr': ['dinner_food.png'],
    'time': '3:30:06',
    'isFavorite': true
  }
];

export const noteIdArray: NoteIdBean[] = [
  {
    'noteId': '1646121972443'
  },
  {
    'noteId': '1646121972444'
  },
  {
    'noteId': '1646121972445'
  },
  {
    'noteId': '1646121972446'
  },
  {
    'noteId': '1646121972447'
  }
]