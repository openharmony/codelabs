/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
import { Logger } from '../common/utils/Logger';
import { Contact, ContactModel } from '../model/ContactModel';
import { Event, EventModel } from '../model/EventModel';
import { DateUtils } from '../common/utils/DateUtils';
import { StringUtils } from '../common/utils/StringUtils';

/**
 * 导出选项接口
 */
export interface ExportOptions {
  includeContacts: boolean;
  includeEvents: boolean;
  encrypt: boolean;
  format: 'JSON' | 'CSV' | 'XML' | 'HTML';
}

/**
 * 导出结果接口
 */
export interface ExportResult {
  success: boolean;
  filePath: string;
  size: number;
  message: string;
}

//  新增：JSON 导出数据的结构定义 (解决 arkts-no-untyped-obj-literals)
interface ExportMeta {
  version: string;
  timestamp: number;
  device: string;
  user: string;
  encrypted: boolean;
}

interface ExportStats {
  contactCount: number;
  eventCount: number;
}

interface ExportDataset {
  contacts: Contact[];
  events: Event[];
}

interface ExportPayload {
  meta: ExportMeta;
  stats: ExportStats;
  dataset: ExportDataset;
}

/**
 * 进度回调类型
 */
type ProgressCallback = (progress: number, message: string) => void;

/**
 * 数据导出与报表服务 (ArkTS 严格模式修复版)
 */
export class DataExportService {
  private static logger = new Logger('DataExportService');
  private static readonly MOCK_PATH = '/internal/storage/backup/';

  /**
   * 核心导出方法
   */
  static async exportData(options: ExportOptions, onProgress?: ProgressCallback): Promise<ExportResult> {
    DataExportService.logger.info(`Starting export with options: ${JSON.stringify(options)}`);

    if (onProgress) {
      onProgress(10, 'Initializing export engine...');
    }
    await DataExportService.simulateDelay(500);

    let resultData = '';

    //  注意：请确保 EventModel.ets 已更新并包含 queryAll 方法
    const contacts = options.includeContacts ? await ContactModel.queryAll() : [];
    //  修复：如果 EventModel 还没更新，这里会暂时报错，请继续操作第二步
    const events = options.includeEvents ? await EventModel.queryAll() : [];

    if (onProgress) {
      onProgress(30, 'Fetching data from database...');
    }

    try {
      switch (options.format) {
        case 'JSON':
          resultData = await DataExportService.generateJson(contacts, events, options.encrypt);
          break;
        case 'CSV':
          resultData = await DataExportService.generateCsv(contacts, events);
          break;
        case 'XML':
          resultData = await DataExportService.generateXml(contacts, events);
          break;
        case 'HTML':
          resultData = await DataExportService.generateHtml(contacts, events);
          break;
        default:
          throw new Error('Unsupported format');
      }
    } catch (err) {
      DataExportService.logger.error(`Export generation failed: ${JSON.stringify(err as object)}`);
      return {
        success: false,
        filePath: '',
        size: 0,
        message: 'Generation Error'
      };
    }

    if (onProgress) {
      onProgress(80, 'Finalizing file writing...');
    }
    await DataExportService.simulateDelay(500);

    const fileName = `backup_${Date.now()}.${options.format.toLowerCase()}`;
    const fileSize = resultData.length;

    if (onProgress) {
      onProgress(100, 'Export completed.');
    }

    DataExportService.logger.info(`Export Success. File: ${fileName}, Size: ${fileSize} bytes`);

    return {
      success: true,
      filePath: DataExportService.MOCK_PATH + fileName,
      size: fileSize,
      message: 'Export Successful'
    };
  }

  // ==========================================
  // 1. JSON 生成器

  static async mockCloudUpload(fileContent: string): Promise<boolean> {
    DataExportService.logger.info('Initiating mock cloud upload...');
    const totalChunks = 5;

    for (let i = 1; i <= totalChunks; i++) {
      await DataExportService.simulateDelay(300);
      DataExportService.logger.info(`Uploading chunk ${i}/${totalChunks}...`);
    }

    DataExportService.logger.info('Upload Complete. Server responded: 200 OK');
    return true;
  }

  // ==========================================
  // 2. CSV 生成器

  // ==========================================
  private static async generateJson(contacts: Contact[], events: Event[], encrypt: boolean): Promise<string> {
    DataExportService.logger.info('Generating JSON...');

    //  修复：显式使用接口类型定义对象字面量
    const meta: ExportMeta = {
      version: '2.0',
      timestamp: Date.now(),
      device: 'HarmonyOS Mock Device',
      user: 'CurrentUser',
      encrypted: encrypt
    };

    const stats: ExportStats = {
      contactCount: contacts.length,
      eventCount: events.length
    };

    const dataset: ExportDataset = {
      contacts: contacts,
      events: events
    };

    const payload: ExportPayload = {
      meta: meta,
      stats: stats,
      dataset: dataset
    };

    let jsonStr = JSON.stringify(payload, null, 2);

    if (encrypt) {
      jsonStr = `ENCRYPTED_HEADER_V1:${StringUtils.generateUUID()}:${jsonStr.split('').reverse().join('')}`;
    }

    await DataExportService.simulateDelay(200);
    return jsonStr;
  }

  // ==========================================
  // 3. XML 生成器

  // ==========================================
  private static async generateCsv(contacts: Contact[], events: Event[]): Promise<string> {
    DataExportService.logger.info('Generating CSV...');

    let csv = '=== CONTACTS ===\nID,Name,Phone,Email,Relation\n';

    for (const c of contacts) {
      csv += `${c.id},"${c.name}","${c.phone}","${c.email}","${c.relation}"\n`;
    }

    csv += '\n=== EVENTS ===\nID,Title,StartTime,EndTime,Description\n';

    for (const e of events) {
      csv += `${e.id},"${e.title}",${e.startTime},${e.endTime},"${e.description}"\n`;
    }

    await DataExportService.simulateDelay(300);
    return csv;
  }

