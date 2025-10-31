using System;
using System.Collections;
using System.Collections.Generic;
using System.Data;
using System.IO;
using System.Windows.Forms;
using Aspose.Cells;

namespace WindowsTest
{
	class AsposeExcelTools
	{
		public static bool DataTableToExcel(DataTable datatable, string filepath, out string error)
		{
			error = "";
			try
			{
				if (datatable == null)
				{
					error = "DataTableToExcel:datatable 为空";
					return false;
				}

				Aspose.Cells.Workbook workbook = new Aspose.Cells.Workbook();
				Aspose.Cells.Worksheet sheet = workbook.Worksheets[0];
				Aspose.Cells.Cells cells = sheet.Cells;
				sheet.Name = datatable.TableName;
				int nRow = 0;
				foreach (DataRow row in datatable.Rows)
				{
					nRow++;
					try
					{
                        for (int i = 0; i < datatable.Columns.Count; i++)
                        {
                            DataColumn col = datatable.Columns[i];
                            string columnName = col.Caption ?? col.ColumnName;
                            cells[0, i].PutValue(columnName);
                            //wb.Worksheets[0].Cells[rowIndex, i].Style = style;
                        }
						for (int i = 0; i < datatable.Columns.Count; i++)
						{
							if (row[i].GetType().ToString() == "System.Drawing.Bitmap")
							{
								//------插入图片数据-------
								System.Drawing.Image image = (System.Drawing.Image)row[i];
								MemoryStream mstream = new MemoryStream();
								image.Save(mstream, System.Drawing.Imaging.ImageFormat.Jpeg);
								sheet.Pictures.Add(nRow, i, mstream);
							}
							else
							{
								cells[nRow, i].PutValue(row[i]);
							}
						}
					}
					catch (System.Exception e)
					{
						error = error + " DataTableToExcel: " + e.Message;
					}
				}

				workbook.Save(filepath);
				return true;
			}
			catch (System.Exception e)
			{
				error = error + " DataTableToExcel: " + e.Message;
				return false;
			}
		}

