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
    public partial class MDIParent1 : Form
    {
        private int childFormNumber = 0;
        static public MDIParent1 _I ;

        public MDIParent1()
        {
            InitializeComponent();
            _I = this;
        }


        static public void Log(string log)
        {
            _I.listBox1.Items.Insert(0, log);

        }

        private void ShowNewForm(object sender, EventArgs e)
        {
            Form childForm = new Form();
            childForm.MdiParent = this;
            childForm.Text = "Window " + childFormNumber++;
            childForm.Show();
        }


        private Form1 NewForm1()
        {
            Form1 childForm = new Form1();
            childForm.MdiParent = this;
            childForm.Text = "Window " + childFormNumber++;
            childForm.Show();

            return childForm;
        }


        private Form3 NewForm3()
        {
            Form3 childForm = new Form3();
            childForm.MdiParent = this;
            childForm.Text = "Window " + childFormNumber++;
            childForm.Show();

            return childForm;
        }



        private void OpenFile(object sender, EventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.InitialDirectory = Environment.GetFolderPath(Environment.SpecialFolder.Personal);
            openFileDialog.Filter = "Text Files (*.txt)|*.txt|All Files (*.*)|*.*";
            if (openFileDialog.ShowDialog(this) == DialogResult.OK)
            {
                string FileName = openFileDialog.FileName;
            }
        }

        private void SaveAsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SaveFileDialog saveFileDialog = new SaveFileDialog();
            saveFileDialog.InitialDirectory = Environment.GetFolderPath(Environment.SpecialFolder.Personal);
            saveFileDialog.Filter = "Text Files (*.txt)|*.txt|All Files (*.*)|*.*";
            if (saveFileDialog.ShowDialog(this) == DialogResult.OK)
            {
                string FileName = saveFileDialog.FileName;
            }
        }

        private void ExitToolsStripMenuItem_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void CutToolStripMenuItem_Click(object sender, EventArgs e)
        {
        }

        private void CopyToolStripMenuItem_Click(object sender, EventArgs e)
        {
        }

        private void PasteToolStripMenuItem_Click(object sender, EventArgs e)
        {
        }

        private void ToolBarToolStripMenuItem_Click(object sender, EventArgs e)
        {
            toolStrip.Visible = toolBarToolStripMenuItem.Checked;
        }

        private void StatusBarToolStripMenuItem_Click(object sender, EventArgs e)
        {
            statusStrip.Visible = statusBarToolStripMenuItem.Checked;
        }

        private void CascadeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            LayoutMdi(MdiLayout.Cascade);
        }

        private void TileVerticalToolStripMenuItem_Click(object sender, EventArgs e)
        {
            LayoutMdi(MdiLayout.TileVertical);
        }

        private void TileHorizontalToolStripMenuItem_Click(object sender, EventArgs e)
        {
            LayoutMdi(MdiLayout.TileHorizontal);
        }

        private void ArrangeIconsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            LayoutMdi(MdiLayout.ArrangeIcons);
        }

        private void CloseAllToolStripMenuItem_Click(object sender, EventArgs e)
        {
            foreach (Form childForm in MdiChildren)
            {
                childForm.Close();
            }
        }

        private void sobelToolStripMenuItem_Click(object sender, EventArgs e)
        {

            Form1 f1 = NewForm1();
            Form1 f2 = NewForm1();
            Form1 f3 = NewForm1();
            Form1 f4 = NewForm1();

            IplImage dstImg1 = new IplImage("box.png", LoadMode.GrayScale);

            IplImage dstImg2 = new IplImage(dstImg1.Size, BitDepth.F32,1);

            IplImage dstImg3 = new IplImage(dstImg1.Size, BitDepth.F32, 1);
            IplImage dstImg4 = new IplImage(dstImg1.Size, BitDepth.F32, 1);


            CvInvoke.cvSobel(dstImg1.CvPtr, dstImg2.CvPtr, 1, 0, ApertureSize.Size5);
            CvInvoke.cvSobel(dstImg1.CvPtr, dstImg3.CvPtr, 2, 0, ApertureSize.Size5);
            CvInvoke.cvSobel(dstImg1.CvPtr, dstImg4.CvPtr, 3, 0, ApertureSize.Size5);

            f1.SetSize( dstImg1);
            f2.SetSize(dstImg2);
            f3.SetSize(dstImg3);
            f4.SetSize(dstImg4);
        }

        private void harrisToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Form1 f1 = NewForm1();
            Form1 f2 = NewForm1();


            IplImage dstImg1 = new IplImage("box.png", LoadMode.GrayScale);
            IplImage dstImg2 = new IplImage(dstImg1.Size, BitDepth.F32, 1);


            CvInvoke.cvCornerHarris(dstImg1.CvPtr, dstImg2.CvPtr, 3, ApertureSize.Size5, 0.04);  
                



            f1.SetSize(dstImg1);
            f2.SetSize(dstImg2);
        }

        private void hessianToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Form1 f1 = NewForm1();
            Form1 f2 = NewForm1();

            IplImage dstImg1 = new IplImage("box.png", LoadMode.GrayScale);


            Bitmap bmp = BitmapConverter.ToBitmap(dstImg1);

            Hessian h = new Hessian();
            Bitmap bmp2 = new Bitmap(bmp.Width, bmp.Height, System.Drawing.Imaging.PixelFormat.Format8bppIndexed);

            bmp2.Palette = bmp.Palette;

            h.Gaussian(bmp, bmp2);

            h.Do(bmp);

            f1.pictureBoxIpl1.Image = bmp;
            f2.pictureBoxIpl1.Image = bmp2;
        }

        private void surfToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Form1 f1 = NewForm1();
            Form1 f2 = NewForm1();


            SURF surf = new SURF();
            IplImage dstImg1 = surf.Init("box.png");

            IplImage dstImg2 = surf.Test("box_in_scene.png");


            f1.SetSize(dstImg1);
            f2.SetSize(dstImg2);
        }


        private void harrisToolStripMenuItem1_Click(object sender, EventArgs e)
        {

            Form1 f1 = NewForm1();
            Form1 f2 = NewForm1();

            IplImage dstImg1 = new IplImage("box.png", LoadMode.GrayScale);



            Bitmap bmp = BitmapConverter.ToBitmap(dstImg1);

            Hessian h = new Hessian();
            Bitmap bmp2 = new Bitmap(bmp.Width, bmp.Height, System.Drawing.Imaging.PixelFormat.Format8bppIndexed);

            bmp2.Palette = bmp.Palette;


            InputThreahold it = new InputThreahold();

            DialogResult dr = it.ShowDialog();

            if (dr == DialogResult.OK)
            {
                

                double th;

                if( double.TryParse(it.textBox1.Text,out th))
                {
                    h.Harris(bmp, bmp2);



                    f1.pictureBoxIpl1.Image = bmp;
                    f2.pictureBoxIpl1.Image = bmp2;

                }

            }


        }

        private void gaussianToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Form1 f1 = NewForm1();
            Form1 f2 = NewForm1();

            IplImage dstImg1 = new IplImage("box.png", LoadMode.GrayScale);

            Bitmap bmp = BitmapConverter.ToBitmap(dstImg1);

            Bitmap bmp2 = new Bitmap(bmp.Width, bmp.Height, System.Drawing.Imaging.PixelFormat.Format8bppIndexed);


            bmp2.Palette = bmp.Palette;

            Hessian h = new Hessian();
            h.Gaussian(bmp, bmp2);





            f1.pictureBoxIpl1.Image = bmp;
            f2.pictureBoxIpl1.Image = bmp2;
        }

        private void laplaceToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Form1 f1 = NewForm1();
            Form1 f2 = NewForm1();

            IplImage dstImg1 = new IplImage("box.png", LoadMode.GrayScale);

            

            Bitmap bmp = BitmapConverter.ToBitmap(dstImg1);
            Bitmap bmp2 = new Bitmap(bmp.Width, bmp.Height, System.Drawing.Imaging.PixelFormat.Format8bppIndexed);

            bmp2.Palette = bmp.Palette;

            Hessian h = new Hessian();
            h.Laplace(bmp, bmp2);

            f1.pictureBoxIpl1.Image = bmp;
            f2.pictureBoxIpl1.Image = bmp2;
        }

        private void imToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Form1 f1 = NewForm1();

            IplImage dstImg1 = new IplImage("box.png", LoadMode.GrayScale);
            Bitmap ibmp = BitmapConverter.ToBitmap(dstImg1);

            //Bitmap obmp = new Bitmap(ibmp.Width, ibmp.Height, System.Drawing.Imaging.PixelFormat.);

            SURF2 surf = new SURF2();

            List<CvSURFPoint> points = surf.FastHessianDetector(ibmp);


            //for (int i = 0; i < points; i++)
            foreach (CvSURFPoint r in points) 
            {


//                CvSURFPoint r = Cv.GetSeqElem<CvSURFPoint>(points, i).Value;
                 CvPoint center;
                int radius;
                center.X = (int)Math.Round(r.Pt.X);
                center.Y = (int)Math.Round(r.Pt.Y);
                radius = (int)Math.Round(r.Size*1.2/9*2);
                CvInvoke.cvCircle(dstImg1.CvPtr,center, radius,new CvScalar(0,0,255), 1, LineType.Link8,0);   


                //Cv.Circle(img, pt, 2, new CvColor(255, 0, 0), -1);
                

            }

            ibmp = BitmapConverter.ToBitmap(dstImg1);

            f1.pictureBoxIpl1.Image = ibmp;

                


            
        }

        private void startToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Form3 form = NewForm3();
        }

        private void MDIParent1_Load(object sender, EventArgs e)
        {
            //Console.
        }
    }
}
