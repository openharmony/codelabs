/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSEt-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

        .fontWeight(FontWeight.Medium)
        .margin({ bottom: 16 })

      Grid() {
        GridItem() {
          this.buildStatItem('鎬绘暟', this.contacts.length.toString(), '#007DFF')
        }
        GridItem() {
          this.buildStatItem(
            '鏀惰棌',
            this.contacts.filter(c => c.isFavorite).length.toString(),
            '#FF9500'
          )
        }
        GridItem() {
          const departments = new Set<string>();
          this.contacts.forEach(c => {
            if (c.department) departments.add(c.department);
          });
          this.buildStatItem('鍒嗙粍', Math.max(departments.size, 1).toString(), '#34C759')
        }
        GridItem() {
          const companies = new Set<string>();
          this.contacts.forEach(c => {
            if (c.company) companies.add(c.company);
          });
          this.buildStatItem('鍏徃', Math.max(companies.size, 1).toString(), '#AF52DE')
        }
      }
      .columnsTemplate('1fr 1fr')
      .rowsTemplate('1fr 1fr')
      .columnsGap(12)
      .rowsGap(12)
    }
    .width('100%')
    .padding(16)
    .backgroundColor(Color.White)
    .borderRadius(12)
  }

  @Builder
  buildStatItem(label: string, value: string, color: string) {
    Column() {
      Text(value)
        .fontSize(20)
        .fontColor('#333333')
        .fontWeight(FontWeight.Bold)

      Text(label)
        .fontSize(12)
        .fontColor('#666666')
        .margin({ top: 4 })
    }
    .width('100%')
    .padding(12)
    .backgroundColor('#F8F8F8')
    .borderRadius(8)
    .justifyContent(FlexAlign.Center)
    .alignItems(HorizontalAlign.Center)
  }
}

// 娣诲姞Contact绫诲瀷瀹氫箟锛堜复鏃讹級
class Contact {
  id: number = 0;
  name: string = '';
  phone: string = '';
  email: string = '';
  company: string = '';
  department: string = '';
  position: string = '';
  address: string = '';
  notes: string = '';
  isFavorite: boolean = false;
  tags: string[] = [];
  createTime: string = '';
  updateTime: string = '';
}





