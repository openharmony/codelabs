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

// @ts-nocheck
import {NotesInfo, FolderType} from '../model/NotesInfo'
import router from '@system.router';
import prompt from '@system.prompt';
import dataStorage from '@ohos.data.storage'
import featureAbility from '@ohos.ability.featureAbility'

@Entry
@Component
struct NotesDetail {
  @State logTag: string = "[NotesLog NotesDetail]"
  @State notesInfo: NotesInfo = router.getParams().notesInfo
  @State operationType: string = router.getParams().operationType
  @State imageSrc: string = ''
  @State imageArr: string[] = this.notesInfo.imageArr
  @State noteIdArray: any[] = []
  @State notesFolder: string = this.notesInfo.folder
  @State backImgSrc: string = 'ic_public_back.png'
  @State okImgSrc: string = 'ic_public_ok.png'
  @State isFavorite: string = this.notesInfo.isFavorite
  @State collectImgSrc: string = this.isFavorite ? 'ic_public_collected.png' : 'ic_public_collect.png'
  @State isDataChange: boolean = false
  @State shareName: string = '分享'
  @State collectName: string = '收藏'
  @State collectedName: string = '取消收藏'
  @State deleteName: string = '删除'
  @State pictureName: string = '图片'
  scroller: Scroller = new Scroller()

  build() {
    Flex({ justifyContent: FlexAlign.Start, direction: FlexDirection.Column }) {
      Flex({ justifyContent: FlexAlign.Start, direction: FlexDirection.Row, alignItems: ItemAlign.Start }) {
        Image($rawfile(this.backImgSrc))
          .width(45)
          .margin({ left: 10 })
          .objectFit(ImageFit.Contain)
          .onClick(() => {
            if (this.isDataChange) {
              this.saveDialogController.open()
            } else {
              router.back()
            }
          })

        Image($rawfile(this.okImgSrc))
          .width(45)
          .position({ x: '40%', y: 0 })
          .objectFit(ImageFit.Contain)
          .visibility(this.isDataChange ? Visibility.Visible : Visibility.None)
          .onClick(() => {
            this.saveDialogController.open()
          })
      }
      .height('7%')
      .padding(5)

      TextInput({ placeholder: this.notesInfo.title != '' ? this.notesInfo.title : 'Title' })
        .width('97%')
        .height('7%')
        .fontSize(30)
        .placeholderFont({ size: 30 })
        .margin({ left: 10, top: 16 })
        .onChange((value: string) => {
          this.notesInfo.title = value
          this.isDataChange = true
        })

      Row() {
        Text(this.notesInfo.time)
          .fontSize(25)
          .fontColor(0xCCCCCC)
          .margin({ left: 26, top: 3 })

        Row() {
          Image($rawfile('ic_public_spinner_small.png'))
            .width(40)
            .height(40)
            .objectFit(ImageFit.Contain)

          Text(this.notesFolder)
            .fontSize(25)
            .fontColor(0x000000)
        }
        .margin({ left: 10, top: 3 })
        .bindMenu([
          {
            value: 'Personal',
            action: () => {
              this.notesFolder = FolderType.Personal
              this.notesInfo.folder = FolderType.Personal
              this.isDataChange = true
              console.info(this.logTag + 'handle Menu1 select')
            }
          },
          {
            value: 'Work',
            action: () => {
              this.notesFolder = FolderType.Work
              this.notesInfo.folder = FolderType.Work
              this.isDataChange = true
              console.info(this.logTag + 'handle Menu2 select')
            }
          },
        ])
      }
      .height('5%')
      .width('100%')

      TextArea({ placeholder: this.notesInfo.content != '' ? this.notesInfo.content : '' })
        .onChange((value: string) => {
          this.notesInfo.content = value
          this.isDataChange = true
        })
        .width('97%')
        .height('25%')
        .fontSize(27)
        .placeholderFont({ size: 27 })
        .margin({ left: 10, top: 3 })

      Scroll(this.scroller) {
        Column() {
          ForEach(this.imageArr, (item) => {
            Image($rawfile(item))
              .width('75%')
              .aspectRatio(1)
              .margin({ top: 5 })
          }, item => item)
        }
        .width('100%')
        .alignItems(HorizontalAlign.Center)
      }
      .height('40%')

      Row() {
        Column() {
          Image($rawfile('ic_public_share.png'))
            .width(40)
            .height(40)
          Text(this.shareName).fontSize(20)
        }
        .width('25%')
        .onClick(() => {
          // 分享笔记
          prompt.showToast({
            message: 'Please implement your share function'
          });
        })

        Column() {
          Image($rawfile(this.collectImgSrc))
            .width(40)
            .height(40)

          Text(this.collectName)
            .fontSize(20)
        }
        .width('25%')
        .onClick(() => {
          // 收藏
          this.notesInfo.isFavorite = !this.isFavorite
          this.isFavorite = this.notesInfo.isFavorite
          this.collectImgSrc = this.isFavorite ? 'ic_public_collected.png' : 'ic_public_collect.png'
          this.isDataChange = true
        })

        Column() {
          Image($rawfile('ic_public_delete.png'))
            .width(40)
            .height(40)

          Text(this.deleteName)
            .fontSize(20)
        }
        .width('25%')
        .onClick(() => {
          if (this.operationType == "modify") {
            // 删除笔记
            this.deleteDialogController.open()
          } else {
            prompt.showToast({
              message: 'The addition operation cannot be deleted'
            });
          }
        })

        Column() {
          Image($rawfile('ic_public_picture.png'))
            .width(40)
            .height(40)

          Text(this.pictureName)
            .fontSize(20)
        }
        .width('25%')
        .onClick(() => {
          // 图片
          this.imageSrc = 'test_image' + Math.ceil(Math.random() * 5) + '.png'
          this.imageArr.push(this.imageSrc);
          this.notesInfo.imageArr = this.imageArr
          this.isDataChange = true
        })
      }
      .height('10%')
      .margin({ bottom: 20 })
    }
    .width('100%')
    .height('100%')
  }

