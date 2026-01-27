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
 */
import { CourseDataService } from '../service/CourseDataService';
import { Course, ExportData } from '../model/CourseModel';
import promptAction from '@ohos.promptAction';
import picker from '@ohos.file.picker';
import fs from '@ohos.file.fs';
import util from '@ohos.util';
import { ColorConstants } from '../model/ColorConstants';

@CustomDialog
export struct ImportCourseDialog {
  controller: CustomDialogController;
  @State importContent: string = '';
  private courseService: CourseDataService = CourseDataService.getInstance();

  private async selectFile() {
    try {
      const documentSelectOptions = new picker.DocumentSelectOptions();
      // 建议选择文件
      documentSelectOptions.maxSelectNumber = 1;

      const documentViewPicker = new picker.DocumentViewPicker();
      const documentSelectResult = await documentViewPicker.select(documentSelectOptions);

      if (documentSelectResult && documentSelectResult.length > 0) {
        const uri = documentSelectResult[0];
        const file = fs.openSync(uri, fs.OpenMode.READ_ONLY);
        const fileStat = fs.statSync(file.fd);
        const buf = new ArrayBuffer(fileStat.size);
        fs.readSync(file.fd, buf);
        fs.closeSync(file);

        const decoder = util.TextDecoder.create('utf-8', { ignoreBOM: true });
        this.importContent = decoder.decodeWithStream(new Uint8Array(buf));

        promptAction.showToast({ message: '文件读取成功', duration: 1500 });
      }
    } catch (error) {
      console.error('File selection failed:', error);
      promptAction.showToast({ message: '文件读取失败', duration: 2000 });
    }
  }

