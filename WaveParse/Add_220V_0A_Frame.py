# -*- coding: gbk -*-
import random
import math
import struct
from AddWaveToWlbWaverecord_1p import WlbWaveRecordWaveAdder
import os
from ParseUIData import FrameParser
class ZeroWaveformGenerator:
    def __init__(self, u_factor, i_factor, chunk_size):
        """
        :param u_factor: 电压缩放因子
        :param i_factor: 电流缩放因子
        :param chunk_size: 每帧字节数，默认 778
        """
        self.u_factor = u_factor
        self.i_factor = i_factor
        self.chunk_size = chunk_size

    def float_to_reverse_bytes(self, value: float, endian: str = "little") -> bytes:
        """
        浮点数转为 3 字节逆变换（与 reverse_bytes_to_float 配套）
        value: 输入的 float 类型
        endian: 'little' 或 'big'
        返回: 3 字节 bytes
        """
        # 转为整数
        int_val = int(round(value))
        if int_val < 0:
            int_val = (1 << 24) + int_val  # 补码表示
        b = int_val.to_bytes(3, byteorder=endian, signed=False)
        return b
    #choose:0 零漂电流 1 输入电流
    def generate_zero_frame(self,choose,i_data):
        """生成一个包含 128 点电压 + 零漂电流的帧,choose:0 使用零漂电流 1 使用输入电流"""
        voltage = []
        zero_current = []
        for i in range(128):
            # 电压噪声：-0.5V ~ 0.5V
            voltage_noise = random.uniform(-0.5, 0.5)
            voltage.append(220 * math.sqrt(2) * math.sin(2 * math.pi * i / 128) + voltage_noise)
            # 电流噪声：-0.01A ~ 0.01A
            if choose == 0:
                zero_current.append(random.uniform(-0.01, 0.01))
            else:
                zero_current.append(i_data[i])

        voltage_bytes = []
        zero_current_bytes = []
        zero_data = []

        for j in range(128):
            voltage_bytes.extend(self.float_to_reverse_bytes(voltage[j] / self.u_factor, "little"))
            zero_current_bytes.extend(self.float_to_reverse_bytes(zero_current[j] / self.i_factor, "little"))

        # 交织排列：电压 3 字节 + 电流 3 字节
        for k in range(128):
            zero_data.extend(voltage_bytes[k * 3:k * 3 + 3])
            zero_data.extend(zero_current_bytes[k * 3:k * 3 + 3])

        # 构建帧：起始 0x68 + 填充 + 数据 + 填充 + 结束 0x16
        zero_frame = bytes([0x68] + [0x00] * 4 + zero_data + [0x00] * 4 + [0x16])

        # 校验
        checksum = sum(zero_frame[0:self.chunk_size - 3]) & 0xFFFF
        zero_frame = bytearray(zero_frame)
        zero_frame[self.chunk_size - 3:self.chunk_size - 1] = checksum.to_bytes(2, "little")

        return bytes(zero_frame)

    def add_zero_frames(self, input_file, output_file, uifactor,mode,zero_cycle_nums):
        """
        在文件前加 zero_cycle_nums 个前置帧
        :param input_file: 原始 bin 文件
        :param output_file: 输出 bin 文件
        :param zero_cycle_nums: 前置帧数量
        """
        zero_frames = [self.generate_zero_frame(0,[]) for _ in range(zero_cycle_nums)]
        start_frame = []
        left_original_data = []
        parser = FrameParser(1,2,uifactor)
        with open(input_file, 'rb') as f1:
            while True:
                buf1 = f1.read(parser.frame_size)
                # 文件结束则跳出
                if not buf1:
                    break
                parser.parse_single_frame(buf1, 0)
                if parser.rms_U > 100:#检测到起始波形
                    start_frame = self.generate_zero_frame(1,parser.current) #电压拓展至一个周波，电流保持原数据
                    left_original_data = f1.read()
                    break            
        with open(output_file, "wb") as f:
            if mode == "preZero":
                for frame in zero_frames:
                    f.write(frame)
                f.write(start_frame)
                f.write(left_original_data)
            elif mode == "endZero":
                f.write(start_frame)
                f.write(left_original_data)
                for frame in zero_frames:
                    f.write(frame)
        print(f"已生成 {zero_cycle_nums} 帧，并写入 {output_file}")


if __name__ == "__main__":
    for i in range(1):
        ui_factor = (0.0003742375, 0.0003742375, 0.0003742375, 0.00001014207, 0.00001014207, 0.00001014207)#三相电压电流系数，电动自行车默认
        # # ui_factor = (0.0002672848, 0.0002672848, 0.0002672848, 0.0002486532, 0.0002486532, 0.0002486532)#三相电压电流系数，录波工装默认
        zeroI_sec = 100 * (i + 1) #0电流秒数
        mode = "endZero" #"endZero" "preZero" 波形起始加0电流还是波形结束后加0电流
        # input_bin_path = "D:\\WorkProjectsSVN\\WaveParse\\wlb_waveRecord_output\\04_01_wlb_waveRecord.bin"
        # input_bin_path = "D:\\work\\project\\WaveParse\\wlb_waveRecord_output\\01_01_wlb_waveRecord.bin"
        input_bin_path = r"D:\WorkProjectsSVN\WaveParse\PreZeroData\电动车前0数据\120s_pre_zeroI_data_03_01_wlb_waveRecord.bin"


        generatr = ZeroWaveformGenerator(ui_factor[0], ui_factor[3], chunk_size=778)
        # 提取文件名（不带目录）
        filename = os.path.basename(input_bin_path)  # "03_01(1)_wlb_waveRecord_1.bin"
        if mode == "preZero":
            out_dir = "PreZeroData"
        elif mode == "endZero":
            out_dir = "endZeroData"
        if not os.path.exists(out_dir):
            os.makedirs(out_dir)
        out_filename = os.path.join(out_dir, str(zeroI_sec)+"s_zeroI_data_" + filename)  # "PreZeroData/pre_zeroI_data_03_01(1)_wlb_waveRecord_1.bin"
        generatr.add_zero_frames(input_bin_path, out_filename,ui_factor, mode,zero_cycle_nums=zeroI_sec*50)
