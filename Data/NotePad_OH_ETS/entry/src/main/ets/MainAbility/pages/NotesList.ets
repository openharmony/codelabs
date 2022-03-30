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

import {NotesInfo, FolderType} from '../model/NotesInfo'
import {getSelectNotes} from '../utils/NotesDataUtils'
import dataStorage from '@ohos.data.storage'
import featureAbility from '@ohos.ability.featureAbility'

@Entry
@Component
struct NotesList {
  @State logTag: string = "[NotesLog NotesList]"
  @State allNotes: Array<NotesInfo> = []
  @State selectNotes: Array<NotesInfo> = this.allNotes.sort()
  @State folderType: string = FolderType.All

  build() {
    Column() {
      Flex({ justifyContent: FlexAlign.Start, direction: FlexDirection.Row, alignItems: ItemAlign.Center }) {
        // 笔记类型选择
        Row() {
          Text(this.folderType)
            .fontSize(30)
            .fontWeight(700)
            .padding({ left: 20 })
            .textAlign(TextAlign.Start)

          Image($rawfile('ic_public_spinner.png'))
            .objectFit(ImageFit.Contain)
            .height(30)
            .width(30)
            .margin({ left: 5 })
        }
        .width('75%')
        .bindMenu([
          {
            value: this.folderType != FolderType.All ? FolderType.All : '',
            action: () => {
              console.info(this.logTag + "----------- bindMenu FolderType.All -----------")
              this.folderType = FolderType.All
              this.selectNotes = this.allNotes
            }
          },
          {
            value: this.folderType != FolderType.Personal ? FolderType.Personal : '',
            action: () => {
              console.info(this.logTag + "----------- bindMenu FolderType.Personal -----------")
              this.folderType = FolderType.Personal
              this.selectNotes = getSelectNotes(FolderType.Personal, this.allNotes)
            }
          },
          {
            value: this.folderType != FolderType.Work ? FolderType.Work : '',
            action: () => {
              console.info(this.logTag + "----------- bindMenu FolderType.Work -----------")
              this.folderType = FolderType.Work
              this.selectNotes = getSelectNotes(FolderType.Work, this.allNotes)
            }
          },
        ])

        // 添加笔记
        Navigator({ target: 'pages/NotesDetail' }) {
          Row() {
            Image($rawfile('ic_public_list_add_light.png'))
              .objectFit(ImageFit.Contain)
              .height(50)
              .width('25%')
          }
        }
        .params({ notesInfo: {
          "noteId": new Date().getTime().toString(),
          "title": '',
          "folder": FolderType.Personal,
          "content": '',
          "imageArr": [],
          "time": new Date().toLocaleTimeString(),
          "isFavorite": false
        }, operationType: 'add' })
      }
      .height(90)

      // 笔记列表
      List() {
        ForEach(this.selectNotes, item => {
          ListItem() {
            NotesInfoItem({ notesInfo: item })
          }
        }, item => item.noteId.toString())

        ListItem().height(200) // 列表底边距
      }
    }
    .height('100%')
    .backgroundColor('#FFf1f3f5')
  }

  onPageShow() {
    console.info(this.logTag + "----------- onPageShow -----------")
    this.getAllNotes()
  }

  async getAllNotes() {
    console.info(this.logTag + "----------- getAllNotes -----------")

    var context = featureAbility.getContext()
    var path = await context.getFilesDir()
    let storage = dataStorage.getStorageSync(path + '/mystore')
    storage.get("noteIdArr", " ")
      .then((value) => {
        var noteIdArr = value.toString();
        console.info(this.logTag + "The value of noteIdArr is " + noteIdArr)

        console.info(this.logTag + "--- allNotes length is " + this.allNotes.length)
        while (this.allNotes.length >= 1) {
          this.allNotes.pop()
          console.info(this.logTag + "--- this.allNotes.pop() ---")
        }
        console.info(this.logTag + "--- allNotes length is " + this.allNotes.length)

        JSON.parse(noteIdArr).forEach(item => {
          storage.get(item.noteId, " ")
            .then((val) => {
              var notesInfo = val.toString();
              console.info(this.logTag + "The value of notesInfo is " + notesInfo)

              var notesInfoObj = JSON.parse(notesInfo)

              this.allNotes.push(new NotesInfo(notesInfoObj.noteId, notesInfoObj.title, notesInfoObj.folder,
                notesInfoObj.time, notesInfoObj.isFavorite, notesInfoObj.imageArr, notesInfoObj.content))


              console.info(this.logTag + "----------- this.allNotes.push -----------" + this.allNotes.toString())
            }).catch((err) => {
            console.info(this.logTag + "Get the value of noteId failed with err: " + err)
          })
        })
      }).catch((err) => {
      console.info(this.logTag + "Get the value of noteIdArr failed with err: " + err)
      this.getAllNotes()
    })
  }
}


@Component
struct NotesInfoItem {
  private notesInfo: NotesInfo

  build() {
    Navigator({ target: 'pages/NotesDetail' }) {
      Flex({ justifyContent: FlexAlign.Start, direction: FlexDirection.Row, alignItems: ItemAlign.Center }) {
        Flex({ justifyContent: FlexAlign.Center, direction: FlexDirection.Column, alignItems: ItemAlign.Start }) {
          Text(this.notesInfo.title != '' ? this.notesInfo.title : this.notesInfo.content)
            .fontSize(24)
            .fontWeight(500)
            .padding(10)
            .textOverflow({ overflow: TextOverflow.Clip })
            .maxLines(1)

          Row() {
            Image($rawfile('ic_favorite_tag.png'))
              .objectFit(ImageFit.Contain)
              .height(23)
              .width(23)
              .margin({ right: 5 })
              .visibility(Visibility[this.notesInfo.isFavorite? 'Visible' : 'None'])

            Text(this.notesInfo.time + " | " + this.notesInfo.content)
              .textAlign(TextAlign.Start)
              .fontSize(20)
              .fontColor('#413f3f')
              .textOverflow({ overflow: TextOverflow.Ellipsis })
              .maxLines(1)
          }
          .padding(10)
        }
        .layoutWeight(1)

        Image($rawfile(this.notesInfo.imageArr[0]))
          .objectFit(ImageFit.Cover)
          .height(120)
          .width(120)
          .padding(10)
          .borderRadius(15)
          .visibility((this.notesInfo.imageArr.length != 0)? Visibility.Visible : Visibility.None)
      }
      .height(120)
      .borderRadius(25)
      .margin({ right: 10, left: 10, bottom: 10 })
      .backgroundColor(this.notesInfo.folder === FolderType.Personal ? '#ffffff' : '#dff5fa')
    }
    .params({ notesInfo: this.notesInfo, operationType: 'modify' })
  }
}