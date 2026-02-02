/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 *
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
import router from '@ohos.router'
import { Routes } from '../constants/Routes'
import { Contact } from '../model/Contact'
import { ContactService } from '../service/ContactService'
import { MetadataService } from '../service/MetadataService'
import { ImportanceLevel } from '../model/ContactMetadata'
import { RecycleBinService } from '../service/RecycleBinService'

// 在应用入口或公共文件定义全局刷新标志
PersistentStorage.PersistProp('contactListRefreshFlag', 0)

// 联系人分组接口
interface ContactGroup {
  letter: string // 首字母
  contacts: Contact[] // 该字母下的联系人列表
}

@Entry
struct ContactListPage {
  @State contactList: Contact[] = []
  @State filteredContactList: Contact[] = []
  @State contactGroups: ContactGroup[] = []
  @State alphabetList: string[] = []
  @State searchText: string = ''
  @State isLoading: boolean = true
  @StorageLink('contactListRefreshFlag') refreshFlag: number = 0
  private lastRefreshFlag: number = 0
  private scroller: Scroller = new Scroller()

  // 重要联系人相关状态
  @State emergencyContacts: Contact[] = []  // 紧急联系人
  @State importantContacts: Contact[] = []  // 重要联系人
  @State showEmergencySection: boolean = true  // 是否显示紧急联系人区域
  @State showImportantSection: boolean = true  // 是否显示重要联系人区域
  private markedContactsMap: Map<string, ImportanceLevel> = new Map()

  // Tab切换状态
  @State currentTab: 'all' | 'recent' = 'all'

  async aboutToAppear(): Promise<void> {
    this.lastRefreshFlag = this.refreshFlag
    
    // 初始化服务
    try {
      await MetadataService.init()
      await RecycleBinService.init()
      console.info('[ContactListPage] 服务初始化成功')
      
      // 自动清理过期的回收站记录（后台执行，不阻塞UI）
      RecycleBinService.cleanExpiredContacts().then((count: number) => {
        if (count > 0) {
          console.info(`[ContactListPage] 自动清理了 ${count} 条过期记录`)
        }
      }).catch((err: Error) => {
        console.error('[ContactListPage] 自动清理失败:', err)
      })
    } catch (err) {
      console.error('[ContactListPage] 服务初始化失败:', err)
    }
    
    this.loadData()
  }

  // 监听刷新标志变化
  aboutToUpdate(): void {
    if (this.refreshFlag !== this.lastRefreshFlag) {
      console.info('[ContactListPage] 检测到刷新标志变化,重新加载数据')
      this.lastRefreshFlag = this.refreshFlag
      this.loadData()
    }
  }

  onPageShow(): void {
    this.loadData()
  }

  async loadData(): Promise<void> {
    this.isLoading = true

    try {
      // 1. 加载所有联系人
      const result = await ContactService.getAllContacts()
      this.contactList = result

      // 2. 加载元数据（重要性标记）
      this.markedContactsMap = await MetadataService.getAllMarkedContacts()

      // 3. 分类联系人
      this.categorizeContacts()

      // 4. 过滤和分组
      this.filterContacts()

      // 5. 清理孤立的元数据
      const validKeys = result.map(c => c.key).filter(k => k)
      await MetadataService.cleanOrphanMetadata(validKeys)

      console.info('[ContactListPage] 加载联系人成功,数量:', result.length)
      console.info('[ContactListPage] 紧急联系人:', this.emergencyContacts.length)
      console.info('[ContactListPage] 重要联系人:', this.importantContacts.length)
    } catch (err) {
      console.error('加载联系人失败:', JSON.stringify(err))
    } finally {
      this.isLoading = false
    }
  }

  /**
   * 根据元数据分类联系人
   */
  categorizeContacts(): void {
    this.emergencyContacts = []
    this.importantContacts = []

    this.contactList.forEach((contact: Contact) => {
      const importance = this.markedContactsMap.get(contact.key)

      if (importance === ImportanceLevel.EMERGENCY) {
        this.emergencyContacts.push(contact)
      } else if (importance === ImportanceLevel.IMPORTANT) {
        this.importantContacts.push(contact)
      }
    })

    // 按名字排序
    this.emergencyContacts.sort((a, b) => (a.name || '').localeCompare(b.name || ''))
    this.importantContacts.sort((a, b) => (a.name || '').localeCompare(b.name || ''))
  }

