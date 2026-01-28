export interface AudioItem {
  id: string
  title: string
  desc: string
  source: Resource
}

export const AUDIO_DATA: AudioItem[] = [
  {
    id: '1',
    title: '正念呼吸背景音',
    desc: '轻柔的环境音，帮助专注当下。',
    source: $rawfile('mindfulness.wav')
  },
  {
    id: '2',
    title: '冥想钟声',
    desc: '适合结束时的提示音。',
    source: $rawfile('meditation.mp3')
  }
]
