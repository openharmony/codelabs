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