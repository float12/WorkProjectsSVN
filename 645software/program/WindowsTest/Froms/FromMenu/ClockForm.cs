using System;
using System.Windows.Forms;

namespace WindowsTest
{
	public partial class ClockForm : Form
	{
		public ClockForm()
		{
			InitializeComponent();
		}

		private void ClockForm_Load(object sender, EventArgs e)
		{

		}

		private void button1_Click(object sender, EventArgs e)
		{
			if (radioButton1.Checked)
			{//当前时间
				this.DialogResult = DialogResult.OK;
			}
			else
			{//手动输入
				this.DialogResult = DialogResult.Cancel;
			}
		}
	}
}