  // 获取首字母：英文字母返回大写，其他字符统一返回 #
  getFirstLetter(name: string): string {
    if (!name || name.length === 0) {
      return '#'
    }

    const firstChar = name.charAt(0)

    // 判断是否为英文字母
    if (/^[A-Za-z]$/.test(firstChar)) {
      return firstChar.toUpperCase()
    }

    // 其他字符（包括中文）统一返回 #
    return '#'
  }

  // 对联系人进行分组
  groupContacts(contacts: Contact[]): ContactGroup[] {
    const groupMap = new Map<string, Contact[]>()

    // 按首字母分组
    contacts.forEach((contact: Contact) => {
      const letter = this.getFirstLetter(contact.name)
      if (!groupMap.has(letter)) {
        groupMap.set(letter, [])
      }
      const group = groupMap.get(letter)
      if (group !== undefined) {
        group.push(contact)
      }
    })

    // 转换为数组并排序
    const groups: ContactGroup[] = []

    // 将 # 放在最后
    const letters = Array.from(groupMap.keys()).sort((a: string, b: string): number => {
      if (a === '#') {
        return 1
      }
      if (b === '#') {
        return -1
      }
      return a.localeCompare(b)
    })

    letters.forEach((letter: string) => {
      const contactsInGroup = groupMap.get(letter)
      if (contactsInGroup !== undefined) {
        // 组内按名字字母序排序
        contactsInGroup.sort((a: Contact, b: Contact): number => {
          const nameA = a.name || ''
          const nameB = b.name || ''
          return nameA.localeCompare(nameB)
        })
        groups.push({
          letter: letter,
          contacts: contactsInGroup
        })
      }
    })

    return groups
  }

  // 过滤联系人的方法
  filterContacts(): void {
    if (!this.searchText || this.searchText.trim() === '') {
      this.filteredContactList = this.contactList
    } else {
      const keyword = this.searchText.toLowerCase().trim()
      this.filteredContactList = this.contactList.filter((contact: Contact): boolean => {
        const nameMatch = contact.name?.toLowerCase().includes(keyword) ?? false
        const phoneMatch = contact.phone?.includes(keyword) ?? false

        return nameMatch || phoneMatch
      })
    }

    // 更新分组数据
    this.contactGroups = this.groupContacts(this.filteredContactList)
    this.alphabetList = this.contactGroups.map((group: ContactGroup): string => group.letter)

    console.info('[ContactListPage] 过滤后联系人数量:', this.filteredContactList.length)
    console.info('[ContactListPage] 字母索引:', this.alphabetList)
  }

  // 处理搜索输入变化
  handleSearchChange(value: string): void {
    this.searchText = value
    this.filterContacts()
  }

  // 清空搜索
  clearSearch(): void {
    this.searchText = ''
    this.filterContacts()
  }

  // 处理字母索引点击
  handleAlphabetSelect(index: number): void {
    const letter = this.alphabetList[index]
    console.info('[ContactListPage] 选中字母:', letter)

    // 计算目标位置(需要累加前面所有组的项数)
    let targetIndex = 0
    for (let i = 0; i < index; i++) {
      targetIndex += this.contactGroups[i].contacts.length + 1 // +1是因为有分组标题
    }

    // 滚动到目标位置
    this.scroller.scrollToIndex(targetIndex)
  }

