export interface VideoItem {
  id: string;
  title: string;
  source: Resource;
  desc: string
  cover: ResourceStr;
  orientation: 'landscape' | 'portrait'
}

export const VIDEO_DATA: VideoItem[] = [
  {
    id: "1",
    title: '正念呼吸',
    desc: '帮助你快速平静下来，适合睡前或焦虑时。',
    cover: $rawfile('cover1.png'),
    source: $rawfile('video1.mp4'),
    orientation: 'portrait'
  },
  {
    id: "2",
    title: '身体扫描冥想',
    desc: '一步步觉察身体的放松与紧张。',
    cover: $rawfile('cover2.png'),
    source: $rawfile('video2.mp4'),
    orientation: 'portrait'
  },
  {
    id: "3",
    title: '正念行走',
    desc: '在行走中练习正念，关注每一步的感受，提升专注力。',
    cover: $rawfile('cover3.png'),
    source: $rawfile('video3.mp4'),
    orientation: 'portrait'
  },
  {
    id: "4",
    title: '冥想钟声与放松音',
    desc: '适合冥想结束或阶段切换时聆听，帮助平稳过渡。',
    cover: $rawfile('cover4.png'),
    source: $rawfile('video4.mp4'),
    orientation: 'portrait'
  }
]
