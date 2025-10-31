namespace LeafSoft.PartPanel
{
    partial class TCPServerPanel
    {
        /// <summary> 
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary> 
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region 组件设计器生成的代码

        /// <summary> 
        /// 设计器支持所需的方法 - 不要
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
			this.tabDataReceiver = new LeafSoft.Units.TabDataReceive();
			this.DataSender = new LeafSoft.Units.DataSend();
			this.Configer = new LeafSoft.Units.NetTCPServer();
			this.txcTransshipment = new TX.Framework.WindowUI.Controls.TXCheckBox();
			this.txcbTransshipment = new TX.Framework.WindowUI.Controls.TXComboBox();
			this.SuspendLayout();
			// 
			// tabDataReceiver
			// 
			this.tabDataReceiver.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.tabDataReceiver.Location = new System.Drawing.Point(173, 198);
			this.tabDataReceiver.Name = "tabDataReceiver";
			this.tabDataReceiver.Size = new System.Drawing.Size(1158, 509);
			this.tabDataReceiver.TabIndex = 7;
			// 
			// DataSender
			// 
			this.DataSender.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.DataSender.Location = new System.Drawing.Point(181, 3);
			this.DataSender.Name = "DataSender";
			this.DataSender.Size = new System.Drawing.Size(1072, 189);
			this.DataSender.TabIndex = 5;
			this.DataSender.EventDataSend += new LeafSoft.Lib.LeafEvent.DataSendHandler(this.DataSender_EventDataSend);
			this.DataSender.Load += new System.EventHandler(this.DataSender_Load);
			// 
			// Configer
			// 
			this.Configer.Location = new System.Drawing.Point(3, -2);
			this.Configer.Name = "Configer";
			this.Configer.Size = new System.Drawing.Size(172, 718);
			this.Configer.TabIndex = 1;
			this.Configer.DataReceived += new LeafSoft.Lib.LeafEvent.DataReceivedHandler(this.Configer_DataReceived);
			// 
			// txcTransshipment
			// 
			this.txcTransshipment.AutoSize = true;
			this.txcTransshipment.Location = new System.Drawing.Point(1103, 67);
			this.txcTransshipment.MinimumSize = new System.Drawing.Size(20, 20);
			this.txcTransshipment.Name = "txcTransshipment";
			this.txcTransshipment.Size = new System.Drawing.Size(72, 20);
			this.txcTransshipment.TabIndex = 68;
			this.txcTransshipment.Text = "中转服务";
			this.txcTransshipment.UseVisualStyleBackColor = true;
			this.txcTransshipment.CheckedChanged += new System.EventHandler(this.txcTransshipment_CheckedChanged);
			// 
			// txcbTransshipment
			// 
			this.txcbTransshipment.FormattingEnabled = true;
			this.txcbTransshipment.Items.AddRange(new object[] {
            "COM4"});
			this.txcbTransshipment.Location = new System.Drawing.Point(1103, 25);
			this.txcbTransshipment.Name = "txcbTransshipment";
			this.txcbTransshipment.Size = new System.Drawing.Size(64, 20);
			this.txcbTransshipment.TabIndex = 67;
			this.txcbTransshipment.Text = "COM3";
			this.txcbTransshipment.SelectedIndexChanged += new System.EventHandler(this.txcbTransshipment_SelectedIndexChanged);
			// 
			// TCPServerPanel
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.AutoSize = true;
			this.Controls.Add(this.txcTransshipment);
			this.Controls.Add(this.txcbTransshipment);
			this.Controls.Add(this.tabDataReceiver);
			this.Controls.Add(this.DataSender);
			this.Controls.Add(this.Configer);
			this.Name = "TCPServerPanel";
			this.Size = new System.Drawing.Size(1337, 725);
			this.ResumeLayout(false);
			this.PerformLayout();

        }

        #endregion

        private Units.NetTCPServer Configer;
        private Units.DataSend DataSender;
        private Units.TabDataReceive tabDataReceiver;
		private TX.Framework.WindowUI.Controls.TXCheckBox txcTransshipment;
		public TX.Framework.WindowUI.Controls.TXComboBox txcbTransshipment;
	}
}