  private parseCsv(content: string): Course[] {
    const lines = content.split(/\r\n|\n|\r/).filter(line => line.trim() !== '');
    if (lines.length < 2) {
      return [];
    } // 至少要有表头和一行数据

    const courses: Course[] = [];
    const colors: string[] = ColorConstants.MACARON_COLORS;

    // 跳过第一行表头
    for (let i = 1; i < lines.length; i++) {
      const line = lines[i];
      // 使用正则分割 CSV，处理引号内的逗号
      // 匹配逗号，前提是该逗号后面紧跟着偶数个引号（即不在引号对内部）
      const parts = line.split(/,(?=(?:(?:[^"]*"){2})*[^"]*$)/);

      if (parts.length < 6) {
        continue;
      }

      try {
        const clean = (s: string) => {
          if (!s) {
            return '';
          }
          let val = s.trim();
          // 去除首尾引号
          if (val.startsWith('"') && val.endsWith('"')) {
            val = val.substring(1, val.length - 1);
          }
          // 处理双引号转义 ("" -> ")
          return val.replace(/""/g, '"');
        };

        const name = clean(parts[0]);
        const location = clean(parts[1]);
        const teacher = clean(parts[2]);
        const day = parseInt(parts[3].trim());
        const startSection = parseInt(parts[4].trim());
        const endSection = parseInt(parts[5].trim());

        // 可选字段
        let startWeek = 1;
        let endWeek = 18;
        if (parts.length > 6) {
          const weeks = clean(parts[6]);
          if (weeks.includes('-')) {
            const w = weeks.split('-');
            startWeek = parseInt(w[0]) || 1;
            endWeek = parseInt(w[1]) || 18;
          }
        }

        if (name && !isNaN(day) && day >= 1 && day <= 7 && !isNaN(startSection) && startSection > 0 &&
          !isNaN(endSection) && endSection >= startSection) {
          const course: Course = {
            id: util.generateRandomUUID(true) + '_' + i, // 添加索引确保唯一性
            name: name,
            location: location,
            teacher: teacher,
            day: day,
            startSection: startSection,
            endSection: endSection,
            color: colors[courses.length % colors.length], // 循环分配颜色
            remark: '',
            semester: '',
            startWeek: startWeek,
            endWeek: endWeek,
            credit: 0
          };
          courses.push(course);
        }
      } catch (e) {
        console.warn(`Skipping invalid CSV line ${i + 1}: ${line}`);
      }
    }
    return courses;
  }

  private parseAndImport(mode: 'overwrite' | 'append') {
    if (!this.importContent.trim()) {
      promptAction.showToast({ message: '请先输入内容或选择文件', duration: 2000 });
      return;
    }

    try {
      let newCourses: Course[] = [];
      const content = this.importContent.trim();

      // 简单的格式探测：如果是 JSON (兼容旧版备份数据)
      if (content.startsWith('{') || content.startsWith('[')) {
        const parsed = JSON.parse(this.importContent) as ExportData | Course[];
        if (Array.isArray(parsed)) {
          newCourses = parsed as Course[];
        } else {
          // 此时 parsed 应该是 ExportData 类型，但为了安全起见做属性检查
          const obj = parsed as ExportData;
          if (obj.courses && Array.isArray(obj.courses)) {
            newCourses = obj.courses;
          } else {
            throw new Error('格式无法识别');
          }
        }
      } else {
        // 尝试作为 CSV 解析
        newCourses = this.parseCsv(content);
        if (newCourses.length === 0) {
          throw new Error('未识别到有效的 CSV 数据');
        }
      }

      if (newCourses.length === 0) {
        promptAction.showToast({ message: '未发现课程数据', duration: 2000 });
        return;
      }

      if (mode === 'overwrite') {
        // 覆盖模式：直接构造 ExportData 格式传给 importData
        const importData = JSON.stringify({ courses: newCourses });
        const success = this.courseService.importData(importData);
        if (success) {
          promptAction.showToast({ message: `成功导入 ${newCourses.length} 门课程`, duration: 2000 });
          this.controller.close();
        } else {
          promptAction.showToast({ message: '导入失败', duration: 2000 });
        }
      } else {
        // 追加模式：启动递归处理流程
        // 强制重新获取最新数据，确保 clearAllCourses 生效
        const currentCourses = [...this.courseService.getAllCourses()];
        const initialCount = currentCourses.length;
        console.info(`Starting append import. Initial courses count: ${initialCount}`);
        this.processAppendCourses(newCourses, currentCourses, initialCount);
      }
    } catch (error) {
      console.error('Import parsing failed:', error);
      promptAction.showToast({ message: '数据格式错误', duration: 2000 });
    }
  }

  private checkConflict(courseA: Course, courseB: Course): boolean {
    if (courseA.day !== courseB.day) {
      return false;
    }
    return (courseA.startSection <= courseB.endSection) && (courseA.endSection >= courseB.startSection);
  }

  private isSameCourse(courseA: Course, courseB: Course): boolean {
    return courseA.name === courseB.name &&
      courseA.location === courseB.location &&
      courseA.teacher === courseB.teacher &&
      courseA.day === courseB.day &&
      courseA.startSection === courseB.startSection &&
      courseA.endSection === courseB.endSection &&
      courseA.startWeek === courseB.startWeek &&
      courseA.endWeek === courseB.endWeek;
  }

  private processAppendCourses(newCourses: Course[], currentCourses: Course[], initialCount: number) {
    this.processNextCourse(0, newCourses, currentCourses, initialCount);
  }

  private processNextCourse(index: number, newCourses: Course[], currentCourses: Course[], initialCount: number) {
    if (index >= newCourses.length) {
      // 所有课程处理完毕，保存数据
      const importData = JSON.stringify({ courses: currentCourses });
      const success = this.courseService.importData(importData);
      if (success) {
        promptAction.showToast({ message: `导入完成`, duration: 2000 });
        this.controller.close();
      } else {
        promptAction.showToast({ message: '导入保存失败', duration: 2000 });
      }
      return;
    }

    const newCourse = newCourses[index];
    const conflictIndex = currentCourses.findIndex(existing => this.checkConflict(existing, newCourse));

    if (conflictIndex !== -1) {
      const oldCourse = currentCourses[conflictIndex];

      // 检查是否完全一致
      if (this.isSameCourse(oldCourse, newCourse)) {
        // 完全一致，直接跳过，不提示冲突
        this.processNextCourse(index + 1, newCourses, currentCourses, initialCount);
        return;
      }

      // 发现冲突且不完全一致，弹出确认框
      const isInternalConflict = conflictIndex >= initialCount;
      const conflictTitle = isInternalConflict ? '导入文件内部冲突' : '与现有课程冲突';
      const conflictSource = isInternalConflict ? '【文件内已有】' : '【原课程】';

      AlertDialog.show({
        title: conflictTitle,
        message: `检测到时间冲突：\n\n${conflictSource}${oldCourse.name}\n时间：周${oldCourse.day} 第${oldCourse.startSection}-${oldCourse.endSection}节\n\n【新课程】${newCourse.name}\n时间：周${newCourse.day} 第${newCourse.startSection}-${newCourse.endSection}节\n\n是否使用新课程覆盖？`,
        autoCancel: false,
        alignment: DialogAlignment.Center,
        primaryButton: {
          value: '保留原有',
          action: () => {
            // 保留原课程，跳过新课程
            this.processNextCourse(index + 1, newCourses, currentCourses, initialCount);
          }
        },
        secondaryButton: {
          value: '覆盖',
          fontColor: '#D94841',
          action: () => {
            // 覆盖原课程
            newCourse.id = oldCourse.id; // 保持ID一致（可选）
            currentCourses[conflictIndex] = newCourse;
            this.processNextCourse(index + 1, newCourses, currentCourses, initialCount);
          }
        }
      });
    } else {
      // 无冲突，直接添加
      // 强制重新生成 ID，防止来自 CSV 解析的 ID 发生重复
      newCourse.id = util.generateRandomUUID(true);
      currentCourses.push(newCourse);
      this.processNextCourse(index + 1, newCourses, currentCourses, initialCount);
    }
  }

  build() {
    Column() {
      Text('课表导入')
        .fontSize(20)
        .fontWeight(FontWeight.Bold)
        .fontColor('#333333')
        .margin({ bottom: 16, top: 8 })

      Text('支持导入 CSV 格式数据: 课程名,地点,教师,周几,开始节,结束节')
        .fontSize(12)
        .fontColor('#999999')
        .margin({ bottom: 4 })

      Text('例如: 高等数学,A101,张三,1,1,2')
        .fontSize(12)
        .fontColor('#999999')
        .margin({ bottom: 12 })

      TextArea({ text: this.importContent, placeholder: '在此粘贴 CSV 数据，或点击下方按钮选择文件' })
        .width('100%')
        .height(200)
        .backgroundColor('#F5F5F5')
        .borderRadius(8)
        .fontSize(14)
        .fontColor('#333333')
        .margin({ bottom: 12 })
        .onChange((value) => {
          this.importContent = value;
        })

      Row() {
        Button('选择文件')
          .fontSize(14)
          .fontColor('#007DFF')
          .backgroundColor('#E6F2FF')
          .height(36)
          .borderRadius(18)
          .onClick(() => {
            this.selectFile();
          })

        Blank()

        Button('清空')
          .fontSize(14)
          .fontColor('#666666')
          .backgroundColor(Color.Transparent)
          .height(36)
          .onClick(() => {
            this.importContent = '';
          })
      }
      .width('100%')
      .margin({ bottom: 20 })

      Row({ space: 12 }) {
        Button('取消')
          .fontSize(16)
          .fontColor('#666666')
          .backgroundColor('#F0F0F0')
          .layoutWeight(1)
          .height(44)
          .borderRadius(22)
          .onClick(() => {
            this.controller.close();
          })

        Button('追加导入')
          .fontSize(16)
          .fontColor('#FFFFFF')
          .backgroundColor('#4ECDC4') // 区分颜色
          .layoutWeight(1)
          .height(44)
          .borderRadius(22)
          .onClick(() => {
            this.parseAndImport('append');
          })

        Button('覆盖导入')
          .fontSize(16)
          .fontColor('#FFFFFF')
          .backgroundColor('#007DFF')
          .layoutWeight(1)
          .height(44)
          .borderRadius(22)
          .onClick(() => {
            promptAction.showDialog({
              title: '警告',
              message: '覆盖导入将清空当前所有课程，确定继续吗？',
              buttons: [
                { text: '取消', color: '#007DFF' },
                { text: '确定覆盖', color: '#FF0000' }
              ]
            }).then(result => {
              if (result.index === 1) {
                this.parseAndImport('overwrite');
              }
            });
          })
      }
      .width('100%')
    }
    .padding(24)
    .backgroundColor('#FFFFFF')
    .borderRadius(16)
    .width('95%')
  }
}
