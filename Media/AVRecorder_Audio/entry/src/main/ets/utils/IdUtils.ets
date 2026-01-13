export function createId(prefix: string = 'rec'): string {
  const now = Date.now();
  const rand = Math.floor(Math.random() * 1000000);
  return `${prefix}_${now}_${rand}`;
}
