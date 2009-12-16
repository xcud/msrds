using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
//using System.Linq;
using System.Text;
using System.Windows.Forms;
//using OpenCvSharp;

namespace OpenCVDSS
{
    public partial class Form3 : Form
    {

        public surfCLR.SURF _surf = new surfCLR.SURF();

        int _tick;
        int _tickCount = 0;


        OpenCVDSSOperations _mainPort;

        public Form3(OpenCVDSSOperations mainPort)
        {

            _mainPort = mainPort;


            InitializeComponent();

            Show();


           this.pictureBoxIpl2.Image = _surf.Init("box2.png");

            
            //Application.Idle += new EventHandler(Application_Idle);

            _tick = Environment.TickCount;


            this.timer1.Start();
        }

        void Application_Idle(object sender, EventArgs e)
        {

            

            Bitmap bmp = _surf.UpdateFrame();

            this.pictureBoxIpl1.Image = bmp;

            _tickCount++;


            if (1000 < (Environment.TickCount - _tick))
            {
                _tick = Environment.TickCount;
               //MDIParent1.Log(string.Format("TICK COUNT({0}) E({1}) D({2}) C({3})",_tickCount,_surf.GetExtremaTime(),_surf.GetDescriptorTime(),_surf.GetCompareTime()));
               //MDIParent1.Log(string.Format("CX({0}) CY({1})",_surf.GetCX(), _surf.GetCY()));
                _tickCount = 0;
            }

            OpenCVDSSState s = new OpenCVDSSState();
            s._X = _surf.GetCX();
            s._Y = _surf.GetCY();

            _mainPort.Post(new SURFInfo(s));

            

            //cap.GrabFrame();
        }

        private void Form3_Load(object sender, EventArgs e)
        {

        }

        private void timer1_Tick(object sender, EventArgs e)
        {

            Bitmap bmp = _surf.UpdateFrame();

            this.pictureBoxIpl1.Image = bmp;

            _tickCount++;


            if (1000 < (Environment.TickCount - _tick))
            {
                _tick = Environment.TickCount;
                //MDIParent1.Log(string.Format("TICK COUNT({0}) E({1}) D({2}) C({3})",_tickCount,_surf.GetExtremaTime(),_surf.GetDescriptorTime(),_surf.GetCompareTime()));
                //MDIParent1.Log(string.Format("CX({0}) CY({1})",_surf.GetCX(), _surf.GetCY()));
                _tickCount = 0;
            }

            OpenCVDSSState s = new OpenCVDSSState();
            s._X = _surf.GetCX();
            s._Y = _surf.GetCY();

            _mainPort.Post(new SURFInfo(s));
        }
    }
}
