# -*- coding: gbk -*-
import csv
import struct

import matplotlib.pyplot as plt
import numpy as np
from PyQt5.QtCore import pyqtSignal, QObject
import os
from datetime import datetime, timedelta
import math
from scipy.signal import hilbert

from matplotlib.testing.compare import calculate_rms
from numpy.ma.extras import average

fcstab =[
		0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
		0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
		0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
		0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
		0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
		0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
		0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
		0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
		0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
		0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
		0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
		0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
		0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
		0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
		0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
		0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
		0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
		0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
		0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
		0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
		0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
		0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
		0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
		0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
		0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
		0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
		0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
		0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
		0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
		0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
		0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
		0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78]
# 波形帧结构常量
# FRAME_SIZE = 782  # 一帧总大小
# FRAME_SIZE = 815


PLOT_TIME = 30  # 解析数据最大时长 ??分钟
SAMPLE_NUM = 128 #周波采样数
MAX_ROW_OF_CSV = 600000 #3的倍数兼容三相
print_progress_cnt = 0  # 打印计数



def fcs16(data: bytes) :
    fcs = 0xffff
    for byte in data:
        fcs = (fcs >> 8) ^ fcstab[(fcs ^ byte) & 0xFF]
    fcs ^= 0xFFFF  # complement
    return fcs

def ScaleData(data, k):
    """缩放数据"""
    data = [x * k for x in data]
    return data


def reverse_bytes_to_float(bytes_,endian):
    """字节序转为float"""
    tmp = bytes_[:3]  # 取前三个字节
    # 将字节序列转换为整数
    if endian =="little":
        tmp_int = int.from_bytes(tmp, byteorder='little', signed=True)
    elif endian == "big":
        tmp_int = int.from_bytes(tmp, byteorder='big', signed=True)
    # 处理符号位扩展
    if tmp_int & 0x800000:  # 检查符号位（最高位）
        tmp_int |= 0xFF800000  # 将高位填充为符号扩展
    # 转换为浮点数
    return float(tmp_int)


