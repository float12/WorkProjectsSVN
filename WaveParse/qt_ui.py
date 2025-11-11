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
#print重定向
class RedirectTextEdit(QTextEdit):
    def __init__(self):
        super().__init__()

    def write(self, text):
        """重定向输出到 QTextEdit 控件"""
        self.insertPlainText(text)
        self.ensureCursorVisible()  # 确保光标可见，文本框滚动到底部
        self.repaint()  # 立即重绘控件
def is_valid_hex(s):
    """检查字符串是否是有效的十六进制数值。"""
    try:
        # 尝试将字符串转为整数，只有合法的十六进制数值才能转换成功
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
        print("转换文件为bin---")
        binary_data =[]
        #去除第一行
        with open(txt_filepath, 'r') as infile:
            lines = infile.readlines()

        with open(txt_filepath, 'w') as outfile:
            outfile.writelines(lines[1:])
        try:
            with open(txt_filepath, 'r') as infile:
                hex_content = infile.read()
        except FileNotFoundError:
            print(f"错误: 文件未找到: {txt_filepath}")
            return
        hex_numbers = hex_content.replace(" ", "").strip()
        if len(hex_numbers) % 2 != 0:
            raise ValueError("Hexadecimal input must have an even length")
        try:
            for i in range(0, len(hex_numbers), 2):
                byte_val_str = hex_numbers[i:i + 2]
                # 检查当前字符对是否是有效的十六进制数值
                if  is_valid_hex(byte_val_str):
                    tmp = int(byte_val_str, 16)
                    binary_data.append(tmp)
                else:
                    print(f"警告：存在无效的十六进制数值 '{byte_val_str}'，检查文件内容")
                    return  -1
        except binascii.Error as e:
            print(f"错误: 无效的十六进制数据: {e}")
            return
        base, ext = os.path.splitext(txt_filepath)
        self.data_file_path = base + ".bin"
        with open(self.data_file_path, 'wb') as f:
            f.write(bytes(binary_data))
        print(f"十六进制数据已转换为二进制文件: {self.data_file_path}")

    def initUI(self):
        self.data_file_path = None
        self.UIfactor = [0, 0, 0, 0, 0, 0]
        # 主布局改为水平布局
        self.main_h_layout = QHBoxLayout()  # 替换原来的 self.main_layout = QVBoxLayout()

        # 左侧垂直布局容器（原main_layout改名为left_layout）
        self.left_layout = QVBoxLayout()


        # 相位选择
        self.phase_select = QComboBox()
        self.phase_select.addItems([str(1), str(3)])
        self.phase_select.setCurrentIndex(0)  # 默认选择一相（1）
        self.left_layout.addWidget(QLabel("选择相数:"))
        self.left_layout.addWidget(self.phase_select)
       
        # 格式选择
        self.format_select = QComboBox()
        self.format_select.addItems(["", str("wlb_waveRecord_with_NO"), str("698"),str("wlb_waveRecord")])
        self.format_select.setCurrentIndex(2)  # 默认选择 "698"
        self.left_layout.addWidget(QLabel("选择数据格式:"))
        self.left_layout.addWidget(self.format_select)
        # 文件选择
        file_layout = QHBoxLayout()
        self.file_edit = QLineEdit()
        self.file_button = QPushButton("选择波形解析bin文件")
        file_layout.addWidget(self.file_edit)
        file_layout.addWidget(self.file_button)
        self.left_layout.addLayout(file_layout)
        # 波形数据大小端选择
        self.endian_select = QComboBox()
        self.endian_select.addItems(["big", "little"])
        self.endian_select.setCurrentIndex(0)  # 默认选择大端
        self.left_layout.addWidget(QLabel("波形数据大小端:"))
        self.left_layout.addWidget(self.endian_select)
        # 增加文本输入框
        self.text_input_label = QLabel("输入格式转换文件目录或文件：")
        self.text_input = QLineEdit()
        self.left_layout.addWidget(self.text_input_label)
        self.left_layout.addWidget(self.text_input)
        # 系数输入区
        self.coeff_container = QWidget()
        self.coeff_layout = QGridLayout()
        self.coeff_container.setLayout(self.coeff_layout)
        self.left_layout.addWidget(self.coeff_container)
        # 勾选框：是否删除系数帧
        # self.delete_coeff_frame_checkbox = QCheckBox("是否删除系数帧")
        # self.delete_coeff_frame_checkbox.setChecked(True)# 默认勾选
        # self.left_layout.addWidget(self.delete_coeff_frame_checkbox)
        # 操作按钮
        btn_layout = QHBoxLayout()
        self.save_btn = QPushButton("保存数据")
        self.plot_btn = QPushButton("绘制图形")
        self.ConvertTo1P = QPushButton("三相转成单相")
        self.convert_action_select = QComboBox()
        self.convert_action_select.addItems([
            "带序号录波格式->数据传输格式",
            "698->数据传输格式"
        ])
        self.convert_action_select.setCurrentIndex(0)  # 默认选择大端
        self.left_layout.addWidget(QLabel("转换格式选择:"))
        self.left_layout.addWidget(self.convert_action_select)
        self.ConvertToDataTransmit = QPushButton("格式转换")
        btn_layout.addWidget(self.ConvertToDataTransmit)
        btn_layout.addWidget(self.save_btn)
        btn_layout.addWidget(self.plot_btn)
        btn_layout.addWidget(self.ConvertTo1P)
        
        self.left_layout.addLayout(btn_layout)

        # 最大值显示
        self.max_Ulabel = QLabel("电压有效值最大值：")
        self.left_layout.addWidget(self.max_Ulabel)
        self.max_Ilabel = QLabel("电流有效值最大值：")
        self.left_layout.addWidget(self.max_Ilabel)

        # 控制台输出区域
        self.text_show = RedirectTextEdit()
        self.text_show.setReadOnly(False)
        self.left_layout.addWidget(self.text_show)

        # 波形显示（注意这里不再添加到left_layout）
        self.plot_widget = pg.GraphicsLayoutWidget()
        self.plot_widget.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)  # 设置扩展策略
        self.plot_widget.setBackground("w")
        # 初始化存储辅助线和标签的字典
        self.crosshairs = {}
        # 连接场景的鼠标移动信号
        self.plot_widget.scene().sigMouseMoved.connect(self.onMouseMoved)

        # 将左侧布局和波形显示添加到主水平布局
        self.main_h_layout.addLayout(self.left_layout, 1)  # 左侧占1份宽度
        self.main_h_layout.addWidget(self.plot_widget, 2)  # 右侧占2份宽度（可按需调整比例）

        # 设置窗口的主布局
        self.setLayout(self.main_h_layout)
        self.resize(2000, 600)

        # 初始化系数输入
        self.update_ui_based_on_phase()
        self.setLayout(self.main_h_layout)
        self.setWindowTitle('波形数据解析')

        # 信号连接
        self.phase_select.currentIndexChanged.connect(self.update_ui_based_on_phase)
        self.file_button.clicked.connect(self.select_file)
        self.save_btn.clicked.connect(self.save_to_csv)
        self.plot_btn.clicked.connect(self.plot_data)
        self.ConvertTo1P.clicked.connect(self.convert_to_1p)
        self.ConvertToDataTransmit.clicked.connect(self.convert_to_data_transmit)
        self.format_select.currentIndexChanged.connect(self.update_format)
        #重定向print到文本框
        sys.stdout = self.text_show
    #转成单相按钮回调函数
    def convert_to_1p(self):
        if self.data_file_path != None:
            converter = phaseNumConverter(mode="3p_to_1p", chunk_size=782,input_3p_file =self.data_file_path)
            converter.run()
        else:
            print("请选择文件")
    #转成数据传输格式按钮回调函数
    def convert_to_data_transmit(self):
        data_file_path = self.text_input.text()
        if data_file_path != None:
            if os.path.isfile(data_file_path):
                # 输入的是单个文件
                bin_files = [data_file_path]
            elif os.path.isdir(data_file_path):
                # 输入的是目录，遍历该目录下所有bin文件
                bin_files = glob.glob(os.path.join(data_file_path, "*.bin"))
            else:
                print("请输入正确的文件路径或目录")
                return
            output_dir = pathlib.Path("wlb_waveRecord_output")
            converter = FormatConverter()
            for input_file in bin_files:
                input_file = pathlib.Path(input_file)  # 转成 Path 对象
                output_dir = pathlib.Path("wlb_WaveRecord_output")
                output_file = FormatConverter.ensure_output_folder_and_unique_file(output_dir,input_file.name.replace(".bin", "_wlb_waveRecord.bin"))
                if self.convert_action_select.currentIndex() == 0:
                    converter.delete_wlbWaveRecord_NO(input_file, output_file, 782, 778)
                elif self.convert_action_select.currentIndex() == 1:
                    converter.convert_698_to_wlb_waveRecord(input_file, output_file, 815, 778)
                else:
                    print("请选择正确的转换动作")
                    return
                # 提取 output_file 的文件名（去掉上级目录），再去掉 "_wlb_waveRecord"
                clean_stem = pathlib.Path(output_file).name.replace("_wlb_waveRecord", "").replace(".bin", "")
                output_dir = pathlib.Path("wlb_Transmit_output")
                transimit_output_file = FormatConverter.ensure_output_folder_and_unique_file(output_dir,clean_stem + "_wlb_Transmit.bin")
                # print(f"转换 {output_file} → {transimit_output_file}")
                converter.convert_record_to_transmit(output_file, transimit_output_file, 778, 778)
                # INSERT_YOUR_CODE
                if os.path.exists(output_file):
                    os.remove(output_file)
            print("转换结束")
        else:
            print("请选择文件")

    #保存数据按钮回调函数
    def save_to_csv(self):
        if self.data_file_path != None:
            self.get_All_UIFactor()
            self.endian = self.endian_select.currentText()  # 获取当前选择的端序字符串（"big" 或 "little"）
            # self.setEnabled(False)  # 禁用窗口的交互
            self.text_show.clear()
            self.update_format()
            self.save_thread = SaveToCsvTheard(self.phase_num, self.format_type,self.data_file_path, self.UIfactor, self.plot_widget,self.endian)  # 创建一个线程
            self.save_thread.SaveSignal.connect(self.save_finish)  # 线程发过来的信号挂接到槽函数update_sum
            self.save_thread.Csv_Saver.frame_parser.PrintSignal.connect(self.printRedirect)
            self.save_thread.start()  # 线程启动
            self.setEnabled(False)
            print("start save")
            print(time.time())
        else:
            self.choose_file_tip()

    #保存到excel执行完成回调函数
    def save_finish(self,RMS_umax,RMS_imax,res):
        self.setEnabled(True)
        print(time.time())
        if res != 0:#解析数据失败
            print("解析失败")
            return
        print("保存完成")
        self.update_max_values(RMS_umax,RMS_imax)

    # 绘制波形按钮回调函数
    def plot_data(self):
        if self.data_file_path != None:
            self.get_All_UIFactor()  # 获取系数
            self.endian = self.endian_select.currentText()  # 获取当前选择的端序字符串（"big" 或 "little"）
            self.plot_widget.clear()  # 清屏
            self.text_show.clear()
            self.setEnabled(False)  # 禁用窗口的交互
            self.update_format()
            # if self.delete_coeff_frame_checkbox.isChecked():
            #     try:
            #         with open(self.data_file_path, 'rb') as f:
            #             data = f.read(53)
            #             if data[0] == 0x68 and data[52] == 0x16:#提取系数
            #                 float1_bytes = bytes(data[39:43])
            #                 float2_bytes = bytes(data[44:48])
            #                 self.UIfactor[0] = struct.unpack('>f', float1_bytes)[0]
            #                 self.UIfactor[3] = struct.unpack('>f', float2_bytes)[0]
            #                 data = f.read()
            #             else:
            #                 raise ValueError("无系数帧")
            #         with open(self.data_file_path, 'wb') as f:
            #             f.write(data)
            #         print("已删除文件前53字节（系数帧）")
            #     except Exception as e:
            #         print(f"删除系数帧失败: {e}")
            self.plot_data_cal_thread = PlotDataTheard.PlotTheard(self.phase_num, self.format_type,self.data_file_path,self.UIfactor,self.endian)  # 创建一个线程
            self.plot_data_cal_thread.plot_signal.connect(self.plot_data_cal_finish)  # 线程发过来的信号挂接到槽函数update_sum
            self.plot_data_cal_thread.Wave_Plotter.frame_parser.PrintSignal.connect(self.printRedirect)
            self.plot_data_cal_thread.start()  # 线程启动
            # print(time.time())
        else:
            self.choose_file_tip()

    def print_average_every_100(self,label, data_list, max_group_count = 50):
        """
        每100个数据点计算一次平均值，并打印前 max_group_count 个平均值。

        :param label: 字符串标签（如 "有功功率", "I RMS", "U RMS"）
        :param data_list: 要处理的数据列表
        :param max_group_count: 最多打印多少组（默认 50）
        """
        print()
        print(f"每50周波平均 {label} 前 {max_group_count} 点")

        count = 0
        data_len = len(data_list)
        for i in range(0, data_len, 50):
            if count >= max_group_count:
                break
            end = min(i + 50, data_len)
            segment = data_list[i:end]
            # 如果 segment 是空的，跳过
            if len(segment) == 0:
                continue
            # 兼容 list 或 NumPy array 取均值
            avg = float(np.mean(segment))
            print(f"{avg:.3f}", end=" ")
            count += 1
     # 绘制完成回调函数
    def plot_data_cal_finish(self,wavePlotter,res):
        if res != 0:#解析数据失败
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
             # 存储波动大的值（包括其索引）
             fluctuation_data_Irms = []
             # 存储波动大的值（包括其索引）
             fluctuation_data_Urms = []
             # 计算每100个值的平均值
             i_len = len(self.plot_IA_rms_data)
             u_len = len(self.plot_UA_rms_data)
             p_len = len(wavePlotter.plot_ActPwr_data)
             self.print_average_every_100("视在功率", wavePlotter.plot_AppPwr_data)
             self.print_average_every_100("有功功率", wavePlotter.plot_ActPwr_data)
             self.print_average_every_100("无功功率", wavePlotter.plot_ReactPwr_data)
             self.print_average_every_100("I RMS", self.plot_IA_rms_data)
             self.print_average_every_100("U RMS", self.plot_UA_rms_data)
             # 处理功率数据
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
        print("绘制完成")
        self.update_max_values(wavePlotter.frame_parser.rms_Umax,wavePlotter.frame_parser.rms_Imax)
        self.setEnabled(True)
    #创建文本框
    def create_line_edit(self,layout):
        line = QLineEdit()
        line.setText(str(0.00026))
        layout.addWidget(line)
        return  line
    # 根据格式更新变量
    def update_format(self):
        if str(self.format_select.currentText()) == "wlb_waveRecord_with_NO":
            self.format_type = 0
        elif str(self.format_select.currentText()) == "698":
            self.format_type = 1
        elif str(self.format_select.currentText()) == "wlb_waveRecord":
            self.format_type = 2
    # 根据相数更新ui
    def update_ui_based_on_phase(self):
        # 获取当前选择的相位
        self.phase_num = int(self.phase_select.currentText())

        # 清除旧的系数输入组件
        if hasattr(self, 'coefficient_widget'):
            self.coefficient_widget.deleteLater()

        # 创建新的容器组件和布局
        self.coefficient_widget = QWidget()
        layout = QVBoxLayout(self.coefficient_widget)

        # 生成所有输入项
        layout.addWidget(QLabel("A相电压（V）系数："))
        self.U_a_Factor = self.create_line_edit(layout)
        self.U_a_Factor.setText("3.7068474922E-4")
        layout.addWidget(QLabel("B相电压（V）系数："))
        self.U_b_Factor = self.create_line_edit(layout)
        self.U_b_Factor.setText("0.0002673594")
        layout.addWidget(QLabel("C相电压（V）系数："))
        self.U_c_Factor = self.create_line_edit(layout)

        layout.addWidget(QLabel("A相电流（A）系数："))
        self.I_a_Factor = self.create_line_edit(layout)
        self.I_a_Factor.setText("1.00476572E-5")
        layout.addWidget(QLabel("B相电流（A）系数："))
        self.I_b_Factor = self.create_line_edit(layout)
        self.I_b_Factor.setText("2.503047E-05")
        layout.addWidget(QLabel("C相电流（A）系数："))
        self.I_c_Factor = self.create_line_edit(layout)
        self.I_c_Factor.setText("5.038112E-06")

        # 根据相位数决定启用哪些输入框
        if self.phase_num == 1:
            self.U_b_Factor.setEnabled(False)
            self.U_c_Factor.setEnabled(False)
            self.I_b_Factor.setEnabled(False)
            self.I_c_Factor.setEnabled(False)
        # 将新组件添加到主界面
        self.main_h_layout.insertWidget(2, self.coefficient_widget)  # 假设主布局中预留了位置
    #选择文件按钮回调函数
    def select_file(self):
        options = QFileDialog.Options()
        # 文件对话框配置
        file_path, _ = QFileDialog.getOpenFileName(
            parent=self,
            caption="选择输入文件",
            directory="",  # 默认打开路径（空字符串表示系统默认）
            filter="所有文件 (*);;文本文件 (*.txt);;CSV文件 (*.csv)",
            options=options
        )
        if file_path:  # 用户选择了文件（未取消）
            self.data_file_path = file_path
            if self.data_file_path.lower().endswith(".txt"): #如果输入的是串口打印输出的txt文件
                if  self.txt_to_bin_file(self.data_file_path) == -1:
                    self.data_file_path = None
                    return
            self.file_edit.setText(file_path)
            print("文件路径已更新：", file_path)
        else:
            print("文件选择已取消")
    #从一个文本框获取系数
    def get_UI_factor(self,line):
        try:
            text =line.text()
            if text:
                return float(text)
            else:
                raise  ValueError
        except ValueError as e:
            QMessageBox.warning(self, "输入错误", "请检查所有输入框是否为有效数字")
    #从文本框获取所有系数
    def get_All_UIFactor(self):
        """收集所有输入并执行计算"""
        try:
            selected_text = self.phase_select.currentText()
            if selected_text == '3':
                # 获取三相电压系数
                self.UIfactor[0] = self.get_UI_factor(self.U_a_Factor)
                self.UIfactor[1] = self.get_UI_factor(self.U_b_Factor)
                self.UIfactor[2] = self.get_UI_factor(self.U_c_Factor)
                # 获取三相电流系数
                self.UIfactor[3] = self.get_UI_factor(self.I_a_Factor)
                self.UIfactor[4] = self.get_UI_factor(self.I_b_Factor)
                self.UIfactor[5] = self.get_UI_factor(self.I_c_Factor)
            elif selected_text == '1':
                # 获取单相电压系数
                self.UIfactor[0] = self.get_UI_factor(self.U_a_Factor)
                # 获取单相电流系数
                self.UIfactor[3] = self.get_UI_factor(self.I_a_Factor)
            return self
        except ValueError as e:
            QMessageBox.warning(self, "输入错误", "请检查所有输入框是否为有效数字")

    #子线程print接收槽
    def printRedirect(self,str):
        self.text_show.insertPlainText(str)
        self.text_show.ensureCursorVisible()  # 确保光标可见，文本框滚动到底部
        print()
    #选择文件提示框
    def choose_file_tip(self):
        msg = QMessageBox()
        msg.setIcon(QMessageBox.Information)  # 设置图标类型
        msg.setText("选择文件！")  # 设置显示的文本
        msg.setWindowTitle("提示")  # 设置弹窗标题
        msg.setStandardButtons(QMessageBox.Ok)
        msg.exec_()

    # 增加plot
    def add_sub_plot(self, row, col, title, x_data,y_data, y_label,x_label):
        plot = self.plot_widget.addPlot(row=row, col=col, title=title)
        plot.plot(x_data, y_data, pen='r')
        plot.setMouseEnabled(x=True, y=False)
        # 设置横纵坐标的图例
        plot.setLabel('bottom', x_label)
        plot.setLabel('left', y_label)
        # 添加垂直和水平辅助线
        vLine = pg.InfiniteLine(angle=90, movable=False, pen=pg.mkPen('k', width=1))
        hLine = pg.InfiniteLine(angle=0, movable=False, pen=pg.mkPen('k', width=1))
        label = pg.TextItem(color="black")

        # 将辅助线和标签添加到图表
        plot.addItem(vLine, ignoreBounds=True)
        plot.addItem(hLine, ignoreBounds=True)
        plot.addItem(label)
        #
        # # 初始隐藏
        vLine.hide()
        hLine.hide()
        label.hide()

        # 存储到字典中，以图表为键
        self.crosshairs[plot] = (vLine, hLine, label)
        return plot

    def onMouseMoved(self, pos):
        # 遍历所有图表的辅助线和标签
        for plot, (vLine, hLine, label) in self.crosshairs.items():
            vb = plot.vb  # 获取视图框
            vb.enableAutoRange(x=False, y=False)
            # 检查鼠标是否在图表的场景边界内
            if vb.sceneBoundingRect().contains(pos):
                # 将场景坐标转换为数据坐标
                mousePoint = vb.mapSceneToView(pos)
                x = mousePoint.x()
                y = mousePoint.y()

                # 更新辅助线位置
                vLine.setPos(x)
                hLine.setPos(y)

                # 计算标签的偏移量（基于视图范围的 5%）
                viewRange = vb.viewRect()
                delta_x = viewRange.width() * 0.05
                delta_y = viewRange.height() * 0.05

                # 设置标签位置和文本
                label.setPos(x + delta_x, y + delta_y)
                label.setText(f"x={x:.2f}, y={y:.2f}")

                # 显示辅助线和标签
                vLine.show()
                hLine.show()
                label.show()
            else:
                # 隐藏辅助线和标签
                vLine.hide()
                hLine.hide()
                label.hide()
    # 绘制波形图
    def add_plot_to_widget(self):
        self.plot_widget.setWindowTitle('PyQtGraph 多个子图示例')
        self.plot_widget.setBackground((0, 0, 0, 0))  # 设置为完全透明背景
        if self.phase_num == 1:
            Cycle_x_values = self.plot_X_data[::SAMPLE_NUM]

            # 使用提取的函数来创建子图
            self.add_sub_plot(0, 0, "UA（原始周波数据）", self.plot_X_data, self.plot_UA_data, 'Voltage (V)','time(ms)')
            self.add_sub_plot(0, 1, "IA（原始周波数据）", self.plot_X_data, self.plot_IA_data, 'Current (A)','time(ms)')
            self.add_sub_plot(1, 0, "UA-RMS（有效值ms级）", Cycle_x_values, self.plot_UA_rms_data, 'Voltage-RMS (V)','time(ms)')
            self.add_sub_plot(1, 1, "IA-RMS（有效值ms级）", Cycle_x_values, self.plot_IA_rms_data, 'Current-RMS (A)','time(ms)')

            X_sec = range(0, len(self.plot_P_sec_level))
            print("绘图秒数")
            print(len(X_sec))
            # 添加秒级子图
            self.add_sub_plot(2, 0, "UA-RMS (秒级数据)", X_sec, self.plot_U_sec_level, 'Voltage-RMS /50 (V)','time(s)')
            self.add_sub_plot(2, 1, "IA-RMS (秒级数据)", X_sec, self.plot_I_sec_level, 'Current-RMS /50 (V)','time(s)')
            self.add_sub_plot(3, 0, "P（周波数据）", Cycle_x_values, self.plot_ActPwr_data, 'P (W)', 'time(ms)')
            self.add_sub_plot(3, 1, "P（秒级数据）", X_sec, self.plot_P_sec_level, 'P (W)','time(s)')

            # folder = "subplots"
            # os.makedirs(folder, exist_ok=True)
            # # 删除目录中的所有文件
            # for filename in os.listdir(folder):
            #     file_path = os.path.join(folder, filename)
            #     if os.path.isfile(file_path):
            #         os.remove(file_path)
            # i = 0
            #
            #
            # # 假设 self.plot_widget 是 GraphicsLayoutWidget
            # i = 1  # 子图编号
            # folder = "output_folder"  # 替换为你的输出文件夹路径
            #
            # # 确保输出文件夹存在
            # if not os.path.exists(folder):
            #     os.makedirs(folder)
            #
            # for item in self.plot_widget.items():
            #     if isinstance(item, pg.PlotItem):
            #         # 设置白底红线（根据你的描述，修正为白色背景）
            #         item.getViewBox().setBackgroundColor('w')  # 白色背景
            #         for curve in item.listDataItems():
            #             curve.setPen(pg.mkPen('r', width=2))  # 红色曲线，线宽2
            #
            #         # 初始化 ImageExporter，绑定到子图的场景
            #         exporter = ImageExporter(item)  # 直接使用 PlotItem，而不是 item.scene()
            #
            #         # 设置导出参数
            #         exporter.parameters()['width'] = 2000  # 设置宽度，高度自动按比例缩放
            #
            #         # 导出子图
            #         filename = os.path.join(folder, f"subplot_{i}.png")
            #         exporter.export(filename)
            #         print(f"导出：{filename}")
            #         i += 1
            print()
            print("秒级功率前四十个点")
            print([f"{float(x):.5f}" for x in self.plot_P_sec_level[:30]])
            print()
            print("秒级电压前四十个点")
            print([f"{float(x):.5f}" for x in self.plot_U_sec_level[:30]])
            print()
            print("秒级电流前四十个点")
            print([f"{float(x):.5f}" for x in self.plot_I_sec_level[:30]])
        elif self.phase_num == 3:
            # 使用提取的函数来创建三相数据的子图
            self.add_sub_plot(0, 0, "UA", self.plot_X_data,self.plot_UA_data, 'Voltage (V)','time(ms)')
            self.add_sub_plot(1, 0, "IA", self.plot_X_data,self.plot_IA_data, 'Current (A)','time(ms)')
            self.add_sub_plot(0, 1, "UB", self.plot_X_data,self.plot_UB_data, 'Voltage (V)','time(ms)')
            self.add_sub_plot(1, 1, "IB", self.plot_X_data,self.plot_IB_data, 'Current (A)','time(ms)')
            self.add_sub_plot(0, 2, "UC", self.plot_X_data,self.plot_UC_data, 'Voltage (V)','time(ms)')
            self.add_sub_plot(1, 2, "IC", self.plot_X_data,self.plot_IC_data, 'Current (A)','time(ms)')

    #更新最大值显示
    def update_max_values(self,rms_Umax,rms_Imax):
        self.max_Ulabel.clear()
        self.max_Ilabel.clear()
        self.max_Ulabel.setText("电压有效值最大值" + str(rms_Umax))  # 将新文本添加到后面
        self.max_Ilabel.setText("电流有效值最大值" + str(rms_Imax))


if __name__ == '__main__':
    app = QApplication(sys.argv)
    ex = WaveDataProcessUI()
    ex.show()
    x = app.exec_()
    sys.exit(x)
