using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace HopfieldRecognizer
{
    public partial class frmAddDistortion : Form
    {
        public int DistortionLevel;
        public frmAddDistortion()
        {
            DistortionLevel = 10;
            InitializeComponent();
        }

        private void btnSetDistortionLevel_Click(object sender, EventArgs e)
        {
            this.nudDistortionLevel_ValueChanged(this, new EventArgs());
            this.Close();
        }

        private void nudDistortionLevel_ValueChanged(object sender, EventArgs e)
        {
            DistortionLevel = Convert.ToInt32(nudDistortionLevel.Value);
        }

        private void nudDistortionLevel_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar == 13) this.btnSetDistortionLevel_Click(this, new EventArgs());
        }
    }
}