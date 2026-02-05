/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

import storageStatistics from '@ohos.file.storageStatistics';

export interface StorageStatus {
  freeBytes: number | null;
  totalBytes?: number | null;
}

export interface IStorageProvider {
  getStatus(): Promise<StorageStatus>;
}

export class StorageStatusImpl implements StorageStatus {
  freeBytes: number | null;
  totalBytes?: number | null;

  constructor(freeBytes: number | null, totalBytes?: number | null) {
    this.freeBytes = freeBytes;
    this.totalBytes = totalBytes;
  }
}

export class DefaultStorageProvider implements IStorageProvider {
  async getStatus(): Promise<StorageStatus> {
    try {
      const freeBytes: number = await new Promise<number>((resolve, reject) => {
        storageStatistics.getFreeSize((err: Error | null, size: number) => {
          if (err) reject(err);
          else resolve(size);
        });
      });

      return new StorageStatusImpl(freeBytes);
    } catch (_) {
      return new StorageStatusImpl(null);
    }
  }
}
