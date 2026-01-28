import subprocess

# 视频和封面对应列表
videos = [f"video{i}.mp4" for i in range(1, 6)]
covers = [f"cover{i}.png" for i in range(1, 6)]

for video, cover in zip(videos, covers):
    # 使用 ffmpeg 提取第一帧
    cmd = [
        "ffmpeg",
        "-i", video,        # 输入视频
        "-ss", "00:00:00",  # 截取 0 秒
        "-vframes", "1",    # 只提取一帧
        cover,              # 输出封面
        "-y"                # 覆盖已有文件
    ]
    
    print(f"正在处理 {video} -> {cover} ...")
    subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    print(f"{cover} 生成完成！")

print("所有封面生成完毕！")
