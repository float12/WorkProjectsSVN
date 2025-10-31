# -*- coding: gbk -*-
import re
from datetime import datetime
from collections import deque
import pandas as pd
import matplotlib.pyplot as plt
import openpyxl
import sys
from PyQt5.QtWidgets import QComboBox, QHBoxLayout, QWidget
from PyQt5.QtWidgets import (
    QApplication, QWidget, QVBoxLayout, QPushButton, QFileDialog, QLabel, QMessageBox
)
from PyQt5.QtCore import Qt

def diff_ms(dt1: datetime, dt2: datetime) -> int:
    """
    计算两个 datetime 的毫秒差 (dt2 - dt1)
    返回整数毫秒
    """
    delta = dt2 - dt1
    return int(delta.total_seconds() * 1000)

class LogAnalyzer:
    def __init__(self, log_path):
        self.log_path = log_path
        self.entries = []
        self.matched = []
        self.no_reply = []
        self.unmatched_recvs = []
        self.df = pd.DataFrame()
        self.send_flag = 0
        self.timeGap = []
        self.No_reply_time = []
        self.send_time = None

    def parse_log(self, time_format="fmt1"):
        """
        解析日志，支持三种格式，通过time_format参数区分:
        fmt1: [15:53:42.790]发→◇.../收←◆...
        fmt2: TX:.../RX:...
        fmt3: [2025-09-01 16:08:03.252]# SEND HEX.../RECV HEX...
        """
        self.timeGap.clear()
        self.No_reply_time.clear()
        self.send_flag = 0
        self.send_time = None

        if time_format == "fmt1":
            # 第一种格式
            time_re = re.compile(r'\[(\d{2}:\d{2}:\d{2}\.\d{3})\]')
            with open(self.log_path, 'r', encoding='gbk') as f:
                for i, line in enumerate(f, 1):
                    m = time_re.search(line)
                    if not m:
                        continue
                    timestr = m.group(1)
                    today = datetime.now().date()
                    dt = datetime.strptime(f"{timestr}", '%H:%M:%S.%f')
                    tail = line[m.end():]
                    if '发' in tail and self.send_flag == 0:
                        self.send_flag = 1
                        self.send_time = dt
                    elif '收' in tail and self.send_flag == 1:
                        self.send_flag = 0
                        self.timeGap.append(diff_ms(self.send_time, dt))
                    elif '发' in tail and self.send_flag == 1:
                        print(f"send without response, time {self.send_time}")
                        self.send_time = dt
                        self.No_reply_time.append(self.send_time)
                        continue
                    else:
                        continue

        elif time_format == "fmt2":
            # 第二种格式
            # 例：17:21:37:419 RX:FE FE FE FE ...
            time_re = re.compile(r'(\d{2}:\d{2}:\d{2}:\d{3})\s+(TX|RX):')
            with open(self.log_path, 'r', encoding='gbk') as f:
                for i, line in enumerate(f, 1):
                    m = time_re.search(line)
                    if not m:
                        continue
                    timestr, direction = m.group(1), m.group(2)
                    today = datetime.now().date()
                    # 17:21:37:419 → %H:%M:%S:%f
                    dt = datetime.strptime(f"{today} {timestr}", '%Y-%m-%d %H:%M:%S:%f')
                    if direction == "TX" and self.send_flag == 0:
                        self.send_flag = 1
                        self.send_time = dt
                    elif direction == "RX" and self.send_flag == 1:
                        self.send_flag = 0
                        self.timeGap.append(diff_ms(self.send_time, dt))
                    elif direction == "TX" and self.send_flag == 1:
                        print(f"send without response, time {self.send_time}")
                        self.send_time = dt
                        self.No_reply_time.append(dt)
                        continue
                    else:
                        continue

        elif time_format == "fmt3":
            # 第三种格式
            # [2025-09-01 16:08:03.252]# SEND HEX/12 >>>
            # [2025-09-01 16:08:03.314]# RECV HEX/109 from SERVER <<<
            time_re = re.compile(r'\[(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}\.\d{3})\].*# (SEND|RECV) HEX')
            with open(self.log_path, 'r', encoding='gbk') as f:
                for i, line in enumerate(f, 1):
                    m = time_re.search(line)
                    if not m:
                        continue
                    timestr, direction = m.group(1), m.group(2)
                    dt = datetime.strptime(timestr, '%Y-%m-%d %H:%M:%S.%f')
                    if direction == "SEND" and self.send_flag == 0:
                        self.send_flag = 1
                        self.send_time = dt
                    elif direction == "RECV" and self.send_flag == 1:
                        self.send_flag = 0
                        self.timeGap.append(diff_ms(self.send_time, dt))
                    elif direction == "SEND" and self.send_flag == 1:
                        print(f"send without response, time {self.send_time}")
                        self.send_time = dt
                        self.No_reply_time.append(dt)
                        continue
                    else:
                        continue

        else:
            print("不支持的日志格式参数，请选择fmt1/fmt2/fmt3")

    def export_excel(self, path: str):
        """将 timeGap 列表导出到 Excel，每一列一行一个数据，并将 No_reply_time 导出到另一个sheet"""
        wb = openpyxl.Workbook()
        ws = wb.active
        ws.title = "Intervals"
        ws.cell(row=1, column=1, value="interval_ms")  # 表头

        for i, val in enumerate(self.timeGap, start=2):
            ws.cell(row=i, column=1, value=val)

        # 新建一个sheet用于无回复时间点
        ws2 = wb.create_sheet(title="无回复时间点")
        ws2.cell(row=1, column=1, value="no_reply_time")  # 表头
        for i, dt in enumerate(self.No_reply_time, start=2):
            # 格式化为字符串写入
            ws2.cell(row=i, column=1, value=str(dt))

        wb.save(path)
        print(f"Excel 已保存: {path}")

    def plot_intervals(self, path):
        plt.figure(figsize=(8, 4))
        x = [i for i in range(len(self.timeGap))]
        plt.plot(x, self.timeGap, marker='o')
        plt.xlabel('send_time')
        plt.ylabel('interval (ms)')
        plt.title('Send→Recv intervals')
        plt.xticks(rotation=45)
        plt.tight_layout()
        plt.savefig(path)
        plt.show()

