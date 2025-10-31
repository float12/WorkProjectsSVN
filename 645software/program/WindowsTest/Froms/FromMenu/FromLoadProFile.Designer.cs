using TX.Framework.WindowUI.Controls;

namespace WindowsTest
{
	partial class FromLoadProFile
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing && (components != null))
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FromLoadProFile));
			this.splitContainer1 = new System.Windows.Forms.SplitContainer();
			this.panel18 = new System.Windows.Forms.Panel();
			this.cb_ForceRead = new System.Windows.Forms.CheckBox();
			this.label33 = new System.Windows.Forms.Label();
			this.progressBar1 = new System.Windows.Forms.ProgressBar();
			this.label29 = new System.Windows.Forms.Label();
			this.btn_SaveLoadCurveRecord = new TX.Framework.WindowUI.Controls.TXButton();
			this.btn_ClearLoadCurveRecord = new TX.Framework.WindowUI.Controls.TXButton();
			this.btn_ReadLoadCurve = new TX.Framework.WindowUI.Controls.TXButton();
			this.label31 = new System.Windows.Forms.Label();
			this.tb_ModulNum = new System.Windows.Forms.TextBox();
			this.label30 = new System.Windows.Forms.Label();
			this.dtp_LoadTime = new TX.Framework.WindowUI.Controls.TXDateTimePicker();
			this.label28 = new System.Windows.Forms.Label();
			this.cb_LoadModulType = new TX.Framework.WindowUI.Controls.TXComboBox();
			this.cb_LoadCurveType = new TX.Framework.WindowUI.Controls.TXComboBox();
			this.dgv_LoadCurve = new System.Windows.Forms.DataGridView();
			((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
			this.splitContainer1.Panel1.SuspendLayout();
			this.splitContainer1.Panel2.SuspendLayout();
			this.splitContainer1.SuspendLayout();
			this.panel18.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.dgv_LoadCurve)).BeginInit();
			this.SuspendLayout();
			// 
			// splitContainer1
			// 
			this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.splitContainer1.Location = new System.Drawing.Point(3, 31);
			this.splitContainer1.Name = "splitContainer1";
			this.splitContainer1.Orientation = System.Windows.Forms.Orientation.Horizontal;
			// 
			// splitContainer1.Panel1
			// 
			this.splitContainer1.Panel1.Controls.Add(this.panel18);
			// 
			// splitContainer1.Panel2
			// 
			this.splitContainer1.Panel2.Controls.Add(this.dgv_LoadCurve);
			this.splitContainer1.Size = new System.Drawing.Size(721, 765);
			this.splitContainer1.SplitterDistance = 76;
			this.splitContainer1.TabIndex = 0;
			// 
			// panel18
			// 
			this.panel18.Controls.Add(this.cb_ForceRead);
			this.panel18.Controls.Add(this.label33);
			this.panel18.Controls.Add(this.progressBar1);
			this.panel18.Controls.Add(this.label29);
			this.panel18.Controls.Add(this.btn_SaveLoadCurveRecord);
			this.panel18.Controls.Add(this.btn_ClearLoadCurveRecord);
			this.panel18.Controls.Add(this.btn_ReadLoadCurve);
			this.panel18.Controls.Add(this.label31);
			this.panel18.Controls.Add(this.tb_ModulNum);
			this.panel18.Controls.Add(this.label30);
			this.panel18.Controls.Add(this.dtp_LoadTime);
			this.panel18.Controls.Add(this.label28);
			this.panel18.Controls.Add(this.cb_LoadModulType);
			this.panel18.Controls.Add(this.cb_LoadCurveType);
			this.panel18.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panel18.Location = new System.Drawing.Point(0, 0);
			this.panel18.Name = "panel18";
			this.panel18.Size = new System.Drawing.Size(721, 76);
			this.panel18.TabIndex = 1;
			// 
			// cb_ForceRead
			// 
			this.cb_ForceRead.AutoSize = true;
			this.cb_ForceRead.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.cb_ForceRead.Location = new System.Drawing.Point(471, 20);
			this.cb_ForceRead.Margin = new System.Windows.Forms.Padding(2);
			this.cb_ForceRead.Name = "cb_ForceRead";
			this.cb_ForceRead.Size = new System.Drawing.Size(72, 16);
			this.cb_ForceRead.TabIndex = 15;
			this.cb_ForceRead.Text = "强制抄读";
			this.cb_ForceRead.UseVisualStyleBackColor = true;
			this.cb_ForceRead.Visible = false;
			// 
			// label33
			// 
			this.label33.AutoSize = true;
			this.label33.Font = new System.Drawing.Font("宋体", 9F);
			this.label33.ForeColor = System.Drawing.SystemColors.ControlText;
			this.label33.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.label33.Location = new System.Drawing.Point(11, 3);
			this.label33.Name = "label33";
			this.label33.Size = new System.Drawing.Size(35, 12);
			this.label33.TabIndex = 14;
			this.label33.Text = "进度:";
			this.label33.Visible = false;
			// 
			// progressBar1
			// 
			this.progressBar1.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.progressBar1.Location = new System.Drawing.Point(48, 5);
			this.progressBar1.Name = "progressBar1";
			this.progressBar1.Size = new System.Drawing.Size(585, 10);
			this.progressBar1.TabIndex = 13;
			this.progressBar1.Visible = false;
			// 
			// label29
			// 
			this.label29.AutoSize = true;
			this.label29.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.label29.Location = new System.Drawing.Point(495, 40);
			this.label29.Name = "label29";
			this.label29.Size = new System.Drawing.Size(29, 12);
			this.label29.TabIndex = 12;
			this.label29.Text = "开始";
			// 
			// btn_SaveLoadCurveRecord
			// 
			this.btn_SaveLoadCurveRecord.Image = null;
			this.btn_SaveLoadCurveRecord.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.btn_SaveLoadCurveRecord.Location = new System.Drawing.Point(558, 3);
			this.btn_SaveLoadCurveRecord.Name = "btn_SaveLoadCurveRecord";
			this.btn_SaveLoadCurveRecord.Size = new System.Drawing.Size(75, 26);
			this.btn_SaveLoadCurveRecord.TabIndex = 11;
			this.btn_SaveLoadCurveRecord.Text = "导出Excel";
			this.btn_SaveLoadCurveRecord.UseVisualStyleBackColor = true;
			this.btn_SaveLoadCurveRecord.Visible = false;
			// 
			// btn_ClearLoadCurveRecord
			// 
			this.btn_ClearLoadCurveRecord.Image = null;
			this.btn_ClearLoadCurveRecord.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.btn_ClearLoadCurveRecord.Location = new System.Drawing.Point(639, 3);
			this.btn_ClearLoadCurveRecord.Name = "btn_ClearLoadCurveRecord";
			this.btn_ClearLoadCurveRecord.Size = new System.Drawing.Size(75, 26);
			this.btn_ClearLoadCurveRecord.TabIndex = 10;
			this.btn_ClearLoadCurveRecord.Text = "清除记录";
			this.btn_ClearLoadCurveRecord.UseVisualStyleBackColor = true;
			this.btn_ClearLoadCurveRecord.Visible = false;
			// 
			// btn_ReadLoadCurve
			// 
			this.btn_ReadLoadCurve.Image = null;
			this.btn_ReadLoadCurve.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.btn_ReadLoadCurve.Location = new System.Drawing.Point(639, 33);
			this.btn_ReadLoadCurve.Name = "btn_ReadLoadCurve";
			this.btn_ReadLoadCurve.Size = new System.Drawing.Size(75, 26);
			this.btn_ReadLoadCurve.TabIndex = 9;
			this.btn_ReadLoadCurve.Text = "抄读数据";
			this.btn_ReadLoadCurve.UseVisualStyleBackColor = true;
			this.btn_ReadLoadCurve.Click += new System.EventHandler(this.btn_ReadLoadCurve_Click);
			// 
			// label31
			// 
			this.label31.AutoSize = true;
			this.label31.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.label31.Location = new System.Drawing.Point(615, 40);
			this.label31.Name = "label31";
			this.label31.Size = new System.Drawing.Size(17, 12);
			this.label31.TabIndex = 8;
			this.label31.Text = "块";
			// 
			// tb_ModulNum
			// 
			this.tb_ModulNum.Location = new System.Drawing.Point(560, 37);
			this.tb_ModulNum.Name = "tb_ModulNum";
			this.tb_ModulNum.Size = new System.Drawing.Size(49, 21);
			this.tb_ModulNum.TabIndex = 7;
			this.tb_ModulNum.Text = "1";
			// 
			// label30
			// 
			this.label30.AutoSize = true;
			this.label30.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.label30.Location = new System.Drawing.Point(525, 40);
			this.label30.Name = "label30";
			this.label30.Size = new System.Drawing.Size(29, 12);
			this.label30.TabIndex = 6;
			this.label30.Text = "抄读";
			// 
			// dtp_LoadTime
			// 
			this.dtp_LoadTime.CalendarForeColor = System.Drawing.Color.Blue;
			this.dtp_LoadTime.CalendarMonthBackground = System.Drawing.Color.FromArgb(((int)(((byte)(246)))), ((int)(((byte)(247)))), ((int)(((byte)(250)))));
			this.dtp_LoadTime.CalendarTitleBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(220)))), ((int)(((byte)(74)))), ((int)(((byte)(181)))), ((int)(((byte)(237)))));
			this.dtp_LoadTime.CalendarTrailingForeColor = System.Drawing.Color.CadetBlue;
			this.dtp_LoadTime.CustomFormat = "yy-MM-dd HH:mm";
			this.dtp_LoadTime.Enabled = false;
			this.dtp_LoadTime.Format = System.Windows.Forms.DateTimePickerFormat.Custom;
			this.dtp_LoadTime.Location = new System.Drawing.Point(338, 35);
			this.dtp_LoadTime.Name = "dtp_LoadTime";
			this.dtp_LoadTime.ShowCheckBox = true;
			this.dtp_LoadTime.Size = new System.Drawing.Size(151, 21);
			this.dtp_LoadTime.TabIndex = 3;
			this.dtp_LoadTime.Value = new System.DateTime(2022, 10, 12, 15, 0, 38, 602);
			// 
			// label28
			// 
			this.label28.AutoSize = true;
			this.label28.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.label28.Location = new System.Drawing.Point(299, 41);
			this.label28.Name = "label28";
			this.label28.Size = new System.Drawing.Size(41, 12);
			this.label28.TabIndex = 2;
			this.label28.Text = "时间：";
			// 
			// cb_LoadModulType
			// 
			this.cb_LoadModulType.AccessibleName = "";
			this.cb_LoadModulType.FormattingEnabled = true;
			this.cb_LoadModulType.Items.AddRange(new object[] {
            "最早记录块",
            "给定时间记录块",
            "最近一个记录块"});
			this.cb_LoadModulType.Location = new System.Drawing.Point(166, 37);
			this.cb_LoadModulType.Name = "cb_LoadModulType";
			this.cb_LoadModulType.Size = new System.Drawing.Size(121, 20);
			this.cb_LoadModulType.TabIndex = 1;
			this.cb_LoadModulType.Text = "最早记录块";
			this.cb_LoadModulType.SelectedIndexChanged += new System.EventHandler(this.cb_LoadModulType_SelectedIndexChanged);
			// 
			// cb_LoadCurveType
			// 
			this.cb_LoadCurveType.DropDownWidth = 200;
			this.cb_LoadCurveType.FormattingEnabled = true;
			this.cb_LoadCurveType.IntegralHeight = false;
			this.cb_LoadCurveType.Location = new System.Drawing.Point(13, 37);
			this.cb_LoadCurveType.Name = "cb_LoadCurveType";
			this.cb_LoadCurveType.Size = new System.Drawing.Size(144, 20);
			this.cb_LoadCurveType.TabIndex = 0;
			this.cb_LoadCurveType.Text = "负荷记录";
			this.cb_LoadCurveType.SelectedIndexChanged += new System.EventHandler(this.cb_LoadCurveType_SelectedIndexChanged);
			// 
			// dgv_LoadCurve
			// 
			this.dgv_LoadCurve.AllowUserToAddRows = false;
			this.dgv_LoadCurve.AllowUserToDeleteRows = false;
			this.dgv_LoadCurve.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.AllCells;
			this.dgv_LoadCurve.AutoSizeRowsMode = System.Windows.Forms.DataGridViewAutoSizeRowsMode.AllCells;
			this.dgv_LoadCurve.BackgroundColor = System.Drawing.Color.Gainsboro;
			this.dgv_LoadCurve.ClipboardCopyMode = System.Windows.Forms.DataGridViewClipboardCopyMode.EnableAlwaysIncludeHeaderText;
			this.dgv_LoadCurve.ColumnHeadersHeight = 34;
			this.dgv_LoadCurve.Dock = System.Windows.Forms.DockStyle.Fill;
			this.dgv_LoadCurve.Location = new System.Drawing.Point(0, 0);
			this.dgv_LoadCurve.Name = "dgv_LoadCurve";
			this.dgv_LoadCurve.RowHeadersVisible = false;
			this.dgv_LoadCurve.RowHeadersWidth = 62;
			this.dgv_LoadCurve.RowTemplate.Height = 23;
			this.dgv_LoadCurve.Size = new System.Drawing.Size(721, 685);
			this.dgv_LoadCurve.TabIndex = 1;
			// 
			// FromLoadProFile
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.Color.Transparent;
			this.ClientSize = new System.Drawing.Size(727, 799);
			this.Controls.Add(this.splitContainer1);
			this.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.Location = new System.Drawing.Point(0, 0);
			this.Name = "FromLoadProFile";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "读取负荷记录";
			this.Load += new System.EventHandler(this.FormAdjustMeter_Load);
			this.splitContainer1.Panel1.ResumeLayout(false);
			this.splitContainer1.Panel2.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
			this.splitContainer1.ResumeLayout(false);
			this.panel18.ResumeLayout(false);
			this.panel18.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.dgv_LoadCurve)).EndInit();
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.SplitContainer splitContainer1;
		private System.Windows.Forms.Panel panel18;
		private System.Windows.Forms.CheckBox cb_ForceRead;
		private System.Windows.Forms.Label label33;
		private System.Windows.Forms.ProgressBar progressBar1;
		private System.Windows.Forms.Label label29;
		private TXButton btn_SaveLoadCurveRecord;
		private TXButton btn_ClearLoadCurveRecord;
		private TXButton btn_ReadLoadCurve;
		private System.Windows.Forms.Label label31;
		private System.Windows.Forms.TextBox tb_ModulNum;
		private System.Windows.Forms.Label label30;
		private TXDateTimePicker dtp_LoadTime;
		private System.Windows.Forms.Label label28;
		private TXComboBox cb_LoadModulType;
		private TXComboBox cb_LoadCurveType;
		private System.Windows.Forms.DataGridView dgv_LoadCurve;
	}
}