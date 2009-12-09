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
    public partial class Form3 : Form
    {

        public surfCLR.SURF _surf = new surfCLR.SURF();

        int _tick;
        int _tickCount = 0;
        public Form3()
        {
            InitializeComponent();



           this.pictureBoxIpl2.Image = _surf.Init("box2.png");


            Application.Idle += new EventHandler(Application_Idle);

            _tick = Environment.TickCount;

        }

        void Application_Idle(object sender, EventArgs e)
        {

            

            Bitmap bmp = _surf.UpdateFrame();

            this.pictureBoxIpl1.Image = bmp;

            _tickCount++;


            if (1000 < (Environment.TickCount - _tick))
            {
                _tick = Environment.TickCount;
               MDIParent1.Log(string.Format("TICK COUNT({0}) E({1}) D({2}) C({3})",_tickCount,_surf.GetExtremaTime(),_surf.GetDescriptorTime(),_surf.GetCompareTime()));
                _tickCount = 0;
            }

            

            //cap.GrabFrame();
        }

        private void Form3_Load(object sender, EventArgs e)
        {

        }
    }
}
