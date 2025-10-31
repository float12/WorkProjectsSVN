# -*- coding: gbk -*-
import time

from PyQt5.QtCore import QThread, pyqtSignal
from datetime import datetime

from ParseUIData import WavePlotter
# import qtUI
class PlotTheard(QThread):
    """���߳����ڼ����ʱ���ۼӲ���"""
    plot_signal = pyqtSignal(object,int)  # �ź����� str

    def __init__(self,phase, type,data_file_path, UIfactor,endian):
        super().__init__()
        self.phase =phase
        self.data_file_path=data_file_path
        self.UIfactor=UIfactor
        self.Wave_Plotter = WavePlotter(self.phase,type, self.data_file_path, self.UIfactor,endian)


    def run(self):
        ret =  self.Wave_Plotter.parse_and_fill_plot_data()
        self.plot_signal.emit( self.Wave_Plotter,ret)  # ��������ɺ󣬷��ͽ��


