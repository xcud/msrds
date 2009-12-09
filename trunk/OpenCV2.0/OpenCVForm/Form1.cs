using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using OpenCvSharp;

namespace OpenCVForm
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        public void SetSize(IplImage i)
        {

            this.Size = new Size(i.Size.Width+10, i.Size.Height+10);

            this.pictureBoxIpl1.ImageIpl = i;
        }
    }
}
