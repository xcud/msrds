using System;
using System.Collections.Generic;
using System.Text;

namespace HopfieldNeuralNetwork
{
    /// <summary>
    /// Static class, which describes neuron states.
    /// </summary>
    public static class NeuronStates
    {
        /// <summary>
        /// If neuron orienatated along local field, then it's state is equal to 1
        /// </summary>
        public static int AlongField = 1;
        /// <summary>
        /// If neuron orienatated against local field, then it's state is equal to -1
        /// </summary>
        public static int AgainstField = -1;     
    }
}
