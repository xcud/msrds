using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using HopfieldNeuralNetwork;

namespace HopfieldRecognizer
{
    public partial class frmMain : Form
    {
        private NeuralNetwork NN;
        private int imageDim = Convert.ToInt32(HopfieldRecognizer.Properties.Resources.ImageDimension);
        private bool patternSelected = false;

        public frmMain()
        {
            InitializeComponent();
        }

        private void UpdatePropertiesPB()
        {
            lblNNSize.Text = NN.N.ToString();
            lblNumberOfPatterns.Text = NN.M.ToString();
            lblEnergy.Text = NN.Energy.ToString();
        }

        private void CreateNNBut_Click(object sender, EventArgs e)
        {
            NN = new NeuralNetwork(imageDim*imageDim);
            panelStoredImages.Controls.Clear();
            NN.EnergyChanged += new EnergyChangedHandler(NN_EnergyChanged);
            Random rnd = new Random();
            int r = 0;
            imNNState.pixels = new int[imageDim, imageDim];
            for (int i = 0; i < imageDim; i++)
                for (int j = 0; j < imageDim; j++)
                {
                    r = rnd.Next(2);
                    if (r == 0) imNNState.pixels[i, j] = Color.Black.ToArgb();
                    else if (r == 1) imNNState.pixels[i, j] = Color.White.ToArgb();                    
                }
            patternSelected = false;
            butAddPattern.Enabled = true;
            butRunDynamics.Enabled = false;
            imNNState.Visible = true;
            imNNState.Invalidate();
            UpdatePropertiesPB();
        }


        private void NN_EnergyChanged(object sender, EnergyEventArgs e)
        {
            lblEnergy.Text = e.Energy.ToString();
            int i = (int)e.NeuronIndex / imageDim;
            int j = e.NeuronIndex % imageDim;
            if (imNNState.pixels[i, j] == Color.White.ToArgb()) imNNState.pixels[i, j] = Color.Black.ToArgb();
            else if (imNNState.pixels[i, j] == Color.Black.ToArgb()) imNNState.pixels[i, j] = Color.White.ToArgb();
            imNNState.Invalidate();
            Application.DoEvents();
            System.Threading.Thread.Sleep(100);
        }

        private void AddPatternBut_Click(object sender, EventArgs e)
        {
            Image imgPattern;
            if (ofd.ShowDialog() == DialogResult.OK)
            {
                imgPattern = Image.FromFile(ofd.FileName);
                if (imgPattern.Width != imageDim || imgPattern.Height != imageDim)
                {
                    MessageBox.Show("Image size must be " + imageDim.ToString() + "x" + imageDim.ToString() + " pixels", "Wrong image size", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
                else
                {                    
                    int[,] patternPixels;
                    int p = 0;
                    int midColor = Math.Abs((int)(Color.Black.ToArgb() / 2));
                    Bitmap b = new Bitmap(imgPattern);
                    patternPixels = new int[imageDim, imageDim];
                    List<Neuron> pattern = new List<Neuron>(imageDim * imageDim);
                    for (int i = 0; i < imageDim; i++)
                        for (int j = 0; j < imageDim; j++)
                        {
                            Neuron n = new Neuron();
                            p = Math.Abs(b.GetPixel(i, j).ToArgb());
                            if (p < midColor)
                            {
                                b.SetPixel(i, j, Color.White);
                                n.State = NeuronStates.AlongField;
                            }
                            else
                            {
                                b.SetPixel(i, j, Color.Black);
                                n.State = NeuronStates.AgainstField;
                            }
                            pattern.Add(n);
                        }
                    NN.AddPattern(pattern);
                    ImageMagnifier.ImageMagnifier im = new ImageMagnifier.ImageMagnifier();
                    im.ImageToMagnify = b;
                    im.MagnificationCoefficient = 8;
                    im.Location = new System.Drawing.Point(1, ((NN.M - 1) * (imageDim + 2) * im.MagnificationCoefficient));
                    im.Size = new System.Drawing.Size(imageDim * im.MagnificationCoefficient, imageDim * im.MagnificationCoefficient);
                    im.TabIndex = 0;
                    im.Click += new EventHandler(im_Click);
                    panelStoredImages.Controls.Add(im);
                    butRunDynamics.Enabled = true;
                    UpdatePropertiesPB();
                }                
            }            
        }

        void im_Click(object sender, EventArgs e)
        {
            ImageMagnifier.ImageMagnifier im = (ImageMagnifier.ImageMagnifier)sender;
            this.imNNState.ImageToMagnify = im.ImageToMagnify;

            frmAddDistortion frmaddDistortion = new frmAddDistortion();
            frmaddDistortion.ShowDialog();
            
            Random r = new Random();
            int p=0;
            int k=0;
            for (int i = 0; i < frmaddDistortion.DistortionLevel; i++)
            {
                p = r.Next(imageDim);
                k = r.Next(imageDim);
                if (imNNState.pixels[k, p] == Color.Black.ToArgb()) imNNState.pixels[k, p] = Color.White.ToArgb();
                else if (imNNState.pixels[k, p] == Color.White.ToArgb()) imNNState.pixels[k, p] = Color.Black.ToArgb();
            }
            lblEnergy.Text = "0";
            patternSelected = true;
            imNNState.Invalidate();
        }

        private void RunDynamicsBut_Click(object sender, EventArgs e)
        {
            if (!patternSelected)
            {
                MessageBox.Show("You must select initial pattern.", "Neural Network Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            else
            {
                List<HopfieldNeuralNetwork.Neuron> initialState = new List<HopfieldNeuralNetwork.Neuron>(NN.N);
                for (int i = 0; i < imageDim; i++)
                    for (int j=0;j<imageDim;j++)
                    {
                        Neuron neuron = new Neuron();
                        if (imNNState.pixels[i, j] == Color.Black.ToArgb()) neuron.State = NeuronStates.AgainstField;
                        else if (imNNState.pixels[i, j] == Color.White.ToArgb()) neuron.State = NeuronStates.AlongField;
                        initialState.Add(neuron);
                    }
                NN.Run(initialState);
                lblEnergy.Text = NN.Energy.ToString();
                patternSelected = false;
            }
        }

        private void HopfieldRecognizerMainForm_Load(object sender, EventArgs e)
        {
            butCreateNN.Text = "Create Neural Network (" + (imageDim * imageDim) + " Neurons)";
        }

        private void panelStoredImages_Paint(object sender, PaintEventArgs e)
        {

        }
    }
}