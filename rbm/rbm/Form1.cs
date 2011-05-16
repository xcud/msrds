using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using Encog.Neural.NeuralData.Bipolar;
using Encog.Neural.Networks;

using Encog.Neural.Networks.Logic;
using Encog.Neural.Networks.Pattern;



namespace rbm
{
	public partial class Form1 : Form
	{
		public Form1()
		{
			InitializeComponent();
		}

		Bitmap _bitmap ;

        private double gamma = 7;
		

		public void CalculateWeights(BasicNetwork network)
		{
			int n1, n2, n3, n4;
			int i, j;
			int predN3, succN3;
			double weight;

			BoltzmannLogic logic = (BoltzmannLogic)network.Logic;

			for (n1 = 0; n1 < _bitmap.Height; n1++)
			{
				for (n2 = 0; n2 < _bitmap.Width; n2++)
				{
					i = n1 * _bitmap.Height + n2;
					for (n3 = 0; n3 < _bitmap.Height; n3++)
					{
						for (n4 = 0; n4 < _bitmap.Width; n4++)
						{
							j = n3 * _bitmap.Height + n4;
							weight = 0;
							if (i != j)
							{
                                predN3 = (n3 == 0 ? _bitmap.Width - 1 : n3 - 1);
                                succN3 = (n3 == _bitmap.Width - 1 ? 0 : n3 + 1);
								if ((n1 == n3) || (n2 == n4))
									weight = -gamma;
								else if ((n1 == predN3) || (n1 == succN3))
									weight = -distance[n2][n4];
							}
							logic.ThermalSynapse.WeightMatrix[i, j] = weight;
						}
					}
					logic.ThermalLayer.BiasWeights[i] = (-gamma / 2);
				}
			}
		}


		private void button1_Click(object sender, EventArgs e)
		{

			Bitmap bitmap = new Bitmap(@"data\\mnist\\2_58.bmp");

			this.pictureBox1.Image = bitmap;


			BoltzmannPattern pattern = new BoltzmannPattern();
			pattern.InputNeurons = bitmap.Width * bitmap.Height;
			BasicNetwork network = pattern.Generate();
			BoltzmannLogic logic = (BoltzmannLogic)network.Logic;

			//CreateCities();
			CalculateWeights(network);




		}
	}
}
