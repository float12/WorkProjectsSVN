# -*- coding: gbk -*-
import openpyxl
from openpyxl.utils.cell import coordinate_from_string, column_index_from_string

def fill_wave_err_to_overview(excel_path):
    # ��excel�ļ�
    wb = openpyxl.load_workbook(excel_path, data_only=True)  # data_only=True ֻ��ȡֵ������ȡ��ʽ
    # ��ȡ������"��"��ͷ��sheet��
    table_sheets = [name for name in wb.sheetnames if name.startswith("��")]
    # ��ȡ����sheet
    overview_sheet = wb["¼����������"]

    # ��Ҫ��ȡ��������Ŀ������
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

    # ����ÿ�����������
    def get_row_range(cell1, cell2):
        col1, row1 = coordinate_from_string(cell1)
        col2, row2 = coordinate_from_string(cell2)
        return int(row1), int(row2)

    # ÿ����д�����ʼ�У�C�У�3���У�
    start_col = 3

    for idx, sheet_name in enumerate(table_sheets):
        sheet = wb[sheet_name]
        col_offset = idx  # ÿ��������һ��

        for (src_range, dst_range) in src_dst_map:
            src_col, src_row_start = coordinate_from_string(src_range[0])
            _, src_row_end = coordinate_from_string(src_range[1])
            dst_col, dst_row_start = coordinate_from_string(dst_range[0])
            _, dst_row_end = coordinate_from_string(dst_range[1])

            # ����Ŀ����
            dst_col_num = column_index_from_string(dst_col) + col_offset
            # ��ȡԴ����
            for i, row in enumerate(range(int(src_row_start), int(src_row_end)+1)):
                value = sheet[f"{src_col}{row}"].value
                # д��Ŀ��sheet
                overview_sheet.cell(row=int(dst_row_start)+i, column=dst_col_num, value=value)

    wb.save(excel_path)
    print("�����ѳɹ�����'¼����������'�ӱ�")

if __name__ == "__main__":
    fill_wave_err_to_overview("C:\\Users\\nh\\Desktop\\����¼����װ����250816 .xlsx")
