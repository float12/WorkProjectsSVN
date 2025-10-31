# -*- coding: gbk -*-
import binascii
import glob
import pathlib
import struct
import sys
import time
# from pyqtgraph.exporters import ImageExporter
import numpy as np
from PyQt5.QtWidgets import (QApplication, QWidget, QVBoxLayout, QHBoxLayout,
                             QPushButton, QLabel, QComboBox, QLineEdit,
                             QFileDialog, QMessageBox, QTextEdit, QSplitter, QGridLayout, QSizePolicy, QCheckBox)
import pyqtgraph as pg
from FormatConvert import FormatConverter
import PlotDataTheard
from SaveToCsvTheard import SaveToCsvTheard
import os
from pyqtgraph.exporters import ImageExporter
from _3p1pConvert import phaseNumConverter
SAMPLE_NUM = 128
#print�ض���
class RedirectTextEdit(QTextEdit):
    def __init__(self):
        super().__init__()

    def write(self, text):
        """�ض�������� QTextEdit �ؼ�"""
        self.insertPlainText(text)
        self.ensureCursorVisible()  # ȷ�����ɼ����ı���������ײ�
        self.repaint()  # �����ػ�ؼ�
def is_valid_hex(s):
    """����ַ����Ƿ�����Ч��ʮ��������ֵ��"""
    try:
        # ���Խ��ַ���תΪ������ֻ�кϷ���ʮ��������ֵ����ת���ɹ�
        int(s, 16)
        return True
    except ValueError:
        return False
