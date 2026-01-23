/*
 * Copyright (c) 2025 Beijing Institute of Technology.
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

        title: title,
        text: text,
        additionalText: '鑱旂郴浜虹瀹?
      };

      // 鍒涘缓閫氱煡璇锋眰
      const notificationRequest: notificationManager.NotificationRequest = {
        id: 1,
        content: {
          // 馃敟 淇锛氫娇鐢?@ohos.notification 涓嬬殑鏋氫妇锛岀被鍨嬪畬缇庡尮閰嶏紝鏃犻渶 as any
          contentType: notification.ContentType.NOTIFICATION_CONTENT_BASIC_TEXT,
          normal: basicContent
        }
      };

      await notificationManager.publish(notificationRequest);
      console.info('[NotificationUtil] 閫氱煡鍙戝竷鎴愬姛');
    } catch (error) {
      console.error('[NotificationUtil] 閫氱煡鍙戝竷澶辫触:', JSON.stringify(error));
    }
  }
}

