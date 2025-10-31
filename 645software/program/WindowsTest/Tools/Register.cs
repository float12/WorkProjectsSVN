using System;
using System.Drawing;
using System.Windows.Forms;
using TX.Framework.WindowUI.Forms;
using WindowsTest;

namespace WindowsTest
{
	public partial class Register : MainForm
	{
		public Register()
		{
			InitializeComponent();
		}

		public Register(string _machineCode)
		{
			InitializeComponent();
			machineCode = _machineCode;
			textBox1.Text = machineCode;
		}

		public string license = "";
		private string machineCode = "";

		#region DrawCaption
		/// <summary>
		/// Draws the caption.
		/// </summary>
		/// <param name="g">The g.</param>
		protected override void DrawCaption(Graphics g)
		{
			base.DrawCaption(g);
			if (this.CaptionHeight > 0)
			{
				winMain.DrawRibbonBtn(g);
			}
		}
		#endregion

		private void button1_Click(object sender, EventArgs e)
		{
			license = textBox2.Text;
			this.DialogResult = DialogResult.OK;
			this.Close();
		}

		private void button2_Click(object sender, EventArgs e)
		{
			this.DialogResult = DialogResult.Cancel;
			this.Close();
		}
	}
}
