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
