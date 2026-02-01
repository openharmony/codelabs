// DialogUtils.ets
export default class DialogUtils {
  /**
   * 弹出通用告警对话框
   * @param title 标题
   * @param message 内容
   * @param onConfirm 点击确定的回调（可选）
   */
  static showNotice(title: string | Resource, message: string | Resource, onConfirm?: () => void) {
    AlertDialog.show({
      title: title,
      message: message,
      alignment: DialogAlignment.Bottom,
      offset: { dx: 0, dy: -20 },
      primaryButton: {
        value: '取消',
        action: () => {
          console.info('Dialog: Cancelled');
        }
      },
      secondaryButton: {
        value: '确定',
        fontColor: '#007DFF',
        action: () => {
          if (onConfirm) {
            onConfirm();
          }
        }
      },
      autoCancel: true
    });
  }

  /**
   * 只有一个按钮的简单提示
   */
  static showAlert(title: string | Resource, message: string | Resource) {
    AlertDialog.show({
      title: title,
      message: message,
      alignment: DialogAlignment.Center,
      confirm: {
        value: '我知道了',
        action: () => {}
      }
    });
  }
}