  async saveNoteIdArray() {
    var context = featureAbility.getContext()
    var path = await context.getFilesDir()
    let storage = dataStorage.getStorageSync(path + '/mystore')
    // 获取备忘录id数组
    storage.get("noteIdArr", " ")
      .then((value) => {
        var data = value.toString();
        // 置空新id数组
        while (this.noteIdArray.length >= 1) {
          this.noteIdArray.pop()
          console.info(this.logTag + "--- this.noteIdArray.pop() ---")
        }
        JSON.parse(data).forEach(item => {
          // 更新操作去掉原来备忘录id
          if (item.noteId != this.notesInfo.noteId) {
            this.noteIdArray.push(item)
          }
        })
        // 新加一条备忘录
        this.noteIdArray.push({ "noteId": this.notesInfo.noteId })
        console.info(this.logTag + "get new noteIdArr" + JSON.stringify(this.noteIdArray))
        // 删除原来的原来备忘录id数组
        let promise = storage.delete("noteIdArr")
        storage.flush()
        promise.then(() => {
          // 新增新的备忘录id数组
          let promise1 = storage.put("noteIdArr", JSON.stringify(this.noteIdArray))
          storage.flush()
          promise1.then(() => {
            console.info(this.logTag + "save new noteIdArr successfully." + JSON.stringify(this.noteIdArray))
            let promise2 = storage.delete(this.notesInfo.noteId)
            storage.flush()
            promise2.then(() => {
              // 新增新备忘录id对应的数据
              let promise3 = storage.put(this.notesInfo.noteId, JSON.stringify(new NotesInfo(this.notesInfo.noteId,
                this.notesInfo.title, this.notesInfo.folder, this.notesInfo.time, this.notesInfo.isFavorite,
                this.notesInfo.imageArr, this.notesInfo.content)))
              storage.flush()
              promise3.then(() => {
                router.push({
                  uri: 'pages/NotesList',
                })
                console.info(this.logTag + "save new value successfully." + this.notesInfo.isFavorite)
              }).catch((err) => {
                console.info(this.logTag + "saveNotesVal failed with err: " + err)
              })
            }).catch((err) => {
              console.info(this.logTag + "deleteNotesVal failed with err: " + err)
            })
          }).catch((err) => {
            console.info(this.logTag + "saveNotesIdArr value failed with err: " + err)
          })
        }).catch((err) => {
          console.info(this.logTag + "deleteNoteIdArr failed with err: " + err)
        })
      }).catch((err) => {
      console.info(this.logTag + "get noteIdArr failed with err: " + err)
    })
  }