  build(): void {
    Navigation() {
      Stack({ alignContent: Alignment.End }) {
        Column({ space: 0 }) {
          // 搜索框区域
          Row() {
            Search({
              value: this.searchText,
              placeholder: '搜索姓名或手机号'
            })
              .searchButton('搜索')
              .width('100%')
              .height(40)
              .backgroundColor(Color.White)
              .placeholderColor(Color.Grey)
              .placeholderFont({ size: 14 })
              .textFont({ size: 14 })
              .onChange((value: string) => {
                this.handleSearchChange(value)
              })
              .onSubmit((value: string) => {
                console.info('[ContactListPage] 搜索提交:', value)
              })
          }
          .width('100%')
          .padding({ left: 16, right: 16, top: 8, bottom: 8 })
          .backgroundColor('#F1F3F5')

          // Tab切换栏
          Row() {
            this.TabButton('全部联系人', 'all', this.currentTab === 'all', () => {
              this.currentTab = 'all'
              this.loadData()
            })
            this.TabButton('最近', 'recent', this.currentTab === 'recent', () => {
              this.currentTab = 'recent'
              router.pushUrl({ url: Routes.RECENT_CONTACTS })
            })
          }
          .width('100%')
          .height(48)
          .padding({ left: 16, right: 16 })
          .backgroundColor(Color.White)
          .justifyContent(FlexAlign.SpaceAround)

          // 搜索结果提示
          if (this.searchText.trim() !== '' && !this.isLoading) {
            Row() {
              Text(`找到 ${this.filteredContactList.length} 个联系人`)
                .fontSize(12)
                .fontColor(Color.Gray)

              Blank()

              Text('清空')
                .fontSize(12)
                .fontColor('#007DFF')
                .onClick(() => this.clearSearch())
            }
            .width('100%')
            .padding({ left: 16, right: 16, top: 4, bottom: 4 })
            .backgroundColor('#F1F3F5')
          }

          // 列表内容区域
          if (this.isLoading) {
            Column() {
              LoadingProgress()
                .width(50)
                .height(50)
            }
            .width('100%')
            .layoutWeight(1)
            .justifyContent(FlexAlign.Center)
            .backgroundColor('#F1F3F5')
          } else if (this.filteredContactList.length === 0) {
            Column() {
              if (this.searchText.trim() !== '') {
                Text('未找到匹配的联系人')
                  .fontSize(16)
                  .fontColor(Color.Gray)
                  .margin({ bottom: 10 })
                Button('清空搜索')
                  .fontSize(14)
                  .onClick(() => this.clearSearch())
              } else {
                Text('暂无联系人')
                  .fontSize(16)
                  .fontColor(Color.Gray)
                  .margin({ bottom: 10 })
                Button('点击刷新')
                  .fontSize(14)
                  .onClick(() => this.loadData())
              }
            }
            .width('100%')
            .layoutWeight(1)
            .justifyContent(FlexAlign.Center)
            .backgroundColor('#F1F3F5')
          } else {
            // 按分组渲染列表
            List({ space: 0, scroller: this.scroller }) {
              // ========== 紧急联系人区域 ==========
              if (this.emergencyContacts.length > 0 && this.searchText.trim() === '') {
                ListItemGroup({
                  header: this.SpecialGroupHeader('紧急联系人', this.emergencyContacts.length, '#FF4444', this.showEmergencySection, () => {
                    this.showEmergencySection = !this.showEmergencySection
                  }),
                  space: 0
                }) {
                  if (this.showEmergencySection) {
                    ForEach(
                      this.emergencyContacts,
                      (item: Contact) => {
                        ListItem() {
                          this.ContactItemBuilder(item, ImportanceLevel.EMERGENCY)
                        }
                        .width('100%')
                        .onClick(() => {
                          router.pushUrl({
                            url: Routes.CONTACT_DETAIL,
                            params: { contactId: item.id }
                          })
                        })
                      },
                      (item: Contact): string => item.key || item.id || ''
                    )
                  }
                }
              }

              // ========== 重要联系人区域 ==========
              if (this.importantContacts.length > 0 && this.searchText.trim() === '') {
                ListItemGroup({
                  header: this.SpecialGroupHeader('重要联系人', this.importantContacts.length, '#FF9800', this.showImportantSection, () => {
                    this.showImportantSection = !this.showImportantSection
                  }),
                  space: 0
                }) {
                  if (this.showImportantSection) {
                    ForEach(
                      this.importantContacts,
                      (item: Contact) => {
                        ListItem() {
                          this.ContactItemBuilder(item, ImportanceLevel.IMPORTANT)
                        }
                        .width('100%')
                        .onClick(() => {
                          router.pushUrl({
                            url: Routes.CONTACT_DETAIL,
                            params: { contactId: item.id }
                          })
                        })
                      },
                      (item: Contact): string => item.key || item.id || ''
                    )
                  }
                }
              }

              // ========== 普通联系人区域（按字母分组） ==========
              ForEach(
                this.contactGroups,
                (group: ContactGroup) => {
                  // 分组标题
                  ListItemGroup({
                    header: this.GroupHeader(group.letter),
                    space: 0
                  }) {
                    ForEach(
                      group.contacts,
                      (item: Contact) => {
                        ListItem() {
                          Row() {
                            // 头像首字母
                            Text(item.name ? item.name.charAt(0) : '?')
                              .width(40)
                              .height(40)
                              .borderRadius(20)
                              .backgroundColor('#007DFF')
                              .fontColor(Color.White)
                              .textAlign(TextAlign.Center)
                              .fontWeight(FontWeight.Bold)
                              .fontSize(18)
                              .margin({ right: 12 })

                            Column() {
                              Text(item.name || '未知')
                                .fontSize(16)
                                .fontWeight(FontWeight.Medium)
                                .fontColor('#000000')
                              Text(item.phone || '')
                                .fontSize(14)
                                .fontColor('#999999')
                                .margin({ top: 2 })
                            }
                            .alignItems(HorizontalAlign.Start)
                            .layoutWeight(1)

                            Image($r('sys.media.ohos_ic_public_arrow_right'))
                              .width(16)
                              .height(16)
                              .fillColor('#CCCCCC')
                          }
                          .width('100%')
                          .height(64)
                          .padding({ left: 16, right: 16 })
                          .backgroundColor(Color.White)
                        }
                        .width('100%')
                        .onClick(() => {
                          console.info('[ContactListPage] 点击联系人:', {
                            id: item.id,
                            key: item.key,
                            name: item.name
                          })

                          router.pushUrl({
                            url: Routes.CONTACT_DETAIL,
                            params: {
                              contactId: item.id
                            }
                          })
                        })
                      },
                      (item: Contact): string => item.key || item.id || ''
                    )
                  }
                },
                (group: ContactGroup): string => group.letter
              )
            }
            .width('100%')
            .layoutWeight(1)
            .backgroundColor('#F1F3F5')
            .divider({
              strokeWidth: 1,
              color: '#F0F0F0',
              startMargin: 68,
              endMargin: 0
            })
            .edgeEffect(EdgeEffect.Spring)
            .sticky(StickyStyle.Header)
          }
        }
        .width('100%')
        .height('100%')

        // 右侧字母索引
        if (!this.isLoading && this.filteredContactList.length > 0 && this.searchText.trim() === '') {
          AlphabetIndexer({ arrayValue: this.alphabetList, selected: 0 })
            .color('#666666')
            .selectedColor('#007DFF')
            .popupColor('#007DFF')
            .selectedBackgroundColor('#E6F2FF')
            .popupBackground('#F0F0F0')
            .usingPopup(true)
            .selectedFont({ size: 14, weight: FontWeight.Bold })
            .popupFont({ size: 32, weight: FontWeight.Bold })
            .font({ size: 12, weight: FontWeight.Regular })
            .itemSize(20)
            .alignStyle(IndexerAlign.Right)
            .popupPosition({ x: 60, y: 0 })
            .onSelect((index: number) => {
              this.handleAlphabetSelect(index)
            })
            .margin({ right: 8 })
        }
      }
      .width('100%')
      .height('100%')
    }
    .title('联系人')
    .titleMode(NavigationTitleMode.Mini)
    .backgroundColor('#F1F3F5')
    .menus([
      {
        value: '新建',
        icon: $r('sys.media.ohos_ic_public_add'),
        action: () => {
          console.info('[ContactListPage] 点击新建联系人')
          router.pushUrl({ url: Routes.CONTACT_CREATE })
        }
      },
      {
        value: '回收站',
        action: () => {
          console.info('[ContactListPage] 点击回收站')
          router.pushUrl({ url: Routes.RECYCLE_BIN })
        }
      },
    ])
  }

