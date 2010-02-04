using System;
using System.Collections.Generic;
using System.Text;

namespace Classifier
{
    public class KLearning
    {

        KNetwork _Network;

        public KLearning(int inputCount,int [] layersCount)
        {
            _Network = new KNetwork(inputCount,layersCount);
        }


        double Run(double[] input)
        {
            _Network.Compute(input);

            int winner = _Network.GetWinner();

            // get layer of the network
            KLayer layer = _Network[0];

            KNeuron neuron = layer[winner];

            // update weight of the winner only
            for (int i = 0, n = neuron._Weights.Length; i < n; i++)
            {
                neuron[i] += (input[i] - neuron[i]) * 0.01;
            }

            



            return 0;

        }

        public double RunEpoch(double[][] input)
        {
            double error = 0.0;

            // walk through all training samples
            foreach (double[] sample in input)
            {
                error += Run(sample);
            }

            // return summary error
            return error;
        }


    }
}
