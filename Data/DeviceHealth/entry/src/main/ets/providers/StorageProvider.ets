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
