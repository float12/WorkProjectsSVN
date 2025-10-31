# -*- coding: gbk -*-
from PyQt5.QtCore import QThread, pyqtSignal

from ParseUIData import CsvSaver


class SaveToCsvTheard(QThread):
    """该线程用于计算耗时的累加操作"""
    SaveSignal = pyqtSignal(float,float,int)  # 信号类型 str

    def __init__(self,phase,type, data_file_path, UIfactor,plot_widget,endian):
        self.phase =phase
        self.data_file_path=data_file_path
        self.UIfactor=UIfactor
        self.plot_widget = plot_widget
        self.Csv_Saver = CsvSaver(self.phase, type,self.data_file_path, self.UIfactor,endian)
        super().__init__()

    def run(self):
        ret = self.Csv_Saver.parse_and_save_to_csv()
        self.SaveSignal.emit(self.Csv_Saver.frame_parser.rms_Umax,self.Csv_Saver.frame_parser.rms_Imax,ret)  # 计算结果完成后，发送结果