# 波形数据解析类
class FrameParser(QObject):
    PrintSignal = pyqtSignal(str)  # 信号类型 str
    """波形数据帧解析类，负责解析波形数据帧"""
    """type:0 有序号的物联表录波格式 1 698格式 2:物联表录波格式
       phase：相数，1：单相，3：三相 """
    def __init__(self, phase, type,uiFactor=(0.0003742375, 0.0003742375, 0.0003742375, 0.00001014207, 0.00001014207, 0.00001014207)):#录波工装0.000265, 0.000265, 0.000265, 0.000247, 0.000247, 0.000247
        super().__init__()  # 调用父类构造函数
        """初始化帧解析器"""
        self.phase_cnt = 0 #0 1 2三相循环
        self.endian = "little"#默认大端  （big）（little）
        self.already_print_first_seq = 0
        #s级数据相关
        self.lastSecond = -1
        self.IsQT = 1 #控制打印信息，外部模块调用置0可向终端打印信息
        self.cur_sec = -1
        self.isLastFrame = 0
        self.U_in_one_sec =[]#一秒内的rms电压
        self.I_in_one_sec = []
        self.P_in_one_sec = []
        self.U_sec_level =[]#秒级数据，一秒一个点
        self.I_sec_level = []
        self.P_sec_level = []
        self.FirstSecond = -1
        self.FirstSecondFrameCnt = 0
        self.single_wave_cnt = -1  # 单个波形计数
        self.ParsedFrameNum = 0  # 解析帧序号，从0开始计数
        self.phase_num = phase
        self.UIfactor = uiFactor  # Ua Ub Uc Ia Ib Ic 系数
        self.voltage = np.zeros(128, dtype=np.float64)  # 128 个 0.0，数据类型为 float64
        self.current = np.zeros(128, dtype=np.float64)
        self.rms_Umax = 0
        self.rms_Imax = 0
        self.prev_time = datetime.now()  # 统计耗时用
        self.rms_U = 0 #支持单相
        self.rms_I = 0 #支持单相
        self.ActPower = 0
        self.ReactivePwr = 0
        self.AppPwr = 0
        self.previous_sequence_number = -1
        self.type = type
        if type == 0:
            self.frame_size = 782
        elif type == 1:
            self.frame_size = 815
        elif type == 2:
            self.frame_size = 778

    def print(self,str):
        # print(str)
        if self.IsQT:
            self.PrintSignal.emit(str)
        else:
            print(str)

    def parse_and_print_sec_to_time(self, seconds):
        # 获取当前年份
        current_year = datetime.now().year
        # 构造年初时间（1月1日 00:00:00）
        start_of_year = datetime(current_year, 1, 1, 0, 0, 0)
        # 加上秒数
        target_time = start_of_year + timedelta(seconds=seconds)
        self.print(target_time.strftime("%Y-%m-%d %H:%M:%S"))

    # 计算一个周波有功功率
    def calculate_ActPower(self,U_data,I_data):
        sumdata = 0
        for u,i in zip(U_data,I_data):
            sumdata += u*i
        return sumdata / SAMPLE_NUM
    #视在功率
    def calculate_ApparentPower(self, U_data, I_data):
        u_sq = sum([u ** 2 for u in U_data])
        i_sq = sum([i ** 2 for i in I_data])
        u_rms = math.sqrt(u_sq / SAMPLE_NUM)
        i_rms = math.sqrt(i_sq / SAMPLE_NUM)
        return u_rms * i_rms


    def calculate_reactive_power(self,u, i, f=50):
        if len(u) != len(i):
            raise ValueError("电压和电流长度不一致")

        N = len(u)

        # 获取电流的正交分量（与电压相位差90°）
        i_analytic = hilbert(i)
        i_quadrature = np.imag(i_analytic)  # 正交分量（90°延迟）

        # 瞬时无功功率：u[n] * i_90[n]
        q_inst = u * i_quadrature

        # 平均无功功率
        Q = np.mean(q_inst)

        return abs(Q)
    #计算有效值
    def calculate_rms(self,input_data):
        sumdata = 0.0
        for value in input_data:
            sumdata += value * value

        sample_num = len(input_data)  # 相当于 C 代码中的 len 或 SAMPLE_NUM
        rms = math.sqrt(sumdata / sample_num)
        return rms

    def scale_single_phase_data(self, phase):
        """传入指定相位及电压电流数据进行缩放"""
        # 分别获取电压和电流的缩放因子
        voltage_factors = self.UIfactor[:3]  # A, B, C 相电压的缩放因子
        current_factors = self.UIfactor[3:]  # A, B, C 相电流的缩放因子
        # 缩放电压和电流数据
        self.voltage = ScaleData(self.voltage, voltage_factors[phase])
        self.current = ScaleData(self.current, current_factors[phase])
    def check_frame_foramt_698(self, buffer,IsWlbWithNo):
        """检查帧格式 -1为异常，0为正常"""
        if len(buffer) != self.frame_size:
            self.print("帧长度错误！")
            return -1
        # 检查帧开始和结束标志
        if buffer[0] != 0x68 or buffer[self.frame_size - 1] != 0x16:
            self.print("起始或结束字节错误！")
            self.print(" ".join(f"{b:02X}" for b in buffer))
            return -1

        # 提取秒字段
        seconds = int.from_bytes(buffer[29:33], byteorder='big')
        self.cur_sec =seconds
        # if seconds != self.lastSecond:
        #     self.lastSecond = seconds

        if self.FirstSecond == -1:
            self.FirstSecond = seconds
            self.FirstSecondFrameCnt += 1
        else:
            if self.FirstSecond == seconds:
                self.FirstSecondFrameCnt += 1
        # 提取序号字段 (此处假设序号在数据部分)
        crc1_calc = fcs16(buffer[1:12])
        crc1_provided = int.from_bytes(buffer[12:14], byteorder='little')
        if crc1_calc != crc1_provided:
            self.print("hcs error")
            return -2
        crc2_calc = fcs16(buffer[1:812])
        crc2_provided = int.from_bytes(buffer[812:814], byteorder='little')
        if crc2_calc != crc2_provided:
            self.print("fcs error")
            return -2
        #帧类型
        frame_type = buffer[25]
        if frame_type == 1:
            self.print("检测到起始帧")
        elif frame_type == 3:
            self.print("检测到结束帧")
            self.isLastFrame = 1
        loop = buffer[27]
        sequence_number = int.from_bytes(buffer[34:38], byteorder='big')
        if self.already_print_first_seq == 0 :
            self.print(f"起始序号：{sequence_number}")
            self.print(f"回路号：{loop}")
            self.print("起始时间：")
            self.parse_and_print_sec_to_time(seconds)
            self.already_print_first_seq = 1
        if frame_type == 3:
            self.print(f"结束序号：{sequence_number}")
            self.print("结束时间：")
            self.parse_and_print_sec_to_time(seconds)
        # 三相数据每三帧检查序号是否连续
        if self.phase_num == 3:
            if self.phase_cnt == 0:  # 每三帧判断一次序号
                if self.previous_sequence_number == -1:
                    self.previous_sequence_number = sequence_number
                else:
                    if sequence_number != self.previous_sequence_number + 1:
                        self.print(f"序号不连续！当前序号：{sequence_number}, 前一个序号：{self.previous_sequence_number},检查相数或文件")
                        self.previous_sequence_number = sequence_number
                        self.parse_and_print_sec_to_time(seconds)
                        return -2
                    else:
                        self.previous_sequence_number = sequence_number
            self.phase_cnt = (self.phase_cnt + 1) % 3
        # 单相处理
        elif self.phase_num == 1:
            if self.previous_sequence_number == -1:
                self.previous_sequence_number = sequence_number
            else:
                if sequence_number != self.previous_sequence_number + 1:
                    self.print(f"警告：序号不连续！当前序号：{sequence_number}, 前一个序号：{self.previous_sequence_number},检查相数或文件")
                    self.previous_sequence_number = sequence_number
                    self.parse_and_print_sec_to_time(seconds)
                    return -2
                else:
                    self.previous_sequence_number = sequence_number

        # 提取毫秒部分，当前假设为全0
        # milliseconds = int.from_bytes(buffer[5 + 128 * 6:7 + 128 * 6], byteorder='big')
        # if milliseconds != 0:
        #     self.print("ms != 0，error")
        #     return -1

        if sequence_number % 1500 == 0:
            if self.phase_cnt == 0:
                self.print(f"format of Frame No:{sequence_number} is correct")
                current_time = datetime.now()
                self.prev_time = current_time
        return 0
    def check_frame_foramt_wlb_waveRecord(self, buffer,IsHasNO):
        """检查帧格式 -1为异常，0为正常"""
        if len(buffer) != self.frame_size:
            self.print("帧长度错误！")
            return -1
        # 检查帧开始和结束标志
        if buffer[0] != 0x68 or buffer[self.frame_size - 1] != 0x16:
            self.print("起始或结束字节错误！")
            self.print(" ".join(f"{b:02X}" for b in buffer))
            return -1

        # 提取秒字段
        seconds = int.from_bytes(buffer[1:5], byteorder='little')
        self.cur_sec = seconds
        if self.FirstSecond == -1:
            self.FirstSecond = seconds
        # 提取序号字段 (此处假设序号在数据部分)
        if IsHasNO:
            sequence_number = int.from_bytes(buffer[775:779], byteorder='little')
            # 三相数据每三帧检查序号是否连续
            if self.phase_num == 3:
                if self.phase_cnt == 0:  # 每三帧判断一次序号
                    if self.previous_sequence_number == -1:
                        self.previous_sequence_number = sequence_number
                    else:
                        if sequence_number != self.previous_sequence_number + 1:
                            self.print(f"警告：序号不连续！当前序号：{sequence_number}, 前一个序号：{self.previous_sequence_number},检查相数或文件")
                            self.parse_and_print_sec_to_time(seconds)
                            self.previous_sequence_number = sequence_number
                            return -2
                        else:
                            self.previous_sequence_number = sequence_number
                self.phase_cnt = (self.phase_cnt + 1) % 3
            # 单相处理
            elif self.phase_num == 1:
                if self.previous_sequence_number == -1:
                    self.previous_sequence_number = sequence_number
                else:
                    if sequence_number != self.previous_sequence_number + 1:
                        self.print(f"警告：序号不连续！当前序号：{sequence_number}, 前一个序号：{self.previous_sequence_number},检查相数或文件")
                        self.previous_sequence_number = sequence_number
                        self.parse_and_print_sec_to_time(seconds)
                        return -2
                    else:
                        self.previous_sequence_number = sequence_number

        # 提取毫秒部分，当前假设为全0
        milliseconds = int.from_bytes(buffer[5 + 128 * 6:7 + 128 * 6], byteorder='big')
        # if milliseconds != 0:
        #     self.print("ms != 0，error")
        #     return -1

        # 提取校验和 (2字节)
        checksum = int.from_bytes(buffer[-3:-1], byteorder='little')
        # 计算校验和（累加前面所有字节，保留低位2字节）
        calculated_checksum = sum(buffer[0:self.frame_size - 3]) & 0xFFFF  # 取低 2 字节作为校验和
        if checksum != 1:#等于1是录波文件问题，校验一直是1 但是数据正常
            if checksum != calculated_checksum:
                self.print(f"校验和错误！计算值: {calculated_checksum}, 读取值: {checksum}")
                return -2

        if IsHasNO:
            if sequence_number % 1500 == 0:
                if self.phase_cnt == 0:
                    self.print(f"format of Frame No:{sequence_number} is correct")
                    current_time = datetime.now()
                    self.prev_time = current_time
        return 0

    #phase 从0开始（0-2）
    def parse_single_frame(self, buffer, phase):
        if self.type == 0:
            return self._parse_frame_common(1,buffer, phase, start_offset=5,
                                            check_func=self.check_frame_foramt_wlb_waveRecord)
        elif self.type == 1:
            return self._parse_frame_common(0,buffer, phase, start_offset=42, check_func=self.check_frame_foramt_698,
                                            is_698=True)
        elif self.type == 2:
            return self._parse_frame_common(0,buffer, phase, start_offset=5,
                                            check_func=self.check_frame_foramt_wlb_waveRecord)
        return -1
