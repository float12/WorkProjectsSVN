# -*- coding: gbk -*-
import os
import glob

def truncate_bin_file(filepath, discard_size=0, keep_size=815*9000):
    """
    丢弃文件前 discard_size 字节，只保留后续 keep_size 字节。
    若 discard_size=0 表示不丢弃；

    """
    with open(filepath, "rb") as f:
        f.seek(0, os.SEEK_END)
        file_size = f.tell()

        # 计算要保留的数据长度
        if keep_size == 0 or (discard_size + keep_size) > file_size:
            keep_size = file_size - discard_size

        # 从丢弃位置开始读取
        f.seek(discard_size)
        data = f.read(keep_size)

    # 覆盖写回，只保留目标数据
    with open(filepath, "wb") as f:
        f.write(data)

# 判断输入路径是文件还是目录，分别处理
bin_dir_or_file = r"Z:\01低压台区产品研发\02软件研发\12-负荷辨识\电动车相关原始波形数据\合成电动车波形（ui系数0.0001）\回放波形\扩展-0功率，电动车启动，0功率，电动车启动，停止wlb_Transmit.bin"# 可以改成文件路径或目录路径
discard_size = 0#512、53
keep_size = 20000 *778  # 若 keep_size=0 表示全保留（即从 discard_size 到文件末尾都保留）。

if os.path.isfile(bin_dir_or_file):
    # 输入的是单个文件
    bin_files = [bin_dir_or_file]
elif os.path.isdir(bin_dir_or_file):
    # 输入的是目录，遍历该目录下所有bin文件
    bin_files = glob.glob(os.path.join(bin_dir_or_file, "*.bin"))
else:
    print(f"输入路径无效: {bin_dir_or_file}")
    bin_files = []

for bin_file in bin_files:
    truncate_bin_file(bin_file, discard_size, keep_size)
    print(f"{bin_file} 处理完成")

