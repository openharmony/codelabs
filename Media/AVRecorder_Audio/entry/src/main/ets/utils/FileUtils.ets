import fs from '@ohos.file.fs';
import { RECORDINGS_DIR_NAME } from '../constants/Paths';

export function ensureDir(path: string): void {
  try {
    fs.accessSync(path);
  } catch (err) {
    try {
      fs.mkdirSync(path, true);
    } catch (mkdirErr) {
      return;
    }
  }
}

export function ensureRecordingsDir(baseDir: string): string {
  ensureDir(baseDir);
  const dir = `${baseDir}/${RECORDINGS_DIR_NAME}`;
  ensureDir(dir);
  return dir;
}

export function buildRecordPath(baseDir: string, fileName: string): string {
  return `${ensureRecordingsDir(baseDir)}/${fileName}`;
}

export function getFileSize(path: string): number {
  try {
    const stat = fs.statSync(path);
    return stat.size ?? 0;
  } catch (err) {
    return 0;
  }
}

export function deleteFile(path: string): boolean {
  try {
    fs.unlinkSync(path);
    return true;
  } catch (err) {
    return false;
  }
}