  // ==========================================
  // 4. HTML 生成器

  // ==========================================
  private static async generateXml(contacts: Contact[], events: Event[]): Promise<string> {
    DataExportService.logger.info('Generating XML...');

    let xml = '<?xml version="1.0" encoding="UTF-8"?>\n';
    xml += '<BackupData>\n';
    xml += '  <Metadata>\n';
    xml += `    <Created>${new Date().toISOString()}</Created>\n`;
    xml += '    <Version>1.0</Version>\n';
    xml += '  </Metadata>\n';

    xml += '  <Contacts>\n';
    for (const c of contacts) {
      xml += '    <Contact>\n';
      xml += `      <Id>${c.id}</Id>\n`;
      xml += `      <Name>${DataExportService.escapeXml(c.name)}</Name>\n`;
      xml += `      <Phone>${DataExportService.escapeXml(c.phone)}</Phone>\n`;
      xml += `      <Relation>${DataExportService.escapeXml(c.relation)}</Relation>\n`;
      xml += '    </Contact>\n';
    }
    xml += '  </Contacts>\n';

    xml += '  <Events>\n';
    for (const e of events) {
      xml += '    <Event>\n';
      xml += `      <Id>${e.id}</Id>\n`;
      xml += `      <Title>${DataExportService.escapeXml(e.title)}</Title>\n`;
      xml += `      <Start>${new Date(e.startTime).toISOString()}</Start>\n`;
      xml += `      <End>${new Date(e.endTime).toISOString()}</End>\n`;
      xml += '    </Event>\n';
    }
    xml += '  </Events>\n';
    xml += '</BackupData>';

    await DataExportService.simulateDelay(400);
    return xml;
  }

  // ==========================================
  // 辅助方法
  // ==========================================

  // ==========================================
  private static async generateHtml(contacts: Contact[], events: Event[]): Promise<string> {
    DataExportService.logger.info('Generating HTML Report...');

    const nowStr = new Date().toLocaleString();

    let html = `
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title>Data Backup Report</title>
<style>
  body { font-family: Arial, sans-serif; background-color: #f4f4f9; padding: 20px; }
  .container { max-width: 800px; margin: 0 auto; background: white; padding: 30px; border-radius: 8px; box-shadow: 0 2px 5px rgba(0,0,0,0.1); }
  h1 { color: #333; border-bottom: 2px solid #007bff; padding-bottom: 10px; }
  h2 { color: #555; margin-top: 30px; }
  .meta { color: #888; font-size: 0.9em; margin-bottom: 20px; }
  table { width: 100%; border-collapse: collapse; margin-top: 10px; }
  th, td { border: 1px solid #ddd; padding: 12px; text-align: left; }
  th { background-color: #007bff; color: white; }
  tr:nth-child(even) { background-color: #f2f2f2; }
  .badge { display: inline-block; padding: 4px 8px; border-radius: 4px; font-size: 0.8em; color: white; }
  .bg-green { background-color: #28a745; }
  .bg-orange { background-color: #fd7e14; }
</style>
</head>
<body>
<div class="container">
  <h1>Personal Assistant Pro - Backup</h1>
  <div class="meta">Generated: ${nowStr}</div>

  <div class="stats">
    <p><strong>Total Contacts:</strong> ${contacts.length}</p>
    <p><strong>Total Events:</strong> ${events.length}</p>
  </div>
`;

    html += '<h2>Contacts List</h2>\n';
    html += '<table>\n<thead><tr><th>ID</th><th>Name</th><th>Relation</th></tr></thead>\n<tbody>\n';
    if (contacts.length === 0) {
      html += '<tr><td colspan="3" style="text-align:center">No Contacts Found</td></tr>\n';
    } else {
      contacts.forEach(c => {
        html += `<tr><td>${c.id}</td><td>${c.name}</td><td>${c.relation}</td></tr>\n`;
      });
    }
    html += '</tbody></table>\n';

    html += '<h2>Events Schedule</h2>\n';
    html += '<table>\n<thead><tr><th>Title</th><th>Time</th><th>Status</th></tr></thead>\n<tbody>\n';
    const now = Date.now();

    if (events.length === 0) {
      html += '<tr><td colspan="3" style="text-align:center">No Events Found</td></tr>\n';
    } else {
      events.forEach(e => {
        const isFuture = e.startTime > now;
        const statusBadge = isFuture
          ? '<span class="badge bg-green">Upcoming</span>'
          : '<span class="badge bg-orange">Done</span>';

        html += `<tr>
          <td>${e.title}</td>
          <td>${DateUtils.formatDate(e.startTime)}</td>
          <td>${statusBadge}</td>
        </tr>\n`;
      });
    }

    html += '</tbody></table>\n';
    html += '<div style="margin-top:40px; text-align:center; color:#ccc;">End of Report</div>\n';
    html += '</div></body></html>';

    await DataExportService.simulateDelay(600);
    return html;
  }

  private static escapeXml(unsafe: string): string {
    if (!unsafe) {
      return '';
    }
    return unsafe.replace(/[<>&'"]/g, (c) => {
      switch (c) {
        case '<':
          return '&lt;';
        case '>':
          return '&gt;';
        case '&':
          return '&amp;';
        case '\'':
          return '&apos;';
        case '"':
          return '&quot;';
      }
      return c;
    });
  }

  private static simulateDelay(ms: number): Promise<void> {
    return new Promise(resolve => setTimeout(resolve, ms));
  }
}