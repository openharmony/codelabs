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
import os

def count_lines(folder_path):
    total_lines = 0
    print(f"{'文件名':<40} | {'代码行数 (不含空行)'}")
    print("-" * 60)
    
    for root, dirs, files in os.walk(folder_path):
        for file in files:
            if file.endswith('.ets'):
                file_path = os.path.join(root, file)
                try:
                    with open(file_path, 'r', encoding='utf-8') as f:
                        # 过滤掉空行和只有空格的行
                        lines = [line for line in f if line.strip()]
                        line_count = len(lines)
                        total_lines += line_count
                        print(f"{file:<40} | {line_count}")
                except Exception as e:
                    print(f"无法读取文件 {file}: {e}")
                    
    print("-" * 60)
    print(f"{'总计':<40} | {total_lines}")

# 你的路径
# path = r".\entry\src\main\ets\pages"
# path = r".\entry\src\main\ets\common\database"
path = r".\entry\src\main\ets"
count_lines(path)