  // 分组标题Builder
  @Builder
  GroupHeader(letter: string): void {
    Row() {
      Text(letter)
        .fontSize(14)
        .fontColor('#666666')
        .fontWeight(FontWeight.Medium)
    }
    .width('100%')
    .height(32)
    .padding({ left: 16 })
    .backgroundColor('#F1F3F5')
  }

  // 特殊分组标题Builder（重要/紧急联系人）
  @Builder
  SpecialGroupHeader(
    title: string,
    count: number,
    color: string,
    isExpanded: boolean,
    onToggle: () => void
  ): void {
    Row() {
      // 彩色标识条
      Row()
        .width(4)
        .height(16)
        .backgroundColor(color)
        .borderRadius(2)
        .margin({ right: 8 })

      Text(title)
        .fontSize(15)
        .fontColor('#333333')
        .fontWeight(FontWeight.Bold)

      Text(`(${count})`)
        .fontSize(13)
        .fontColor('#999999')
        .margin({ left: 4 })

      Blank()

      // 展开/收起图标
      Image(isExpanded ? $r('sys.media.ohos_ic_public_arrow_up') : $r('sys.media.ohos_ic_public_arrow_down'))
        .width(20)
        .height(20)
        .fillColor('#999999')
    }
    .width('100%')
    .height(44)
    .padding({ left: 16, right: 16 })
    .backgroundColor('#FFFFFF')
    .onClick(() => onToggle())
  }

