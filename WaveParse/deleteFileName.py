import os

def rename_files_remove_before_L(folder):
    for filename in os.listdir(folder):
        old_path = os.path.join(folder, filename)

        # 跳过目录
        if not os.path.isfile(old_path):
            continue

        # 找第一个 L
        idx = filename.find("L")
        if idx == -1:
            continue  # 没有 L 的文件跳过

        # 删除 L 及其前面的所有内容
        new_name = filename[idx+1:]  # +1 代表把 L 本身也删掉
        new_path = os.path.join(folder, new_name)

        print(f"{filename} -> {new_name}")
        os.rename(old_path, new_path)

if __name__ == "__main__":
    folder = r"D:\work\project\ElectricityCycle\电动车精度数据\6"
    rename_files_remove_before_L(folder)
