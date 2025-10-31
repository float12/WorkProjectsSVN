using TX.Framework.WindowUI.Controls;

namespace WindowsTest
{
	partial class FormReadMeterSystemEvent
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
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FormReadMeterSystemEvent));
			this.splitContainer1 = new System.Windows.Forms.SplitContainer();
			this.btn_导出至Excel = new TX.Framework.WindowUI.Controls.TXButton();
			this.label108 = new System.Windows.Forms.Label();
			this.textBox_EventEnd = new System.Windows.Forms.TextBox();
			this.label107 = new System.Windows.Forms.Label();
			this.textBox_EventBegin = new System.Windows.Forms.TextBox();
			this.comboBox_EventType = new TX.Framework.WindowUI.Controls.TXComboBox();
			this.btnEventRead = new TX.Framework.WindowUI.Controls.TXButton();
			this.dataGridView_Event = new System.Windows.Forms.DataGridView();
			this.Column_EventType = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.Column_addInfo = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.Column_EventTime = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.Column8 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.cbType = new System.Windows.Forms.CheckBox();
			((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
			this.splitContainer1.Panel1.SuspendLayout();
			this.splitContainer1.Panel2.SuspendLayout();
			this.splitContainer1.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.dataGridView_Event)).BeginInit();
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
			this.splitContainer1.Panel1.Controls.Add(this.cbType);
			this.splitContainer1.Panel1.Controls.Add(this.btn_导出至Excel);
			this.splitContainer1.Panel1.Controls.Add(this.label108);
			this.splitContainer1.Panel1.Controls.Add(this.textBox_EventEnd);
			this.splitContainer1.Panel1.Controls.Add(this.label107);
			this.splitContainer1.Panel1.Controls.Add(this.textBox_EventBegin);
			this.splitContainer1.Panel1.Controls.Add(this.comboBox_EventType);
			this.splitContainer1.Panel1.Controls.Add(this.btnEventRead);
			// 
			// splitContainer1.Panel2
			// 
			this.splitContainer1.Panel2.Controls.Add(this.dataGridView_Event);
			this.splitContainer1.Size = new System.Drawing.Size(1077, 684);
			this.splitContainer1.SplitterDistance = 47;
			this.splitContainer1.TabIndex = 0;
			// 
			// btn_导出至Excel
			// 
			this.btn_导出至Excel.Image = null;
			this.btn_导出至Excel.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.btn_导出至Excel.Location = new System.Drawing.Point(757, 7);
			this.btn_导出至Excel.Name = "btn_导出至Excel";
			this.btn_导出至Excel.Size = new System.Drawing.Size(95, 30);
			this.btn_导出至Excel.TabIndex = 24;
			this.btn_导出至Excel.Text = "导出至Excel";
			this.btn_导出至Excel.UseVisualStyleBackColor = true;
			this.btn_导出至Excel.Click += new System.EventHandler(this.btn_导出至Excel_Click);
			// 
			// label108
			// 
			this.label108.AutoSize = true;
			this.label108.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.label108.Location = new System.Drawing.Point(347, 15);
			this.label108.Name = "label108";
			this.label108.Size = new System.Drawing.Size(65, 12);
			this.label108.TabIndex = 23;
			this.label108.Text = "截止次数：";
			// 
			// textBox_EventEnd
			// 
			this.textBox_EventEnd.Location = new System.Drawing.Point(418, 11);
			this.textBox_EventEnd.Name = "textBox_EventEnd";
			this.textBox_EventEnd.Size = new System.Drawing.Size(55, 21);
			this.textBox_EventEnd.TabIndex = 22;
			this.textBox_EventEnd.Text = "252";
			// 
			// label107
			// 
			this.label107.AutoSize = true;
			this.label107.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.label107.Location = new System.Drawing.Point(205, 16);
			this.label107.Name = "label107";
			this.label107.Size = new System.Drawing.Size(65, 12);
			this.label107.TabIndex = 21;
			this.label107.Text = "起始次数：";
			// 
			// textBox_EventBegin
			// 
			this.textBox_EventBegin.Location = new System.Drawing.Point(286, 11);
			this.textBox_EventBegin.Name = "textBox_EventBegin";
			this.textBox_EventBegin.Size = new System.Drawing.Size(55, 21);
			this.textBox_EventBegin.TabIndex = 20;
			this.textBox_EventBegin.Text = "0";
			// 
			// comboBox_EventType
			// 
			this.comboBox_EventType.FormattingEnabled = true;
			this.comboBox_EventType.Items.AddRange(new object[] {
            "自由事件",
            "异常事件"});
			this.comboBox_EventType.Location = new System.Drawing.Point(21, 12);
			this.comboBox_EventType.Name = "comboBox_EventType";
			this.comboBox_EventType.Size = new System.Drawing.Size(89, 20);
			this.comboBox_EventType.TabIndex = 19;
			this.comboBox_EventType.Text = "自由事件";
			// 
			// btnEventRead
			// 
			this.btnEventRead.Image = null;
			this.btnEventRead.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.btnEventRead.Location = new System.Drawing.Point(629, 7);
			this.btnEventRead.Name = "btnEventRead";
			this.btnEventRead.Size = new System.Drawing.Size(95, 30);
			this.btnEventRead.TabIndex = 18;
			this.btnEventRead.Text = "读取";
			this.btnEventRead.UseVisualStyleBackColor = true;
			this.btnEventRead.Click += new System.EventHandler(this.btnEventRead_Click);
			// 
			// dataGridView_Event
			// 
			this.dataGridView_Event.AllowUserToAddRows = false;
			this.dataGridView_Event.AllowUserToDeleteRows = false;
			this.dataGridView_Event.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			this.dataGridView_Event.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.Column_EventType,
            this.Column_addInfo,
            this.Column_EventTime,
            this.Column8});
			this.dataGridView_Event.Dock = System.Windows.Forms.DockStyle.Fill;
			this.dataGridView_Event.Location = new System.Drawing.Point(0, 0);
			this.dataGridView_Event.Name = "dataGridView_Event";
			this.dataGridView_Event.ReadOnly = true;
			this.dataGridView_Event.RowHeadersWidthSizeMode = System.Windows.Forms.DataGridViewRowHeadersWidthSizeMode.AutoSizeToDisplayedHeaders;
			this.dataGridView_Event.RowTemplate.Height = 23;
			this.dataGridView_Event.Size = new System.Drawing.Size(1077, 633);
			this.dataGridView_Event.TabIndex = 4;
			// 
			// Column_EventType
			// 
			this.Column_EventType.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
			this.Column_EventType.HeaderText = "事件类型（HEX）";
			this.Column_EventType.MinimumWidth = 8;
			this.Column_EventType.Name = "Column_EventType";
			this.Column_EventType.ReadOnly = true;
			// 
			// Column_addInfo
			// 
			this.Column_addInfo.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
			this.Column_addInfo.HeaderText = "事件附加记录";
			this.Column_addInfo.MinimumWidth = 8;
			this.Column_addInfo.Name = "Column_addInfo";
			this.Column_addInfo.ReadOnly = true;
			// 
			// Column_EventTime
			// 
			this.Column_EventTime.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
			this.Column_EventTime.HeaderText = "事件发生时间";
			this.Column_EventTime.MinimumWidth = 8;
			this.Column_EventTime.Name = "Column_EventTime";
			this.Column_EventTime.ReadOnly = true;
			// 
			// Column8
			// 
			this.Column8.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
			this.Column8.HeaderText = "事件解析";
			this.Column8.MinimumWidth = 8;
			this.Column8.Name = "Column8";
			this.Column8.ReadOnly = true;
			// 
			// cbType
			// 
			this.cbType.AutoSize = true;
			this.cbType.Location = new System.Drawing.Point(126, 15);
			this.cbType.Name = "cbType";
			this.cbType.Size = new System.Drawing.Size(60, 16);
			this.cbType.TabIndex = 25;
			this.cbType.Text = "4G模块";
			this.cbType.UseVisualStyleBackColor = true;
			// 
			// FormReadMeterSystemEvent
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.Color.Transparent;
			this.ClientSize = new System.Drawing.Size(1083, 718);
			this.Controls.Add(this.splitContainer1);
			this.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.Location = new System.Drawing.Point(0, 0);
			this.Name = "FormReadMeterSystemEvent";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "自由及异常事件读取";
			this.Load += new System.EventHandler(this.FormAdjustMeter_Load);
			this.splitContainer1.Panel1.ResumeLayout(false);
			this.splitContainer1.Panel1.PerformLayout();
			this.splitContainer1.Panel2.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
			this.splitContainer1.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.dataGridView_Event)).EndInit();
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.SplitContainer splitContainer1;
		private TXButton btn_导出至Excel;
		private System.Windows.Forms.Label label108;
		private System.Windows.Forms.TextBox textBox_EventEnd;
		private System.Windows.Forms.Label label107;
		private System.Windows.Forms.TextBox textBox_EventBegin;
		private TXComboBox comboBox_EventType;
		private TXButton btnEventRead;
		private System.Windows.Forms.DataGridView dataGridView_Event;
		private System.Windows.Forms.DataGridViewTextBoxColumn Column_EventType;
		private System.Windows.Forms.DataGridViewTextBoxColumn Column_addInfo;
		private System.Windows.Forms.DataGridViewTextBoxColumn Column_EventTime;
		private System.Windows.Forms.DataGridViewTextBoxColumn Column8;
		private System.Windows.Forms.CheckBox cbType;
	}
}