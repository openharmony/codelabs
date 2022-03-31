export default class SongInfo {
  private name: string
  private fileUri: string
  private duration: number

  constructor(name, fileUri, duration) {
    this.name = name;
    this.fileUri = fileUri;
    this.duration = duration;
  }

  setName(name: string) {
    this.name = name
  }

  getName() {
    return this.name
  }

  setFileUri(uri: string) {
    this.fileUri = uri
  }

  getFileUri() {
    return this.fileUri
  }

  setDuration(duration: number) {
    this.duration = duration
  }

  getDuration() {
    return this.duration
  }
}