using System;
using System.Collections.Generic;
using System.Text;

namespace Classifier
{
    public class KLayer
    {

        public KNeuron[] _Neurons ;



        public KNeuron this[int index]
        {
            get { return _Neurons[index]; }
        }


        public KLayer(int inputsCount, int neuronsCount)
        {
            _Neurons = new KNeuron[neuronsCount];

            //foreach (var n in _Neuron)
            //{
                
            //}
            
        }

        public double[] Compute(double [] input)
        {



            return input;
        }
    }
}
