# -*- coding: gbk -*-
import os
import math
import random
#֧������ŵ�������¼����ʽ��������¼����ʽ
class phaseNumConverter:
    """
    ����˵����
    mode��ת��ģʽ��"3p2_1p" ��ʾ����ת���࣬"1p2_3p" ��ʾ����ת����
    chunk_size��֡��С��778��������¼����ʽ 782������ŵ�������¼����ʽ
    3p2_1p��
        input_3p_file�����������ļ�·��
        output_1p_files����������ļ�·���б�
    1p2_3p��
        a_file��A�������ļ�·��
        b_file��B�������ļ�·��
        c_file��C�������ļ�·��
        output_3p_file����������ļ�·����mode="1p2_3p"ʱʹ�ã�
        a_zero_cycle_sec��A�ಹ���루mode="1p2_3p"ʱʹ�ã�Ĭ��Ϊ0��
        b_zero_cycle_sec��B�ಹ���루mode="1p2_3p"ʱʹ�ã�Ĭ��Ϊ0��
        c_zero_cycle_sec��C�ಹ���루mode="1p2_3p"ʱʹ�ã�Ĭ��Ϊ0��
    """
    def __init__(self, mode, chunk_size=778,
                 input_3p_file=None, output_1p_files=None, 
                 a_file=None, b_file=None, c_file=None, output_3p_file=None, 
                 a_zero_cycle_sec=0, b_zero_cycle_sec=0, c_zero_cycle_sec=0,
                 ui_factor=None):
        self.mode = mode
        self.chunk_size = chunk_size
        self.group_size = chunk_size * 3

        # ����ת�������
        self.input_file = input_3p_file

        output_dir = "3p-1p-output"
        os.makedirs(output_dir, exist_ok=True)
        self.output_files = output_1p_files if output_1p_files is not None else [
            os.path.join(output_dir, f"{os.path.splitext(os.path.basename(input_3p_file))[0]}_A.bin"),
            os.path.join(output_dir, f"{os.path.splitext(os.path.basename(input_3p_file))[0]}_B.bin"),
            os.path.join(output_dir, f"{os.path.splitext(os.path.basename(input_3p_file))[0]}_C.bin")
        ]

        # ����ת�������
        self.a_file = a_file
        self.b_file = b_file
        self.c_file = c_file
        self.output_file = output_3p_file
        self.a_zero_cycle_nums = a_zero_cycle_sec * 50
        self.b_zero_cycle_nums = b_zero_cycle_sec * 50
        self.c_zero_cycle_nums = c_zero_cycle_sec * 50
        self.ui_factor = ui_factor

    def float_to_reverse_bytes(self,value: float, endian: str = "little") -> bytes:
        """
        ������תΪ 3 �ֽ���任���� reverse_bytes_to_float ���ף�
        value: ����� float ����
        endian: 'little' �� 'big'
        ����: 3 �ֽ� bytes
        """
        # תΪ����
        int_val = int(round(value))
        if int_val < 0:
            int_val = (1 << 24) + int_val  # �����ʾ
        b = int_val.to_bytes(3, byteorder=endian, signed=False)
        return b

    def run(self):
        if self.mode == "3p_to_1p":
            self._convert_3p_to_1p()
        elif self.mode == "1p_to_3p":
            self._convert_1p_to_3p()
        else:
            print("δ֪ģʽ��������modeΪ'3p2_1p'��'1p2_3p'")

    def _convert_3p_to_1p(self):
        """
        ���ನ������ת�����������ļ�
        """
        # ɾ���Ѵ��ڵ�����ļ�
        for f in self.output_files:
            if os.path.exists(f):
                os.remove(f)
                print(f"{f} ��ɾ��")
            else:
                print(f"{f} ������")

        with open(self.input_file, "rb") as f, \
             open(self.output_files[0], "wb") as out_a, \
             open(self.output_files[1], "wb") as out_b, \
             open(self.output_files[2], "wb") as out_c:
            cnt = 0
            while True:
                group = f.read(self.group_size)
                if len(group) < self.group_size:
                    break
                a_chunk = group[0:self.chunk_size]
                b_chunk = group[self.chunk_size:self.chunk_size*2]
                c_chunk = group[self.chunk_size*2:self.chunk_size*3]
                if (a_chunk[0] != 0x68 or b_chunk[0] != 0x68 or c_chunk[0] != 0x68):
                    print("start data error")
                    break
                if (a_chunk[self.chunk_size-1] != 0x16 or b_chunk[self.chunk_size-1] != 0x16 or c_chunk[self.chunk_size-1] != 0x16):
                    print("end data error")
                    break
                out_a.write(a_chunk)
                out_b.write(b_chunk)
                out_c.write(c_chunk)
                cnt += 1
                if cnt % 1000 == 0:
                    print(f"write chunk {cnt} done")
        print("A��B��C �ನ�������ѷֱ𱣴浽��", self.output_files)

    def _convert_1p_to_3p(self):
        """
        ���������ļ��ϳ�һ�������ļ���֧�ָ���ǰ�ò���
        """
        # ��ȡ���е�������
        def read_all_frames(filename):
            frames = []
            with open(filename, "rb") as f:
                while True:
                    chunk = f.read(self.chunk_size)
                    if len(chunk) < self.chunk_size:
                        break
                    frames.append(chunk)
            return frames

        a_frames = read_all_frames(self.a_file) if self.a_file else []
        b_frames = read_all_frames(self.b_file) if self.b_file else []
        c_frames = read_all_frames(self.c_file) if self.c_file else []

        # ����128��220V��Чֵ�ĵ�ѹ����Ư��������ֵ
        voltage = []
        zero_current = []
        for i in range(128):
            # ��ѹ������-0.5V~0.5V
            voltage_noise = random.uniform(-0.5, 0.5)
            voltage.append(220 * (2 ** 0.5) * math.sin(2 * math.pi * i / 128) + voltage_noise)
            # ����������-0.01A~0.01A
            zero_current.append(random.uniform(-0.01, 0.01))
        
        for i in range(3):
            voltage_bytes = []
            zero_current_bytes = []
            zero_data = []
            #����ת��ѹ�������ֽ�
            for j in range(128):
                voltage_bytes.extend(self.float_to_reverse_bytes(voltage[j] / self.ui_factor[i], 'little'))
                zero_current_bytes.extend(self.float_to_reverse_bytes(zero_current[j] / self.ui_factor[i+3], 'little'))
            # ÿ���ֽ�voltage_bytes�����ֽ�zero_current_bytes���б��浽zero_data
            for k in range(128):
                zero_data.extend(voltage_bytes[k*3:k*3+3])
                zero_data.extend(zero_current_bytes[k*3:k*3+3])
            zero_frame = bytes([0x68] +  [0x00]*4 + zero_data + [0x00]*4 + [0x16])
            checksum = sum(zero_frame[0:self.chunk_size-3]) & 0xFFFF
            zero_frame = bytearray(zero_frame)  # ת�ɿɱ��ֽ�����
            zero_frame[775:777] = checksum.to_bytes(2, 'little')
            if i == 0:
                a_frames =  [zero_frame]*self.a_zero_cycle_nums + a_frames
            elif i == 1:
                b_frames = [zero_frame]*self.b_zero_cycle_nums + b_frames
            elif i == 2:
                c_frames = [zero_frame]*self.c_zero_cycle_nums + c_frames
        # ���볤��
        min_len = min(len(a_frames), len(b_frames), len(c_frames))
        a_frames = a_frames[:min_len]
        b_frames = b_frames[:min_len]
        c_frames = c_frames[:min_len]


        if os.path.exists(self.output_file):
            os.remove(self.output_file)
            print(f"{self.output_file}ɾ�����")
        with open(self.output_file, "wb") as fout:
            for i in range(min_len):
                group = a_frames[i] + b_frames[i] + c_frames[i]
                fout.write(group)
                if (i+1) % 5000 == 0:
                    print(f"write chunk {i+1} done")
        print(f"�����ļ��ѱ��浽��{self.output_file}")