		public static bool DataTableToExcel2(DataTable datatable, string filepath, out string error)
		{
			error = "";
			Aspose.Cells.Workbook wb = new Aspose.Cells.Workbook();

			try
			{
				if (datatable == null)
				{
					error = "DataTableToExcel:datatable 为空";
					return false;
				}

				//为单元格添加样式    
				Aspose.Cells.Style style = wb.Styles[wb.Styles.Add()];
				//设置居中
				style.HorizontalAlignment = Aspose.Cells.TextAlignmentType.Center;
				//设置背景颜色
				style.ForegroundColor = System.Drawing.Color.FromArgb(153, 204, 0);
				style.Pattern = BackgroundType.Solid;
				style.Font.IsBold = true;

				int rowIndex = 0;
				for (int i = 0; i < datatable.Columns.Count; i++)
				{
					DataColumn col = datatable.Columns[i];
					string columnName = col.Caption ?? col.ColumnName;
					wb.Worksheets[0].Cells[rowIndex, i].PutValue(columnName);
					wb.Worksheets[0].Cells[rowIndex, i].Style = style;
				}
				rowIndex++;

				foreach (DataRow row in datatable.Rows)
				{
					for (int i = 0; i < datatable.Columns.Count; i++)
					{
						wb.Worksheets[0].Cells[rowIndex, i].PutValue(row[i].ToString());
					}
					rowIndex++;
				}

				for (int k = 0; k < datatable.Columns.Count; k++)
				{
					wb.Worksheets[0].AutoFitColumn(k, 0, 150);
				}
				wb.Worksheets[0].FreezePanes(1, 0, 1, datatable.Columns.Count);
				wb.Save(filepath);
				return true;
			}
			catch (Exception e)
			{
				error = error + " DataTableToExcel: " + e.Message;
				return false;
			}

		}
		public static bool DataTableToExcel3(DataTable datatable, string filepath, out string error)
		{
			error = "";
			try
			{
				if (datatable == null)
				{
					error = "DataTableToExcel:datatable 为空";
					return false;
				}

				Aspose.Cells.Workbook workbook = new Aspose.Cells.Workbook();
				Aspose.Cells.Worksheet sheet = workbook.Worksheets[0];
				Aspose.Cells.Cells cells = sheet.Cells;

				int nRow = 0;
				foreach (DataRow row in datatable.Rows)
				{
					nRow++;
					try
					{
						for (int i = 0; i < datatable.Columns.Count; i++)
						{
							cells[0, i].PutValue(datatable.Columns[i].ColumnName);
							if (row[i].GetType().ToString() == "System.Drawing.Bitmap")
							{
								//------插入图片数据-------
								System.Drawing.Image image = (System.Drawing.Image)row[i];
								MemoryStream mstream = new MemoryStream();
								image.Save(mstream, System.Drawing.Imaging.ImageFormat.Jpeg);
								sheet.Pictures.Add(nRow, i, mstream);
							}
							else
							{
								cells[nRow, i].PutValue(row[i]);
							}
						}
					}
					catch (System.Exception e)
					{
						error = error + " DataTableToExcel: " + e.Message;
					}
				}

				workbook.Save(filepath);
				return true;
			}
			catch (System.Exception e)
			{
				error = error + " DataTableToExcel: " + e.Message;
				return false;
			}
		}
		/// <summary>
		/// 将DataTable型的List集合转换到一个excel文件里，每个DataTable占用一张工作表
		/// </summary>
		/// <param name="filepath">Excel文件的全路径</param>
		/// <param name="sheetname">工作表的名称</param>
		/// <param name="dtlist">DataTable:返回值</param>
		/// <param name="error">错误信息:返回错误信息，没有错误返回""</param>
		/// <returns>true:函数正确执行 false:函数执行错误</returns>
		public static bool DataTableListToExcel(List<DataTable> dtlist, string filepath, string[] sheetname, out string error)
		{
			error = "";
			try
			{
				if (dtlist == null)
				{
					error = "DataTableListToExcel:List<DataTable> 为空";
					return false;
				}
				if (dtlist.Count != sheetname.Length)
				{
					error = "需要导出的工作表数量与工作表标签名个数不相符，请检查";
					return false;
				}

				Aspose.Cells.Workbook workbook = new Aspose.Cells.Workbook();
				Worksheets worksheets = workbook.Worksheets;
				for (int m = 0; m < sheetname.Length; m++)
				{
					//Aspose.Cells.Worksheet sheet = workbook.Worksheets[0];
					//Aspose.Cells.Worksheet worksheets.Add(sheetname[m]);
					worksheets.Add(sheetname[m]);
					Aspose.Cells.Worksheet sheet = worksheets[sheetname[m]];
					Aspose.Cells.Cells cells = sheet.Cells;

					int nRow = 0;
					foreach (DataRow row in dtlist[m].Rows)
					{
						nRow++;
						try
						{
							for (int i = 0; i < dtlist[m].Columns.Count; i++)
							{
								cells[0, i].PutValue(dtlist[m].Columns[i].ColumnName);
								if (row[i].GetType().ToString() == "System.Drawing.Bitmap")
								{
									//------插入图片数据-------
									System.Drawing.Image image = (System.Drawing.Image)row[i];
									MemoryStream mstream = new MemoryStream();
									image.Save(mstream, System.Drawing.Imaging.ImageFormat.Jpeg);
									sheet.Pictures.Add(nRow, i, mstream);
								}
								else
								{
									cells[nRow, i].PutValue(row[i]);
								}
							}
						}

						catch (System.Exception e)
						{
							error = error + " DataTableListToExcel: " + e.Message;
						}
					}
				}
				worksheets.RemoveAt(0);
				workbook.Save(filepath);
				return true;
			}
			catch (System.Exception e)
			{
				error = error + " DataTableListToExcel: " + e.Message;
				return false;
			}
		}

		/// <summary>
		/// Excel文件转换为DataTable.
		/// </summary>
		/// <param name="filepath">Excel文件的全路径</param>
		/// <param name="datatable">DataTable:返回值</param>
		/// <param name="error">错误信息:返回错误信息，没有错误返回""</param>
		/// <returns>true:函数正确执行 false:函数执行错误</returns>
		public static bool ExcelFileToDataTable(string filepath, out DataTable datatable, out string error)
		{
			error = "";
			datatable = null;
			try
			{
				if (File.Exists(filepath) == false)
				{
					error = "文件不存在";
					datatable = null;
					return false;
				}
				Aspose.Cells.Workbook workbook = new Aspose.Cells.Workbook();
				workbook.Open(filepath);
				Aspose.Cells.Worksheet worksheet = workbook.Worksheets[0];
				datatable = worksheet.Cells.ExportDataTable(0, 0, worksheet.Cells.MaxRow + 1, worksheet.Cells.MaxColumn + 1);
				//-------------图片处理-------------
				//Aspose.Cells.Pictures pictures = worksheet.Pictures;
				//if (pictures.Count > 0)
				//{
				//    string error2 = "";
				//    if (InsertPicturesIntoDataTable(pictures, datatable, out datatable, out error2) == false)
				//    {
				//        error = error + error2;
				//    }
				//}
				return true;
			}
			catch (System.Exception e)
			{
				error = e.Message;
				return false;
			}

		}

