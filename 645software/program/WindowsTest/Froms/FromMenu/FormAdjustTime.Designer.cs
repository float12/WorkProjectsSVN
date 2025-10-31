using TX.Framework.WindowUI.Controls;

namespace WindowsTest
{
    partial class FormAdjustTime
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
			this.components = new System.ComponentModel.Container();
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FormAdjustTime));
			this.groupBox36 = new System.Windows.Forms.GroupBox();
			this.cb_TrueAddr = new System.Windows.Forms.CheckBox();
			this.btn_ReadTime = new TX.Framework.WindowUI.Controls.TXButton();
			this.btn_ReadDate = new TX.Framework.WindowUI.Controls.TXButton();
			this.txt_SetMeterTime = new System.Windows.Forms.DateTimePicker();
			this.rb_CustomTime = new System.Windows.Forms.RadioButton();
			this.rb_SysTime = new System.Windows.Forms.RadioButton();
			this.btn_RadioSetMeterTime = new TX.Framework.WindowUI.Controls.TXButton();
			this.btn_SetMeterTime = new TX.Framework.WindowUI.Controls.TXButton();
			this.btn_ReadMeterDateTime = new TX.Framework.WindowUI.Controls.TXButton();
			this.btn_SetMeterDate = new TX.Framework.WindowUI.Controls.TXButton();
			this.txt_ReadMeterTime = new System.Windows.Forms.TextBox();
			this.btn_SetMeterDateTime = new TX.Framework.WindowUI.Controls.TXButton();
			this.label17 = new System.Windows.Forms.Label();
			this.label10 = new System.Windows.Forms.Label();
			this.timerSystem = new System.Windows.Forms.Timer(this.components);
			this.groupBox36.SuspendLayout();
			this.SuspendLayout();
			// 
			// groupBox36
			// 
			this.groupBox36.Controls.Add(this.cb_TrueAddr);
			this.groupBox36.Controls.Add(this.btn_ReadTime);
			this.groupBox36.Controls.Add(this.btn_ReadDate);
			this.groupBox36.Controls.Add(this.txt_SetMeterTime);
			this.groupBox36.Controls.Add(this.rb_CustomTime);
			this.groupBox36.Controls.Add(this.rb_SysTime);
			this.groupBox36.Controls.Add(this.btn_RadioSetMeterTime);
			this.groupBox36.Controls.Add(this.btn_SetMeterTime);
			this.groupBox36.Controls.Add(this.btn_ReadMeterDateTime);
			this.groupBox36.Controls.Add(this.btn_SetMeterDate);
			this.groupBox36.Controls.Add(this.txt_ReadMeterTime);
			this.groupBox36.Controls.Add(this.btn_SetMeterDateTime);
			this.groupBox36.Controls.Add(this.label17);
			this.groupBox36.Controls.Add(this.label10);
			this.groupBox36.Dock = System.Windows.Forms.DockStyle.Fill;
			this.groupBox36.Font = new System.Drawing.Font("微软雅黑", 9F);
			this.groupBox36.Location = new System.Drawing.Point(3, 31);
			this.groupBox36.Name = "groupBox36";
			this.groupBox36.Size = new System.Drawing.Size(372, 173);
			this.groupBox36.TabIndex = 52;
			this.groupBox36.TabStop = false;
			this.groupBox36.Text = "电表时间设置";
			// 
			// cb_TrueAddr
			// 
			this.cb_TrueAddr.AutoSize = true;
			this.cb_TrueAddr.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.cb_TrueAddr.Location = new System.Drawing.Point(221, 15);
			this.cb_TrueAddr.Name = "cb_TrueAddr";
			this.cb_TrueAddr.Size = new System.Drawing.Size(123, 21);
			this.cb_TrueAddr.TabIndex = 57;
			this.cb_TrueAddr.Text = "真实地址广播校时";
			this.cb_TrueAddr.UseVisualStyleBackColor = true;
			// 
			// btn_ReadTime
			// 
			this.btn_ReadTime.Image = null;
			this.btn_ReadTime.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.btn_ReadTime.Location = new System.Drawing.Point(249, 132);
			this.btn_ReadTime.Name = "btn_ReadTime";
			this.btn_ReadTime.Size = new System.Drawing.Size(95, 26);
			this.btn_ReadTime.TabIndex = 56;
			this.btn_ReadTime.Text = "抄时间";
			this.btn_ReadTime.UseVisualStyleBackColor = true;
			this.btn_ReadTime.Click += new System.EventHandler(this.btn_ReadTime_Click);
			// 
			// btn_ReadDate
			// 
			this.btn_ReadDate.Image = null;
			this.btn_ReadDate.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.btn_ReadDate.Location = new System.Drawing.Point(148, 132);
			this.btn_ReadDate.Name = "btn_ReadDate";
			this.btn_ReadDate.Size = new System.Drawing.Size(95, 26);
			this.btn_ReadDate.TabIndex = 55;
			this.btn_ReadDate.Text = "抄日期";
			this.btn_ReadDate.UseVisualStyleBackColor = true;
			this.btn_ReadDate.Click += new System.EventHandler(this.btn_ReadDate_Click);
			// 
			// txt_SetMeterTime
			// 
			this.txt_SetMeterTime.CalendarForeColor = System.Drawing.Color.Blue;
			this.txt_SetMeterTime.CalendarMonthBackground = System.Drawing.Color.FromArgb(((int)(((byte)(246)))), ((int)(((byte)(247)))), ((int)(((byte)(250)))));
			this.txt_SetMeterTime.CalendarTitleBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(220)))), ((int)(((byte)(74)))), ((int)(((byte)(181)))), ((int)(((byte)(237)))));
			this.txt_SetMeterTime.CalendarTrailingForeColor = System.Drawing.Color.CadetBlue;
			this.txt_SetMeterTime.CustomFormat = "yy-MM-dd HH:mm:ss";
			this.txt_SetMeterTime.Format = System.Windows.Forms.DateTimePickerFormat.Custom;
			this.txt_SetMeterTime.Location = new System.Drawing.Point(80, 42);
			this.txt_SetMeterTime.Name = "txt_SetMeterTime";
			this.txt_SetMeterTime.ShowCheckBox = true;
			this.txt_SetMeterTime.Size = new System.Drawing.Size(163, 23);
			this.txt_SetMeterTime.TabIndex = 54;
			this.txt_SetMeterTime.Value = new System.DateTime(2022, 8, 16, 19, 6, 2, 97);
			this.txt_SetMeterTime.ValueChanged += new System.EventHandler(this.txt_SetMeterTime_ValueChanged);
			// 
			// rb_CustomTime
			// 
			this.rb_CustomTime.AutoSize = true;
			this.rb_CustomTime.Checked = true;
			this.rb_CustomTime.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.rb_CustomTime.Location = new System.Drawing.Point(98, 15);
			this.rb_CustomTime.Name = "rb_CustomTime";
			this.rb_CustomTime.Size = new System.Drawing.Size(86, 21);
			this.rb_CustomTime.TabIndex = 10;
			this.rb_CustomTime.TabStop = true;
			this.rb_CustomTime.Text = "自定义时间";
			this.rb_CustomTime.UseVisualStyleBackColor = true;
			this.rb_CustomTime.CheckedChanged += new System.EventHandler(this.rb_CustomTime_CheckedChanged);
			// 
			// rb_SysTime
			// 
			this.rb_SysTime.AutoSize = true;
			this.rb_SysTime.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.rb_SysTime.Location = new System.Drawing.Point(18, 15);
			this.rb_SysTime.Name = "rb_SysTime";
			this.rb_SysTime.Size = new System.Drawing.Size(74, 21);
			this.rb_SysTime.TabIndex = 9;
			this.rb_SysTime.Text = "系统时间";
			this.rb_SysTime.UseVisualStyleBackColor = true;
			this.rb_SysTime.CheckedChanged += new System.EventHandler(this.rb_SysTime_CheckedChanged);
			// 
			// btn_RadioSetMeterTime
			// 
			this.btn_RadioSetMeterTime.Image = null;
			this.btn_RadioSetMeterTime.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.btn_RadioSetMeterTime.Location = new System.Drawing.Point(47, 72);
			this.btn_RadioSetMeterTime.Name = "btn_RadioSetMeterTime";
			this.btn_RadioSetMeterTime.Size = new System.Drawing.Size(95, 26);
			this.btn_RadioSetMeterTime.TabIndex = 8;
			this.btn_RadioSetMeterTime.Text = "广播校时";
			this.btn_RadioSetMeterTime.UseVisualStyleBackColor = true;
			this.btn_RadioSetMeterTime.Click += new System.EventHandler(this.btn_RadioSetMeterTime_Click);
			// 
			// btn_SetMeterTime
			// 
			this.btn_SetMeterTime.Image = null;
			this.btn_SetMeterTime.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.btn_SetMeterTime.Location = new System.Drawing.Point(249, 72);
			this.btn_SetMeterTime.Name = "btn_SetMeterTime";
			this.btn_SetMeterTime.Size = new System.Drawing.Size(95, 26);
			this.btn_SetMeterTime.TabIndex = 7;
			this.btn_SetMeterTime.Text = "设时间";
			this.btn_SetMeterTime.UseVisualStyleBackColor = true;
			this.btn_SetMeterTime.Click += new System.EventHandler(this.btn_SetMeterTime_Click);
			// 
			// btn_ReadMeterDateTime
			// 
			this.btn_ReadMeterDateTime.Image = null;
			this.btn_ReadMeterDateTime.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.btn_ReadMeterDateTime.Location = new System.Drawing.Point(249, 102);
			this.btn_ReadMeterDateTime.Name = "btn_ReadMeterDateTime";
			this.btn_ReadMeterDateTime.Size = new System.Drawing.Size(95, 26);
			this.btn_ReadMeterDateTime.TabIndex = 6;
			this.btn_ReadMeterDateTime.Text = "抄读日期时间";
			this.btn_ReadMeterDateTime.UseVisualStyleBackColor = true;
			this.btn_ReadMeterDateTime.Click += new System.EventHandler(this.btn_ReadMeterDateTime_Click);
			// 
			// btn_SetMeterDate
			// 
			this.btn_SetMeterDate.Image = null;
			this.btn_SetMeterDate.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.btn_SetMeterDate.Location = new System.Drawing.Point(148, 72);
			this.btn_SetMeterDate.Name = "btn_SetMeterDate";
			this.btn_SetMeterDate.Size = new System.Drawing.Size(95, 26);
			this.btn_SetMeterDate.TabIndex = 5;
			this.btn_SetMeterDate.Text = "设日期";
			this.btn_SetMeterDate.UseVisualStyleBackColor = true;
			this.btn_SetMeterDate.Click += new System.EventHandler(this.btn_SetMeterDate_Click);
			// 
			// txt_ReadMeterTime
			// 
			this.txt_ReadMeterTime.Location = new System.Drawing.Point(80, 105);
			this.txt_ReadMeterTime.Name = "txt_ReadMeterTime";
			this.txt_ReadMeterTime.ReadOnly = true;
			this.txt_ReadMeterTime.Size = new System.Drawing.Size(163, 23);
			this.txt_ReadMeterTime.TabIndex = 4;
			this.txt_ReadMeterTime.Text = "年-月-日 星期 时:分:秒";
			// 
			// btn_SetMeterDateTime
			// 
			this.btn_SetMeterDateTime.Image = null;
			this.btn_SetMeterDateTime.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.btn_SetMeterDateTime.Location = new System.Drawing.Point(249, 42);
			this.btn_SetMeterDateTime.Name = "btn_SetMeterDateTime";
			this.btn_SetMeterDateTime.Size = new System.Drawing.Size(95, 26);
			this.btn_SetMeterDateTime.TabIndex = 3;
			this.btn_SetMeterDateTime.Text = "设置日期时间";
			this.btn_SetMeterDateTime.UseVisualStyleBackColor = true;
			this.btn_SetMeterDateTime.Click += new System.EventHandler(this.btn_SetMeterDateTime_Click);
			// 
			// label17
			// 
			this.label17.AutoSize = true;
			this.label17.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.label17.Location = new System.Drawing.Point(15, 111);
			this.label17.Name = "label17";
			this.label17.Size = new System.Drawing.Size(59, 17);
			this.label17.TabIndex = 1;
			this.label17.Text = "电表时间:";
			// 
			// label10
			// 
			this.label10.AutoSize = true;
			this.label10.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.label10.Location = new System.Drawing.Point(15, 42);
			this.label10.Name = "label10";
			this.label10.Size = new System.Drawing.Size(59, 17);
			this.label10.TabIndex = 0;
			this.label10.Text = "系统时间:";
			this.label10.Click += new System.EventHandler(this.label10_Click);
			// 
			// timerSystem
			// 
			this.timerSystem.Tick += new System.EventHandler(this.timerSystem_Tick);
			// 
			// FormAdjustTime
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.Color.Transparent;
			this.ClientSize = new System.Drawing.Size(378, 207);
			this.Controls.Add(this.groupBox36);
			this.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.Location = new System.Drawing.Point(0, 0);
			this.MaximizeBox = false;
			this.Name = "FormAdjustTime";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Load += new System.EventHandler(this.FormAdjustMeter_Load);
			this.groupBox36.ResumeLayout(false);
			this.groupBox36.PerformLayout();
			this.ResumeLayout(false);

        }

        #endregion
		private System.Windows.Forms.GroupBox groupBox36;
		private System.Windows.Forms.CheckBox cb_TrueAddr;
		private TXButton btn_ReadTime;
		private TXButton btn_ReadDate;
		private System.Windows.Forms.DateTimePicker txt_SetMeterTime;
		private System.Windows.Forms.RadioButton rb_CustomTime;
		private System.Windows.Forms.RadioButton rb_SysTime;
		private TXButton btn_RadioSetMeterTime;
		private TXButton btn_SetMeterTime;
		private TXButton btn_ReadMeterDateTime;
		private TXButton btn_SetMeterDate;
		private System.Windows.Forms.TextBox txt_ReadMeterTime;
		private TXButton btn_SetMeterDateTime;
		private System.Windows.Forms.Label label17;
		private System.Windows.Forms.Label label10;
		private System.Windows.Forms.Timer timerSystem;
	}
}