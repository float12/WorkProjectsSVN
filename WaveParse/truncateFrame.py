# -*- coding: gbk -*-
import os
import glob

def truncate_bin_file(filepath, discard_size=0, keep_size=815*9000):
    """
    �����ļ�ǰ discard_size �ֽڣ�ֻ�������� keep_size �ֽڡ�
    �� discard_size=0 ��ʾ��������

    """
    with open(filepath, "rb") as f:
        f.seek(0, os.SEEK_END)
        file_size = f.tell()

        # ����Ҫ���������ݳ���
        if keep_size == 0 or (discard_size + keep_size) > file_size:
            keep_size = file_size - discard_size

        # �Ӷ���λ�ÿ�ʼ��ȡ
        f.seek(discard_size)
        data = f.read(keep_size)

    # ����д�أ�ֻ����Ŀ������
    with open(filepath, "wb") as f:
        f.write(data)

# �ж�����·�����ļ�����Ŀ¼���ֱ���
bin_dir_or_file = r"Z:\01��ѹ̨����Ʒ�з�\02����з�\12-���ɱ�ʶ\�綯�����ԭʼ��������\�ϳɵ綯�����Σ�uiϵ��0.0001��\�طŲ���\��չ-0���ʣ��綯��������0���ʣ��綯��������ֹͣwlb_Transmit.bin"# ���Ըĳ��ļ�·����Ŀ¼·��
discard_size = 0#512��53
keep_size = 20000 *778  # �� keep_size=0 ��ʾȫ���������� discard_size ���ļ�ĩβ����������

if os.path.isfile(bin_dir_or_file):
    # ������ǵ����ļ�
    bin_files = [bin_dir_or_file]
elif os.path.isdir(bin_dir_or_file):
    # �������Ŀ¼��������Ŀ¼������bin�ļ�
    bin_files = glob.glob(os.path.join(bin_dir_or_file, "*.bin"))
else:
    print(f"����·����Ч: {bin_dir_or_file}")
    bin_files = []

for bin_file in bin_files:
    truncate_bin_file(bin_file, discard_size, keep_size)
    print(f"{bin_file} �������")

