using TX.Framework.WindowUI.Controls;

namespace WindowsTest
{
    partial class FormChipRegisters
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
			System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle3 = new System.Windows.Forms.DataGridViewCellStyle();
			System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle4 = new System.Windows.Forms.DataGridViewCellStyle();
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FormChipRegisters));
			this.splitContainer1 = new System.Windows.Forms.SplitContainer();
			this.gbRegisterThree = new System.Windows.Forms.GroupBox();
			this.tb_C相位1 = new System.Windows.Forms.TextBox();
			this.tb_A有功功率 = new System.Windows.Forms.TextBox();
			this.ckb_A有功增益 = new System.Windows.Forms.CheckBox();
			this.ckb_Toffset校正寄存器 = new System.Windows.Forms.CheckBox();
			this.tb_Toffset校正寄存器 = new System.Windows.Forms.TextBox();
			this.tb_B有功功率 = new System.Windows.Forms.TextBox();
			this.ckb_温度传感器数据寄存器 = new System.Windows.Forms.CheckBox();
			this.tb_温度传感器数据寄存器 = new System.Windows.Forms.TextBox();
			this.ckb_三相寄存器全选 = new System.Windows.Forms.CheckBox();
			this.btnSetRegister = new TX.Framework.WindowUI.Controls.TXButton();
			this.tb_A无功功率 = new System.Windows.Forms.TextBox();
			this.btnReadRegister = new TX.Framework.WindowUI.Controls.TXButton();
			this.ckb_B有功功率 = new System.Windows.Forms.CheckBox();
			this.ckb_相位补偿区域设置1 = new System.Windows.Forms.CheckBox();
			this.ckb_A无功功率 = new System.Windows.Forms.CheckBox();
			this.ckb_C有功功率 = new System.Windows.Forms.CheckBox();
			this.tb_相位补偿区域设置1 = new System.Windows.Forms.TextBox();
			this.tb_C有功功率 = new System.Windows.Forms.TextBox();
			this.tb_B无功功率 = new System.Windows.Forms.TextBox();
			this.ckb_相位补偿区域设置0 = new System.Windows.Forms.CheckBox();
			this.ckb_B无功功率 = new System.Windows.Forms.CheckBox();
			this.tb_相位补偿区域设置0 = new System.Windows.Forms.TextBox();
			this.tb_C无功功率 = new System.Windows.Forms.TextBox();
			this.ckb_C相位1 = new System.Windows.Forms.CheckBox();
			this.ckb_C无功功率 = new System.Windows.Forms.CheckBox();
			this.tb_启动电流阈值设置 = new System.Windows.Forms.TextBox();
			this.ckb_B相位1 = new System.Windows.Forms.CheckBox();
			this.ckb_启动电流阈值设置 = new System.Windows.Forms.CheckBox();
			this.tb_B相位1 = new System.Windows.Forms.TextBox();
			this.tb_A视在功率 = new System.Windows.Forms.TextBox();
			this.ckb_A相位1 = new System.Windows.Forms.CheckBox();
			this.ckb_A视在功率 = new System.Windows.Forms.CheckBox();
			this.tb_A相位1 = new System.Windows.Forms.TextBox();
			this.tb_B视在功率 = new System.Windows.Forms.TextBox();
			this.ckb_脉冲加倍寄存器 = new System.Windows.Forms.CheckBox();
			this.ckb_B视在功率 = new System.Windows.Forms.CheckBox();
			this.tb_脉冲加倍寄存器 = new System.Windows.Forms.TextBox();
			this.tb_C视在功率 = new System.Windows.Forms.TextBox();
			this.ckb_C电流 = new System.Windows.Forms.CheckBox();
			this.ckb_C视在功率 = new System.Windows.Forms.CheckBox();
			this.tb_C电流 = new System.Windows.Forms.TextBox();
			this.tb_A相位0 = new System.Windows.Forms.TextBox();
			this.ckb_B电流 = new System.Windows.Forms.CheckBox();
			this.ckb_A相位0 = new System.Windows.Forms.CheckBox();
			this.tb_B电流 = new System.Windows.Forms.TextBox();
			this.tb_B相位0 = new System.Windows.Forms.TextBox();
			this.ckb_A电流 = new System.Windows.Forms.CheckBox();
			this.ckb_B相位0 = new System.Windows.Forms.CheckBox();
			this.tb_A电流 = new System.Windows.Forms.TextBox();
			this.tb_C相位0 = new System.Windows.Forms.TextBox();
			this.ckb_C电压 = new System.Windows.Forms.CheckBox();
			this.ckb_C相位0 = new System.Windows.Forms.CheckBox();
			this.tb_C电压 = new System.Windows.Forms.TextBox();
			this.tb_高频脉冲输出设置 = new System.Windows.Forms.TextBox();
			this.ckb_B电压 = new System.Windows.Forms.CheckBox();
			this.ckb_高频脉冲输出设置 = new System.Windows.Forms.CheckBox();
			this.tb_B电压 = new System.Windows.Forms.TextBox();
			this.tb_A电压 = new System.Windows.Forms.TextBox();
			this.ckb_A电压 = new System.Windows.Forms.CheckBox();
			this.gbRegisterSingle = new System.Windows.Forms.GroupBox();
			this.tb_7017_L线有效值补偿 = new System.Windows.Forms.TextBox();
			this.btn_设7017 = new TX.Framework.WindowUI.Controls.TXButton();
			this.tbn_读7017 = new TX.Framework.WindowUI.Controls.TXButton();
			this.ckb_7017_L线有效值补偿 = new System.Windows.Forms.CheckBox();
			this.tb_7017_ADC通道增益选择 = new System.Windows.Forms.TextBox();
			this.ckb_7017_ADC通道增益选择 = new System.Windows.Forms.CheckBox();
			this.ckb_7017_全选 = new System.Windows.Forms.CheckBox();
			this.tb_7017_有功功率校正 = new System.Windows.Forms.TextBox();
			this.ckb_7017_有功功率校正 = new System.Windows.Forms.CheckBox();
			this.tb_7017_无功功率校正 = new System.Windows.Forms.TextBox();
			this.ckb_7017_无功功率校正 = new System.Windows.Forms.CheckBox();
			this.ckb_7017_视在功率校正 = new System.Windows.Forms.CheckBox();
			this.tb_7017_视在功率校正 = new System.Windows.Forms.TextBox();
			this.tb_7017_起动功率寄存器 = new System.Windows.Forms.TextBox();
			this.ckb_7017_起动功率寄存器 = new System.Windows.Forms.CheckBox();
			this.tb_7017_输出脉冲频率 = new System.Windows.Forms.TextBox();
			this.ckb_7017_输出脉冲频率 = new System.Windows.Forms.CheckBox();
			this.tb_7017_有功功率偏置校正 = new System.Windows.Forms.TextBox();
			this.ckb_7017_有功功率偏置校正 = new System.Windows.Forms.CheckBox();
			this.ckb_7017_L线有功功率偏置校正低字节 = new System.Windows.Forms.CheckBox();
			this.tb_7017_L线有功功率偏置校正低字节 = new System.Windows.Forms.TextBox();
			this.ckb_7017_相位校正 = new System.Windows.Forms.CheckBox();
			this.tb_7017_相位校正 = new System.Windows.Forms.TextBox();
			this.tb_7017_N线有效值补偿 = new System.Windows.Forms.TextBox();
			this.ckb_7017_N线有效值补偿 = new System.Windows.Forms.CheckBox();
			this.splitContainer2 = new System.Windows.Forms.SplitContainer();
			this.ttcRegisters = new TX.Framework.WindowUI.Controls.TXTabControl();
			this.tpRegistersHT = new System.Windows.Forms.TabPage();
			this.tptpRegistersRN = new System.Windows.Forms.TabPage();
			this.splitContainer3 = new System.Windows.Forms.SplitContainer();
			this.gbRegisterThreeRN = new System.Windows.Forms.GroupBox();
			this.dgvCommon = new System.Windows.Forms.DataGridView();
			this.dataGridViewCheckBoxColumn3 = new System.Windows.Forms.DataGridViewCheckBoxColumn();
			this.dataGridViewTextBoxColumn20 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.dataGridViewTextBoxColumn21 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.dataGridViewTextBoxColumn22 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.dataGridViewTextBoxColumn23 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.dataGridViewTextBoxColumn24 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.dataGridViewTextBoxColumn32 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.dataGridViewTextBoxColumn25 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.dataGridViewTextBoxColumn26 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.dataGridViewTextBoxColumn27 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.dataGridViewTextBoxColumn28 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.dataGridViewTextBoxColumn29 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.dataGridViewTextBoxColumn30 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.dataGridViewTextBoxColumn31 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.dataGridViewTextBoxColumn33 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.cbEEPROM = new System.Windows.Forms.CheckBox();
			this.tcbTypeRN = new TX.Framework.WindowUI.Controls.TXComboBox();
			this.tbWriteRegister = new TX.Framework.WindowUI.Controls.TXButton();
			this.tbReadRegister = new TX.Framework.WindowUI.Controls.TXButton();
			this.tpRegistersRN8302 = new System.Windows.Forms.TabPage();
			this.splitContainer4 = new System.Windows.Forms.SplitContainer();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.dgvRN8302 = new System.Windows.Forms.DataGridView();
			this.dataGridViewCheckBoxColumn1 = new System.Windows.Forms.DataGridViewCheckBoxColumn();
			this.dataGridViewTextBoxColumn1 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.dataGridViewTextBoxColumn2 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.dataGridViewTextBoxColumn3 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.dataGridViewTextBoxColumn4 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.dataGridViewTextBoxColumn5 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.dataGridViewTextBoxColumn6 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.dataGridViewTextBoxColumn7 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.dataGridViewTextBoxColumn8 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.dataGridViewTextBoxColumn9 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.dataGridViewTextBoxColumn10 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.dataGridViewTextBoxColumn11 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.dataGridViewTextBoxColumn12 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.dataGridViewTextBoxColumn13 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.dataGridViewTextBoxColumn14 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.cbEEPROM8302 = new System.Windows.Forms.CheckBox();
			this.txComboBox1 = new TX.Framework.WindowUI.Controls.TXComboBox();
			this.txbSetRN8302 = new TX.Framework.WindowUI.Controls.TXButton();
			this.txbReadRN8302 = new TX.Framework.WindowUI.Controls.TXButton();
			this.groupBox54 = new System.Windows.Forms.GroupBox();
			this.textBox_xiaobiaoData = new System.Windows.Forms.TextBox();
			this.label104 = new System.Windows.Forms.Label();
			this.button_xiaobiaoSet = new TX.Framework.WindowUI.Controls.TXButton();
			this.label105 = new System.Windows.Forms.Label();
			this.label106 = new System.Windows.Forms.Label();
			this.textBox_xiaobiaoLen = new System.Windows.Forms.TextBox();
			this.textBox_xiaobiaoAddr = new System.Windows.Forms.TextBox();
			this.button_xiaobiaoRead = new TX.Framework.WindowUI.Controls.TXButton();
			((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
			this.splitContainer1.Panel1.SuspendLayout();
			this.splitContainer1.Panel2.SuspendLayout();
			this.splitContainer1.SuspendLayout();
			this.gbRegisterThree.SuspendLayout();
			this.gbRegisterSingle.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).BeginInit();
			this.splitContainer2.Panel1.SuspendLayout();
			this.splitContainer2.Panel2.SuspendLayout();
			this.splitContainer2.SuspendLayout();
			this.ttcRegisters.SuspendLayout();
			this.tpRegistersHT.SuspendLayout();
			this.tptpRegistersRN.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.splitContainer3)).BeginInit();
			this.splitContainer3.Panel1.SuspendLayout();
			this.splitContainer3.Panel2.SuspendLayout();
			this.splitContainer3.SuspendLayout();
			this.gbRegisterThreeRN.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.dgvCommon)).BeginInit();
			this.tpRegistersRN8302.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.splitContainer4)).BeginInit();
			this.splitContainer4.Panel1.SuspendLayout();
			this.splitContainer4.Panel2.SuspendLayout();
			this.splitContainer4.SuspendLayout();
			this.groupBox1.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.dgvRN8302)).BeginInit();
			this.groupBox54.SuspendLayout();
			this.SuspendLayout();
			// 
			// splitContainer1
			// 
			this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.splitContainer1.Location = new System.Drawing.Point(3, 3);
			this.splitContainer1.Name = "splitContainer1";
			// 
			// splitContainer1.Panel1
			// 
			this.splitContainer1.Panel1.Controls.Add(this.gbRegisterThree);
			// 
			// splitContainer1.Panel2
			// 
			this.splitContainer1.Panel2.Controls.Add(this.gbRegisterSingle);
			this.splitContainer1.Size = new System.Drawing.Size(941, 623);
			this.splitContainer1.SplitterDistance = 516;
			this.splitContainer1.TabIndex = 0;
			// 
			// gbRegisterThree
			// 
			this.gbRegisterThree.Controls.Add(this.tb_C相位1);
			this.gbRegisterThree.Controls.Add(this.tb_A有功功率);
			this.gbRegisterThree.Controls.Add(this.ckb_A有功增益);
			this.gbRegisterThree.Controls.Add(this.ckb_Toffset校正寄存器);
			this.gbRegisterThree.Controls.Add(this.tb_Toffset校正寄存器);
			this.gbRegisterThree.Controls.Add(this.tb_B有功功率);
			this.gbRegisterThree.Controls.Add(this.ckb_温度传感器数据寄存器);
			this.gbRegisterThree.Controls.Add(this.tb_温度传感器数据寄存器);
			this.gbRegisterThree.Controls.Add(this.ckb_三相寄存器全选);
			this.gbRegisterThree.Controls.Add(this.btnSetRegister);
			this.gbRegisterThree.Controls.Add(this.tb_A无功功率);
			this.gbRegisterThree.Controls.Add(this.btnReadRegister);
			this.gbRegisterThree.Controls.Add(this.ckb_B有功功率);
			this.gbRegisterThree.Controls.Add(this.ckb_相位补偿区域设置1);
			this.gbRegisterThree.Controls.Add(this.ckb_A无功功率);
			this.gbRegisterThree.Controls.Add(this.ckb_C有功功率);
			this.gbRegisterThree.Controls.Add(this.tb_相位补偿区域设置1);
			this.gbRegisterThree.Controls.Add(this.tb_C有功功率);
			this.gbRegisterThree.Controls.Add(this.tb_B无功功率);
			this.gbRegisterThree.Controls.Add(this.ckb_相位补偿区域设置0);
			this.gbRegisterThree.Controls.Add(this.ckb_B无功功率);
			this.gbRegisterThree.Controls.Add(this.tb_相位补偿区域设置0);
			this.gbRegisterThree.Controls.Add(this.tb_C无功功率);
			this.gbRegisterThree.Controls.Add(this.ckb_C相位1);
			this.gbRegisterThree.Controls.Add(this.ckb_C无功功率);
			this.gbRegisterThree.Controls.Add(this.tb_启动电流阈值设置);
			this.gbRegisterThree.Controls.Add(this.ckb_B相位1);
			this.gbRegisterThree.Controls.Add(this.ckb_启动电流阈值设置);
			this.gbRegisterThree.Controls.Add(this.tb_B相位1);
			this.gbRegisterThree.Controls.Add(this.tb_A视在功率);
			this.gbRegisterThree.Controls.Add(this.ckb_A相位1);
			this.gbRegisterThree.Controls.Add(this.ckb_A视在功率);
			this.gbRegisterThree.Controls.Add(this.tb_A相位1);
			this.gbRegisterThree.Controls.Add(this.tb_B视在功率);
			this.gbRegisterThree.Controls.Add(this.ckb_脉冲加倍寄存器);
			this.gbRegisterThree.Controls.Add(this.ckb_B视在功率);
			this.gbRegisterThree.Controls.Add(this.tb_脉冲加倍寄存器);
			this.gbRegisterThree.Controls.Add(this.tb_C视在功率);
			this.gbRegisterThree.Controls.Add(this.ckb_C电流);
			this.gbRegisterThree.Controls.Add(this.ckb_C视在功率);
			this.gbRegisterThree.Controls.Add(this.tb_C电流);
			this.gbRegisterThree.Controls.Add(this.tb_A相位0);
			this.gbRegisterThree.Controls.Add(this.ckb_B电流);
			this.gbRegisterThree.Controls.Add(this.ckb_A相位0);
			this.gbRegisterThree.Controls.Add(this.tb_B电流);
			this.gbRegisterThree.Controls.Add(this.tb_B相位0);
			this.gbRegisterThree.Controls.Add(this.ckb_A电流);
			this.gbRegisterThree.Controls.Add(this.ckb_B相位0);
			this.gbRegisterThree.Controls.Add(this.tb_A电流);
			this.gbRegisterThree.Controls.Add(this.tb_C相位0);
			this.gbRegisterThree.Controls.Add(this.ckb_C电压);
			this.gbRegisterThree.Controls.Add(this.ckb_C相位0);
			this.gbRegisterThree.Controls.Add(this.tb_C电压);
			this.gbRegisterThree.Controls.Add(this.tb_高频脉冲输出设置);
			this.gbRegisterThree.Controls.Add(this.ckb_B电压);
			this.gbRegisterThree.Controls.Add(this.ckb_高频脉冲输出设置);
			this.gbRegisterThree.Controls.Add(this.tb_B电压);
			this.gbRegisterThree.Controls.Add(this.tb_A电压);
			this.gbRegisterThree.Controls.Add(this.ckb_A电压);
			this.gbRegisterThree.Dock = System.Windows.Forms.DockStyle.Fill;
			this.gbRegisterThree.Location = new System.Drawing.Point(0, 0);
			this.gbRegisterThree.Name = "gbRegisterThree";
			this.gbRegisterThree.Size = new System.Drawing.Size(516, 623);
			this.gbRegisterThree.TabIndex = 104;
			this.gbRegisterThree.TabStop = false;
			this.gbRegisterThree.Text = "三相";
			// 
			// tb_C相位1
			// 
			this.tb_C相位1.Location = new System.Drawing.Point(17, 440);
			this.tb_C相位1.Name = "tb_C相位1";
			this.tb_C相位1.Size = new System.Drawing.Size(66, 21);
			this.tb_C相位1.TabIndex = 90;
			// 
			// tb_A有功功率
			// 
			this.tb_A有功功率.Location = new System.Drawing.Point(17, 20);
			this.tb_A有功功率.Name = "tb_A有功功率";
			this.tb_A有功功率.Size = new System.Drawing.Size(66, 21);
			this.tb_A有功功率.TabIndex = 38;
			// 
			// ckb_A有功增益
			// 
			this.ckb_A有功增益.AutoSize = true;
			this.ckb_A有功增益.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.ckb_A有功增益.Location = new System.Drawing.Point(89, 22);
			this.ckb_A有功增益.Name = "ckb_A有功增益";
			this.ckb_A有功增益.Size = new System.Drawing.Size(114, 16);
			this.ckb_A有功增益.TabIndex = 39;
			this.ckb_A有功增益.Text = "A相有功功率增益";
			this.ckb_A有功增益.UseVisualStyleBackColor = true;
			// 
			// ckb_Toffset校正寄存器
			// 
			this.ckb_Toffset校正寄存器.AutoSize = true;
			this.ckb_Toffset校正寄存器.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.ckb_Toffset校正寄存器.Location = new System.Drawing.Point(281, 352);
			this.ckb_Toffset校正寄存器.Name = "ckb_Toffset校正寄存器";
			this.ckb_Toffset校正寄存器.Size = new System.Drawing.Size(126, 16);
			this.ckb_Toffset校正寄存器.TabIndex = 99;
			this.ckb_Toffset校正寄存器.Text = "Toffset校正寄存器";
			this.ckb_Toffset校正寄存器.UseVisualStyleBackColor = true;
			// 
			// tb_Toffset校正寄存器
			// 
			this.tb_Toffset校正寄存器.Location = new System.Drawing.Point(209, 350);
			this.tb_Toffset校正寄存器.Name = "tb_Toffset校正寄存器";
			this.tb_Toffset校正寄存器.Size = new System.Drawing.Size(66, 21);
			this.tb_Toffset校正寄存器.TabIndex = 98;
			// 
			// tb_B有功功率
			// 
			this.tb_B有功功率.Location = new System.Drawing.Point(17, 50);
			this.tb_B有功功率.Name = "tb_B有功功率";
			this.tb_B有功功率.Size = new System.Drawing.Size(66, 21);
			this.tb_B有功功率.TabIndex = 40;
			// 
			// ckb_温度传感器数据寄存器
			// 
			this.ckb_温度传感器数据寄存器.AutoSize = true;
			this.ckb_温度传感器数据寄存器.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.ckb_温度传感器数据寄存器.Location = new System.Drawing.Point(281, 382);
			this.ckb_温度传感器数据寄存器.Name = "ckb_温度传感器数据寄存器";
			this.ckb_温度传感器数据寄存器.Size = new System.Drawing.Size(144, 16);
			this.ckb_温度传感器数据寄存器.TabIndex = 97;
			this.ckb_温度传感器数据寄存器.Text = "温度传感器数据寄存器";
			this.ckb_温度传感器数据寄存器.UseVisualStyleBackColor = true;
			// 
			// tb_温度传感器数据寄存器
			// 
			this.tb_温度传感器数据寄存器.Location = new System.Drawing.Point(209, 380);
			this.tb_温度传感器数据寄存器.Name = "tb_温度传感器数据寄存器";
			this.tb_温度传感器数据寄存器.Size = new System.Drawing.Size(66, 21);
			this.tb_温度传感器数据寄存器.TabIndex = 96;
			// 
			// ckb_三相寄存器全选
			// 
			this.ckb_三相寄存器全选.AutoSize = true;
			this.ckb_三相寄存器全选.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.ckb_三相寄存器全选.Location = new System.Drawing.Point(281, 415);
			this.ckb_三相寄存器全选.Name = "ckb_三相寄存器全选";
			this.ckb_三相寄存器全选.Size = new System.Drawing.Size(48, 16);
			this.ckb_三相寄存器全选.TabIndex = 100;
			this.ckb_三相寄存器全选.Text = "全选";
			this.ckb_三相寄存器全选.UseVisualStyleBackColor = true;
			this.ckb_三相寄存器全选.CheckedChanged += new System.EventHandler(this.ckb_三相寄存器全选_CheckedChanged);
			// 
			// btnSetRegister
			// 
			this.btnSetRegister.Image = null;
			this.btnSetRegister.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.btnSetRegister.Location = new System.Drawing.Point(209, 445);
			this.btnSetRegister.Name = "btnSetRegister";
			this.btnSetRegister.Size = new System.Drawing.Size(66, 23);
			this.btnSetRegister.TabIndex = 102;
			this.btnSetRegister.Text = "设数据";
			this.btnSetRegister.UseVisualStyleBackColor = true;
			this.btnSetRegister.Click += new System.EventHandler(this.btnSetRegister_Click);
			// 
			// tb_A无功功率
			// 
			this.tb_A无功功率.Location = new System.Drawing.Point(17, 110);
			this.tb_A无功功率.Name = "tb_A无功功率";
			this.tb_A无功功率.Size = new System.Drawing.Size(66, 21);
			this.tb_A无功功率.TabIndex = 44;
			// 
			// btnReadRegister
			// 
			this.btnReadRegister.Image = null;
			this.btnReadRegister.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.btnReadRegister.Location = new System.Drawing.Point(209, 412);
			this.btnReadRegister.Name = "btnReadRegister";
			this.btnReadRegister.Size = new System.Drawing.Size(66, 23);
			this.btnReadRegister.TabIndex = 101;
			this.btnReadRegister.Text = "读数据";
			this.btnReadRegister.UseVisualStyleBackColor = true;
			this.btnReadRegister.Click += new System.EventHandler(this.btnReadRegister_Click);
			// 
			// ckb_B有功功率
			// 
			this.ckb_B有功功率.AutoSize = true;
			this.ckb_B有功功率.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.ckb_B有功功率.Location = new System.Drawing.Point(89, 52);
			this.ckb_B有功功率.Name = "ckb_B有功功率";
			this.ckb_B有功功率.Size = new System.Drawing.Size(114, 16);
			this.ckb_B有功功率.TabIndex = 41;
			this.ckb_B有功功率.Text = "B相有功功率增益";
			this.ckb_B有功功率.UseVisualStyleBackColor = true;
			// 
			// ckb_相位补偿区域设置1
			// 
			this.ckb_相位补偿区域设置1.AutoSize = true;
			this.ckb_相位补偿区域设置1.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.ckb_相位补偿区域设置1.Location = new System.Drawing.Point(281, 322);
			this.ckb_相位补偿区域设置1.Name = "ckb_相位补偿区域设置1";
			this.ckb_相位补偿区域设置1.Size = new System.Drawing.Size(126, 16);
			this.ckb_相位补偿区域设置1.TabIndex = 95;
			this.ckb_相位补偿区域设置1.Text = "相位补偿区域设置1";
			this.ckb_相位补偿区域设置1.UseVisualStyleBackColor = true;
			// 
			// ckb_A无功功率
			// 
			this.ckb_A无功功率.AutoSize = true;
			this.ckb_A无功功率.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.ckb_A无功功率.Location = new System.Drawing.Point(89, 112);
			this.ckb_A无功功率.Name = "ckb_A无功功率";
			this.ckb_A无功功率.Size = new System.Drawing.Size(114, 16);
			this.ckb_A无功功率.TabIndex = 45;
			this.ckb_A无功功率.Text = "A相无功功率增益";
			this.ckb_A无功功率.UseVisualStyleBackColor = true;
			// 
			// ckb_C有功功率
			// 
			this.ckb_C有功功率.AutoSize = true;
			this.ckb_C有功功率.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.ckb_C有功功率.Location = new System.Drawing.Point(89, 82);
			this.ckb_C有功功率.Name = "ckb_C有功功率";
			this.ckb_C有功功率.Size = new System.Drawing.Size(114, 16);
			this.ckb_C有功功率.TabIndex = 43;
			this.ckb_C有功功率.Text = "C相有功功率增益";
			this.ckb_C有功功率.UseVisualStyleBackColor = true;
			// 
			// tb_相位补偿区域设置1
			// 
			this.tb_相位补偿区域设置1.Location = new System.Drawing.Point(209, 320);
			this.tb_相位补偿区域设置1.Name = "tb_相位补偿区域设置1";
			this.tb_相位补偿区域设置1.Size = new System.Drawing.Size(66, 21);
			this.tb_相位补偿区域设置1.TabIndex = 94;
			// 
			// tb_C有功功率
			// 
			this.tb_C有功功率.Location = new System.Drawing.Point(17, 80);
			this.tb_C有功功率.Name = "tb_C有功功率";
			this.tb_C有功功率.Size = new System.Drawing.Size(66, 21);
			this.tb_C有功功率.TabIndex = 42;
			// 
			// tb_B无功功率
			// 
			this.tb_B无功功率.Location = new System.Drawing.Point(17, 140);
			this.tb_B无功功率.Name = "tb_B无功功率";
			this.tb_B无功功率.Size = new System.Drawing.Size(66, 21);
			this.tb_B无功功率.TabIndex = 46;
			// 
			// ckb_相位补偿区域设置0
			// 
			this.ckb_相位补偿区域设置0.AutoSize = true;
			this.ckb_相位补偿区域设置0.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.ckb_相位补偿区域设置0.Location = new System.Drawing.Point(281, 292);
			this.ckb_相位补偿区域设置0.Name = "ckb_相位补偿区域设置0";
			this.ckb_相位补偿区域设置0.Size = new System.Drawing.Size(126, 16);
			this.ckb_相位补偿区域设置0.TabIndex = 93;
			this.ckb_相位补偿区域设置0.Text = "相位补偿区域设置0";
			this.ckb_相位补偿区域设置0.UseVisualStyleBackColor = true;
			// 
			// ckb_B无功功率
			// 
			this.ckb_B无功功率.AutoSize = true;
			this.ckb_B无功功率.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.ckb_B无功功率.Location = new System.Drawing.Point(89, 142);
			this.ckb_B无功功率.Name = "ckb_B无功功率";
			this.ckb_B无功功率.Size = new System.Drawing.Size(114, 16);
			this.ckb_B无功功率.TabIndex = 47;
			this.ckb_B无功功率.Text = "B相无功功率增益";
			this.ckb_B无功功率.UseVisualStyleBackColor = true;
			// 
			// tb_相位补偿区域设置0
			// 
			this.tb_相位补偿区域设置0.Location = new System.Drawing.Point(209, 290);
			this.tb_相位补偿区域设置0.Name = "tb_相位补偿区域设置0";
			this.tb_相位补偿区域设置0.Size = new System.Drawing.Size(66, 21);
			this.tb_相位补偿区域设置0.TabIndex = 92;
			// 
			// tb_C无功功率
			// 
			this.tb_C无功功率.Location = new System.Drawing.Point(17, 170);
			this.tb_C无功功率.Name = "tb_C无功功率";
			this.tb_C无功功率.Size = new System.Drawing.Size(66, 21);
			this.tb_C无功功率.TabIndex = 48;
			// 
			// ckb_C相位1
			// 
			this.ckb_C相位1.AutoSize = true;
			this.ckb_C相位1.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.ckb_C相位1.Location = new System.Drawing.Point(89, 442);
			this.ckb_C相位1.Name = "ckb_C相位1";
			this.ckb_C相位1.Size = new System.Drawing.Size(96, 16);
			this.ckb_C相位1.TabIndex = 91;
			this.ckb_C相位1.Text = "C相相位校正1";
			this.ckb_C相位1.UseVisualStyleBackColor = true;
			// 
			// ckb_C无功功率
			// 
			this.ckb_C无功功率.AutoSize = true;
			this.ckb_C无功功率.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.ckb_C无功功率.Location = new System.Drawing.Point(89, 172);
			this.ckb_C无功功率.Name = "ckb_C无功功率";
			this.ckb_C无功功率.Size = new System.Drawing.Size(114, 16);
			this.ckb_C无功功率.TabIndex = 49;
			this.ckb_C无功功率.Text = "C相无功功率增益";
			this.ckb_C无功功率.UseVisualStyleBackColor = true;
			// 
			// tb_启动电流阈值设置
			// 
			this.tb_启动电流阈值设置.Location = new System.Drawing.Point(209, 200);
			this.tb_启动电流阈值设置.Name = "tb_启动电流阈值设置";
			this.tb_启动电流阈值设置.Size = new System.Drawing.Size(66, 21);
			this.tb_启动电流阈值设置.TabIndex = 50;
			// 
			// ckb_B相位1
			// 
			this.ckb_B相位1.AutoSize = true;
			this.ckb_B相位1.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.ckb_B相位1.Location = new System.Drawing.Point(89, 412);
			this.ckb_B相位1.Name = "ckb_B相位1";
			this.ckb_B相位1.Size = new System.Drawing.Size(96, 16);
			this.ckb_B相位1.TabIndex = 89;
			this.ckb_B相位1.Text = "B相相位校正1";
			this.ckb_B相位1.UseVisualStyleBackColor = true;
			// 
			// ckb_启动电流阈值设置
			// 
			this.ckb_启动电流阈值设置.AutoSize = true;
			this.ckb_启动电流阈值设置.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.ckb_启动电流阈值设置.Location = new System.Drawing.Point(281, 202);
			this.ckb_启动电流阈值设置.Name = "ckb_启动电流阈值设置";
			this.ckb_启动电流阈值设置.Size = new System.Drawing.Size(96, 16);
			this.ckb_启动电流阈值设置.TabIndex = 51;
			this.ckb_启动电流阈值设置.Text = "启动电流阈值";
			this.ckb_启动电流阈值设置.UseVisualStyleBackColor = true;
			// 
			// tb_B相位1
			// 
			this.tb_B相位1.Location = new System.Drawing.Point(17, 410);
			this.tb_B相位1.Name = "tb_B相位1";
			this.tb_B相位1.Size = new System.Drawing.Size(66, 21);
			this.tb_B相位1.TabIndex = 88;
			// 
			// tb_A视在功率
			// 
			this.tb_A视在功率.Location = new System.Drawing.Point(17, 200);
			this.tb_A视在功率.Name = "tb_A视在功率";
			this.tb_A视在功率.Size = new System.Drawing.Size(66, 21);
			this.tb_A视在功率.TabIndex = 54;
			// 
			// ckb_A相位1
			// 
			this.ckb_A相位1.AutoSize = true;
			this.ckb_A相位1.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.ckb_A相位1.Location = new System.Drawing.Point(89, 382);
			this.ckb_A相位1.Name = "ckb_A相位1";
			this.ckb_A相位1.Size = new System.Drawing.Size(96, 16);
			this.ckb_A相位1.TabIndex = 87;
			this.ckb_A相位1.Text = "A相相位校正1";
			this.ckb_A相位1.UseVisualStyleBackColor = true;
			// 
			// ckb_A视在功率
			// 
			this.ckb_A视在功率.AutoSize = true;
			this.ckb_A视在功率.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.ckb_A视在功率.Location = new System.Drawing.Point(89, 202);
			this.ckb_A视在功率.Name = "ckb_A视在功率";
			this.ckb_A视在功率.Size = new System.Drawing.Size(114, 16);
			this.ckb_A视在功率.TabIndex = 55;
			this.ckb_A视在功率.Text = "A相视在功率增益";
			this.ckb_A视在功率.UseVisualStyleBackColor = true;
			// 
			// tb_A相位1
			// 
			this.tb_A相位1.Location = new System.Drawing.Point(17, 380);
			this.tb_A相位1.Name = "tb_A相位1";
			this.tb_A相位1.Size = new System.Drawing.Size(66, 21);
			this.tb_A相位1.TabIndex = 86;
			// 
			// tb_B视在功率
			// 
			this.tb_B视在功率.Location = new System.Drawing.Point(17, 230);
			this.tb_B视在功率.Name = "tb_B视在功率";
			this.tb_B视在功率.Size = new System.Drawing.Size(66, 21);
			this.tb_B视在功率.TabIndex = 56;
			// 
			// ckb_脉冲加倍寄存器
			// 
			this.ckb_脉冲加倍寄存器.AutoSize = true;
			this.ckb_脉冲加倍寄存器.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.ckb_脉冲加倍寄存器.Location = new System.Drawing.Point(281, 262);
			this.ckb_脉冲加倍寄存器.Name = "ckb_脉冲加倍寄存器";
			this.ckb_脉冲加倍寄存器.Size = new System.Drawing.Size(108, 16);
			this.ckb_脉冲加倍寄存器.TabIndex = 83;
			this.ckb_脉冲加倍寄存器.Text = "脉冲加倍寄存器";
			this.ckb_脉冲加倍寄存器.UseVisualStyleBackColor = true;
			// 
			// ckb_B视在功率
			// 
			this.ckb_B视在功率.AutoSize = true;
			this.ckb_B视在功率.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.ckb_B视在功率.Location = new System.Drawing.Point(89, 232);
			this.ckb_B视在功率.Name = "ckb_B视在功率";
			this.ckb_B视在功率.Size = new System.Drawing.Size(114, 16);
			this.ckb_B视在功率.TabIndex = 57;
			this.ckb_B视在功率.Text = "B相视在功率增益";
			this.ckb_B视在功率.UseVisualStyleBackColor = true;
			// 
			// tb_脉冲加倍寄存器
			// 
			this.tb_脉冲加倍寄存器.Location = new System.Drawing.Point(209, 260);
			this.tb_脉冲加倍寄存器.Name = "tb_脉冲加倍寄存器";
			this.tb_脉冲加倍寄存器.Size = new System.Drawing.Size(66, 21);
			this.tb_脉冲加倍寄存器.TabIndex = 82;
			// 
			// tb_C视在功率
			// 
			this.tb_C视在功率.Location = new System.Drawing.Point(17, 260);
			this.tb_C视在功率.Name = "tb_C视在功率";
			this.tb_C视在功率.Size = new System.Drawing.Size(66, 21);
			this.tb_C视在功率.TabIndex = 58;
			// 
			// ckb_C电流
			// 
			this.ckb_C电流.AutoSize = true;
			this.ckb_C电流.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.ckb_C电流.Location = new System.Drawing.Point(281, 172);
			this.ckb_C电流.Name = "ckb_C电流";
			this.ckb_C电流.Size = new System.Drawing.Size(90, 16);
			this.ckb_C电流.TabIndex = 81;
			this.ckb_C电流.Text = "C相电流增益";
			this.ckb_C电流.UseVisualStyleBackColor = true;
			// 
			// ckb_C视在功率
			// 
			this.ckb_C视在功率.AutoSize = true;
			this.ckb_C视在功率.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.ckb_C视在功率.Location = new System.Drawing.Point(89, 262);
			this.ckb_C视在功率.Name = "ckb_C视在功率";
			this.ckb_C视在功率.Size = new System.Drawing.Size(114, 16);
			this.ckb_C视在功率.TabIndex = 59;
			this.ckb_C视在功率.Text = "C相视在功率增益";
			this.ckb_C视在功率.UseVisualStyleBackColor = true;
			// 
			// tb_C电流
			// 
			this.tb_C电流.Location = new System.Drawing.Point(209, 170);
			this.tb_C电流.Name = "tb_C电流";
			this.tb_C电流.Size = new System.Drawing.Size(66, 21);
			this.tb_C电流.TabIndex = 80;
			// 
			// tb_A相位0
			// 
			this.tb_A相位0.Location = new System.Drawing.Point(17, 290);
			this.tb_A相位0.Name = "tb_A相位0";
			this.tb_A相位0.Size = new System.Drawing.Size(66, 21);
			this.tb_A相位0.TabIndex = 60;
			// 
			// ckb_B电流
			// 
			this.ckb_B电流.AutoSize = true;
			this.ckb_B电流.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.ckb_B电流.Location = new System.Drawing.Point(281, 142);
			this.ckb_B电流.Name = "ckb_B电流";
			this.ckb_B电流.Size = new System.Drawing.Size(90, 16);
			this.ckb_B电流.TabIndex = 79;
			this.ckb_B电流.Text = "B相电流增益";
			this.ckb_B电流.UseVisualStyleBackColor = true;
			// 
			// ckb_A相位0
			// 
			this.ckb_A相位0.AutoSize = true;
			this.ckb_A相位0.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.ckb_A相位0.Location = new System.Drawing.Point(89, 292);
			this.ckb_A相位0.Name = "ckb_A相位0";
			this.ckb_A相位0.Size = new System.Drawing.Size(96, 16);
			this.ckb_A相位0.TabIndex = 61;
			this.ckb_A相位0.Text = "A相相位校正0";
			this.ckb_A相位0.UseVisualStyleBackColor = true;
			// 
			// tb_B电流
			// 
			this.tb_B电流.Location = new System.Drawing.Point(209, 140);
			this.tb_B电流.Name = "tb_B电流";
			this.tb_B电流.Size = new System.Drawing.Size(66, 21);
			this.tb_B电流.TabIndex = 78;
			// 
			// tb_B相位0
			// 
			this.tb_B相位0.Location = new System.Drawing.Point(17, 320);
			this.tb_B相位0.Name = "tb_B相位0";
			this.tb_B相位0.Size = new System.Drawing.Size(66, 21);
			this.tb_B相位0.TabIndex = 62;
			// 
			// ckb_A电流
			// 
			this.ckb_A电流.AutoSize = true;
			this.ckb_A电流.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.ckb_A电流.Location = new System.Drawing.Point(281, 112);
			this.ckb_A电流.Name = "ckb_A电流";
			this.ckb_A电流.Size = new System.Drawing.Size(90, 16);
			this.ckb_A电流.TabIndex = 77;
			this.ckb_A电流.Text = "A相电流增益";
			this.ckb_A电流.UseVisualStyleBackColor = true;
			// 
			// ckb_B相位0
			// 
			this.ckb_B相位0.AutoSize = true;
			this.ckb_B相位0.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.ckb_B相位0.Location = new System.Drawing.Point(89, 322);
			this.ckb_B相位0.Name = "ckb_B相位0";
			this.ckb_B相位0.Size = new System.Drawing.Size(96, 16);
			this.ckb_B相位0.TabIndex = 63;
			this.ckb_B相位0.Text = "B相相位校正0";
			this.ckb_B相位0.UseVisualStyleBackColor = true;
			// 
			// tb_A电流
			// 
			this.tb_A电流.Location = new System.Drawing.Point(209, 110);
			this.tb_A电流.Name = "tb_A电流";
			this.tb_A电流.Size = new System.Drawing.Size(66, 21);
			this.tb_A电流.TabIndex = 76;
			// 
			// tb_C相位0
			// 
			this.tb_C相位0.Location = new System.Drawing.Point(17, 350);
			this.tb_C相位0.Name = "tb_C相位0";
			this.tb_C相位0.Size = new System.Drawing.Size(66, 21);
			this.tb_C相位0.TabIndex = 64;
			// 
			// ckb_C电压
			// 
			this.ckb_C电压.AutoSize = true;
			this.ckb_C电压.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.ckb_C电压.Location = new System.Drawing.Point(281, 82);
			this.ckb_C电压.Name = "ckb_C电压";
			this.ckb_C电压.Size = new System.Drawing.Size(90, 16);
			this.ckb_C电压.TabIndex = 75;
			this.ckb_C电压.Text = "C相电压增益";
			this.ckb_C电压.UseVisualStyleBackColor = true;
			// 
			// ckb_C相位0
			// 
			this.ckb_C相位0.AutoSize = true;
			this.ckb_C相位0.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.ckb_C相位0.Location = new System.Drawing.Point(89, 352);
			this.ckb_C相位0.Name = "ckb_C相位0";
			this.ckb_C相位0.Size = new System.Drawing.Size(96, 16);
			this.ckb_C相位0.TabIndex = 65;
			this.ckb_C相位0.Text = "C相相位校正0";
			this.ckb_C相位0.UseVisualStyleBackColor = true;
			// 
			// tb_C电压
			// 
			this.tb_C电压.Location = new System.Drawing.Point(209, 80);
			this.tb_C电压.Name = "tb_C电压";
			this.tb_C电压.Size = new System.Drawing.Size(66, 21);
			this.tb_C电压.TabIndex = 74;
			// 
			// tb_高频脉冲输出设置
			// 
			this.tb_高频脉冲输出设置.Location = new System.Drawing.Point(209, 230);
			this.tb_高频脉冲输出设置.Name = "tb_高频脉冲输出设置";
			this.tb_高频脉冲输出设置.Size = new System.Drawing.Size(66, 21);
			this.tb_高频脉冲输出设置.TabIndex = 66;
			// 
			// ckb_B电压
			// 
			this.ckb_B电压.AutoSize = true;
			this.ckb_B电压.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.ckb_B电压.Location = new System.Drawing.Point(281, 52);
			this.ckb_B电压.Name = "ckb_B电压";
			this.ckb_B电压.Size = new System.Drawing.Size(90, 16);
			this.ckb_B电压.TabIndex = 73;
			this.ckb_B电压.Text = "B相电压增益";
			this.ckb_B电压.UseVisualStyleBackColor = true;
			// 
			// ckb_高频脉冲输出设置
			// 
			this.ckb_高频脉冲输出设置.AutoSize = true;
			this.ckb_高频脉冲输出设置.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.ckb_高频脉冲输出设置.Location = new System.Drawing.Point(281, 232);
			this.ckb_高频脉冲输出设置.Name = "ckb_高频脉冲输出设置";
			this.ckb_高频脉冲输出设置.Size = new System.Drawing.Size(120, 16);
			this.ckb_高频脉冲输出设置.TabIndex = 67;
			this.ckb_高频脉冲输出设置.Text = "高频脉冲输出设置";
			this.ckb_高频脉冲输出设置.UseVisualStyleBackColor = true;
			// 
			// tb_B电压
			// 
			this.tb_B电压.Location = new System.Drawing.Point(209, 50);
			this.tb_B电压.Name = "tb_B电压";
			this.tb_B电压.Size = new System.Drawing.Size(66, 21);
			this.tb_B电压.TabIndex = 72;
			// 
			// tb_A电压
			// 
			this.tb_A电压.Location = new System.Drawing.Point(209, 20);
			this.tb_A电压.Name = "tb_A电压";
			this.tb_A电压.Size = new System.Drawing.Size(66, 21);
			this.tb_A电压.TabIndex = 70;
			// 
			// ckb_A电压
			// 
			this.ckb_A电压.AutoSize = true;
			this.ckb_A电压.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.ckb_A电压.Location = new System.Drawing.Point(281, 22);
			this.ckb_A电压.Name = "ckb_A电压";
			this.ckb_A电压.Size = new System.Drawing.Size(90, 16);
			this.ckb_A电压.TabIndex = 71;
			this.ckb_A电压.Text = "A相电压增益";
			this.ckb_A电压.UseVisualStyleBackColor = true;
			// 
			// gbRegisterSingle
			// 
			this.gbRegisterSingle.Controls.Add(this.tb_7017_L线有效值补偿);
			this.gbRegisterSingle.Controls.Add(this.btn_设7017);
			this.gbRegisterSingle.Controls.Add(this.tbn_读7017);
			this.gbRegisterSingle.Controls.Add(this.ckb_7017_L线有效值补偿);
			this.gbRegisterSingle.Controls.Add(this.tb_7017_ADC通道增益选择);
			this.gbRegisterSingle.Controls.Add(this.ckb_7017_ADC通道增益选择);
			this.gbRegisterSingle.Controls.Add(this.ckb_7017_全选);
			this.gbRegisterSingle.Controls.Add(this.tb_7017_有功功率校正);
			this.gbRegisterSingle.Controls.Add(this.ckb_7017_有功功率校正);
			this.gbRegisterSingle.Controls.Add(this.tb_7017_无功功率校正);
			this.gbRegisterSingle.Controls.Add(this.ckb_7017_无功功率校正);
			this.gbRegisterSingle.Controls.Add(this.ckb_7017_视在功率校正);
			this.gbRegisterSingle.Controls.Add(this.tb_7017_视在功率校正);
			this.gbRegisterSingle.Controls.Add(this.tb_7017_起动功率寄存器);
			this.gbRegisterSingle.Controls.Add(this.ckb_7017_起动功率寄存器);
			this.gbRegisterSingle.Controls.Add(this.tb_7017_输出脉冲频率);
			this.gbRegisterSingle.Controls.Add(this.ckb_7017_输出脉冲频率);
			this.gbRegisterSingle.Controls.Add(this.tb_7017_有功功率偏置校正);
			this.gbRegisterSingle.Controls.Add(this.ckb_7017_有功功率偏置校正);
			this.gbRegisterSingle.Controls.Add(this.ckb_7017_L线有功功率偏置校正低字节);
			this.gbRegisterSingle.Controls.Add(this.tb_7017_L线有功功率偏置校正低字节);
			this.gbRegisterSingle.Controls.Add(this.ckb_7017_相位校正);
			this.gbRegisterSingle.Controls.Add(this.tb_7017_相位校正);
			this.gbRegisterSingle.Controls.Add(this.tb_7017_N线有效值补偿);
			this.gbRegisterSingle.Controls.Add(this.ckb_7017_N线有效值补偿);
			this.gbRegisterSingle.Dock = System.Windows.Forms.DockStyle.Fill;
			this.gbRegisterSingle.Location = new System.Drawing.Point(0, 0);
			this.gbRegisterSingle.Name = "gbRegisterSingle";
			this.gbRegisterSingle.Size = new System.Drawing.Size(421, 623);
			this.gbRegisterSingle.TabIndex = 4;
			this.gbRegisterSingle.TabStop = false;
			this.gbRegisterSingle.Text = "单相(HT7017)";
			// 
			// tb_7017_L线有效值补偿
			// 
			this.tb_7017_L线有效值补偿.Location = new System.Drawing.Point(7, 213);
			this.tb_7017_L线有效值补偿.Name = "tb_7017_L线有效值补偿";
			this.tb_7017_L线有效值补偿.Size = new System.Drawing.Size(66, 21);
			this.tb_7017_L线有效值补偿.TabIndex = 107;
			// 
			// btn_设7017
			// 
			this.btn_设7017.Image = null;
			this.btn_设7017.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.btn_设7017.Location = new System.Drawing.Point(7, 354);
			this.btn_设7017.Name = "btn_设7017";
			this.btn_设7017.Size = new System.Drawing.Size(66, 23);
			this.btn_设7017.TabIndex = 104;
			this.btn_设7017.Text = "设数据";
			this.btn_设7017.UseVisualStyleBackColor = true;
			this.btn_设7017.Click += new System.EventHandler(this.btn_设7017_Click);
			// 
			// tbn_读7017
			// 
			this.tbn_读7017.Image = null;
			this.tbn_读7017.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.tbn_读7017.Location = new System.Drawing.Point(7, 325);
			this.tbn_读7017.Name = "tbn_读7017";
			this.tbn_读7017.Size = new System.Drawing.Size(66, 23);
			this.tbn_读7017.TabIndex = 103;
			this.tbn_读7017.Text = "读数据";
			this.tbn_读7017.UseVisualStyleBackColor = true;
			this.tbn_读7017.Click += new System.EventHandler(this.tbn_读7017_Click);
			// 
			// ckb_7017_L线有效值补偿
			// 
			this.ckb_7017_L线有效值补偿.AutoSize = true;
			this.ckb_7017_L线有效值补偿.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.ckb_7017_L线有效值补偿.Location = new System.Drawing.Point(79, 215);
			this.ckb_7017_L线有效值补偿.Name = "ckb_7017_L线有效值补偿";
			this.ckb_7017_L线有效值补偿.Size = new System.Drawing.Size(192, 16);
			this.ckb_7017_L线有效值补偿.TabIndex = 108;
			this.ckb_7017_L线有效值补偿.Text = "L线有效值补偿( RI1RMSOFFSET)";
			this.ckb_7017_L线有效值补偿.UseVisualStyleBackColor = true;
			// 
			// tb_7017_ADC通道增益选择
			// 
			this.tb_7017_ADC通道增益选择.Location = new System.Drawing.Point(7, 101);
			this.tb_7017_ADC通道增益选择.Name = "tb_7017_ADC通道增益选择";
			this.tb_7017_ADC通道增益选择.Size = new System.Drawing.Size(66, 21);
			this.tb_7017_ADC通道增益选择.TabIndex = 105;
			// 
			// ckb_7017_ADC通道增益选择
			// 
			this.ckb_7017_ADC通道增益选择.AutoSize = true;
			this.ckb_7017_ADC通道增益选择.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.ckb_7017_ADC通道增益选择.Location = new System.Drawing.Point(79, 103);
			this.ckb_7017_ADC通道增益选择.Name = "ckb_7017_ADC通道增益选择";
			this.ckb_7017_ADC通道增益选择.Size = new System.Drawing.Size(168, 16);
			this.ckb_7017_ADC通道增益选择.TabIndex = 106;
			this.ckb_7017_ADC通道增益选择.Text = "ADC通道增益选择(RADCCON)";
			this.ckb_7017_ADC通道增益选择.UseVisualStyleBackColor = true;
			// 
			// ckb_7017_全选
			// 
			this.ckb_7017_全选.AutoSize = true;
			this.ckb_7017_全选.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.ckb_7017_全选.Location = new System.Drawing.Point(79, 325);
			this.ckb_7017_全选.Name = "ckb_7017_全选";
			this.ckb_7017_全选.Size = new System.Drawing.Size(48, 16);
			this.ckb_7017_全选.TabIndex = 101;
			this.ckb_7017_全选.Text = "全选";
			this.ckb_7017_全选.UseVisualStyleBackColor = true;
			this.ckb_7017_全选.CheckedChanged += new System.EventHandler(this.ckb_7017_全选_CheckedChanged);
			// 
			// tb_7017_有功功率校正
			// 
			this.tb_7017_有功功率校正.Location = new System.Drawing.Point(7, 17);
			this.tb_7017_有功功率校正.Name = "tb_7017_有功功率校正";
			this.tb_7017_有功功率校正.Size = new System.Drawing.Size(66, 21);
			this.tb_7017_有功功率校正.TabIndex = 76;
			// 
			// ckb_7017_有功功率校正
			// 
			this.ckb_7017_有功功率校正.AutoSize = true;
			this.ckb_7017_有功功率校正.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.ckb_7017_有功功率校正.Location = new System.Drawing.Point(79, 19);
			this.ckb_7017_有功功率校正.Name = "ckb_7017_有功功率校正";
			this.ckb_7017_有功功率校正.Size = new System.Drawing.Size(132, 16);
			this.ckb_7017_有功功率校正.TabIndex = 77;
			this.ckb_7017_有功功率校正.Text = "有功功率校正（GP1)";
			this.ckb_7017_有功功率校正.UseVisualStyleBackColor = true;
			// 
			// tb_7017_无功功率校正
			// 
			this.tb_7017_无功功率校正.Location = new System.Drawing.Point(7, 45);
			this.tb_7017_无功功率校正.Name = "tb_7017_无功功率校正";
			this.tb_7017_无功功率校正.Size = new System.Drawing.Size(66, 21);
			this.tb_7017_无功功率校正.TabIndex = 78;
			// 
			// ckb_7017_无功功率校正
			// 
			this.ckb_7017_无功功率校正.AutoSize = true;
			this.ckb_7017_无功功率校正.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.ckb_7017_无功功率校正.Location = new System.Drawing.Point(79, 47);
			this.ckb_7017_无功功率校正.Name = "ckb_7017_无功功率校正";
			this.ckb_7017_无功功率校正.Size = new System.Drawing.Size(138, 16);
			this.ckb_7017_无功功率校正.TabIndex = 79;
			this.ckb_7017_无功功率校正.Text = "无功功率校正（GQ1）";
			this.ckb_7017_无功功率校正.UseVisualStyleBackColor = true;
			// 
			// ckb_7017_视在功率校正
			// 
			this.ckb_7017_视在功率校正.AutoSize = true;
			this.ckb_7017_视在功率校正.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.ckb_7017_视在功率校正.Location = new System.Drawing.Point(79, 75);
			this.ckb_7017_视在功率校正.Name = "ckb_7017_视在功率校正";
			this.ckb_7017_视在功率校正.Size = new System.Drawing.Size(126, 16);
			this.ckb_7017_视在功率校正.TabIndex = 81;
			this.ckb_7017_视在功率校正.Text = "视在功率校正(GS1)";
			this.ckb_7017_视在功率校正.UseVisualStyleBackColor = true;
			// 
			// tb_7017_视在功率校正
			// 
			this.tb_7017_视在功率校正.Location = new System.Drawing.Point(7, 73);
			this.tb_7017_视在功率校正.Name = "tb_7017_视在功率校正";
			this.tb_7017_视在功率校正.Size = new System.Drawing.Size(66, 21);
			this.tb_7017_视在功率校正.TabIndex = 80;
			// 
			// tb_7017_起动功率寄存器
			// 
			this.tb_7017_起动功率寄存器.Location = new System.Drawing.Point(7, 129);
			this.tb_7017_起动功率寄存器.Name = "tb_7017_起动功率寄存器";
			this.tb_7017_起动功率寄存器.Size = new System.Drawing.Size(66, 21);
			this.tb_7017_起动功率寄存器.TabIndex = 82;
			// 
			// ckb_7017_起动功率寄存器
			// 
			this.ckb_7017_起动功率寄存器.AutoSize = true;
			this.ckb_7017_起动功率寄存器.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.ckb_7017_起动功率寄存器.Location = new System.Drawing.Point(79, 131);
			this.ckb_7017_起动功率寄存器.Name = "ckb_7017_起动功率寄存器";
			this.ckb_7017_起动功率寄存器.Size = new System.Drawing.Size(162, 16);
			this.ckb_7017_起动功率寄存器.TabIndex = 83;
			this.ckb_7017_起动功率寄存器.Text = "起动功率寄存器(RPStart)";
			this.ckb_7017_起动功率寄存器.UseVisualStyleBackColor = true;
			// 
			// tb_7017_输出脉冲频率
			// 
			this.tb_7017_输出脉冲频率.Location = new System.Drawing.Point(7, 157);
			this.tb_7017_输出脉冲频率.Name = "tb_7017_输出脉冲频率";
			this.tb_7017_输出脉冲频率.Size = new System.Drawing.Size(66, 21);
			this.tb_7017_输出脉冲频率.TabIndex = 84;
			// 
			// ckb_7017_输出脉冲频率
			// 
			this.ckb_7017_输出脉冲频率.AutoSize = true;
			this.ckb_7017_输出脉冲频率.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.ckb_7017_输出脉冲频率.Location = new System.Drawing.Point(79, 159);
			this.ckb_7017_输出脉冲频率.Name = "ckb_7017_输出脉冲频率";
			this.ckb_7017_输出脉冲频率.Size = new System.Drawing.Size(156, 16);
			this.ckb_7017_输出脉冲频率.TabIndex = 85;
			this.ckb_7017_输出脉冲频率.Text = "输出脉冲频率(RHFConst)";
			this.ckb_7017_输出脉冲频率.UseVisualStyleBackColor = true;
			// 
			// tb_7017_有功功率偏置校正
			// 
			this.tb_7017_有功功率偏置校正.Location = new System.Drawing.Point(7, 185);
			this.tb_7017_有功功率偏置校正.Name = "tb_7017_有功功率偏置校正";
			this.tb_7017_有功功率偏置校正.Size = new System.Drawing.Size(66, 21);
			this.tb_7017_有功功率偏置校正.TabIndex = 86;
			// 
			// ckb_7017_有功功率偏置校正
			// 
			this.ckb_7017_有功功率偏置校正.AutoSize = true;
			this.ckb_7017_有功功率偏置校正.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.ckb_7017_有功功率偏置校正.Location = new System.Drawing.Point(79, 187);
			this.ckb_7017_有功功率偏置校正.Name = "ckb_7017_有功功率偏置校正";
			this.ckb_7017_有功功率偏置校正.Size = new System.Drawing.Size(186, 16);
			this.ckb_7017_有功功率偏置校正.TabIndex = 87;
			this.ckb_7017_有功功率偏置校正.Text = "有功功率偏置校正(RP1OFFSET)";
			this.ckb_7017_有功功率偏置校正.UseVisualStyleBackColor = true;
			// 
			// ckb_7017_L线有功功率偏置校正低字节
			// 
			this.ckb_7017_L线有功功率偏置校正低字节.AutoSize = true;
			this.ckb_7017_L线有功功率偏置校正低字节.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.ckb_7017_L线有功功率偏置校正低字节.Location = new System.Drawing.Point(79, 299);
			this.ckb_7017_L线有功功率偏置校正低字节.Name = "ckb_7017_L线有功功率偏置校正低字节";
			this.ckb_7017_L线有功功率偏置校正低字节.Size = new System.Drawing.Size(246, 16);
			this.ckb_7017_L线有功功率偏置校正低字节.TabIndex = 93;
			this.ckb_7017_L线有功功率偏置校正低字节.Text = "L线有功功率偏置校正低字节(RP1OFFSETL)";
			this.ckb_7017_L线有功功率偏置校正低字节.UseVisualStyleBackColor = true;
			// 
			// tb_7017_L线有功功率偏置校正低字节
			// 
			this.tb_7017_L线有功功率偏置校正低字节.Location = new System.Drawing.Point(7, 297);
			this.tb_7017_L线有功功率偏置校正低字节.Name = "tb_7017_L线有功功率偏置校正低字节";
			this.tb_7017_L线有功功率偏置校正低字节.Size = new System.Drawing.Size(66, 21);
			this.tb_7017_L线有功功率偏置校正低字节.TabIndex = 92;
			// 
			// ckb_7017_相位校正
			// 
			this.ckb_7017_相位校正.AutoSize = true;
			this.ckb_7017_相位校正.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.ckb_7017_相位校正.Location = new System.Drawing.Point(79, 271);
			this.ckb_7017_相位校正.Name = "ckb_7017_相位校正";
			this.ckb_7017_相位校正.Size = new System.Drawing.Size(120, 16);
			this.ckb_7017_相位校正.TabIndex = 91;
			this.ckb_7017_相位校正.Text = "相位校正(RGPhs1)";
			this.ckb_7017_相位校正.UseVisualStyleBackColor = true;
			// 
			// tb_7017_相位校正
			// 
			this.tb_7017_相位校正.Location = new System.Drawing.Point(7, 269);
			this.tb_7017_相位校正.Name = "tb_7017_相位校正";
			this.tb_7017_相位校正.Size = new System.Drawing.Size(66, 21);
			this.tb_7017_相位校正.TabIndex = 90;
			// 
			// tb_7017_N线有效值补偿
			// 
			this.tb_7017_N线有效值补偿.Location = new System.Drawing.Point(7, 241);
			this.tb_7017_N线有效值补偿.Name = "tb_7017_N线有效值补偿";
			this.tb_7017_N线有效值补偿.Size = new System.Drawing.Size(66, 21);
			this.tb_7017_N线有效值补偿.TabIndex = 88;
			// 
			// ckb_7017_N线有效值补偿
			// 
			this.ckb_7017_N线有效值补偿.AutoSize = true;
			this.ckb_7017_N线有效值补偿.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.ckb_7017_N线有效值补偿.Location = new System.Drawing.Point(79, 243);
			this.ckb_7017_N线有效值补偿.Name = "ckb_7017_N线有效值补偿";
			this.ckb_7017_N线有效值补偿.Size = new System.Drawing.Size(192, 16);
			this.ckb_7017_N线有效值补偿.TabIndex = 89;
			this.ckb_7017_N线有效值补偿.Text = "N线有效值补偿( RI2RMSOFFSET)";
			this.ckb_7017_N线有效值补偿.UseVisualStyleBackColor = true;
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
			this.splitContainer2.Panel1.Controls.Add(this.ttcRegisters);
			// 
			// splitContainer2.Panel2
			// 
			this.splitContainer2.Panel2.Controls.Add(this.groupBox54);
			this.splitContainer2.Size = new System.Drawing.Size(955, 799);
			this.splitContainer2.SplitterDistance = 662;
			this.splitContainer2.TabIndex = 1;
			// 
			// ttcRegisters
			// 
			this.ttcRegisters.BaseTabColor = System.Drawing.Color.FromArgb(((int)(((byte)(246)))), ((int)(((byte)(247)))), ((int)(((byte)(250)))));
			this.ttcRegisters.BorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(182)))), ((int)(((byte)(168)))), ((int)(((byte)(192)))));
			this.ttcRegisters.CaptionFont = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
			this.ttcRegisters.CheckedTabColor = System.Drawing.Color.FromArgb(((int)(((byte)(67)))), ((int)(((byte)(165)))), ((int)(((byte)(220)))));
			this.ttcRegisters.Controls.Add(this.tpRegistersHT);
			this.ttcRegisters.Controls.Add(this.tptpRegistersRN);
			this.ttcRegisters.Controls.Add(this.tpRegistersRN8302);
			this.ttcRegisters.Dock = System.Windows.Forms.DockStyle.Fill;
			this.ttcRegisters.HeightLightTabColor = System.Drawing.Color.FromArgb(((int)(((byte)(140)))), ((int)(((byte)(67)))), ((int)(((byte)(165)))), ((int)(((byte)(220)))));
			this.ttcRegisters.Location = new System.Drawing.Point(0, 0);
			this.ttcRegisters.Name = "ttcRegisters";
			this.ttcRegisters.SelectedIndex = 0;
			this.ttcRegisters.Size = new System.Drawing.Size(955, 662);
			this.ttcRegisters.TabClosd = false;
			this.ttcRegisters.TabCornerRadius = 3;
			this.ttcRegisters.TabIndex = 1;
			// 
			// tpRegistersHT
			// 
			this.tpRegistersHT.Controls.Add(this.splitContainer1);
			this.tpRegistersHT.Location = new System.Drawing.Point(4, 29);
			this.tpRegistersHT.Name = "tpRegistersHT";
			this.tpRegistersHT.Padding = new System.Windows.Forms.Padding(3);
			this.tpRegistersHT.Size = new System.Drawing.Size(947, 629);
			this.tpRegistersHT.TabIndex = 0;
			this.tpRegistersHT.Text = "钜泉芯片";
			this.tpRegistersHT.UseVisualStyleBackColor = true;
			// 
			// tptpRegistersRN
			// 
			this.tptpRegistersRN.Controls.Add(this.splitContainer3);
			this.tptpRegistersRN.Location = new System.Drawing.Point(4, 29);
			this.tptpRegistersRN.Name = "tptpRegistersRN";
			this.tptpRegistersRN.Padding = new System.Windows.Forms.Padding(3);
			this.tptpRegistersRN.Size = new System.Drawing.Size(947, 629);
			this.tptpRegistersRN.TabIndex = 1;
			this.tptpRegistersRN.Text = "锐能微芯片";
			this.tptpRegistersRN.UseVisualStyleBackColor = true;
			// 
			// splitContainer3
			// 
			this.splitContainer3.Dock = System.Windows.Forms.DockStyle.Fill;
			this.splitContainer3.Location = new System.Drawing.Point(3, 3);
			this.splitContainer3.Name = "splitContainer3";
			this.splitContainer3.Orientation = System.Windows.Forms.Orientation.Horizontal;
			// 
			// splitContainer3.Panel1
			// 
			this.splitContainer3.Panel1.Controls.Add(this.gbRegisterThreeRN);
			// 
			// splitContainer3.Panel2
			// 
			this.splitContainer3.Panel2.Controls.Add(this.cbEEPROM);
			this.splitContainer3.Panel2.Controls.Add(this.tcbTypeRN);
			this.splitContainer3.Panel2.Controls.Add(this.tbWriteRegister);
			this.splitContainer3.Panel2.Controls.Add(this.tbReadRegister);
			this.splitContainer3.Size = new System.Drawing.Size(941, 623);
			this.splitContainer3.SplitterDistance = 574;
			this.splitContainer3.TabIndex = 1;
			// 
			// gbRegisterThreeRN
			// 
			this.gbRegisterThreeRN.Controls.Add(this.dgvCommon);
			this.gbRegisterThreeRN.Location = new System.Drawing.Point(0, 0);
			this.gbRegisterThreeRN.Name = "gbRegisterThreeRN";
			this.gbRegisterThreeRN.Size = new System.Drawing.Size(941, 574);
			this.gbRegisterThreeRN.TabIndex = 104;
			this.gbRegisterThreeRN.TabStop = false;
			this.gbRegisterThreeRN.Text = "常用寄存器读写";
			// 
			// dgvCommon
			// 
			this.dgvCommon.AllowDrop = true;
			this.dgvCommon.AllowUserToAddRows = false;
			dataGridViewCellStyle1.BackColor = System.Drawing.Color.AliceBlue;
			this.dgvCommon.AlternatingRowsDefaultCellStyle = dataGridViewCellStyle1;
			dataGridViewCellStyle2.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter;
			dataGridViewCellStyle2.BackColor = System.Drawing.SystemColors.Control;
			dataGridViewCellStyle2.Font = new System.Drawing.Font("微软雅黑", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
			dataGridViewCellStyle2.ForeColor = System.Drawing.Color.SkyBlue;
			dataGridViewCellStyle2.SelectionBackColor = System.Drawing.SystemColors.Highlight;
			dataGridViewCellStyle2.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
			dataGridViewCellStyle2.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
			this.dgvCommon.ColumnHeadersDefaultCellStyle = dataGridViewCellStyle2;
			this.dgvCommon.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			this.dgvCommon.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.dataGridViewCheckBoxColumn3,
            this.dataGridViewTextBoxColumn20,
            this.dataGridViewTextBoxColumn21,
            this.dataGridViewTextBoxColumn22,
            this.dataGridViewTextBoxColumn23,
            this.dataGridViewTextBoxColumn24,
            this.dataGridViewTextBoxColumn32,
            this.dataGridViewTextBoxColumn25,
            this.dataGridViewTextBoxColumn26,
            this.dataGridViewTextBoxColumn27,
            this.dataGridViewTextBoxColumn28,
            this.dataGridViewTextBoxColumn29,
            this.dataGridViewTextBoxColumn30,
            this.dataGridViewTextBoxColumn31,
            this.dataGridViewTextBoxColumn33});
			this.dgvCommon.Dock = System.Windows.Forms.DockStyle.Fill;
			this.dgvCommon.Location = new System.Drawing.Point(3, 17);
			this.dgvCommon.Name = "dgvCommon";
			this.dgvCommon.RowHeadersWidth = 62;
			this.dgvCommon.RowTemplate.Height = 23;
			this.dgvCommon.Size = new System.Drawing.Size(935, 554);
			this.dgvCommon.TabIndex = 109;
			// 
			// dataGridViewCheckBoxColumn3
			// 
			this.dataGridViewCheckBoxColumn3.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.AllCells;
			this.dataGridViewCheckBoxColumn3.DataPropertyName = "选择";
			this.dataGridViewCheckBoxColumn3.HeaderText = "";
			this.dataGridViewCheckBoxColumn3.MinimumWidth = 8;
			this.dataGridViewCheckBoxColumn3.Name = "dataGridViewCheckBoxColumn3";
			this.dataGridViewCheckBoxColumn3.Width = 8;
			// 
			// dataGridViewTextBoxColumn20
			// 
			this.dataGridViewTextBoxColumn20.DataPropertyName = "ID";
			this.dataGridViewTextBoxColumn20.HeaderText = "ID";
			this.dataGridViewTextBoxColumn20.MinimumWidth = 8;
			this.dataGridViewTextBoxColumn20.Name = "dataGridViewTextBoxColumn20";
			this.dataGridViewTextBoxColumn20.Visible = false;
			this.dataGridViewTextBoxColumn20.Width = 150;
			// 
			// dataGridViewTextBoxColumn21
			// 
			this.dataGridViewTextBoxColumn21.DataPropertyName = "ParentID";
			this.dataGridViewTextBoxColumn21.HeaderText = "ParentID";
			this.dataGridViewTextBoxColumn21.MinimumWidth = 8;
			this.dataGridViewTextBoxColumn21.Name = "dataGridViewTextBoxColumn21";
			this.dataGridViewTextBoxColumn21.Visible = false;
			this.dataGridViewTextBoxColumn21.Width = 150;
			// 
			// dataGridViewTextBoxColumn22
			// 
			this.dataGridViewTextBoxColumn22.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.AllCells;
			this.dataGridViewTextBoxColumn22.DataPropertyName = "Name";
			this.dataGridViewTextBoxColumn22.HeaderText = "数据项名称";
			this.dataGridViewTextBoxColumn22.MinimumWidth = 8;
			this.dataGridViewTextBoxColumn22.Name = "dataGridViewTextBoxColumn22";
			this.dataGridViewTextBoxColumn22.Width = 93;
			// 
			// dataGridViewTextBoxColumn23
			// 
			this.dataGridViewTextBoxColumn23.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.AllCells;
			this.dataGridViewTextBoxColumn23.DataPropertyName = "DataMark";
			this.dataGridViewTextBoxColumn23.HeaderText = "数据标识";
			this.dataGridViewTextBoxColumn23.MinimumWidth = 8;
			this.dataGridViewTextBoxColumn23.Name = "dataGridViewTextBoxColumn23";
			this.dataGridViewTextBoxColumn23.Visible = false;
			// 
			// dataGridViewTextBoxColumn24
			// 
			this.dataGridViewTextBoxColumn24.DataPropertyName = "DataMarkFather";
			this.dataGridViewTextBoxColumn24.HeaderText = "扩展数据标识";
			this.dataGridViewTextBoxColumn24.MinimumWidth = 8;
			this.dataGridViewTextBoxColumn24.Name = "dataGridViewTextBoxColumn24";
			this.dataGridViewTextBoxColumn24.ReadOnly = true;
			this.dataGridViewTextBoxColumn24.Visible = false;
			this.dataGridViewTextBoxColumn24.Width = 150;
			// 
			// dataGridViewTextBoxColumn32
			// 
			this.dataGridViewTextBoxColumn32.DataPropertyName = "寄存器地址";
			this.dataGridViewTextBoxColumn32.HeaderText = "寄存器地址";
			this.dataGridViewTextBoxColumn32.MinimumWidth = 8;
			this.dataGridViewTextBoxColumn32.Name = "dataGridViewTextBoxColumn32";
			this.dataGridViewTextBoxColumn32.Width = 150;
			// 
			// dataGridViewTextBoxColumn25
			// 
			this.dataGridViewTextBoxColumn25.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.AllCells;
			this.dataGridViewTextBoxColumn25.DataPropertyName = "DataLen";
			this.dataGridViewTextBoxColumn25.HeaderText = "长度";
			this.dataGridViewTextBoxColumn25.MinimumWidth = 8;
			this.dataGridViewTextBoxColumn25.Name = "dataGridViewTextBoxColumn25";
			this.dataGridViewTextBoxColumn25.Width = 57;
			// 
			// dataGridViewTextBoxColumn26
			// 
			this.dataGridViewTextBoxColumn26.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.AllCells;
			this.dataGridViewTextBoxColumn26.DataPropertyName = "DataBlockType";
			this.dataGridViewTextBoxColumn26.HeaderText = "数据块类型";
			this.dataGridViewTextBoxColumn26.MinimumWidth = 8;
			this.dataGridViewTextBoxColumn26.Name = "dataGridViewTextBoxColumn26";
			this.dataGridViewTextBoxColumn26.Visible = false;
			// 
			// dataGridViewTextBoxColumn27
			// 
			this.dataGridViewTextBoxColumn27.DataPropertyName = "DataFormat";
			this.dataGridViewTextBoxColumn27.HeaderText = "数据格式";
			this.dataGridViewTextBoxColumn27.MinimumWidth = 8;
			this.dataGridViewTextBoxColumn27.Name = "dataGridViewTextBoxColumn27";
			this.dataGridViewTextBoxColumn27.Visible = false;
			this.dataGridViewTextBoxColumn27.Width = 150;
			// 
			// dataGridViewTextBoxColumn28
			// 
			this.dataGridViewTextBoxColumn28.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.AllCells;
			this.dataGridViewTextBoxColumn28.DataPropertyName = "DecimalPlaces";
			this.dataGridViewTextBoxColumn28.HeaderText = "小数位数";
			this.dataGridViewTextBoxColumn28.MinimumWidth = 8;
			this.dataGridViewTextBoxColumn28.Name = "dataGridViewTextBoxColumn28";
			this.dataGridViewTextBoxColumn28.Visible = false;
			// 
			// dataGridViewTextBoxColumn29
			// 
			this.dataGridViewTextBoxColumn29.DataPropertyName = "DefaultValue";
			this.dataGridViewTextBoxColumn29.HeaderText = "默认值";
			this.dataGridViewTextBoxColumn29.MinimumWidth = 8;
			this.dataGridViewTextBoxColumn29.Name = "dataGridViewTextBoxColumn29";
			this.dataGridViewTextBoxColumn29.Visible = false;
			this.dataGridViewTextBoxColumn29.Width = 150;
			// 
			// dataGridViewTextBoxColumn30
			// 
			this.dataGridViewTextBoxColumn30.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.AllCells;
			this.dataGridViewTextBoxColumn30.DataPropertyName = "DataUnit";
			this.dataGridViewTextBoxColumn30.HeaderText = "单位";
			this.dataGridViewTextBoxColumn30.MinimumWidth = 8;
			this.dataGridViewTextBoxColumn30.Name = "dataGridViewTextBoxColumn30";
			this.dataGridViewTextBoxColumn30.Visible = false;
			// 
			// dataGridViewTextBoxColumn31
			// 
			this.dataGridViewTextBoxColumn31.DataPropertyName = "数据值";
			this.dataGridViewTextBoxColumn31.HeaderText = "数据值";
			this.dataGridViewTextBoxColumn31.MinimumWidth = 8;
			this.dataGridViewTextBoxColumn31.Name = "dataGridViewTextBoxColumn31";
			this.dataGridViewTextBoxColumn31.Width = 150;
			// 
			// dataGridViewTextBoxColumn33
			// 
			this.dataGridViewTextBoxColumn33.DataPropertyName = "操作结论";
			this.dataGridViewTextBoxColumn33.HeaderText = "操作结论";
			this.dataGridViewTextBoxColumn33.MinimumWidth = 8;
			this.dataGridViewTextBoxColumn33.Name = "dataGridViewTextBoxColumn33";
			this.dataGridViewTextBoxColumn33.Width = 150;
			// 
			// cbEEPROM
			// 
			this.cbEEPROM.AutoSize = true;
			this.cbEEPROM.Checked = true;
			this.cbEEPROM.CheckState = System.Windows.Forms.CheckState.Checked;
			this.cbEEPROM.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.cbEEPROM.Location = new System.Drawing.Point(274, 13);
			this.cbEEPROM.Name = "cbEEPROM";
			this.cbEEPROM.Size = new System.Drawing.Size(84, 16);
			this.cbEEPROM.TabIndex = 105;
			this.cbEEPROM.Text = "写入EEPROM";
			this.cbEEPROM.UseVisualStyleBackColor = true;
			// 
			// tcbTypeRN
			// 
			this.tcbTypeRN.FormattingEnabled = true;
			this.tcbTypeRN.Items.AddRange(new object[] {
            "增益类寄存器",
            "控制类寄存器"});
			this.tcbTypeRN.Location = new System.Drawing.Point(22, 13);
			this.tcbTypeRN.Name = "tcbTypeRN";
			this.tcbTypeRN.Size = new System.Drawing.Size(119, 20);
			this.tcbTypeRN.TabIndex = 2;
			this.tcbTypeRN.Text = "增益类寄存器";
			this.tcbTypeRN.SelectedIndexChanged += new System.EventHandler(this.tcbTypeRN_SelectedIndexChanged);
			// 
			// tbWriteRegister
			// 
			this.tbWriteRegister.Image = null;
			this.tbWriteRegister.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.tbWriteRegister.Location = new System.Drawing.Point(383, 9);
			this.tbWriteRegister.Name = "tbWriteRegister";
			this.tbWriteRegister.Size = new System.Drawing.Size(66, 23);
			this.tbWriteRegister.TabIndex = 104;
			this.tbWriteRegister.Text = "写寄存器";
			this.tbWriteRegister.UseVisualStyleBackColor = true;
			this.tbWriteRegister.Click += new System.EventHandler(this.tbWriteRegister_Click);
			// 
			// tbReadRegister
			// 
			this.tbReadRegister.Image = null;
			this.tbReadRegister.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.tbReadRegister.Location = new System.Drawing.Point(174, 10);
			this.tbReadRegister.Name = "tbReadRegister";
			this.tbReadRegister.Size = new System.Drawing.Size(66, 23);
			this.tbReadRegister.TabIndex = 101;
			this.tbReadRegister.Text = "读寄存器";
			this.tbReadRegister.UseVisualStyleBackColor = true;
			this.tbReadRegister.Click += new System.EventHandler(this.tbReadRegister_Click);
			// 
			// tpRegistersRN8302
			// 
			this.tpRegistersRN8302.Controls.Add(this.splitContainer4);
			this.tpRegistersRN8302.Location = new System.Drawing.Point(4, 29);
			this.tpRegistersRN8302.Name = "tpRegistersRN8302";
			this.tpRegistersRN8302.Padding = new System.Windows.Forms.Padding(3);
			this.tpRegistersRN8302.Size = new System.Drawing.Size(947, 629);
			this.tpRegistersRN8302.TabIndex = 2;
			this.tpRegistersRN8302.Text = "RN8302";
			this.tpRegistersRN8302.UseVisualStyleBackColor = true;
			// 
			// splitContainer4
			// 
			this.splitContainer4.Dock = System.Windows.Forms.DockStyle.Fill;
			this.splitContainer4.Location = new System.Drawing.Point(3, 3);
			this.splitContainer4.Name = "splitContainer4";
			this.splitContainer4.Orientation = System.Windows.Forms.Orientation.Horizontal;
			// 
			// splitContainer4.Panel1
			// 
			this.splitContainer4.Panel1.Controls.Add(this.groupBox1);
			// 
			// splitContainer4.Panel2
			// 
			this.splitContainer4.Panel2.Controls.Add(this.cbEEPROM8302);
			this.splitContainer4.Panel2.Controls.Add(this.txComboBox1);
			this.splitContainer4.Panel2.Controls.Add(this.txbSetRN8302);
			this.splitContainer4.Panel2.Controls.Add(this.txbReadRN8302);
			this.splitContainer4.Size = new System.Drawing.Size(941, 623);
			this.splitContainer4.SplitterDistance = 574;
			this.splitContainer4.TabIndex = 2;
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.dgvRN8302);
			this.groupBox1.Location = new System.Drawing.Point(0, 0);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(941, 574);
			this.groupBox1.TabIndex = 104;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "常用寄存器读写";
			// 
			// dgvRN8302
			// 
			this.dgvRN8302.AllowDrop = true;
			this.dgvRN8302.AllowUserToAddRows = false;
			dataGridViewCellStyle3.BackColor = System.Drawing.Color.AliceBlue;
			this.dgvRN8302.AlternatingRowsDefaultCellStyle = dataGridViewCellStyle3;
			dataGridViewCellStyle4.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleCenter;
			dataGridViewCellStyle4.BackColor = System.Drawing.SystemColors.Control;
			dataGridViewCellStyle4.Font = new System.Drawing.Font("微软雅黑", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
			dataGridViewCellStyle4.ForeColor = System.Drawing.Color.SkyBlue;
			dataGridViewCellStyle4.SelectionBackColor = System.Drawing.SystemColors.Highlight;
			dataGridViewCellStyle4.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
			dataGridViewCellStyle4.WrapMode = System.Windows.Forms.DataGridViewTriState.True;
			this.dgvRN8302.ColumnHeadersDefaultCellStyle = dataGridViewCellStyle4;
			this.dgvRN8302.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			this.dgvRN8302.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.dataGridViewCheckBoxColumn1,
            this.dataGridViewTextBoxColumn1,
            this.dataGridViewTextBoxColumn2,
            this.dataGridViewTextBoxColumn3,
            this.dataGridViewTextBoxColumn4,
            this.dataGridViewTextBoxColumn5,
            this.dataGridViewTextBoxColumn6,
            this.dataGridViewTextBoxColumn7,
            this.dataGridViewTextBoxColumn8,
            this.dataGridViewTextBoxColumn9,
            this.dataGridViewTextBoxColumn10,
            this.dataGridViewTextBoxColumn11,
            this.dataGridViewTextBoxColumn12,
            this.dataGridViewTextBoxColumn13,
            this.dataGridViewTextBoxColumn14});
			this.dgvRN8302.Dock = System.Windows.Forms.DockStyle.Fill;
			this.dgvRN8302.Location = new System.Drawing.Point(3, 17);
			this.dgvRN8302.Name = "dgvRN8302";
			this.dgvRN8302.RowHeadersWidth = 62;
			this.dgvRN8302.RowTemplate.Height = 23;
			this.dgvRN8302.Size = new System.Drawing.Size(935, 554);
			this.dgvRN8302.TabIndex = 109;
			// 
			// dataGridViewCheckBoxColumn1
			// 
			this.dataGridViewCheckBoxColumn1.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.AllCells;
			this.dataGridViewCheckBoxColumn1.DataPropertyName = "选择";
			this.dataGridViewCheckBoxColumn1.HeaderText = "";
			this.dataGridViewCheckBoxColumn1.MinimumWidth = 8;
			this.dataGridViewCheckBoxColumn1.Name = "dataGridViewCheckBoxColumn1";
			this.dataGridViewCheckBoxColumn1.Width = 8;
			// 
			// dataGridViewTextBoxColumn1
			// 
			this.dataGridViewTextBoxColumn1.DataPropertyName = "ID";
			this.dataGridViewTextBoxColumn1.HeaderText = "ID";
			this.dataGridViewTextBoxColumn1.MinimumWidth = 8;
			this.dataGridViewTextBoxColumn1.Name = "dataGridViewTextBoxColumn1";
			this.dataGridViewTextBoxColumn1.Visible = false;
			this.dataGridViewTextBoxColumn1.Width = 150;
			// 
			// dataGridViewTextBoxColumn2
			// 
			this.dataGridViewTextBoxColumn2.DataPropertyName = "ParentID";
			this.dataGridViewTextBoxColumn2.HeaderText = "ParentID";
			this.dataGridViewTextBoxColumn2.MinimumWidth = 8;
			this.dataGridViewTextBoxColumn2.Name = "dataGridViewTextBoxColumn2";
			this.dataGridViewTextBoxColumn2.Visible = false;
			this.dataGridViewTextBoxColumn2.Width = 150;
			// 
			// dataGridViewTextBoxColumn3
			// 
			this.dataGridViewTextBoxColumn3.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.AllCells;
			this.dataGridViewTextBoxColumn3.DataPropertyName = "Name";
			this.dataGridViewTextBoxColumn3.HeaderText = "数据项名称";
			this.dataGridViewTextBoxColumn3.MinimumWidth = 8;
			this.dataGridViewTextBoxColumn3.Name = "dataGridViewTextBoxColumn3";
			this.dataGridViewTextBoxColumn3.Width = 93;
			// 
			// dataGridViewTextBoxColumn4
			// 
			this.dataGridViewTextBoxColumn4.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.AllCells;
			this.dataGridViewTextBoxColumn4.DataPropertyName = "DataMark";
			this.dataGridViewTextBoxColumn4.HeaderText = "数据标识";
			this.dataGridViewTextBoxColumn4.MinimumWidth = 8;
			this.dataGridViewTextBoxColumn4.Name = "dataGridViewTextBoxColumn4";
			this.dataGridViewTextBoxColumn4.Visible = false;
			// 
			// dataGridViewTextBoxColumn5
			// 
			this.dataGridViewTextBoxColumn5.DataPropertyName = "DataMarkFather";
			this.dataGridViewTextBoxColumn5.HeaderText = "扩展数据标识";
			this.dataGridViewTextBoxColumn5.MinimumWidth = 8;
			this.dataGridViewTextBoxColumn5.Name = "dataGridViewTextBoxColumn5";
			this.dataGridViewTextBoxColumn5.ReadOnly = true;
			this.dataGridViewTextBoxColumn5.Visible = false;
			this.dataGridViewTextBoxColumn5.Width = 150;
			// 
			// dataGridViewTextBoxColumn6
			// 
			this.dataGridViewTextBoxColumn6.DataPropertyName = "寄存器地址";
			this.dataGridViewTextBoxColumn6.HeaderText = "寄存器地址";
			this.dataGridViewTextBoxColumn6.MinimumWidth = 8;
			this.dataGridViewTextBoxColumn6.Name = "dataGridViewTextBoxColumn6";
			this.dataGridViewTextBoxColumn6.Width = 150;
			// 
			// dataGridViewTextBoxColumn7
			// 
			this.dataGridViewTextBoxColumn7.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.AllCells;
			this.dataGridViewTextBoxColumn7.DataPropertyName = "DataLen";
			this.dataGridViewTextBoxColumn7.HeaderText = "长度";
			this.dataGridViewTextBoxColumn7.MinimumWidth = 8;
			this.dataGridViewTextBoxColumn7.Name = "dataGridViewTextBoxColumn7";
			this.dataGridViewTextBoxColumn7.Width = 57;
			// 
			// dataGridViewTextBoxColumn8
			// 
			this.dataGridViewTextBoxColumn8.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.AllCells;
			this.dataGridViewTextBoxColumn8.DataPropertyName = "DataBlockType";
			this.dataGridViewTextBoxColumn8.HeaderText = "数据块类型";
			this.dataGridViewTextBoxColumn8.MinimumWidth = 8;
			this.dataGridViewTextBoxColumn8.Name = "dataGridViewTextBoxColumn8";
			this.dataGridViewTextBoxColumn8.Visible = false;
			// 
			// dataGridViewTextBoxColumn9
			// 
			this.dataGridViewTextBoxColumn9.DataPropertyName = "DataFormat";
			this.dataGridViewTextBoxColumn9.HeaderText = "数据格式";
			this.dataGridViewTextBoxColumn9.MinimumWidth = 8;
			this.dataGridViewTextBoxColumn9.Name = "dataGridViewTextBoxColumn9";
			this.dataGridViewTextBoxColumn9.Visible = false;
			this.dataGridViewTextBoxColumn9.Width = 150;
			// 
			// dataGridViewTextBoxColumn10
			// 
			this.dataGridViewTextBoxColumn10.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.AllCells;
			this.dataGridViewTextBoxColumn10.DataPropertyName = "DecimalPlaces";
			this.dataGridViewTextBoxColumn10.HeaderText = "小数位数";
			this.dataGridViewTextBoxColumn10.MinimumWidth = 8;
			this.dataGridViewTextBoxColumn10.Name = "dataGridViewTextBoxColumn10";
			this.dataGridViewTextBoxColumn10.Visible = false;
			// 
			// dataGridViewTextBoxColumn11
			// 
			this.dataGridViewTextBoxColumn11.DataPropertyName = "DefaultValue";
			this.dataGridViewTextBoxColumn11.HeaderText = "默认值";
			this.dataGridViewTextBoxColumn11.MinimumWidth = 8;
			this.dataGridViewTextBoxColumn11.Name = "dataGridViewTextBoxColumn11";
			this.dataGridViewTextBoxColumn11.Visible = false;
			this.dataGridViewTextBoxColumn11.Width = 150;
			// 
			// dataGridViewTextBoxColumn12
			// 
			this.dataGridViewTextBoxColumn12.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.AllCells;
			this.dataGridViewTextBoxColumn12.DataPropertyName = "DataUnit";
			this.dataGridViewTextBoxColumn12.HeaderText = "单位";
			this.dataGridViewTextBoxColumn12.MinimumWidth = 8;
			this.dataGridViewTextBoxColumn12.Name = "dataGridViewTextBoxColumn12";
			this.dataGridViewTextBoxColumn12.Visible = false;
			// 
			// dataGridViewTextBoxColumn13
			// 
			this.dataGridViewTextBoxColumn13.DataPropertyName = "数据值";
			this.dataGridViewTextBoxColumn13.HeaderText = "数据值";
			this.dataGridViewTextBoxColumn13.MinimumWidth = 8;
			this.dataGridViewTextBoxColumn13.Name = "dataGridViewTextBoxColumn13";
			this.dataGridViewTextBoxColumn13.Width = 150;
			// 
			// dataGridViewTextBoxColumn14
			// 
			this.dataGridViewTextBoxColumn14.DataPropertyName = "操作结论";
			this.dataGridViewTextBoxColumn14.HeaderText = "操作结论";
			this.dataGridViewTextBoxColumn14.MinimumWidth = 8;
			this.dataGridViewTextBoxColumn14.Name = "dataGridViewTextBoxColumn14";
			this.dataGridViewTextBoxColumn14.Width = 150;
			// 
			// cbEEPROM8302
			// 
			this.cbEEPROM8302.AutoSize = true;
			this.cbEEPROM8302.Checked = true;
			this.cbEEPROM8302.CheckState = System.Windows.Forms.CheckState.Checked;
			this.cbEEPROM8302.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.cbEEPROM8302.Location = new System.Drawing.Point(274, 13);
			this.cbEEPROM8302.Name = "cbEEPROM8302";
			this.cbEEPROM8302.Size = new System.Drawing.Size(84, 16);
			this.cbEEPROM8302.TabIndex = 105;
			this.cbEEPROM8302.Text = "写入EEPROM";
			this.cbEEPROM8302.UseVisualStyleBackColor = true;
			// 
			// txComboBox1
			// 
			this.txComboBox1.FormattingEnabled = true;
			this.txComboBox1.Items.AddRange(new object[] {
            "增益类寄存器",
            "控制类寄存器"});
			this.txComboBox1.Location = new System.Drawing.Point(22, 13);
			this.txComboBox1.Name = "txComboBox1";
			this.txComboBox1.Size = new System.Drawing.Size(119, 20);
			this.txComboBox1.TabIndex = 2;
			this.txComboBox1.Text = "增益类寄存器";
			this.txComboBox1.Visible = false;
			// 
			// txbSetRN8302
			// 
			this.txbSetRN8302.Image = null;
			this.txbSetRN8302.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.txbSetRN8302.Location = new System.Drawing.Point(383, 9);
			this.txbSetRN8302.Name = "txbSetRN8302";
			this.txbSetRN8302.Size = new System.Drawing.Size(66, 23);
			this.txbSetRN8302.TabIndex = 104;
			this.txbSetRN8302.Text = "写寄存器";
			this.txbSetRN8302.UseVisualStyleBackColor = true;
			this.txbSetRN8302.Click += new System.EventHandler(this.txbSetRN8302_Click);
			// 
			// txbReadRN8302
			// 
			this.txbReadRN8302.Image = null;
			this.txbReadRN8302.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.txbReadRN8302.Location = new System.Drawing.Point(174, 10);
			this.txbReadRN8302.Name = "txbReadRN8302";
			this.txbReadRN8302.Size = new System.Drawing.Size(66, 23);
			this.txbReadRN8302.TabIndex = 101;
			this.txbReadRN8302.Text = "读寄存器";
			this.txbReadRN8302.UseVisualStyleBackColor = true;
			this.txbReadRN8302.Click += new System.EventHandler(this.txbReadRN8302_Click);
			// 
			// groupBox54
			// 
			this.groupBox54.Controls.Add(this.textBox_xiaobiaoData);
			this.groupBox54.Controls.Add(this.label104);
			this.groupBox54.Controls.Add(this.button_xiaobiaoSet);
			this.groupBox54.Controls.Add(this.label105);
			this.groupBox54.Controls.Add(this.label106);
			this.groupBox54.Controls.Add(this.textBox_xiaobiaoLen);
			this.groupBox54.Controls.Add(this.textBox_xiaobiaoAddr);
			this.groupBox54.Controls.Add(this.button_xiaobiaoRead);
			this.groupBox54.Dock = System.Windows.Forms.DockStyle.Fill;
			this.groupBox54.Location = new System.Drawing.Point(0, 0);
			this.groupBox54.Name = "groupBox54";
			this.groupBox54.Size = new System.Drawing.Size(955, 133);
			this.groupBox54.TabIndex = 57;
			this.groupBox54.TabStop = false;
			this.groupBox54.Text = "指定寄存器地址读写";
			// 
			// textBox_xiaobiaoData
			// 
			this.textBox_xiaobiaoData.Location = new System.Drawing.Point(48, 40);
			this.textBox_xiaobiaoData.Multiline = true;
			this.textBox_xiaobiaoData.Name = "textBox_xiaobiaoData";
			this.textBox_xiaobiaoData.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
			this.textBox_xiaobiaoData.Size = new System.Drawing.Size(898, 76);
			this.textBox_xiaobiaoData.TabIndex = 15;
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
			// button_xiaobiaoSet
			// 
			this.button_xiaobiaoSet.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.button_xiaobiaoSet.Image = null;
			this.button_xiaobiaoSet.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.button_xiaobiaoSet.Location = new System.Drawing.Point(653, 12);
			this.button_xiaobiaoSet.Name = "button_xiaobiaoSet";
			this.button_xiaobiaoSet.Size = new System.Drawing.Size(69, 21);
			this.button_xiaobiaoSet.TabIndex = 13;
			this.button_xiaobiaoSet.Text = "写寄存器";
			this.button_xiaobiaoSet.UseVisualStyleBackColor = true;
			this.button_xiaobiaoSet.Click += new System.EventHandler(this.button_xiaobiaoSet_Click);
			// 
			// label105
			// 
			this.label105.AutoSize = true;
			this.label105.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.label105.Location = new System.Drawing.Point(264, 17);
			this.label105.Name = "label105";
			this.label105.Size = new System.Drawing.Size(41, 12);
			this.label105.TabIndex = 12;
			this.label105.Text = "长度：";
			// 
			// label106
			// 
			this.label106.AutoSize = true;
			this.label106.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.label106.Location = new System.Drawing.Point(107, 17);
			this.label106.Name = "label106";
			this.label106.Size = new System.Drawing.Size(41, 12);
			this.label106.TabIndex = 11;
			this.label106.Text = "地址：";
			// 
			// textBox_xiaobiaoLen
			// 
			this.textBox_xiaobiaoLen.Location = new System.Drawing.Point(320, 13);
			this.textBox_xiaobiaoLen.Name = "textBox_xiaobiaoLen";
			this.textBox_xiaobiaoLen.Size = new System.Drawing.Size(69, 21);
			this.textBox_xiaobiaoLen.TabIndex = 10;
			this.textBox_xiaobiaoLen.Text = "2";
			// 
			// textBox_xiaobiaoAddr
			// 
			this.textBox_xiaobiaoAddr.Location = new System.Drawing.Point(157, 13);
			this.textBox_xiaobiaoAddr.Name = "textBox_xiaobiaoAddr";
			this.textBox_xiaobiaoAddr.Size = new System.Drawing.Size(69, 21);
			this.textBox_xiaobiaoAddr.TabIndex = 9;
			this.textBox_xiaobiaoAddr.Text = "0000";
			// 
			// button_xiaobiaoRead
			// 
			this.button_xiaobiaoRead.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
			this.button_xiaobiaoRead.Image = null;
			this.button_xiaobiaoRead.ImeMode = System.Windows.Forms.ImeMode.NoControl;
			this.button_xiaobiaoRead.Location = new System.Drawing.Point(545, 13);
			this.button_xiaobiaoRead.Name = "button_xiaobiaoRead";
			this.button_xiaobiaoRead.Size = new System.Drawing.Size(69, 21);
			this.button_xiaobiaoRead.TabIndex = 8;
			this.button_xiaobiaoRead.Text = "读寄存器";
			this.button_xiaobiaoRead.UseVisualStyleBackColor = true;
			this.button_xiaobiaoRead.Click += new System.EventHandler(this.button_xiaobiaoRead_Click);
			// 
			// FormChipRegisters
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.BackColor = System.Drawing.Color.Transparent;
			this.ClientSize = new System.Drawing.Size(961, 833);
			this.Controls.Add(this.splitContainer2);
			this.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.Location = new System.Drawing.Point(0, 0);
			this.MaximizeBox = false;
			this.Name = "FormChipRegisters";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Load += new System.EventHandler(this.FormAdjustMeter_Load);
			this.splitContainer1.Panel1.ResumeLayout(false);
			this.splitContainer1.Panel2.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
			this.splitContainer1.ResumeLayout(false);
			this.gbRegisterThree.ResumeLayout(false);
			this.gbRegisterThree.PerformLayout();
			this.gbRegisterSingle.ResumeLayout(false);
			this.gbRegisterSingle.PerformLayout();
			this.splitContainer2.Panel1.ResumeLayout(false);
			this.splitContainer2.Panel2.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).EndInit();
			this.splitContainer2.ResumeLayout(false);
			this.ttcRegisters.ResumeLayout(false);
			this.tpRegistersHT.ResumeLayout(false);
			this.tptpRegistersRN.ResumeLayout(false);
			this.splitContainer3.Panel1.ResumeLayout(false);
			this.splitContainer3.Panel2.ResumeLayout(false);
			this.splitContainer3.Panel2.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.splitContainer3)).EndInit();
			this.splitContainer3.ResumeLayout(false);
			this.gbRegisterThreeRN.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.dgvCommon)).EndInit();
			this.tpRegistersRN8302.ResumeLayout(false);
			this.splitContainer4.Panel1.ResumeLayout(false);
			this.splitContainer4.Panel2.ResumeLayout(false);
			this.splitContainer4.Panel2.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.splitContainer4)).EndInit();
			this.splitContainer4.ResumeLayout(false);
			this.groupBox1.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.dgvRN8302)).EndInit();
			this.groupBox54.ResumeLayout(false);
			this.groupBox54.PerformLayout();
			this.ResumeLayout(false);

        }

		#endregion

		private System.Windows.Forms.SplitContainer splitContainer1;
		private System.Windows.Forms.GroupBox gbRegisterThree;
		private System.Windows.Forms.TextBox tb_C相位1;
		private System.Windows.Forms.TextBox tb_A有功功率;
		private System.Windows.Forms.CheckBox ckb_A有功增益;
		private System.Windows.Forms.CheckBox ckb_Toffset校正寄存器;
		private System.Windows.Forms.TextBox tb_Toffset校正寄存器;
		private System.Windows.Forms.TextBox tb_B有功功率;
		private System.Windows.Forms.CheckBox ckb_温度传感器数据寄存器;
		private System.Windows.Forms.TextBox tb_温度传感器数据寄存器;
		private System.Windows.Forms.CheckBox ckb_三相寄存器全选;
		private TXButton btnSetRegister;
		private System.Windows.Forms.TextBox tb_A无功功率;
		private TXButton btnReadRegister;
		private System.Windows.Forms.CheckBox ckb_B有功功率;
		private System.Windows.Forms.CheckBox ckb_相位补偿区域设置1;
		private System.Windows.Forms.CheckBox ckb_A无功功率;
		private System.Windows.Forms.CheckBox ckb_C有功功率;
		private System.Windows.Forms.TextBox tb_相位补偿区域设置1;
		private System.Windows.Forms.TextBox tb_C有功功率;
		private System.Windows.Forms.TextBox tb_B无功功率;
		private System.Windows.Forms.CheckBox ckb_相位补偿区域设置0;
		private System.Windows.Forms.CheckBox ckb_B无功功率;
		private System.Windows.Forms.TextBox tb_相位补偿区域设置0;
		private System.Windows.Forms.TextBox tb_C无功功率;
		private System.Windows.Forms.CheckBox ckb_C相位1;
		private System.Windows.Forms.CheckBox ckb_C无功功率;
		private System.Windows.Forms.TextBox tb_启动电流阈值设置;
		private System.Windows.Forms.CheckBox ckb_B相位1;
		private System.Windows.Forms.CheckBox ckb_启动电流阈值设置;
		private System.Windows.Forms.TextBox tb_B相位1;
		private System.Windows.Forms.TextBox tb_A视在功率;
		private System.Windows.Forms.CheckBox ckb_A相位1;
		private System.Windows.Forms.CheckBox ckb_A视在功率;
		private System.Windows.Forms.TextBox tb_A相位1;
		private System.Windows.Forms.TextBox tb_B视在功率;
		private System.Windows.Forms.CheckBox ckb_脉冲加倍寄存器;
		private System.Windows.Forms.CheckBox ckb_B视在功率;
		private System.Windows.Forms.TextBox tb_脉冲加倍寄存器;
		private System.Windows.Forms.TextBox tb_C视在功率;
		private System.Windows.Forms.CheckBox ckb_C电流;
		private System.Windows.Forms.CheckBox ckb_C视在功率;
		private System.Windows.Forms.TextBox tb_C电流;
		private System.Windows.Forms.TextBox tb_A相位0;
		private System.Windows.Forms.CheckBox ckb_B电流;
		private System.Windows.Forms.CheckBox ckb_A相位0;
		private System.Windows.Forms.TextBox tb_B电流;
		private System.Windows.Forms.TextBox tb_B相位0;
		private System.Windows.Forms.CheckBox ckb_A电流;
		private System.Windows.Forms.CheckBox ckb_B相位0;
		private System.Windows.Forms.TextBox tb_A电流;
		private System.Windows.Forms.TextBox tb_C相位0;
		private System.Windows.Forms.CheckBox ckb_C电压;
		private System.Windows.Forms.CheckBox ckb_C相位0;
		private System.Windows.Forms.TextBox tb_C电压;
		private System.Windows.Forms.TextBox tb_高频脉冲输出设置;
		private System.Windows.Forms.CheckBox ckb_B电压;
		private System.Windows.Forms.CheckBox ckb_高频脉冲输出设置;
		private System.Windows.Forms.TextBox tb_B电压;
		private System.Windows.Forms.TextBox tb_A电压;
		private System.Windows.Forms.CheckBox ckb_A电压;
		private System.Windows.Forms.GroupBox gbRegisterSingle;
		private System.Windows.Forms.TextBox tb_7017_L线有效值补偿;
		private TXButton btn_设7017;
		private TXButton tbn_读7017;
		private System.Windows.Forms.CheckBox ckb_7017_L线有效值补偿;
		private System.Windows.Forms.TextBox tb_7017_ADC通道增益选择;
		private System.Windows.Forms.CheckBox ckb_7017_ADC通道增益选择;
		private System.Windows.Forms.CheckBox ckb_7017_全选;
		private System.Windows.Forms.TextBox tb_7017_有功功率校正;
		private System.Windows.Forms.CheckBox ckb_7017_有功功率校正;
		private System.Windows.Forms.TextBox tb_7017_无功功率校正;
		private System.Windows.Forms.CheckBox ckb_7017_无功功率校正;
		private System.Windows.Forms.CheckBox ckb_7017_视在功率校正;
		private System.Windows.Forms.TextBox tb_7017_视在功率校正;
		private System.Windows.Forms.TextBox tb_7017_起动功率寄存器;
		private System.Windows.Forms.CheckBox ckb_7017_起动功率寄存器;
		private System.Windows.Forms.TextBox tb_7017_输出脉冲频率;
		private System.Windows.Forms.CheckBox ckb_7017_输出脉冲频率;
		private System.Windows.Forms.TextBox tb_7017_有功功率偏置校正;
		private System.Windows.Forms.CheckBox ckb_7017_有功功率偏置校正;
		private System.Windows.Forms.CheckBox ckb_7017_L线有功功率偏置校正低字节;
		private System.Windows.Forms.TextBox tb_7017_L线有功功率偏置校正低字节;
		private System.Windows.Forms.CheckBox ckb_7017_相位校正;
		private System.Windows.Forms.TextBox tb_7017_相位校正;
		private System.Windows.Forms.TextBox tb_7017_N线有效值补偿;
		private System.Windows.Forms.CheckBox ckb_7017_N线有效值补偿;
		private System.Windows.Forms.SplitContainer splitContainer2;
		private System.Windows.Forms.GroupBox groupBox54;
		private System.Windows.Forms.TextBox textBox_xiaobiaoData;
		private System.Windows.Forms.Label label104;
		private TXButton button_xiaobiaoSet;
		private System.Windows.Forms.Label label105;
		private System.Windows.Forms.Label label106;
		private System.Windows.Forms.TextBox textBox_xiaobiaoLen;
		private System.Windows.Forms.TextBox textBox_xiaobiaoAddr;
		private TXButton button_xiaobiaoRead;
		private TXComboBox tcbTypeRN;
		private TXTabControl ttcRegisters;
		private System.Windows.Forms.TabPage tpRegistersHT;
		private System.Windows.Forms.TabPage tptpRegistersRN;
		private System.Windows.Forms.SplitContainer splitContainer3;
		private TXButton tbReadRegister;
		private TXButton tbWriteRegister;
		private System.Windows.Forms.CheckBox cbEEPROM;
		private System.Windows.Forms.TabPage tpRegistersRN8302;
		private System.Windows.Forms.GroupBox gbRegisterThreeRN;
		private System.Windows.Forms.DataGridView dgvCommon;
		private System.Windows.Forms.DataGridViewCheckBoxColumn dataGridViewCheckBoxColumn3;
		private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn20;
		private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn21;
		private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn22;
		private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn23;
		private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn24;
		private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn32;
		private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn25;
		private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn26;
		private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn27;
		private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn28;
		private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn29;
		private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn30;
		private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn31;
		private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn33;
		private System.Windows.Forms.SplitContainer splitContainer4;
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.DataGridView dgvRN8302;
		private System.Windows.Forms.DataGridViewCheckBoxColumn dataGridViewCheckBoxColumn1;
		private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn1;
		private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn2;
		private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn3;
		private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn4;
		private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn5;
		private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn6;
		private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn7;
		private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn8;
		private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn9;
		private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn10;
		private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn11;
		private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn12;
		private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn13;
		private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn14;
		private System.Windows.Forms.CheckBox cbEEPROM8302;
		private TXComboBox txComboBox1;
		private TXButton txbSetRN8302;
		private TXButton txbReadRN8302;
	}
}