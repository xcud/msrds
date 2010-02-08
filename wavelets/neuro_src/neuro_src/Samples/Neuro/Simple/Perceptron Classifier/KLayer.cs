using System;
using System.Collections.Generic;
using System.Text;

namespace Classifier
{
    public class KLayer
    {

        public KNeuron[] _Neurons ;

        protected double[] _Output;

        public KNeuron this[int index]
        {
            get { return _Neurons[index]; }
        }


        public KLayer(int inputsCount, int neuronsCount)
        {
            _Neurons = new KNeuron[neuronsCount];
            _Output = new double[neuronsCount];
            //foreach (var n in _Neuron)
            //{
                
            //}
            
        }

        public double[] Compute(double [] input)
        {

            for (int i = 0; i < _Neurons.Length; i++)
                _Output[i] = _Neurons[i].Compute(input);

            return _Output;

            
        }
    }
}