#IsWlbWithNO  0 ：物联表录波格式不带序号 1： 带序号
    def _parse_frame_common(self, IsWlbWithNO,buffer, phase, start_offset, check_func, is_698=False):
        if check_func(buffer,IsWlbWithNO) == -1:
            return -1

        for i in range(128):
            start = start_offset + i * 6
            voltage_bytes = buffer[start:start + 3]
            current_bytes = buffer[start + 3:start + 6]
            self.voltage[i] = reverse_bytes_to_float(voltage_bytes,self.endian)
            self.current[i] = reverse_bytes_to_float(current_bytes,self.endian)

        self.scale_single_phase_data(phase)

        if self.phase_num == 1:
            self.rms_U = self.calculate_rms(self.voltage)
            self.rms_I = self.calculate_rms(self.current)
            self.ActPower = self.calculate_ActPower(self.voltage, self.current)
            self.AppPwr = self.calculate_ApparentPower(self.voltage, self.current)
            self.ReactivePwr = self.calculate_reactive_power(self.voltage, self.current)
            # 更新最大值
            self.rms_Umax = max(getattr(self, 'rms_Umax', 0), self.rms_U)
            self.rms_Imax = max(getattr(self, 'rms_Imax', 0), self.rms_I)

            # 如果是698帧格式，还需额外处理秒级数据缓存
            # if is_698:
            self._update_sec_level_buffers()
        return 0

    def _update_sec_level_buffers(self):
        if self.lastSecond == -1:
            self.lastSecond = self.cur_sec
        elif self.lastSecond != self.cur_sec:
            self.U_sec_level.append(np.mean(self.U_in_one_sec))
            self.I_sec_level.append(np.mean(self.I_in_one_sec))
            self.P_sec_level.append(np.mean(self.P_in_one_sec))
            self.U_in_one_sec.clear()
            self.I_in_one_sec.clear()
            self.P_in_one_sec.clear()
            self.lastSecond = self.cur_sec

        self.U_in_one_sec.append(self.rms_U)
        self.I_in_one_sec.append(self.rms_I)
        self.P_in_one_sec.append(self.ActPower)

        if self.isLastFrame == 1:
            self.U_sec_level.append(np.mean(self.U_in_one_sec))
            self.I_sec_level.append(np.mean(self.I_in_one_sec))
            self.P_sec_level.append(np.mean(self.P_in_one_sec))
            self.U_in_one_sec.clear()
            self.I_in_one_sec.clear()
            self.P_in_one_sec.clear()

