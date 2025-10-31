using TX.Framework.WindowUI.Controls;

namespace WindowsTest
{
    partial class Form_DisplayDataBlock
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
			System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle1 = new System.Windows.Forms.DataGridViewCellStyle();
			System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle2 = new System.Windows.Forms.DataGridViewCellStyle();
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form_DisplayDataBlock));
			this.dgv_dataBlock = new System.Windows.Forms.DataGridView();
			this.splitContainer1 = new System.Windows.Forms.SplitContainer();
			this.tbOutPut = new TX.Framework.WindowUI.Controls.TXButton();
			this.btn_添加到设置列 = new TX.Framework.WindowUI.Controls.TXButton();
			this.btn_确认 = new TX.Framework.WindowUI.Controls.TXButton();
			this.Column1 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.Column2 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.Column3 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.Column4 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.Column5 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.Column6 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			((System.ComponentModel.ISupportInitialize)(this.dgv_dataBlock)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
			this.splitContainer1.Panel1.SuspendLayout();
			this.splitContainer1.Panel2.SuspendLayout();
			this.splitContainer1.SuspendLayout();
			this.SuspendLayout();
			// 
			// dgv_dataBlock
			// 
			this.dgv_dataBlock.AllowUserToAddRows = false;
			dataGridViewCellStyle1.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter;
			dataGridViewCellStyle1.BackColor = System.Drawing.SystemColors.Control;
			dataGridViewCellStyle1.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
			dataGridViewCellStyle1.ForeColor = System.Drawing.SystemColors.WindowText;
			dataGridViewCellStyle1.SelectionBackColor = System.Drawing.SystemColors.Highlight;
			dataGridViewCellStyle1.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
			dataGridViewCellStyle1.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
			this.dgv_dataBlock.ColumnHeadersDefaultCellStyle = dataGridViewCellStyle1;
			this.dgv_dataBlock.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			this.dgv_dataBlock.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.Column1,
            this.Column2,
            this.Column3,
            this.Column4,
            this.Column5,
            this.Column6});
			this.dgv_dataBlock.Dock = System.Windows.Forms.DockStyle.Fill;
			this.dgv_dataBlock.Location = new System.Drawing.Point(0, 0);
			this.dgv_dataBlock.Margin = new System.Windows.Forms.Padding(4);
			this.dgv_dataBlock.Name = "dgv_dataBlock";
			this.dgv_dataBlock.RowHeadersVisible = false;
			this.dgv_dataBlock.RowHeadersWidth = 62;
			this.dgv_dataBlock.RowTemplate.Height = 23;
			this.dgv_dataBlock.Size = new System.Drawing.Size(1129, 348);
			this.dgv_dataBlock.TabIndex = 0;
			// 
			// splitContainer1
			// 
			this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.splitContainer1.Location = new System.Drawing.Point(3, 31);
			this.splitContainer1.Margin = new System.Windows.Forms.Padding(4);
			this.splitContainer1.Name = "splitContainer1";
			this.splitContainer1.Orientation = System.Windows.Forms.Orientation.Horizontal;
			// 
			// splitContainer1.Panel1
			// 
			this.splitContainer1.Panel1.Controls.Add(this.dgv_dataBlock);
			// 
			// splitContainer1.Panel2
			// 
			this.splitContainer1.Panel2.Controls.Add(this.tbOutPut);
			this.splitContainer1.Panel2.Controls.Add(this.btn_添加到设置列);
			this.splitContainer1.Panel2.Controls.Add(this.btn_确认);
			this.splitContainer1.Size = new System.Drawing.Size(1129, 416);
			this.splitContainer1.SplitterDistance = 348;
			this.splitContainer1.SplitterWidth = 6;
			this.splitContainer1.TabIndex = 2;
			// 
			// tbOutPut
			// 
			this.tbOutPut.Image = null;
			this.tbOutPut.Location = new System.Drawing.Point(641, 23);
			this.tbOutPut.Name = "tbOutPut";
			this.tbOutPut.Size = new System.Drawing.Size(100, 28);
			this.tbOutPut.TabIndex = 2;
			this.tbOutPut.Text = "导出数据";
			this.tbOutPut.UseVisualStyleBackColor = true;
			this.tbOutPut.Click += new System.EventHandler(this.tbOutPut_Click);
			// 
			// btn_添加到设置列
			// 
			this.btn_添加到设置列.Image = null;
			this.btn_添加到设置列.Location = new System.Drawing.Point(292, 20);
			this.btn_添加到设置列.Margin = new System.Windows.Forms.Padding(4);
			this.btn_添加到设置列.Name = "btn_添加到设置列";
			this.btn_添加到设置列.Size = new System.Drawing.Size(150, 34);
			this.btn_添加到设置列.TabIndex = 1;
			this.btn_添加到设置列.Text = "添加到设置列";
			this.btn_添加到设置列.UseVisualStyleBackColor = true;
			this.btn_添加到设置列.Click += new System.EventHandler(this.btn_添加到设置列_Click);
			// 
			// btn_确认
			// 
			this.btn_确认.Image = null;
			this.btn_确认.Location = new System.Drawing.Point(517, 20);
			this.btn_确认.Margin = new System.Windows.Forms.Padding(4);
			this.btn_确认.Name = "btn_确认";
			this.btn_确认.Size = new System.Drawing.Size(72, 34);
			this.btn_确认.TabIndex = 0;
			this.btn_确认.Text = "确认";
			this.btn_确认.UseVisualStyleBackColor = true;
			this.btn_确认.Visible = false;
			this.btn_确认.Click += new System.EventHandler(this.btn_确认_Click);
			// 
			// Column1
			// 
			this.Column1.DataPropertyName = "Name";
			dataGridViewCellStyle2.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter;
			this.Column1.DefaultCellStyle = dataGridViewCellStyle2;
			this.Column1.HeaderText = "分项名称";
			this.Column1.MinimumWidth = 8;
			this.Column1.Name = "Column1";
			this.Column1.ReadOnly = true;
			this.Column1.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable;
			this.Column1.Width = 480;
			// 
			// Column2
			// 
			this.Column2.DataPropertyName = "DataLen";
			this.Column2.HeaderText = "数据长度";
			this.Column2.MinimumWidth = 8;
			this.Column2.Name = "Column2";
			this.Column2.ReadOnly = true;
			this.Column2.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable;
			this.Column2.Width = 60;
			// 
			// Column3
			// 
			this.Column3.DataPropertyName = "DataFormat";
			this.Column3.HeaderText = "数据格式";
			this.Column3.MinimumWidth = 8;
			this.Column3.Name = "Column3";
			this.Column3.ReadOnly = true;
			this.Column3.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable;
			this.Column3.Visible = false;
			this.Column3.Width = 60;
			// 
			// Column4
			// 
			this.Column4.DataPropertyName = "DecimalPlaces";
			this.Column4.HeaderText = "小数位数";
			this.Column4.MinimumWidth = 8;
			this.Column4.Name = "Column4";
			this.Column4.ReadOnly = true;
			this.Column4.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable;
			this.Column4.Width = 80;
			// 
			// Column5
			// 
			this.Column5.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.AllCells;
			this.Column5.DataPropertyName = "DataUnit";
			this.Column5.HeaderText = "单位";
			this.Column5.MinimumWidth = 80;
			this.Column5.Name = "Column5";
			this.Column5.ReadOnly = true;
			this.Column5.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable;
			this.Column5.Width = 80;
			// 
			// Column6
			// 
			this.Column6.DataPropertyName = "Value";
			this.Column6.HeaderText = "值";
			this.Column6.MinimumWidth = 8;
			this.Column6.Name = "Column6";
			this.Column6.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable;
			this.Column6.Width = 200;
			// 
			// Form_DisplayDataBlock
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(1135, 450);
			this.Controls.Add(this.splitContainer1);
			this.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.Location = new System.Drawing.Point(0, 0);
			this.Name = "Form_DisplayDataBlock";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Load += new System.EventHandler(this.Form_DisplayDataBlock_Load);
			((System.ComponentModel.ISupportInitialize)(this.dgv_dataBlock)).EndInit();
			this.splitContainer1.Panel1.ResumeLayout(false);
			this.splitContainer1.Panel2.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
			this.splitContainer1.ResumeLayout(false);
			this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.DataGridView dgv_dataBlock;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private TXButton btn_添加到设置列;
        private TXButton btn_确认;
		private TXButton tbOutPut;
		private System.Windows.Forms.DataGridViewTextBoxColumn Column1;
		private System.Windows.Forms.DataGridViewTextBoxColumn Column2;
		private System.Windows.Forms.DataGridViewTextBoxColumn Column3;
		private System.Windows.Forms.DataGridViewTextBoxColumn Column4;
		private System.Windows.Forms.DataGridViewTextBoxColumn Column5;
		private System.Windows.Forms.DataGridViewTextBoxColumn Column6;
	}
}