  // 联系人项Builder（带重要性标识）
  @Builder
  ContactItemBuilder(contact: Contact, importance?: ImportanceLevel): void {
    Row() {
      // 头像
      Stack() {
        Text(contact.name ? contact.name.charAt(0) : '?')
          .width(40)
          .height(40)
          .borderRadius(20)
          .backgroundColor(
            importance === ImportanceLevel.EMERGENCY ? '#FF4444' :
              importance === ImportanceLevel.IMPORTANT ? '#FF9800' :
                '#007DFF'
          )
          .fontColor(Color.White)
          .textAlign(TextAlign.Center)
          .fontWeight(FontWeight.Bold)
          .fontSize(18)

        // 重要性角标
        if (importance === ImportanceLevel.EMERGENCY || importance === ImportanceLevel.IMPORTANT) {
          Text(importance === ImportanceLevel.EMERGENCY ? '急' : '重')
            .fontSize(10)
            .fontColor(Color.White)
            .fontWeight(FontWeight.Bold)
            .width(16)
            .height(16)
            .borderRadius(8)
            .backgroundColor(importance === ImportanceLevel.EMERGENCY ? '#D32F2F' : '#F57C00')
            .textAlign(TextAlign.Center)
            .position({ x: 26, y: -2 })
        }
      }
      .margin({ right: 12 })

      Column() {
        Text(contact.name || '未知')
          .fontSize(16)
          .fontWeight(FontWeight.Medium)
          .fontColor('#000000')
        Text(contact.phone || '')
          .fontSize(14)
          .fontColor('#999999')
          .margin({ top: 2 })
      }
      .alignItems(HorizontalAlign.Start)
      .layoutWeight(1)

      Image($r('sys.media.ohos_ic_public_arrow_right'))
        .width(16)
        .height(16)
        .fillColor('#CCCCCC')
    }
    .width('100%')
    .height(64)
    .padding({ left: 16, right: 16 })
    .backgroundColor(Color.White)
  }

  // Tab按钮Builder
  @Builder
  TabButton(label: string, value: string, isSelected: boolean, onClick: () => void): void {
    Text(label)
      .fontSize(15)
      .fontColor(isSelected ? '#007DFF' : '#666666')
      .fontWeight(isSelected ? FontWeight.Bold : FontWeight.Normal)
      .padding({ left: 16, right: 16, top: 12, bottom: 12 })
      .backgroundColor(isSelected ? '#E6F2FF' : Color.Transparent)
      .borderRadius(20)
      .onClick(onClick)
  }
}