using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace test
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            XMLLib.Digit di = new XMLLib.Digit();
            di._NUM = 1;
            di._SIMBOL = new uint[] { 1, 1 };
            XMLLib.Xml.SaveXml<XMLLib.Digit>("111.xml", di);

        }
    }
}
