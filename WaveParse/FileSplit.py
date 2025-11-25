import os

min = 30 #拆分分钟数
chunk_size = 782 * 3*60*50*min   # 每块大小（字节）
input_path = r"F:\冻结为空\17日\开启时间16：25，文件16：04-20：04.BIN"

# 提取目录、基础文件名、扩展名
folder, filename = os.path.split(input_path)
name, ext = os.path.splitext(filename)

with open(input_path, "rb") as f:
    idx = 0
    while True:
        data = f.read(chunk_size)
        if not data:
            break

        # 构造新的文件名
        out_path = os.path.join(folder, f"{name}_{idx}{ext}")

        with open(out_path, "wb") as out:
            out.write(data)
            print(f"file{idx} done ")
        idx += 1