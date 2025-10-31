#!/usr/bin/python
# -*- coding: gbk -*-
#����������¼����ʽ���β�תΪ������¼����ʽ,֧�ֵ��ࡢ������¼������ź�ԭʼ����Ÿ�ʽ,ת����һ���ܲ�128��
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
    def __init__(self, file1path, file2path, frame_size=778, uifactor1=(0.0003742375, 0, 0, 0.00001014207, 0, 0), uifactor2=(0.0003742375, 0, 0, 0.00001014207, 0, 0), add_frame_num=0, format=2,peak_value=0):
        # �ļ�1·��
        self.file1path = file1path
        # �ļ�2·��
        self.file2path = file2path
        # ÿ֡�ֽ�����Ĭ��Ϊ778��������¼����ʽ����782������ŵ�������¼����ʽ��
        self.frame_size = frame_size
        # �ļ�1��UIϵ������ѹ�����������ӣ�
        self.uifactor1 = uifactor1
        # �ļ�2��UIϵ������ѹ�����������ӣ�
        self.uifactor2 = uifactor2
        # ���ӵ�֡����0��ʾȫ������
        self.add_frame_num = add_frame_num
        # �����ļ���ʽ��2��ʾ������¼����ʽ
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
    # ��ֵ�㷨������data���������⣩���������128���6.4k��������
    def interpolate_to_128(self, data):
        """
        ���������� data ��ֵ/��ֵ������ 128��
        """
        def lcm(a, b):
            return a * b // math.gcd(a, b)

        arr = list(data)
        n = len(arr)

        # ֱ�����Բ�ֵ��128�㣨�ȼ�������չ��lcm�ٳ���������Ч��
        result = [0.0] * 128
        for k in range(128):
            t = k / 127               # ���λ�� [0,1]
            pos = t * (n - 1)         # ��ԭ�����ϵĸ�������
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
            out.extend(group[::-1])  # ����׷��
        return bytes(out)
    def find_all_zero_cross(self,u):
        """�ҵ����и��������������"""
        idxs = [0]#��һ�����Ѿ��ǹ������
        flag = 0
        for i in range(len(u) - 1):
            if  u[i] <=0 and u[i+1] > 0  and u[i+1] -u[i]  > 2:
                idxs.append(i + 1)
        return idxs
    def align_waveform_to_128(self,voltage, current, voltage_aligned_128, current_aligned_128, name="����"):
        zero_crossings = self.find_all_zero_cross(voltage)
        for i in range(len(zero_crossings) - 1):
            cycle_len = zero_crossings[i + 1] - zero_crossings[i]
            start, end = zero_crossings[i], zero_crossings[i + 1]

            if cycle_len < 128:
                if cycle_len < 126:
                    print(f"{name}{i + 1}: {cycle_len} �㣬index��{end}��{start}")
                v_cycle = self.interpolate_to_128(voltage[start:end])
                c_cycle = self.interpolate_to_128(current[start:end])
            elif cycle_len > 128:
                if cycle_len > 130:
                    print(f"{name}{i + 1}: {cycle_len} �㣬index��{end}��{start}")
                v_cycle = self.interpolate_to_128(voltage[start:end])
                c_cycle = self.interpolate_to_128(current[start:end])
            else:  # ����128��
                v_cycle = voltage[start:end]
                c_cycle = current[start:end]

            # д��Ŀ������
            voltage_aligned_128[i * 128:(i + 1) * 128] = v_cycle
            current_aligned_128[i * 128:(i + 1) * 128] = c_cycle

    def find_zero_cross(self,u):
        UstartFlag = 0
        """�ҵ���һ���������Ĺ��������"""
        for i in range(len(u) - 1):
            if UstartFlag == 0 and abs(u[i]) > 60:
                UstartFlag = 1
            if UstartFlag == 1 and u[i] <= 0 and u[i + 1] > 0 and u[i + 1] - u[i] > 2 and u[i + 2] - u[i + 1] > 2:
                return i + 1  # ȡ�����֮���Ǹ���
        return 0  # ���û�ҵ��ʹ�ͷ

    def align_and_add(self,ua, ia, ub, ib):
        """
        �������������ĵ�ѹ�������Σ�������
        ua, ia: �豸 A �ĵ�ѹ�������� (numpy array)
        ub, ib: �豸 B �ĵ�ѹ�������� (numpy array)
        """

        # �ҵ���ѹ���ζ����
        idx_a = self.find_zero_cross(ua)
        idx_b = self.find_zero_cross(ub)

        start_ua =ua[idx_a-128:idx_a]
        start_ia = ia[idx_a-128:idx_a]

        start_ub =ub[idx_b-128:idx_b]
        start_ib = ib[idx_b-128:idx_b]
        # ���벨�Σ��ӹ���㿪ʼ��ȡ��
        ua_aligned, ia_aligned = ua[idx_a:], ia[idx_a:]
        ub_aligned, ib_aligned = ub[idx_b:], ib[idx_b:]
        # ��ȡ����ͬ����
        length = min(len(ua_aligned), len(ub_aligned))
        ua_aligned, ia_aligned = ua_aligned[:length], ia_aligned[:length]
        # print(ua_aligned[:128])
        ub_aligned, ib_aligned = ub_aligned[:length], ib_aligned[:length]

        ua_aligned_128 = []
        ia_aligned_128 = []
        ub_aligned_128 = []
        ib_aligned_128 = []
        # === ���ÿ�����ڵ��� ===
        self.align_waveform_to_128(ua_aligned, ia_aligned, ua_aligned_128, ia_aligned_128, "����1����")
        self.align_waveform_to_128(ub_aligned, ib_aligned, ub_aligned_128, ib_aligned_128, "����2����")

        # ��ȡ����ͬ����
        length = min(len(ua_aligned_128), len(ub_aligned_128))
        ua_aligned_128, ia_aligned_128 = ua_aligned_128[:length], ia_aligned_128[:length]
        ub_aligned_128, ib_aligned_128 = ub_aligned_128[:length], ib_aligned_128[:length]
        #������ʼ�ܲ�
        ua_aligned_128 =start_ua+ua_aligned_128
        ia_aligned_128 =start_ia+ia_aligned_128
        ub_aligned_128 =start_ub+ub_aligned_128
        ib_aligned_128 =start_ib+ib_aligned_128
        # ��������
        i_sum = []
        for a, b in zip(ia_aligned_128, ib_aligned_128):
            i_sum.append(a + b)
        

        return ua_aligned_128, i_sum

    def parse_files(self):
        parser = ParseUIData.FrameParser(1, self.format, self.uifactor1)
        parser.endian = "little"
        parser.IsQT = 0
        cnt = 0
        # 1. ��ʼ��Python�б��ռ�����
        ua1_list, ia1_list, ua2_list, ia2_list = [], [], [], []
        cnt = -1
        with open(self.file1path, 'rb') as f1, open(self.file2path, 'rb') as f2:
            while True:
                cnt += 1
                if self.add_frame_num != 0:
                    if cnt == self.add_frame_num:
                        print(f"�ѽ������{cnt}֡")
                        break

                buf1 = f1.read(self.frame_size)
                buf2 = f2.read(self.frame_size)

                # ��һ�ļ�����������
                if not buf1 or not buf2:
                    break

                parser.UIfactor = self.uifactor1
                # �����ļ�1
                if parser.parse_single_frame(buf1, 0) == -1:
                    print("�ļ�1����ʧ��")
                    return None, None, None, None
                 # �������δ���200��������־����������������С��100��break
                if not self.rmsu1_started:
                    if parser.rms_U > 200:
                        self.rmsu1_above_200_count += 1
                    else:
                        self.rmsu1_above_200_count = 0
                    if self.rmsu1_above_200_count >= 3:
                        self.rmsu1_started = True
                        self.rmsu1_below_100_count = 0  # ����������
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
                # �����ļ�2
                if parser.parse_single_frame(buf2, 0) == -1:
                    print("�ļ�2����ʧ��")
                    return None, None, None, None
                # �������δ���200��������־���������������ε�ѹ��ЧֵС��100��break

                if not self.rmsu2_started:
                    if parser.rms_U > 200:
                        self.rmsu2_above_200_count += 1
                    else:
                        self.rmsu2_above_200_count = 0
                    if self.rmsu2_above_200_count >= 3:
                        self.rmsu2_started = True
                        self.rmsu2_below_100_count = 0  # ����������
                else:
                    if parser.rms_U < 100:
                        self.rmsu2_below_100_count += 1
                    else:
                        self.rmsu2_below_100_count = 0
                    if self.rmsu2_below_100_count >= 3:
                        break
                # print(parser.rms_U)
                # ֻ����֮ǰ����200������£���������С��100��break
                if self.last_rmsu2 > 200:
                    if parser.rms_U < 100:
                        self.rmsu2_below_100_count += 1
                    else:
                        self.rmsu2_below_100_count = 0
                    if self.rmsu2_below_100_count >= 3:
                        break
                self.last_rmsu2 = parser.rms_U
                ua2_list.append(parser.voltage.copy())
                if any(abs(x) > self.peak_value for x in parser.current) and self.uifactor2[3]<0: #�綯�����ʱȥ������ʱ�ļ��
                    continue
                else:
                    ia2_list.append(parser.current.copy())

        # 2. ѭ����תΪһάlist
        print("����֡������ɣ�����ת��ΪNumPy����...")
        ua1 = list(itertools.chain.from_iterable(ua1_list))
        ia1 = list(itertools.chain.from_iterable(ia1_list))
        ua2 = list(itertools.chain.from_iterable(ua2_list))
        ia2 = list(itertools.chain.from_iterable(ia2_list))
        print("ת����ɣ�")

        return ua1, ia1, ua2, ia2
    def ensure_output_folder_and_unique_file(self,folder, filename):
        """
        ȷ���ļ��д��ڣ�������������Ψһ�ļ���
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
        # ���������ļ�����ȡ���Եĵ�ѹ����������
        ua1, ia1, ua2, ia2 = self.parse_files()
        if ua1 is not None and ua2 is not None and ia1 is not None and ia2 is not None:
            # ���벢��������ļ��ĵ�������
            U, sumI = self.align_and_add(ua1, ia1, ua2, ia2)
            U = U[:len(U)//128*128]
            sumI = sumI[:len(sumI)//128*128]
            # ��ȡ�����н϶̵ĳ��ȣ�����һ��
            min_len = min(len(U), len(sumI))
            U = U[:min_len]
            sumI = sumI[:min_len]
            print(f"U����: {len(U)}, sumI����: {len(sumI)}")

            # �Ż������ݲ������ܣ�ʹ��NumPy�������������ƴ�ӣ�����ѭ���Ͷ��extend
            # �������͵�ѹ����ת��ΪNumPy����
            sumI_np = np.asarray(sumI, dtype=np.float64)
            U_np = np.asarray(U, dtype=np.float64)

            # ʹ��FormatConverter��to_signed_24bit_little_endian����ת��Ϊ�ֽ���
            byte_I_data = formatConvert.to_signed_24bit_little_endian(sumI_np, 4)
            byte_U_data = formatConvert.to_signed_24bit_little_endian(U_np, 4)

            # ���ֽ���ת��ΪNumPy���鲢reshapeΪ(N, 3)
            arr_I = np.frombuffer(byte_I_data, dtype=np.uint8).reshape(-1, 3)
            arr_U = np.frombuffer(byte_U_data, dtype=np.uint8).reshape(-1, 3)

            # ƴ��U��I��3�ֽڣ��γ�(N, 6)�����飬Ȼ����չƽ��һά
            interleaved = np.empty((arr_I.shape[0], 6), dtype=np.uint8)
            interleaved[:, 0:3] = arr_U
            interleaved[:, 3:6] = arr_I
            byte_data = interleaved.reshape(-1)

            # �ض��ֽ����飬ʹ�䳤��Ϊ768����������ÿ֡768�ֽڣ�
            byte_data = byte_data[:len(byte_data)//768*768]

            # �������ļ��Ѵ��ڣ���ɾ��
            if os.path.exists(output_file):
                os.remove(output_file)

            # �����������ݵ��������ļ�
            print(f"byte_data����: {len(byte_data)}")
            formatConvert.save_data_as_bin(output_file, byte_data, 778)
            print(f"�����ѱ��浽 {output_file}")
        else:
            print("���ݽ���ʧ�ܣ�δ��������ļ���")

if __name__ == "__main__":
    #����綯����ʼ������ڵ��ܲ�û�и������Ĺ���㣬���ܲ��ᱻ������Ŀǰ��ʼ���ж�Ϊ�����������
    # file1path = "D:\\work\\project\\WaveParse\\wlb_waveRecord_output\\�����������У�10���ӣ�.bin"
    # file2path = "D:\\work\\project\\WaveParse\\pre_zeroI_data_01_01_wlb_waveRecord.bin"
    # file1path = "D:\\WorkProjectsSVN\\WaveParse\\wlb_waveRecord_output\\03_01_wlb_waveRecord.bin"
    # file2path = "D:\\WorkProjectsSVN\\WaveParse\\wlb_waveRecord_output\\03_01_wlb_waveRecord.bin"
    peak_value = 10 #�綯����ʼ�����ֵ,�������ʱ������ϵ����-1�ٵ��ӣ�ȥ������ʱ�ļ�壬������Ӳ��ù�,����ϵ��Ϊ����ʾ���,file2ʹ�õ綯��
    file1_Dir_or_path = r"D:\WorkProjectsSVN\WaveParse\endZeroData\120s_zeroI_data_��ˮ��_wlb_waveRecord.bin"
    # file1path = r"C:\Users\����\Desktop\addWave\����̺һ����1���ӣ�������̺һ��+�ȵÿ����У�1-2���ӣ�������̺����+�ȵÿ����У�2-3���ӣ�.BIN"
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
        # ������ǵ����ļ�
        bin_files = [file1_Dir_or_path]
    elif os.path.isdir(file1_Dir_or_path):
        # �������Ŀ¼��������Ŀ¼������bin�ļ�
        bin_files = glob.glob(os.path.join(file1_Dir_or_path, "*.bin"))
    # ���Ŀ¼���ڵ�ǰ�ű�Ŀ¼�� addedData �ļ��У�
    current_dir = os.path.dirname(os.path.abspath(__file__))
    output_dir = os.path.join(current_dir, "addedData")
    # ѭ���������� bin �ļ�
    for file1path in bin_files:
        print(f"���ڴ���: {file1path}")

        # ����������
        adder = WlbWaveRecordWaveAdder(
            file1path,
            file2path,
            uifactor1=uifactor1,
            uifactor2=uifactor2,
            add_frame_num=0,
            peak_value=peak_value
        )

        # �Զ�����Ψһ����ļ���
        base_name = os.path.basename(file1path)
        output_name = f"addedData_{os.path.splitext(base_name)[0]}.bin"
        filepath = adder.ensure_output_folder_and_unique_file(output_dir, output_name)

        # ���Ӳ�����
        output_file = os.path.join(output_dir, filepath)
        adder.process_and_save(output_file)

        print(f"����ļ�: {output_file}\n")

    print("ȫ��������ɣ�")