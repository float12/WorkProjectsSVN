# -*- coding: gbk -*-
#������Ŀ¼������bin�ļ����и�ʽת����698��ʽ����ת��������¼����ʽ��������¼�������ת����� �Լ�ѡ��main��ĺ�����
import glob
import os
from pathlib import Path
import scipy.io as sio
import numpy as np
import matplotlib.pyplot as plt

class FormatConverter:
    # ������
    point_in_one_cycle = 200  # mat���ܲ�����
    float_to_decimal_points = 4  # С��תʮ���Ʊ�����λ��
    out_point_in_one_cycle = 128  # ת��������ܲ�����

    def __init__(self):
        pass

    @staticmethod
    def sample_chunks_evenly(data_array, chunk_size=200, sample_size=128):
        """
        ��һ��NumPy����ָ�ɹ̶���С�Ŀ飬����ÿ�����о��ȵ�ȡ��ָ��������Ԫ�ء�
        """
        if not isinstance(data_array, np.ndarray):
            data_array = np.array(data_array)

        total_size = data_array.size
        if total_size % chunk_size != 0:
            raise ValueError(f"�����С ({total_size}) �޷������С ({chunk_size}) ������")

        num_chunks = total_size // chunk_size
        reshaped_array = data_array.reshape(num_chunks, chunk_size)
        indices = np.linspace(0, chunk_size, sample_size, endpoint=False, dtype=int)
        sampled_chunks = reshaped_array[:, indices]
        return sampled_chunks.flatten()

    @staticmethod
    def to_signed_24bit_little_endian(data, scale_power=0):
        """
        �Ѹ������� -> ���� -> �з������� -> 3�ֽ�С�˱�ʾ
        """
        arr = np.array(data, dtype=np.float64)
        scaled = np.round(arr * (10 ** scale_power)).astype(np.int64)
        scaled = np.clip(scaled, -2 ** 23, 2 ** 23 - 1)
        byte_array = bytearray()
        for val in scaled:
            val = int(val)
            if val < 0:
                val = (1 << 24) + val  # �����ʾ
            b = val.to_bytes(3, byteorder='little', signed=False)
            byte_array.extend(b)
        return bytes(byte_array)

    def save_data_as_bin(self, output_path, buffer, out_frame_size):
        chunk_size = 6 * self.out_point_in_one_cycle
        with open(output_path, 'wb') as f_out:
            for i in range(0, len(buffer), chunk_size):
                chunk = buffer[i:i + chunk_size]
                frame_index = 0
                new_frame = bytearray(out_frame_size)
                new_frame[0] = 0x68
                new_frame[-1] = 0x16
                seconds = 0
                new_frame[1:5] = seconds.to_bytes(4, 'little')
                new_frame[5:773] = bytes(chunk)
                new_frame[773:775] = (0).to_bytes(2, 'big')
                checksum = sum(new_frame[0:775]) & 0xFFFF
                new_frame[775:777] = checksum.to_bytes(2, 'little')
                f_out.write(new_frame)
                frame_index += 1
    def parse_mat_file(self, input_file_path, output_path):
        try:
            mat_data = sio.loadmat(input_file_path, mat_dtype=True, squeeze_me=True)
            print(f"�ɹ������ļ�: {input_file_path}")
            print("-" * 30)
            byte_u_data = None
            for key, value in mat_data.items():
                if not key.startswith('__'):
                    print(f"������: {key}")
                    print(f"��������: {type(value)}")
                    if hasattr(value, 'shape'):
                        print(f"������״: {value.shape}")
                        print(f"����ֵ (����):")
                        if value.size > 10:
                            print(value.ravel()[:10])
                        else:
                            print(value)
                        new_length = (len(value) // self.point_in_one_cycle) * self.point_in_one_cycle
                        value = value[:new_length]
                        if key == "CH1":
                            u_data = self.sample_chunks_evenly(value)
                            byte_u_data = self.to_signed_24bit_little_endian(u_data, self.float_to_decimal_points)
                        elif key == "CH2":
                            I_data = self.sample_chunks_evenly(value)
                            byte_I_data = self.to_signed_24bit_little_endian(I_data, self.float_to_decimal_points)
                            byte_data = bytearray()
                            for i in range(0, len(byte_I_data), 3):
                                byte_data.extend(byte_u_data[i:i + 3])
                                byte_data.extend(byte_I_data[i:i + 3])
                            self.save_data_as_bin(output_path, byte_data, 778)
                    else:
                        print(f"����ֵ: {value}")
                    print("-" * 30)
        except FileNotFoundError:
            print(f"����: �ļ� '{input_file_path}' δ�ҵ��������ļ�·���Ƿ���ȷ��")
        except NotImplementedError as e:
            print(f"����: scipy.io �޷�������ļ���ʽ��{e}")
            print("�������һ�����µ�MATLAB .mat�ļ���ʽ����ȷ������MATLAB v5��v7.3֮ǰ�İ汾��")
        except Exception as e:
            print(f"�����ļ�ʱ����δ֪����: {e}")

    @staticmethod
    def print_buffer_hex(buffer, bytes_per_line=50):
        for i in range(0, len(buffer), bytes_per_line):
            line = buffer[i:i+bytes_per_line]
            hex_str = ' '.join(f"{b:02X}" for b in line)
            print(hex_str)
    def convert_record_to_transmit(self, input_path, output_path, frame_in, frame_out):
        with open(input_path, 'rb') as f_in, open(output_path, 'wb') as f_out:
                    frame_index = 0
                    seq = 1
                    while True:
                        buffer = f_in.read(frame_in)
                        if len(buffer) < frame_in:
                            print(f"ת����ɣ�֡����{frame_index}")
                            break
                        if buffer[0] != 0x68 or buffer[-1] != 0x16:
                            print("��ʽ����")
                            return
                        new_frame = bytearray(frame_out)
                        new_frame[0] = 0x68
                        new_frame[1] = 0x31
                        length = 776
                        new_frame[2:4] = length.to_bytes(2, 'little')
                        new_frame[4:5] = seq.to_bytes(1, 'little')
                        seq = (seq+1)%255
                        new_frame[-1] = 0x16
                        new_frame[5:773] = buffer[5:5+768]
                        checksum = sum(new_frame[1:773]) & 0xFFFFFFFF
                        new_frame[773:777] = checksum.to_bytes(4, 'little')
                        f_out.write(new_frame)
                        frame_index += 1
    def delete_wlbWaveRecord_NO(self, input_path, output_path, frame_in, frame_out):
        with open(input_path, 'rb') as f_in, open(output_path, 'wb') as f_out:
            frame_index = 0
            while True:
                buffer = f_in.read(frame_in)
                if len(buffer) < frame_in:
                    print(f"ת����ɣ�֡����{frame_index}")
                    break
                if buffer[0] != 0x68 or buffer[-1] != 0x16:
                    print("��ʽ����")
                    return
                new_frame = bytearray(frame_out)
                new_frame[0] = 0x68
                new_frame[-1] = 0x16
                seconds = int.from_bytes(buffer[1:5], 'little')
                new_frame[1:5] = seconds.to_bytes(4, 'little')
                new_frame[5:773] = b''.join(
                    buffer[5 + i:5 + i + 3][::-1] for i in range(0, 768, 3)
                )
                new_frame[773:775] = (0).to_bytes(2, 'big')
                checksum = sum(new_frame[0:775]) & 0xFFFF
                new_frame[775:777] = checksum.to_bytes(2, 'little')
                f_out.write(new_frame)
                frame_index += 1

    def convert_698_to_wlb_waveRecord(self, input_path, output_path, input_frame_size, output_frame_size):
        with open(input_path, 'rb') as f_in, open(output_path, 'wb') as f_out:
            frame_index = 0
            buffer = f_in.read(53)  # ���ĵ�ϵ��֡
            while True:
                buffer = f_in.read(input_frame_size)
                if len(buffer) < input_frame_size:
                    print(f"ת����ɣ�֡����{frame_index}")
                    break
                if buffer[0] != 0x68 or buffer[-1] != 0x16:
                    print("��ʽ����")
                    return
                new_frame = bytearray(output_frame_size)
                new_frame[0] = 0x68
                new_frame[-1] = 0x16
                seconds = int.from_bytes(buffer[29:33], 'big')
                new_frame[1:5] = seconds.to_bytes(4, 'little')
                new_frame[5:773] = b''.join(
                    buffer[42 + i:42 + i + 3][::-1] for i in range(0, 768, 3)
                )
                new_frame[773:775] = (0).to_bytes(2, 'big')
                checksum = sum(new_frame[0:775]) & 0xFFFF
                new_frame[775:777] = checksum.to_bytes(2, 'little')
                f_out.write(new_frame)
                frame_index += 1

    # hex2bin.py
    def txt_to_bin(self,txt_file, bin_file):
        with open(txt_file, "r", encoding="utf-8") as f:
            content = f.read()

        # ɾ������ո�ͻ��У��ָ���б�
        hex_list = content.strip().split()

        # ת��Ϊ�ֽ�
        byte_data = bytes(int(x, 16) for x in hex_list)

        # ����Ϊ�������ļ�
        with open(bin_file, "wb") as f:
            f.write(byte_data)
    @staticmethod
    def ensure_output_folder_and_unique_file(folder, filename):
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

if __name__ == "__main__":
    format_list = ["mat", "698", "wlb_waveRecord_with_NO","waveRecord_to_dataTransmit","wlb_waveRecord_txt"]

    #����Ŀ¼�����Ŀ¼�������ʽ
    # input_dir = Path("D:\\work\\project\\WaveParse\\003")
    # input_dir = Path("D:\\WorkProjectsSVN\\WaveParse\\3p-1p-output")
    # input_dir = Path("C:\\Users\\nh\\Downloads")
    # input_dir = Path(r"C:\Users\����\Desktop\wave")
    # input_dir = Path(r"C:\Users\����\Desktop")
    input_dir_or_path = Path(r"D:\WorkProjectsSVN\WaveParse\WaveExtend")
    # 0:mat->������¼����ʽ,
    # 1��698->������¼����ʽ,
    # 2��wlb_waveRecord_with_NO(����ŵ�������¼����ʽ)->������¼����ʽ,
    # 3��������¼����ʽ->���������ݴ����ʽ
    # 4 :������¼����ʽtxtתbin
    input_format = format_list[3]
    convert_record_to_dataTransmit = False # �Ƿ�ֱ��ת�������������ݴ����ʽ True False


    if os.path.isfile(input_dir_or_path):
        # ������ǵ����ļ�
        bin_files = [input_dir_or_path]
    elif os.path.isdir(input_dir_or_path):
        # �������Ŀ¼��������Ŀ¼������bin�ļ�
        bin_files = glob.glob(os.path.join(input_dir_or_path, "*.bin"))
    output_dir = Path("wlb_waveRecord_output")
    if input_format == "mat":
        filesuffix = ".mat"
    elif input_format == "wlb_waveRecord_txt":
        filesuffix = ".txt"
    else:
        filesuffix = ".bin"
    converter = FormatConverter()
    for input_file in bin_files:
        if input_format != "waveRecord_to_dataTransmit":
            output_file = FormatConverter.ensure_output_folder_and_unique_file(output_dir,input_file.stem + "_wlb_waveRecord.bin")
        else:
            input_file = Path(input_file)  # ת�� Path ����
            clean_stem = input_file.stem.replace("_wlb_waveRecord", "")
            output_dir = Path("wlb_Transmit_output")
            output_file = FormatConverter.ensure_output_folder_and_unique_file(output_dir,clean_stem + "_wlb_Transmit.bin")
        print(f"ת�� {input_file} �� {output_file}")
        if input_format == "mat":
            converter.parse_mat_file(input_file, output_file)
        elif input_format == "698":
            converter.convert_698_to_wlb_waveRecord(input_file, output_file, 815, 778)
        elif input_format == "wlb_waveRecord_with_NO":
            converter.delete_wlbWaveRecord_NO(input_file, output_file, 782, 778)
        elif input_format == "waveRecord_to_dataTransmit":
            converter.convert_record_to_transmit(input_file, output_file, 778, 778)
        elif input_format == "wlb_waveRecord_txt":
            converter.txt_to_bin(input_file,output_file)
        if convert_record_to_dataTransmit:
            # ��ȡ output_file ���ļ�����ȥ���ϼ�Ŀ¼������ȥ�� "_wlb_waveRecord"
            clean_stem = Path(output_file).name.replace("_wlb_waveRecord", "").replace(".bin", "")
            output_dir = Path("wlb_Transmit_output")
            transimit_output_file = FormatConverter.ensure_output_folder_and_unique_file(output_dir,clean_stem + "_wlb_Transmit.bin")
            converter.convert_record_to_transmit(output_file, transimit_output_file, 778, 778)
            # INSERT_YOUR_CODE
            if os.path.exists(output_file):
                os.remove(output_file)
                print(f"��ɾ���м��ļ�: {output_file}")
        print("ת������")