# 保存为Csv的类
class CsvSaver:
    """保存波形数据到Excel的类"""
    def __init__(self, phase,type, FilePath,uiFactor,endian):
        """初始化ExcelSaver，可传入FrameParser实例"""
        self.phase_num = phase
        self.data_file_path = FilePath
        self.file_name_without_suffix = os.path.splitext(os.path.basename(self.data_file_path))[0]
        self.csv_file_name = None
        # csv相关变量
        self.written_row_cnt = 0
        self.csv_num = 0
        # 使用组合而不是继承
        self.frame_parser = FrameParser(phase,type,uiFactor)
        self.frame_parser.endian = endian
        self.PLOT_READ_CNT = PLOT_TIME * 60 * 50 * self.phase_num  # 读取文件次数
        self.create_new_csv()
    #调用解析器的print发送给ui显示
    def print(self,str):
        self.frame_parser.print(str)
    def create_new_csv(self):
        # 使用 append 方法将数据写入 CSV 文件
        self.csv_file_name = f"{self.file_name_without_suffix}_csv-{self.csv_num + 1}.csv"
        self.print(self.csv_file_name)
        self.csvFile=open(self.csv_file_name,newline='', mode='w')
        self.writer = csv.writer(self.csvFile)

    def write_single_phase_data_to_csv(self):
        """将单相数据写入 CSV 文件，每行包含 128 个电压点和 128 个电流点"""
        # 确保输入数据长度为 128 点
        assert len(self.frame_parser.voltage) == len(self.frame_parser.current) == 128, "电压和电流数据长度必须为 128 点！"
        # 将电压和电流数据合并为一个列表
        row_data = self.frame_parser.voltage + self.frame_parser.current
        self.writer.writerow(row_data)

    def save_csv(self):
        self.csvFile.close()

    def save_process(self):
        """处理Sheet和Excel文件的保存过程"""
        self.written_row_cnt += 1
        # 一个csv写满
        if (self.written_row_cnt == MAX_ROW_OF_CSV) and (
                self.written_row_cnt % self.phase_num == 0):
            self.written_row_cnt = 0
            self.csvFile.close()
            self.create_new_csv()

    def parse_and_save_to_csv(self):

        """解析数据文件并将数据保存到Excel"""
        phase = 0
        try:
            with open(self.data_file_path, "rb") as file:
                while True:
                    # 一帧一帧读取
                    buffer = file.read(self.frame_parser.frame_size)
                    if not buffer:  # 空字节串
                        self.print("读取已到达文件末尾或失败")
                        self.save_csv()
                        return 0
                    if self.PLOT_READ_CNT == self.frame_parser.ParsedFrameNum:
                        self.print(f"{PLOT_TIME}分钟数据解析完成,csv保存结束")
                        self.save_csv()
                        return 0
                    else:
                        if self.frame_parser.parse_single_frame(buffer, phase) == -1:
                            return -1
                        self.write_single_phase_data_to_csv()
                        self.save_process()
                        if self.phase_num == 3:
                            phase = (phase + 1) % 3
                        else:
                            phase = 0
                        self.frame_parser.ParsedFrameNum += 1
        except FileNotFoundError:
            self.print("波形数据文件打开失败")
            return -2


