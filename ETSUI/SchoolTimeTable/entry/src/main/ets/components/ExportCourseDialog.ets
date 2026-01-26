import { CourseDataService } from '../service/CourseDataService';
import { Course, WeekDay } from '../model/CourseModel';
import promptAction from '@ohos.promptAction';
import picker from '@ohos.file.picker';
import fs from '@ohos.file.fs';
import { BusinessError } from '@ohos.base';

@CustomDialog
export struct ExportCourseDialog {
  controller: CustomDialogController;
  @State courseJson: string = '';
  private courseService: CourseDataService = CourseDataService.getInstance();
  // 预览用的二维数组 [节次][星期0-6]
  @State previewData: string[][] = [];
  private maxSection: number = 12; // 默认最大节次，可根据实际课程调整

  aboutToAppear() {
    this.preparePreviewData();
  }

  private preparePreviewData() {
    const courses = this.courseService.getAllCourses();
    // 初始化 12 行 7 列的数据 (周一到周日)
    // 实际需要 8 列，第一列是节次
    this.previewData = [];

    // 计算最大节次
    courses.forEach(c => {
      if (c.endSection > this.maxSection) {
        this.maxSection = c.endSection;
      }
    });

    for (let i = 0; i < this.maxSection; i++) {
      const row: string[] = new Array(8).fill('');
      row[0] = `第${i + 1}节`; // 第一列
      this.previewData.push(row);
    }

    // 填充课程数据
    courses.forEach(course => {
      // 课程的星期 (1-7) -> 数组索引 (1-7)
      const dayIndex = course.day;
      const courseText = `${course.name}\n${course.location}`;

      for (let section = course.startSection; section <= course.endSection; section++) {
        if (section <= this.maxSection) {
          // 节次 (1-based) -> 数组索引 (0-based)
          const rowIndex = section - 1;
          // 如果该格子已有内容（冲突），则追加
          if (this.previewData[rowIndex][dayIndex]) {
            this.previewData[rowIndex][dayIndex] += `\n\n${courseText}`;
          } else {
            this.previewData[rowIndex][dayIndex] = courseText;
          }
        }
      }
    });
  }

  private async exportToExcel() {
    try {
      const courses = this.courseService.getAllCourses();

      // 1. 生成 CSV 内容 (符合导入格式标准)
      let csvContent = '\uFEFF'; // BOM for Excel UTF-8
      // 表头：课程名,教室,教师,星期(1-7),开始节次,结束节次,周数范围
      csvContent += '课程名称,上课地点,授课教师,星期,开始节次,结束节次,周数范围\n';

      // 数据行
      courses.forEach(course => {
        const row = [
          course.name,
          course.location,
          course.teacher,
          course.day.toString(),
          course.startSection.toString(),
          course.endSection.toString(),
          `${course.startWeek}-${course.endWeek}`
        ];

        // 转义处理
        const escapedRow = row.map(cell => {
          const str = String(cell || '');
          if (str.includes(',') || str.includes('\n') || str.includes('"')) {
            return `"${str.replace(/"/g, '""')}"`;
          }
          return str;
        });
        csvContent += escapedRow.join(',') + '\n';
      });

      // 2. 创建文件保存选择器
      const documentSaveOptions = new picker.DocumentSaveOptions();
      documentSaveOptions.newFileNames = ['课程表.csv'];
      documentSaveOptions.fileSuffixChoices = ['.csv'];

      const documentViewPicker = new picker.DocumentViewPicker();

      // 3. 用户选择保存路径
      const documentSaveResult = await documentViewPicker.save(documentSaveOptions);

      if (documentSaveResult && documentSaveResult.length > 0) {
        const uri = documentSaveResult[0];
        // 4. 写入文件
        const file = fs.openSync(uri, fs.OpenMode.READ_WRITE | fs.OpenMode.CREATE | fs.OpenMode.TRUNC);
        fs.writeSync(file.fd, csvContent);
        fs.closeSync(file);

        promptAction.showToast({
          message: '导出成功',
          duration: 2000
        });
        this.controller.close();
      }
    } catch (error) {
      console.error('Export failed:', error);
      promptAction.showToast({
        message: '导出失败: ' + (error as BusinessError).message,
        duration: 2000
      });
    }
  }

  build() {
    Column() {
      Text('导出课表预览')
        .fontSize(20)
        .fontWeight(FontWeight.Bold)
        .fontColor('#333333')
        .margin({ bottom: 16, top: 8 })

      Text('预览即将导出的Excel(CSV)文件内容 (导出格式为标准数据列表，可用于备份和重新导入)')
        .fontSize(14)
        .fontColor('#666666')
        .margin({ bottom: 12 })

      // 表格预览
      Scroll() {
        Scroll() {
          Column() {
            // 表头
            Row() {
              ForEach(['节次', '周一', '周二', '周三', '周四', '周五', '周六', '周日'], (item: string) => {
                Text(item)
                  .fontSize(12)
                  .fontWeight(FontWeight.Bold)
                  .fontColor('#333333')
                  .width(item === '节次' ? 50 : 80)
                  .height(40)
                  .textAlign(TextAlign.Center)
                  .border({ width: 0.5, color: '#E0E0E0' })
                  .backgroundColor('#F0F0F0')
              })
            }

            // 内容
            ForEach(this.previewData, (row: string[], index: number) => {
              Row() {
                ForEach(row, (cell: string, colIndex: number) => {
                  Text(cell)
                    .fontSize(10)
                    .fontColor('#333333')
                    .width(colIndex === 0 ? 50 : 80)
                    .height(60)
                    .textAlign(TextAlign.Center)
                    .border({ width: 0.5, color: '#E0E0E0' })
                    .padding(4)
                    .textOverflow({ overflow: TextOverflow.Ellipsis })
                })
              }
            })
          }
        }
        .scrollable(ScrollDirection.Horizontal) // 内部支持横向滚动
      }
      .scrollable(ScrollDirection.Vertical) // 外部支持纵向滚动
      .width('100%')
      .height(300)
      .margin({ bottom: 20 })
      .backgroundColor('#FFFFFF')
      .border({ width: 1, color: '#E0E0E0' })

      Row() {
        Button('取消')
          .fontSize(16)
          .fontColor('#666666')
          .backgroundColor('#F0F0F0')
          .layoutWeight(1)
          .height(44)
          .borderRadius(22)
          .margin({ right: 12 })
          .onClick(() => {
            this.controller.close();
          })

        Button('导出到文件')
          .fontSize(16)
          .fontColor('#FFFFFF')
          .backgroundColor('#007DFF')
          .layoutWeight(1)
          .height(44)
          .borderRadius(22)
          .onClick(() => {
            this.exportToExcel();
          })
      }
      .width('100%')
    }
    .padding(24)
    .backgroundColor('#FFFFFF')
    .borderRadius(16)
    .width('95%') // 增加宽度以显示更多表格内容
  }
}
