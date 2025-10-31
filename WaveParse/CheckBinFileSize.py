# -*- coding: gbk -*-
import os

def check_bin_files(directory: str, expected_size: int):
    """
    ���Ŀ¼������ .bin �ļ��Ĵ�С�Ƿ����ָ����С

    :param directory: Ŀ���ļ���·��
    :param expected_size: �������ļ���С���ֽڣ�
    """
    if not os.path.isdir(directory):
        print(f"����{directory} ������Ч���ļ���·��")
        return

    files = [f for f in os.listdir(directory) if f.lower().endswith(".bin")]
    if not files:
        print("��Ŀ¼��û�� .bin �ļ�")
        return

    all_passed = True
    for filename in files:
        filepath = os.path.join(directory, filename)
        size = os.path.getsize(filepath)
        if size == expected_size:
            print(f"[OK]  {filename} ��СΪ {size} �ֽڣ�����Ҫ��")
        else:
            print(f"[FAIL] {filename} ��СΪ {size} �ֽڣ�������Ҫ������ {expected_size}��")
            all_passed = False

    if all_passed:
        print("\n���� .bin �ļ�������Ҫ��")
    else:
        print("\n���� .bin �ļ���С������Ҫ��")


if __name__ == "__main__":
    # ʾ�����޸�Ϊ���Լ���·�����ļ���С
    dir_path ="C:\\Users\\����\\Downloads"
    expected_size =73350053

    # dir_path ="F:\\"
    # expected_size =211140000

    check_bin_files(dir_path, expected_size)