# 绘制波形图的类
class WavePlotter:
    """绘制波形图的类"""

    new_x = [print_progress_cnt for print_progress_cnt in range(SAMPLE_NUM)]
    fig = None

    def __init__(self, phase, type,FilePath,uiFactor,endian):
        """初始化WavePlotter，可传入FrameParser实例"""
        self.phase_num = phase
        self.uiFactor = uiFactor
        self.data_file_path = FilePath
        self.frame_parser = FrameParser(phase,type,self.uiFactor)  # 使用组合
        self.frame_parser.endian = endian
        self.plot_X_data =  None
        self.plot_UA_data = None
        self.plot_UB_data = None
        self.plot_UC_data = None
        self.plot_IA_data = None
        self.plot_IB_data = None
        self.plot_IC_data = None
        self.plot_time = []
        self.plot_UA_rms_data = None #1个周波的urms数据
        self.plot_IA_rms_data = None #1个周波的irms数据
        self.plot_ActPwr_data = None #1个周波的有功功率数据
        self.plot_ReactPwr_data = None #1个周波的无功功率数据
        self.plot_AppPwr_data = None #1个周波的视在功率数据
        self.Cycle_cnt = 0
        self.plot_read_cnt = PLOT_TIME * 60 * 50 * self.phase_num  # 读取文件次数,每次读一帧
        self.init_plot()
    #调用解析器的print发送给ui显示
    def print(self,str):
        self.frame_parser.print(str)
    # 创建坐标和波形数据列表
    def init_plot(self):
        """初始化绘图数据列表"""
        max_FrameNum = self.plot_read_cnt / self.phase_num  # 最大帧序号数量
        self.frame_parser.ParsedFrameNum = 0  # 帧序号，从0开始计数
        max_point = max_FrameNum * SAMPLE_NUM #

        size = os.path.getsize(self.data_file_path)
        if size / self.frame_parser.frame_size /self.phase_num >max_FrameNum: #文件大小超过规定时间大小则采用规定时间的大小，否则使用文件大小创建列表
            points = max_point #x轴点数
        else:
            points = size // self.frame_parser.frame_size /self.phase_num *SAMPLE_NUM

        self.plot_X_data  = np.zeros(int(points))
        self.plot_UA_data = np.zeros(int(points))
        self.plot_UB_data = np.zeros(int(points))
        self.plot_UC_data = np.zeros(int(points))
        self.plot_IA_data = np.zeros(int(points))
        self.plot_IB_data = np.zeros(int(points))
        self.plot_IC_data = np.zeros(int(points))
        self.plot_UA_rms_data = np.zeros(int(points / SAMPLE_NUM))
        self.plot_IA_rms_data = np.zeros(int(points / SAMPLE_NUM))
        self.plot_ActPwr_data = np.zeros(int(points / SAMPLE_NUM))
        self.plot_AppPwr_data = np.zeros(int(points / SAMPLE_NUM))
        self.plot_ReactPwr_data = np.zeros(int(points / SAMPLE_NUM))

    # 填充波形绘制数据
    def fill_plot_data(self):
        """填充用于绘制波形图的数据"""
        self.frame_parser.single_wave_cnt += 1
        start = self.frame_parser.ParsedFrameNum * SAMPLE_NUM
        end = self.frame_parser.ParsedFrameNum * SAMPLE_NUM + SAMPLE_NUM

        new_x = [i + self.frame_parser.single_wave_cnt // self.phase_num * SAMPLE_NUM for i in
                 range(SAMPLE_NUM)]  # 假设 SAMPLE_NUM = 128
        new_x = [i / SAMPLE_NUM * 20 for i in new_x]


        if self.phase_num == 3:
            phase = self.frame_parser.single_wave_cnt % 3

            self.plot_X_data[start:end] = new_x[::]
            if phase == 1:
                self.plot_IB_data[start:end] = self.frame_parser.current[::]
                self.plot_UB_data[start:end] = self.frame_parser.voltage[::]
            elif phase == 2:
                self.plot_IC_data[start:end] = self.frame_parser.current[::]
                self.plot_UC_data[start:end] = self.frame_parser.voltage[::]
                self.frame_parser.ParsedFrameNum += 1
            elif phase == 0:
                self.plot_IA_data[start:end] = self.frame_parser.current[::]
                self.plot_UA_data[start:end] = self.frame_parser.voltage[::]
        elif self.phase_num == 1:
            time = self.frame_parser.FirstSecond
            phase = 0
            new_x = [time + i*0.001 for i in new_x]
            self.plot_X_data[start:end] =new_x[::]
            self.plot_IA_data[start:end] = self.frame_parser.current[::]
            self.plot_UA_data[start:end] = self.frame_parser.voltage[::]
            self.frame_parser.ParsedFrameNum += 1
            #RMS
            self.plot_IA_rms_data[self.Cycle_cnt] = self.frame_parser.rms_I
            self.plot_UA_rms_data[self.Cycle_cnt] = self.frame_parser.rms_U
            self.plot_ActPwr_data[self.Cycle_cnt] = self.frame_parser.ActPower
            self.plot_ReactPwr_data[self.Cycle_cnt] = self.frame_parser.ReactivePwr
            self.plot_AppPwr_data[self.Cycle_cnt] = self.frame_parser.AppPwr
            self.plot_U_sec_level = self.frame_parser.U_sec_level
            self.plot_I_sec_level = self.frame_parser.I_sec_level
            self.plot_P_sec_level = self.frame_parser.P_sec_level
            self.Cycle_cnt = self.Cycle_cnt + 1

    def plot_single_phase_Data(self, voltage, current, phaseNo):
        """绘制单相波形"""
        if phaseNo == 0:
            phase = 'A'
        elif phaseNo == 1:
            phase = 'B'
        elif phaseNo == 2:
            phase = 'C'
        else:
            phase = 'A'
        # 检查长度是否一致
        assert len(current) == len(voltage), "电流和电压数据长度不一致！"
        # 假设 current 和 voltage 是你已有的数据
        x = range(1, len(current) + 1)

        fig, (sub_plot1, sub_plot2) = plt.subplots(1, 2, figsize=(14, 6))

        # 左侧子图显示电压
        sub_plot1.plot(x, voltage, label="Scaled Voltage", color="blue", linestyle="-", marker="o", markersize=4)
        sub_plot1.set_xlabel("Sample Index", fontsize=12)
        sub_plot1.set_ylabel("Voltage", fontsize=12, color="blue")
        sub_plot1.tick_params(axis='y', labelcolor="blue")  # 设置电压 y 轴的颜色
        sub_plot1.grid(True)
        sub_plot1.set_title("Voltage", fontsize=14)

        # 右侧子图显示电流
        sub_plot2.plot(x, current, label="Scaled Current", color="red", linestyle="--", marker="x", markersize=4)
        sub_plot2.set_xlabel("Sample Index", fontsize=12)
        sub_plot2.set_ylabel("Current", fontsize=12, color="red")
        sub_plot2.tick_params(axis='y', labelcolor="red")  # 设置电流 y 轴的颜色
        sub_plot2.grid(True)
        sub_plot2.set_title("Current", fontsize=14)

        # 调整布局，避免标签重叠
        plt.tight_layout()

        # 显示图像
        plt.show(block=True)

    # 返回值 0 正常；  -1格式错误 -2 文件打开失败
    def parse_and_fill_plot_data(self):
        """解析数据文件并绘制波形图"""
        phase = 0
        try:
            with open(self.data_file_path, "rb") as file:
                data = file.read(54)
                if data[0] == 0x68 and data[52] == 0x16  and data[53] == 0x68 and self.frame_parser.type ==1:  # 如果有系数帧提取系数
                    float1_bytes = bytes(data[39:43])
                    float2_bytes = bytes(data[44:48])
                    self.uiFactor[0] = struct.unpack('>f', float1_bytes)[0]
                    self.uiFactor[3] = struct.unpack('>f', float2_bytes)[0]
                    self.print(f"电压系数{self.uiFactor[0]}")
                    self.print(f"电流系数{self.uiFactor[3]}")
                    file.seek(53)
                else:
                    file.seek(0)
                while True:
                    # 一帧一帧读取
                    buffer = file.read(self.frame_parser.frame_size)
                    if self.plot_read_cnt == self.frame_parser.ParsedFrameNum * self.phase_num:
                        self.print(f"{PLOT_TIME}分钟数据解析完成")
                        return 0
                    if not buffer:  # 空字节串
                        self.print("读取已到达文件末尾")
                        return 0
                    else:
                        if self.frame_parser.parse_single_frame(buffer, phase) == -1:
                            return -1
                        self.fill_plot_data()
                        if self.phase_num == 3:
                            phase = (phase + 1) % 3
                        else:
                            phase = 0
        except FileNotFoundError:
            self.print("波形数据文件打开失败")
            return -2


def main():
    """主函数"""
    # 文件路径和相位数
    file_path_3phase = "D:/work/waveRecord/波形数据/2-3P.BIN"  # 三相文件路径
    file_path_1phase = "D:/work/waveRecord/波形数据/4-1P.BIN"  # 单相文件路径

    # 创建 FrameParser 实例 (可以共享或者每个类创建独立的)
    frame_parser_3phase = FrameParser(phase=3)
    # frame_parser_1phase = FrameParser(phase=1)

    # # 使用 ExcelSaver 保存数据到 Excel (三相)
    # excel_saver_3phase = ExcelSaver(phase=3, FilePath=file_path_3phase)
    # excel_saver_3phase.parse_and_save_to_excel()

    # # 使用 WavePlotter 绘制波形图 (三相)
    # plotter_3phase = WavePlotter(phase=3, FilePath=file_path_3phase)
    # plotter_3phase.parse_and_plot_data()

    # 使用 ExcelSaver 保存数据到 Excel (单相)
    excel_saver_1phase = SvcSaver(phase=1, FilePath=file_path_1phase)
    excel_saver_1phase.parse_and_save_to_excel()
    #
    # # 使用 WavePlotter 绘制波形图 (单相)
    plotter_1phase = WavePlotter(phase=1, FilePath=file_path_1phase)
    plotter_1phase.parse_and_fill_plot_data()


