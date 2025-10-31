import openpyxl
import numpy as np
import matplotlib.pyplot as plt

# 1. 打开 Excel 文件
file_path = r"C:\Users\宁浩\Desktop\waveAddVerify.xlsx"
wb = openpyxl.load_workbook(file_path)
sheet = wb["Sheet1"]

# 2. 定义读取 Excel 指定范围的函数
def read_range(sheet, cell_range):
    data_range = sheet[cell_range]
    values = []
    for row in data_range:
        for cell in row:
            if cell.value is not None:
                values.append(float(cell.value))
    return np.array(values)

# 3. 用户输入两个范围
range1 = "A18:DX18"
range2 = "A20:DX20"

data1 = read_range(sheet, range1)
data2 = read_range(sheet, range2)
print(data1)
print(data2)
# 4. 计算 RMS
rms1 = np.sqrt(np.mean(data1**2))
rms2 = np.sqrt(np.mean(data2**2))

print(f"{range1} 的 RMS 值为: {rms1}")
print(f"{range2} 的 RMS 值为: {rms2}")

# 5. 绘图对比
plt.figure(figsize=(10,4))
plt.plot(data1, label=f"{range1} RMS={rms1:.3f}")
plt.plot(data2, label=f"{range2} RMS={rms2:.3f}")
plt.xlabel("样本点序号")
plt.ylabel("值")
plt.title("两组数据波形及 RMS 对比")
plt.legend()
plt.grid(True)
plt.show()