		public static bool ExcelAllSheetToDataTableList(string filepath, out List<DataTable> datatables, out string error)
		{
			error = "";
			datatables = new List<DataTable>();
			try
			{
				if (File.Exists(filepath) == false)
				{
					error = "文件不存在";
					datatables = null;
					return false;
				}
				Aspose.Cells.Workbook workbook = new Aspose.Cells.Workbook();
				workbook.Open(filepath);

				for (int i = 0; i < workbook.Worksheets.Count; i++)
				{
					Aspose.Cells.Worksheet sourceSheet = workbook.Worksheets[i];
					if ((sourceSheet.Cells.MaxColumn == 0) || (sourceSheet.Cells.MaxRow == 0))
					{
						MessageBox.Show("请检查 Sheet'" + sourceSheet.Name + "'的格式");
						continue;
					}
					DataTable dt = sourceSheet.Cells.ExportDataTableAsString(0, 0, sourceSheet.Cells.MaxRow + 1, sourceSheet.Cells.MaxColumn + 1, true);
					dt.TableName = sourceSheet.Name;
					datatables.Add(dt);
				}
				return true;
			}
			catch (System.Exception e)
			{
				error = e.Message;
				return false;
			}
		}

		/// <summary>
		/// Excel中某一工作表文件转换为DataTable.
		/// </summary>
		/// <param name="filepath">Excel文件的全路径</param>
		/// <param name="sheetname">工作表的名称</param>
		/// <param name="MaxRowNum">限定转换的最大行数，MaxRowNum和MaxColumnNum中一个为0，则不做限定</param>
		/// <param name="MaxColumnNum">限定转换的最大列数，MaxRowNum和MaxColumnNum中一个为0，则不做限定</param>
		/// <param name="datatable">DataTable:返回值</param>
		/// <param name="error">错误信息:返回错误信息，没有错误返回""</param>
		/// <returns>true:函数正确执行 false:函数执行错误</returns>
		public static bool ExcelSheetToDataTable(string filepath, string sheetname, int MaxRowNum, int MaxColumnNum, out DataTable datatable, out string error)
		{
			error = "";
			datatable = null;
			try
			{
				if (File.Exists(filepath) == false)
				{
					error = "文件不存在";
					datatable = null;
					return false;
				}
				Aspose.Cells.Workbook workbook = new Aspose.Cells.Workbook();
				workbook.Open(filepath);
				int sheetcount = 10000;
				for (int i = 0; i < workbook.Worksheets.Count; i++)
				{
					Aspose.Cells.Worksheet sourceSheet = workbook.Worksheets[i];
					if (sourceSheet.Name == sheetname)
					{
						sheetcount = i;
						break;
					}
				}
				if (sheetcount == 10000)
				{
					error = string.Format("文件中不包含名为 {0} 的工作表", sheetname);
					datatable = null;
					return false;
				}

				Aspose.Cells.Worksheet worksheet = workbook.Worksheets[sheetcount];
				if ((MaxRowNum == 0) || (MaxColumnNum == 0))
					datatable = worksheet.Cells.ExportDataTableAsString(0, 0, worksheet.Cells.MaxRow + 1, worksheet.Cells.MaxColumn + 1, true);
				else
				{
					if ((worksheet.Cells.MaxRow + 1 >= MaxRowNum) && (worksheet.Cells.MaxColumn + 1 >= MaxColumnNum))
						datatable = worksheet.Cells.ExportDataTableAsString(0, 0, MaxRowNum, MaxColumnNum, true);
					if ((worksheet.Cells.MaxRow + 1 >= MaxRowNum) && (worksheet.Cells.MaxColumn + 1 < MaxColumnNum))
						datatable = worksheet.Cells.ExportDataTableAsString(0, 0, MaxRowNum, worksheet.Cells.MaxColumn + 1, true);
					if ((worksheet.Cells.MaxRow + 1 < MaxRowNum) && (worksheet.Cells.MaxColumn + 1 >= MaxColumnNum))
						datatable = worksheet.Cells.ExportDataTableAsString(0, 0, worksheet.Cells.MaxRow + 1, MaxColumnNum, true);
					if ((worksheet.Cells.MaxRow + 1 < MaxRowNum) && (worksheet.Cells.MaxColumn + 1 < MaxColumnNum))
						datatable = worksheet.Cells.ExportDataTableAsString(0, 0, worksheet.Cells.MaxRow + 1, worksheet.Cells.MaxColumn + 1, true);
				}
				//-------------图片处理-------------
				//Aspose.Cells.Pictures pictures = worksheet.Pictures;
				//if (pictures.Count > 0)
				//{
				//    string error2 = "";
				//    if (InsertPicturesIntoDataTable(pictures, datatable, out datatable, out error2) == false)
				//    {
				//        error = error + error2;
				//    }
				//}
				return true;
			}
			catch (System.Exception e)
			{
				error = e.Message;
				return false;
			}

		}

