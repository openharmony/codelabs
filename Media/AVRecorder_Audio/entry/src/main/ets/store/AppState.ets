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

export type ToastListener = (message: string | null) => void;

class AppState {
  private message: string | null = null;
  private listeners: Set<ToastListener> = new Set();

  showToast(message: string): void {
    this.message = message;
    this.notify();
  }

  clearToast(): void {
    this.message = null;
    this.notify();
  }

  subscribeToast(listener: ToastListener): () => void {
    this.listeners.add(listener);
    listener(this.message);
    return () => this.listeners.delete(listener);
  }

  private notify(): void {
    this.listeners.forEach(listener => listener(this.message));
  }
}

export const appState = new AppState();