if __name__ == "__main__":
    hex_string = """
    68 2D 03 C3 05 02 00 00 00 00 00 00 30 7A 85 01 00 EE EE 44 44 01 02 05 0F 02 0F 00 06 00 25 23 6C 06 00 01 41 E2 0C 82 03 00 
    43 F2 14 51 A1 00 43 F6 14 51 0E 00 43 D8 14 4F B0 00 44 15 14 4E 4F 00 44 09 14 4D 97 00 43 F8 14 4C 26 00 44 1F 14 4A D3 00 
    44 72 14 48 D4 00 44 77 14 46 D4 00 44 10 14 43 29 00 43 EC 14 3A 85 00 42 E4 14 14 11 00 40 FF 13 05 77 00 3E 2A 11 78 1B 00 
    3A FD 10 D8 DD 00 38 4A 10 05 64 00 35 A3 0F 13 0B 00 32 D5 0E 67 87 00 30 AC 0D B1 D7 00 2E 51 0C EA 3E 00 2B A6 0C 15 28 00 
    28 96 0B 28 29 00 25 98 0A 2A 60 00 22 CA 09 33 25 00 1F 43 07 FB 63 00 1B 32 06 45 2B 00 16 22 04 C5 33 00 11 26 03 AC 58 00 
    0D 45 02 9C 4C 00 09 B5 01 82 E5 00 06 66 00 4D 6C 00 02 9D FF 0F 6E FF FE 72 FE 01 B4 FF FB 18 FD 23 34 FF F8 13 FC 5F 86 FF 
    F5 26 FB BB 0A FF F2 FA FB 14 3B FF F0 A9 FA 50 02 FF ED EB F9 6A 9E FF EB 2B F8 6C B0 FF E7 FF F7 67 BC FF E4 D0 F6 65 38 FF 
    E1 8E F5 55 48 FF DD DB F4 41 73 FF DA 30 F3 5A A6 FF D7 29 F2 A5 E1 FF D4 B3 F1 BD 71 FF D1 D0 F0 AF CA FF CE AE EF D2 A2 FF 
    CB 94 EF 3F 40 FF C9 73 EE FF 39 FF C8 77 EE EE 9B FF C8 31 EE BE EA FF C7 0A EE 56 FA FF C5 FF ED CE 0B FF C4 4B ED 3A 88 FF 
    C2 67 EC A8 DA FF C0 A6 EC 1D A0 FF BF 12 EB BC 7B FF BD B3 EB A8 38 FF BC CD EB A8 A4 FF BC C9 EB A9 61 FF BC D0 EB AA 97 FF 
    BC 4D EB AB A9 FF BC 32 EB AC C7 FF BB FE EB AD FD FF BC 3F EB AF 3D FF BB A2 EB B0 47 FF BB 99 EB B1 64 FF BB F9 EB B3 19 FF 
    BB 9B EB B4 41 FF BB ED EB B6 0B FF BC 10 EB B8 37 FF BB 8B EB BB 94 FF BB E5 EB C3 E4 FF BD 3C EB E8 59 FF BF 9E EC EE 01 FF 
    C2 66 EE 79 C7 FF C5 17 EF 19 71 FF C7 B1 EF F2 17 FF CA 7F F0 EC 8C FF CD 79 F1 8E CE FF CF 66 F2 3F 0A FF D1 FE F3 0E 0C FF 
    D4 D6 F3 E3 97 FF D7 AC F4 CF 38 FF DA BB F5 CA E5 FF DD AD F6 C3 F0 FF E0 8C F8 00 86 FF E5 2C F9 B1 F6 FF EA 01 FB 33 D4 FF 
    EE AE FC 55 EC FF F3 0F FD 65 6E FF F6 72 FE 72 9C FF F9 C7 FF 9A 9E FF FD 6D 00 D7 1B 00 01 D6 01 EB 3C 00 05 6A 02 CD C2 00 
    08 41 03 A0 FC 00 0B 0D 04 4F AD 00 0D 53 04 EE 44 00 0F 16 05 AA D4 00 11 CB 06 8C 1E 00 14 C3 07 86 F1 00 17 B1 08 89 5F 00 
    1B 1A 09 8E F6 00 1E B3 0A 9E A7 00 21 8B 0B B1 35 00 25 3B 0C A5 B2 00 28 58 0D 66 0A 00 2B 37 0E 45 64 00 2E 34 0F 46 B6 00 
    30 C6 10 1E 9A 00 34 0A 10 B9 53 00 36 48 11 09 8D 00 37 85 11 1B 6E 00 38 3F 11 40 39 00 38 7F 11 A2 EA 00 3A 00 12 28 D6 00 
    3B 5F 12 BE 5D 00 3D 40 13 50 F6 00 3E FE 13 DC 66 00 40 8F 14 3F BC 00 42 46 14 56 AA 00 43 34 14 56 9D 00 43 1A 14 56 F9 00 
    43 5C 14 55 E4 00 43 BB 00 00 00 00 00 00 FB 1D 16
    """

    data = [
        0x68, 0x2d, 0x03, 0xc3, 0x05, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0x00, 0xbf, 0x37, 0x85, 0x01,
        0x00, 0xee, 0xee, 0x44, 0x44, 0x01, 0x02, 0x05,
        0x0f, 0x02, 0x0f, 0x01, 0x06, 0x00, 0x6d, 0x7c,
        0x1e, 0x06, 0x00, 0x00, 0x03, 0x85, 0x0c, 0x82,
        0x03, 0x00, 0xf1, 0xfc, 0xe7, 0x07, 0xfc, 0x06,
        0xf2, 0x8b, 0xad, 0x08, 0x22, 0x64, 0xf3, 0x22,
        0x88, 0x08, 0x43, 0xeb, 0xf3, 0xc1, 0x70, 0x08,
        0x60, 0xdf, 0xf4, 0x67, 0x33, 0x08, 0x78, 0x70,
        0xf5, 0x14, 0x41, 0x08, 0x8a, 0x68, 0xf5, 0xc8,
        0x32, 0x08, 0x97, 0x04, 0xf6, 0x82, 0x55, 0x08,
        0x9e, 0x9c, 0xf7, 0x42, 0x78, 0x08, 0xa1, 0x1d,
        0xf8, 0x08, 0x21, 0x08, 0x9e, 0x38, 0xf8, 0xd2,
        0xd9, 0x08, 0x95, 0x5e, 0xf9, 0xa1, 0xf4, 0x08,
        0x87, 0x80, 0xfa, 0x74, 0x9e, 0x08, 0x74, 0xfd,
        0xfb, 0x4a, 0xed, 0x08, 0x5d, 0x23, 0xfc, 0x24,
        0x66, 0x08, 0x3f, 0x7d, 0xfd, 0x00, 0x36, 0x08,
        0x1c, 0xbc, 0xfd, 0xdd, 0x8e, 0x07, 0xf5, 0x53,
        0xfe, 0xbc, 0x50, 0x07, 0xc9, 0x47, 0xff, 0x9b,
        0x93, 0x07, 0x98, 0x3e, 0x00, 0x7a, 0xda, 0x07,
        0x62, 0x67, 0x01, 0x5b, 0x02, 0x07, 0x28, 0x3e,
        0x02, 0x3a, 0xbf, 0x06, 0xe9, 0x58, 0x03, 0x17,
        0xb8, 0x06, 0xa5, 0xeb, 0x03, 0xf2, 0xb3, 0x06,
        0x5e, 0xc8, 0x04, 0xcb, 0xc7, 0x06, 0x13, 0xa8,
        0x05, 0xa1, 0x66, 0x05, 0xc4, 0x66, 0x06, 0x73,
        0x88, 0x05, 0x72, 0x3c, 0x07, 0x42, 0x06, 0x05,
        0x1d, 0x08, 0x08, 0x0c, 0x34, 0x04, 0xc3, 0xed,
        0x08, 0xd1, 0x67, 0x04, 0x67, 0xa7, 0x09, 0x90,
        0xad, 0x04, 0x09, 0x00, 0x0a, 0x4a, 0x14, 0x03,
        0xa8, 0x17, 0x0a, 0xfd, 0xc2, 0x03, 0x44, 0x7c,
        0x0b, 0xaa, 0x77, 0x02, 0xde, 0xc9, 0x0c, 0x4f,
        0x4a, 0x02, 0x77, 0xb7, 0x0c, 0xec, 0x60, 0x02,
        0x0f, 0x35, 0x0d, 0x82, 0x98, 0x01, 0xa5, 0x49,
        0x0e, 0x10, 0xde, 0x01, 0x3a, 0x5a, 0x0e, 0x95,
        0x76, 0x00, 0xce, 0x92, 0x0f, 0x10, 0xab, 0x00,
        0x62, 0x02, 0x0f, 0x82, 0xf9, 0xff, 0xf5, 0x37,
        0x0f, 0xec, 0x4a, 0xff, 0x88, 0x9b, 0x10, 0x4c,
        0x18, 0xff, 0x1c, 0x6e, 0x10, 0xa0, 0xdb, 0xfe,
        0xb0, 0xbf, 0x10, 0xeb, 0x14, 0xfe, 0x46, 0x29,
        0x11, 0x2d, 0x80, 0xfd, 0xdc, 0xbb, 0x11, 0x62,
        0x0f, 0xfd, 0x74, 0x0e, 0x11, 0x80, 0x47, 0xfd,
        0x0d, 0x01, 0x11, 0x90, 0x3b, 0xfc, 0xa8, 0x17,
        0x11, 0x9f, 0x71, 0xfc, 0x45, 0x32, 0x11, 0xad,
        0x39, 0xfb, 0xe4, 0x7c, 0x11, 0xb3, 0xad, 0xfb,
        0x86, 0x7a, 0x11, 0xb1, 0xe8, 0xfb, 0x2a, 0xfc,
        0x11, 0xa6, 0x60, 0xfa, 0xd2, 0x54, 0x11, 0x96,
        0x8a, 0xfa, 0x7d, 0x0d, 0x11, 0x77, 0xbe, 0xfa,
        0x2b, 0x86, 0x11, 0x31, 0xdd, 0xf9, 0xdd, 0x4c,
        0x10, 0xdf, 0xa3, 0xf9, 0x92, 0x7a, 0x10, 0x96,
        0x45, 0xf9, 0x4b, 0xc7, 0x10, 0x43, 0x91, 0xf9,
        0x09, 0xb4, 0x0f, 0xe0, 0x46, 0xf8, 0xcb, 0xee,
        0x0f, 0x77, 0xd2, 0xf8, 0x92, 0x70, 0x0f, 0x03,
        0x90, 0xf8, 0x5d, 0x32, 0x0e, 0x86, 0xab, 0xf8,
        0x2c, 0x97, 0x0e, 0x00, 0xd6, 0xf8, 0x00, 0xd0,
        0x0d, 0x72, 0x2e, 0xf7, 0xda, 0x74, 0x0c, 0xdb,
        0xc6, 0xf7, 0xb9, 0x26, 0x0c, 0x3d, 0x35, 0xf7,
        0x9c, 0x94, 0x0b, 0x96, 0xef, 0xf7, 0x84, 0xfa,
        0x0a, 0xe9, 0xa3, 0xf7, 0x72, 0x92, 0x0a, 0x35,
        0xdf, 0xf7, 0x65, 0x89, 0x09, 0x7b, 0xd3, 0xf7,
        0x5e, 0x52, 0x08, 0xbb, 0xe8, 0xf7, 0x5c, 0x36,
        0x07, 0xf6, 0x34, 0xf7, 0x5f, 0x0a, 0x07, 0x2b,
        0x4b, 0xf7, 0x67, 0xd8, 0x06, 0x5c, 0x50, 0xf7,
        0x76, 0x40, 0x05, 0x89, 0xb3, 0xf7, 0x89, 0x2a,
        0x04, 0xb3, 0x6c, 0xf7, 0xa1, 0x2a, 0x03, 0xda,
        0x4b, 0xf7, 0xbe, 0xb1, 0x02, 0xfe, 0x99, 0xf7,
        0xe0, 0xed, 0x02, 0x21, 0x69, 0xf8, 0x08, 0x42,
        0x01, 0x42, 0xca, 0xf8, 0x34, 0xd2, 0x00, 0x63,
        0x09, 0xf8, 0x65, 0xf0, 0xff, 0x83, 0x01, 0xf8,
        0x9b, 0x8c, 0xfe, 0xa3, 0x6d, 0xf8, 0xd5, 0xfa,
        0xfd, 0xc4, 0x6c, 0xf9, 0x15, 0x27, 0xfc, 0xe7,
        0x21, 0xf9, 0x58, 0x79, 0xfc, 0x0b, 0xd9, 0xf9,
        0x9f, 0x6d, 0xfb, 0x32, 0x6b, 0xf9, 0xea, 0x5d,
        0xfa, 0x5c, 0x43, 0xfa, 0x39, 0x35, 0xf9, 0x8a,
        0x22, 0xfa, 0x8b, 0x6e, 0xf8, 0xbb, 0xcb, 0xfa,
        0xe1, 0x6d, 0xf7, 0xf2, 0x1b, 0xfb, 0x3a, 0xca,
        0xf7, 0x2d, 0x8a, 0xfb, 0x96, 0xa4, 0xf6, 0x6d,
        0x98, 0xfb, 0xf5, 0x3a, 0xf5, 0xb3, 0x80, 0xfc,
        0x56, 0x62, 0xf5, 0x00, 0x41, 0xfc, 0xb9, 0x59,
        0xf4, 0x53, 0xc6, 0xfd, 0x1e, 0x67, 0xf3, 0xae,
        0x80, 0xfd, 0x85, 0xe0, 0xf3, 0x10, 0xd1, 0xfd,
        0xee, 0xc7, 0xf2, 0x7b, 0x22, 0xfe, 0x58, 0x80,
        0xf1, 0xed, 0xb5, 0xfe, 0xc3, 0x77, 0xf1, 0x68,
        0xe2, 0xff, 0x2f, 0x53, 0xf0, 0xed, 0x41, 0xff,
        0x9b, 0xa7, 0xf0, 0x7a, 0xb6, 0x00, 0x07, 0xf4,
        0xf0, 0x11, 0xaf, 0x00, 0x74, 0x63, 0xef, 0xb2,
        0xa9, 0x00, 0xe1, 0x22, 0xef, 0x5d, 0x9b, 0x01,
        0x4d, 0x2d, 0xef, 0x13, 0x1e, 0x01, 0xb7, 0x4e,
        0xee, 0xd1, 0xa7, 0x02, 0x20, 0x8f, 0xee, 0x9d,
        0x08, 0x02, 0x89, 0x35, 0xee, 0x7e, 0x0a, 0x02,
        0xf0, 0x43, 0xee, 0x6d, 0xc7, 0x03, 0x55, 0x8b,
        0xee, 0x5e, 0x69, 0x03, 0xb8, 0xa3, 0xee, 0x51,
        0x00, 0x04, 0x19, 0x3c, 0xee, 0x4b, 0x7f, 0x04,
        0x77, 0x39, 0xee, 0x4d, 0x9a, 0x04, 0xd2, 0xe4,
        0xee, 0x58, 0x96, 0x05, 0x2b, 0xa6, 0xee, 0x68,
        0x06, 0x05, 0x80, 0xe9, 0xee, 0x87, 0x11, 0x05,
        0xd2, 0xa2, 0xee, 0xcd, 0x28, 0x06, 0x20, 0xf4,
        0xef, 0x1f, 0xd6, 0x06, 0x6b, 0x85, 0xef, 0x6a,
        0x25, 0x06, 0xb2, 0x3e, 0xef, 0xbd, 0x7f, 0x06,
        0xf4, 0xa5, 0xf0, 0x20, 0xb6, 0x07, 0x32, 0x77,
        0xf0, 0x89, 0x14, 0x07, 0x6c, 0x11, 0xf0, 0xfd,
        0x89, 0x07, 0xa1, 0x19, 0xf1, 0x7a, 0xb5, 0x07,
        0xd1, 0x7d, 0x00, 0x00, 0x4b, 0x05, 0x16,
    ]

    # 转换为 Python 列表
    # data_list = [int(byte, 16) for byte in hex_string.split()]
    fp= FrameParser(1,1,(2.6916657222E-4,2.6916657222E-4,2.6916657222E-4,2.53459467E-4,2.53459467E-4,2.53459467E-4))
    # fp.check_frame_foramt_698(data)
    fp.parse_single_frame_698(data,1)

    print(list(map(float, fp.voltage)))
    print(len(fp.voltage))
    print(max(fp.voltage))
    print(list(map(float, fp.current)))
    print(len(fp.current))
    print(max(fp.current))

    wplt = WavePlotter(1,1,"1-1P.BIN",(0.00026,0.00026,0.00026,0.00026,0.00026,0.00026))
    wplt.parse_and_fill_plot_data()


