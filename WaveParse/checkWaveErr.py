import os
import pandas as pd
import sys
import numpy as np
import xlwings as xw
def check_errors(directory):
    check_cell_cnt = 0
    for filename in os.listdir(directory):
        if filename.endswith('.xlsx'):
            full_path = os.path.join(directory, filename)  # 拼成完整路径
            try:
                wb = xw.Book(full_path)  # 使用完整路径打开文件
                wb.app.calculate()  # 让 Excel 计算所有公式
                wb.save()  # 保存计算结果（写入 cached value）
                wb.close()
                print(f"Recalculated: {full_path}")
            except Exception as e:
                print(f"Error processing: {full_path}, error: {e}")
        if filename.endswith('.xlsx'):
            filepath = os.path.join(directory, filename)
            try:
                xl = pd.ExcelFile(filepath)
                for sheet_name in xl.sheet_names:
                    if sheet_name == "11" or sheet_name == "12":
                        continue
                    df = xl.parse(sheet_name, header=0)  # Assuming first row is header
                    # Error columns: 0-based indices for E (4), I (8), M (12)
                    error_cols = [4, 8, 12]
                    for row_idx, row in df.iterrows():
                        for col_idx in error_cols:
                            value = row.iloc[col_idx]
                            if np.isnan(value):
                                print(f"cell is empty,sheet{sheet_name}")
                            if isinstance(value, (int, float)) and abs(value) > 1:
                                # Calculate Excel row (header is 1, data starts at 2)
                                excel_row = row_idx + 2
                                # Calculate column letter (A=0 -> 'A')
                                col_letter = chr(65 + col_idx)
                                print(f"File: {filename}, Sheet: {sheet_name}, Position: {col_letter}{excel_row}, Value: {value}")
                # print(f"检查单元格非nan个数：{check_cell_cnt}")
            except Exception as e:
                print(f"Error processing file {filename}: {e}")

if __name__ == "__main__":
    check_errors(r"D:\work\project\ElectricityCycle\电动车精度数据")