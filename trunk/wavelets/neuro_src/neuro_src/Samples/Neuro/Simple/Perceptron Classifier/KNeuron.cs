using System;
using System.Collections.Generic;
using System.Text;

namespace Classifier
{
    public class KNeuron
    {
        public double[] _Weights;

        public KNeuron(int inputs)
        {
            _Weights = new double[inputs];
        }

        public double this[int index]
        {
            get { return _Weights[index]; }
            set { _Weights[index] = value; }
        } 
    }
}