if __name__ == "__main__":
    # ʾ��������ת����
    input_dir = "F:\\"  # ����Ŀ¼·��
    for file_name in os.listdir(input_dir):
        if not file_name.lower().endswith(".bin"):
            continue
        else:
            converter = phaseNumConverter(mode="3p_to_1p", chunk_size=782,input_3p_file =os.path.join(input_dir, file_name))
            converter.run()

    # ʾ��������ת����
    # a_file = "01_01_school_format.bin"
    # b_file = "03_01_school_format.bin"
    # c_file = "04_01_school_format.bin"
    # ui_factor = (0.0003742375, 0.0003742375, 0.0003742375, 0.00001014207, 0.00001014207, 0.00001014207)#�����ѹ����ϵ�����綯���г�Ĭ��
    # # ui_factor = (0.0002672848, 0.0002672848, 0.0002672848, 0.0002486532, 0.0002486532, 0.0002486532)#�����ѹ����ϵ����¼����װĬ��
    # converter = phaseNumConverter(mode="1p_to_3p", a_file = a_file, b_file= b_file,
    #                               c_file= c_file, output_3p_file="3p_out_wlb_waverecord.bin", a_zero_cycle_sec=0,
    #                                  b_zero_cycle_sec=10, c_zero_cycle_sec=200, ui_factor=ui_factor)
    # converter.run()

