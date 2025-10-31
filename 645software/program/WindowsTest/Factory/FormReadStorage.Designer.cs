using TX.Framework.WindowUI.Controls;

namespace WindowsTest
{
	partial class FormReadStorage
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
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FormReadStorage));
			this.splitContainer2 = new System.Windows.Forms.SplitContainer();
			this.splitContainer1 = new System.Windows.Forms.SplitContainer();
			this.label52 = new System.Windows.Forms.Label();
			this.button117 = new System.Windows.Forms.Button();
			this.label50 = new System.Windows.Forms.Label();
			this.bReadStorage = new System.Windows.Forms.Button();
			this.comboBox_RegData = new System.Windows.Forms.ComboBox();
			this.textBox_RegAddr = new System.Windows.Forms.TextBox();
			this.label51 = new System.Windows.Forms.Label();
			this.textBox_RegLen = new System.Windows.Forms.TextBox();
			this.rtbData = new System.Windows.Forms.RichTextBox();
			this.groupBox54 = new System.Windows.Forms.GroupBox();
			this.tbFactoryData = new System.Windows.Forms.TextBox();
			this.label104 = new System.Windows.Forms.Label();
			this.bSetFactory = new TX.Framework.WindowUI.Controls.TXButton();
			this.label105 = new System.Windows.Forms.Label();
			this.label106 = new System.Windows.Forms.Label();
			this.tbFactorySaveLen = new System.Windows.Forms.TextBox();
			this.tbFactorySaveAddr = new System.Windows.Forms.TextBox();
			this.bReadFactory = new TX.Framework.WindowUI.Controls.TXButton();
			((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).BeginInit();
			this.splitContainer2.Panel1.SuspendLayout();
			this.splitContainer2.Panel2.SuspendLayout();
			this.splitContainer2.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
			this.splitContainer1.Panel1.SuspendLayout();
			this.splitContainer1.Panel2.SuspendLayout();
			this.splitContainer1.SuspendLayout();
			this.groupBox54.SuspendLayout();
			this.SuspendLayout();
			// 
			// splitContainer2
			// 
			this.splitContainer2.Dock = System.Windows.Forms.DockStyle.Fill;
			this.splitContainer2.FixedPanel = System.Windows.Forms.FixedPanel.Panel2;
			this.splitContainer2.Location = new System.Drawing.Point(3, 31);
			this.splitContainer2.Name = "splitContainer2";
			this.splitContainer2.Orientation = System.Windows.Forms.Orientation.Horizontal;
			// 
			// splitContainer2.Panel1
			// 
			this.splitContainer2.Panel1.Controls.Add(this.splitContainer1);
			// 
			// splitContainer2.Panel2
			// 
			this.splitContainer2.Panel2.Controls.Add(this.groupBox54);
			this.splitContainer2.Size = new System.Drawing.Size(955, 545);
			this.splitContainer2.SplitterDistance = 408;
			this.splitContainer2.TabIndex = 1;
			// 
			// splitContainer1
			// 
			this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.splitContainer1.Location = new System.Drawing.Point(0, 0);
			this.splitContainer1.Name = "splitContainer1";
			this.splitContainer1.Orientation = System.Windows.Forms.Orientation.Horizontal;
			// 
			// splitContainer1.Panel1
			// 
			this.splitContainer1.Panel1.Controls.Add(this.label52);
			this.splitContainer1.Panel1.Controls.Add(this.button117);
			this.splitContainer1.Panel1.Controls.Add(this.label50);
			this.splitContainer1.Panel1.Controls.Add(this.bReadStorage);
			this.splitContainer1.Panel1.Controls.Add(this.comboBox_RegData);
			this.splitContainer1.Panel1.Controls.Add(this.textBox_RegAddr);
			this.splitContainer1.Panel1.Controls.Add(this.label51);
			this.splitContainer1.Panel1.Controls.Add(this.textBox_RegLen);
			// 
			// splitContainer1.Panel2
			// 
			this.splitContainer1.Panel2.Controls.Add(this.rtbData);
			this.splitContainer1.Size = new System.Drawing.Size(955, 408);
			this.splitContainer1.SplitterDistance = 74;
			this.splitContainer1.TabIndex = 21;
			// 
			// label52
			// 
			this.label52.AutoSize = true;
			this.label52.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.label52.Location = new System.Drawing.Point(41, 32);
			this.label52.Name = "label52";
			this.label52.Size = new System.Drawing.Size(65, 12);
			this.label52.TabIndex = 18;
			this.label52.Text = "读取空间：";
			// 
			// button117
			// 
			this.button117.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.button117.Location = new System.Drawing.Point(744, 14);
			this.button117.Name = "button117";
			this.button117.Size = new System.Drawing.Size(95, 30);
			this.button117.TabIndex = 20;
			this.button117.Text = "比较";
			this.button117.UseVisualStyleBackColor = true;
			this.button117.Visible = false;
			// 
			// label50
			// 
			this.label50.AutoSize = true;
			this.label50.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.label50.Location = new System.Drawing.Point(324, 32);
			this.label50.Name = "label50";
			this.label50.Size = new System.Drawing.Size(41, 12);
			this.label50.TabIndex = 16;
			this.label50.Text = "地址：";
			// 
			// bReadStorage
			// 
			this.bReadStorage.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.bReadStorage.Location = new System.Drawing.Point(628, 14);
			this.bReadStorage.Name = "bReadStorage";
			this.bReadStorage.Size = new System.Drawing.Size(95, 30);
			this.bReadStorage.TabIndex = 12;
			this.bReadStorage.Text = "读取";
			this.bReadStorage.UseVisualStyleBackColor = true;
			this.bReadStorage.Click += new System.EventHandler(this.bReadStorage_Click);
			// 
			// comboBox_RegData
			// 
			this.comboBox_RegData.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.comboBox_RegData.Font = new System.Drawing.Font("黑体", 10.5F);
			this.comboBox_RegData.FormattingEnabled = true;
			this.comboBox_RegData.Items.AddRange(new object[] {
            "管理芯RAM数据",
            "管理芯FLASH数据",
            "管理芯第一片EEPROM",
            "管理芯第二片EEPROM",
            "管理芯第三片EEPROM",
            "管理芯铁电",
            "管理芯连续空间",
            "计量芯RAM数据",
            "计量芯FLASH数据",
            "计量芯第一片EEPROM",
            "计量芯第二片EEPROM",
            "计量芯第三片EEPROM（外部flash）",
            "计量芯铁电",
            "计量芯连续空间"});
			this.comboBox_RegData.Location = new System.Drawing.Point(127, 22);
			this.comboBox_RegData.Name = "comboBox_RegData";
			this.comboBox_RegData.Size = new System.Drawing.Size(176, 22);
			this.comboBox_RegData.TabIndex = 13;
			// 
			// textBox_RegAddr
			// 
			this.textBox_RegAddr.Location = new System.Drawing.Point(386, 23);
			this.textBox_RegAddr.Name = "textBox_RegAddr";
			this.textBox_RegAddr.Size = new System.Drawing.Size(83, 21);
			this.textBox_RegAddr.TabIndex = 14;
			this.textBox_RegAddr.Text = "FFFFFFFF";
			// 
			// label51
			// 
			this.label51.AutoSize = true;
			this.label51.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.label51.Location = new System.Drawing.Point(490, 32);
			this.label51.Name = "label51";
			this.label51.Size = new System.Drawing.Size(41, 12);
			this.label51.TabIndex = 17;
			this.label51.Text = "长度：";
			// 
			// textBox_RegLen
			// 
			this.textBox_RegLen.Location = new System.Drawing.Point(552, 23);
			this.textBox_RegLen.Name = "textBox_RegLen";
			this.textBox_RegLen.Size = new System.Drawing.Size(55, 21);
			this.textBox_RegLen.TabIndex = 15;
			this.textBox_RegLen.Text = "255";
			// 
			// rtbData
			// 
			this.rtbData.Dock = System.Windows.Forms.DockStyle.Fill;
			this.rtbData.Location = new System.Drawing.Point(0, 0);
			this.rtbData.Name = "rtbData";
			this.rtbData.Size = new System.Drawing.Size(955, 330);
			this.rtbData.TabIndex = 19;
			this.rtbData.Text = "";
			// 
			// groupBox54
			// 
			this.groupBox54.Controls.Add(this.tbFactoryData);
			this.groupBox54.Controls.Add(this.label104);
			this.groupBox54.Controls.Add(this.bSetFactory);
			this.groupBox54.Controls.Add(this.label105);
			this.groupBox54.Controls.Add(this.label106);
			this.groupBox54.Controls.Add(this.tbFactorySaveLen);
			this.groupBox54.Controls.Add(this.tbFactorySaveAddr);
			this.groupBox54.Controls.Add(this.bReadFactory);
			this.groupBox54.Dock = System.Windows.Forms.DockStyle.Fill;
			this.groupBox54.Location = new System.Drawing.Point(0, 0);
			this.groupBox54.Name = "groupBox54";
			this.groupBox54.Size = new System.Drawing.Size(955, 133);
			this.groupBox54.TabIndex = 57;
			this.groupBox54.TabStop = false;
			this.groupBox54.Text = "厂内存储区";
			// 
			// tbFactoryData
			// 
			this.tbFactoryData.Location = new System.Drawing.Point(48, 40);
			this.tbFactoryData.Multiline = true;
			this.tbFactoryData.Name = "tbFactoryData";
			this.tbFactoryData.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
			this.tbFactoryData.Size = new System.Drawing.Size(898, 76);
			this.tbFactoryData.TabIndex = 15;
			// 
			// label104
			// 
			this.label104.AutoSize = true;
			this.label104.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.label104.Location = new System.Drawing.Point(6, 47);
			this.label104.Name = "label104";
			this.label104.Size = new System.Drawing.Size(41, 12);
			this.label104.TabIndex = 14;
			this.label104.Text = "数据：";
			// 
			// bSetFactory
			// 
			this.bSetFactory.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.bSetFactory.Image = null;
			this.bSetFactory.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.bSetFactory.Location = new System.Drawing.Point(653, 12);
			this.bSetFactory.Name = "bSetFactory";
			this.bSetFactory.Size = new System.Drawing.Size(69, 21);
			this.bSetFactory.TabIndex = 13;
			this.bSetFactory.Text = "写入";
			this.bSetFactory.UseVisualStyleBackColor = true;
			this.bSetFactory.Visible = false;
			this.bSetFactory.Click += new System.EventHandler(this.bSetFactory_Click);
			// 
			// label105
			// 
			this.label105.AutoSize = true;
			this.label105.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.label105.Location = new System.Drawing.Point(258, 16);
			this.label105.Name = "label105";
			this.label105.Size = new System.Drawing.Size(41, 12);
			this.label105.TabIndex = 12;
			this.label105.Text = "长度：";
			// 
			// label106
			// 
			this.label106.AutoSize = true;
			this.label106.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.label106.Location = new System.Drawing.Point(101, 16);
			this.label106.Name = "label106";
			this.label106.Size = new System.Drawing.Size(41, 12);
			this.label106.TabIndex = 11;
			this.label106.Text = "地址：";
			// 
			// tbFactorySaveLen
			// 
			this.tbFactorySaveLen.Location = new System.Drawing.Point(314, 12);
			this.tbFactorySaveLen.Name = "tbFactorySaveLen";
			this.tbFactorySaveLen.Size = new System.Drawing.Size(69, 21);
			this.tbFactorySaveLen.TabIndex = 10;
			this.tbFactorySaveLen.Text = "255";
			// 
			// tbFactorySaveAddr
			// 
			this.tbFactorySaveAddr.Location = new System.Drawing.Point(151, 12);
			this.tbFactorySaveAddr.Name = "tbFactorySaveAddr";
			this.tbFactorySaveAddr.Size = new System.Drawing.Size(69, 21);
			this.tbFactorySaveAddr.TabIndex = 9;
			this.tbFactorySaveAddr.Text = "FF";
			// 
			// bReadFactory
			// 
			this.bReadFactory.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.bReadFactory.Image = null;
			this.bReadFactory.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.bReadFactory.Location = new System.Drawing.Point(545, 13);
			this.bReadFactory.Name = "bReadFactory";
			this.bReadFactory.Size = new System.Drawing.Size(69, 21);
			this.bReadFactory.TabIndex = 8;
			this.bReadFactory.Text = "读取";
			this.bReadFactory.UseVisualStyleBackColor = true;
			this.bReadFactory.Click += new System.EventHandler(this.bReadFactory_Click);
			// 
			// FormReadStorage
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.Color.Transparent;
			this.ClientSize = new System.Drawing.Size(961, 579);
			this.Controls.Add(this.splitContainer2);
			this.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.Location = new System.Drawing.Point(0, 0);
			this.MaximizeBox = false;
			this.Name = "FormReadStorage";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Load += new System.EventHandler(this.FormAdjustMeter_Load);
			this.splitContainer2.Panel1.ResumeLayout(false);
			this.splitContainer2.Panel2.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).EndInit();
			this.splitContainer2.ResumeLayout(false);
			this.splitContainer1.Panel1.ResumeLayout(false);
			this.splitContainer1.Panel1.PerformLayout();
			this.splitContainer1.Panel2.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
			this.splitContainer1.ResumeLayout(false);
			this.groupBox54.ResumeLayout(false);
			this.groupBox54.PerformLayout();
			this.ResumeLayout(false);

		}

		#endregion
		private System.Windows.Forms.SplitContainer splitContainer2;
		private System.Windows.Forms.GroupBox groupBox54;
		private System.Windows.Forms.TextBox tbFactoryData;
		private System.Windows.Forms.Label label104;
		private TXButton bSetFactory;
		private System.Windows.Forms.Label label105;
		private System.Windows.Forms.Label label106;
		private System.Windows.Forms.TextBox tbFactorySaveLen;
		private System.Windows.Forms.TextBox tbFactorySaveAddr;
		private TXButton bReadFactory;
		private System.Windows.Forms.Button button117;
		private System.Windows.Forms.RichTextBox rtbData;
		private System.Windows.Forms.Label label52;
		private System.Windows.Forms.Label label51;
		private System.Windows.Forms.Label label50;
		private System.Windows.Forms.TextBox textBox_RegLen;
		private System.Windows.Forms.TextBox textBox_RegAddr;
		private System.Windows.Forms.ComboBox comboBox_RegData;
		private System.Windows.Forms.Button bReadStorage;
		private System.Windows.Forms.SplitContainer splitContainer1;
	}
}