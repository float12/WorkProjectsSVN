using TX.Framework.WindowUI.Controls;

namespace WindowsTest
{
    partial class FromContinuedFreezeTest
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
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FromContinuedFreezeTest));
			this.gbFreeze = new System.Windows.Forms.GroupBox();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.label2 = new System.Windows.Forms.Label();
			this.txtbCycle = new TX.Framework.WindowUI.Controls.TXTextBox();
			this.txbFreezeMinuteLTU = new TX.Framework.WindowUI.Controls.TXButton();
			this.txbFreezeMinute = new TX.Framework.WindowUI.Controls.TXButton();
			this.txbFreezeSettlementDate = new TX.Framework.WindowUI.Controls.TXButton();
			this.pbFreeze = new System.Windows.Forms.ProgressBar();
			this.txcbAdjustTime = new TX.Framework.WindowUI.Controls.TXCheckBox();
			this.txbFreezeHour = new TX.Framework.WindowUI.Controls.TXButton();
			this.txbFreezeDay = new TX.Framework.WindowUI.Controls.TXButton();
			this.label1 = new System.Windows.Forms.Label();
			this.txtbCounts = new TX.Framework.WindowUI.Controls.TXTextBox();
			this.timerSystem = new System.Windows.Forms.Timer(this.components);
			this.gbFreeze.SuspendLayout();
			this.groupBox1.SuspendLayout();
			this.SuspendLayout();
			// 
			// gbFreeze
			// 
			this.gbFreeze.Controls.Add(this.groupBox1);
			this.gbFreeze.Controls.Add(this.txbFreezeSettlementDate);
			this.gbFreeze.Controls.Add(this.pbFreeze);
			this.gbFreeze.Controls.Add(this.txcbAdjustTime);
			this.gbFreeze.Controls.Add(this.txbFreezeHour);
			this.gbFreeze.Controls.Add(this.txbFreezeDay);
			this.gbFreeze.Controls.Add(this.label1);
			this.gbFreeze.Controls.Add(this.txtbCounts);
			this.gbFreeze.Dock = System.Windows.Forms.DockStyle.Fill;
			this.gbFreeze.Font = new System.Drawing.Font("微软雅黑", 9F);
			this.gbFreeze.Location = new System.Drawing.Point(3, 31);
			this.gbFreeze.Name = "gbFreeze";
			this.gbFreeze.Size = new System.Drawing.Size(568, 173);
			this.gbFreeze.TabIndex = 52;
			this.gbFreeze.TabStop = false;
			this.gbFreeze.Text = "连续冻结测试";
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.label2);
			this.groupBox1.Controls.Add(this.txtbCycle);
			this.groupBox1.Controls.Add(this.txbFreezeMinuteLTU);
			this.groupBox1.Controls.Add(this.txbFreezeMinute);
			this.groupBox1.Location = new System.Drawing.Point(147, 62);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(397, 58);
			this.groupBox1.TabIndex = 66;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "负荷曲线";
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Location = new System.Drawing.Point(6, 28);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(65, 17);
			this.label2.TabIndex = 67;
			this.label2.Text = "周期:(Min)";
			// 
			// txtbCycle
			// 
			this.txtbCycle.BackColor = System.Drawing.Color.Transparent;
			this.txtbCycle.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(182)))), ((int)(((byte)(168)))), ((int)(((byte)(192)))));
			this.txtbCycle.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
			this.txtbCycle.ForeColor = System.Drawing.SystemColors.WindowText;
			this.txtbCycle.HeightLightBolorColor = System.Drawing.Color.FromArgb(((int)(((byte)(140)))), ((int)(((byte)(67)))), ((int)(((byte)(165)))), ((int)(((byte)(220)))));
			this.txtbCycle.Image = null;
			this.txtbCycle.ImageSize = new System.Drawing.Size(0, 0);
			this.txtbCycle.Location = new System.Drawing.Point(77, 22);
			this.txtbCycle.Name = "txtbCycle";
			this.txtbCycle.Padding = new System.Windows.Forms.Padding(2);
			this.txtbCycle.PasswordChar = '\0';
			this.txtbCycle.Required = false;
			this.txtbCycle.Size = new System.Drawing.Size(47, 22);
			this.txtbCycle.TabIndex = 66;
			this.txtbCycle.Text = "15";
			// 
			// txbFreezeMinuteLTU
			// 
			this.txbFreezeMinuteLTU.Image = null;
			this.txbFreezeMinuteLTU.Location = new System.Drawing.Point(273, 22);
			this.txbFreezeMinuteLTU.Name = "txbFreezeMinuteLTU";
			this.txbFreezeMinuteLTU.Size = new System.Drawing.Size(124, 28);
			this.txbFreezeMinuteLTU.TabIndex = 65;
			this.txbFreezeMinuteLTU.Text = "LTU负荷曲线测试";
			this.txbFreezeMinuteLTU.UseVisualStyleBackColor = true;
			this.txbFreezeMinuteLTU.Click += new System.EventHandler(this.txbFreezeMinuteLTU_Click);
			// 
			// txbFreezeMinute
			// 
			this.txbFreezeMinute.Image = null;
			this.txbFreezeMinute.Location = new System.Drawing.Point(130, 22);
			this.txbFreezeMinute.Name = "txbFreezeMinute";
			this.txbFreezeMinute.Size = new System.Drawing.Size(124, 28);
			this.txbFreezeMinute.TabIndex = 63;
			this.txbFreezeMinute.Text = "负荷曲线测试";
			this.txbFreezeMinute.UseVisualStyleBackColor = true;
			this.txbFreezeMinute.Click += new System.EventHandler(this.txbFreezeMinute_Click);
			// 
			// txbFreezeSettlementDate
			// 
			this.txbFreezeSettlementDate.Image = null;
			this.txbFreezeSettlementDate.Location = new System.Drawing.Point(420, 16);
			this.txbFreezeSettlementDate.Name = "txbFreezeSettlementDate";
			this.txbFreezeSettlementDate.Size = new System.Drawing.Size(124, 28);
			this.txbFreezeSettlementDate.TabIndex = 64;
			this.txbFreezeSettlementDate.Text = "结算日转存测试";
			this.txbFreezeSettlementDate.UseVisualStyleBackColor = true;
			this.txbFreezeSettlementDate.Click += new System.EventHandler(this.tbSettlementDate_Click);
			// 
			// pbFreeze
			// 
			this.pbFreeze.Location = new System.Drawing.Point(9, 134);
			this.pbFreeze.Name = "pbFreeze";
			this.pbFreeze.Size = new System.Drawing.Size(535, 23);
			this.pbFreeze.TabIndex = 62;
			// 
			// txcbAdjustTime
			// 
			this.txcbAdjustTime.AutoSize = true;
			this.txcbAdjustTime.Font = new System.Drawing.Font("微软雅黑", 9F, System.Drawing.FontStyle.Bold);
			this.txcbAdjustTime.Location = new System.Drawing.Point(9, 62);
			this.txcbAdjustTime.MinimumSize = new System.Drawing.Size(20, 20);
			this.txcbAdjustTime.Name = "txcbAdjustTime";
			this.txcbAdjustTime.Size = new System.Drawing.Size(99, 21);
			this.txcbAdjustTime.TabIndex = 61;
			this.txcbAdjustTime.Text = "是否校准时间";
			this.txcbAdjustTime.UseVisualStyleBackColor = true;
			// 
			// txbFreezeHour
			// 
			this.txbFreezeHour.Image = null;
			this.txbFreezeHour.Location = new System.Drawing.Point(147, 16);
			this.txbFreezeHour.Name = "txbFreezeHour";
			this.txbFreezeHour.Size = new System.Drawing.Size(124, 28);
			this.txbFreezeHour.TabIndex = 60;
			this.txbFreezeHour.Text = "整点冻结测试";
			this.txbFreezeHour.UseVisualStyleBackColor = true;
			this.txbFreezeHour.Click += new System.EventHandler(this.txbFreezeHour_Click);
			// 
			// txbFreezeDay
			// 
			this.txbFreezeDay.Image = null;
			this.txbFreezeDay.Location = new System.Drawing.Point(277, 16);
			this.txbFreezeDay.Name = "txbFreezeDay";
			this.txbFreezeDay.Size = new System.Drawing.Size(124, 28);
			this.txbFreezeDay.TabIndex = 59;
			this.txbFreezeDay.Text = "日冻结测试";
			this.txbFreezeDay.UseVisualStyleBackColor = true;
			this.txbFreezeDay.Click += new System.EventHandler(this.txbFreezeDay_Click);
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(6, 27);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(59, 17);
			this.label1.TabIndex = 58;
			this.label1.Text = "转存次数:";
			// 
			// txtbCounts
			// 
			this.txtbCounts.BackColor = System.Drawing.Color.Transparent;
			this.txtbCounts.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(182)))), ((int)(((byte)(168)))), ((int)(((byte)(192)))));
			this.txtbCounts.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
			this.txtbCounts.ForeColor = System.Drawing.SystemColors.WindowText;
			this.txtbCounts.HeightLightBolorColor = System.Drawing.Color.FromArgb(((int)(((byte)(140)))), ((int)(((byte)(67)))), ((int)(((byte)(165)))), ((int)(((byte)(220)))));
			this.txtbCounts.Image = null;
			this.txtbCounts.ImageSize = new System.Drawing.Size(0, 0);
			this.txtbCounts.Location = new System.Drawing.Point(71, 22);
			this.txtbCounts.Name = "txtbCounts";
			this.txtbCounts.Padding = new System.Windows.Forms.Padding(2);
			this.txtbCounts.PasswordChar = '\0';
			this.txtbCounts.Required = false;
			this.txtbCounts.Size = new System.Drawing.Size(65, 22);
			this.txtbCounts.TabIndex = 57;
			this.txtbCounts.Text = "5";
			// 
			// FromContinuedFreezeTest
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.Color.Transparent;
			this.ClientSize = new System.Drawing.Size(574, 207);
			this.Controls.Add(this.gbFreeze);
			this.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.Location = new System.Drawing.Point(0, 0);
			this.MaximizeBox = false;
			this.Name = "FromContinuedFreezeTest";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Load += new System.EventHandler(this.FromContinuedFreezeTest_Load);
			this.gbFreeze.ResumeLayout(false);
			this.gbFreeze.PerformLayout();
			this.groupBox1.ResumeLayout(false);
			this.groupBox1.PerformLayout();
			this.ResumeLayout(false);

        }

        #endregion
		private System.Windows.Forms.GroupBox gbFreeze;
		private System.Windows.Forms.Timer timerSystem;
		private TXButton txbFreezeDay;
		private System.Windows.Forms.Label label1;
		private TXTextBox txtbCounts;
		private TXButton txbFreezeHour;
		private TXCheckBox txcbAdjustTime;
		private System.Windows.Forms.ProgressBar pbFreeze;
		private TXButton txbFreezeMinute;
		private TXButton txbFreezeSettlementDate;
		private TXButton txbFreezeMinuteLTU;
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.Label label2;
		private TXTextBox txtbCycle;
	}
}