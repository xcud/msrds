using System;
using System.Collections.Generic;
using System.Text;

namespace Classifier
{
    public class KNeuron
    {

        public Dictionary<string, uint> _Symbols = new Dictionary<string, uint>();

        public double[] _Weights;

        public KNeuron(int inputs)
        {
            _Weights = new double[inputs];


        }

        protected static Random rand = new Random((int)DateTime.Now.Ticks);
        protected static DoubleRange randRange = new DoubleRange(0.0, 1.0);


        public virtual void Randomize()
        {
            double d = randRange.Length;

            // randomize weights
            for (int i = 0; i < _Weights.Length; i++)
                _Weights[i] = rand.NextDouble() * d + randRange.Min;


        }

        public double this[int index]
        {
            get { return _Weights[index]; }
            set { _Weights[index] = value; }
        }

        internal void AddSymbol(string symbol)
        {
            if (_Symbols.ContainsKey(symbol) == false)
            {
                _Symbols.Add(symbol, 0);

            }

            _Symbols[symbol]++;
        }
    }
}
