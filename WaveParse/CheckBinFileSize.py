# -*- coding: gbk -*-
import os

def check_bin_files(directory: str, expected_size: int):
    """
    检查目录下所有 .bin 文件的大小是否等于指定大小

    :param directory: 目标文件夹路径
    :param expected_size: 期望的文件大小（字节）
    """
    if not os.path.isdir(directory):
        print(f"错误：{directory} 不是有效的文件夹路径")
        return

    files = [f for f in os.listdir(directory) if f.lower().endswith(".bin")]
    if not files:
        print("该目录下没有 .bin 文件")
        return

    all_passed = True
    for filename in files:
        filepath = os.path.join(directory, filename)
        size = os.path.getsize(filepath)
        if size == expected_size:
            print(f"[OK]  {filename} 大小为 {size} 字节，符合要求")
        else:
            print(f"[FAIL] {filename} 大小为 {size} 字节，不符合要求（期望 {expected_size}）")
            all_passed = False

    if all_passed:
        print("\n所有 .bin 文件均符合要求")
    else:
        print("\n部分 .bin 文件大小不符合要求")


if __name__ == "__main__":
    # 示例：修改为你自己的路径和文件大小
    dir_path ="C:\\Users\\宁浩\\Downloads"
    expected_size =73350053

    # dir_path ="F:\\"
    # expected_size =211140000

    check_bin_files(dir_path, expected_size)