# -*- coding: gbk -*-
import openpyxl
from openpyxl.utils.cell import coordinate_from_string, column_index_from_string

def fill_wave_err_to_overview(excel_path):
    # 打开excel文件
    wb = openpyxl.load_workbook(excel_path, data_only=True)  # data_only=True 只读取值，不读取公式
    # 获取所有以"表"开头的sheet名
    table_sheets = [name for name in wb.sheetnames if name.startswith("表")]
    # 获取总览sheet
    overview_sheet = wb["录波精度总览"]

    # 需要读取的区域及其目标区域
    src_dst_map = [
        (("H2", "H7"), ("C3", "C8")),
        (("N2", "N7"), ("C10", "C15")),
        (("S2", "S7"), ("C17", "C22")),
        (("X2", "X7"), ("C24", "C29")),
        (("AC2", "AC7"), ("C31", "C36")),
        (("AH2", "AH7"), ("C38", "C43")),
        (("AM2", "AM7"), ("C45", "C50")),
        (("D10", "D15"), ("C53", "C58")),
        (("H10", "H15"), ("C61", "C66")),
    ]

    # 计算每个区域的行数
    def get_row_range(cell1, cell2):
        col1, row1 = coordinate_from_string(cell1)
        col2, row2 = coordinate_from_string(cell2)
        return int(row1), int(row2)

    # 每个表写入的起始列（C列，3号列）
    start_col = 3

    for idx, sheet_name in enumerate(table_sheets):
        sheet = wb[sheet_name]
        col_offset = idx  # 每个表右移一列

        for (src_range, dst_range) in src_dst_map:
            src_col, src_row_start = coordinate_from_string(src_range[0])
            _, src_row_end = coordinate_from_string(src_range[1])
            dst_col, dst_row_start = coordinate_from_string(dst_range[0])
            _, dst_row_end = coordinate_from_string(dst_range[1])

            # 计算目标列
            dst_col_num = column_index_from_string(dst_col) + col_offset
            # 读取源数据
            for i, row in enumerate(range(int(src_row_start), int(src_row_end)+1)):
                value = sheet[f"{src_col}{row}"].value
                # 写入目标sheet
                overview_sheet.cell(row=int(dst_row_start)+i, column=dst_col_num, value=value)

    wb.save(excel_path)
    print("数据已成功填入'录波精度总览'子表。")

if __name__ == "__main__":
    fill_wave_err_to_overview("C:\\Users\\nh\\Desktop\\三相录波工装点检表250816 .xlsx")
