// @ts-nocheck
export default {
    data: {
        title: "图像开发",
        imageUrl: "/common/images/shanghai.jpg",
        isRevolve: true,
        isCut: false,
        isScale: false,
        isMirror: false,
        scaleState: false,
        degrees: 0,
        xDegrees: 0,
        yDegrees: 0,
        rotateIndex: 0,
        width: 576,
        height: 488,
        clipArray:[0, 0, 0, 0]
    },
    // 旋转
    imageRotate() {
        if (!this.isRevolve) {
            this.isRevolve = true
            this.isCut =  false
            this.isScale = false
            this.isMirror = false
            this.rotateIndex = 0
        }
        if (this.rotateIndex <3) {
            this.rotateIndex++
        } else{
            this.rotateIndex = 0
        }
        this.degrees = this.rotateIndex * 90
    },
    // 剪裁
    imageCut() {
        if (!this.isCut) {
            this.isRevolve = false
            this.isCut =  true
            this.isScale = false
            this.isMirror = false

        }
        if (this.clipArray[2] === 0) {
            // 值指的是 top, right, bottom, left 四个点
            this.clipArray = [0, 0, this.height/2,0]
        } else {
            this.clipArray = [0, 0, 0, 0]
        }
    },
    // 缩放
    imageScale() {
        if (!this.isScale) {
            this.isRevolve = false
            this.isCut =  false
            this.isScale = true
            this.isMirror = false
            if (this.width === 576) {
                this.width = this.width/2
                this.height = this.height/2
            }
        } else {
            if (this.width === 576) {
                this.width = this.width/2
                this.height = this.height/2
            } else {
                this.width = this.width*2
                this.height = this.height*2
            }
        }
    },
    imageMirror() {
        if (!this.isMirror) {
            this.isRevolve = false
            this.isCut =  false
            this.isScale = false
            this.isMirror = true
            this.yDegrees = 0
        }
        if (this.yDegrees === 0) {
            this.yDegrees = 180
        } else{
            this.yDegrees = 0
        }
    }
}