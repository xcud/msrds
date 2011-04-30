using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace ImageMagnifier 
{
    public partial class ImageMagnifier : Control
    {
        public int[,] pixels;

        private Image imageToMagnify;

        public Image ImageToMagnify
        {
            get { return imageToMagnify; }
            set 
            {
                if (value != null)
                {
                    imageToMagnify = value;
                    Bitmap b = new Bitmap(this.ImageToMagnify);

                    pixels = new int[this.ImageToMagnify.Width, this.ImageToMagnify.Height];

                    for (int i = 0; i < this.ImageToMagnify.Width; i++)
                        for (int j = 0; j < this.ImageToMagnify.Height; j++)
                        {
                            pixels[i, j] = b.GetPixel(i, j).ToArgb();
                        }
                    this.Invalidate();
                }
            }
        }
        public const int MaxMagnificationCoefficient=64;
        public const int MinMagnificationCoefficient=1;

        private int magnificationCoefficient=1;

        public int MagnificationCoefficient
        {
            get { return magnificationCoefficient; }
            set 
            {
                if (value < MinMagnificationCoefficient)
                    magnificationCoefficient = MinMagnificationCoefficient;
                else
                    magnificationCoefficient = value > MaxMagnificationCoefficient ? MaxMagnificationCoefficient : value;
                this.Invalidate();
            }
        }

        public ImageMagnifier()
        {
            InitializeComponent();
            this.DoubleBuffered = true;
        }

        public ImageMagnifier(Image ImageToMagnify)
        {
            InitializeComponent();
            this.DoubleBuffered = true;
            this.ImageToMagnify = ImageToMagnify;
        }

        protected override void OnPaint(PaintEventArgs pe)
        {
            if (imageToMagnify != null)
            {
                this.Width = imageToMagnify.Width * magnificationCoefficient;
                this.Height = imageToMagnify.Height * magnificationCoefficient;

                int currentTop = 0;
                int currentLeft = 0;
                for (int i = 0; i < this.ImageToMagnify.Width; i++)
                {
                    currentTop = i * magnificationCoefficient;
                    for (int j = 0; j < this.ImageToMagnify.Height; j++)
                    {
                        currentLeft = j * magnificationCoefficient;
                        Brush b = new SolidBrush(Color.FromArgb(pixels[i, j]));
                        pe.Graphics.FillRectangle(b, currentTop, currentLeft, magnificationCoefficient, magnificationCoefficient);
                    }
                }
            }
        }
    }
}
