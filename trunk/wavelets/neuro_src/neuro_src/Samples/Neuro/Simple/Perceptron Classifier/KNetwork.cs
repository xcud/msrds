using System;
using System.Collections.Generic;
using System.Text;

namespace Classifier
{
    public class KNetwork
    {

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
                input = layer.Compute(input);
            }

            return input;
        }


        internal int GetWinner()
        {
            throw new NotImplementedException();
        }
    }
}
