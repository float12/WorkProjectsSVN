#!/usr/bin/python
# -*- coding: gbk -*-
#叠加物联表录波格式波形并转为物联表录波格式,支持单相、物联表录波带序号和原始无序号格式,转换后一个周波128点
from configparser import Interpolation
import numpy as np
import itertools
import ParseUIData
import FormatConvert
import math
import os
import glob
import matplotlib.pyplot as plt
class WlbWaveRecordWaveAdder:
    def __init__(self, file1path, file2path, frame_size=778, uifactor1=(0.0003742375, 0, 0, 0.00001014207, 0, 0), uifactor2=(0.0003742375, 0, 0, 0.00001014207, 0, 0), add_frame_num=0, format=2,peak_value=0, add_or_abstract=0):
        # 文件1路径
        self.file1path = file1path
        # 文件2路径
        self.file2path = file2path
        # 每帧字节数，默认为778（物联表录波格式），782（有序号的物联表录波格式）
        self.frame_size = frame_size
        # 文件1的UI系数（电压电流缩放因子）
        self.uifactor1 = uifactor1
        # 文件2的UI系数（电压电流缩放因子）
        self.uifactor2 = uifactor2
        # 叠加的帧数，0表示全部叠加
        self.add_frame_num = add_frame_num
        # 输入文件格式，2表示物联表录波格式
        self.format = format
        self.rmsu1_above_200_count = 0
        self.rmsu1_below_100_count = 0
        self.rmsu1_started = False
        self.last_rmsu1 = 0
        self.rmsu2_above_200_count = 0
        self.rmsu2_below_100_count = 0
        self.rmsu2_started = False
        self.last_rmsu2 = 0
        self.peak_value = peak_value
        self.add_or_abstract = add_or_abstract
    # 插值算法：输入data（长度任意），输出定长128点的6.4k采样数据
    def interpolate_to_128(self, data):
        """
        把输入序列 data 插值/抽值到长度 128。
        """
        def lcm(a, b):
            return a * b // math.gcd(a, b)

        arr = list(data)
        n = len(arr)

        # 直接线性插值到128点（等价于先扩展到lcm再抽样，更高效）
        result = [0.0] * 128
        for k in range(128):
            t = k / 127               # 相对位置 [0,1]
            pos = t * (n - 1)         # 在原数组上的浮点索引
            i = int(math.floor(pos))
            if i >= n - 1:
                result[k] = float(arr[-1])
            else:
                frac = pos - i
                result[k] = float(arr[i]) + (float(arr[i + 1]) - float(arr[i])) * frac
        return result

    @staticmethod
    def reverse_every_three(data: bytes) -> bytes:
        out = bytearray()
        for i in range(0, len(data), 3):
            group = data[i:i+3]
            out.extend(group[::-1])  # 倒序追加
        return bytes(out)
    def find_all_zero_cross(self,u):
        """找到所有负到正过零点索引"""
        idxs = [0]#第一个点已经是过零点了
        flag = 0
        for i in range(len(u) - 1):
            if  u[i] <=0 and u[i+1] > 0  and u[i+1] -u[i]  > 2:
                idxs.append(i + 1)
        return idxs
    def align_waveform_to_128(self,voltage, current, voltage_aligned_128, current_aligned_128, name="波形"):
        zero_crossings = self.find_all_zero_cross(voltage)
        for i in range(len(zero_crossings) - 1):
            cycle_len = zero_crossings[i + 1] - zero_crossings[i]
            start, end = zero_crossings[i], zero_crossings[i + 1]

            if cycle_len < 128:
                if cycle_len < 126:
                    print(f"{name}{i + 1}: {cycle_len} 点，index：{end}，{start}")
                v_cycle = self.interpolate_to_128(voltage[start:end])
                c_cycle = self.interpolate_to_128(current[start:end])
            elif cycle_len > 128:
                if cycle_len > 130:
                    print(f"{name}{i + 1}: {cycle_len} 点，index：{end}，{start}")
                v_cycle = self.interpolate_to_128(voltage[start:end])
                c_cycle = self.interpolate_to_128(current[start:end])
            else:  # 正好128点
                v_cycle = voltage[start:end]
                c_cycle = current[start:end]

            # 写入目标数组
            voltage_aligned_128[i * 128:(i + 1) * 128] = v_cycle
            current_aligned_128[i * 128:(i + 1) * 128] = c_cycle

    def find_zero_cross(self,u):
        UstartFlag = 0
        """找到第一个负到正的过零点索引"""
        for i in range(len(u) - 1):
            if UstartFlag == 0 and abs(u[i]) > 60:
                UstartFlag = 1
            if UstartFlag == 1 and u[i] <= 0 and u[i + 1] > 0 and u[i + 1] - u[i] > 2 and u[i + 2] - u[i + 1] > 2:
                return i + 1  # 取过零点之后那个点
        return 0  # 如果没找到就从头

    def align_and_add(self,ua, ia, ub, ib):
        """
        对齐两个电器的电压电流波形，并叠加
        ua, ia: 设备 A 的电压电流波形 (numpy array)
        ub, ib: 设备 B 的电压电流波形 (numpy array)
        """

        # 找到电压波形对齐点
        idx_a = self.find_zero_cross(ua)
        idx_b = self.find_zero_cross(ub)

        start_ua =ua[idx_a-128:idx_a]
        start_ia = ia[idx_a-128:idx_a]

        start_ub =ub[idx_b-128:idx_b]
        start_ib = ib[idx_b-128:idx_b]
        # 对齐波形（从过零点开始截取）
        ua_aligned, ia_aligned = ua[idx_a:], ia[idx_a:]
        ub_aligned, ib_aligned = ub[idx_b:], ib[idx_b:]
        # 截取到相同长度
        length = min(len(ua_aligned), len(ub_aligned))
        ua_aligned, ia_aligned = ua_aligned[:length], ia_aligned[:length]
        # print(ua_aligned[:128])
        ub_aligned, ib_aligned = ub_aligned[:length], ib_aligned[:length]

        ua_aligned_128 = []
        ia_aligned_128 = []
        ub_aligned_128 = []
        ib_aligned_128 = []
        # === 检查每个周期点数 ===
        self.align_waveform_to_128(ua_aligned, ia_aligned, ua_aligned_128, ia_aligned_128, "波形1周期")
        self.align_waveform_to_128(ub_aligned, ib_aligned, ub_aligned_128, ib_aligned_128, "波形2周期")

        # 截取到相同长度
        length = min(len(ua_aligned_128), len(ub_aligned_128))
        ua_aligned_128, ia_aligned_128 = ua_aligned_128[:length], ia_aligned_128[:length]
        ub_aligned_128, ib_aligned_128 = ub_aligned_128[:length], ib_aligned_128[:length]
        #加上起始周波
        ua_aligned_128 =start_ua+ua_aligned_128
        ia_aligned_128 =start_ia+ia_aligned_128
        ub_aligned_128 =start_ub+ub_aligned_128
        ib_aligned_128 =start_ib+ib_aligned_128
        # 电流叠加
        i_sum = []
        for a, b in zip(ia_aligned_128, ib_aligned_128):
            if self.add_or_abstract == 0:
                i_sum.append(a + b)
            else:
                i_sum.append(a - b)
        return ua_aligned_128, i_sum

    def parse_files(self):
        parser = ParseUIData.FrameParser(1, self.format, self.uifactor1)
        parser.endian = "little"
        parser.IsQT = 0
        cnt = 0
        # 1. 初始化Python列表收集数据
        ua1_list, ia1_list, ua2_list, ia2_list = [], [], [], []
        cnt = -1
        with open(self.file1path, 'rb') as f1, open(self.file2path, 'rb') as f2:
            while True:
                cnt += 1
                if self.add_frame_num != 0:
                    if cnt == self.add_frame_num:
                        print(f"已解析完成{cnt}帧")
                        break

                buf1 = f1.read(self.frame_size)
                buf2 = f2.read(self.frame_size)

                # 任一文件结束则跳出
                if not buf1 or not buf2:
                    break

                parser.UIfactor = self.uifactor1
                # 解析文件1
                if parser.parse_single_frame(buf1, 0) == -1:
                    print("文件1解析失败")
                    return None, None, None, None
                 # 连续三次大于200置启动标志，启动后连续三次小于100则break
                if not self.rmsu1_started:
                    if parser.rms_U > 200:
                        self.rmsu1_above_200_count += 1
                    else:
                        self.rmsu1_above_200_count = 0
                    if self.rmsu1_above_200_count >= 3:
                        self.rmsu1_started = True
                        self.rmsu1_below_100_count = 0  # 启动后清零
                else:
                    if parser.rms_U < 100:
                        self.rmsu1_below_100_count += 1
                    else:
                        self.rmsu1_below_100_count = 0
                    if self.rmsu1_below_100_count >= 3:
                        break
                if self.rmsu1_started:
                    factor = parser.rms_U / 220
                else:
                    factor =1
                parser.voltage = [v / factor for v in parser.voltage]
                parser.current = [i / factor for i in parser.current]
                ua1_list.append(parser.voltage.copy())
                ia1_list.append(parser.current.copy())

                parser.UIfactor = self.uifactor2
                # 解析文件2
                if parser.parse_single_frame(buf2, 0) == -1:
                    print("文件2解析失败")
                    return None, None, None, None
                # 连续三次大于200置启动标志，启动后连续三次电压有效值小于100则break

                if not self.rmsu2_started:
                    if parser.rms_U > 200:
                        self.rmsu2_above_200_count += 1
                    else:
                        self.rmsu2_above_200_count = 0
                    if self.rmsu2_above_200_count >= 3:
                        self.rmsu2_started = True
                        self.rmsu2_below_100_count = 0  # 启动后清零
                else:
                    if parser.rms_U < 100:
                        self.rmsu2_below_100_count += 1
                    else:
                        self.rmsu2_below_100_count = 0
                    if self.rmsu2_below_100_count >= 3:
                        break
                # print(parser.rms_U)
                # 只有在之前大于200的情况下，连续三次小于100才break
                if self.last_rmsu2 > 200:
                    if parser.rms_U < 100:
                        self.rmsu2_below_100_count += 1
                    else:
                        self.rmsu2_below_100_count = 0
                    if self.rmsu2_below_100_count >= 3:
                        break
                self.last_rmsu2 = parser.rms_U
                ua2_list.append(parser.voltage.copy())
                if any(abs(x) > self.peak_value for x in parser.current) and self.add_or_abstract == 1: #电动车相减时去掉启动时的尖峰
                    continue
                else:
                    ia2_list.append(parser.current.copy())

        # 2. 循环后转为一维list
        print("所有帧解析完成，正在转换为NumPy数组...")
        ua1 = list(itertools.chain.from_iterable(ua1_list))
        ia1 = list(itertools.chain.from_iterable(ia1_list))
        ua2 = list(itertools.chain.from_iterable(ua2_list))
        ia2 = list(itertools.chain.from_iterable(ia2_list))
        print("转换完成！")

        return ua1, ia1, ua2, ia2
    def ensure_output_folder_and_unique_file(self,folder, filename):
        """
        确保文件夹存在，并在其中生成唯一文件名
        """
        os.makedirs(folder, exist_ok=True)
        name, ext = os.path.splitext(filename)
        counter = 0
        new_filename = filename
        full_path = os.path.join(folder, new_filename)
        while os.path.exists(full_path):
            counter += 1
            new_filename = f"{name}_{counter}{ext}"
            full_path = os.path.join(folder, new_filename)
        return full_path
    def process_and_save(self, output_file="addedData.bin"):
        formatConvert = FormatConvert.FormatConverter()
        # 解析两个文件，获取各自的电压、电流数据
        ua1, ia1, ua2, ia2 = self.parse_files()
        if ua1 is not None and ua2 is not None and ia1 is not None and ia2 is not None:
            # 对齐并相加两个文件的电流数据
            U, sumI = self.align_and_add(ua1, ia1, ua2, ia2)
            U = U[:len(U)//128*128]
            sumI = sumI[:len(sumI)//128*128]
            # 再取两者中较短的长度，保持一致
            min_len = min(len(U), len(sumI))
            U = U[:min_len]
            sumI = sumI[:min_len]
            print(f"U长度: {len(U)}, sumI长度: {len(sumI)}")

            # 优化大数据操作性能，使用NumPy进行批量处理和拼接，避免循环和多次extend
            # 将电流和电压数据转换为NumPy数组
            sumI_np = np.asarray(sumI, dtype=np.float64)
            U_np = np.asarray(U, dtype=np.float64)

            # 使用FormatConverter的to_signed_24bit_little_endian方法转换为字节流
            byte_I_data = formatConvert.to_signed_24bit_little_endian(sumI_np, 4)
            byte_U_data = formatConvert.to_signed_24bit_little_endian(U_np, 4)

            # 将字节流转换为NumPy数组并reshape为(N, 3)
            arr_I = np.frombuffer(byte_I_data, dtype=np.uint8).reshape(-1, 3)
            arr_U = np.frombuffer(byte_U_data, dtype=np.uint8).reshape(-1, 3)

            # 拼接U和I的3字节，形成(N, 6)的数组，然后再展平成一维
            interleaved = np.empty((arr_I.shape[0], 6), dtype=np.uint8)
            interleaved[:, 0:3] = arr_U
            interleaved[:, 3:6] = arr_I
            byte_data = interleaved.reshape(-1)

            # 截断字节数组，使其长度为768的整数倍（每帧768字节）
            byte_data = byte_data[:len(byte_data)//768*768]

            # 如果输出文件已存在，先删除
            if os.path.exists(output_file):
                os.remove(output_file)

            # 保存最终数据到二进制文件
            print(f"byte_data长度: {len(byte_data)}")
            formatConvert.save_data_as_bin(output_file, byte_data, 778)
            print(f"数据已保存到 {output_file}")
        else:
            print("数据解析失败，未生成输出文件。")

if __name__ == "__main__":
    #如果电动车起始尖峰所在的周波没有负到正的过零点，该周波会被跳过，目前起始点判断为负到正过零点
    # file1path = "D:\\work\\project\\WaveParse\\wlb_waveRecord_output\\联想计算机运行（10分钟）.bin"
    # file2path = "D:\\work\\project\\WaveParse\\pre_zeroI_data_01_01_wlb_waveRecord.bin"
    # file1path = "D:\\WorkProjectsSVN\\WaveParse\\wlb_waveRecord_output\\03_01_wlb_waveRecord.bin"
    # file2path = "D:\\WorkProjectsSVN\\WaveParse\\wlb_waveRecord_output\\03_01_wlb_waveRecord.bin"
    add_or_abstract = 0 #0波形相加，1波形相减（file1减file2）
    peak_value = 10 #电动车起始尖峰阈值,波形相减时去掉启动时的尖峰，波形相加不用管,file2使用电动车
    file1_Dir_or_path = r"D:\work\project\WaveParse\PreZeroData\150s_zeroI_data_12-1P_wlb_waveRecord.BIN"
    # file1path = r"C:\Users\宁浩\Desktop\addWave\电热毯一档（1分钟），电热毯一档+热得快运行（1-2分钟），电热毯二档+热得快运行（2-3分钟）.BIN"
    file2path = r"D:\WorkProjectsSVN\WaveParse\endZeroData\100s_zeroI_data_120s_pre_zeroI_data_03_01_wlb_waveRecord.bin"
    # file1path = "D:\\work\\project\\WaveParse\\school_format_without_No_output\\A_phase_school_without_NO_wlb_waveRecord_1.bin"
    # file2path = "D:\\work\\project\\WaveParse\\school_format_without_No_output\\A_phase_school_without_NO_wlb_waveRecord_1.bin"
    # uifactor2 =  (0.0001, 0.0001, 0.0001, 0.0001, 0.0001, 0.0001)
    # uifactor1 = (356983488e-12, 0.000265, 0.000265,57135464e-12, 0.000247, 0.000247)
    # uifactor2 = (0.0002674399, 0.000265, 0.000265,0.0002483747, 0.000247, 0.000247)
    # uifactor1 = (0.0003742375, 0.0003742375, 0.0003742375, 0.00001014207, 0.00001014207, 0.00001014207)
    uifactor1 = (0.0002673968, 0.0003742375, 0.0003742375, -0.0002514363, 0.00001014207, 0.00001014207)
    # uifactor1 = (0.0001, 0.0001, 0.0001, 0.0001, 0.0001, 0.0001)
    # uifactor2 =  (0.0001, 0.0001, 0.0001, 0.0001, 0.0001, 0.0001)
    # uifactor1 = (0.0003742375, 0.0003742375, 0.0003742375, 0.00001014207, 0.00001014207, 0.00001014207)
    uifactor2 = (0.0003742375, 0.0003742375, 0.0003742375, 0.00001014207, 0.00001014207, 0.00001014207)

    if os.path.isfile(file1_Dir_or_path):
        # 输入的是单个文件
        bin_files = [file1_Dir_or_path]
    elif os.path.isdir(file1_Dir_or_path):
        # 输入的是目录，遍历该目录下所有bin文件
        bin_files = glob.glob(os.path.join(file1_Dir_or_path, "*.bin"))
    # 输出目录（在当前脚本目录下 addedData 文件夹）
    current_dir = os.path.dirname(os.path.abspath(__file__))
    output_dir = os.path.join(current_dir, "addedData")
    # 循环处理所有 bin 文件
    for file1path in bin_files:
        print(f"正在处理: {file1path}")

        # 创建叠加器
        adder = WlbWaveRecordWaveAdder(
            file1path,
            file2path,
            uifactor1=uifactor1,
            uifactor2=uifactor2,
            add_frame_num=0,
            peak_value=peak_value,
            add_or_abstract=add_or_abstract
        )

        # 自动生成唯一输出文件名
        base_name = os.path.basename(file1path)
        output_name = f"addedData_{os.path.splitext(base_name)[0]}.bin"
        filepath = adder.ensure_output_folder_and_unique_file(output_dir, output_name)

        # 叠加并保存
        output_file = os.path.join(output_dir, filepath)
        adder.process_and_save(output_file)

        print(f"输出文件: {output_file}\n")

    print("全部处理完成！")