		public static bool ExcelFileToLists(string filepath, out IList[] lists, out string error)
		{
			error = "";
			lists = null;
			DataTable datatable = new DataTable();
			IList list = new ArrayList();
			//Pictures[] pictures;

			//if (ExcelFileToDataTable(filepath, out datatable, out error) && GetPicturesFromExcelFile(filepath, out pictures, out error))
			if (ExcelFileToDataTable(filepath, out datatable, out error))
			{
				lists = new ArrayList[datatable.Rows.Count];
				//------------DataTable转换成IList[]--------------
				//数据
				int nRow = 0;
				foreach (DataRow row in datatable.Rows)
				{
					lists[nRow] = new ArrayList(datatable.Columns.Count);
					for (int i = 0; i <= datatable.Columns.Count - 1; i++)
					{
						lists[nRow].Add(row[i]);
					}
					nRow++;
				}
				////图片
				//for (int i = 0; i < pictures.Length; i++)
				//{
				//    foreach (Picture picture in pictures[i])
				//    {
				//        try
				//        {
				//            //----把图片转换成System.Drawing.Image----
				//            //MemoryStream mstream = new MemoryStream();
				//            //mstream.Write(picture.Data, 0, picture.Data.Length);
				//            //System.Drawing.Image image = System.Drawing.Image.FromStream(mstream);
				//            //----Image放入IList------
				//            //图片有可能越界
				//            if (picture.UpperLeftRow <= datatable.Rows.Count && picture.UpperLeftColumn <= datatable.Columns.Count)
				//            {
				//                lists[picture.UpperLeftRow][picture.UpperLeftColumn] = picture.Data;
				//            }

				//        }
				//        catch (System.Exception e)
				//        {
				//            error = error + e.Message;
				//        }

				//    }
				//}

			}
			else
			{

				return false;
			}
			return true;
		}

		public static bool ListsToExcelFile(string filepath, IList[] lists, out string error)
		{
			error = "";
			//----------Aspose变量初始化----------------
			Aspose.Cells.Workbook workbook = new Aspose.Cells.Workbook();
			Aspose.Cells.Worksheet sheet = workbook.Worksheets[0];
			Aspose.Cells.Cells cells = sheet.Cells;
			//-------------输入数据-------------
			int nRow = 0;
			sheet.Pictures.Clear();
			cells.Clear();
			foreach (IList list in lists)
			{

				for (int i = 0; i <= list.Count - 1; i++)
				{
					try
					{
						System.Console.WriteLine(i.ToString() + "  " + list[i].GetType());
						if (list[i].GetType().ToString() == "System.Drawing.Bitmap")
						{
							//插入图片数据
							System.Drawing.Image image = (System.Drawing.Image)list[i];

							MemoryStream mstream = new MemoryStream();

							image.Save(mstream, System.Drawing.Imaging.ImageFormat.Jpeg);

							sheet.Pictures.Add(nRow, i, mstream);
						}
						else
						{
							cells[nRow, i].PutValue(list[i]);
						}
					}
					catch (System.Exception e)
					{
						error = error + e.Message;
					}

				}

				nRow++;
			}
			//-------------保存-------------
			workbook.Save(filepath);

			return true;
		}
	}
}
