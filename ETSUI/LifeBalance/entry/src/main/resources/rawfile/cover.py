# Copyright (c) 2026 Huawei Device Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
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