class MainWindow(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("TCP日志解析工具")
        self.setGeometry(300, 300, 400, 200)
        self.log_path = ""
        self.analyzer = None

        self.layout = QVBoxLayout()
        self.setLayout(self.layout)

        self.label = QLabel("请选择日志文件")
        self.label.setAlignment(Qt.AlignCenter)
        self.layout.addWidget(self.label)

        self.btn_select = QPushButton("选择日志文件")
        self.btn_select.clicked.connect(self.select_file)
        self.layout.addWidget(self.btn_select)

        # 增加下拉框，选择时间格式
        self.format_group = QWidget()
        self.format_layout = QHBoxLayout()
        self.format_group.setLayout(self.format_layout)
        
        self.format_label = QLabel("日志格式:")
        self.format_layout.addWidget(self.format_label)
        
        self.format_combo = QComboBox()
        self.format_combo.addItem("格式1: [hh:mm:ss.xxx]发→◇.../收←◆...", "fmt1")
        self.format_combo.addItem("格式2: TX:.../RX:...", "fmt2")
        self.format_combo.addItem("格式3: [yyyy-mm-dd hh:mm:ss.xxx]# SEND/RECV HEX...", "fmt3")
        self.format_layout.addWidget(self.format_combo)
        
        self.layout.addWidget(self.format_group)

        self.btn_parse = QPushButton("开始解析")
        self.btn_parse.clicked.connect(self.start_parse)
        self.btn_parse.setEnabled(False)
        self.layout.addWidget(self.btn_parse)
        self.result_label = QLabel("")
        self.result_label.setAlignment(Qt.AlignCenter)
        self.layout.addWidget(self.result_label)

    def select_file(self):
        file_path, _ = QFileDialog.getOpenFileName(self, "选择日志文件", "", "日志文件 (*.log *.txt);;所有文件 (*)")
        if file_path:
            self.log_path = file_path
            self.label.setText(f"已选择文件: {file_path}")
            self.btn_parse.setEnabled(True)
            self.result_label.setText("")
        else:
            self.label.setText("未选择文件")
            self.btn_parse.setEnabled(False)

    def start_parse(self):
        if not self.log_path:
            QMessageBox.warning(self, "错误", "请先选择日志文件！")
            return
        try:
            self.analyzer = LogAnalyzer(self.log_path)
            # 获取选择的格式
            selected_format = self.format_combo.currentData()
            self.analyzer.parse_log(selected_format)
            # 导出Excel
            excel_path = self.log_path + "_intervals.xlsx"
            self.analyzer.export_excel(excel_path)
            # 绘图
            img_path = self.log_path + "_intervals.png"
            self.analyzer.plot_intervals(img_path)
            self.result_label.setText(f"解析完成！\nExcel: {excel_path}\n图片: {img_path}")
        except Exception as e:
            QMessageBox.critical(self, "解析失败", f"发生错误: {e}")

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())
