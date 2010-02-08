using System;
using System.Collections.Generic;
using System.Text;

namespace Classifier
{
    



    public class KLearnData
    {
        public double[] _Input;
        public string _Symbol;






    }

    public class KLearning
    {





        KNetwork _Network;
        int _Width;
        int _Height;


        public KLearning(int inputCount, int width,int height)
        {

            _Network = new KNetwork(inputCount, new int[] { width  * height});
        }

        public double _LearningRadius = 7;
        private double _SquaredRadius2 = 2 * 7 * 7;

         //learning rate
        private double _LearningRate = 0.1;

        
        


        double Run(KLearnData sample)
        {

            double error = 0.0;


            _Network.Compute(sample._Input);

            int winner = _Network.GetWinner();

            // get layer of the network
            KLayer layer = _Network[0];

            layer[winner].AddSymbol(sample._Symbol);

            

            // update weight of the winner only


            // check learning radius
            if (_LearningRadius == 0)
            {
                KNeuron neuron = layer[winner];

                for (int i = 0, n = neuron._Weights.Length; i < n; i++)
                {
                    neuron[i] += (sample._Input[i] - neuron[i]) * 0.01;
                }
            }
            else
            {
                // winner's X and Y
                int wx = winner % _Width;
                int wy = winner / _Width;

                // walk through all neurons of the layer
                for (int j = 0, m = layer._Neurons.Length; j < m; j++)
                {
                    KNeuron neuron = layer[j];

                    int dx = (j % _Width) - wx;
                    int dy = (j / _Width) - wy;

                    // update factor ( Gaussian based )
                    double factor = Math.Exp(-(double)(dx * dx + dy * dy) / _SquaredRadius2);

                    // update weight of the neuron
                    for (int i = 0, n = neuron._Weights.Length; i < n; i++)
                    {
                        // calculate the error
                        double e = (sample._Input[i] - neuron[i]) * factor;
                        error += Math.Abs(e);
                        // update weight
                        neuron[i] += e * _LearningRate;
                    }
                }
            }


            return 0;

        }



        public double RunEpoch(KLearnData[] input)
        {
            double error = 0.0;

            // walk through all training samples
            foreach (KLearnData sample in input)
            {
                error += Run(sample);
            }

            // return summary error
            return error;
        }


    }
}
