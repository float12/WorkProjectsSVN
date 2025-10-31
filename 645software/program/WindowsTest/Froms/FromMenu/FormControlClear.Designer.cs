using TX.Framework.WindowUI.Controls;

namespace WindowsTest
{
    partial class FormControlClear
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
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FormControlClear));
			this.timerSystem = new System.Windows.Forms.Timer(this.components);
			this.groupBox35 = new System.Windows.Forms.GroupBox();
			this.cbCycle = new TX.Framework.WindowUI.Controls.TXComboBox();
			this.ck_CKeyWord14 = new System.Windows.Forms.CheckBox();
			this.btn_Control = new TX.Framework.WindowUI.Controls.TXButton();
			this.label27 = new System.Windows.Forms.Label();
			this.cmb_Control = new TX.Framework.WindowUI.Controls.TXComboBox();
			this.dtp_ControlTime = new TX.Framework.WindowUI.Controls.TXDateTimePicker();
			this.label7 = new System.Windows.Forms.Label();
			this.cmb_StartWindUpTime = new TX.Framework.WindowUI.Controls.TXComboBox();
			this.btnReadyWindUpToDown = new TX.Framework.WindowUI.Controls.TXButton();
			this.btnReadyWindUpToAllow = new TX.Framework.WindowUI.Controls.TXButton();
			this.btnLiftAlarm = new TX.Framework.WindowUI.Controls.TXButton();
			this.btnLiftPaulPower = new TX.Framework.WindowUI.Controls.TXButton();
			this.btnPaulPower = new TX.Framework.WindowUI.Controls.TXButton();
			this.btnAlarm = new TX.Framework.WindowUI.Controls.TXButton();
			this.btnAllowWindDown = new TX.Framework.WindowUI.Controls.TXButton();
			this.btnWindDown = new TX.Framework.WindowUI.Controls.TXButton();
			this.btnWindUp = new TX.Framework.WindowUI.Controls.TXButton();
			this.groupBox28 = new System.Windows.Forms.GroupBox();
			this.dtp_ClearControlTime = new TX.Framework.WindowUI.Controls.TXDateTimePicker();
			this.label34 = new System.Windows.Forms.Label();
			this.label32 = new System.Windows.Forms.Label();
			this.txt_ClearEvent = new TX.Framework.WindowUI.Controls.TXTextBox();
			this.btnEventClear = new TX.Framework.WindowUI.Controls.TXButton();
			this.btnMeterClear = new TX.Framework.WindowUI.Controls.TXButton();
			this.btnDemandClear = new TX.Framework.WindowUI.Controls.TXButton();
			this.groupBox45 = new System.Windows.Forms.GroupBox();
			this.btn_MultifunctionalTerminalOutput = new TX.Framework.WindowUI.Controls.TXButton();
			this.rb_MultifunctionalTerminalOutput_SwitchTime = new TX.Framework.WindowUI.Controls.TXRadioButton();
			this.rb_MultifunctionalTerminalOutput_Demand = new TX.Framework.WindowUI.Controls.TXRadioButton();
			this.rb_MultifunctionalTerminalOutput_Second = new TX.Framework.WindowUI.Controls.TXRadioButton();
			this.groupBox35.SuspendLayout();
			this.groupBox28.SuspendLayout();
			this.groupBox45.SuspendLayout();
			this.SuspendLayout();
			// 
			// timerSystem
			// 
			this.timerSystem.Tick += new System.EventHandler(this.timerSystem_Tick);
			// 
			// groupBox35
			// 
			this.groupBox35.Controls.Add(this.cbCycle);
			this.groupBox35.Controls.Add(this.ck_CKeyWord14);
			this.groupBox35.Controls.Add(this.btn_Control);
			this.groupBox35.Controls.Add(this.label27);
			this.groupBox35.Controls.Add(this.cmb_Control);
			this.groupBox35.Controls.Add(this.dtp_ControlTime);
			this.groupBox35.Controls.Add(this.label7);
			this.groupBox35.Controls.Add(this.cmb_StartWindUpTime);
			this.groupBox35.Controls.Add(this.btnReadyWindUpToDown);
			this.groupBox35.Controls.Add(this.btnReadyWindUpToAllow);
			this.groupBox35.Controls.Add(this.btnLiftAlarm);
			this.groupBox35.Controls.Add(this.btnLiftPaulPower);
			this.groupBox35.Controls.Add(this.btnPaulPower);
			this.groupBox35.Controls.Add(this.btnAlarm);
			this.groupBox35.Controls.Add(this.btnAllowWindDown);
			this.groupBox35.Controls.Add(this.btnWindDown);
			this.groupBox35.Controls.Add(this.btnWindUp);
			this.groupBox35.Font = new System.Drawing.Font("微软雅黑", 9F);
			this.groupBox35.Location = new System.Drawing.Point(6, 34);
			this.groupBox35.Name = "groupBox35";
			this.groupBox35.Size = new System.Drawing.Size(333, 344);
			this.groupBox35.TabIndex = 46;
			this.groupBox35.TabStop = false;
			this.groupBox35.Text = "控制类操作";
			// 
			// cbCycle
			// 
			this.cbCycle.FormattingEnabled = true;
			this.cbCycle.Items.AddRange(new object[] {
            "第一回路",
            "第二回路",
            "第三回路",
            "三路同时"});
			this.cbCycle.Location = new System.Drawing.Point(15, 19);
			this.cbCycle.Name = "cbCycle";
			this.cbCycle.Size = new System.Drawing.Size(86, 25);
			this.cbCycle.TabIndex = 49;
			this.cbCycle.Text = "第一回路";
			this.cbCycle.SelectedIndexChanged += new System.EventHandler(this.cbCycle_SelectedIndexChanged);
			// 
			// ck_CKeyWord14
			// 
			this.ck_CKeyWord14.AutoSize = true;
			this.ck_CKeyWord14.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.ck_CKeyWord14.Location = new System.Drawing.Point(15, 56);
			this.ck_CKeyWord14.Name = "ck_CKeyWord14";
			this.ck_CKeyWord14.Size = new System.Drawing.Size(101, 21);
			this.ck_CKeyWord14.TabIndex = 48;
			this.ck_CKeyWord14.Text = "14返回状态字";
			this.ck_CKeyWord14.UseVisualStyleBackColor = true;
			this.ck_CKeyWord14.Visible = false;
			// 
			// btn_Control
			// 
			this.btn_Control.Font = new System.Drawing.Font("微软雅黑", 9F);
			this.btn_Control.Image = null;
			this.btn_Control.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.btn_Control.Location = new System.Drawing.Point(177, 77);
			this.btn_Control.Name = "btn_Control";
			this.btn_Control.Size = new System.Drawing.Size(132, 25);
			this.btn_Control.TabIndex = 47;
			this.btn_Control.Text = "执行控制命令";
			this.btn_Control.UseVisualStyleBackColor = true;
			this.btn_Control.Click += new System.EventHandler(this.btn_Control_Click);
			// 
			// label27
			// 
			this.label27.AutoSize = true;
			this.label27.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.label27.Location = new System.Drawing.Point(19, 87);
			this.label27.Name = "label27";
			this.label27.Size = new System.Drawing.Size(71, 17);
			this.label27.TabIndex = 41;
			this.label27.Text = "控制命令字:";
			// 
			// cmb_Control
			// 
			this.cmb_Control.FormattingEnabled = true;
			this.cmb_Control.Items.AddRange(new object[] {
            "1A",
            "1B",
            "1C",
            "1D",
            "1E",
            "2A",
            "2B",
            "3A",
            "3B"});
			this.cmb_Control.Location = new System.Drawing.Point(96, 78);
			this.cmb_Control.Name = "cmb_Control";
			this.cmb_Control.Size = new System.Drawing.Size(55, 25);
			this.cmb_Control.TabIndex = 40;
			this.cmb_Control.Text = "1A";
			// 
			// dtp_ControlTime
			// 
			this.dtp_ControlTime.CalendarForeColor = System.Drawing.Color.Blue;
			this.dtp_ControlTime.CalendarMonthBackground = System.Drawing.Color.FromArgb(((int)(((byte)(246)))), ((int)(((byte)(247)))), ((int)(((byte)(250)))));
			this.dtp_ControlTime.CalendarTitleBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(220)))), ((int)(((byte)(74)))), ((int)(((byte)(181)))), ((int)(((byte)(237)))));
			this.dtp_ControlTime.CalendarTrailingForeColor = System.Drawing.Color.CadetBlue;
			this.dtp_ControlTime.CustomFormat = "yy-MM-dd HH:mm:ss";
			this.dtp_ControlTime.Format = System.Windows.Forms.DateTimePickerFormat.Custom;
			this.dtp_ControlTime.Location = new System.Drawing.Point(157, 19);
			this.dtp_ControlTime.Name = "dtp_ControlTime";
			this.dtp_ControlTime.ShowCheckBox = true;
			this.dtp_ControlTime.Size = new System.Drawing.Size(152, 23);
			this.dtp_ControlTime.TabIndex = 39;
			this.dtp_ControlTime.Value = new System.DateTime(2022, 8, 16, 19, 1, 30, 348);
			// 
			// label7
			// 
			this.label7.AutoSize = true;
			this.label7.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.label7.Location = new System.Drawing.Point(107, 22);
			this.label7.Name = "label7";
			this.label7.Size = new System.Drawing.Size(44, 17);
			this.label7.TabIndex = 38;
			this.label7.Text = "时间戳";
			// 
			// cmb_StartWindUpTime
			// 
			this.cmb_StartWindUpTime.FormattingEnabled = true;
			this.cmb_StartWindUpTime.Items.AddRange(new object[] {
            "5",
            "10",
            "15",
            "20"});
			this.cmb_StartWindUpTime.Location = new System.Drawing.Point(177, 163);
			this.cmb_StartWindUpTime.Name = "cmb_StartWindUpTime";
			this.cmb_StartWindUpTime.Size = new System.Drawing.Size(132, 25);
			this.cmb_StartWindUpTime.TabIndex = 37;
			this.cmb_StartWindUpTime.Text = "跳闸自动恢复时间";
			// 
			// btnReadyWindUpToDown
			// 
			this.btnReadyWindUpToDown.Image = null;
			this.btnReadyWindUpToDown.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.btnReadyWindUpToDown.Location = new System.Drawing.Point(22, 196);
			this.btnReadyWindUpToDown.Name = "btnReadyWindUpToDown";
			this.btnReadyWindUpToDown.Size = new System.Drawing.Size(287, 25);
			this.btnReadyWindUpToDown.TabIndex = 21;
			this.btnReadyWindUpToDown.Text = "跳闸自动恢复(延时时间到直接合闸)";
			this.btnReadyWindUpToDown.UseVisualStyleBackColor = true;
			this.btnReadyWindUpToDown.Click += new System.EventHandler(this.btnReadyWindUpToDown_Click);
			// 
			// btnReadyWindUpToAllow
			// 
			this.btnReadyWindUpToAllow.Image = null;
			this.btnReadyWindUpToAllow.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.btnReadyWindUpToAllow.Location = new System.Drawing.Point(22, 235);
			this.btnReadyWindUpToAllow.Name = "btnReadyWindUpToAllow";
			this.btnReadyWindUpToAllow.Size = new System.Drawing.Size(290, 25);
			this.btnReadyWindUpToAllow.TabIndex = 20;
			this.btnReadyWindUpToAllow.Text = "跳闸自动恢复(延时时间到合闸允许)";
			this.btnReadyWindUpToAllow.UseVisualStyleBackColor = true;
			this.btnReadyWindUpToAllow.Click += new System.EventHandler(this.btnReadyWindUpToAllow_Click);
			// 
			// btnLiftAlarm
			// 
			this.btnLiftAlarm.Image = null;
			this.btnLiftAlarm.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.btnLiftAlarm.Location = new System.Drawing.Point(177, 266);
			this.btnLiftAlarm.Name = "btnLiftAlarm";
			this.btnLiftAlarm.Size = new System.Drawing.Size(132, 25);
			this.btnLiftAlarm.TabIndex = 19;
			this.btnLiftAlarm.Text = "报警解除";
			this.btnLiftAlarm.UseVisualStyleBackColor = true;
			this.btnLiftAlarm.Click += new System.EventHandler(this.btnLiftAlarm_Click);
			// 
			// btnLiftPaulPower
			// 
			this.btnLiftPaulPower.Image = null;
			this.btnLiftPaulPower.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.btnLiftPaulPower.Location = new System.Drawing.Point(175, 306);
			this.btnLiftPaulPower.Name = "btnLiftPaulPower";
			this.btnLiftPaulPower.Size = new System.Drawing.Size(134, 25);
			this.btnLiftPaulPower.TabIndex = 14;
			this.btnLiftPaulPower.Text = "解除保电";
			this.btnLiftPaulPower.UseVisualStyleBackColor = true;
			this.btnLiftPaulPower.Click += new System.EventHandler(this.btnLiftPaulPower_Click);
			// 
			// btnPaulPower
			// 
			this.btnPaulPower.Image = null;
			this.btnPaulPower.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.btnPaulPower.Location = new System.Drawing.Point(22, 306);
			this.btnPaulPower.Name = "btnPaulPower";
			this.btnPaulPower.Size = new System.Drawing.Size(135, 25);
			this.btnPaulPower.TabIndex = 13;
			this.btnPaulPower.Text = "保电";
			this.btnPaulPower.UseVisualStyleBackColor = true;
			this.btnPaulPower.Click += new System.EventHandler(this.btnPaulPower_Click);
			// 
			// btnAlarm
			// 
			this.btnAlarm.Image = null;
			this.btnAlarm.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.btnAlarm.Location = new System.Drawing.Point(22, 266);
			this.btnAlarm.Name = "btnAlarm";
			this.btnAlarm.Size = new System.Drawing.Size(137, 25);
			this.btnAlarm.TabIndex = 18;
			this.btnAlarm.Text = "报警";
			this.btnAlarm.UseVisualStyleBackColor = true;
			this.btnAlarm.Click += new System.EventHandler(this.btnAlarm_Click);
			// 
			// btnAllowWindDown
			// 
			this.btnAllowWindDown.Image = null;
			this.btnAllowWindDown.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.btnAllowWindDown.Location = new System.Drawing.Point(177, 123);
			this.btnAllowWindDown.Name = "btnAllowWindDown";
			this.btnAllowWindDown.Size = new System.Drawing.Size(132, 25);
			this.btnAllowWindDown.TabIndex = 17;
			this.btnAllowWindDown.Text = "合闸允许";
			this.btnAllowWindDown.UseVisualStyleBackColor = true;
			this.btnAllowWindDown.Click += new System.EventHandler(this.btnAllowWindDown_Click);
			// 
			// btnWindDown
			// 
			this.btnWindDown.Image = null;
			this.btnWindDown.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.btnWindDown.Location = new System.Drawing.Point(22, 163);
			this.btnWindDown.Name = "btnWindDown";
			this.btnWindDown.Size = new System.Drawing.Size(133, 25);
			this.btnWindDown.TabIndex = 16;
			this.btnWindDown.Text = "直接合闸";
			this.btnWindDown.UseVisualStyleBackColor = true;
			this.btnWindDown.Click += new System.EventHandler(this.btnWindDown_Click);
			// 
			// btnWindUp
			// 
			this.btnWindUp.Image = null;
			this.btnWindUp.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.btnWindUp.Location = new System.Drawing.Point(22, 123);
			this.btnWindUp.Name = "btnWindUp";
			this.btnWindUp.Size = new System.Drawing.Size(132, 25);
			this.btnWindUp.TabIndex = 15;
			this.btnWindUp.Text = "跳闸";
			this.btnWindUp.UseVisualStyleBackColor = true;
			this.btnWindUp.Click += new System.EventHandler(this.btnWindUp_Click);
			// 
			// groupBox28
			// 
			this.groupBox28.Controls.Add(this.dtp_ClearControlTime);
			this.groupBox28.Controls.Add(this.label34);
			this.groupBox28.Controls.Add(this.label32);
			this.groupBox28.Controls.Add(this.txt_ClearEvent);
			this.groupBox28.Controls.Add(this.btnEventClear);
			this.groupBox28.Controls.Add(this.btnMeterClear);
			this.groupBox28.Controls.Add(this.btnDemandClear);
			this.groupBox28.Font = new System.Drawing.Font("微软雅黑", 9F);
			this.groupBox28.Location = new System.Drawing.Point(345, 48);
			this.groupBox28.Name = "groupBox28";
			this.groupBox28.Size = new System.Drawing.Size(253, 158);
			this.groupBox28.TabIndex = 47;
			this.groupBox28.TabStop = false;
			this.groupBox28.Text = "清零操作";
			// 
			// dtp_ClearControlTime
			// 
			this.dtp_ClearControlTime.CalendarForeColor = System.Drawing.Color.Blue;
			this.dtp_ClearControlTime.CalendarMonthBackground = System.Drawing.Color.FromArgb(((int)(((byte)(246)))), ((int)(((byte)(247)))), ((int)(((byte)(250)))));
			this.dtp_ClearControlTime.CalendarTitleBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(220)))), ((int)(((byte)(74)))), ((int)(((byte)(181)))), ((int)(((byte)(237)))));
			this.dtp_ClearControlTime.CalendarTrailingForeColor = System.Drawing.Color.CadetBlue;
			this.dtp_ClearControlTime.CustomFormat = "yy-MM-dd HH:mm:ss";
			this.dtp_ClearControlTime.Format = System.Windows.Forms.DateTimePickerFormat.Custom;
			this.dtp_ClearControlTime.Location = new System.Drawing.Point(55, 16);
			this.dtp_ClearControlTime.Name = "dtp_ClearControlTime";
			this.dtp_ClearControlTime.ShowCheckBox = true;
			this.dtp_ClearControlTime.Size = new System.Drawing.Size(177, 23);
			this.dtp_ClearControlTime.TabIndex = 53;
			this.dtp_ClearControlTime.Value = new System.DateTime(2022, 8, 16, 19, 1, 30, 316);
			// 
			// label34
			// 
			this.label34.AutoSize = true;
			this.label34.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.label34.Location = new System.Drawing.Point(6, 18);
			this.label34.Name = "label34";
			this.label34.Size = new System.Drawing.Size(44, 17);
			this.label34.TabIndex = 52;
			this.label34.Text = "时间戳";
			// 
			// label32
			// 
			this.label32.AutoSize = true;
			this.label32.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.label32.Location = new System.Drawing.Point(6, 83);
			this.label32.Name = "label32";
			this.label32.Size = new System.Drawing.Size(111, 17);
			this.label32.TabIndex = 51;
			this.label32.Text = "事件清零数据标识 :";
			// 
			// txt_ClearEvent
			// 
			this.txt_ClearEvent.BackColor = System.Drawing.Color.Transparent;
			this.txt_ClearEvent.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(182)))), ((int)(((byte)(168)))), ((int)(((byte)(192)))));
			this.txt_ClearEvent.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
			this.txt_ClearEvent.ForeColor = System.Drawing.SystemColors.WindowText;
			this.txt_ClearEvent.HeightLightBolorColor = System.Drawing.Color.FromArgb(((int)(((byte)(140)))), ((int)(((byte)(67)))), ((int)(((byte)(165)))), ((int)(((byte)(220)))));
			this.txt_ClearEvent.Image = null;
			this.txt_ClearEvent.ImageSize = new System.Drawing.Size(0, 0);
			this.txt_ClearEvent.Location = new System.Drawing.Point(123, 77);
			this.txt_ClearEvent.MaxLength = 8;
			this.txt_ClearEvent.Name = "txt_ClearEvent";
			this.txt_ClearEvent.Padding = new System.Windows.Forms.Padding(2);
			this.txt_ClearEvent.PasswordChar = '\0';
			this.txt_ClearEvent.Required = false;
			this.txt_ClearEvent.Size = new System.Drawing.Size(109, 23);
			this.txt_ClearEvent.TabIndex = 50;
			this.txt_ClearEvent.Text = "FFFFFFFF";
			// 
			// btnEventClear
			// 
			this.btnEventClear.Image = null;
			this.btnEventClear.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.btnEventClear.Location = new System.Drawing.Point(123, 106);
			this.btnEventClear.Name = "btnEventClear";
			this.btnEventClear.Size = new System.Drawing.Size(75, 26);
			this.btnEventClear.TabIndex = 17;
			this.btnEventClear.Text = "事件清零";
			this.btnEventClear.UseVisualStyleBackColor = true;
			this.btnEventClear.Click += new System.EventHandler(this.btnEventClear_Click);
			// 
			// btnMeterClear
			// 
			this.btnMeterClear.Image = null;
			this.btnMeterClear.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.btnMeterClear.Location = new System.Drawing.Point(9, 45);
			this.btnMeterClear.Name = "btnMeterClear";
			this.btnMeterClear.Size = new System.Drawing.Size(75, 26);
			this.btnMeterClear.TabIndex = 16;
			this.btnMeterClear.Text = "电表清零";
			this.btnMeterClear.UseVisualStyleBackColor = true;
			this.btnMeterClear.Click += new System.EventHandler(this.btnMeterClear_Click);
			// 
			// btnDemandClear
			// 
			this.btnDemandClear.Image = null;
			this.btnDemandClear.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.btnDemandClear.Location = new System.Drawing.Point(123, 45);
			this.btnDemandClear.Name = "btnDemandClear";
			this.btnDemandClear.Size = new System.Drawing.Size(75, 26);
			this.btnDemandClear.TabIndex = 15;
			this.btnDemandClear.Text = "需量清零";
			this.btnDemandClear.UseVisualStyleBackColor = true;
			this.btnDemandClear.Click += new System.EventHandler(this.btnDemandClear_Click);
			// 
			// groupBox45
			// 
			this.groupBox45.Controls.Add(this.btn_MultifunctionalTerminalOutput);
			this.groupBox45.Controls.Add(this.rb_MultifunctionalTerminalOutput_SwitchTime);
			this.groupBox45.Controls.Add(this.rb_MultifunctionalTerminalOutput_Demand);
			this.groupBox45.Controls.Add(this.rb_MultifunctionalTerminalOutput_Second);
			this.groupBox45.Location = new System.Drawing.Point(348, 226);
			this.groupBox45.Name = "groupBox45";
			this.groupBox45.Size = new System.Drawing.Size(250, 152);
			this.groupBox45.TabIndex = 55;
			this.groupBox45.TabStop = false;
			this.groupBox45.Text = "多功能端子输出控制";
			// 
			// btn_MultifunctionalTerminalOutput
			// 
			this.btn_MultifunctionalTerminalOutput.Image = null;
			this.btn_MultifunctionalTerminalOutput.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.btn_MultifunctionalTerminalOutput.Location = new System.Drawing.Point(29, 101);
			this.btn_MultifunctionalTerminalOutput.Name = "btn_MultifunctionalTerminalOutput";
			this.btn_MultifunctionalTerminalOutput.Size = new System.Drawing.Size(85, 26);
			this.btn_MultifunctionalTerminalOutput.TabIndex = 56;
			this.btn_MultifunctionalTerminalOutput.Text = "设置";
			this.btn_MultifunctionalTerminalOutput.UseVisualStyleBackColor = true;
			this.btn_MultifunctionalTerminalOutput.Click += new System.EventHandler(this.btn_MultifunctionalTerminalOutput_Click);
			// 
			// rb_MultifunctionalTerminalOutput_SwitchTime
			// 
			this.rb_MultifunctionalTerminalOutput_SwitchTime.AutoSize = true;
			this.rb_MultifunctionalTerminalOutput_SwitchTime.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.rb_MultifunctionalTerminalOutput_SwitchTime.Location = new System.Drawing.Point(29, 68);
			this.rb_MultifunctionalTerminalOutput_SwitchTime.MaxRadius = 8;
			this.rb_MultifunctionalTerminalOutput_SwitchTime.MinimumSize = new System.Drawing.Size(22, 22);
			this.rb_MultifunctionalTerminalOutput_SwitchTime.MinRadius = 4;
			this.rb_MultifunctionalTerminalOutput_SwitchTime.Name = "rb_MultifunctionalTerminalOutput_SwitchTime";
			this.rb_MultifunctionalTerminalOutput_SwitchTime.Size = new System.Drawing.Size(71, 22);
			this.rb_MultifunctionalTerminalOutput_SwitchTime.TabIndex = 2;
			this.rb_MultifunctionalTerminalOutput_SwitchTime.Text = "时段投切";
			this.rb_MultifunctionalTerminalOutput_SwitchTime.UseVisualStyleBackColor = true;
			// 
			// rb_MultifunctionalTerminalOutput_Demand
			// 
			this.rb_MultifunctionalTerminalOutput_Demand.AutoSize = true;
			this.rb_MultifunctionalTerminalOutput_Demand.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.rb_MultifunctionalTerminalOutput_Demand.Location = new System.Drawing.Point(29, 45);
			this.rb_MultifunctionalTerminalOutput_Demand.MaxRadius = 8;
			this.rb_MultifunctionalTerminalOutput_Demand.MinimumSize = new System.Drawing.Size(22, 22);
			this.rb_MultifunctionalTerminalOutput_Demand.MinRadius = 4;
			this.rb_MultifunctionalTerminalOutput_Demand.Name = "rb_MultifunctionalTerminalOutput_Demand";
			this.rb_MultifunctionalTerminalOutput_Demand.Size = new System.Drawing.Size(71, 22);
			this.rb_MultifunctionalTerminalOutput_Demand.TabIndex = 1;
			this.rb_MultifunctionalTerminalOutput_Demand.Text = "需量周期";
			this.rb_MultifunctionalTerminalOutput_Demand.UseVisualStyleBackColor = true;
			// 
			// rb_MultifunctionalTerminalOutput_Second
			// 
			this.rb_MultifunctionalTerminalOutput_Second.AutoSize = true;
			this.rb_MultifunctionalTerminalOutput_Second.Checked = true;
			this.rb_MultifunctionalTerminalOutput_Second.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.rb_MultifunctionalTerminalOutput_Second.Location = new System.Drawing.Point(29, 22);
			this.rb_MultifunctionalTerminalOutput_Second.MaxRadius = 8;
			this.rb_MultifunctionalTerminalOutput_Second.MinimumSize = new System.Drawing.Size(22, 22);
			this.rb_MultifunctionalTerminalOutput_Second.MinRadius = 4;
			this.rb_MultifunctionalTerminalOutput_Second.Name = "rb_MultifunctionalTerminalOutput_Second";
			this.rb_MultifunctionalTerminalOutput_Second.Size = new System.Drawing.Size(83, 22);
			this.rb_MultifunctionalTerminalOutput_Second.TabIndex = 0;
			this.rb_MultifunctionalTerminalOutput_Second.TabStop = true;
			this.rb_MultifunctionalTerminalOutput_Second.Text = "时钟秒脉冲";
			this.rb_MultifunctionalTerminalOutput_Second.UseVisualStyleBackColor = true;
			// 
			// FormControlClear
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.Color.Transparent;
			this.ClientSize = new System.Drawing.Size(604, 386);
			this.Controls.Add(this.groupBox45);
			this.Controls.Add(this.groupBox28);
			this.Controls.Add(this.groupBox35);
			this.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.Location = new System.Drawing.Point(0, 0);
			this.MaximizeBox = false;
			this.Name = "FormControlClear";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Load += new System.EventHandler(this.FormAdjustMeter_Load);
			this.groupBox35.ResumeLayout(false);
			this.groupBox35.PerformLayout();
			this.groupBox28.ResumeLayout(false);
			this.groupBox28.PerformLayout();
			this.groupBox45.ResumeLayout(false);
			this.groupBox45.PerformLayout();
			this.ResumeLayout(false);

        }

        #endregion
		private System.Windows.Forms.Timer timerSystem;
		private System.Windows.Forms.GroupBox groupBox35;
		private System.Windows.Forms.CheckBox ck_CKeyWord14;
		private TXButton btn_Control;
		private System.Windows.Forms.Label label27;
		private TXComboBox cmb_Control;
		private TXDateTimePicker dtp_ControlTime;
		private System.Windows.Forms.Label label7;
		private TXComboBox cmb_StartWindUpTime;
		private TXButton btnReadyWindUpToDown;
		private TXButton btnReadyWindUpToAllow;
		private TXButton btnLiftAlarm;
		private TXButton btnLiftPaulPower;
		private TXButton btnPaulPower;
		private TXButton btnAlarm;
		private TXButton btnAllowWindDown;
		private TXButton btnWindDown;
		private TXButton btnWindUp;
		private System.Windows.Forms.GroupBox groupBox28;
		private TXDateTimePicker dtp_ClearControlTime;
		private System.Windows.Forms.Label label34;
		private System.Windows.Forms.Label label32;
		private TXTextBox txt_ClearEvent;
		private TXButton btnEventClear;
		private TXButton btnMeterClear;
		private TXButton btnDemandClear;
		private System.Windows.Forms.GroupBox groupBox45;
		private TXButton btn_MultifunctionalTerminalOutput;
		private TXRadioButton rb_MultifunctionalTerminalOutput_SwitchTime;
		private TXRadioButton rb_MultifunctionalTerminalOutput_Demand;
		private TXRadioButton rb_MultifunctionalTerminalOutput_Second;
		private TXComboBox cbCycle;
	}
}