class WaveDataProcessUI(QWidget):

    def __init__(self):
        super().__init__()
        self.phase_num = 1
        self.initUI()
        self.plot_X_data =  None
        self.plot_UA_data = None
        self.plot_UB_data = None
        self.plot_UC_data = None
        self.plot_IA_data = None
        self.plot_IB_data = None
        self.plot_IC_data = None
        self.plot_IA_rms_data = None
        self.plot_UA_rms_data = None
        self.plot_ActPwr_data = None



    def txt_to_bin_file(self,txt_filepath):
        print("ת���ļ�Ϊbin---")
        binary_data =[]
        #ȥ����һ��
        with open(txt_filepath, 'r') as infile:
            lines = infile.readlines()

        with open(txt_filepath, 'w') as outfile:
            outfile.writelines(lines[1:])
        try:
            with open(txt_filepath, 'r') as infile:
                hex_content = infile.read()
        except FileNotFoundError:
            print(f"����: �ļ�δ�ҵ�: {txt_filepath}")
            return
        hex_numbers = hex_content.replace(" ", "").strip()
        if len(hex_numbers) % 2 != 0:
            raise ValueError("Hexadecimal input must have an even length")
        try:
            for i in range(0, len(hex_numbers), 2):
                byte_val_str = hex_numbers[i:i + 2]
                # ��鵱ǰ�ַ����Ƿ�����Ч��ʮ��������ֵ
                if  is_valid_hex(byte_val_str):
                    tmp = int(byte_val_str, 16)
                    binary_data.append(tmp)
                else:
                    print(f"���棺������Ч��ʮ��������ֵ '{byte_val_str}'������ļ�����")
                    return  -1
        except binascii.Error as e:
            print(f"����: ��Ч��ʮ����������: {e}")
            return
        base, ext = os.path.splitext(txt_filepath)
        self.data_file_path = base + ".bin"
        with open(self.data_file_path, 'wb') as f:
            f.write(bytes(binary_data))
        print(f"ʮ������������ת��Ϊ�������ļ�: {self.data_file_path}")

    def initUI(self):
        self.data_file_path = None
        self.UIfactor = [0, 0, 0, 0, 0, 0]
        # �����ָ�Ϊˮƽ����
        self.main_h_layout = QHBoxLayout()  # �滻ԭ���� self.main_layout = QVBoxLayout()

        # ��ഹֱ����������ԭmain_layout����Ϊleft_layout��
        self.left_layout = QVBoxLayout()


        # ��λѡ��
        self.phase_select = QComboBox()
        self.phase_select.addItems([str(1), str(3)])
        self.phase_select.setCurrentIndex(0)  # Ĭ��ѡ��һ�ࣨ1��
        self.left_layout.addWidget(QLabel("ѡ������:"))
        self.left_layout.addWidget(self.phase_select)
        # �����ı������
        self.text_input_label = QLabel("�����ʽת���ļ�Ŀ¼���ļ���")
        self.text_input = QLineEdit()
        self.left_layout.addWidget(self.text_input_label)
        self.left_layout.addWidget(self.text_input)
        # ��ʽѡ��
        self.format_select = QComboBox()
        self.format_select.addItems(["", str("wlb_waveRecord_with_NO"), str("698"),str("wlb_waveRecord")])
        self.format_select.setCurrentIndex(2)  # Ĭ��ѡ�� "698"
        self.left_layout.addWidget(QLabel("ѡ�����ݸ�ʽ:"))
        self.left_layout.addWidget(self.format_select)
        # �ļ�ѡ��
        file_layout = QHBoxLayout()
        self.file_edit = QLineEdit()
        self.file_button = QPushButton("ѡ��bin�ļ�")
        file_layout.addWidget(self.file_edit)
        file_layout.addWidget(self.file_button)
        self.left_layout.addLayout(file_layout)
        # �������ݴ�С��ѡ��
        self.endian_select = QComboBox()
        self.endian_select.addItems(["big", "little"])
        self.endian_select.setCurrentIndex(0)  # Ĭ��ѡ����
        self.left_layout.addWidget(QLabel("�������ݴ�С��:"))
        self.left_layout.addWidget(self.endian_select)
        # ϵ��������
        self.coeff_container = QWidget()
        self.coeff_layout = QGridLayout()
        self.coeff_container.setLayout(self.coeff_layout)
        self.left_layout.addWidget(self.coeff_container)
        # ��ѡ���Ƿ�ɾ��ϵ��֡
        # self.delete_coeff_frame_checkbox = QCheckBox("�Ƿ�ɾ��ϵ��֡")
        # self.delete_coeff_frame_checkbox.setChecked(True)# Ĭ�Ϲ�ѡ
        # self.left_layout.addWidget(self.delete_coeff_frame_checkbox)
        # ������ť
        btn_layout = QHBoxLayout()
        self.save_btn = QPushButton("��������")
        self.plot_btn = QPushButton("����ͼ��")
        self.ConvertTo1P = QPushButton("ת�ɵ���")
        self.ConvertToDataTransmit = QPushButton("�����¼����ʽ->���ݴ����ʽ")
        btn_layout.addWidget(self.save_btn)
        btn_layout.addWidget(self.plot_btn)
        btn_layout.addWidget(self.ConvertTo1P)
        btn_layout.addWidget(self.ConvertToDataTransmit)
        self.left_layout.addLayout(btn_layout)

        # ���ֵ��ʾ
        self.max_Ulabel = QLabel("��ѹ��Чֵ���ֵ��")
        self.left_layout.addWidget(self.max_Ulabel)
        self.max_Ilabel = QLabel("������Чֵ���ֵ��")
        self.left_layout.addWidget(self.max_Ilabel)

        # ����̨�������
        self.text_show = RedirectTextEdit()
        self.text_show.setReadOnly(False)
        self.left_layout.addWidget(self.text_show)

        # ������ʾ��ע�����ﲻ����ӵ�left_layout��
        self.plot_widget = pg.GraphicsLayoutWidget()
        self.plot_widget.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)  # ������չ����
        self.plot_widget.setBackground("w")
        # ��ʼ���洢�����ߺͱ�ǩ���ֵ�
        self.crosshairs = {}
        # ���ӳ���������ƶ��ź�
        self.plot_widget.scene().sigMouseMoved.connect(self.onMouseMoved)

        # ����಼�ֺͲ�����ʾ��ӵ���ˮƽ����
        self.main_h_layout.addLayout(self.left_layout, 1)  # ���ռ1�ݿ��
        self.main_h_layout.addWidget(self.plot_widget, 2)  # �Ҳ�ռ2�ݿ�ȣ��ɰ������������

        # ���ô��ڵ�������
        self.setLayout(self.main_h_layout)
        self.resize(2000, 600)

        # ��ʼ��ϵ������
        self.update_ui_based_on_phase()
        self.setLayout(self.main_h_layout)
        self.setWindowTitle('�������ݽ���')

        # �ź�����
        self.phase_select.currentIndexChanged.connect(self.update_ui_based_on_phase)
        self.file_button.clicked.connect(self.select_file)
        self.save_btn.clicked.connect(self.save_to_csv)
        self.plot_btn.clicked.connect(self.plot_data)
        self.ConvertTo1P.clicked.connect(self.convert_to_1p)
        self.ConvertToDataTransmit.clicked.connect(self.convert_to_data_transmit)
        self.format_select.currentIndexChanged.connect(self.update_format)
        #�ض���print���ı���
        sys.stdout = self.text_show
    #ת�ɵ��ఴť�ص�����
    def convert_to_1p(self):
        if self.data_file_path != None:
            converter = phaseNumConverter(mode="3p_to_1p", chunk_size=782,input_3p_file =self.data_file_path)
            converter.run()
        else:
            print("��ѡ���ļ�")
    #ת�����ݴ����ʽ��ť�ص�����
    def convert_to_data_transmit(self):
        data_file_path = self.text_input.text()
        if data_file_path != None:
            if os.path.isfile(data_file_path):
                # ������ǵ����ļ�
                bin_files = [data_file_path]
            elif os.path.isdir(data_file_path):
                # �������Ŀ¼��������Ŀ¼������bin�ļ�
                bin_files = glob.glob(os.path.join(data_file_path, "*.bin"))
            else:
                print("��������ȷ���ļ�·����Ŀ¼")
                return
            output_dir = pathlib.Path("wlb_waveRecord_output")
            converter = FormatConverter()
            for input_file in bin_files:
                input_file = pathlib.Path(input_file)  # ת�� Path ����
                output_dir = pathlib.Path("wlb_WaveRecord_output")
                output_file = FormatConverter.ensure_output_folder_and_unique_file(output_dir,input_file.name.replace(".bin", "_wlb_waveRecord.bin"))
                converter.delete_wlbWaveRecord_NO(input_file, output_file, 782, 778)
                # ��ȡ output_file ���ļ�����ȥ���ϼ�Ŀ¼������ȥ�� "_wlb_waveRecord"
                clean_stem = pathlib.Path(output_file).name.replace("_wlb_waveRecord", "").replace(".bin", "")
                output_dir = pathlib.Path("wlb_Transmit_output")
                transimit_output_file = FormatConverter.ensure_output_folder_and_unique_file(output_dir,clean_stem + "_wlb_Transmit.bin")
                # print(f"ת�� {output_file} �� {transimit_output_file}")
                converter.convert_record_to_transmit(output_file, transimit_output_file, 778, 778)
                # INSERT_YOUR_CODE
                if os.path.exists(output_file):
                    os.remove(output_file)
            print("ת������")
        else:
            print("��ѡ���ļ�")
    #�������ݰ�ť�ص�����
    def save_to_csv(self):
        if self.data_file_path != None:
            self.get_All_UIFactor()
            self.endian = self.endian_select.currentText()  # ��ȡ��ǰѡ��Ķ����ַ�����"big" �� "little"��
            # self.setEnabled(False)  # ���ô��ڵĽ���
            self.text_show.clear()
            self.update_format()
            self.save_thread = SaveToCsvTheard(self.phase_num, self.format_type,self.data_file_path, self.UIfactor, self.plot_widget,self.endian)  # ����һ���߳�
            self.save_thread.SaveSignal.connect(self.save_finish)  # �̷߳��������źŹҽӵ��ۺ���update_sum
            self.save_thread.Csv_Saver.frame_parser.PrintSignal.connect(self.printRedirect)
            self.save_thread.start()  # �߳�����
            self.setEnabled(False)
            print("start save")
            print(time.time())
        else:
            self.choose_file_tip()

    #���浽excelִ����ɻص�����
    def save_finish(self,RMS_umax,RMS_imax,res):
        self.setEnabled(True)
        print(time.time())
        if res != 0:#��������ʧ��
            print("����ʧ��")
            return
        print("�������")
        self.update_max_values(RMS_umax,RMS_imax)

    # ���Ʋ��ΰ�ť�ص�����
    def plot_data(self):
        if self.data_file_path != None:
            self.get_All_UIFactor()  # ��ȡϵ��
            self.endian = self.endian_select.currentText()  # ��ȡ��ǰѡ��Ķ����ַ�����"big" �� "little"��
            self.plot_widget.clear()  # ����
            self.text_show.clear()
            self.setEnabled(False)  # ���ô��ڵĽ���
            self.update_format()
            # if self.delete_coeff_frame_checkbox.isChecked():
            #     try:
            #         with open(self.data_file_path, 'rb') as f:
            #             data = f.read(53)
            #             if data[0] == 0x68 and data[52] == 0x16:#��ȡϵ��
            #                 float1_bytes = bytes(data[39:43])
            #                 float2_bytes = bytes(data[44:48])
            #                 self.UIfactor[0] = struct.unpack('>f', float1_bytes)[0]
            #                 self.UIfactor[3] = struct.unpack('>f', float2_bytes)[0]
            #                 data = f.read()
            #             else:
            #                 raise ValueError("��ϵ��֡")
            #         with open(self.data_file_path, 'wb') as f:
            #             f.write(data)
            #         print("��ɾ���ļ�ǰ53�ֽڣ�ϵ��֡��")
            #     except Exception as e:
            #         print(f"ɾ��ϵ��֡ʧ��: {e}")
            self.plot_data_cal_thread = PlotDataTheard.PlotTheard(self.phase_num, self.format_type,self.data_file_path,self.UIfactor,self.endian)  # ����һ���߳�
            self.plot_data_cal_thread.plot_signal.connect(self.plot_data_cal_finish)  # �̷߳��������źŹҽӵ��ۺ���update_sum
            self.plot_data_cal_thread.Wave_Plotter.frame_parser.PrintSignal.connect(self.printRedirect)
            self.plot_data_cal_thread.start()  # �߳�����
            # print(time.time())
        else:
            self.choose_file_tip()

    def print_average_every_100(self,label, data_list, max_group_count = 50):
        """
        ÿ100�����ݵ����һ��ƽ��ֵ������ӡǰ max_group_count ��ƽ��ֵ��

        :param label: �ַ�����ǩ���� "�й�����", "I RMS", "U RMS"��
        :param data_list: Ҫ����������б�
        :param max_group_count: ����ӡ�����飨Ĭ�� 50��
        """
        print()
        print(f"ÿ50�ܲ�ƽ�� {label} ǰ {max_group_count} ��")

        count = 0
        data_len = len(data_list)
        for i in range(0, data_len, 50):
            if count >= max_group_count:
                break
            end = min(i + 50, data_len)
            segment = data_list[i:end]
            # ��� segment �ǿյģ�����
            if len(segment) == 0:
                continue
            # ���� list �� NumPy array ȡ��ֵ
            avg = float(np.mean(segment))
            print(f"{avg:.3f}", end=" ")
            count += 1
     # ������ɻص�����
    def plot_data_cal_finish(self,wavePlotter,res):
        if res != 0:#��������ʧ��
            self.setEnabled(True)
            return
        self.plot_X_data =  wavePlotter.plot_X_data
        if self.phase_num ==1:
             self.plot_UA_data =  wavePlotter.plot_UA_data
             self.plot_IA_data =  wavePlotter.plot_IA_data
             self.plot_IA_rms_data = wavePlotter.plot_IA_rms_data
             self.plot_UA_rms_data = wavePlotter.plot_UA_rms_data
             self.plot_ActPwr_data = wavePlotter.plot_ActPwr_data
             self.plot_U_sec_level = wavePlotter.plot_U_sec_level
             self.plot_I_sec_level = wavePlotter.plot_I_sec_level
             self.plot_P_sec_level = wavePlotter.plot_P_sec_level
             # �洢�������ֵ��������������
             fluctuation_data_Irms = []
             # �洢�������ֵ��������������
             fluctuation_data_Urms = []
             # ����ÿ100��ֵ��ƽ��ֵ
             i_len = len(self.plot_IA_rms_data)
             u_len = len(self.plot_UA_rms_data)
             p_len = len(wavePlotter.plot_ActPwr_data)
             self.print_average_every_100("���ڹ���", wavePlotter.plot_AppPwr_data)
             self.print_average_every_100("�й�����", wavePlotter.plot_ActPwr_data)
             self.print_average_every_100("�޹�����", wavePlotter.plot_ReactPwr_data)
             self.print_average_every_100("I RMS", self.plot_IA_rms_data)
             self.print_average_every_100("U RMS", self.plot_UA_rms_data)
             # ����������
        elif self.phase_num == 3:
            self.plot_UA_data = wavePlotter.plot_UA_data
            self.plot_IA_data = wavePlotter.plot_IA_data
            self.plot_UB_data = wavePlotter.plot_UB_data
            self.plot_IB_data = wavePlotter.plot_IB_data
            self.plot_UC_data = wavePlotter.plot_UC_data
            self.plot_IC_data = wavePlotter.plot_IC_data
            # self.plot_P_data = wavePlotter.plot_P_data
        self.FirstSecondFrames = wavePlotter.frame_parser.FirstSecondFrameCnt
        self.add_plot_to_widget()
        self.plot_widget.show()
        # print(time.time())
        print("�������")
        self.update_max_values(wavePlotter.frame_parser.rms_Umax,wavePlotter.frame_parser.rms_Imax)
        self.setEnabled(True)
    #�����ı���
    def create_line_edit(self,layout):
        line = QLineEdit()
        line.setText(str(0.00026))
        layout.addWidget(line)
        return  line
    # ���ݸ�ʽ���±���
    def update_format(self):
        if str(self.format_select.currentText()) == "wlb_waveRecord_with_NO":
            self.format_type = 0
        elif str(self.format_select.currentText()) == "698":
            self.format_type = 1
        elif str(self.format_select.currentText()) == "wlb_waveRecord":
            self.format_type = 2
    # ������������ui
    def update_ui_based_on_phase(self):
        # ��ȡ��ǰѡ�����λ
        self.phase_num = int(self.phase_select.currentText())

        # ����ɵ�ϵ���������
        if hasattr(self, 'coefficient_widget'):
            self.coefficient_widget.deleteLater()

        # �����µ���������Ͳ���
        self.coefficient_widget = QWidget()
        layout = QVBoxLayout(self.coefficient_widget)

        # ��������������
        layout.addWidget(QLabel("A���ѹ��V��ϵ����"))
        self.U_a_Factor = self.create_line_edit(layout)
        self.U_a_Factor.setText("3.7068474922E-4")
        layout.addWidget(QLabel("B���ѹ��V��ϵ����"))
        self.U_b_Factor = self.create_line_edit(layout)
        self.U_b_Factor.setText("0.0002673594")
        layout.addWidget(QLabel("C���ѹ��V��ϵ����"))
        self.U_c_Factor = self.create_line_edit(layout)

        layout.addWidget(QLabel("A�������A��ϵ����"))
        self.I_a_Factor = self.create_line_edit(layout)
        self.I_a_Factor.setText("1.00476572E-5")
        layout.addWidget(QLabel("B�������A��ϵ����"))
        self.I_b_Factor = self.create_line_edit(layout)
        self.I_b_Factor.setText("2.503047E-05")
        layout.addWidget(QLabel("C�������A��ϵ����"))
        self.I_c_Factor = self.create_line_edit(layout)
        self.I_c_Factor.setText("5.038112E-06")

        # ������λ������������Щ�����
        if self.phase_num == 1:
            self.U_b_Factor.setEnabled(False)
            self.U_c_Factor.setEnabled(False)
            self.I_b_Factor.setEnabled(False)
            self.I_c_Factor.setEnabled(False)
        # ���������ӵ�������
        self.main_h_layout.insertWidget(2, self.coefficient_widget)  # ������������Ԥ����λ��
    #ѡ���ļ���ť�ص�����
    def select_file(self):
        options = QFileDialog.Options()
        # �ļ��Ի�������
        file_path, _ = QFileDialog.getOpenFileName(
            parent=self,
            caption="ѡ�������ļ�",
            directory="",  # Ĭ�ϴ�·�������ַ�����ʾϵͳĬ�ϣ�
            filter="�����ļ� (*);;�ı��ļ� (*.txt);;CSV�ļ� (*.csv)",
            options=options
        )
        if file_path:  # �û�ѡ�����ļ���δȡ����
            self.data_file_path = file_path
            if self.data_file_path.lower().endswith(".txt"): #���������Ǵ��ڴ�ӡ�����txt�ļ�
                if  self.txt_to_bin_file(self.data_file_path) == -1:
                    self.data_file_path = None
                    return
            self.file_edit.setText(file_path)
            print("�ļ�·���Ѹ��£�", file_path)
        else:
            print("�ļ�ѡ����ȡ��")
    #��һ���ı����ȡϵ��
    def get_UI_factor(self,line):
        try:
            text =line.text()
            if text:
                return float(text)
            else:
                raise  ValueError
        except ValueError as e:
            QMessageBox.warning(self, "�������", "��������������Ƿ�Ϊ��Ч����")
    #���ı����ȡ����ϵ��
    def get_All_UIFactor(self):
        """�ռ��������벢ִ�м���"""
        try:
            selected_text = self.phase_select.currentText()
            if selected_text == '3':
                # ��ȡ�����ѹϵ��
                self.UIfactor[0] = self.get_UI_factor(self.U_a_Factor)
                self.UIfactor[1] = self.get_UI_factor(self.U_b_Factor)
                self.UIfactor[2] = self.get_UI_factor(self.U_c_Factor)
                # ��ȡ�������ϵ��
                self.UIfactor[3] = self.get_UI_factor(self.I_a_Factor)
                self.UIfactor[4] = self.get_UI_factor(self.I_b_Factor)
                self.UIfactor[5] = self.get_UI_factor(self.I_c_Factor)
            elif selected_text == '1':
                # ��ȡ�����ѹϵ��
                self.UIfactor[0] = self.get_UI_factor(self.U_a_Factor)
                # ��ȡ�������ϵ��
                self.UIfactor[3] = self.get_UI_factor(self.I_a_Factor)
            return self
        except ValueError as e:
            QMessageBox.warning(self, "�������", "��������������Ƿ�Ϊ��Ч����")

    #���߳�print���ղ�
    def printRedirect(self,str):
        self.text_show.insertPlainText(str)
        self.text_show.ensureCursorVisible()  # ȷ�����ɼ����ı���������ײ�
        print()
    #ѡ���ļ���ʾ��
    def choose_file_tip(self):
        msg = QMessageBox()
        msg.setIcon(QMessageBox.Information)  # ����ͼ������
        msg.setText("ѡ���ļ���")  # ������ʾ���ı�
        msg.setWindowTitle("��ʾ")  # ���õ�������
        msg.setStandardButtons(QMessageBox.Ok)
        msg.exec_()

    # ����plot
    def add_sub_plot(self, row, col, title, x_data,y_data, y_label,x_label):
        plot = self.plot_widget.addPlot(row=row, col=col, title=title)
        plot.plot(x_data, y_data, pen='r')
        plot.setMouseEnabled(x=True, y=False)
        # ���ú��������ͼ��
        plot.setLabel('bottom', x_label)
        plot.setLabel('left', y_label)
        # ��Ӵ�ֱ��ˮƽ������
        vLine = pg.InfiniteLine(angle=90, movable=False, pen=pg.mkPen('k', width=1))
        hLine = pg.InfiniteLine(angle=0, movable=False, pen=pg.mkPen('k', width=1))
        label = pg.TextItem(color="black")

        # �������ߺͱ�ǩ��ӵ�ͼ��
        plot.addItem(vLine, ignoreBounds=True)
        plot.addItem(hLine, ignoreBounds=True)
        plot.addItem(label)
        #
        # # ��ʼ����
        vLine.hide()
        hLine.hide()
        label.hide()

        # �洢���ֵ��У���ͼ��Ϊ��
        self.crosshairs[plot] = (vLine, hLine, label)
        return plot

    def onMouseMoved(self, pos):
        # ��������ͼ��ĸ����ߺͱ�ǩ
        for plot, (vLine, hLine, label) in self.crosshairs.items():
            vb = plot.vb  # ��ȡ��ͼ��
            vb.enableAutoRange(x=False, y=False)
            # �������Ƿ���ͼ��ĳ����߽���
            if vb.sceneBoundingRect().contains(pos):
                # ����������ת��Ϊ��������
                mousePoint = vb.mapSceneToView(pos)
                x = mousePoint.x()
                y = mousePoint.y()

                # ���¸�����λ��
                vLine.setPos(x)
                hLine.setPos(y)

                # �����ǩ��ƫ������������ͼ��Χ�� 5%��
                viewRange = vb.viewRect()
                delta_x = viewRange.width() * 0.05
                delta_y = viewRange.height() * 0.05

                # ���ñ�ǩλ�ú��ı�
                label.setPos(x + delta_x, y + delta_y)
                label.setText(f"x={x:.2f}, y={y:.2f}")

                # ��ʾ�����ߺͱ�ǩ
                vLine.show()
                hLine.show()
                label.show()
            else:
                # ���ظ����ߺͱ�ǩ
                vLine.hide()
                hLine.hide()
                label.hide()
    # ���Ʋ���ͼ
    def add_plot_to_widget(self):
        self.plot_widget.setWindowTitle('PyQtGraph �����ͼʾ��')
        self.plot_widget.setBackground((0, 0, 0, 0))  # ����Ϊ��ȫ͸������
        if self.phase_num == 1:
            Cycle_x_values = self.plot_X_data[::SAMPLE_NUM]

            # ʹ����ȡ�ĺ�����������ͼ
            self.add_sub_plot(0, 0, "UA��ԭʼ�ܲ����ݣ�", self.plot_X_data, self.plot_UA_data, 'Voltage (V)','time(ms)')
            self.add_sub_plot(0, 1, "IA��ԭʼ�ܲ����ݣ�", self.plot_X_data, self.plot_IA_data, 'Current (A)','time(ms)')
            self.add_sub_plot(1, 0, "UA-RMS����Чֵms����", Cycle_x_values, self.plot_UA_rms_data, 'Voltage-RMS (V)','time(ms)')
            self.add_sub_plot(1, 1, "IA-RMS����Чֵms����", Cycle_x_values, self.plot_IA_rms_data, 'Current-RMS (A)','time(ms)')

            X_sec = range(0, len(self.plot_P_sec_level))
            print("��ͼ����")
            print(len(X_sec))
            # ����뼶��ͼ
            self.add_sub_plot(2, 0, "UA-RMS (�뼶����)", X_sec, self.plot_U_sec_level, 'Voltage-RMS /50 (V)','time(s)')
            self.add_sub_plot(2, 1, "IA-RMS (�뼶����)", X_sec, self.plot_I_sec_level, 'Current-RMS /50 (V)','time(s)')
            self.add_sub_plot(3, 0, "P���ܲ����ݣ�", Cycle_x_values, self.plot_ActPwr_data, 'P (W)', 'time(ms)')
            self.add_sub_plot(3, 1, "P���뼶���ݣ�", X_sec, self.plot_P_sec_level, 'P (W)','time(s)')

            # folder = "subplots"
            # os.makedirs(folder, exist_ok=True)
            # # ɾ��Ŀ¼�е������ļ�
            # for filename in os.listdir(folder):
            #     file_path = os.path.join(folder, filename)
            #     if os.path.isfile(file_path):
            #         os.remove(file_path)
            # i = 0
            #
            #
            # # ���� self.plot_widget �� GraphicsLayoutWidget
            # i = 1  # ��ͼ���
            # folder = "output_folder"  # �滻Ϊ�������ļ���·��
            #
            # # ȷ������ļ��д���
            # if not os.path.exists(folder):
            #     os.makedirs(folder)
            #
            # for item in self.plot_widget.items():
            #     if isinstance(item, pg.PlotItem):
            #         # ���ð׵׺��ߣ������������������Ϊ��ɫ������
            #         item.getViewBox().setBackgroundColor('w')  # ��ɫ����
            #         for curve in item.listDataItems():
            #             curve.setPen(pg.mkPen('r', width=2))  # ��ɫ���ߣ��߿�2
            #
            #         # ��ʼ�� ImageExporter���󶨵���ͼ�ĳ���
            #         exporter = ImageExporter(item)  # ֱ��ʹ�� PlotItem�������� item.scene()
            #
            #         # ���õ�������
            #         exporter.parameters()['width'] = 2000  # ���ÿ�ȣ��߶��Զ�����������
            #
            #         # ������ͼ
            #         filename = os.path.join(folder, f"subplot_{i}.png")
            #         exporter.export(filename)
            #         print(f"������{filename}")
            #         i += 1
            print()
            print("�뼶����ǰ��ʮ����")
            print([f"{float(x):.5f}" for x in self.plot_P_sec_level[:30]])
            print()
            print("�뼶��ѹǰ��ʮ����")
            print([f"{float(x):.5f}" for x in self.plot_U_sec_level[:30]])
            print()
            print("�뼶����ǰ��ʮ����")
            print([f"{float(x):.5f}" for x in self.plot_I_sec_level[:30]])
        elif self.phase_num == 3:
            # ʹ����ȡ�ĺ����������������ݵ���ͼ
            self.add_sub_plot(0, 0, "UA", self.plot_X_data,self.plot_UA_data, 'Voltage (V)','time(ms)')
            self.add_sub_plot(1, 0, "IA", self.plot_X_data,self.plot_IA_data, 'Current (A)','time(ms)')
            self.add_sub_plot(0, 1, "UB", self.plot_X_data,self.plot_UB_data, 'Voltage (V)','time(ms)')
            self.add_sub_plot(1, 1, "IB", self.plot_X_data,self.plot_IB_data, 'Current (A)','time(ms)')
            self.add_sub_plot(0, 2, "UC", self.plot_X_data,self.plot_UC_data, 'Voltage (V)','time(ms)')
            self.add_sub_plot(1, 2, "IC", self.plot_X_data,self.plot_IC_data, 'Current (A)','time(ms)')

    #�������ֵ��ʾ
    def update_max_values(self,rms_Umax,rms_Imax):
        self.max_Ulabel.clear()
        self.max_Ilabel.clear()
        self.max_Ulabel.setText("��ѹ��Чֵ���ֵ" + str(rms_Umax))  # �����ı���ӵ�����
        self.max_Ilabel.setText("������Чֵ���ֵ" + str(rms_Imax))


if __name__ == '__main__':
    app = QApplication(sys.argv)
    ex = WaveDataProcessUI()
    ex.show()
    x = app.exec_()
    sys.exit(x)
