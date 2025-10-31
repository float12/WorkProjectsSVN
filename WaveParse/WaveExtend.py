import os

def extend_frames(frame_size, start_frame, copy_count, src_path):
    """
    从 start_frame 往前10帧开始读取，共 copy_count 帧，
    然后将这些帧附加到文件末尾，输出到当前目录下的 WaveExtend 文件夹中。
    """
    start_byte = max(0, (start_frame - 10) * frame_size)
    copy_bytes = copy_count * frame_size

    # 检查源文件大小
    file_size = os.path.getsize(src_path)
    if start_byte >= file_size:
        print("错误：起始帧位置超出文件大小！")
        return

    # 读取要扩展的帧数据
    with open(src_path, "rb") as fsrc:
        fsrc.seek(start_byte)
        data = fsrc.read(copy_bytes)
        if not data:
            print("错误：读取到的数据为空！")
            return

    # 创建输出目录 WaveExtend
    output_dir = os.path.join(os.getcwd(), "WaveExtend")
    os.makedirs(output_dir, exist_ok=True)

    # 生成输出文件路径（不覆盖原文件）
    base_name = os.path.basename(src_path)
    name_no_ext, ext = os.path.splitext(base_name)
    dst_path = os.path.join(output_dir, f"{name_no_ext}_extended{ext}")

    # 复制原文件 + 扩展数据
    with open(src_path, "rb") as fsrc, open(dst_path, "wb") as fdst:
        begin_data = fsrc.read(start_frame * frame_size)
        left_data = fsrc.read()
        last_frame = begin_data[-frame_size:]
        copy_data = last_frame * copy_count
        fdst.write(begin_data)
        fdst.write(copy_data)
        fdst.write(left_data)
        print(f"{fdst}写入完成")

if __name__ == "__main__":
    try:
        frame_size = 778
        start_frame = 1500
        copy_count = 2000
        src_path =r"D:\WorkProjectsSVN\WaveParse\WaveExtend\电动车运行中，停止，电动车启动，停止_extended_extended.bin"

        extend_frames(frame_size, start_frame, copy_count, src_path)
    except Exception as e:
        print("发生错误：", e)
