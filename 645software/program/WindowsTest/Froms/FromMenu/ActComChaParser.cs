using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Runtime.InteropServices.ComTypes;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows.Forms;
using WindowsTest;

namespace Test.Froms.FromMenu
{
    public partial class ActComChaParser : Form
    {
        public ActComChaParser()
        {
            InitializeComponent();
        }

        private void label1_Click(object sender, EventArgs e)
        {

        }
        public void parse(string dataIn)
        {
            uint uintVal = Convert.ToUInt32(dataIn, 16);
        
            radioButton7.Checked = (uintVal & (1 << 0)) == 0;
            radioButton8.Checked = (uintVal & (1 << 0)) != 0;

            radioButton5.Checked = (uintVal & (1 << 1)) == 0;
            radioButton6.Checked = (uintVal & (1 << 1)) != 0;

            radioButton3.Checked = (uintVal & (1 << 2)) == 0;
            radioButton4.Checked = (uintVal & (1 << 2)) != 0;

            radioButton1.Checked = (uintVal & (1 << 3)) == 0;
            radioButton2.Checked = (uintVal & (1 << 3)) != 0;
        }
        public byte GenerateFeatureByte()
        {
            byte result = 0;

            if (radioButton8.Checked) result |= (1 << 0); 
            if (radioButton6.Checked) result |= (1 << 1); 
            if (radioButton4.Checked) result |= (1 << 2); 
            if (radioButton2.Checked) result |= (1 << 3); 

            return result;
        }
        private void radioButton1_CheckedChanged(object sender, EventArgs e)
        {

        }

        private void label1_Click_1(object sender, EventArgs e)
        {

        }

        private void radioButton1_CheckedChanged_1(object sender, EventArgs e)
        {

        }

        private void ActiveComCha_Load(object sender, EventArgs e)
        {

        }

        private void radioButton2_CheckedChanged(object sender, EventArgs e)
        {

        }

        private void button1_Click(object sender, EventArgs e)
        {         
            this.DialogResult = DialogResult.OK;
            this.Close();
        }

        private void button2_Click(object sender, EventArgs e)
        {

        }
    }
}
