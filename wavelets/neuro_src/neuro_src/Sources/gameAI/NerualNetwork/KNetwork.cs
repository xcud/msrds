using System;
using System.Collections.Generic;
using System.Text;

namespace gameAI.NerualNetwork
{
    public class KNetwork
    {

        /// <summary>
        /// Network's output vector
        /// </summary>
        protected double[] _Output;



        public KNetwork(int inputsCount, int[] layersCount)
        {
            for (int i = 0; i < layersCount.Length; i++)
            {
                _Layers.Add(new KLayer(inputsCount,layersCount[i]));
            }


        }

        public KLayer this[int index]
        {
            get { return _Layers[index]; }
        }

        List<KLayer> _Layers = new List<KLayer>();
        //List<string> _Input

        public double [] Compute(double [] input)
        {

            foreach (var layer in _Layers)
            {
                _Output = layer.Compute(input);
            }

            return _Output;
        }


        /// <summary>
        /// Get winner neuron
        /// </summary>
        /// 
        /// <returns>Index of the winner neuron</returns>
        /// 
        /// <remarks>The method returns index of the neuron, which weights have
        /// the minimum distance from network's input.</remarks>
        /// 
        public int GetWinner()
        {
            // find the MIN value
            double min = _Output[0];
            int minIndex = 0;

            for (int i = 1, n = _Output.Length; i < n; i++)
            {
                if (_Output[i] < min)
                {
                    // found new MIN value
                    min = _Output[i];
                    minIndex = i;
                }
            }

            return minIndex;
        }

        public virtual void Randomize()
        {
            foreach (var layer in _Layers)
            {
                layer.Randomize();
            }
            
        }
        
        internal string Estimate(double[] p)
        {
            Compute(p);
            int winner = GetWinner();

            string strSymobl = null;
            uint max = 0;
            foreach (var s in _Layers[0]._Neurons[winner]._Symbols)
            {
                if (max < s.Value)
                {
                    max = s.Value;
                    strSymobl = s.Key;
                }
            }
                

            return strSymobl;
        }
    }
}
