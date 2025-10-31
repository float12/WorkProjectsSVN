# -*- coding: gbk -*-
from PyQt5.QtCore import QThread, pyqtSignal

from ParseUIData import CsvSaver


class SaveToCsvTheard(QThread):
    """���߳����ڼ����ʱ���ۼӲ���"""
    SaveSignal = pyqtSignal(float,float,int)  # �ź����� str

    def __init__(self,phase,type, data_file_path, UIfactor,plot_widget,endian):
        self.phase =phase
        self.data_file_path=data_file_path
        self.UIfactor=UIfactor
        self.plot_widget = plot_widget
        self.Csv_Saver = CsvSaver(self.phase, type,self.data_file_path, self.UIfactor,endian)
        super().__init__()

    def run(self):
        ret = self.Csv_Saver.parse_and_save_to_csv()
        self.SaveSignal.emit(self.Csv_Saver.frame_parser.rms_Umax,self.Csv_Saver.frame_parser.rms_Imax,ret)  # ��������ɺ󣬷��ͽ��
