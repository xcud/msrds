using System;
using System.Collections.Generic;
using System.Text;

namespace gameAI.NerualNetwork
{
    public class KNeuron
    {

        /// <summary>
        /// Neuron's output value
        /// </summary>
        protected double output = 0;				


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

        internal double Compute(double[] input)
        {
            output = 0.0;

            // compute distance between inputs and weights
            for (int i = 0; i < _Weights.Length; i++)
            {
                output += Math.Abs(_Weights[i] - input[i]);
            }
            return output;
        }
    }
}
