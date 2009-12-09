using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;

namespace OpenCVForm
{
    class Wavelet
    {
        void haar1d(double[] vec, int n)
        {
            int i = 0;
            int w = n;
            double[] vecp = new double[n];
            for (i = 0; i < n; i++)
                vecp[i] = 0;

            while (w > 1)
            {
                w /= 2;
                for (i = 0; i < w; i++)
                {
                    vecp[i] = (vec[2 * i] + vec[2 * i + 1]) / Math.Sqrt(2.0);
                    vecp[i + w] = (vec[2 * i] - vec[2 * i + 1]) / Math.Sqrt(2.0);
                }

                for (i = 0; i < (w * 2); i++)
                    vec[i] = vecp[i];
            }

            //delete[] vecp;
        }

    }
}