  async deleteNoteIdArray() {
    // 删除笔记ID
    console.info(this.logTag + "--- deleteNoteIdArray() ---")
    var context = featureAbility.getContext()
    var path = await context.getFilesDir()
    let storage = dataStorage.getStorageSync(path + '/mystore')
    // 获取备忘录id数组
    storage.get("noteIdArr", " ")
      .then((value) => {
        var data = value.toString();
        while (this.noteIdArray.length >= 1) {
          this.noteIdArray.pop()
          console.info(this.logTag + "--- this.noteIdArray.pop() ---")
        }
        JSON.parse(data).forEach(item => {
          if ((item.noteId) != this.notesInfo.noteId) {
            this.noteIdArray.push(item)
          }
        })
        console.info(this.logTag + "get new noteIdArr" + JSON.stringify(this.noteIdArray))

        let promise = storage.delete("noteIdArr")
        storage.flush()
        promise.then(() => {
          console.info(this.logTag + "delete noteIdArr successfully.")
          let promise1 = storage.put("noteIdArr", JSON.stringify(this.noteIdArray))
          promise1.then(() => {
            console.info(this.logTag + "save new noteIdArr successfully." + JSON.stringify(this.noteIdArray))
            let promise2 = storage.delete(this.notesInfo.noteId)
            storage.flush()
            promise2.then(() => {
              router.push({
                uri: 'pages/NotesList',
              })
              console.info(this.logTag + "delete value successfully." + this.notesInfo.noteId)
            }).catch((err) => {
              console.info(this.logTag + "deleteNotesVal failed with err: " + err)
            })
          }).catch((err) => {
            console.info(this.logTag + "saveNotesIdArr value failed with err: " + err)
          })
        }).catch((err) => {
          console.info(this.logTag + "deleteNoteIdArr failed with err: " + err)
        })
      }).catch((err) => {
      console.info(this.logTag + "Get the value of noteIdArr failed with err: " + err)
    })
  }

  saveDialogController: CustomDialogController = new CustomDialogController({
    builder: CustomDialogSave({
      cancel: this.onCancel,
      confirm: this.onAcceptSave.bind(this)
    }),
    autoCancel: false,
    alignment: DialogAlignment.Center,
  })

  onCancel() {
    router.back()
    console.info(this.logTag + 'Callback when the first button is clicked')
  }

  onAcceptSave() {
    this.saveNoteIdArray()
    console.info('Callback when the second button is clicked')
  }

  onAcceptDelete() {
    this.deleteNoteIdArray()
    console.info('Callback when the second button is clicked')
  }

  deleteDialogController: CustomDialogController = new CustomDialogController({
    builder: CustomDialogDelete({
      cancel: this.onCancel,
      confirm: this.onAcceptDelete.bind(this)
    }),
    autoCancel: false,
    alignment: DialogAlignment.Center,
  })
}

@CustomDialog
struct CustomDialogSave {
  @State dialogText: string = '是否保存修改?'
  controller: CustomDialogController
  cancel: () => void
  confirm: () => void

  build() {
    Column() {
      Text(this.dialogText)
        .fontSize(30)
        .height(100)

      Flex({ justifyContent: FlexAlign.Center }) {
        Button('不保存')
          .onClick(() => {
            this.controller.close()
            this.cancel()
          })
          .backgroundColor(0xffffff)
          .fontColor(Color.Blue)
          .fontSize(30)

        Button('保存')
          .onClick(() => {
            this.controller.close()
            this.confirm()
          })
          .backgroundColor(0xffffff)
          .fontColor(Color.Blue)
          .fontSize(30)
      }
      .height(100)
    }
    .width('60%')
  }
}

@CustomDialog
struct CustomDialogDelete {
  @State dialogText: string = '是否删除?'
  controller: CustomDialogController
  cancel: () => void
  confirm: () => void

  build() {
    Column() {
      Text(this.dialogText)
        .fontSize(30)
        .height(100)

      Flex({ justifyContent: FlexAlign.Center }) {
        Button('取消')
          .onClick(() => {
            this.controller.close()
            this.cancel()
          })
          .backgroundColor(0xffffff)
          .fontColor(Color.Blue)
          .fontSize(30)

        Button('删除')
          .onClick(() => {
            this.controller.close()
            this.confirm()
          })
          .backgroundColor(0xffffff)
          .fontColor(Color.Red)
          .fontSize(30)
      }
      .height(100)
    }
    .width('60%')
  }
}